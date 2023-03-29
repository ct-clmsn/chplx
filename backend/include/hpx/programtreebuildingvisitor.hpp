/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#ifndef __CHPLX_PROGRAMTREEBUILDINGVISITOR_HPP__
#define __CHPLX_PROGRAMTREEBUILDINGVISITOR_HPP__

#include "chpl/uast/Builder.h"
#include "chpl/uast/AstNode.h"
#include "chpl/uast/Module.h"

#include "symboltypes.hpp"
#include "programtree.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace chplx::ast::hpx;
using namespace chpl;
using namespace chpl::ast::visitors::hpx;

namespace chplx { namespace ast { namespace visitors { namespace hpx {

struct ProgramTreeBuildingVisitor {

   std::string emitChapelLine(uast::AstNode const* ast);
   bool enter(const uast::AstNode * node);
   void exit(const uast::AstNode * node);

   std::optional<Statement> stmt;
   std::optional<uast::AstNode const*> node;

   std::size_t scopePtr;
   chpl::uast::BuilderResult const& br;
   SymbolTable & symbolTable;
   ProgramTree const& program;

   std::vector< std::vector<Statement> * > curStmts;
   std::optional<uast::AstTag> prevTag;

   static std::unordered_map<std::string, int> operatorEncoder;
};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chplx */

#endif
