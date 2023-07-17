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

#define SYMBOL_VISITOR_DEF(x) \
    template <>\
    void enter_helper<uast::asttags::x>(const uast::AstNode* ast);\

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

   SYMBOL_VISITOR_DEF(AnonFormal)    //
   SYMBOL_VISITOR_DEF(As)            //
   SYMBOL_VISITOR_DEF(Array)         //
   SYMBOL_VISITOR_DEF(Attributes)    //
   //SYMBOL_VISITOR_DEF(AssociativeArray)         //
   SYMBOL_VISITOR_DEF(Break)                  // old AST: GotoStmt
   SYMBOL_VISITOR_DEF(Catch)                  // old AST: CatchStmt
   SYMBOL_VISITOR_DEF(Cobegin)                //
   SYMBOL_VISITOR_DEF(Conditional)            // old AST: IfExpr/CondStmt
   SYMBOL_VISITOR_DEF(Comment)                //
   SYMBOL_VISITOR_DEF(Continue)               // old AST: GotoStmt
   SYMBOL_VISITOR_DEF(Delete)                 //
   SYMBOL_VISITOR_DEF(Domain)                 //
   SYMBOL_VISITOR_DEF(Dot)                    //
   SYMBOL_VISITOR_DEF(EmptyStmt)              //
   SYMBOL_VISITOR_DEF(ErroneousExpression)    //
   SYMBOL_VISITOR_DEF(ExternBlock)            // old AST: ExternBlockStmt
   SYMBOL_VISITOR_DEF(FunctionSignature)      // old AST: ExternBlockStmt
   SYMBOL_VISITOR_DEF(Identifier)             // old AST: UnresolvedSymExpr
   SYMBOL_VISITOR_DEF(Implements)             // old AST: ImplementsStmt
   SYMBOL_VISITOR_DEF(Import)                 // old AST: ImportStmt
   SYMBOL_VISITOR_DEF(Include)                //
   SYMBOL_VISITOR_DEF(Label)                  //
   SYMBOL_VISITOR_DEF(Let)                    //
   SYMBOL_VISITOR_DEF(New)                    //
   SYMBOL_VISITOR_DEF(Range)                  //
   SYMBOL_VISITOR_DEF(Require)                //
   SYMBOL_VISITOR_DEF(Return)                 //
   SYMBOL_VISITOR_DEF(Select)                 //
   SYMBOL_VISITOR_DEF(Throw)                  //
   SYMBOL_VISITOR_DEF(Try)                    // old AST: TryStmt
   SYMBOL_VISITOR_DEF(TypeQuery)              //
   SYMBOL_VISITOR_DEF(Use)                    // old AST: UseStmt
   SYMBOL_VISITOR_DEF(VisibilityClause)       //
   SYMBOL_VISITOR_DEF(WithClause)             //
   SYMBOL_VISITOR_DEF(Yield)                  //

   SYMBOL_VISITOR_DEF(Begin)
   SYMBOL_VISITOR_DEF(Block)     // old AST: BlockStmt
   SYMBOL_VISITOR_DEF(Defer)     // old AST: DeferStmt
   SYMBOL_VISITOR_DEF(Local)     //
   SYMBOL_VISITOR_DEF(Manage)    //
   SYMBOL_VISITOR_DEF(On)        //
   SYMBOL_VISITOR_DEF(Serial)    //
   SYMBOL_VISITOR_DEF(Sync)      //
   SYMBOL_VISITOR_DEF(When)      //

   SYMBOL_VISITOR_DEF(DoWhile)    // old AST: DoWhileStmt
   SYMBOL_VISITOR_DEF(While)      // old AST: WhileStmt

   SYMBOL_VISITOR_DEF(BracketLoop)
   SYMBOL_VISITOR_DEF(Coforall)
   SYMBOL_VISITOR_DEF(For)        // old AST: ForLoop / LoopExpr
   SYMBOL_VISITOR_DEF(Forall)     // old AST: ForallStmt / LoopExpr
   SYMBOL_VISITOR_DEF(Foreach)    //

   SYMBOL_VISITOR_DEF(BoolLiteral)
   SYMBOL_VISITOR_DEF(ImagLiteral)
   SYMBOL_VISITOR_DEF(IntLiteral)
   SYMBOL_VISITOR_DEF(RealLiteral)
   SYMBOL_VISITOR_DEF(UintLiteral)

   SYMBOL_VISITOR_DEF(BytesLiteral)
   SYMBOL_VISITOR_DEF(CStringLiteral)
   SYMBOL_VISITOR_DEF(StringLiteral)

   SYMBOL_VISITOR_DEF(FnCall)
   SYMBOL_VISITOR_DEF(OpCall)
   SYMBOL_VISITOR_DEF(PrimCall)    // old AST: CallExpr/PrimitiveOp
   SYMBOL_VISITOR_DEF(Reduce)      //
   SYMBOL_VISITOR_DEF(Scan)        //
   SYMBOL_VISITOR_DEF(Tuple)       //
   SYMBOL_VISITOR_DEF(Zip)

   SYMBOL_VISITOR_DEF(MultiDecl)
   SYMBOL_VISITOR_DEF(TupleDecl)
   SYMBOL_VISITOR_DEF(ForwardingDecl)

   SYMBOL_VISITOR_DEF(EnumElement)    // old AST: EnumSymbol
   SYMBOL_VISITOR_DEF(Function)       // old AST: FnSymbol
   SYMBOL_VISITOR_DEF(Interface)      // old AST: InterfaceSymbol
   SYMBOL_VISITOR_DEF(Module)         // old AST: ModuleSymbol
   SYMBOL_VISITOR_DEF(ReduceIntent)

   SYMBOL_VISITOR_DEF(Formal)          // old AST: ArgSymbol
   SYMBOL_VISITOR_DEF(TaskVar)         // old AST: ShadowVarSymbol
   SYMBOL_VISITOR_DEF(VarArgFormal)    // old AST: ArgSymbol
   SYMBOL_VISITOR_DEF(Variable)        // old AST: VarSymbol
   SYMBOL_VISITOR_DEF(Enum)            // old AST: EnumType

   SYMBOL_VISITOR_DEF(Class)     //
   SYMBOL_VISITOR_DEF(Record)    //
   SYMBOL_VISITOR_DEF(Union)     //

};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

#endif
