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

#include "hpx/visitor.hpp"
#include "ErrorGuard.h"

#include <string>
#include <regex>
#include <algorithm>
#include <streambuf>
#include <fstream>
#include <iostream>

#include <getopt.h> /* getopt API */

using namespace chpl;
using namespace uast;

extern char *optarg;
extern int optind, opterr, optopt;
const char *optstring;

int main(int argc, char ** argv) {

   Context context;
   Context * ctx = &context;
   ErrorGuard guard(ctx);

   std::string filePath{};

   // https://azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
   //
   int opt = 0;

   // Retrieve the options:
   while ( (opt = getopt(argc, argv, "f:h")) != -1 ) {  // for each option...
      switch ( opt ) {
         case 'f':
            filePath = std::string{optarg};
         break;
         case 'h':
            std::cout << "chplx -help: `chplx -f <full path to file name or file name>`" << std::endl << std::flush;
            return 1;
         break;
      }
   }

   std::ifstream is(filePath);
   if(!is.good()) {
      std::cerr << "chplx : error, file not found\t" << filePath << std::endl << std::flush;
      return 0;
   }

   std::string fileContent(
      (std::istreambuf_iterator<char>(is)),
      std::istreambuf_iterator<char>()
   );

   chpl::parsing::setFileText(ctx, filePath, fileContent);

   uast::BuilderResult const& br =
      parsing::parseFileToBuilderResult(ctx, chpl::UniqueString::get(ctx, filePath), {});

   for (auto & e : br.errors()) {
      ctx->report(e);
   }

   uast::Module const* mod = br.singleModule();

   // name resolution; turned off currently b/c it
   // requires adding the chapel standard library
   // as the 'prolog' to the compiler; however, the
   // chapel standard library contains compile time
   // generated code that includes OS and architecture
   // specific implementations
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
            std::sregex_iterator(filePath.begin(), filePath.end(), file_regex);
         auto words_end = std::sregex_iterator();

         if( std::distance(words_begin, words_end) != 1) {
            std::cerr << "Invalid input file encountered\t" << filePath << std::endl;
            return -1;
         }

         std::smatch match = *words_begin;
         ofilePath = match.str() + ".cpp";
      }

      std::ofstream ofs(ofilePath);
      chpl::ast::visitors::hpx::Visitor v{ofs};

      AstNode const* ast = static_cast<AstNode const*>(mod);
      ast->traverse(v);
   }

   return 1;
}
