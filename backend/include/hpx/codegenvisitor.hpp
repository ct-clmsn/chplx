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

#include "chpl/uast/AstNode.h"
#include "chpl/uast/Builder.h"
#include "chpl/uast/Module.h"

#include "programtree.hpp"
#include "symboltypes.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace chplx::ast::hpx;

namespace chpl {
namespace ast {
namespace visitors {
namespace hpx {

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

  CodegenVisitor(CodegenVisitor const &v) = delete;
  CodegenVisitor(CodegenVisitor &&v) = delete;

  CodegenVisitor &operator=(CodegenVisitor const &) = delete;
  CodegenVisitor &operator=(CodegenVisitor &&) = delete;

  CodegenVisitor(CodegenVisitor *v) = delete;
  CodegenVisitor(CodegenVisitor const *v) = delete;

  CodegenVisitor(SymbolTable &symTable, ProgramTree &prgmTree,
                 chpl::uast::BuilderResult const &chapelBr,
                 std::string const &chapelFilePathStr, std::ostream &fstrm);

  ~CodegenVisitor() = default;

  void generateApplicationHeader();

  // template <typename Kind>
  // void addSymbolEntry(char const* type);

  // bool enter(const uast::AstNode * node);
  // void exit(const uast::AstNode * node);

  void emitIndent() const;
  void emitChapelLine(uast::AstNode const *ast) const;
  void emitArrayKind(Symbol const &s, uast::AstNode const *ast,
                     std::shared_ptr<array_kind> &k);
  void emitArrayKindLit(Symbol const &s, uast::AstNode const *ast,
                        std::shared_ptr<array_kind> &k);

  template <typename T>
  void emit(std::string const &ident, uast::AstNode const *ast, T &t,
            char const *type) const;
  template <typename Kind, typename T>
  void emitLit(std::string const &ident, uast::AstNode const *ast, T &sym,
               char const *type) const;

  bool emit(const uast::AstNode *ast, std::optional<Symbol> &sym);
  bool emit_literal(const uast::AstNode *ast, std::optional<Symbol> &sym);

  template <typename T> void visit(T &&visitor) {}

  void visit();

  SymbolTable &symbolTable;
  ProgramTree &programTree;
  chpl::uast::BuilderResult const &br;
  std::size_t indent;
  std::size_t scope;
  std::optional<std::string> identifier;
  std::ostream &fstrm_;
  std::string chplFilePathStr;
  std::vector<bool> headers;
};

} /* namespace hpx */
} /* namespace visitors */
} /* namespace ast */
} /* namespace chpl */

#endif
