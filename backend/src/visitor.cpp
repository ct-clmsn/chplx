/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "hpx/visitor.hpp"
#include "chpl/uast/all-uast.h"

#include <variant>
#include <fstream>
#include <cctype>
#include <numeric>

#define INDENT "    "

using namespace chpl::uast;
 
namespace chpl { namespace ast { namespace visitors { namespace hpx {

Visitor::Visitor(chpl::uast::BuilderResult const& chapel_br, std::string const& chapel_file_path_str, std::ostream & fstrm)
   : br(chapel_br), indent(0), fstrm_(fstrm),
     chpl_file_path_str(chapel_file_path_str),
     sym(), symnode(), symboltable(),
     headers(static_cast<std::size_t>(HeaderEnum::HeaderCount), false)
{
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

   Symbol tmplsym{};
   tmplsym.kind = template_kind{};
   tmplsym.identifier = "?";
   symboltable.addEntry(*(tmplsym.identifier), tmplsym);
}

static inline void upper(std::string & s) {
   std::transform(std::begin(s), std::end(s), std::begin(s), 
      [](const unsigned char c){ return std::toupper(c); } // correct
   );
}

void Visitor::generateSourceHeader() {

   fstrm_ << "#include <hpx/hpx_init.hpp>" << std::endl << std::endl;

   {
      const auto pos = chpl_file_path_str.find(".");
      const std::string prefix = chpl_file_path_str.substr(0, pos);
      fstrm_ << "#include \"" << prefix << ".hpp\"" << std::endl;
   }
}

void Visitor::generateSourceFooter() {
   fstrm_ << std::endl
          << "int main(int argc, char ** argv) {" << std::endl
          << "    return hpx::init(argc, argv);" << std::endl
          << "}";
}

void Visitor::generate_hpx_main_beg() {
      fstrm_ << std::endl
             << "int hpx_main(int argc, char ** argv) {" << std::endl
             << std::endl;
}

void Visitor::generate_hpx_main_end() {
   fstrm_ << std::endl
          << "    return hpx::finalize();" << std::endl
          << "}" << std::endl;
}

void Visitor::generateApplicationHeader() {
   const auto pos = chpl_file_path_str.find(".");
   std::string prefix = chpl_file_path_str.substr(0, pos);

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

bool Visitor::enter(const uast::AstNode * ast) {
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
             auto fsym = symboltable.find(identifier_str);
             if(fsym.has_value()) {
                std::get<std::shared_ptr<array_kind>>( *(sym->kind))->kind = *(fsym->kind);
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
    break;
    case asttags::ImagLiteral:
    break;
    case asttags::IntLiteral:
    {
        if(sym && sym->kind.has_value()) {
           if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->kind))) {
              std::shared_ptr<array_kind> & symref = std::get<std::shared_ptr<array_kind>>(*(sym->kind));
              symref->dom.ranges[symref->dom.ranges.size()-1].points.push_back( dynamic_cast<IntLiteral const*>(ast)->value() );
           }
        }
    }
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
    {
//std::cout << "variable\tname?\t" << reinterpret_cast<NamedDecl const*>(ast)->name().c_str() << "\tis_init?\t" << (reinterpret_cast<Variable const*>(ast)->initExpression() == nullptr) << "\tis_type?\t" << (reinterpret_cast<Variable const*>(ast)->typeExpression() == nullptr) << std::endl;
       Symbol var{};
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

void Visitor::exit(const uast::AstNode * ast) {
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
    {
       if(sym.has_value()) {
          if(sym->kind.has_value()) {

             if(std::holds_alternative<std::shared_ptr<array_kind>>(*(sym->kind))) {
                headers[static_cast<std::size_t>(HeaderEnum::std_vector)] = true;
                auto & symref = std::get<std::shared_ptr<array_kind>>(*(sym->kind));

                for(std::size_t i = 0; i < indent; ++i) {
                    fstrm_ << INDENT; 
                }
     
                auto fpth = br.filePath();
                fstrm_ << "#line " << br.idToLocation(ast->id(), fpth).line()  << " \"" << fpth.c_str() << "\"" << std::endl;

                for(std::size_t i = 0; i < indent; ++i) {
                    fstrm_ << INDENT; 
                }
                fstrm_ << "std::vector<";

                if(std::holds_alternative<int_kind>(symref->kind)) {
                   fstrm_ << "std::int64_t";
                }
                else if(std::holds_alternative<byte_kind>(symref->kind)) {
                   fstrm_ << "std::uint8_t";
                }
                else if(std::holds_alternative<real_kind>(symref->kind)) {
                   fstrm_ << "double";
                }
                else if(std::holds_alternative<complex_kind>(symref->kind)) {
                   headers[static_cast<std::size_t>(HeaderEnum::std_complex)] = true;
                   fstrm_ << "std::complex<double>";
                }
                else if(std::holds_alternative<string_kind>(symref->kind)) {
                   headers[static_cast<std::size_t>(HeaderEnum::std_string)] = true;
                   fstrm_ << "std::string";
                }
                assert( sym->identifier.has_value() );

                int range_size = 1;
                for(const auto & rng : symref->dom.ranges) {
                   if(rng.points.size() == 2) {
                      range_size *= ( (rng.points[1] - rng.points[0]) + rng.points[0] );
                   }
                   else if(rng.points.size() == 1) {
                      range_size *= rng.points[0];
                   }
                }

                if(range_size > 1) {
                   fstrm_ << "> " << (*(sym->identifier)) << "(" << range_size << ");" << std::endl;
                }
                else {
                   fstrm_ << "> " << (*(sym->identifier)) << "{};" << std::endl;
                }
             }
          }

          assert( sym->identifier.has_value() );

          auto lusym = symboltable.scopedFind((*(sym->identifier)));
          if(!lusym) {
             symboltable.addEntry((*(sym->identifier)), *sym);
          }
          else {
             std::cerr << "chplx : " << (*(sym->identifier)) << " identifier already defined in current scope" << std::endl;
             return;
          }

          sym.reset();
          symnode.reset();
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
