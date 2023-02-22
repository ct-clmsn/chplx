/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "chpl/uast/AstNode.h"
#include "chpl/uast/Module.h"
#include "symbols.hpp"

#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>

namespace chpl { namespace ast { namespace visitors { namespace hpx {

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
      entries.push_back( std::unordered_map<std::string, chpl::backend::symbols::Symbol>{} );
      return entries.size();
   }

   std::size_t popScope() {
      entries.pop_back();
      return entries.size();
   }

   void addEntry(std::string & entry_str, chpl::backend::symbols::Symbol & s) {
      entries.back().insert({entry_str, s});
   }

   std::optional<chpl::backend::symbols::Symbol> find(std::string const& entry_str) const {
      using itr_t = std::vector<std::unordered_map<std::string, chpl::backend::symbols::Symbol>>::const_iterator;
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

   std::vector< std::unordered_map<std::string, chpl::backend::symbols::Symbol> > entries;
};

struct Visitor {

   Visitor() = delete;
   Visitor(Visitor & v) = delete;
   Visitor(Visitor const& v) = delete;
   Visitor(Visitor * v) = delete;
   Visitor(Visitor const* v) = delete;

   Visitor(std::ostream & fstrm)
      : symboltable(), fstrm_(fstrm) {
   }

   bool enter(const uast::AstNode * node);
   void exit(const uast::AstNode * node);

   SymbolTable symboltable;
   std::ostream & fstrm_;

};

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */
