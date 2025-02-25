/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "chpl/framework/Context.h"
#include "chpl/framework/Location.h"
#include "chpl/framework/UniqueString.h"
#include "chpl/uast/all-uast.h"
#include "chpl/parsing/parsing-queries.h"
#include "chpl/resolution/resolution-queries.h"

#include "hpx/symboltypes.hpp"
#include "hpx/programtree.hpp"
#include "hpx/programtreebuildingvisitor.hpp"
#include "hpx/symbolbuildingvisitor.hpp"
#include "hpx/codegenvisitor.hpp"
#include "hpx/cmakegen.hpp"
#include "hpx/utils.hpp"

#include "ErrorGuard.h"

#include <optional>
#include <string>
#include <regex>
#include <filesystem>
#include <algorithm>
#include <streambuf>
#include <fstream>
#include <iostream>

#include <getopt.h> /* getopt API */
#include <fmt/core.h>

using namespace chpl;
using namespace uast;

extern char *optarg;
extern int optind, opterr, optopt;
const char *optstring;

// generate help string
void usage() {
   constexpr char const* helpstring =
R"(chplx -help:
  chplx [options] -f <full path to file name or file name>
    options: -E: suppress generating #line directives (default: false)
             -F: suppress generating full path for #line directives (default: true)
             -d: debug compiler phases (default : false) 
             -o: output path (default : $CWD)
)";

   std::cout << helpstring << std::flush;
}

int main(int argc, char ** argv) {

   Context context;
   Context * ctx = &context;
   ErrorGuard guard(ctx);

   std::optional<std::string> filePath{};

   // https://azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
   //
   int opt = 0;

   // Retrieve the options:
   while ( (opt = getopt(argc, argv, "f:hEFdo:")) != -1 ) {  // for each option...
      switch ( opt ) {
         case 'f':
            filePath = std::string{optarg};
         break;
         case 'E':
            chplx::util::suppressLineDirectives = true;
         break;
         case 'F':
            chplx::util::fullFilePath = false;
         break;
         case 'd':
            chplx::util::compilerDebug = true;
         break;
         case 'o':
            chplx::util::output_path = std::filesystem::path{std::string{optarg}};
            if(!std::filesystem::exists(chplx::util::output_path)) {
               std::error_code ec;
               if(!std::filesystem::create_directory(chplx::util::output_path, ec)) {
                  std::cerr << "chplx: unable to create " << chplx::util::output_path << "; check the `-o` argument for valid parent path" << std::endl;
                  return 0;
               }
            }
            else if(!std::filesystem::is_directory(chplx::util::output_path)) {
               std::cerr << "chplx: `-o` argument may not be a valid path to a directory" << std::endl;
               return 0;
            }
         break;
         default:
            std::cout << "chplx: unknown command line option: -" << opt << std::endl << std::flush;
            [[fallthrough]];
         case 'h':
            usage();
            return 1;
      }
   }

   if(!filePath.has_value()) {
      std::cerr << "chplx : error, file not provided" << std::endl << std::flush;
      return 0;
   }

   std::filesystem::path chplFilePth{*filePath};
   {
      if(!std::filesystem::exists(chplFilePth)) {
         std::cerr << "chplx : error, file does not exist error\t" << *filePath << std::endl << std::flush;
         return 0;
      }
   }

   if(chplx::util::output_path.string().size() < 1) {
       chplx::util::output_path = std::filesystem::current_path();
   }

   std::ifstream is(*filePath);
   if(!is.good()) {
      std::cerr << "chplx : error, file open error\t" << *filePath << std::endl << std::flush;
      return 0;
   }

   std::string fileContent(
      (std::istreambuf_iterator<char>(is)),
      std::istreambuf_iterator<char>()
   );

   chpl::parsing::setFileText(ctx, *filePath, fileContent);

   uast::BuilderResult const& br =
      parsing::parseFileToBuilderResult(ctx, chpl::UniqueString::get(ctx, *filePath), {});

   // for (auto & e : br.errors()) {
   //    ctx->report(e);
   // }

   uast::Module const* mod = br.singleModule();

   // name resolution; turned off currently b/c it
   // requires adding the chapel standard library
   // as the 'prolog' to the compiler; however, the
   // chapel standard library contains compile time
   // generated code that includes OS and architecture
   // specific implementations
   //
   // need to figure out how multi-module name
   // resolution works
   //
   //const auto& rr =
   //   chpl::resolution::resolveModule(ctx, mod->id());

   const auto numErrors = guard.numErrors();

   if(numErrors > 0) {
      guard.realizeErrors();
   }
   else {

      std::string ofilePath{};
      {
         std::regex file_regex{ R"([ \w-]+?(?=\.))" };
         auto words_begin = 
            std::sregex_iterator(filePath->begin(), filePath->end(), file_regex);
         auto words_end = std::sregex_iterator();

         if( std::distance(words_begin, words_end) != 1) {
            std::cerr << "Invalid input file encountered\t" << *filePath << std::endl;
            return -1;
         }

         std::smatch match = *words_begin;
         ofilePath = match.str() + ".cpp";
      }

      chplx::ast::hpx::ProgramTree program;

      chpl::ast::visitors::hpx::SymbolBuildingVisitor sbv{br, ctx, ofilePath};
      AstNode const* ast = static_cast<AstNode const*>(mod);
      if(chplx::util::compilerDebug) {
         std::cout << "[SymbolBuildingVisitor] Enter" << std::endl;
      }
      ast->traverse(sbv);
      if(chplx::util::compilerDebug) {
         std::cout << "[SymbolBuildingVisitor] Exit" << std::endl;
      }

      chplx::ast::visitors::hpx::ProgramTreeBuildingVisitor pbv{{}, nullptr, sbv.symbolTable.symbolTableRef, br, ctx, sbv.symbolTable, program, { &(program.statements) }, {}};
      if(chplx::util::compilerDebug) {
         std::cout << "[ProgramTreeBuildingVisitor] Enter" << std::endl;
      }
      ast->traverse(pbv);
      if(chplx::util::compilerDebug) {
         std::cout << "[ProgramTreeBuildingVisitor] Exit" << std::endl;
      }

      chpl::ast::visitors::hpx::CodegenVisitor cgv{sbv.symbolTable, sbv.configVars, program, br, ctx, ofilePath, chplFilePth.filename().string()};
      cgv.indent += 1;

      cgv.visit();

      CMakeGenerator gen{};
      gen.generate(chplFilePth);
   }

   return 0;
}
