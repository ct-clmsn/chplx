/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "chpl/uast/AstNode.h"
#include "chpl/uast/Module.h"

#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace chpl { namespace ast { namespace visitors { namespace hpx {

// support for chapel's '?' type
//
struct undefined_kind {};

// support for chapel's template/generics "t : ?" types
//
struct template_kind {
   std::string identifier;

   template_kind() = default;
};

struct byte_kind {};
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
   undefined_kind,
   template_kind,
   byte_kind,
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

   Visitor(std::string chapel_file_path_str, std::ostream & fstrm)
      : indent(0), fstrm_(fstrm), chpl_file_path_str(std::move(chapel_file_path_str)), sym(), symnode(), symboltable(), headers(static_cast<std::size_t>(HeaderEnum::HeaderCount), false) {

      Symbol strsym{};
      strsym.kind = string_kind{};
      strsym.identifier = "string";
      symboltable.addEntry(*(strsym.identifier), strsym);

      Symbol intsym{};
      intsym.kind = int_kind{};
      intsym.identifier = "int";
      symboltable.addEntry(*(intsym.identifier), intsym);

      Symbol realsym{};
      realsym.kind = real_kind{};
      realsym.identifier = "real";
      symboltable.addEntry(*(realsym.identifier), realsym);

      Symbol bytesym{};
      bytesym.kind = byte_kind{};
      bytesym.identifier = "byte";
      symboltable.addEntry(*(bytesym.identifier), bytesym);

      Symbol cmplxsym{};
      cmplxsym.kind = complex_kind{};
      cmplxsym.identifier = "complex";
      symboltable.addEntry(*(cmplxsym.identifier), cmplxsym);

      Symbol rngsym{};
      rngsym.kind = range_kind{};
      rngsym.identifier = "range";
      symboltable.addEntry(*(rngsym.identifier), rngsym);

      Symbol domsym{};
      domsym.kind = domain_kind{};
      domsym.identifier = "domain";
      symboltable.addEntry(*(domsym.identifier), domsym);

      Symbol unksym{};
      unksym.kind = undefined_kind{};
      unksym.identifier = "?";
      symboltable.addEntry(*(unksym.identifier), unksym);
   }

   bool enter(const uast::AstNode * node);
   void exit(const uast::AstNode * node);

   void generateSourceHeader();
   void generateSourceFooter();
   void generate_hpx_main_beg();
   void generate_hpx_main_end();
   void generateApplicationHeader();

   std::size_t indent; 
   std::ostream & fstrm_;
   std::string chpl_file_path_str;
   std::optional<Symbol> sym;
   std::optional<uast::AstNode const*> symnode;
   SymbolTable symboltable;
   std::vector<bool> headers;
};


} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */
