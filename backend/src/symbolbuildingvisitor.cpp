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
#include <filesystem>
#include <numeric>

using namespace chpl::uast;
 
namespace chpl { namespace ast { namespace visitors { namespace hpx {

void SymbolBuildingVisitor::addSymbolEntry(char const* type, Symbol && symbol) {
   symbolTable.addEntry(*symbol.identifier, symbol);
}

SymbolBuildingVisitor::SymbolBuildingVisitor(chpl::uast::BuilderResult const& chapelBr, std::string const& cfps)
   : br(chapelBr), indent(0),
     chplFilePathStr(cfps),
     symstack(), sym(), symnode(), symbolTable()
{
   // populate the symbol table with a base set of entries:
   //
   // bool, string, int, real, byte, complex, range, domain, template(?)
   //

   addSymbolEntry("nil", Symbol{{nil_kind{}, std::string{"nil"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("bool", Symbol{{bool_kind{}, std::string{"bool"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("string", Symbol{{string_kind{}, std::string{"string"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("int", Symbol{{int_kind{}, std::string{"int"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("real", Symbol{{real_kind{}, std::string{"real"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("byte", Symbol{{byte_kind{}, std::string{"byte"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("complex", Symbol{{complex_kind{}, std::string{"complex"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("range", Symbol{{range_kind{}, std::string{"range"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("domain", Symbol{{domain_kind{}, std::string{"domain"}, {}, symbolTable.symbolTableRef->id}, {}});
   addSymbolEntry("?", Symbol{{template_kind{}, std::string{"?"}, {}, symbolTable.symbolTableRef->id}, {}});

   // inlinecxx - allows users to inline c++ code into their chapel programs
   //
   addSymbolEntry("inlinecxx",
      Symbol{{std::make_shared<cxxfunc_kind>(cxxfunc_kind{{{}, "inlinecxx", {}, nil_kind{}}}), std::string{"inlinecxx"}, {}, symbolTable.symbolTableRef->id}, {}}
   );

   // +,-,*,/,%,=
   //
   addSymbolEntry("+",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "+", {}}}), std::string{"+"}, {}, symbolTable.symbolTableRef->id}, {}}
   );
   addSymbolEntry("-",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "-", {}}}), std::string{"-"}, {}, symbolTable.symbolTableRef->id}, {}}
   );
   addSymbolEntry("*",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "*", {}}}), std::string{"*"}, {}, symbolTable.symbolTableRef->id}, {}}
   );
   addSymbolEntry("/",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "/", {}}}), std::string{"/"}, {}, symbolTable.symbolTableRef->id}, {}}
   );
   addSymbolEntry("%",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "%", {}}}), std::string{"%"}, {}, symbolTable.symbolTableRef->id}, {}}
   );
   addSymbolEntry("=",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "=", {}}}), std::string{"="}, {}, symbolTable.symbolTableRef->id}, {}}
   );
   addSymbolEntry("[]",
      Symbol{{std::make_shared<func_kind>(func_kind{{{}, "[]", {}}}), std::string{"[]"}, {}, symbolTable.symbolTableRef->id}, {}}
   );
}

std::string SymbolBuildingVisitor::emitChapelLine(uast::AstNode const* ast) {
   auto fp = br.filePath();
   std::stringstream os{};
   std::filesystem::path p(fp.c_str());
   os << "#line " << br.idToLocation(ast->id(), fp).line()  << " \"" << p.filename().string() << "\"";
   return os.str();
}

bool SymbolBuildingVisitor::enter(const uast::AstNode * ast) {
//std::cout << "node tag\t" << ast->tag() << '\t' << tagToString(ast->tag()) << ' ' << symbolTable.symbolTableRef->id << std::endl;
   switch(ast->tag()) {
    case asttags::AnonFormal:
    break;
    case asttags::As:
    break;
    case asttags::Array:
    {
        if(sym.has_value() && sym->get().kind->index() < 1) {
           sym->get().kind = std::make_shared<array_kind>();
           std::get<std::shared_ptr<array_kind>>(*sym->get().kind)->kind =
              std::make_shared<kind_node_type>(kind_node_type{});
        }
        else {
           std::get<std::shared_ptr<kind_node_type>>(
              std::get<std::shared_ptr<array_kind>>(*sym->get().kind)->kind
           )->children.emplace_back(
              std::make_shared<kind_node_type>(kind_node_type{})
           );
        }
    }
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
            if(sym->get().kind.has_value()) {
                if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->get().kind))) {
                    std::get<std::shared_ptr<array_kind>>(*(sym->get().kind))->dom = domain_kind{};
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

       if(sym && sym->get().kind.has_value()) {
          const std::size_t lutId =
             std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind)) ?
                std::get<std::shared_ptr<func_kind>>(*sym->get().kind)->lutId :
                0;
          if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->get().kind))) {
             auto fsym = symbolTable.find(lutId, identifier_str);
             if(fsym) {
                std::get<std::shared_ptr<array_kind>>( *(sym->get().kind))->kind = *(fsym->kind);
                sym->get().scopeId = symbolTable.symbolTableRef->id;
             }
          }
          else if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind)) && 
                  !std::holds_alternative<std::shared_ptr<cxxfunc_kind>>(*(sym->get().kind))) {

             std::shared_ptr<func_kind> & fk =
                std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));

             auto fsym = symbolTable.find(sym->get().scopeId, identifier_str);

             if(fsym.has_value()) {
                if(std::holds_alternative<std::shared_ptr<cxxfunc_kind>>(*fsym->kind)) {
                      return true;
                }
                else if(0 < fk->args.size()) {
                   if(std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                      return true;
                   }
                   else if(fk->args.back().kind->index() < 1) {
                     fk->args.back().kind = (*(fsym->kind));
                   }
                   else if( !std::holds_alternative<std::shared_ptr<func_kind>>(*(fsym->kind)) && 
                            !std::holds_alternative<std::shared_ptr<cxxfunc_kind>>(*(fsym->kind)) ) {
                      fk->retKind = (*(fsym->kind));
                   }
                }
                else if(0 == fk->args.size()) {
                   fk->retKind = (*(fsym->kind));
                }
             }
          }
          else {
             auto fsym = symbolTable.find(lutId, identifier_str);
             if(fsym.has_value()) {
                sym->get().kind = (*(fsym->kind));
                sym->get().scopeId = symbolTable.symbolTableRef->id;
             }
          }
       }
       else if (sym) {
          std::optional<Symbol> fsym{};
          symbolTable.find(identifier_str, fsym);
          if(fsym.has_value()) {
             sym->get().kind = (*(fsym->kind));
             sym->get().scopeId = symbolTable.symbolTableRef->id;
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
       if(sym && sym->get().kind.has_value()) {
          if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->get().kind))) {
             std::get<std::shared_ptr<array_kind>>(*(sym->get().kind))->dom->ranges.push_back(range_kind{});
          }
          else if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind)) && (sym->get().identifier->find("for") != std::string::npos)) {

             std::shared_ptr<func_kind> & fk =
                std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));

             std::string ident{std::string{"range_" + emitChapelLine(ast)}};
             fk->args.emplace_back(
                Symbol{{
                   std::make_optional<kind_types>(range_kind{}),
                   ident,
                   {}, symbolTable.symbolTableRef->id
                }}
             );

             symbolTable.addEntry(fk->lutId, ident, fk->args.back());

             sym = symstack.back();
             symnode = ast;
          }
       }
       else {
          symstack.emplace_back(Symbol{{
            std::make_optional<kind_types>(range_kind{}),
            std::string{"range_" + emitChapelLine(ast)},
            {}, symbolTable.symbolTableRef->id
          }});

          sym = symstack.back();
          symnode = ast;
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
        if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
           if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(0 < fk->args.size() && std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }

           auto fsym = symbolTable.find("bool");
           if(fsym.has_value()) {
             sym->get().kind = (*(fsym->kind));
             if(sym->get().literal) {
                sym->get().literal->push_back(ast);
             }
             else {
                sym->get().literal = {ast,};
             }
           }
        }
    }
    break;
    case asttags::ImagLiteral:
    {
        if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
           if(std::holds_alternative<std::shared_ptr<func_kind>>(*( sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(0 < fk->args.size() && std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }

           auto fsym = symbolTable.find("complex");
           if(fsym.has_value()) {
             sym->get().kind = (*(fsym->kind));
             if(sym->get().literal) {
                sym->get().literal->push_back(ast);
             }
             else {
                sym->get().literal = {ast,};
             }
           }
        }
    }
    break;
    case asttags::IntLiteral:
    {
        if(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index()) {
           if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(0 < fk->args.size() && std::holds_alternative<range_kind>(*(fk->args.back().kind))) {
                 range_kind & symref =
                    std::get<range_kind>(*(fk->args.back().kind));
                 symref.points.emplace_back( int_kind::value(ast) );
              }
              else if(0 < fk->args.size() && std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }
           else if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->get().kind))) {
              std::shared_ptr<array_kind> & symref =
                 std::get<std::shared_ptr<array_kind>>(*(sym->get().kind));

              if(!sym->get().literal && symref->dom) {
                 symref->dom->ranges.back().points.push_back( int_kind::value(ast) );
              }
              else if(!sym->get().literal) {
                 sym->get().literal = std::vector<uast::AstNode const*>{ast};
                 std::get<std::shared_ptr<kind_node_type>>(symref->kind)->children.emplace_back(
                    int_kind{}
                 );
              }
              else {
                 sym->get().literal->push_back(ast);
                 std::get<std::shared_ptr<kind_node_type>>(symref->kind)->children.emplace_back(
                    int_kind{}
                 );
              }
           }
           else if(std::holds_alternative<range_kind>(*(sym->get().kind))) {
              range_kind & symref =
                 std::get<range_kind>(*sym->get().kind);
              symref.points.push_back( int_kind::value(ast) );
           }
        }
        else if (sym) {
           auto fsym = symbolTable.find(0, "int");
           if(fsym.has_value()) {
              sym->get().kind = (*(fsym->kind));
              if(sym->get().literal) {
                 sym->get().literal->push_back(ast);
              }
              else {
                 sym->get().literal = {ast,};
              }
           }
        }
    }
    break;
    case asttags::RealLiteral:
    {
        if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
           if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }

           if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(0 < fk->args.size() && std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }

           auto fsym = symbolTable.find("real");
           if(fsym.has_value()) {
             sym->get().kind = (*(fsym->kind));
             if(sym->get().literal) {
                sym->get().literal->push_back(ast);
             }
             else {
                sym->get().literal = {ast,};
             }
           }
        }
    }
    break;
    case asttags::UintLiteral:
    {
        if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
           if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(0 < fk->args.size() && std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }

           auto fsym = symbolTable.find("int");
           if(fsym.has_value()) {
             sym->get().kind = (*(fsym->kind));
             if(sym->get().literal) {
                sym->get().literal->push_back(ast);
             }
             else {
                sym->get().literal = {ast,};
             }
           }
        }
    }
    break;
    case asttags::START_StringLikeLiteral:
    break;
    case asttags::BytesLiteral:
    {
        if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
           if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(0 < fk->args.size() && std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }

           auto fsym = symbolTable.find("byte");
           if(fsym.has_value()) {
             sym->get().kind = (*(fsym->kind));
             if(sym->get().literal) {
                sym->get().literal->push_back(ast);
             }
             else {
                sym->get().literal = {ast,};
             }
           }
        }
    }
    break;
    case asttags::CStringLiteral:
    {
        if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
           if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(0 < fk->args.size() && std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }

           auto fsym = symbolTable.find("string");
           if(fsym.has_value()) {
             sym->get().kind = (*(fsym->kind));
             if(sym->get().literal) {
                sym->get().literal->push_back(ast);
             }
             else {
                sym->get().literal = {ast,};
             }
           }
        }
    }
    break;
    case asttags::StringLiteral:
    {
        if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
           if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
              std::shared_ptr<func_kind> & fk =
                 std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
              if(0 < fk->args.size() && std::holds_alternative<nil_kind>(*(fk->args.back().kind))) {
                 return true;
              }
           }

           auto fsym = symbolTable.find("string");
           if(fsym.has_value()) {
             sym->get().kind = (*(fsym->kind));
             if(sym->get().literal) {
                sym->get().literal->push_back(ast);
             }
             else {
                sym->get().literal = {ast,};
             }
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
    {
       if(sym.has_value() && sym->get().kind.has_value() &&
          std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {

          std::shared_ptr<func_kind> & fk =
             std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));

          std::string ident{dynamic_cast<Formal const*>(ast)->name().c_str()};
          fk->args.emplace_back(
             Symbol{{
                std::make_optional<kind_types>(),
                ident,
                {}, symbolTable.symbolTableRef->id
             }}
          );

          symbolTable.addEntry(fk->lutId, ident, fk->args.back());
       }
    }
    break;
    case asttags::TaskVar:
    break;
    case asttags::VarArgFormal:
    break;
    case asttags::Variable:
    {
       sym.reset();
       symstack.emplace_back(Symbol{{
          std::make_optional<kind_types>(),
          std::string{dynamic_cast<NamedDecl const*>(ast)->name().c_str()},
          {}, symbolTable.symbolTableRef->id
       }});

       sym = symstack.back();
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
    {
       if(sym && sym->get().kind.has_value()) {
         if(std::holds_alternative<std::shared_ptr<func_kind>>(*(sym->get().kind))) {
             std::shared_ptr<func_kind> & fk =
                std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));
             fk->args.emplace_back(Symbol{{nil_kind{}, std::string{"nil"}, {}, symbolTable.symbolTableRef->id}, {}});
          }
       }
    }
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
            sym->get().kind = std::make_shared<array_kind>();
        }
    }
    break;
    case asttags::Coforall:
    break;
    case asttags::For:
    {
       if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
          // symbol.scopePtr = the scope where the function is defined (equivalent to a lutId)
          //
          symstack.emplace_back(
             Symbol{{
                std::optional<kind_types>{
                   std::make_shared<func_kind>(func_kind{{
                      symbolTable.symbolTableRef->id, {}, {}, {}}})
                },
                std::string{"for" + emitChapelLine(ast)},
                {}, symbolTable.symbolTableRef->id
             }});

          std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef = symbolTable.symbolTableRef;
          const std::size_t parScope = symbolTable.symbolTableRef->id;
          symbolTable.pushScope();
          sym = symstack.back();

          std::shared_ptr<func_kind> & fk = 
             std::get<std::shared_ptr<func_kind>>(*sym->get().kind);

          fk->symbolTableSignature = (*sym->get().identifier);
          // func_kind.lutId = the scope where the function's symboltable references
          //
          fk->lutId = symbolTable.symbolTableRef->id;

          symbolTable.parentSymbolTableId = parScope;
          symbolTable.symbolTableRef->parent = prevSymbolTableRef;

          symnode = ast;
       }
    }
    break;
    case asttags::Forall:
    {
       if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
          // symbol.scopePtr = the scope where the function is defined (equivalent to a lutId)
          //
          symstack.emplace_back(
             Symbol{{
                std::optional<kind_types>{
                   std::make_shared<func_kind>(func_kind{{
                      symbolTable.symbolTableRef->id, {}, {}, {}}})
                },
                std::string{"forall" + emitChapelLine(ast)},
                {}, symbolTable.symbolTableRef->id
             }});

          std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef = symbolTable.symbolTableRef;
          const std::size_t parScope = symbolTable.symbolTableRef->id;
          symbolTable.pushScope();
          sym = symstack.back();

          std::shared_ptr<func_kind> & fk = 
             std::get<std::shared_ptr<func_kind>>(*sym->get().kind);

          fk->symbolTableSignature = (*sym->get().identifier);
          // func_kind.lutId = the scope where the function's symboltable references
          //
          fk->lutId = symbolTable.symbolTableRef->id;

          symbolTable.parentSymbolTableId = parScope;
          symbolTable.symbolTableRef->parent = prevSymbolTableRef;

          symnode = ast;
       }
    }
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
    {
       // pattern to repeat in the symbol table builder
       //
       struct ProgramTreeFunctionVisitor {
          bool enter(uast::AstNode const* ast) {
             const auto tag = ast->tag();
             if(tag == asttags::Function && !complete) {
                lookup += std::string{dynamic_cast<Function const*>(ast)->name().c_str()};
             }
             else if(tag == asttags::Identifier && !complete) {
                lookup += "|" + std::string{dynamic_cast<Identifier const*>(ast)->name().c_str()};
             }
             else if(tag == asttags::Block && !complete) {
                complete = true;
             }
             return true;
          }

          void exit(uast::AstNode const* ast) {
          }

          bool complete;
          std::string lookup;
       };

       if(!(sym && sym->get().kind.has_value() && 0 < sym->get().kind->index())) {
          ProgramTreeFunctionVisitor v{false, {}};
          ast->traverse(v);

          // symbol.scopePtr = the scope where the function is defined (equivalent to a lutId)
          //
          symstack.emplace_back(
             Symbol{{
                std::optional<kind_types>{
                   std::make_shared<func_kind>(func_kind{{
                      symbolTable.symbolTableRef->id, {}, {}, {}}})
                },
                v.lookup,
                {}, symbolTable.symbolTableRef->id
             }});

          std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef = symbolTable.symbolTableRef;
          const std::size_t parScope = symbolTable.symbolTableRef->id;
          symbolTable.pushScope();
          sym = symstack.back();

          std::shared_ptr<func_kind> & fk = 
             std::get<std::shared_ptr<func_kind>>(*sym->get().kind);

          fk->symbolTableSignature = v.lookup;
          // func_kind.lutId = the scope where the function's symboltable references
          //
          fk->lutId = symbolTable.symbolTableRef->id;

          symbolTable.parentSymbolTableId = parScope;
          symbolTable.symbolTableRef->parent = prevSymbolTableRef;

          symnode = ast;
       }
    }
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
//std::cout << "exit\t" << ast->tag() << '\t' << tagToString(ast->tag()) << std::endl;
   switch(ast->tag()) {
    case asttags::AnonFormal:
    break;
    case asttags::As:
    break;
    case asttags::Array:
    {
        if(sym.has_value()) {
           std::get<std::shared_ptr<kind_node_type>>(
              std::get<std::shared_ptr<array_kind>>(*sym->get().kind)->kind
           )->children.emplace_back(
              kind_node_term_type{}
           );
        }
    }
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
    {
        if(sym && std::holds_alternative<range_kind>(*(sym->get().kind))) {
           symbolTable.addEntry((*(sym->get().identifier)), *sym);
           sym.reset();
           symnode.reset();
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
            assert( sym->get().identifier.has_value() );
            const std::size_t lutId = sym->get().scopeId;

            auto lusym = symbolTable.find(lutId, (*(sym->get().identifier)));
            if(!lusym) {
                // segmentation fault when used
                //
                symbolTable.addEntry(lutId, (*(sym->get().identifier)), *sym);
                
                sym.reset();
                symnode.reset();
                assert(0 < symstack.size());
                symstack.pop_back();
                if (!symstack.empty())
                  sym = symstack.back();
            }
            else {
                std::cerr << "chplx : " << (*(sym->get().identifier)) << " identifier already defined in current scope" << std::endl;
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
    case asttags::Forall:
    {
       if(sym) {
          assert( sym->get().identifier.has_value() );

          auto lusym = symbolTable.find(sym->get().scopeId, (*(sym->get().identifier)));
          if(!lusym) {
             std::shared_ptr<func_kind> & fk =
                std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));

             if(!fk->retKind) {
                fk->retKind = nil_kind{};
             }
             else if(fk->retKind->index() < 1) {
                fk->retKind = nil_kind{};
             }

             symbolTable.addEntry(sym->get().scopeId, (*(fk->symbolTableSignature)), *sym);
             sym.reset();
             symnode.reset();

             if(0 < symstack.size()) {
               symstack.pop_back();
             }
             if(1 < symstack.size()) {
               sym = symstack.back();
             }

             symbolTable.popScope();
          }
          else {
             std::shared_ptr<func_kind> & fk =
                std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));

             std::cerr << "chplx : " << (*(fk->symbolTableSignature)) << " identifier already defined in current scope" << std::endl;
             return;
          }
       }
    }
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
    {
       if(sym) {
          assert( sym->get().identifier.has_value() );
          auto lusym = symbolTable.find(sym->get().scopeId, (*(sym->get().identifier)));
          if(!lusym) {
             std::shared_ptr<func_kind> & fk =
                std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));

             if(!fk->retKind) {
                fk->retKind = nil_kind{};
             }
             else if(fk->retKind->index() < 1) {
                fk->retKind = nil_kind{};
             }

             symbolTable.addEntry(sym->get().scopeId, (*(fk->symbolTableSignature)), *sym);
             sym.reset();
             symnode.reset();

             if(0 < symstack.size()) {
               symstack.pop_back();
             }
             if(1 < symstack.size()) {
               sym = symstack.back();
             }

             symbolTable.popScope();
          }
          else {
             std::shared_ptr<func_kind> & fk =
                std::get<std::shared_ptr<func_kind>>(*(sym->get().kind));

             std::cerr << "chplx : " << (*(fk->symbolTableSignature)) << " identifier already defined in current scope" << std::endl;
             return;
          }
       }
    }
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
