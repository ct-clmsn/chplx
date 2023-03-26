/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#ifndef __CHPLX_PROGRAMTREE_HPP__
#define __CHPLX_PROGRAMTREE_HPP__

#include "chpl/uast/AstNode.h"

#include "symboltypes.hpp"

#include <string>
#include <variant>
#include <utility>
#include <tuple>
#include <vector>
#include <ostream>

using namespace chpl;
using namespace chpl::ast::visitors::hpx;

namespace chplx { namespace ast { namespace hpx {

struct ExpressionBase {
   std::size_t scopeId;
};

struct VariableDeclarationExpression : public ExpressionBase {
   std::string identifier;
   kind_types kind;
   std::string chplLine;
};

struct ScalarDeclarationExpression : public VariableDeclarationExpression {
   void emit(std::ostream & os) const;
};

struct ScalarDeclarationLiteralExpression : public VariableDeclarationExpression {
   std::vector<uast::AstNode const*> literalValue;
   void emit(std::ostream & os) const;
};

struct ScalarDeclarationLiteralExpressionVisitor {
    template<typename T>
    void operator()(T const&) {}

    void operator()(bool_kind const&);
    void operator()(byte_kind const&);
    void operator()(int_kind const&);
    void operator()(real_kind const&);
    void operator()(string_kind const&);

    uast::AstNode const* ast;
    std::ostream & os;
};

struct ArrayDeclarationExpression : public VariableDeclarationExpression {
   void emit(std::ostream & os) const;
};

struct ArrayDeclarationLiteralExpression : public VariableDeclarationExpression {
   std::vector<uast::AstNode const*> literalValues;
   void emit(std::ostream & os) const;
};

struct TupleDeclarationExpression : public VariableDeclarationExpression {
   void emit(std::ostream & os) const;
};

struct TupleDeclarationLiteralExpression : public VariableDeclarationExpression {
   std::vector<uast::AstNode const*> literalValues;
   void emit(std::ostream & os) const;
};

struct ArithmeticOpExpression : public ExpressionBase {
   std::string op;
   uast::AstNode const *ast;
};

struct LiteralExpression {
   kind_types kind;
   uast::AstNode const * value;
   void emit(std::ostream & os) const;
};

struct VariableExpression {
   std::shared_ptr<Symbol> sym;
   void emit(std::ostream & os) const;
};

struct OpExpression {
   std::shared_ptr<Symbol> sym;
};

struct UnaryOpExpression;
struct BinaryOpExpression;
struct TernaryOpExpression;

struct ScopeExpression;
struct FunctionDeclarationExpression;
struct FunctionCallExpression;

struct ForallLoopExpression;

struct StatementList;

using Statement = std::variant<
   std::monostate,
   std::shared_ptr<StatementList>,
   ScalarDeclarationExpression,
   ScalarDeclarationLiteralExpression,
   ArrayDeclarationExpression,
   ArrayDeclarationLiteralExpression,
   TupleDeclarationExpression,
   TupleDeclarationLiteralExpression,
   LiteralExpression,
   VariableExpression,
   OpExpression,
   std::shared_ptr<UnaryOpExpression>,
   std::shared_ptr<BinaryOpExpression>,
   std::shared_ptr<TernaryOpExpression>,
   std::shared_ptr<ScopeExpression>,
   std::shared_ptr<FunctionDeclarationExpression>,
   std::shared_ptr<FunctionCallExpression>,
   std::shared_ptr<ForallLoopExpression>
>;

struct StatementList {
   std::vector<Statement> statements;
};

struct UnaryOpExpression : public ArithmeticOpExpression {
   std::vector<Statement> statements;

   void emit(std::ostream & os) const;
};

struct BinaryOpExpression : public ArithmeticOpExpression {
   std::vector<Statement> statements;

   void emit(std::ostream & os) const;
};

struct TernaryOpExpression : public ArithmeticOpExpression {
   std::vector<Statement> statements;

   void emit(std::ostream & os) const;
};

struct ScopeExpression : public ExpressionBase {
   std::optional<uast::AstNode const*> node;
   SymbolTable symbolTable;

   void emit(std::ostream & os) const;
};

struct FunctionDeclarationExpression : public ScopeExpression {
   Symbol symbol;
   std::vector<Statement> statements;
   std::string chplLine;

   void emit(std::ostream & os) const;
};

struct FunctionCallExpression : public ExpressionBase {
   Symbol symbol;
   std::vector<Statement> arguments;
   std::string chplLine;
   SymbolTable & symbolTable;

   void emit(std::ostream & os) const;
};

struct ForallLoopExpression : public ScopeExpression {
   std::vector<Statement> statements;
   Symbol iterator;
   Symbol index_set;

   void emit(std::ostream & os) const;
};

struct ProgramTree {

   std::vector<Statement> statements;

   ProgramTree() : statements() {}
};

/*
   std::shared_ptr<MethodCallExpression>,
   std::shared_ptr<ForallExpression>,
   std::shared_ptr<IfExpression>,
   std::shared_ptr<ElseExpression>,
   std::shared_ptr<BeginExpression>,
   std::shared_ptr<CobeginExpression>,
   std::shared_ptr<CoforallExpression>

struct ForExpression : public ExpressionBase {
    std::vector<Statement> statements;
};

struct MethodCallExpression : public ExpressionBase {
   std::string identifier;
   std::string methodIdentifier;
   std::vector<std::string> arguments;
};

struct FunctionDeclarationExpression;
struct IfExpression;
struct ElseExpression;
struct BeginExpression;
struct CobeginExpression;
struct CoforallExpression;
*/

} /* namespace hpx */ } /* namespace ast */ } /* namespace chplx */

#endif
