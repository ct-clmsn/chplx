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

#include "chpl/uast/Builder.h"
#include "chpl/uast/AstNode.h"
#include "chpl/uast/Module.h"

#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

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

struct func_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
};

struct record_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
};

struct class_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
};

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
    bool literalAssigned;
    std::size_t scopeCoord;

    SymbolBase() = default;
};

struct Symbol : public SymbolBase {
    std::optional<SymbolBase> parent;

    Symbol() = default;
};

struct SymbolTable {

   SymbolTable(SymbolTable & v) = delete;
   SymbolTable(SymbolTable const& v) = delete;
   SymbolTable(SymbolTable * v) = delete;
   SymbolTable(SymbolTable const* v) = delete;

   SymbolTable() : entries() {
      pushScope();
   }

   std::size_t pushScope() {
      // new scopes are appended to
      // the end of the symboltable
      //
      entries.emplace_back();
      return entries.size();
   }

   std::size_t popScope() {
      entries.pop_back();
      return entries.size();
   }

   void addEntry(std::string const& ident, Symbol s) {
      s.scopeCoord = entries.size()-1;
      entries[s.scopeCoord].insert(std::make_pair(ident, std::move(s)));
   }

   std::optional<Symbol> scopedFind(std::string const& ident) const {
      const std::size_t idx = entries.size()-1;
      auto const itr = entries[idx].find(ident);
      if(itr != std::end(entries[idx])) {
         return itr->second;
      }

      return {};
   }

   std::optional<Symbol> find(std::string const& ident) const {
      // search through scopes backwards
      //
      const int entries_size = entries.size() - 1;
      for(int i = entries_size; i > -1; i--) {
         auto entry = entries[i].find(ident);
         if( entry != std::end(entries[i]) ) {
            return entry->second;
         }
      } 

      return {};
   }

   std::optional<Symbol> find(const std::size_t idx, std::string const& ident) const {
      // search through scopes backwards
      //
      auto entry = entries[idx].find(ident);
      if( entry != std::end(entries[idx]) ) {
          return entry->second;
      } 

      return {};
   }

   std::vector< std::unordered_map<std::string, Symbol> > entries;
};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

#endif
