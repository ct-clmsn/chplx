/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "chpl/uast/AstNode.h"
#include "chpl/uast/Module.h"

#include <optional>
#include <ostream>
#include <optional>
#include <variant>
#include <string>
#include <vector>
#include <unordered_map>

namespace chpl { namespace ast { namespace visitors { namespace hpx {

struct byte_kind {};
struct int_kind {};
struct real_kind {};
struct complex_kind {};
struct string_kind {};

struct range_kind {
   std::vector<std::int64_t> points;

   range_kind() : points() {}
};

struct domain_kind {
   std::vector<range_kind> ranges;

   domain_kind() : ranges() {};
};

using simple_kinds = std::variant<
   std::monostate,
   byte_kind,
   int_kind,
   real_kind,
   complex_kind,
   string_kind,
   range_kind,
   domain_kind
>;

struct func_base_kind {};
struct record_base_kind {};
struct class_base_kind {};

using complex_kinds = std::variant<
   std::monostate,
   func_base_kind,
   record_base_kind,
   class_base_kind
>;

using kind_instance_types = std::variant<
   std::monostate,
   simple_kinds,
   complex_kinds
>;

struct array_kind {
   kind_instance_types kind;
   domain_kind dom;   

   array_kind() : kind(), dom() {}
};

using kind_types = std::variant<
   std::monostate,
   kind_instance_types,
   array_kind
>;

struct func_kind : public func_base_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
};

struct record_kind : public record_base_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
};

struct class_kind : public class_base_kind {
   std::vector<std::string> identifiers;
   std::vector<kind_types> kinds;
};

struct SymbolBase {
    std::optional<kind_types> kind;
    std::optional<std::string> identifier;

    SymbolBase() : kind(), identifier() {}
};

struct Symbol : public SymbolBase {
    std::optional<SymbolBase> parent;

    Symbol() : SymbolBase(), parent() {}
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
      entries.push_back( std::unordered_map<std::string, Symbol>{} );
      return entries.size();
   }

   std::size_t popScope() {
      entries.pop_back();
      return entries.size();
   }

   void addEntry(std::string & entry_str, Symbol & s) {
      entries.back().insert({entry_str, s});
   }

   void addEntry(std::string & entry_str, Symbol && s) {
      entries.back().insert({entry_str, s});
   }

   std::optional<Symbol> find(std::string const& entry_str) const {
      using itr_t = std::vector<std::unordered_map<std::string, Symbol>>::const_iterator;
      const itr_t itr_beg = std::begin(entries);

      // search through scopes backwards
      //
      for(itr_t i = std::end(entries); i != itr_beg; --i) {
         auto entry = i->find(entry_str);
         if( entry != std::end(*i) ) {
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

   Visitor(std::string const& chapel_file_path_str, std::ostream & fstrm)
      : indent(0), fstrm_(fstrm), chpl_file_path_str(chapel_file_path_str), sym(), symnode(), symboltable(), headers(static_cast<std::size_t>(HeaderEnum::HeaderCount), false) {
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
