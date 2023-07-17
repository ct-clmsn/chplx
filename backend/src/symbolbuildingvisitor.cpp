/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "hpx/symbolbuildingvisitor.hpp"
#include "hpx/util.hpp"
#include "chpl/uast/all-uast.h"

#include <variant>
#include <fstream>
#include <cctype>
#include <filesystem>
#include <numeric>
#include <cassert>

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

void SymbolBuildingVisitor::addSymbolEntry(char const* type, Symbol && symbol) {
   symbolTable.addEntry(symbol.identifier, symbol);
}

SymbolBuildingVisitor::SymbolBuildingVisitor(chpl::uast::BuilderResult const& chapelBr, std::string const& cfps)
   : br(chapelBr), indent(0),
     chplFilePathStr(cfps),
     symstack(), sym(), symnode(), configVars(), symbolTable()
{
   // populate the symbol table with a base set of entries:
   //
   // bool, string, int, real, byte, complex, range, domain, template(?)
   //
   addSymbolEntry("nil", Symbol{{nil_kind{}, std::string{"nil"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("bool", Symbol{{bool_kind{}, std::string{"bool"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("string", Symbol{{string_kind{}, std::string{"string"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("int", Symbol{{int_kind{}, std::string{"int"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("real", Symbol{{real_kind{}, std::string{"real"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("byte", Symbol{{byte_kind{}, std::string{"byte"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("complex", Symbol{{complex_kind{}, std::string{"complex"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("tuple", Symbol{{std::make_shared<tuple_kind>(tuple_kind{{}}), std::string{"tuple"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("range", Symbol{{std::make_shared<range_kind>(range_kind{{}}), std::string{"range"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("domain", Symbol{{std::make_shared<domain_kind>(domain_kind{{}}), std::string{"domain"}, {}, -1, false, symbolTable.symbolTableRef->id}});
   addSymbolEntry("?", Symbol{{template_kind{}, std::string{"?"}, {}, -1, false, symbolTable.symbolTableRef->id}});

   // inlinecxx - allows users to inline c++ code into their chapel programs
   //
   addSymbolEntry("inlinecxx",
      Symbol{{std::make_shared<cxxfunc_kind>(cxxfunc_kind{{{}, "inlinecxx", {}, nil_kind{}}}), std::string{"inlinecxx"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );

   // incdirs - allows users to provide include paths for header files used in `inlinecxx` 
   //
   addSymbolEntry("incdirs",
      Symbol{{std::make_shared<cxxfunc_kind>(cxxfunc_kind{{{}, "incdirs", {}, nil_kind{}}}), std::string{"incdirs"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );

   // libdirs - allows users to provide library paths for libraries used in `inlinecxx` 
   //
   addSymbolEntry("libdirs",
      Symbol{{std::make_shared<cxxfunc_kind>(cxxfunc_kind{{{}, "libdirs", {}, nil_kind{}}}), std::string{"libdirs"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );

   // libs - allows users to provide libraries used in `inlinecxx` 
   //
   addSymbolEntry("libs",
      Symbol{{std::make_shared<cxxfunc_kind>(cxxfunc_kind{{{}, "libs", {}, nil_kind{}}}), std::string{"libs"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );

   // findpkgs - allows users to provide cmake packages used in `inlinecxx` 
   //
   addSymbolEntry("findpkgs",
      Symbol{{std::make_shared<cxxfunc_kind>(cxxfunc_kind{{{}, "findpkgs", {}, nil_kind{}}}), std::string{"findpkgs"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );

   // pkgcfg - allows users to provide gnu pkg-config packages used in `inlinecxx` 
   //
   addSymbolEntry("pkgcfg",
      Symbol{{std::make_shared<cxxfunc_kind>(cxxfunc_kind{{{}, "pkgcfg", {}, nil_kind{}}}), std::string{"pkgcfg"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );

   // flagscxx - allows users to provide `-D` compile time arguments
   //
   addSymbolEntry("flagscxx",
      Symbol{{std::make_shared<cxxfunc_kind>(cxxfunc_kind{{{}, "flagscxx", {}, nil_kind{}}}), std::string{"flagscxx"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );

   // +,-,*,/,%,=
   //
   addSymbolEntry("+",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "+", {}, {}}}), std::string{"+"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
   addSymbolEntry("-",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "-", {}, {}}}), std::string{"-"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
   addSymbolEntry("*",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "*", {}, {}}}), std::string{"*"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
   addSymbolEntry("/",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "/", {}, {}}}), std::string{"/"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
   addSymbolEntry("%",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "%", {}, {}}}), std::string{"%"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
   addSymbolEntry("=",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "=", {}, {}}}), std::string{"="}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
   addSymbolEntry("[]",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "[]", {}, {}}}), std::string{"[]"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
   addSymbolEntry("==",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "==", {}, {}}}), std::string{"=="}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
   addSymbolEntry("<=>",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "<=>", {}, {}}}), std::string{"<=>"}, {}, -1, false, symbolTable.symbolTableRef->id}}
   );
}

std::string SymbolBuildingVisitor::emitChapelLine(uast::AstNode const* ast) {
   auto const fp = br.filePath();
   return chplx::util::emitLineDirective(fp.c_str(), br.idToLocation(ast->id(), fp).line());
}

bool SymbolBuildingVisitor::enter(const uast::AstNode * ast) {
   if(chplx::util::compilerDebug) {
      std::cout << "***Enter AST Node\t" << tagToString(ast->tag()) << std::endl
                << "***\tCurrent Scope\t" << symbolTable.symbolTableRef->id << std::endl
                << "***\t" << emitChapelLine(ast);
   }
   auto val = ast->tag();
   switch(val) {
    MAKE_CASE_NO_F(AnonFormal)
    MAKE_CASE_NO_F(As)
    MAKE_CASE_WITH_ENTER_HELPER_F(Array)
    MAKE_CASE_NO_F(Attributes)
    MAKE_CASE_NO_F(Break)
    MAKE_CASE_NO_F(Comment)
    MAKE_CASE_NO_F(Continue)
    MAKE_CASE_NO_F(Delete)
    MAKE_CASE_WITH_ENTER_HELPER_F(Domain)
    MAKE_CASE_NO_F(Dot)
    MAKE_CASE_NO_F(EmptyStmt)
    MAKE_CASE_NO_F(ErroneousExpression)
    MAKE_CASE_NO_F(ExternBlock)
    MAKE_CASE_NO_F(FunctionSignature)
    MAKE_CASE_WITH_ENTER_HELPER_F(Identifier)
    MAKE_CASE_NO_F(Import)
    MAKE_CASE_NO_F(Include)
    MAKE_CASE_NO_F(Let)
    MAKE_CASE_NO_F(New)
    MAKE_CASE_WITH_ENTER_HELPER_F(Range)
    MAKE_CASE_NO_F(Require)
    MAKE_CASE_NO_F(Return)
    MAKE_CASE_NO_F(Throw)
    MAKE_CASE_NO_F(TypeQuery)
    MAKE_CASE_NO_F(Use)
    MAKE_CASE_NO_F(VisibilityClause)
    MAKE_CASE_NO_F(WithClause)
    MAKE_CASE_NO_F(Yield)
    MAKE_CASE_NO_F(START_Literal)
    MAKE_CASE_WITH_ENTER_HELPER_F(BoolLiteral)
    MAKE_CASE_WITH_ENTER_HELPER_F(ImagLiteral)
    MAKE_CASE_WITH_ENTER_HELPER_F(IntLiteral)
    MAKE_CASE_WITH_ENTER_HELPER_F(RealLiteral)
    MAKE_CASE_WITH_ENTER_HELPER_F(UintLiteral)
    MAKE_CASE_NO_F(START_StringLikeLiteral)
    MAKE_CASE_WITH_ENTER_HELPER_F(BytesLiteral)
    MAKE_CASE_WITH_ENTER_HELPER_F(CStringLiteral)
    MAKE_CASE_WITH_ENTER_HELPER_F(StringLiteral)
    MAKE_CASE_NO_F(END_StringLikeLiteral)
    MAKE_CASE_NO_F(END_Literal)
    MAKE_CASE_NO_F(START_Call)
    MAKE_CASE_WITH_ENTER_HELPER_F(FnCall)
    MAKE_CASE_WITH_ENTER_HELPER_F(OpCall)
    MAKE_CASE_NO_F(PrimCall)
    MAKE_CASE_NO_F(Reduce)
    MAKE_CASE_NO_F(ReduceIntent)
    MAKE_CASE_NO_F(Scan)
    MAKE_CASE_WITH_ENTER_HELPER_F(Tuple)
    MAKE_CASE_NO_F(Zip)
    MAKE_CASE_NO_F(END_Call)
    MAKE_CASE_NO_F(MultiDecl)
    MAKE_CASE_NO_F(TupleDecl)
    MAKE_CASE_NO_F(ForwardingDecl)
    MAKE_CASE_NO_F(EnumElement)
    MAKE_CASE_NO_F(START_VarLikeDecl)
    MAKE_CASE_WITH_ENTER_HELPER_F(Formal)
    MAKE_CASE_NO_F(TaskVar)
    MAKE_CASE_NO_F(VarArgFormal)
    MAKE_CASE_WITH_ENTER_HELPER_F(Variable)
    MAKE_CASE_NO_F(END_VarLikeDecl)
    MAKE_CASE_NO_F(Enum)
    MAKE_CASE_NO_F(Catch)
    MAKE_CASE_NO_F(Cobegin)
    MAKE_CASE_WITH_ENTER_HELPER_F(Conditional)
    MAKE_CASE_NO_F(Implements)
    MAKE_CASE_NO_F(Label) // contains a loop
    MAKE_CASE_NO_F(Select)
    MAKE_CASE_NO_F(Sync)
    MAKE_CASE_NO_F(Try)
    MAKE_CASE_NO_F(START_SimpleBlockLike)
    MAKE_CASE_NO_F(Begin)
    MAKE_CASE_WITH_ENTER_HELPER_F(Block)
    MAKE_CASE_NO_F(Defer)
    MAKE_CASE_NO_F(Local)
    MAKE_CASE_NO_F(Manage)
    MAKE_CASE_NO_F(On)
    MAKE_CASE_NO_F(Serial)
    MAKE_CASE_NO_F(When)
    MAKE_CASE_NO_F(END_SimpleBlockLike)
    MAKE_CASE_NO_F(START_Loop)
    MAKE_CASE_NO_F(DoWhile)
    MAKE_CASE_NO_F(While)
    MAKE_CASE_NO_F(START_IndexableLoop)
    MAKE_CASE_WITH_ENTER_HELPER_F(BracketLoop)
    MAKE_CASE_WITH_ENTER_HELPER_F(Coforall)
    MAKE_CASE_WITH_ENTER_HELPER_F(For)
    MAKE_CASE_WITH_ENTER_HELPER_F(Forall)
    MAKE_CASE_NO_F(Foreach)
    MAKE_CASE_NO_F(END_IndexableLoop)
    MAKE_CASE_NO_F(END_Loop)
    MAKE_CASE_NO_F(START_Decl)
    MAKE_CASE_NO_F(START_NamedDecl)
    MAKE_CASE_NO_F(START_TypeDecl)
    MAKE_CASE_WITH_ENTER_HELPER_F(Function)
    MAKE_CASE_NO_F(Interface)
    MAKE_CASE_WITH_ENTER_HELPER_F(Module)
    MAKE_CASE_NO_F(START_AggregateDecl)
    MAKE_CASE_WITH_ENTER_HELPER_F(Record)
    MAKE_CASE_WITH_ENTER_HELPER_F(Class)
    MAKE_CASE_NO_F(Union)
    MAKE_CASE_NO_F(END_AggregateDecl)
    MAKE_CASE_NO_F(END_Decl)
    MAKE_CASE_NO_F(END_NamedDecl)
    MAKE_CASE_NO_F(END_TypeDecl)
    MAKE_CASE_NO_F(NUM_AST_TAGS)
    MAKE_CASE_NO_F(AST_TAG_UNKNOWN)
    default:
    break;
   }

   return true;
}



void SymbolBuildingVisitor::exit(const uast::AstNode * ast) {
   if(chplx::util::compilerDebug) {
      std::cout << "---Exit AST Node\t" << tagToString(ast->tag()) << std::endl
                << "---\tCurrent Scope\t" << symbolTable.symbolTableRef->id << std::endl
                << "---\t" << emitChapelLine(ast);
   }
   switch(ast->tag()) {
    MAKE_CASE_NO_F(AnonFormal)
    MAKE_CASE_NO_F(As)
    MAKE_CASE_NO_F(Array)
    MAKE_CASE_NO_F(Attributes)
    MAKE_CASE_NO_F(Break)
    MAKE_CASE_NO_F(Comment)
    MAKE_CASE_NO_F(Continue)
    MAKE_CASE_NO_F(Delete)
    MAKE_CASE_NO_F(Domain)
    MAKE_CASE_NO_F(Dot)
    MAKE_CASE_NO_F(EmptyStmt)
    MAKE_CASE_NO_F(ErroneousExpression)
    MAKE_CASE_NO_F(ExternBlock)
    MAKE_CASE_NO_F(FunctionSignature)
    MAKE_CASE_NO_F(Identifier)
    MAKE_CASE_NO_F(Import)
    MAKE_CASE_NO_F(Include)
    MAKE_CASE_NO_F(Let)
    MAKE_CASE_NO_F(New)
    MAKE_CASE_WITH_EXIT_HELPER_F(Range)
    MAKE_CASE_NO_F(Require)
    MAKE_CASE_NO_F(Return)
    MAKE_CASE_NO_F(Throw)
    MAKE_CASE_NO_F(TypeQuery)
    MAKE_CASE_NO_F(Use)
    MAKE_CASE_NO_F(VisibilityClause)
    MAKE_CASE_NO_F(WithClause)
    MAKE_CASE_NO_F(Yield)
    MAKE_CASE_NO_F(START_Literal)
    MAKE_CASE_NO_F(BoolLiteral)
    MAKE_CASE_NO_F(ImagLiteral)
    MAKE_CASE_NO_F(IntLiteral)
    MAKE_CASE_NO_F(RealLiteral)
    MAKE_CASE_NO_F(UintLiteral)
    MAKE_CASE_NO_F(BytesLiteral)
    MAKE_CASE_NO_F(CStringLiteral)
    MAKE_CASE_NO_F(StringLiteral)
    MAKE_CASE_NO_F(START_StringLikeLiteral)
    MAKE_CASE_NO_F(END_StringLikeLiteral)
    MAKE_CASE_NO_F(END_Literal)
    MAKE_CASE_NO_F(START_Call)
    MAKE_CASE_NO_F(FnCall)
    MAKE_CASE_NO_F(OpCall)
    MAKE_CASE_NO_F(PrimCall)
    MAKE_CASE_NO_F(Reduce)
    MAKE_CASE_NO_F(ReduceIntent)
    MAKE_CASE_NO_F(Scan)
    MAKE_CASE_NO_F(Tuple)
    MAKE_CASE_NO_F(Zip)
    MAKE_CASE_NO_F(END_Call)
    MAKE_CASE_NO_F(MultiDecl)
    MAKE_CASE_NO_F(TupleDecl)
    MAKE_CASE_NO_F(ForwardingDecl)
    MAKE_CASE_NO_F(EnumElement)
    MAKE_CASE_NO_F(START_VarLikeDecl)
    MAKE_CASE_NO_F(Formal)
    MAKE_CASE_NO_F(TaskVar)
    MAKE_CASE_NO_F(VarArgFormal)
    MAKE_CASE_WITH_EXIT_HELPER_F(Variable)
    MAKE_CASE_NO_F(END_VarLikeDecl)
    MAKE_CASE_NO_F(Enum)
    MAKE_CASE_NO_F(Catch)
    MAKE_CASE_NO_F(Cobegin)
    MAKE_CASE_NO_F(Implements)
    MAKE_CASE_NO_F(Label) // contains a loop
    MAKE_CASE_NO_F(Select)
    MAKE_CASE_NO_F(Sync)
    MAKE_CASE_NO_F(Try)
    MAKE_CASE_NO_F(START_SimpleBlockLike)
    MAKE_CASE_NO_F(Begin)
    MAKE_CASE_NO_F(Defer)
    MAKE_CASE_NO_F(Local)
    MAKE_CASE_NO_F(Manage)
    MAKE_CASE_NO_F(On)
    MAKE_CASE_NO_F(Serial)
    MAKE_CASE_NO_F(When)
    MAKE_CASE_NO_F(END_SimpleBlockLike)
    MAKE_CASE_NO_F(START_Loop)
    MAKE_CASE_NO_F(DoWhile)
    MAKE_CASE_NO_F(While)
    MAKE_CASE_NO_F(START_IndexableLoop)
    MAKE_CASE_NO_F(BracketLoop)
    MAKE_CASE_NO_F(Coforall)
    MAKE_CASE_NO_F(For)
    MAKE_CASE_NO_F(Forall)
    MAKE_CASE_NO_F(Conditional)
    MAKE_CASE_WITH_EXIT_HELPER_F(Block)
    MAKE_CASE_NO_F(Foreach)
    MAKE_CASE_NO_F(END_IndexableLoop)
    MAKE_CASE_NO_F(END_Loop)
    MAKE_CASE_NO_F(START_Decl)
    MAKE_CASE_NO_F(START_NamedDecl)
    MAKE_CASE_NO_F(START_TypeDecl)
    MAKE_CASE_WITH_EXIT_HELPER_F(Function)
    MAKE_CASE_NO_F(Interface)
    MAKE_CASE_WITH_EXIT_HELPER_F(Module)
    MAKE_CASE_NO_F(START_AggregateDecl)
    MAKE_CASE_WITH_EXIT_HELPER_F(Record)
    MAKE_CASE_WITH_EXIT_HELPER_F(Class)
    MAKE_CASE_NO_F(Union)
    MAKE_CASE_NO_F(END_AggregateDecl)
    MAKE_CASE_NO_F(END_Decl)
    MAKE_CASE_NO_F(END_NamedDecl)
    MAKE_CASE_NO_F(END_TypeDecl)
    MAKE_CASE_NO_F(NUM_AST_TAGS)
    MAKE_CASE_NO_F(AST_TAG_UNKNOWN)
    default:
    break;
   }
}

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

