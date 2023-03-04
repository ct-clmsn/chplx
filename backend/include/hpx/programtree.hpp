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
#include <vector>
#include <ostream>

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
   std::string literalValue;
   void emit(std::ostream & os) const;
};

struct ArrayDeclarationExpression : public VariableDeclarationExpression {
   void emit(std::ostream & os) const;
};

struct ArrayDeclarationLiteralExpression : public VariableDeclarationExpression {
   std::vector<std::string> literalValues;
   void emit(std::ostream & os) const;
};

/*
struct FunctionCallExpression : public ExpressionBase {
   std::string functionIdentifier;
   std::vector<std::string> arguments;
};

struct MethodCallExpression : public ExpressionBase {
   std::string identifier;
   std::string methodIdentifier;
   std::vector<std::string> arguments;
};

struct FunctionDeclarationExpression;
struct ForallExpression;
struct IfExpression;
struct ElseExpression;
struct BeginExpression;
struct CobeginExpression;
struct CoforallExpression;
*/

struct StatementList;

using Statement = std::variant<
   std::monostate,
   std::shared_ptr<StatementList>,
   ScalarDeclarationExpression,
   ScalarDeclarationLiteralExpression,
   ArrayDeclarationExpression
>;

/*
   std::shared_ptr<FunctionDeclarationExpression>,
   std::shared_ptr<FunctionCallExpression>,
   std::shared_ptr<MethodCallExpression>,
   std::shared_ptr<ForallExpression>,
   std::shared_ptr<IfExpression>,
   std::shared_ptr<ElseExpression>,
   std::shared_ptr<BeginExpression>,
   std::shared_ptr<CobeginExpression>,
   std::shared_ptr<CoforallExpression>
*/


struct StatementList {
   std::vector<Statement> statements;
};

/*
struct FunctionDeclarationExpression : public ExpressionBase {
    std::string identifier;
    SymbolTable st;
    std::vector<Statement> statements;

    FunctionDeclarationExpression() = default;
};

struct ForExpression : public ExpressionBase {
    std::vector<Statement> statements;
};
*/

struct ProgramTree {

   std::vector<Statement> statements;

   ProgramTree() : statements() {}
};

} /* namespace hpx */ } /* namespace ast */ } /* namespace chplx */

#endif
