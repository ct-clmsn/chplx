/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#ifndef __CHPLX_VISITOR_HPP__
#define __CHPLX_VISITOR_HPP__

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

   void addEntry(std::string const& entry_str, Symbol s) {
      entries[entries.size()-1].insert(std::make_pair(entry_str, s));
   }

   std::optional<Symbol> scopedFind(std::string const& entry_str) const {
      const std::size_t idx = entries.size()-1;
      auto const itr = entries[idx].find(entry_str);
      if(itr != std::end(entries[idx])) {
         return itr->second;
      }

      return {};
   }

   std::optional<Symbol> find(std::string const& entry_str) const {
      // search through scopes backwards
      //
      const int entries_size = entries.size() - 1;
      for(int i = entries_size; i > -1; i--) {
         auto entry = entries[i].find(entry_str);
         if( entry != std::end(entries[i]) ) {
            return entry->second;
         }
      } 

      return {};
   }

   std::vector< std::unordered_map<std::string, Symbol> > entries;
};

enum class HeaderEnum {
    std_vector = 0,
    std_string,
    std_complex,
    std_unordered_map,
    std_optional,
    std_variant,
    std_algorithm,
    std_numeric,
    hpx_lco_channel,
    hpx_lco_future,
    hpx_algorithm,
    hpx_numeric,
    hpx_components,
    hpx_partitioned_vector,
    hpx_unordered_map,
    hpx_spmd,
    HeaderCount
};

struct Visitor {

   Visitor() = delete;
   Visitor(Visitor & v) = delete;
   Visitor(Visitor const& v) = delete;
   Visitor(Visitor * v) = delete;
   Visitor(Visitor const* v) = delete;

   Visitor(chpl::uast::BuilderResult const& chapel_br, std::string const& chapel_file_path_str, std::ostream & fstrm);

   bool enter(const uast::AstNode * node);
   void exit(const uast::AstNode * node);

   void generateSourceHeader();
   void generateSourceFooter();
   void generate_hpx_main_beg();
   void generate_hpx_main_end();
   void generateApplicationHeader();

   void emitIndent();
   void emitArrayKind(uast::AstNode const* ast, std::shared_ptr<array_kind> & sym);
   void emitArrayKindLit(uast::AstNode const* ast, std::shared_ptr<array_kind> & sym);
   void emitByteKind(uast::AstNode const* ast, byte_kind & sym);
   void emitByteKindLit(uast::AstNode const* ast, byte_kind & sym);
   void emitIntKind(uast::AstNode const* ast, int_kind & sym);
   void emitIntKindLit(uast::AstNode const* ast, int_kind & symref);
   void emitRealKind(uast::AstNode const* ast, real_kind & sym);
   void emitRealKindLit(uast::AstNode const* ast, real_kind & sym);
   void emitComplexKind(uast::AstNode const* ast, complex_kind & sym);
   void emitComplexKindLit(uast::AstNode const* ast, complex_kind & sym);
   void emitStringKind(uast::AstNode const* ast, string_kind & sym);
   void emitStringKindLit(uast::AstNode const* ast, string_kind & sym);
   void emitBoolKind(uast::AstNode const* ast, bool_kind & sym);
   void emitBoolKindLit(uast::AstNode const* ast, bool_kind & sym);

   chpl::uast::BuilderResult const& br;
   std::size_t indent; 
   std::ostream & fstrm_;
   std::string chpl_file_path_str;
   std::optional<Symbol> sym;
   std::optional<uast::AstNode const*> symnode;
   SymbolTable symboltable;
   std::vector<bool> headers;
};


} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */

#endif
