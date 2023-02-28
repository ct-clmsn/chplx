/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#ifndef __CHPLX_SYMBOLBUILDINGVISITOR_HPP__
#define __CHPLX_SYMBOLBUILDINGVISITOR_HPP__

#include "chpl/uast/Builder.h"
#include "chpl/uast/AstNode.h"
#include "chpl/uast/Module.h"

#include "symboltypes.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace chpl { namespace ast { namespace visitors { namespace hpx {

struct SymbolBuildingVisitor {

   SymbolBuildingVisitor() = delete;

   SymbolBuildingVisitor(SymbolBuildingVisitor const& v) = delete;
   SymbolBuildingVisitor(SymbolBuildingVisitor && v) = delete;

   SymbolBuildingVisitor& operator=(SymbolBuildingVisitor const&) = delete;
   SymbolBuildingVisitor& operator=(SymbolBuildingVisitor &&) = delete;

   SymbolBuildingVisitor(SymbolBuildingVisitor * v) = delete;
   SymbolBuildingVisitor(SymbolBuildingVisitor const* v) = delete;

   SymbolBuildingVisitor(chpl::uast::BuilderResult const& chapel_br, std::string const& chapel_file_path_str, std::ostream & fstrm);
   ~SymbolBuildingVisitor() = default;

   template <typename Kind>
   void addSymbolEntry(char const* type);

   bool enter(const uast::AstNode * node);
   void exit(const uast::AstNode * node);

   void emitIndent() const;
   void emitChapelLine(uast::AstNode const* ast) const;
   void emitArrayKind(uast::AstNode const* ast, std::shared_ptr<array_kind> & sym);
   void emitArrayKindLit(uast::AstNode const* ast, std::shared_ptr<array_kind> & sym);

   template <typename T>
   void emit(uast::AstNode const* ast, T & sym, char const* type) const;
   template <typename Kind, typename T>
   void emitLit(uast::AstNode const* ast, T & sym, char const* type) const;

   bool emit(const uast::AstNode * ast, std::optional<Symbol> & sym);
   bool emit_literal(const uast::AstNode * ast, std::optional<Symbol> & sym);

   chpl::uast::BuilderResult const& br;
   std::size_t indent; 

   std::ostream & fstrm_;
   std::string chplFilePathStr;

   std::optional<Symbol> sym;
   std::optional<uast::AstNode const*> symnode;

   SymbolTable symbolTable;
};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

#endif
