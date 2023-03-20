/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "hpx/symboltypes.hpp"

#include <cassert>

using namespace chplx::util;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

SymbolTable::SymbolTable() : symbolTableCount(0), symbolTableRef(), lut(), parentSymbolTableId(0) {
   symbolTableRoot =
      std::make_shared<SymbolTableNode>(
         SymbolTableNode{0, {}, {}, {}}
      );
   symbolTableRef = symbolTableRoot;
   lut.push_back(symbolTableRef);
   ++symbolTableCount;
}

std::size_t SymbolTable::pushScope() {
   // new scopes are appended to
   // the end of the symboltable
   //
   symbolTableRef->children.push_back(
      std::make_shared<SymbolTableNode>(SymbolTableNode{symbolTableCount, {}, {}, symbolTableRef})
   );
   ++symbolTableCount;

   symbolTableRef = std::get<std::shared_ptr<SymbolTableNode>>(symbolTableRef->children.back());
   lut.push_back(symbolTableRef);
   
   return symbolTableCount;
}

void SymbolTable::popScope() {
   if(symbolTableRef->parent && std::holds_alternative<std::shared_ptr<SymbolTableNode>>(*symbolTableRef->parent)) {
      symbolTableRef = std::get<std::shared_ptr<SymbolTableNode>>(*symbolTableRef->parent);
   }
}

void SymbolTable::addEntry(const std::size_t lutid, std::string const& ident, Symbol s) {
   assert( 0 <= lutid && lutid < lut.size());
   lut[lutid]->entries.insert(std::make_pair(ident, s));
}

void SymbolTable::addEntry(std::string const& ident, Symbol s) {
   symbolTableRef->entries.insert(std::make_pair(ident, s));
}

bool SymbolTable::findImpl(SymbolTableNode& stref, std::string const& ident, std::unordered_map<std::string, Symbol>::iterator & ret) {
   ret = stref.entries.find(ident);
   if(ret != std::end(stref.entries)) {
      return true;
   }

   if(!stref.parent && 0 == stref.parent->index()) {
      return false;
   }

   return findImpl(*std::get<std::shared_ptr<SymbolTableNode>>(*(stref.parent)), ident, ret);
}

std::optional<Symbol> SymbolTable::find(std::string const& ident) {
   SymbolTableNode& stref = *symbolTableRef; 
   std::unordered_map<std::string, Symbol>::iterator entry = stref.entries.find(ident);
   if(entry != std::end(stref.entries)) {
      return entry->second;
   } 

   const bool found = findImpl(stref, ident, entry);
   if(found) { return entry->second; }
   return {};
}

void SymbolTable::find(std::string const& ident, std::optional<Symbol> &s) {
   SymbolTableNode& stref = *symbolTableRef; 
   std::unordered_map<std::string, Symbol>::iterator entry = stref.entries.find(ident);
   if(entry != std::end(stref.entries)) {
      s = entry->second;
   } 

   const bool found = findImpl(stref, ident, entry);
   if(found) { s = entry->second; }
}

std::optional<Symbol> SymbolTable::find(const std::size_t idx, std::string const& ident) {
   assert(idx >= 0 && idx < lut.size());
   SymbolTableNode & stref = *lut[idx];

   if(stref.entries.size() < 1) { return {}; }
   std::unordered_map<std::string, Symbol>::iterator entry = stref.entries.find(ident);

   if(entry != std::end(stref.entries)) {
      return entry->second;
   } 

   const bool found = findImpl(stref, ident, entry);
   if(found) { return entry->second; }
   return {};
}

void SymbolTable::find(const std::size_t idx, std::string const& ident, std::optional<Symbol> & s) {
   assert(idx < lut.size());
   SymbolTableNode & stref = *lut[idx];
   std::unordered_map<std::string, Symbol>::iterator entry = stref.entries.find(ident);
   if(entry != std::end(stref.entries)) {
      s = entry->second;
   } 

   const bool found = findImpl(stref, ident, entry);
   if(found) { s = entry->second; }
}

void SymbolTable::dumpImpl(SymbolTableNode const& node) {
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

void SymbolTable::dump() {
   SymbolTableNode& stref = *symbolTableRoot;
   dumpImpl(stref);
}

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */
