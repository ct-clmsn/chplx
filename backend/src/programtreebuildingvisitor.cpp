/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "hpx/programtree.hpp"
#include "hpx/programtreebuildingvisitor.hpp"
#include "hpx/util.hpp"
#include "chpl/uast/all-uast.h"

#include <variant>
#include <fstream>
#include <cctype>
#include <numeric>
#include <sstream>
#include <string>
#include <filesystem>

using namespace chplx::ast::hpx;
using namespace chpl::uast;
using namespace chpl::ast::visitors::hpx;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chplx { namespace ast { namespace visitors { namespace hpx {

std::unordered_map<std::string, int> ProgramTreeBuildingVisitor::operatorEncoder = {
    {"=",  0},
    {"+",  1},
    {"-",  2},
    {"*",  3},
    {"/",  4},
    {"%",  5},
    {"[]", 6},
    {"==", 7},
    {"<=>",8}
};

struct VariableVisitor {

   const std::size_t scopePtr;
   std::string identifier;
   Symbol & sym;
   std::vector<Statement> & curStmts;
   chpl::uast::BuilderResult const& br;
   uast::AstNode const* ast;

   std::string emitChapelLine(uast::AstNode const* ast) const {
      auto const fp = br.filePath();
      return chplx::util::emitLineDirective(fp.c_str(), br.idToLocation(ast->id(), fp).line());
   }

   template<typename T>
   void operator()(T const& t) {
   }

   void operator()(byte_kind const&) {
      curStmts.push_back(ScalarDeclarationExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}});
   }
   void operator()(bool_kind const&) {
      curStmts.push_back(ScalarDeclarationExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}});
   }
   void operator()(int_kind const&) {
      curStmts.push_back(ScalarDeclarationExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}});
   }
   void operator()(real_kind const&) {
      curStmts.push_back(ScalarDeclarationExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}});
   }
   void operator()(complex_kind const&) {
      curStmts.push_back(ScalarDeclarationExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}});
   }
   void operator()(string_kind const&) {
      curStmts.push_back(ScalarDeclarationExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}});
   }
   void operator()(template_kind const&) {
   }
   void operator()(range_kind const&) {
   }
   void operator()(domain_kind const&) {
   }
   void operator()(std::shared_ptr<func_kind> const&) {
   }
   void operator()(std::shared_ptr<record_kind> const&) {
   }
   void operator()(std::shared_ptr<class_kind> const&) {
   }
   void operator()(std::shared_ptr<array_kind> const& t) {
      curStmts.push_back(ArrayDeclarationExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}});
   }
   void operator()(std::shared_ptr<associative_kind> const&) {
   }
};

struct VariableLiteralVisitor {

   const std::size_t scopePtr;
   std::string identifier;
   Symbol & sym;
   std::vector<Statement> & curStmts;
   chpl::uast::BuilderResult const& br;
   uast::AstNode const* ast;

   std::string emitChapelLine(uast::AstNode const* ast) const {
      auto const fp = br.filePath();
      return chplx::util::emitLineDirective(fp.c_str(), br.idToLocation(ast->id(), fp).line());
   }

  template<typename T>
   void operator()(T const& t) {
   }

   void operator()(byte_kind const&) {
      curStmts.push_back(ScalarDeclarationLiteralExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}, *sym.literal});
   }
   void operator()(bool_kind const&) {
      curStmts.push_back(ScalarDeclarationLiteralExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}, *sym.literal});
   }
   void operator()(int_kind const&) {
      curStmts.push_back(ScalarDeclarationLiteralExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}, *sym.literal});
   }
   void operator()(real_kind const&) {
      curStmts.push_back(ScalarDeclarationLiteralExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}, *sym.literal});
   }
   void operator()(complex_kind const&) {
      curStmts.push_back(ScalarDeclarationLiteralExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}, *sym.literal});
   }
   void operator()(string_kind const&) {
      curStmts.push_back(ScalarDeclarationLiteralExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}, *sym.literal});
   }
   void operator()(template_kind const&) {
   }
   void operator()(range_kind const&) {
   }
   void operator()(domain_kind const&) {
   }
   void operator()(std::shared_ptr<func_kind> const&) {
   }
   void operator()(std::shared_ptr<record_kind> const&) {
   }
   void operator()(std::shared_ptr<class_kind> const&) {
   }
   void operator()(std::shared_ptr<array_kind> const&) {
      curStmts.push_back(ArrayDeclarationLiteralExpression{{{scopePtr}, identifier, *sym.kind, emitChapelLine(ast), sym.kindqualifier, sym.isConfig}, *sym.literal});
   }
   void operator()(std::shared_ptr<associative_kind> const&) {
   }
};

std::string ProgramTreeBuildingVisitor::emitChapelLine(uast::AstNode const* ast) {
   auto const fp = br.filePath();
   return chplx::util::emitLineDirective(fp.c_str(), br.idToLocation(ast->id(), fp).line());
}

bool ProgramTreeBuildingVisitor::enter(const uast::AstNode * ast) {
   if(chplx::util::compilerDebug) {
      std::cout << "***Enter AST Node\t" << tagToString(ast->tag()) << std::endl
                << "***\tCurrent Scope\t" << symbolTable.symbolTableRef->id << std::endl
                << "***\tCurrent Statement List Size\t" << curStmts.size() << std::endl
                << "***\t" << emitChapelLine(ast);
   }

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
    {
       std::vector<Statement> * cStmts = curStmts.back();
       const bool cStmtsnz = 0 < cStmts->size();
       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<BinaryOpExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
           std::string identifier{dynamic_cast<Identifier const*>(ast)->name().c_str()};
           std::optional<Symbol> varsym =
               symbolTable.find(symbolTableRef->id, identifier);
           if(!varsym) {
              auto rsym =
                 symbolTable.findPrefix(symbolTableRef->id, identifier);

              if(!rsym) {
                 std::cerr << "chplx error: Undefined symbol \"" << identifier << "\" detected; check\t" << emitChapelLine(ast) << std::endl << std::flush;
                 return false;
              }

              auto itr = rsym->first;
              for(; itr != rsym->second; ++itr) {
                 if(itr->first.size() >= identifier.size() && itr->first.substr(0, identifier.size()) == identifier) {
                    cStmts->emplace_back(VariableExpression{std::make_shared<Symbol>(itr->second)});
                    break;
                 }
              }
           }
           else {
              cStmts->emplace_back(VariableExpression{std::make_shared<Symbol>(*varsym)});
           }
       }
       else if(cStmtsnz && std::holds_alternative<std::shared_ptr<ReturnExpression>>(cStmts->back())) {
           std::shared_ptr<ReturnExpression> & ret = std::get<std::shared_ptr<ReturnExpression>>(cStmts->back());
           std::string identifier{dynamic_cast<Identifier const*>(ast)->name().c_str()};
           std::optional<Symbol> varsym =
               symbolTable.find(symbolTableRef->id, identifier);
           if(varsym) { 
              ret->statement.emplace_back(VariableExpression{std::make_shared<Symbol>(*varsym)});
              return true;
           }

           auto rsym =
              symbolTable.findPrefix(symbolTableRef->id, identifier);

           auto itr = rsym->first;
           for(; itr != rsym->second; ++itr) {
              if(itr->first.size() >= identifier.size() && itr->first.substr(0, identifier.size()) == identifier) {
                 ret->statement.emplace_back(VariableExpression{std::make_shared<Symbol>(itr->second)});
                 break;
              }
           }
       }
       else {
          std::string identifier{dynamic_cast<Identifier const*>(ast)->name().c_str()};
          std::optional<Symbol> varsym =
             symbolTable.find(symbolTableRef->id, identifier);
          if(varsym) {
              cStmts->emplace_back(VariableExpression{std::make_shared<Symbol>(*varsym)});
          }
          else {
             std::optional< std::pair< std::map<std::string, Symbol>::iterator, std::map<std::string, Symbol>::iterator > > fnsym =
                symbolTable.findPrefix(symbolTableRef->id, identifier);
             
             if(!fnsym) {
                 std::cerr << "chplx error: Undefined symbol \"" << identifier << "\" detected; check\t" << emitChapelLine(ast) << std::endl << std::flush;
                 return false;
             } 
             else {
                auto itr = fnsym->first;
                for(; itr != fnsym->second; ++itr) {
                    if(itr->first.size() >= identifier.size() && itr->first.substr(0, identifier.size()) == identifier) {
                        cStmts->emplace_back(VariableExpression{std::make_shared<Symbol>(itr->second)});
                        break;
                    }
                }
             }
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
       //std::vector<Statement> * cStmts = curStmts.back();

       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<ForLoopExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
          std::optional<Symbol> varsym =
             symbolTable.find(symbolTableRef->id, std::string{"range_" + emitChapelLine(ast)});
          if(varsym) {
             std::shared_ptr<ForLoopExpression> & fl =
                std::get<std::shared_ptr<ForLoopExpression>>(curStmts[curStmts.size()-2]->back());

             std::shared_ptr<func_kind> & fc = std::get<std::shared_ptr<func_kind>>(*fl->symbol.kind);

             if(!(fl->indexSet)) {
                fl->indexSet = fc->args[0];
             }
          }
       }
       else if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<ForallLoopExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
          std::optional<Symbol> varsym =
             symbolTable.find(symbolTableRef->id, std::string{"range_" + emitChapelLine(ast)});
          if(varsym) {
             std::shared_ptr<ForallLoopExpression> & fl =
                std::get<std::shared_ptr<ForallLoopExpression>>(curStmts[curStmts.size()-2]->back());

             std::shared_ptr<func_kind> & fc = std::get<std::shared_ptr<func_kind>>(*fl->symbol.kind);

             if(!(fl->indexSet)) {
                fl->indexSet = fc->args[0];
             }
          }
       }
       else if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<CoforallLoopExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
          std::optional<Symbol> varsym =
             symbolTable.find(symbolTableRef->id, std::string{"range_" + emitChapelLine(ast)});
          if(varsym) {
             std::shared_ptr<CoforallLoopExpression> & fl =
                std::get<std::shared_ptr<CoforallLoopExpression>>(curStmts[curStmts.size()-2]->back());

             std::shared_ptr<func_kind> & fc = std::get<std::shared_ptr<func_kind>>(*fl->symbol.kind);

             if(!(fl->indexSet)) {
                fl->indexSet = fc->args[0];
             }
          }
       }
    }
    break;
    case asttags::Require:
    break;
    case asttags::Return:
    {
       std::vector<Statement> * cStmts = curStmts.back();

       if(std::holds_alternative<std::shared_ptr<FunctionDeclarationExpression>>(curStmts[curStmts.size()-2]->back())) {
           cStmts->emplace_back(
               std::make_shared<ReturnExpression>(ReturnExpression{{}, {emitChapelLine(ast)}})
           );

           curStmts.push_back(&(std::get<std::shared_ptr<ReturnExpression>>(cStmts->back())->statement));
       }
    }
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
       std::vector<Statement> * cStmts = curStmts.back();
       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<BinaryOpExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
           cStmts->emplace_back(LiteralExpression{bool_kind{}, ast});
       }
       else if(0 < cStmts->size() && std::holds_alternative<std::shared_ptr<FunctionCallExpression>>(cStmts->back())) {
          std::shared_ptr<FunctionCallExpression> & fce =
             std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back()); 
          fce->arguments.push_back(LiteralExpression{bool_kind{}, ast});
       }
       else if(0 < cStmts->size() && std::holds_alternative<std::shared_ptr<ReturnExpression>>(cStmts->back())) {
           std::shared_ptr<ReturnExpression> & ret = std::get<std::shared_ptr<ReturnExpression>>(cStmts->back());
           ret->statement.emplace_back(LiteralExpression{bool_kind{}, ast});
       }
       else {
           cStmts->emplace_back(LiteralExpression{bool_kind{}, ast});
       }
    }
    break;
    case asttags::ImagLiteral:
    break;
    case asttags::IntLiteral:
    {
       std::vector<Statement> * cStmts = curStmts.back();
       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<BinaryOpExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
           cStmts->emplace_back(LiteralExpression{int_kind{}, ast});
       }
       else if(0 < cStmts->size() && std::holds_alternative<std::shared_ptr<FunctionCallExpression>>(cStmts->back())) {
          std::shared_ptr<FunctionCallExpression> & fce =
             std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back()); 
          fce->arguments.push_back(LiteralExpression{int_kind{}, ast});
       }
       else if(0 < cStmts->size() && std::holds_alternative<std::shared_ptr<ReturnExpression>>(cStmts->back())) {
           std::shared_ptr<ReturnExpression> & ret = std::get<std::shared_ptr<ReturnExpression>>(cStmts->back());
           ret->statement.emplace_back(LiteralExpression{int_kind{}, ast});
       }
       else {
           cStmts->emplace_back(LiteralExpression{int_kind{}, ast});
       }
    }
    break;
    case asttags::RealLiteral:
    {
       std::vector<Statement> * cStmts = curStmts.back();
       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<BinaryOpExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
           cStmts->emplace_back(LiteralExpression{real_kind{}, ast});
       }
       else if(0 < cStmts->size() && std::holds_alternative<std::shared_ptr<FunctionCallExpression>>(cStmts->back())) {
          std::shared_ptr<FunctionCallExpression> & fce =
             std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back()); 
          fce->arguments.push_back(LiteralExpression{real_kind{}, ast});
       }
       else if(0 < cStmts->size() && std::holds_alternative<std::shared_ptr<ReturnExpression>>(cStmts->back())) {
           std::shared_ptr<ReturnExpression> & ret = std::get<std::shared_ptr<ReturnExpression>>(cStmts->back());
           ret->statement.emplace_back(LiteralExpression{real_kind{}, ast});
       }
       else {
           cStmts->emplace_back(LiteralExpression{real_kind{}, ast});
       }
    }
    break;
    case asttags::UintLiteral:
    break;
    case asttags::START_StringLikeLiteral:
    break;
    case asttags::BytesLiteral:
    break;
    case asttags::CStringLiteral:
    break;
    case asttags::StringLiteral:
    {
       std::vector<Statement> * cStmts = curStmts.back();
       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<BinaryOpExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
          cStmts->emplace_back(LiteralExpression{real_kind{}, ast});
       }
       else if(0 < cStmts->size() && std::holds_alternative<std::shared_ptr<FunctionCallExpression>>(cStmts->back())) {
          std::shared_ptr<FunctionCallExpression> & fce =
             std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back()); 
          fce->arguments.push_back(LiteralExpression{string_kind{}, ast});
       }
       else if(0 < cStmts->size() && std::holds_alternative<std::shared_ptr<ReturnExpression>>(cStmts->back())) {
           std::shared_ptr<ReturnExpression> & ret = std::get<std::shared_ptr<ReturnExpression>>(cStmts->back());
           ret->statement.emplace_back(LiteralExpression{string_kind{}, ast});
       }
       else {
           cStmts->emplace_back(LiteralExpression{string_kind{}, ast});
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
    {
       std::vector<Statement> * cStmts = curStmts.back();

       const FnCall* fc = dynamic_cast<const FnCall*>(ast);

       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<BinaryOpExpression>>( curStmts[curStmts.size()-2]->back() ) &&
           fc->calledExpression()->tag() == asttags::Identifier && !fc->callUsedSquareBrackets() ) {
           std::string identifier{dynamic_cast<const Identifier*>(fc->calledExpression())->name().c_str()};

           std::optional<Symbol> varsym =
              symbolTable.find(symbolTableRef->id, identifier);

           if(varsym.has_value() && std::holds_alternative<std::shared_ptr<array_kind>>(*varsym->kind)) {
              auto paren = symbolTable.find(symbolTableRef->id, "[]");

              auto encop = operatorEncoder.find("[]");
              if(std::end(operatorEncoder) == encop) {
                 std::cerr << "programtreebuildingvisitor.cpp, enter, OpCall, identifier not found" << std::endl << std::flush;
                 return false;
              }

              cStmts->emplace_back(
                 std::make_shared<FunctionCallExpression>(
                    FunctionCallExpression{{symbolTableRef->id}, {*paren}, {}, emitChapelLine(ast), symbolTable}
              ));
              curStmts.push_back(&(std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back())->arguments));
           }
           else { 
              auto rsym =
                symbolTable.findPrefix(symbolTableRef->id, identifier);
              auto itr = rsym->first;
              for(; itr != rsym->second; ++itr) {

                  auto pipeloc = itr->first.find('|');
                  std::string itrstr {(pipeloc == itr->first.npos) ? itr->first : itr->first.substr(0,pipeloc)};

                  if(itrstr.size() >= identifier.size() && itrstr == identifier) {
                      cStmts->emplace_back(
                         std::make_shared<FunctionCallExpression>(
                            FunctionCallExpression{{symbolTableRef->id}, {itr->second}, {}, emitChapelLine(ast), symbolTable}
                      ));
                      curStmts.push_back(&(std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back())->arguments));
                      break;
                  }
              }
           }
       }
       // function called
       //
       else if(fc->calledExpression()->tag() == asttags::Identifier && !fc->callUsedSquareBrackets()) {
           std::string identifier{dynamic_cast<const Identifier*>(fc->calledExpression())->name().c_str()};

           std::optional<Symbol> fnsym =
              symbolTable.find(symbolTableRef->id, identifier);

           if(fnsym) { 
              cStmts->emplace_back(
                 std::make_shared<FunctionCallExpression>(
                    FunctionCallExpression{{symbolTableRef->id}, {*fnsym}, {}, emitChapelLine(ast), symbolTable}
              ));
              curStmts.push_back(&(std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back())->arguments));
              return true;
           }

           std::optional< std::pair< std::map<std::string, Symbol>::iterator, std::map<std::string, Symbol>::iterator > > fsym =
              symbolTable.findPrefix(symbolTableRef->id, identifier);

           if(!fsym) {
              std::cerr << "chplx error: Undefined symbol \"" << identifier << "\" detected; check\t" << emitChapelLine(ast) << std::endl << std::flush;
              return false;
           }
           else {
              if(std::holds_alternative<ScalarDeclarationExpression>(cStmts->back())) {
                 auto scalarDecl = std::get<ScalarDeclarationExpression>(cStmts->back());
                 cStmts->pop_back();
                 cStmts->emplace_back(
                     std::make_shared<BinaryOpExpression>(BinaryOpExpression{
                         {{symbolTableRef->id}, "=", ast}, {}
                     })
                 );
                 auto & bo = std::get<std::shared_ptr<BinaryOpExpression>>(cStmts->back());
                 bo->statements.emplace_back(
                    ScalarDeclarationLiteralExpression{{{scalarDecl.scopeId}, scalarDecl.identifier, scalarDecl.kind, scalarDecl.chplLine, scalarDecl.qualifier, scalarDecl.config}, {}}
                 );

                 auto itr = fsym->first;
                 for(; itr != fsym->second; ++itr) {
                    if(itr->first.size() >= identifier.size() && itr->first.substr(0, identifier.size()) == identifier) {
                       bo->statements.emplace_back(
                          std::make_shared<FunctionCallExpression>(
                             FunctionCallExpression{{symbolTableRef->id}, itr->second, {}, emitChapelLine(ast), symbolTable}
                       ));
                       curStmts.push_back(&(std::get<std::shared_ptr<FunctionCallExpression>>(bo->statements.back())->arguments));
                       break;
                    }
                 }
              }
              else if(std::holds_alternative<VariableExpression>(cStmts->back())) {
                 cStmts->pop_back();
                 auto scalarDecl = std::get<ScalarDeclarationExpression>(cStmts->back());
                 cStmts->pop_back();
                 cStmts->emplace_back(
                     std::make_shared<BinaryOpExpression>(BinaryOpExpression{
                         {{symbolTableRef->id}, "=", ast}, {}
                     })
                 );
                 auto & bo = std::get<std::shared_ptr<BinaryOpExpression>>(cStmts->back());
                 bo->statements.emplace_back(
                    ScalarDeclarationLiteralExpression{{{scalarDecl.scopeId}, scalarDecl.identifier, scalarDecl.kind, scalarDecl.chplLine, scalarDecl.qualifier, scalarDecl.config}, {}}
                 );

                 auto itr = fsym->first;
                 for(; itr != fsym->second; ++itr) {
                    if(itr->first.size() >= identifier.size() && itr->first.substr(0, identifier.size()) == identifier) {
                       bo->statements.emplace_back(
                          std::make_shared<FunctionCallExpression>(
                             FunctionCallExpression{{symbolTableRef->id}, itr->second, {}, emitChapelLine(ast), symbolTable}
                       ));
                       curStmts.push_back(&(std::get<std::shared_ptr<FunctionCallExpression>>(bo->statements.back())->arguments));
                       break;
                    }
                 }
              }
              else {
                 auto itr = fsym->first;
                 for(; itr != fsym->second; ++itr) {
                    if(itr->first.size() >= identifier.size() && itr->first.substr(0, identifier.size()) == identifier) {
                       cStmts->emplace_back(
                          std::make_shared<FunctionCallExpression>(
                             FunctionCallExpression{{symbolTableRef->id}, itr->second, {}, emitChapelLine(ast), symbolTable}
                       ));
                       curStmts.push_back(&(std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back())->arguments));
                       break;
                    }
                 }
              }
           }
       }
       // array index invoked
       //
       else if(fc->calledExpression()->tag() == asttags::Identifier && fc->callUsedSquareBrackets()) {
           std::optional<Symbol> varsym =
               symbolTable.find(symbolTableRef->id, "[]");

           auto encop = operatorEncoder.find("[]");
           if(std::end(operatorEncoder) == encop) {
               std::cerr << "programtreebuildingvisitor.cpp, enter, OpCall, identifier not found" << std::endl << std::flush;
               return false;
           }

           cStmts->emplace_back(
              std::make_shared<FunctionCallExpression>(
                 FunctionCallExpression{{symbolTableRef->id}, {*varsym}, {}, emitChapelLine(ast), symbolTable}
           ));
              curStmts.push_back(&(std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back())->arguments));
       }
       else {
           std::cerr << "programtreebuildingvisitor.cpp, enter, FnCall, identifier not found" << std::endl << std::flush;
           return false;
       }
    }
    break;
    case asttags::OpCall:
    {
       OpCall const* ptr =
          dynamic_cast<OpCall const*>(ast);

       std::string identifier{ptr->op().c_str()};

       std::optional<Symbol> varsym =
           symbolTable.find(symbolTableRef->id, identifier);

       if(!varsym) {
           std::cerr << "programtreebuildingvisitor.cpp, enter, OpCall, identifier not found" << std::endl << std::flush;
           return false;
       }

       auto encop = operatorEncoder.find(identifier);
       if(std::end(operatorEncoder) == encop) {
           std::cerr << "programtreebuildingvisitor.cpp, enter, OpCall, identifier not found" << std::endl << std::flush;
           return false;
       }

       std::vector<Statement> * cStmts = curStmts.back();

       switch(encop->second) {
           case 0: // =
           case 1: // +
           case 2: // -
           case 3: // *
           case 4: // /
           case 5: // %
           case 7: // ==
           case 8: // <=>
           {
               cStmts->emplace_back(
                   std::make_shared<BinaryOpExpression>(BinaryOpExpression{
                       {{symbolTableRef->id}, identifier, ast}, {}
                   })
               );
               auto & bo = std::get<std::shared_ptr<BinaryOpExpression>>(cStmts->back());
               curStmts.emplace_back(&(bo->statements));
           }
           break;
       }
    }
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
       std::string identifier =
          std::string{dynamic_cast<NamedDecl const*>(ast)->name().c_str()};

       std::optional<Symbol> varsym{};
       symbolTable.find(symbolTableRef->id, identifier, varsym);

       bool stmt = true;

       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<ForLoopExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
           std::shared_ptr<ForLoopExpression> & fl =
               std::get<std::shared_ptr<ForLoopExpression>>(curStmts[curStmts.size()-2]->back());

           if(!(fl->iterator)) {
               fl->iterator = *varsym;
               stmt = false;
           }
       }
       else if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<ForallLoopExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
           std::shared_ptr<ForallLoopExpression> & fl =
               std::get<std::shared_ptr<ForallLoopExpression>>(curStmts[curStmts.size()-2]->back());

           if(!(fl->iterator)) {
               fl->iterator = *varsym;
               stmt = false;
           }
       }
       else if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<CoforallLoopExpression>>( curStmts[curStmts.size()-2]->back() ) ) {
           std::shared_ptr<CoforallLoopExpression> & fl =
               std::get<std::shared_ptr<CoforallLoopExpression>>(curStmts[curStmts.size()-2]->back());

           if(!(fl->iterator)) {
               fl->iterator = *varsym;
               stmt = false;
           }
       }
     
       if(stmt) {
          if(varsym && !varsym->literal) {
             std::vector<Statement> * cStmts = curStmts.back();
             std::visit(
                VariableVisitor{symbolTableRef->id, identifier, *varsym, *cStmts, br, ast},
                *varsym->kind
             );
          }
          else if(varsym && varsym->literal) {
             std::vector<Statement> * cStmts = curStmts.back();
             std::visit(
                VariableLiteralVisitor{symbolTableRef->id, identifier, *varsym, *cStmts, br, ast},
                *varsym->kind
             );
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
    {
       std::string identifier{"if" + emitChapelLine(ast)};

       std::optional<Symbol> varsym =
          symbolTable.find(symbolTableRef->id, identifier);

       if(varsym.has_value() && std::holds_alternative<std::shared_ptr<func_kind>>(*varsym->kind)) {
          std::vector<Statement> * cStmts = curStmts.back();

          std::shared_ptr<func_kind> & fk = std::get<std::shared_ptr<func_kind>>(*varsym->kind);
          symbolTableRef = symbolTable.lut[fk->lutId];

          if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<ConditionalExpression>>( curStmts[curStmts.size()-2]->back() )) {
             curStmts.pop_back();
             cStmts = curStmts.back();

             auto ce = std::get<std::shared_ptr<ConditionalExpression>>(cStmts->back());
             ce->exprs.back().scopeId = fk->lutId;
             ce->exprs.back().node = ast;

             curStmts.emplace_back(&(ce->exprs.back().conditions));
          }
          else {

             cStmts->emplace_back(
                std::make_shared<ConditionalExpression>(
                   ConditionalExpression{{{fk->lutId}, ast}, *varsym, {ConditionedExpression{{{fk->lutId}, ast},{},{}}}}
             ));
             auto & fndecl = std::get<std::shared_ptr<ConditionalExpression>>(cStmts->back());
             curStmts.emplace_back(&(fndecl->exprs.back().conditions));
          }
       }
    }
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
       if (1 < curStmts.size() && 0 < curStmts[curStmts.size()-2]->size() && std::holds_alternative<std::shared_ptr<ConditionalExpression>>( curStmts[curStmts.size()-2]->back() )) {
          curStmts.pop_back();
          std::vector<Statement> * cStmts = curStmts.back();
          auto & fndecl = std::get<std::shared_ptr<ConditionalExpression>>(cStmts->back());
          curStmts.emplace_back(&(fndecl->exprs.back().statements));
       }
       else if(0 < curStmts.size() && 0 < curStmts[curStmts.size()-1]->size() && std::holds_alternative<std::shared_ptr<ConditionalExpression>>( curStmts[curStmts.size()-1]->back() )) {
          std::vector<Statement> * cStmts = curStmts.back();
          auto & ce = std::get<std::shared_ptr<ConditionalExpression>>(cStmts->back());
          ce->exprs.emplace_back(
             ConditionedExpression{{{symbolTableRef->id}, ast},{},{}}
          );
          curStmts.emplace_back(&(ce->exprs.back().statements));
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
    break;
    case asttags::Coforall:
    {
       std::string identifier{"coforall" + emitChapelLine(ast)};
       std::optional<Symbol> varsym =
          symbolTable.find(symbolTableRef->id, identifier);

       if(varsym.has_value() && std::holds_alternative<std::shared_ptr<func_kind>>(*varsym->kind)) {
          std::vector<Statement> * cStmts = curStmts.back();

          std::shared_ptr<func_kind> & fk = std::get<std::shared_ptr<func_kind>>(*varsym->kind);
          symbolTableRef = symbolTable.lut[fk->lutId];

          cStmts->emplace_back(
             std::make_shared<CoforallLoopExpression>(
                CoforallLoopExpression{{{fk->lutId}, ast, {}}, *varsym, {}, {}, {}, emitChapelLine(ast)}
          ));

          auto & fndecl = std::get<std::shared_ptr<CoforallLoopExpression>>(cStmts->back());
          curStmts.emplace_back(&(fndecl->statements));
       }
    }
    break;
    case asttags::For:
    {
       std::string identifier{"for" + emitChapelLine(ast)};
       std::optional<Symbol> varsym =
          symbolTable.find(symbolTableRef->id, identifier);

       if(varsym.has_value() && std::holds_alternative<std::shared_ptr<func_kind>>(*varsym->kind)) {
          std::vector<Statement> * cStmts = curStmts.back();

          std::shared_ptr<func_kind> & fk = std::get<std::shared_ptr<func_kind>>(*varsym->kind);
          symbolTableRef = symbolTable.lut[fk->lutId];

          cStmts->emplace_back(
             std::make_shared<ForLoopExpression>(
                ForLoopExpression{{{fk->lutId}, ast, {}}, *varsym, {}, {}, {}, emitChapelLine(ast)}
          ));

          auto & fndecl = std::get<std::shared_ptr<ForLoopExpression>>(cStmts->back());
          curStmts.emplace_back(&(fndecl->statements));
       }
    }
    break;
    case asttags::Forall:
    {
       std::string identifier{"forall" + emitChapelLine(ast)};
       std::optional<Symbol> varsym =
          symbolTable.find(symbolTableRef->id, identifier);

       if(varsym.has_value() && std::holds_alternative<std::shared_ptr<func_kind>>(*varsym->kind)) {
          std::vector<Statement> * cStmts = curStmts.back();

          std::shared_ptr<func_kind> & fk = std::get<std::shared_ptr<func_kind>>(*varsym->kind);
          symbolTableRef = symbolTable.lut[fk->lutId];

          cStmts->emplace_back(
             std::make_shared<ForallLoopExpression>(
                ForallLoopExpression{{{fk->lutId}, ast, {}}, *varsym, {}, {}, {}, emitChapelLine(ast)}
          ));

          auto & fndecl = std::get<std::shared_ptr<ForallLoopExpression>>(cStmts->back());
          curStmts.emplace_back(&(fndecl->statements));
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

       if(node.has_value() && ast != (*node)) {
          ProgramTreeFunctionVisitor v{false, {}};
          ast->traverse(v);

          std::optional<Symbol> sym =
             symbolTable.find(symbolTableRef->id, v.lookup);

          if(sym) {
             std::shared_ptr<func_kind> & fk = std::get<std::shared_ptr<func_kind>>(*sym->kind);
             symbolTableRef = symbolTable.lut[fk->lutId];

             std::vector<Statement> * cStmts = curStmts.back();

             cStmts->emplace_back(
                std::make_shared<FunctionDeclarationExpression>(
                   FunctionDeclarationExpression{{{fk->lutId}, ast, {}}, *sym, {}, emitChapelLine(ast)}
             ));

             auto & fndecl = std::get<std::shared_ptr<FunctionDeclarationExpression>>(cStmts->back());
             curStmts.emplace_back(&(fndecl->statements));
          }
          else {
             std::optional< std::pair< std::map<std::string, Symbol>::iterator, std::map<std::string, Symbol>::iterator > > fnsym
                = symbolTable.findPrefix(symbolTableRef->id, v.lookup);

             auto val = fnsym->second;
             for(auto itr = fnsym->first; itr != fnsym->second; ++itr) {
                 if(v.lookup == itr->first) {
                    val = itr;
                 } 
             }

             if(val == fnsym->second) { return false; }

             //Symbol & fsym = val->second;

             std::shared_ptr<func_kind> & fk = std::get<std::shared_ptr<func_kind>>(*sym->kind);
             symbolTableRef = symbolTable.lut[fk->lutId];
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

   return true;
}

void ProgramTreeBuildingVisitor::exit(const uast::AstNode * ast) {
   if(chplx::util::compilerDebug) {
      std::cout << "---Exit AST Node\t" << tagToString(ast->tag()) << std::endl
                << "---\tCurrent Scope\t" << symbolTable.symbolTableRef->id << std::endl
                << "---\tCurrent Statement List Size\t" << curStmts.size() << std::endl
                << "---\t" << emitChapelLine(ast);
   }

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
    case asttags::START_StringLikeLiteral:
    break;
    case asttags::BytesLiteral:
    break;
    case asttags::CStringLiteral:
    break;
    case asttags::StringLiteral:
    break;
    case asttags::END_StringLikeLiteral:
    break;
    case asttags::END_Literal:
    break;
    case asttags::START_Call:
    break;
    case asttags::For:
    break;
    case asttags::Forall:
    break;
    case asttags::Return:
    case asttags::FnCall:
    case asttags::OpCall:
    {
       curStmts.pop_back();
    }
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
       if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<ConditionalExpression>>( curStmts[curStmts.size()-2]->back() )) {
          curStmts.pop_back();

          std::vector<Statement> * cStmts = curStmts.back();
          std::shared_ptr<ConditionalExpression> & fde =
             std::get<std::shared_ptr<ConditionalExpression>>(cStmts->back());
       
          if(symbolTable.lut[fde->scopeId]->parent && std::holds_alternative<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent)) {
             symbolTableRef = std::get<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent);
          }
          else {
             symbolTableRef = symbolTable.lut[0];
          }
       }
       else if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<ForLoopExpression>>( curStmts[curStmts.size()-2]->back() )) {
          curStmts.pop_back();

          std::vector<Statement> * cStmts = curStmts.back();
          std::shared_ptr<ForLoopExpression> & fde =
             std::get<std::shared_ptr<ForLoopExpression>>(cStmts->back());
       
          if(symbolTable.lut[fde->scopeId]->parent && std::holds_alternative<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent)) {
             symbolTableRef = std::get<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent);
          }
          else {
             symbolTableRef = symbolTable.lut[0];
          }

       }
       else if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<ForallLoopExpression>>( curStmts[curStmts.size()-2]->back() )) {
          curStmts.pop_back();

          std::vector<Statement> * cStmts = curStmts.back();
          std::shared_ptr<ForallLoopExpression> & fde =
             std::get<std::shared_ptr<ForallLoopExpression>>(cStmts->back());
       
          if(symbolTable.lut[fde->scopeId]->parent && std::holds_alternative<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent)) {
             symbolTableRef = std::get<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent);
          }
          else {
             symbolTableRef = symbolTable.lut[0];
          }
       }
       else if (1 < curStmts.size() && std::holds_alternative<std::shared_ptr<CoforallLoopExpression>>( curStmts[curStmts.size()-2]->back() )) {
          curStmts.pop_back();

          std::vector<Statement> * cStmts = curStmts.back();
          std::shared_ptr<CoforallLoopExpression> & fde =
             std::get<std::shared_ptr<CoforallLoopExpression>>(cStmts->back());
       
          if(symbolTable.lut[fde->scopeId]->parent && std::holds_alternative<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent)) {
             symbolTableRef = std::get<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent);
          }
          else {
             symbolTableRef = symbolTable.lut[0];
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
    break;
    case asttags::Coforall:
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
       curStmts.pop_back();
       std::vector<Statement> * cStmts = curStmts.back();

       std::shared_ptr<FunctionDeclarationExpression> & fde =
           std::get<std::shared_ptr<FunctionDeclarationExpression>>(cStmts->back());
       
       if(symbolTable.lut[fde->scopeId]->parent && std::holds_alternative<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent)) {
           symbolTableRef = std::get<std::shared_ptr<SymbolTable::SymbolTableNode>>(*symbolTable.lut[fde->scopeId]->parent);
       }
       else {
           symbolTableRef = symbolTable.lut[0];
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
