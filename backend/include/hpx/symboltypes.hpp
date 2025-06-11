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
//#include "hpx/variant.hpp"
#include "fmt/core.h"

#include <ostream>
#include <string>
#include <complex>
#include <unordered_map>
#include <variant>
#include <vector>
#include <functional>
#include <memory>
#include <optional>

//using namespace chplx::util;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

// support for chapel's template/generics '?" and "t : ?" types
//
struct template_kind {
   std::string identifier;
   template_kind() = default;
};

struct nil_kind {};

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

struct auto_kind {};

struct ref_kind;
struct const_kind;
struct config_kind;
struct cxxfunc_kind;
struct func_kind;
struct record_kind;
struct class_kind;
struct array_kind;
struct associative_kind;
struct tuple_kind;
struct kind_node_type;
struct kind_node_term_type {};
struct expr_kind {};
struct module_kind;
struct iter_kind;
struct range_kind;
struct domain_kind;

using kind_types = std::variant<
   std::monostate,
   template_kind,
   nil_kind,
   byte_kind,
   bool_kind,
   int_kind,
   real_kind,
   complex_kind,
   string_kind,
   std::shared_ptr<range_kind>,
   std::shared_ptr<domain_kind>,
   std::shared_ptr<ref_kind>,
   std::shared_ptr<const_kind>,
   std::shared_ptr<config_kind>,
   std::shared_ptr<cxxfunc_kind>,
   std::shared_ptr<func_kind>,
   std::shared_ptr<record_kind>,
   std::shared_ptr<class_kind>,
   std::shared_ptr<array_kind>,
   std::shared_ptr<associative_kind>,
   std::shared_ptr<tuple_kind>,
   std::shared_ptr<kind_node_type>,
   std::shared_ptr<module_kind>,
   kind_node_term_type,
   expr_kind,
   auto_kind,
   std::shared_ptr<iter_kind>
>;

struct kind_node_type {
   std::vector<kind_types> children;
};

struct ref_kind {
   kind_types kind;
};

struct const_kind {
   kind_types kind;
};

struct config_kind {
   kind_types kind;
};

struct associative_kind {
   kind_types key_kind;
   kind_types value_kind;
}; 

enum class ScopeKind : std::uint8_t {
   Module       = 0,
   ForLoop      = 1,
   ForallLoop   = 2,
   CoforallLoop = 3,
   Loop         = 4,
   Function     = 5,
   Lambda       = 6,
   Record       = 7, 
   Class        = 8 
};

struct SymbolBase {
    kind_types kind;
    std::string identifier;
    std::vector<uast::AstNode const*> literal;
    int kindqualifier;
    bool isConfig;
    std::size_t scopeId;

    bool isIntegralKind() {
        return kind.index() < 10 && scopeId == 0 && (
           identifier.find("nil") != std::string::npos ||
           identifier.find("bool") != std::string::npos ||
           identifier.find("string") != std::string::npos ||
           identifier.find("int") != std::string::npos ||
           identifier.find("real") != std::string::npos ||
           identifier.find("byte") != std::string::npos ||
           identifier.find("complex") != std::string::npos ||
           identifier.find("range") != std::string::npos ||
           identifier.find("domain") != std::string::npos );
    }
};

struct Symbol : public SymbolBase {
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
      // ScopeKind kind;
      std::map<std::string, Symbol> entries;
      std::vector<SymbolTableNodeImpl> children;
      SymbolTableNodeImpl parent;
      std::size_t parentModuleId;
   };

   SymbolTable(SymbolTable const& v) = default;
   SymbolTable(SymbolTable & v) = default;
   SymbolTable(SymbolTable && v) = default;
   SymbolTable(SymbolTable * v) = delete;
   SymbolTable(SymbolTable const* v) = delete;

   SymbolTable();

   std::size_t pushScope();
   void popScope();

   void addEntry(const std::size_t lutid, std::string const& ident, Symbol s);
   void addEntry(std::string const& ident, Symbol s);

   bool findImpl(std::shared_ptr<SymbolTableNode> & stref, std::string const& ident, std::map<std::string, Symbol>::iterator & ret);

   std::optional<Symbol> find(std::string const& ident);
   std::optional<Symbol> find(const std::size_t idx, std::string const& ident);

   std::optional< std::pair< std::map<std::string, Symbol>::iterator, std::map<std::string, Symbol>::iterator > > findPrefixImpl(std::shared_ptr<SymbolTableNode> & stref, std::string const& ident);
   std::optional< std::pair< std::map<std::string, Symbol>::iterator, std::map<std::string, Symbol>::iterator > > findPrefix(const std::size_t idx, std::string const& ident);

   void find(std::string const& ident, std::optional<Symbol> & s);
   void find(const std::size_t idx, std::string const& ident, std::optional<Symbol> & s);

   void dumpImpl(SymbolTableNode const& node);
   void dump();

   std::size_t scopePtr;

   std::size_t symbolTableCount() { return lut.size(); }

   // root of the SymbolTable
   //
   std::shared_ptr<SymbolTableNode> symbolTableRoot;

   // pointer to the current SymbolTableNode
   //
   std::shared_ptr<SymbolTableNode> symbolTableRef;

   // the symboltable contains a tree of variable scopes; each
   // node in the symboltable tree has a number associated with
   // it. the numbers are used to: count the symboltable tree
   // nodes, provide quick access to symbol table tree nodes,
   // and to minimize the use of pointers across the compiler
   //
   // lut is used to quickly access the entire symboltable tree;
   // the lut helps minimize log-ish look-up queries of the
   // symboltable tree; the tree should end up being log-ish sized
   // (it's a k-ary tree).
   //
   // note all entities that can be used to access the symboltable
   // have a 'scopeId' or 'scopePtr' member variable
   //
   std::vector<std::shared_ptr<SymbolTableNode>> lut;

   std::size_t parentSymbolTableId;

   std::vector< Symbol* > modules;
   std::size_t parentModuleId;
};

struct funcbase_kind {
   std::uint64_t lutId;
   std::string symbolTableSignature;
   std::vector<Symbol> args; 
   kind_types retKind;
};

struct cxxfunc_kind : public funcbase_kind {
};

struct range_kind : public funcbase_kind {
};

struct domain_kind : public funcbase_kind {
};

struct array_kind : public funcbase_kind {
};

struct func_kind : public funcbase_kind {
   bool is_iter;
   bool is_lambda;
};

struct tuple_kind : public funcbase_kind {
};

struct record_kind {
   std::uint64_t lutId;
   std::string symbolTableSignature;
   std::vector<Symbol> members; 
};

struct class_kind : record_kind {
   std::vector<Symbol> lineage; 
};

struct module_kind : funcbase_kind {
   // std::size_t parentModuleId;
};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

#endif
