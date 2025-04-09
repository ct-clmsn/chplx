/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#ifndef __CHPLX_CODEGENVISITOR_HPP__
#define __CHPLX_CODEGENVISITOR_HPP__

#include "chpl/uast/Builder.h"
#include "chpl/uast/AstNode.h"
#include "chpl/uast/Module.h"

#include "symboltypes.hpp"
#include "programtree.hpp"

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <filesystem>

using namespace chplx::ast::hpx;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

enum class HeaderEnum {
    std_vector = 0,
    std_string,
    std_complex,
    std_unordered_map,
    std_optional,
    std_variant,
    std_algorithm,
    std_numeric,
    std_fstream,
    std_iostream,
    std_functional,
    hpx_lco_channel,
    hpx_lco_future,
    hpx_algorithm,
    hpx_numeric,
    hpx_components,
    hpx_partitioned_vector,
    hpx_unordered_map,
    hpx_spmd,
    hpx_iostream,
    HeaderCount
};

struct CodegenVisitor {

   CodegenVisitor() = delete;

   CodegenVisitor(CodegenVisitor const& v) = delete;
   CodegenVisitor(CodegenVisitor && v) = delete;

   CodegenVisitor& operator=(CodegenVisitor const&) = delete;
   CodegenVisitor& operator=(CodegenVisitor &&) = delete;

   CodegenVisitor(CodegenVisitor * v) = delete;
   CodegenVisitor(CodegenVisitor const* v) = delete;

   CodegenVisitor(
      SymbolTable & symTable,
      std::vector< Symbol > & configVars,
      ProgramTree & prgmTree,
      chpl::uast::BuilderResult const& chapelBr,
      std::string const& cppFilePathStr,
      std::string const& chapelFilePathStr,
      Context* ctx_
   );

   ~CodegenVisitor() = default;

   void generateApplicationHeader(std::string const& chplpth, std::string const& prefix, std::fstream & os);

   //template <typename Kind>
   //void addSymbolEntry(char const* type);

   //bool enter(const uast::AstNode * node);
   //void exit(const uast::AstNode * node);

   void emitIndent() const;
   void emitArrayKind(Symbol const& s, uast::AstNode const* ast, std::shared_ptr<array_kind> & k);
   void emitArrayKindLit(Symbol const& s, uast::AstNode const* ast, std::shared_ptr<array_kind> & k);

   template <typename T>
   void emit(std::string const& ident, uast::AstNode const* ast, T & t, char const* type) const;
   template <typename Kind, typename T>
   void emitLit(std::string const& ident, uast::AstNode const* ast, T & sym, char const* type) const;

   bool emit(const uast::AstNode * ast, Symbol & sym);
   bool emit_literal(const uast::AstNode * ast, Symbol & sym);

   template<typename T>
   void visit(T && visitor) {}

   void visit(); 

   SymbolTable & symbolTable;
   std::vector< Symbol > & cfgVars;
   ProgramTree & programTree;
   chpl::uast::BuilderResult const& br;
   Context* ctx = nullptr;
   std::size_t indent; 
   std::size_t scope;
   std::string identifier;
   std::string cppFilePathStr;
   std::string chplFilePathStr;
   std::vector<bool> headers;
};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

#endif
