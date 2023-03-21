/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "hpx/programtreebuildingvisitor.hpp"
#include "chpl/uast/all-uast.h"

#include <cctype>
#include <fstream>
#include <numeric>
#include <sstream>
#include <variant>

using namespace chplx::ast::hpx;
using namespace chpl::uast;
using namespace chpl::ast::visitors::hpx;

namespace chplx {
namespace ast {
namespace visitors {
namespace hpx {

struct VariableVisitor {

  std::size_t const scopePtr;
  std::string identifier;
  Symbol &sym;
  std::vector<Statement> &curStmts;
  chpl::uast::BuilderResult const &br;
  uast::AstNode const *ast;

  std::string emitChapelLine(uast::AstNode const *ast) const {
    auto fp = br.filePath();
    std::stringstream os{};
    os << "#line " << br.idToLocation(ast->id(), fp).line() << " \""
       << fp.c_str() << "\"";
    return os.str();
  }

  template <typename T> void operator()(T const &t) {}

  void operator()(byte_kind const &) {
    curStmts.push_back(ScalarDeclarationExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)}});
  }
  void operator()(bool_kind const &) {
    curStmts.push_back(ScalarDeclarationExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)}});
  }
  void operator()(int_kind const &) {
    curStmts.push_back(ScalarDeclarationExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)}});
  }
  void operator()(real_kind const &) {
    curStmts.push_back(ScalarDeclarationExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)}});
  }
  void operator()(complex_kind const &) {
    curStmts.push_back(ScalarDeclarationExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)}});
  }
  void operator()(string_kind const &) {
    curStmts.push_back(ScalarDeclarationExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)}});
  }
  void operator()(template_kind const &) {}
  void operator()(range_kind const &) {}
  void operator()(domain_kind const &) {}
  void operator()(std::shared_ptr<func_kind> const &) {}
  void operator()(std::shared_ptr<record_kind> const &) {}
  void operator()(std::shared_ptr<class_kind> const &) {}
  void operator()(std::shared_ptr<array_kind> const &t) {
    curStmts.push_back(ArrayDeclarationExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)}});
  }
  void operator()(std::shared_ptr<associative_kind> const &) {}
};

struct VariableLiteralVisitor {

  std::size_t const scopePtr;
  std::string identifier;
  Symbol &sym;
  std::vector<Statement> &curStmts;
  chpl::uast::BuilderResult const &br;
  uast::AstNode const *ast;

  std::string emitChapelLine(uast::AstNode const *ast) const {
    auto fp = br.filePath();
    std::stringstream os{};
    os << "#line " << br.idToLocation(ast->id(), fp).line() << " \""
       << fp.c_str() << "\"";
    return os.str();
  }

  template <typename T> void operator()(T const &t) {}

  void operator()(byte_kind const &) {
    curStmts.push_back(ScalarDeclarationLiteralExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)},
        *sym.literal});
  }
  void operator()(bool_kind const &) {
    curStmts.push_back(ScalarDeclarationLiteralExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)},
        *sym.literal});
  }
  void operator()(int_kind const &) {
    curStmts.push_back(ScalarDeclarationLiteralExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)},
        *sym.literal});
  }
  void operator()(real_kind const &) {
    curStmts.push_back(ScalarDeclarationLiteralExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)},
        *sym.literal});
  }
  void operator()(complex_kind const &) {
    curStmts.push_back(ScalarDeclarationLiteralExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)},
        *sym.literal});
  }
  void operator()(string_kind const &) {
    curStmts.push_back(ScalarDeclarationLiteralExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)},
        *sym.literal});
  }
  void operator()(template_kind const &) {}
  void operator()(range_kind const &) {}
  void operator()(domain_kind const &) {}
  void operator()(std::shared_ptr<func_kind> const &) {}
  void operator()(std::shared_ptr<record_kind> const &) {}
  void operator()(std::shared_ptr<class_kind> const &) {}
  void operator()(std::shared_ptr<array_kind> const &) {
    curStmts.push_back(ArrayDeclarationLiteralExpression{
        {{scopePtr}, identifier, *sym.kind, emitChapelLine(ast)},
        *sym.literal});
  }
  void operator()(std::shared_ptr<associative_kind> const &) {}
};

std::string
ProgramTreeBuildingVisitor::emitChapelLine(uast::AstNode const *ast) {
  auto fp = br.filePath();
  std::stringstream os{};
  os << "#line " << br.idToLocation(ast->id(), fp).line() << " \"" << fp.c_str()
     << "\"";
  return os.str();
}

bool ProgramTreeBuildingVisitor::enter(uast::AstNode const *ast) {
  // std::cout << "enter node tag\t" << ast->tag() << '\t' <<
  // tagToString(ast->tag()) << std::endl;
  switch (ast->tag()) {
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
  case asttags::Identifier: {
    std::vector<Statement> *cStmts = curStmts.back();
    if (0 < cStmts->size() &&
        std::holds_alternative<std::shared_ptr<FunctionCallExpression>>(
            cStmts->back())) {

      std::shared_ptr<FunctionCallExpression> &fce =
          std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back());

      std::string identifier{
          dynamic_cast<Identifier const *>(ast)->name().c_str()};
      std::optional<Symbol> varsym = symbolTable.find(scopePtr, identifier);

      if (varsym) {
        if (!std::holds_alternative<std::shared_ptr<func_kind>>(
                (*(varsym->kind))) &&
            !std::holds_alternative<std::shared_ptr<cxxfunc_kind>>(
                (*(varsym->kind)))) {
          fce->arguments.emplace_back(VariableExpression{*varsym});
        } else {
          fce->symbol = *varsym;
        }
      } else {
        std::cerr << "chplx error: Undefined symbol \"" << identifier
                  << "\" detected; check\t" << emitChapelLine(ast) << std::endl
                  << std::flush;
        return false;
      }
    }
  } break;
  case asttags::Import:
    break;
  case asttags::Include:
    break;
  case asttags::Let:
    break;
  case asttags::New:
    break;
  case asttags::Range: {
    std::vector<Statement> *cStmts = curStmts.back();
    if (0 < cStmts->size() &&
        std::holds_alternative<std::shared_ptr<ForallLoopExpression>>(
            cStmts->back())) {
      std::optional<Symbol> varsym = symbolTable.find(
          scopePtr, std::string{"range_" + emitChapelLine(ast)});
      if (varsym) {
        ForallLoopExpression &expr =
            *std::get<std::shared_ptr<ForallLoopExpression>>(cStmts->back());
        expr.index_set = *varsym;
      }
    }
  } break;
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
  case asttags::START_StringLikeLiteral:
    break;
  case asttags::BytesLiteral:
    break;
  case asttags::CStringLiteral:
    break;
  case asttags::StringLiteral: {
    std::vector<Statement> *cStmts = curStmts.back();
    if (0 < cStmts->size() &&
        std::holds_alternative<std::shared_ptr<FunctionCallExpression>>(
            cStmts->back())) {
      // std::string strliteral{dynamic_cast<StringLiteral
      // const*>(ast)->value().c_str()};

      std::shared_ptr<FunctionCallExpression> &fce =
          std::get<std::shared_ptr<FunctionCallExpression>>(cStmts->back());
      fce->arguments.push_back(LiteralExpression{string_kind{}, ast});
    }
  } break;
  case asttags::END_StringLikeLiteral:
    break;
  case asttags::END_Literal:
    break;
  case asttags::START_Call:
    break;
  case asttags::FnCall: {
    std::vector<Statement> *cStmts = curStmts.back();
    // FnCall const *astptr = dynamic_cast<FnCall const *>(ast);
    if (0 < cStmts->size() &&
        std::holds_alternative<std::shared_ptr<FunctionDeclarationExpression>>(
            cStmts->back())) {
      auto &fndecl = std::get<std::shared_ptr<FunctionDeclarationExpression>>(
          cStmts->back());
      fndecl->statements.emplace_back(
          std::make_shared<FunctionCallExpression>(FunctionCallExpression{
              {scopePtr}, {}, {}, emitChapelLine(ast), fndecl->symbolTable}));
    } else {
      cStmts->emplace_back(
          std::make_shared<FunctionCallExpression>(FunctionCallExpression{
              {scopePtr}, {}, {}, emitChapelLine(ast), symbolTable}));
    }

    /*
          std::string identifier =
              std::string{astptr->actualName(0).c_str()};
    std::cout << identifier << std::endl;

           std::optional<Symbol> varsym =
              symbolTable.find(scopePtr, identifier);

           if(varsym && !varsym->literal) {
                 std::visit(
                    VariableVisitor{scopePtr, identifier, *varsym, curStmts, br,
    ast}, *varsym->kind
                 );
           }
    */

    /*
           // is assignment
           //
           if(ptr->callUsedSquareBrackets() && ptr->numActuals() == 1 &&
       std::strcmp("", ptr->actualName(0).c_str()) == 0 ) { if(0 <
       curStmts.size() &&
       std::holds_alternative<std::shared_ptr<ForallLoopExpression>>(curStmts.back()))
       { ForallLoopExpression & expr =
       *std::get<std::shared_ptr<ForallLoopExpression>>(curStmts.back());
                 expr.statements.push_back(std::make_shared<BinaryOpExpression>(BinaryOpExpression{{{symbolTable.symbolTableCount},"[]",
       ast}, {}}));
              }
           }
        }
        break;
        case asttags::OpCall:
        {
           if(0 < curStmts.size() &&
       std::holds_alternative<std::shared_ptr<ForallLoopExpression>>(curStmts.back()))
       { ForallLoopExpression & expr =
       *std::get<std::shared_ptr<ForallLoopExpression>>(curStmts.back()); OpCall
       const* ptr = dynamic_cast<OpCall const*>(ast);
              expr.statements.push_back(std::make_shared<BinaryOpExpression>(BinaryOpExpression{{{symbolTable.symbolTableCount},std::string{ptr->op().c_str()},
       ast}, {}}));
           }
    */
  } break;
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
  case asttags::Variable: {
    std::string identifier =
        std::string{dynamic_cast<NamedDecl const *>(ast)->name().c_str()};

    // TODO: going to have to modify to handle scope (function calls, forloops,
    // etc)
    //
    std::optional<Symbol> varsym{};
    symbolTable.find(scopePtr, identifier, varsym);

    if (varsym && !varsym->literal) {
      std::vector<Statement> *cStmts = curStmts.back();
      std::visit(
          VariableVisitor{scopePtr, identifier, *varsym, *cStmts, br, ast},
          *varsym->kind);
    } else if (varsym && varsym->literal) {
      std::vector<Statement> *cStmts = curStmts.back();
      std::visit(VariableLiteralVisitor{scopePtr, identifier, *varsym, *cStmts,
                                        br, ast},
                 *varsym->kind);
    }
  } break;
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
  case asttags::Forall: {
    /*
           ++scopePtr;
           auto stn = symbolTable.lut[scopePtr];
           SymbolTable st{};
           st.symbolTableRoot = stn;
           curStmts.push_back(std::make_shared<ForallLoopExpression>(ForallLoopExpression{{{symbolTable.symbolTableCount},
       ast, {}, std::move(st)}, {},{}}));
    */
  } break;
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
  case asttags::Function: {
    // pattern to repeat in the symbol table builder
    //
    struct ProgramTreeFunctionVisitor {
      bool enter(uast::AstNode const *ast) {
        auto const tag = ast->tag();
        if (tag == asttags::Function && !complete) {
          lookup +=
              std::string{dynamic_cast<Function const *>(ast)->name().c_str()};
        } else if (tag == asttags::Identifier && !complete) {
          lookup +=
              "_" + std::string{
                        dynamic_cast<Identifier const *>(ast)->name().c_str()};
        } else if (tag == asttags::Block && !complete) {
          complete = true;
        }
        return true;
      }

      void exit(uast::AstNode const *ast) {}

      bool complete;
      std::string lookup;
    };

    if (node.has_value() && ast != (*node)) {
      ProgramTreeFunctionVisitor v{false, {}};
      ast->traverse(v);

      std::optional<Symbol> sym = symbolTable.find(scopePtr, v.lookup);

      if (sym) {
        std::vector<Statement> *cStmts = curStmts.back();
        cStmts->emplace_back(std::make_shared<FunctionDeclarationExpression>(
            FunctionDeclarationExpression{
                {{scopePtr}, ast, {}}, *sym, {}, emitChapelLine(ast)}));

        auto &fndecl = std::get<std::shared_ptr<FunctionDeclarationExpression>>(
            cStmts->back());

        curStmts.emplace_back(&(fndecl->statements));
        ++scopePtr;
      }
    }
  } break;
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

void ProgramTreeBuildingVisitor::exit(uast::AstNode const *ast) {
  // std::cout << "exit node tag\t" << ast->tag() << '\t' <<
  // tagToString(ast->tag()) << std::endl;
  switch (ast->tag()) {
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
  case asttags::Function: {
    curStmts.pop_back();
    --scopePtr;
  } break;
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

} /* namespace hpx */
} /* namespace visitors */
} /* namespace ast */
} // namespace chplx
