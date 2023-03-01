/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "hpx/symbolbuildingvisitor.hpp"
#include "chpl/uast/all-uast.h"

#include <variant>
#include <fstream>
#include <cctype>
#include <numeric>

using namespace chpl::uast;
 
namespace chpl { namespace ast { namespace visitors { namespace hpx {

template <typename Kind>
void SymbolBuildingVisitor::addSymbolEntry(char const* type) {
   Symbol sym{{Kind{}, std::string{type}, false, symbolTable.symbolTableCount}, {}};
   symbolTable.addEntry(*sym.identifier, sym);
}

SymbolBuildingVisitor::SymbolBuildingVisitor(chpl::uast::BuilderResult const& chapelBr, std::string const& cfps, std::ostream & fstrm)
   : br(chapelBr), indent(0), fstrm_(fstrm),
     chplFilePathStr(cfps),
     sym(), symnode(), symbolTable()
{
   // populate the symbol table with a base set of entries:
   //
   // bool, string, int, real, byte, complex, range, domain, template
   //
   addSymbolEntry<bool_kind>("bool");
   addSymbolEntry<string_kind>("string");
   addSymbolEntry<int_kind>("int");
   addSymbolEntry<real_kind>("real");
   addSymbolEntry<byte_kind>("byte");
   addSymbolEntry<complex_kind>("complex");
   addSymbolEntry<range_kind>("range");
   addSymbolEntry<domain_kind>("domain");
   addSymbolEntry<template_kind>("?");
}

bool SymbolBuildingVisitor::enter(const uast::AstNode * ast) {
//std::cout << "node tag\t" << ast->tag() << '\t' << tagToString(ast->tag()) << std::endl;
   switch(ast->tag()) {
    case asttags::AnonFormal:
    break;
    case asttags::As:
    break;
    case asttags::Array:
    break;
    case asttags::Attributes:
    break;
    case asttags::Break:
    break;
    case asttags::Comment:
    break;
    case asttags::Continue:
    break;
    case asttags::Delete:
    break;
    case asttags::Domain:
    {
        if(sym.has_value()) {
            if(sym->kind.has_value()) {
                if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->kind))) {
                    std::get<std::shared_ptr<array_kind>>(*(sym->kind))->dom = domain_kind{};
                }
            }
        }
    }
    break;
    case asttags::Dot:
    break;
    case asttags::EmptyStmt:
    break;
    case asttags::ErroneousExpression:
    break;
    case asttags::ExternBlock:
    break;
    case asttags::FunctionSignature:
    break;
    case asttags::Identifier:
    {
       std::string identifier_str{dynamic_cast<Identifier const*>(ast)->name().c_str()};

       if(sym && sym->kind.has_value()) {
          if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->kind))) {
             auto fsym = symbolTable.find(identifier_str);
             if(fsym) {
                std::get<std::shared_ptr<array_kind>>( *(sym->kind))->kind = *(fsym->kind);
             }
          }
       }
       else {
          auto fsym = symbolTable.find(identifier_str);
          if(fsym.has_value()) {
             sym->kind = (*(fsym->kind));
          }
       }

    }
    break;
    case asttags::Import:
    break;
    case asttags::Include:
    break;
    case asttags::Let:
    break;
    case asttags::New:
    break;
    case asttags::Range:
    {
        if(sym && sym->kind.has_value()) {
           if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->kind))) {
              std::get<std::shared_ptr<array_kind>>(*(sym->kind))->dom.ranges.push_back(range_kind{});
           }
        }
    }
    break;
    case asttags::Require:
    break;
    case asttags::Return:
    break;
    case asttags::Throw:
    break;
    case asttags::TypeQuery:
    break;
    case asttags::Use:
    break;
    case asttags::VisibilityClause:
    break;
    case asttags::WithClause:
    break;
    case asttags::Yield:
    break;
    case asttags::START_Literal:
    break;
    case asttags::BoolLiteral:
    {
        if(sym) {
           auto fsym = symbolTable.find("bool");
           if(fsym.has_value()) {
             sym->kind = (*(fsym->kind));
             sym->literalAssigned = true;
           }
        }
    }
    break;
    case asttags::ImagLiteral:
    {
        if(sym) {
           auto fsym = symbolTable.find("complex");
           if(fsym.has_value()) {
             sym->kind = (*(fsym->kind));
             sym->literalAssigned = true;
           }
        }
    }
    break;
    case asttags::IntLiteral:
    {
        if(sym && sym->kind.has_value()) {
           if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->kind))) {
              std::shared_ptr<array_kind> & symref = std::get<std::shared_ptr<array_kind>>(*(sym->kind));
              symref->dom.ranges[symref->dom.ranges.size()-1].points.push_back( dynamic_cast<IntLiteral const*>(ast)->value() );
           }
        }
        else {
           auto fsym = symbolTable.find("int");
           if(fsym.has_value()) {
              sym->kind = (*(fsym->kind));
             sym->literalAssigned = true;
           }
        }
    }
    break;
    case asttags::RealLiteral:
    {
        if(sym) {
           auto fsym = symbolTable.find("real");
           if(fsym.has_value()) {
             sym->kind = (*(fsym->kind));
             sym->literalAssigned = true;
           }
        }
    }
    break;
    case asttags::UintLiteral:
    {
        if(sym) {
           auto fsym = symbolTable.find("int");
           if(fsym.has_value()) {
             sym->kind = (*(fsym->kind));
             sym->literalAssigned = true;
           }
        }
    }
    break;
    case asttags::START_StringLikeLiteral:
    break;
    case asttags::BytesLiteral:
    {
        if(sym) {
           auto fsym = symbolTable.find("byte");
           if(fsym.has_value()) {
             sym->kind = (*(fsym->kind));
             sym->literalAssigned = true;
           }
        }
    }
    break;
    case asttags::CStringLiteral:
    {
        if(sym) {
           auto fsym = symbolTable.find("string");
           if(fsym.has_value()) {
             sym->kind = (*(fsym->kind));
             sym->literalAssigned = true;
           }
        }
    }
    break;
    case asttags::StringLiteral:
    {
        if(sym) {
           auto fsym = symbolTable.find("string");
           if(fsym.has_value()) {
             sym->kind = (*(fsym->kind));
             sym->literalAssigned = true;
           }
        }
    }
    break;
    case asttags::END_StringLikeLiteral:
    break;
    case asttags::END_Literal:
    break;
    case asttags::START_Call:
    break;
    case asttags::FnCall:
    break;
    case asttags::OpCall:
    break;
    case asttags::PrimCall:
    break;
    case asttags::Reduce:
    break;
    case asttags::ReduceIntent:
    break;
    case asttags::Scan:
    break;
    case asttags::Tuple:
    break;
    case asttags::Zip:
    break;
    case asttags::END_Call:
    break;
    case asttags::MultiDecl:
    break;
    case asttags::TupleDecl:
    break;
    case asttags::ForwardingDecl:
    break;
    case asttags::EnumElement:
    break;
    case asttags::START_VarLikeDecl:
    break;
    case asttags::Formal:
    break;
    case asttags::TaskVar:
    break;
    case asttags::VarArgFormal:
    break;
    case asttags::Variable:
    {
//std::cout << "variable\tname?\t" << reinterpret_cast<NamedDecl const*>(ast)->name().c_str() << "\tis_init?\t" << (reinterpret_cast<Variable const*>(ast)->initExpression() == nullptr) << "\tis_type?\t" << (reinterpret_cast<Variable const*>(ast)->typeExpression() == nullptr) << std::endl;
       //const std::string prefix = std::string{ast->id().symbolPath().c_str()};
       //const std::string name = std::string{dynamic_cast<NamedDecl const*>(ast)->name().c_str()};
       Symbol var{};
       //var.identifier = std::string{prefix + "." + name};
       var.identifier = std::string{dynamic_cast<NamedDecl const*>(ast)->name().c_str()};
       sym = std::move(var);
       symnode = ast;
    }
    break;
    case asttags::END_VarLikeDecl:
    break;
    case asttags::Enum:
    break;
    case asttags::Catch:
    break;
    case asttags::Cobegin:
    break;
    case asttags::Conditional:
    break;
    case asttags::Implements:
    break;
    case asttags::Label: // contains a loop
    break;
    case asttags::Select:
    break;
    case asttags::Sync:
    break;
    case asttags::Try:
    break;
    case asttags::START_SimpleBlockLike:
    break;
    case asttags::Begin:
    break;
    case asttags::Block:
    break;
    case asttags::Defer:
    break;
    case asttags::Local:
    break;
    case asttags::Manage:
    break;
    case asttags::On:
    break;
    case asttags::Serial:
    break;
    case asttags::When:
    break;
    case asttags::END_SimpleBlockLike:
    break;
    case asttags::START_Loop:
    break;
    case asttags::DoWhile:
    break;
    case asttags::While:
    break;
    case asttags::START_IndexableLoop:
    break;
    case asttags::BracketLoop:
    {
        if(sym.has_value()) {
            sym->kind = std::make_shared<array_kind>();
        }
    }
    break;
    case asttags::Coforall:
    break;
    case asttags::For:
    break;
    case asttags::Forall:
    break;
    case asttags::Foreach:
    break;
    case asttags::END_IndexableLoop:
    break;
    case asttags::END_Loop:
    break;
    case asttags::START_Decl:
    break;
    case asttags::START_NamedDecl:
    break;
    case asttags::START_TypeDecl:
    break;
    case asttags::Function:
    break;
    case asttags::Interface:
    break;
    case asttags::Module:
    break;
    case asttags::START_AggregateDecl:
    break;
    case asttags::Class:
    break;
    case asttags::Record:
    break;
    case asttags::Union:
    break;
    case asttags::END_AggregateDecl:
    break;
    case asttags::END_Decl:
    break;
    case asttags::END_NamedDecl:
    break;
    case asttags::END_TypeDecl:
    break;
    case asttags::NUM_AST_TAGS:
    break;
    case asttags::AST_TAG_UNKNOWN:
    break;
    default:
    break;
   }

   return true;
}

void SymbolBuildingVisitor::exit(const uast::AstNode * ast) {
//std::cout << "exit\t" << ast->tag() << std::endl;
   switch(ast->tag()) {
    case asttags::AnonFormal:
    break;
    case asttags::As:
    break;
    case asttags::Array:
    break;
    case asttags::Attributes:
    break;
    case asttags::Break:
    break;
    case asttags::Comment:
    break;
    case asttags::Continue:
    break;
    case asttags::Delete:
    break;
    case asttags::Domain:
    break;
    case asttags::Dot:
    break;
    case asttags::EmptyStmt:
    break;
    case asttags::ErroneousExpression:
    break;
    case asttags::ExternBlock:
    break;
    case asttags::FunctionSignature:
    break;
    case asttags::Identifier:
    break;
    case asttags::Import:
    break;
    case asttags::Include:
    break;
    case asttags::Let:
    break;
    case asttags::New:
    break;
    case asttags::Range:
    break;
    case asttags::Require:
    break;
    case asttags::Return:
    break;
    case asttags::Throw:
    break;
    case asttags::TypeQuery:
    break;
    case asttags::Use:
    break;
    case asttags::VisibilityClause:
    break;
    case asttags::WithClause:
    break;
    case asttags::Yield:
    break;
    case asttags::START_Literal:
    break;
    case asttags::BoolLiteral:
    break;
    case asttags::ImagLiteral:
    break;
    case asttags::IntLiteral:
    break;
    case asttags::RealLiteral:
    break;
    case asttags::UintLiteral:
    break;
    case asttags::BytesLiteral:
    break;
    case asttags::CStringLiteral:
    break;
    case asttags::StringLiteral:
    break;
    case asttags::START_StringLikeLiteral:
    break;
    case asttags::END_StringLikeLiteral:
    break;
    case asttags::END_Literal:
    break;
    case asttags::START_Call:
    break;
    case asttags::FnCall:
    break;
    case asttags::OpCall:
    break;
    case asttags::PrimCall:
    break;
    case asttags::Reduce:
    break;
    case asttags::ReduceIntent:
    break;
    case asttags::Scan:
    break;
    case asttags::Tuple:
    break;
    case asttags::Zip:
    break;
    case asttags::END_Call:
    break;
    case asttags::MultiDecl:
    break;
    case asttags::TupleDecl:
    break;
    case asttags::ForwardingDecl:
    break;
    case asttags::EnumElement:
    break;
    case asttags::START_VarLikeDecl:
    break;
    case asttags::Formal:
    break;
    case asttags::TaskVar:
    break;
    case asttags::VarArgFormal:
    break;
    case asttags::Variable:
    {
        if(sym) {
            assert( sym->identifier.has_value() );

            auto lusym = symbolTable.find((*(sym->identifier)));
            if(!lusym) {
                symbolTable.addEntry((*(sym->identifier)), *sym);
                sym.reset();
                symnode.reset();
            }
            else {
                std::cerr << "chplx : " << (*(sym->identifier)) << " identifier already defined in current scope" << std::endl;
                return;
            }
        }
    }
    break;
    case asttags::END_VarLikeDecl:
    break;
    case asttags::Enum:
    break;
    case asttags::Catch:
    break;
    case asttags::Cobegin:
    break;
    case asttags::Conditional:
    break;
    case asttags::Implements:
    break;
    case asttags::Label: // contains a loop
    break;
    case asttags::Select:
    break;
    case asttags::Sync:
    break;
    case asttags::Try:
    break;
    case asttags::START_SimpleBlockLike:
    break;
    case asttags::Begin:
    break;
    case asttags::Block:
    break;
    case asttags::Defer:
    break;
    case asttags::Local:
    break;
    case asttags::Manage:
    break;
    case asttags::On:
    break;
    case asttags::Serial:
    break;
    case asttags::When:
    break;
    case asttags::END_SimpleBlockLike:
    break;
    case asttags::START_Loop:
    break;
    case asttags::DoWhile:
    break;
    case asttags::While:
    break;
    case asttags::START_IndexableLoop:
    break;
    case asttags::BracketLoop:
    break;
    case asttags::Coforall:
    break;
    case asttags::For:
    break;
    case asttags::Forall:
    break;
    case asttags::Foreach:
    break;
    case asttags::END_IndexableLoop:
    break;
    case asttags::END_Loop:
    break;
    case asttags::START_Decl:
    break;
    case asttags::START_NamedDecl:
    break;
    case asttags::START_TypeDecl:
    break;
    case asttags::Function:
    break;
    case asttags::Interface:
    break;
    case asttags::Module:
    break;
    case asttags::START_AggregateDecl:
    break;
    case asttags::Class:
    break;
    case asttags::Record:
    break;
    case asttags::Union:
    break;
    case asttags::END_AggregateDecl:
    break;
    case asttags::END_Decl:
    break;
    case asttags::END_NamedDecl:
    break;
    case asttags::END_TypeDecl:
    break;
    case asttags::NUM_AST_TAGS:
    break;
    case asttags::AST_TAG_UNKNOWN:
    break;
    default:
    break;
   }
}

} /* namespace hpx */ } /* namespace visitors */ } /* namespace ast */ } /* namespace chpl */
