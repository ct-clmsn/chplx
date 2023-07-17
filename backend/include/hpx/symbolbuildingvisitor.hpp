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

#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace chpl { namespace ast { namespace visitors { namespace hpx {

// clang-format off

#define SYMBOL_VISITOR_DEC_ENTER(x) \
    template <>\
    void enter_helper<uast::asttags::x>(const uast::AstNode* ast);\

#define SYMBOL_VISITOR_DEC_EXIT(x) \
    template <>\
    void exit_helper<uast::asttags::x>(const uast::AstNode* ast);\

#define SYMBOL_VISITOR_DEF_ENTER_H_F(x) \
    template <> \
    void SymbolBuildingVisitor::enter_helper<uast::asttags::x>(const uast::AstNode* ast)\

#define SYMBOL_VISITOR_DEF_EXIT_H_F(x) \
    template <> \
    void SymbolBuildingVisitor::exit_helper<uast::asttags::x>(const uast::AstNode* ast)\

#define MAKE_CASE_WITH_ENTER_HELPER_F(x) \
         case asttags::x:\
            enter_helper<MAKE_ENUM_T(x)>(ast);\
         break;\

#define MAKE_CASE_WITH_EXIT_HELPER_F(x) \
         case asttags::x:\
            exit_helper<MAKE_ENUM_T(x)>(ast);\
         break;\

#define MAKE_CASE_NO_F(x) \
         case asttags::x:\
         break;\

#define MAKE_ENUM_T(x) uast::asttags::x

// clang-format on

struct SymbolBuildingVisitor {

   SymbolBuildingVisitor() = delete;

   SymbolBuildingVisitor(SymbolBuildingVisitor const& v) = delete;
   SymbolBuildingVisitor(SymbolBuildingVisitor && v) = delete;

   SymbolBuildingVisitor& operator=(SymbolBuildingVisitor const&) = delete;
   SymbolBuildingVisitor& operator=(SymbolBuildingVisitor &&) = delete;

   SymbolBuildingVisitor(SymbolBuildingVisitor * v) = delete;
   SymbolBuildingVisitor(SymbolBuildingVisitor const* v) = delete;

   SymbolBuildingVisitor(chpl::uast::BuilderResult const& chapel_br, std::string const& chapel_file_path_str);
   ~SymbolBuildingVisitor() = default;

   template <typename Kind>
   void addSymbolEntry(char const* type);

   void addSymbolEntry(char const* type, Symbol && symbol);

   std::string emitChapelLine(uast::AstNode const* ast);

   bool enter(const uast::AstNode * node);
   void exit(const uast::AstNode * node);

   chpl::uast::BuilderResult const& br;
   std::size_t indent; 

   std::string chplFilePathStr;

   std::vector<Symbol> symstack;
   std::optional< std::reference_wrapper<Symbol> > sym;
   uast::AstNode * symnode;

   std::vector< Symbol > configVars;
   SymbolTable symbolTable;

   // static std::unordered_map<uast::asttags::AstTag, auto> vals_map = {{}};
   template<uast::asttags::AstTag t>
   void enter_helper(const uast::AstNode*);

   template<uast::asttags::AstTag t>
   void exit_helper(const uast::AstNode*);

   SYMBOL_VISITOR_DEC_ENTER(Array)         //
   SYMBOL_VISITOR_DEC_ENTER(Domain)        //
   SYMBOL_VISITOR_DEC_ENTER(Identifier)    // old AST: UnresolvedSymExpr
   SYMBOL_VISITOR_DEC_ENTER(Range)

   SYMBOL_VISITOR_DEC_ENTER(BoolLiteral)
   SYMBOL_VISITOR_DEC_ENTER(ImagLiteral)
   SYMBOL_VISITOR_DEC_ENTER(IntLiteral)
   SYMBOL_VISITOR_DEC_ENTER(RealLiteral)
   SYMBOL_VISITOR_DEC_ENTER(UintLiteral)

   SYMBOL_VISITOR_DEC_ENTER(BytesLiteral)
   SYMBOL_VISITOR_DEC_ENTER(CStringLiteral)
   SYMBOL_VISITOR_DEC_ENTER(StringLiteral)

   SYMBOL_VISITOR_DEC_ENTER(FnCall)
   SYMBOL_VISITOR_DEC_ENTER(OpCall)
   SYMBOL_VISITOR_DEC_ENTER(Tuple)    //

   SYMBOL_VISITOR_DEC_ENTER(Formal)      // old AST: ArgSymbol
   SYMBOL_VISITOR_DEC_ENTER(Variable)    // old AST: VarSymbol

   SYMBOL_VISITOR_DEC_ENTER(Conditional)    //
   SYMBOL_VISITOR_DEC_ENTER(Block)          //
   SYMBOL_VISITOR_DEC_ENTER(BracketLoop)
   SYMBOL_VISITOR_DEC_ENTER(Coforall)
   SYMBOL_VISITOR_DEC_ENTER(For)
   SYMBOL_VISITOR_DEC_ENTER(Forall)

   SYMBOL_VISITOR_DEC_ENTER(Function)
   SYMBOL_VISITOR_DEC_ENTER(Module)
   SYMBOL_VISITOR_DEC_ENTER(Record)
   SYMBOL_VISITOR_DEC_ENTER(Class)

   SYMBOL_VISITOR_DEC_EXIT(Range)
   SYMBOL_VISITOR_DEC_EXIT(Variable)
   SYMBOL_VISITOR_DEC_EXIT(Block)
   SYMBOL_VISITOR_DEC_EXIT(Function)
   SYMBOL_VISITOR_DEC_EXIT(Module)
   SYMBOL_VISITOR_DEC_EXIT(Record)
   SYMBOL_VISITOR_DEC_EXIT(Class)
};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

#endif
