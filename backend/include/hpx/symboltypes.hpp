/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#ifndef __CHPLX_SYMBOLTYPES_HPP__
#define __CHPLX_SYMBOLTYPES_HPP__

#include "chpl/uast/all-uast.h"
#include "hpx/variant.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <complex>
#include <unordered_map>
#include <variant>
#include <vector>
#include <functional>
#include <memory>

using namespace chplx::util;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

// support for chapel's template/generics '?" and "t : ?" types
//
struct template_kind {
   std::optional<std::string> identifier;
   template_kind() = default;
};

struct byte_kind {
   using uast_type = uast::BytesLiteral;
   using cxx_type = std::string;
   static cxx_type value(uast::AstNode const* ast) {
      return dynamic_cast<uast_type const*>(ast)->value().str();
   }
};
struct bool_kind {
   using uast_type = uast::BoolLiteral;
   using cxx_type = bool;
   static cxx_type value(uast::AstNode const* ast) {
      return dynamic_cast<uast_type const*>(ast)->value();
   }
};
struct int_kind {
   using uast_type = uast::IntLiteral;
   using cxx_type = std::int32_t;
   static cxx_type value(uast::AstNode const* ast) {
      return dynamic_cast<uast_type const*>(ast)->value();
   }
};
struct real_kind {
   using uast_type = uast::RealLiteral;
   using cxx_type = double;
   static cxx_type value(uast::AstNode const* ast) {
      return dynamic_cast<uast_type const*>(ast)->value();
   }
};
struct complex_kind {
//   using uast_type = uast::ComplexLiteral;
   using cxx_type = std::complex<double>;
   static cxx_type value(uast::AstNode const* ast) {
        return std::complex<double>{};
//      return dynamic_cast<uast_type const*>(ast)->value();
   }
};
struct string_kind {
   using uast_type = uast::StringLiteral;
   using cxx_type = std::string;
   static cxx_type value(uast::AstNode const* ast) {
      return dynamic_cast<uast_type const*>(ast)->value().str();
   }
};
struct range_kind {
   std::vector<std::int64_t> points;
   range_kind() = default;
};

struct domain_kind {
   std::vector<range_kind> ranges;
   domain_kind() = default;
};

struct func_kind;
struct record_kind;
struct class_kind;
struct array_kind;
struct associative_kind;
struct kind_node_type;
struct kind_node_term_type {};

using kind_types = std::variant<
   std::monostate,
   template_kind,
   byte_kind,
   bool_kind,
   int_kind,
   real_kind,
   complex_kind,
   string_kind,
   range_kind,
   domain_kind,
   std::shared_ptr<func_kind>,
   std::shared_ptr<record_kind>,
   std::shared_ptr<class_kind>,
   std::shared_ptr<array_kind>,
   std::shared_ptr<associative_kind>,
   std::shared_ptr<kind_node_type>,
   kind_node_term_type
>;

struct kind_node_type {
   std::vector<kind_types> children;
};

struct array_kind {
   kind_types kind;
   std::optional<domain_kind> dom;
   array_kind() = default;
};

struct associative_kind {
   kind_types key_kind;
   kind_types value_kind;
}; 

struct SymbolBase {
    std::optional<kind_types> kind;
    std::optional<std::string> identifier;
    std::optional<std::vector<uast::AstNode const*>> literal;
    std::size_t scopeId;

    SymbolBase() = default;
};

struct Symbol : public SymbolBase {
    std::optional<SymbolBase> parent;

    Symbol() = default;
};

struct SymbolTable {

   // The symbol table stores symbols using an
   // n-ary tree, nodes in the tree represent
   // variable scopes.
   //
   // Nodes are added to the tree when a new
   // scope encountered in the ast.
   //
   // The symbol table maintains a pointer
   // to a portion of the symbol table that
   // is currently being populated with
   // symbols.
   // 
   // Queries performed against the symbol table are
   // started from the current child or 'leaf' node
   // being populated, the search then continues up
   // the tree until the desired symbol is found or
   // the root is reached.
   // 

   struct SymbolTableNode;

   using SymbolTableNodeImpl = std::variant<
      std::monostate,
      std::shared_ptr<SymbolTableNode>
   >;

   struct SymbolTableNode {
      std::size_t id;
      std::unordered_map<std::string, Symbol> entries;
      std::vector<SymbolTableNodeImpl> children;
      std::optional<SymbolTableNodeImpl> parent;

      SymbolTableNode() = default;
   };

   SymbolTable(SymbolTable & v) = delete;
   SymbolTable(SymbolTable const& v) = delete;
   SymbolTable(SymbolTable * v) = delete;
   SymbolTable(SymbolTable const* v) = delete;

   SymbolTable();
   std::size_t pushScope();
   void popScope();
   void addEntry(std::string const& ident, Symbol s);
   std::optional<Symbol> findImpl(SymbolTableNode& stref, std::string const& ident);
   std::optional<Symbol> find(std::string const& ident);
   std::optional<Symbol> findImpl(SymbolTableNode& stref, const std::size_t idx, std::string const& ident);
   std::optional<Symbol> find(const std::size_t idx, std::string const& ident);
   void dumpImpl(SymbolTableNode const& node);
   void dump();

   // used to assign each SymbolTableNode a unique identifier
   //
   std::size_t symbolTableCount;

   // root of the SymbolTable
   //
   std::shared_ptr<SymbolTableNode> symbolTableRoot;

   // pointer to the current SymbolTableNode
   //
   std::shared_ptr<SymbolTableNode> symbolTableRef;

   std::vector<SymbolTableNode> lut;
};

struct func_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
   SymbolTable symbolTable;
};

struct record_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
   SymbolTable symbolTable;
};

struct class_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
   SymbolTable symbolTable;
};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

#endif
