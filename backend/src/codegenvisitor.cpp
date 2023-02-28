/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "hpx/codegenvisitor.hpp"
#include "chpl/uast/all-uast.h"

#include <variant>
#include <fstream>
#include <cctype>
#include <numeric>
#include <type_traits>

#define INDENT "    "

using namespace chpl::uast;
 
namespace chpl { namespace ast { namespace visitors { namespace hpx {

CodegenVisitor::CodegenVisitor(const chpl::ast::visitors::hpx::SymbolTable & st, chpl::uast::BuilderResult const& chapelBr, std::string const& chapelFilePath, std::ostream & fstrm)
   : br(chapelBr), indent(0), scope(0), fstrm_(fstrm),
     chplFilePathStr(chapelFilePath),
     symbolTable(st),
     headers(static_cast<std::size_t>(HeaderEnum::HeaderCount), false)
{
}

static void upper(std::string & s) {
   std::transform(std::begin(s), std::end(s), std::begin(s), 
      [](const unsigned char c){ return std::toupper(c); } // correct
   );
}

void CodegenVisitor::emitIndent() const {
   for(std::size_t i = 0; i < indent; ++i) {
         fstrm_ << INDENT; 
   }
}

void CodegenVisitor::emitChapelLine(uast::AstNode const* ast) const {
   emitIndent();

   auto fpth = br.filePath();
   fstrm_ << "#line " << br.idToLocation(ast->id(), fpth).line()  << " \"" << fpth.c_str() << "\"" << std::endl;

   emitIndent();
}

void CodegenVisitor::generateApplicationHeader() {
   const auto pos = chplFilePathStr.find(".");
   std::string prefix = chplFilePathStr.substr(0, pos);

   std::ofstream os{prefix + ".hpp"};
   upper(prefix);

   os << "#pragma once" << std::endl << std::endl;
   os << "#ifndef __" << prefix << "_HPP__" << std::endl;
   os << "#define __" << prefix << "_HPP__" << std::endl << std::endl;

   for(std::size_t i = 0; i < static_cast<std::size_t>(HeaderEnum::HeaderCount); ++i) {
      if(headers[i]) {
         if(i == static_cast<std::size_t>(HeaderEnum::std_vector)) {
            os << "#include <vector>" << std::endl;
         }
         else if(i == static_cast<std::size_t>(HeaderEnum::std_complex)) {
            os << "#include <complex>" << std::endl;
         }
         else if(i == static_cast<std::size_t>(HeaderEnum::std_string)) {
            os << "#include <string>" << std::endl;
         }
      }
   }

   os << std::endl << "#endif" << std::endl;

   os.flush();
   os.close();
}

bool CodegenVisitor::enter(const uast::AstNode * ast) {
   switch(ast->tag()) {
    case asttags::Variable:
    {
       //const std::string prefix = std::string{ast->id().symbolPath().c_str()};
       //const std::string name = std::string{dynamic_cast<NamedDecl const*>(ast)->name().c_str()};
       //std::optional<Symbol> sym = symbolTable.scopedFind(prefix + "." + name);

       std::string ident{dynamic_cast<NamedDecl const*>(ast)->name().c_str()};
       //std::optional<Symbol> sym = symbolTable.scopedFind(ident);
       std::optional<Symbol> sym = symbolTable.find(scope, ident);

       if(!sym->literalAssigned) {
           emit(ast, sym);
           identifier.reset();
       }
       else {
           identifier = ident;
       }
    }
    break;
    case asttags::BoolLiteral:
    case asttags::ImagLiteral:
    case asttags::IntLiteral:
    case asttags::RealLiteral:
    case asttags::UintLiteral:
    case asttags::BytesLiteral:
    case asttags::CStringLiteral:
    case asttags::StringLiteral:
    {
       //std::optional<Symbol> s = symbolTable.scopedFind(*identifier);
       std::optional<Symbol> s = symbolTable.find(scope, *identifier);
       if(s) {
           emit_literal(ast, s);
           identifier.reset();
       }
    }
    break;
    default:
    break;
   }

   return true;
}

void CodegenVisitor::emitArrayKind(Symbol const& s, uast::AstNode const* ast, std::shared_ptr<array_kind> &) {
    headers[static_cast<std::size_t>(HeaderEnum::std_vector)] = true;
     
    std::shared_ptr<array_kind> const& akref =
        std::get<std::shared_ptr<array_kind>>(*s.kind);

    fstrm_ << "std::vector<";
    if(std::holds_alternative<int_kind>(akref->kind)) {
        fstrm_ << "std::int64_t";
    }
    else if(std::holds_alternative<byte_kind>(akref->kind)) {
        fstrm_ << "std::uint8_t";
    }
    else if(std::holds_alternative<real_kind>(akref->kind)) {
        fstrm_ << "double";
    }
    else if(std::holds_alternative<bool_kind>(akref->kind)) {
        fstrm_ << "bool";
    }
    else if(std::holds_alternative<complex_kind>(akref->kind)) {
        headers[static_cast<std::size_t>(HeaderEnum::std_complex)] = true;
        fstrm_ << "std::complex<double>";
    }
    else if(std::holds_alternative<string_kind>(akref->kind)) {
        headers[static_cast<std::size_t>(HeaderEnum::std_string)] = true;
        fstrm_ << "std::string";
    }

    int range_size = 1;
    const auto & rngs = akref->dom.ranges;
    for(const auto & rng : rngs) {
        if(rng.points.size() == 2) {
            range_size *= ( (rng.points[1] - rng.points[0]) + rng.points[0] );
        }
        else if(rng.points.size() == 1) {
            range_size *= rng.points[0];
        }
    }

    if(range_size > 1) {
        fstrm_ << "> " << *s.identifier << "(" << range_size << ");" << std::endl;
    }
    else {
        fstrm_ << "> " << *s.identifier << "{};" << std::endl;
    }
}

template <typename T>
void CodegenVisitor::emit(std::string const& ident, uast::AstNode const* ast, T & t, char const* type) const {
   fstrm_ << type << " " << ident << ";" << std::endl;
}

template <typename Kind, typename T>
void CodegenVisitor::emitLit(std::string const& ident, uast::AstNode const* ast, T & t, char const* type) const {
    if(std::is_same<string_kind, T>::value) {
        fstrm_ << type << " " << ident << " = \"" << dynamic_cast<Kind const*>(ast)->value() << "\";" << std::endl;
    }
    else if(std::is_same<bool_kind, T>::value) {
        fstrm_ << type << " " << ident << " = " << std::boolalpha << dynamic_cast<Kind const*>(ast)->value() << ";" << std::endl;
    }
    else if(std::is_same<real_kind, T>::value) {
        fstrm_ << type << " " << ident << " = " << std::fixed << dynamic_cast<Kind const*>(ast)->value() << ";" << std::endl;
    }
    else {
        fstrm_ << type << " " << ident << " = " << dynamic_cast<Kind const*>(ast)->value() << ";" << std::endl;
    }
}

struct symbol_visitor {

   template <typename T>
   void emit(std::string const& ident, T & kind, char const* type) const {
      visitor.emitChapelLine(ast);
      visitor.emit(ident, ast, kind, type);
   }

   void operator()(byte_kind & kind) const {
      emit(*sym.identifier, kind, "std::uint8_t");
   }
   void operator()(int_kind & kind) const {
      emit(*sym.identifier, kind, "std::int64_t");
   }
   void operator()(real_kind & kind) const {
      emit(*sym.identifier, kind, "double");
   }
   void operator()(complex_kind & kind) const {
      emit(*sym.identifier, kind, "std::complex<double>");
   }
   void operator()(string_kind & kind) const {
      emit(*sym.identifier, kind, "std::string");
   }
   void operator()(bool_kind & kind) const {
      emit(*sym.identifier, kind, "bool");
   }
   void operator()(std::shared_ptr<array_kind> & kind) const {
      visitor.emitChapelLine(ast);
      visitor.emitArrayKind(sym, ast, kind);
   }

   template <typename T>
   constexpr void operator()(T & kind) const {}

   CodegenVisitor& visitor;
   const uast::AstNode * ast;
   Symbol const& sym;
};

bool CodegenVisitor::emit(const uast::AstNode * ast, std::optional<Symbol> & sym) {
   if(sym && sym->kind.has_value()) {
       std::visit(symbol_visitor{*this, ast, *sym}, *sym->kind);
       return true;
   }
   return false;
}

struct symbol_visitor_literal {

   template <typename Kind, typename T>
   void emit(std::string const& ident, T & kind, char const* type) const {
      visitor.emitChapelLine(ast);
      visitor.emitLit<Kind>(ident, ast, kind, type);
   }

   void operator()(byte_kind & kind) const {
      emit<uast::BytesLiteral>(*sym.identifier, kind, "std::uint8_t");
   }
   void operator()(int_kind & kind) const {
      emit<uast::IntLiteral>(*sym.identifier, kind, "std::int64_t");
   }
   void operator()(real_kind & kind) const {
      emit<uast::RealLiteral>(*sym.identifier, kind, "double");
   }
   void operator()(complex_kind & kind) const {
      emit<uast::ImagLiteral>(*sym.identifier, kind, "std::complex<double>");
   }
   void operator()(string_kind & kind) const {
      emit<uast::StringLiteral>(*sym.identifier, kind, "std::string");
   }
   void operator()(bool_kind & kind) const {
      emit<uast::BoolLiteral>(*sym.identifier, kind, "bool");
   }

   template <typename T>
   constexpr void operator()(T & kind) const {}

   CodegenVisitor& visitor;
   const uast::AstNode * ast;
   Symbol const& sym;
};

bool CodegenVisitor::emit_literal(const uast::AstNode * ast, std::optional<Symbol> & sym) {
   if(sym && sym->kind.has_value()) {
       std::visit(symbol_visitor_literal{*this, ast, *sym}, *sym->kind);
       sym.reset();
       return true;
   }
   return false;
}

void CodegenVisitor::exit(const uast::AstNode * ast) {
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
