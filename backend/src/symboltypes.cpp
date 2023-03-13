/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "hpx/symboltypes.hpp"

using namespace chplx::util;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

SymbolTable::SymbolTable() : symbolTableCount(0), symbolTableRef(), lut(), parentSymbolTableId(0) {
   symbolTableRoot =
      std::make_shared<SymbolTableNode>(
         std::move(SymbolTableNode{0, {}, {}, {}})
      );
   symbolTableRef = symbolTableRoot;
   lut.push_back(symbolTableRef);
   ++symbolTableCount;
}

std::size_t SymbolTable::pushScope() {
   // new scopes are appended to
   // the end of the symboltable
   //
   symbolTableRef->children.emplace_back(
      std::make_shared<SymbolTableNode>(std::move(SymbolTableNode{symbolTableCount, {}, {}, symbolTableRef}))
   );
   symbolTableCount+=1;

   symbolTableRef = std::get<std::shared_ptr<SymbolTableNode>>(symbolTableRef->children.back());
   lut.push_back(symbolTableRef);
   
   return symbolTableCount;
}

void SymbolTable::popScope() {
   if(symbolTableRef->parent && std::holds_alternative<std::shared_ptr<SymbolTableNode>>(*symbolTableRef->parent)) {
      symbolTableRef = std::get<std::shared_ptr<SymbolTableNode>>(*symbolTableRef->parent);
   }
}

void SymbolTable::addEntry(std::string const& ident, Symbol s) {
   symbolTableRef->entries.insert(std::make_pair(ident, std::move(s)));
}

std::optional<Symbol> SymbolTable::findImpl(SymbolTableNode& stref, std::string const& ident) {
   auto entry = stref.entries.find(ident);
   if(entry != std::end(stref.entries)) {
      return entry->second;
   }

   if(!stref.parent) {
      return {};
   }

   return findImpl(*std::get<std::shared_ptr<SymbolTableNode>>(*stref.parent), ident);
}

std::optional<Symbol> SymbolTable::find(std::string const& ident) {
   SymbolTableNode& stref = *symbolTableRef; 
   return findImpl(stref, ident);
}

std::optional<Symbol> SymbolTable::findImpl(SymbolTableNode& stref, const std::size_t idx, std::string const& ident) {
   if(stref.entries.size() < 1) {
      return {};
   }

   if(idx == stref.id) {
      auto entry = stref.entries.find(ident);
      if(entry != std::end(stref.entries)) {
         return {entry->second};
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

std::optional<Symbol> SymbolTable::find(const std::size_t idx, std::string const& ident) {
   SymbolTableNode & stref = *lut[idx];
   return findImpl(stref, idx, ident);
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
