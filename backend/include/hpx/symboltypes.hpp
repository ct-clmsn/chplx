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

#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <functional>
#include <memory>

namespace chpl { namespace ast { namespace visitors { namespace hpx {

// support for chapel's template/generics '?" and "t : ?" types
//
struct template_kind {
   std::optional<std::string> identifier;
   template_kind() = default;
};

struct byte_kind {};
struct bool_kind {};
struct int_kind {};
struct real_kind {};
struct complex_kind {};
struct string_kind {};
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
   std::shared_ptr<associative_kind>
>;

struct array_kind {
   kind_types kind;
   domain_kind dom;   

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

   SymbolTable() : symbolTableCount(0), symbolTableRef() {
       symbolTableRoot =
          std::make_shared<SymbolTableNode>(
             std::move(SymbolTableNode{0, {}, {}, {}})
          );
       ++symbolTableCount;
       symbolTableRef = symbolTableRoot;
   }

   std::size_t pushScope() {
      // new scopes are appended to
      // the end of the symboltable
      //
      ++symbolTableCount;
      symbolTableRef->children.emplace_back(
         std::make_shared<SymbolTableNode>(std::move(SymbolTableNode{symbolTableCount, {}, {}, symbolTableRef}))
      );

      symbolTableRef = std::get<std::shared_ptr<SymbolTableNode>>(symbolTableRef->children.back());
      return symbolTableCount;
   }

   void popScope() {
      if(symbolTableRef->parent && std::holds_alternative<std::shared_ptr<SymbolTableNode>>(*symbolTableRef->parent)) {
          symbolTableRef = std::get<std::shared_ptr<SymbolTableNode>>(*symbolTableRef->parent);
      }
   }

   void addEntry(std::string const& ident, Symbol s) {
      symbolTableRef->entries.insert(std::make_pair(ident, std::move(s)));
   }

   std::optional<Symbol> findImpl(SymbolTableNode& stref, std::string const& ident) {
      auto entry = stref.entries.find(ident);
      if(entry != std::end(stref.entries)) {
          return entry->second;
      }

      if(!stref.parent) {
         return {};
      }

      return findImpl(*std::get<std::shared_ptr<SymbolTableNode>>(*stref.parent), ident);
   }

   std::optional<Symbol> find(std::string const& ident) {
      SymbolTableNode& stref = *symbolTableRef; 
      return findImpl(stref, ident);
   }

   std::optional<Symbol> findImpl(SymbolTableNode& stref, const std::size_t idx, std::string const& ident) {
      if(stref.entries.size() < 1) {
         return {};
      }

      if(idx == stref.id) {
         auto entry = stref.entries.find(ident);
         if(entry != std::end(stref.entries)) {
            return entry->second;
         }
         return {};
      }

      for(auto & child : stref.children) {
         auto chld =
            findImpl(*std::get<std::shared_ptr<SymbolTableNode>>(child), idx, ident);
         if(chld) {
            return chld;
         }
      }

      return {};
   }

   std::optional<Symbol> find(const std::size_t idx, std::string const& ident) {
      SymbolTableNode& stref = *symbolTableRoot; 
      return findImpl(stref, idx, ident);
   }

   void dumpImpl(SymbolTableNode const& node) {
      if(node.entries.size() < 1) {
         return;
      }

      for(auto const& entry : node.entries) {
         std::cout << entry.first << std::endl;
      }

      for(auto const& child : node.children) {
         dumpImpl(*std::get<std::shared_ptr<SymbolTableNode>>(child));
      }
   }

   void dump() {
      SymbolTableNode& stref = *symbolTableRoot;
      dumpImpl(stref);
   }

   // used to assign each SymbolTableNode a unique identifier
   //
   std::size_t symbolTableCount;

   // root of the SymbolTable
   //
   std::shared_ptr<SymbolTableNode> symbolTableRoot;

   // pointer to the current SymbolTableNode
   //
   std::shared_ptr<SymbolTableNode> symbolTableRef;
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
