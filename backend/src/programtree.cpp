/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "hpx/programtree.hpp"
#include "chpl/uast/all-uast.h"
#include <fmt/args.h>

#include <variant>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <numeric>
#include <type_traits>

namespace chplx { namespace ast { namespace hpx {

struct ScalarDeclarationExpressionVisitor {
    template<typename T>
    void operator()(T const&) {}

    void operator()(bool_kind const& kind) {
       os << "bool";
    }
    void operator()(byte_kind const&) {
       os << "std::uint8_t";
    }
    void operator()(int_kind const&) {
       os << "std::int64_t";
    }
    void operator()(real_kind const&) {
       os << "double";
    }
    void operator()(complex_kind const&) {
       os << "std::complex<double>";
    }
    void operator()(string_kind const&) {
       os << "std::string";
    }

    std::ostream & os;
};

void VisitQualifierPrefix(std::ostream & os, const int qualifier) {
   if(qualifier == 1) {
      os << "const ";
   }
   else if(qualifier == 4) {
      os << "const ";
   }
}

void VisitQualifierSuffix(std::ostream & os, const int qualifier) {
   if(qualifier == 2) {
      os << " const&";
   }
   else if(qualifier == 3) {
      os << "&";
   }
   else if(qualifier == 6) {
      os << "&";
   }
   else if(qualifier == 7) {
      os << " const&";
   }
   else if(qualifier == 8) {
      os << "&";
   }
   else if(qualifier == 9) {
      os << "&";
   }
}

void ScalarDeclarationExpression::emit(std::ostream & os) const {
   if(!config && -1 < qualifier) {
//      os << "// ";
      VisitQualifierPrefix(os, qualifier);
   }

   std::visit(ScalarDeclarationExpressionVisitor{os}, kind);

   if(!config && -1 < qualifier) {
      VisitQualifierSuffix(os, qualifier);
   }

   os << " " << identifier << ";" << std::endl;
}

void ScalarDeclarationLiteralExpressionVisitor::operator()(bool_kind const&) {
   os << std::boolalpha << bool_kind::value(ast);
}

void ScalarDeclarationLiteralExpressionVisitor::operator()(byte_kind const&) {
   os << byte_kind::value(ast);
}

void ScalarDeclarationLiteralExpressionVisitor::operator()(int_kind const&) {
   os << int_kind::value(ast);
}

void ScalarDeclarationLiteralExpressionVisitor::operator()(real_kind const&) {
   os << std::fixed << real_kind::value(ast);
}

void ScalarDeclarationLiteralExpressionVisitor::operator()(string_kind const&) {
   os << "{\"" << string_kind::value(ast) << "\"}";
}

void ScalarDeclarationLiteralExpressionVisitor::operator()(std::shared_ptr<array_kind> const& n)  {
   std::cout << "ARRK" << std::endl;
}

void ScalarDeclarationLiteralExpression::emit(std::ostream & os) const {
   if(!config && -1 < qualifier) {
//      os << "// ";
      VisitQualifierPrefix(os, qualifier);
   }

   std::visit(ScalarDeclarationExpressionVisitor{os}, kind);
   os << " " << identifier;
}

void ArrayDeclarationExpression::emit(std::ostream & os) const {
   std::shared_ptr<array_kind> const& akref =
      std::get<std::shared_ptr<array_kind>>(kind);

   if(!config && -1 < qualifier) {
      VisitQualifierPrefix(os, qualifier);
   }

   os << "chplx::Array<";
   std::visit(ScalarDeclarationExpressionVisitor{os}, akref->retKind);

   const auto & rngs= std::get<std::shared_ptr<domain_kind>>(akref->args[0].kind)->args;
   os << ", chplx::Domain<" << rngs.size() << "> > " << identifier << "(";

   bool first = true;
   for(const auto & rng : rngs) {
      auto & indices = std::get<std::shared_ptr<range_kind>>(rng.kind)->args;

      if (!first) {
         first = false;
         os << ", ";
      }

      if(indices.size() == 2) {
         os << "chplx::Range(";
         if(std::holds_alternative<int_kind>(indices[0].kind)) {
            if(indices[0].identifier.find("#line") == std::string::npos &&
               indices[0].identifier.find("intlit") == std::string::npos) { 
               os << indices[0].identifier;
            }
            else {
               os << int_kind::value(indices[0].literal[0]);
            }
         }
         os << ", ";

         if(std::holds_alternative<int_kind>(indices[1].kind)) {
            if(indices[1].identifier.find("#line") == std::string::npos &&
               indices[1].identifier.find("intlit") == std::string::npos) { 
               os << indices[1].identifier;
            }
            else {
               os << int_kind::value(indices[1].literal[0]);
            }
         }
         os << ")";
      }
      else if (indices.size() == 1) {
         os << "chplx::Range(";
         if(std::holds_alternative<int_kind>(indices[0].kind)) {
            if(indices[0].identifier.find("#line") == std::string::npos &&
               indices[0].identifier.find("intlit") == std::string::npos) { 
               os << indices[0].identifier;
            }
            else {
               os << int_kind::value(indices[0].literal[0]);
            }
         }
         os << ")";
      }
   }

   os << ");" << std::endl;
}

struct ArrayDeclarationLiteralExpressionVisitor {
    template<typename T>
    void operator()(T const&) {}

    void operator()(bool_kind const& kind) {
       os << "bool";
    }
    void operator()(byte_kind const&) {
       os << "std::uint8_t";
    }
    void operator()(int_kind const&) {
       os << "std::int64_t";
    }
    void operator()(real_kind const&) {
       os << "double";
    }
    void operator()(complex_kind const&) {
       os << "std::complex<double>";
    }
    void operator()(string_kind const&) {
       os << "std::string";
    }
    void operator()(std::shared_ptr<array_kind> const& arr) {
       std::cout << "ARRK" << std::endl;
    }
    void operator()(kind_node_type const& n) {
       // does not terminate vector declaration
       // does not process next element in sequence
       //
       os << "std::vector<";
    }

    std::ostream & os;
};

struct TupleDeclarationExpressionVisitor {
    template<typename T>
    void operator()(T const&) {}

    void operator()(bool_kind const& kind) {
       os << "bool";
    }
    void operator()(byte_kind const&) {
       os << "std::uint8_t";
    }
    void operator()(int_kind const&) {
       os << "std::int64_t";
    }
    void operator()(real_kind const&) {
       os << "double";
    }
    void operator()(complex_kind const&) {
       os << "std::complex<double>";
    }
    void operator()(string_kind const&) {
       os << "std::string";
    }
    void operator()(std::shared_ptr<kind_node_type> const& n) {
       os << "chplx::Tuple<";
       for(std::size_t i = 0; i < n->children.size(); ++i) {
          std::visit(*this, n->children[i]);
          if(0 < i && i != n->children.size()-1) {
             os << ',';
          }
       }
       os << ">";
    }

    std::ostream & os;
};

void ArrayDeclarationLiteralExpression::emit(std::ostream & os) const {
   std::shared_ptr<array_kind> const& akref =
      std::get<std::shared_ptr<array_kind>>(kind);
   std::stringstream typelist{}, literallist{};

   auto & kindtypes =
      std::get<std::shared_ptr<kind_node_type>>(akref->retKind)->children;

   const std::size_t children_sz = kindtypes.size();

   std::size_t vec_count = 0;

   if(-1 < qualifier) {
      VisitQualifierPrefix(os, qualifier);
   }

   // the following 2 loops could be performed in
   // parallel
   //
   // this implementation needs work
   //
   // the following loop captures the nesting
   // depth and gets the first `kind_type`
   // stored in the literal array then the
   // loop needs to terminate
   //
   typelist << "chplx::Array<";

   bool skip = false;
   std::size_t domcount = 0;
   std::size_t prev_idx = 0;
   for(auto & kt : kindtypes) {
      if(std::holds_alternative<std::shared_ptr<array_kind>>(kt) &&
         (prev_idx == 0 || prev_idx == 18)) {
         ++domcount;
         prev_idx = kt.index();
      }
      else if(!skip) {
         std::visit(ArrayDeclarationLiteralExpressionVisitor{typelist}, kt);
         skip = true;
         prev_idx = kt.index();
      }
   }

   typelist << ", chplx::Domain<" << domcount << ">";

   typelist << ">";

   // this implementation needs work
   //
   // the next loop cycles through the literal list
   // and generates the correct C++ initializer
   // list for the vector type generated in the loop
   // above
   //
   std::size_t lit = 0;
   std::size_t bracec = 0;

   auto & domk = std::get<std::shared_ptr<domain_kind>>(literalValues[0]->kind);

   for(std::size_t i = 0; i < kindtypes.size(); ++i) {
      const bool end = i == kindtypes.size()-1;
      const bool knt =
         std::holds_alternative<std::shared_ptr<array_kind>>(kindtypes[i]);
      const bool kntend =
         i < kindtypes.size()-1 && std::holds_alternative<std::shared_ptr<array_kind>>(kindtypes[i+1]);

     if(knt) {
        if(1 < i && kntend) {
           literallist.seekp(-1, std::ios_base::end);
           literallist << "},{";
        }
        else {
           literallist << "{";
           ++bracec;
        }
     }
     else {
      if(!std::holds_alternative<std::shared_ptr<array_kind>>(kindtypes[i])) {
         if(std::holds_alternative<std::shared_ptr<func_kind>>(kindtypes[i])) {
            auto & fk = std::get<std::shared_ptr<func_kind>>(kindtypes[i]);

            const auto pos = domk->args[lit].identifier.find('|');
            literallist << domk->args[lit].identifier.substr(0, pos) << '('; 
            for(std::size_t it = 0; it < fk->args.size(); ++it) {
            }
            literallist << ')';
         }
         else {
           if(domk->args[lit].identifier.find("#line") != std::string::npos) {
              std::visit(ScalarDeclarationLiteralExpressionVisitor{domk->args[lit].literal[0], literallist}, kindtypes[i]);
              ++lit;
           }
           else if(domk->args[lit].identifier.find("intlit_") != std::string::npos) {
              std::visit(ScalarDeclarationLiteralExpressionVisitor{domk->args[lit].literal[0], literallist}, kindtypes[i]);
              ++lit;
           }
        }

        if(i < kindtypes.size()-1 && std::holds_alternative<std::shared_ptr<array_kind>>(kindtypes[i+1])) {
           literallist << "}";
           --bracec;
        }

        if(!end) {
           literallist << ",";
        }
      }

      if(end) {
         literallist << "}";
         --bracec;
      }
     }
   }
   
   for(std::size_t i = 0; i < bracec; ++i) {
      literallist << "}";
   }

   os << typelist.str() << " " << identifier << "(" << literallist.str() << ");" << std::endl;
}

void TupleDeclarationExpression::emit(std::ostream & os) const {
   std::shared_ptr<tuple_kind> const& akref =
      std::get<std::shared_ptr<tuple_kind>>(kind);
   std::stringstream typelist{};

   std::visit(TupleDeclarationExpressionVisitor{typelist}, akref->retKind);

   os << typelist.str() << " " << identifier << "{};" << std::endl;
}

void TupleDeclarationLiteralExpression::emit(std::ostream & os) const {
   std::shared_ptr<tuple_kind> const& akref =
      std::get<std::shared_ptr<tuple_kind>>(kind);
   std::stringstream typelist{}, literallist{};

   if(identifier.find("tuple_") == std::string::npos) {
      typelist << "chplx::Tuple<";
      for(std::size_t i = 0; i < akref->args.size(); ++i) {
         auto & elem = akref->args[i];
         std::visit(ScalarDeclarationExpressionVisitor{typelist}, elem.kind);

         if(!elem.isIntegralKind() && 0 < elem.literal.size() && elem.identifier.find("lit_") != std::string::npos) {
            std::visit(ScalarDeclarationLiteralExpressionVisitor{elem.literal[0], literallist}, elem.kind);
            if(i != akref->args.size()-1) {
               typelist << ',';
               literallist << ',';
            }
         }
      }
      typelist << ">";

      os << typelist.str() << " " << identifier << "{" << literallist.str() << "};" << std::endl;
   }
   else {
      typelist << "chplx::Tuple<";
      for(std::size_t i = 0; i < literalValues.size(); ++i) {
         Symbol const& elem = literalValues[i];
         std::visit(ScalarDeclarationExpressionVisitor{typelist}, elem.kind);
         std::visit(ScalarDeclarationLiteralExpressionVisitor{elem.literal[0], literallist}, elem.kind);
         if(i != literalValues.size()-1) {
            typelist << ',';
            literallist << ',';
         }
      }
      typelist << ">";
      
      os << typelist.str() << " " << identifier << "{" << literallist.str() << "}";
   }
}

void ArrayDeclarationExprExpression::emit(std::ostream & os) const {
   os << "auto " << identifier;
}

void TupleDeclarationExprExpression::emit(std::ostream & os) const {
   os << "auto " << identifier;
}

struct ArgumentVisitor {
    template<typename T>
    void operator()(T const&) {}
    void operator()(bool_kind const&) {
       os << std::boolalpha << bool_kind::value(node);
    }
    void operator()(int_kind const&) {
       os << int_kind::value(node);
    }
    void operator()(real_kind const&) {
       os << std::fixed << real_kind::value(node);
    }
    void operator()(string_kind const&) {
       os << '\"' << string_kind::value(node) << '\"';
    }
    void operator()(LiteralExpression const& e) {
       node = e.value;
       std::visit(*this, e.kind);
    }
    void operator()(VariableExpression const& e) {
       os << e.sym->identifier;
    }
    void operator()(std::shared_ptr<FunctionCallExpression> const& node) {
       node->emit(os);
    }
    void operator()(std::shared_ptr<BinaryOpExpression> const& node) {
       if(node->statements.size() == 2) {
          const bool rop = std::holds_alternative<std::shared_ptr<BinaryOpExpression>>(node->statements[1]);
          const bool lop = std::holds_alternative<std::shared_ptr<BinaryOpExpression>>(node->statements[0]);

          if(rop) {
             os << "( ";
          }
          std::visit(*this, node->statements[1]);

          if(rop) {
             os << " )";
          }

          os << ' ' << node->op << ' ';

          if(lop) {
             os << "( ";
          }
          std::visit(*this, node->statements[0]);

          if(lop) {
             os << " )";
          }
       }
       else if(node->statements.size() == 1) {
          os << ' ' << node->op;
          std::visit(*this, node->statements[0]);
       }
    }

    uast::AstNode const* node;
    std::stringstream os; 
};

void ReturnExpression::emit(std::ostream & os) const {
    ArgumentVisitor v{nullptr, std::stringstream{}};
    std::string retfmt{"return {};"};
    fmt::dynamic_format_arg_store<fmt::format_context> store;
    std::visit(v, statement[0]);
    store.push_back(v.os.str());
    os << fmt::vformat(retfmt, store) << std::endl;
}

void FunctionCallExpression::emit(std::ostream & os) const {
   if(std::holds_alternative<std::shared_ptr<cxxfunc_kind>>(symbol.kind)) {

      if(symbol.identifier != "inlinecxx") return;

      const std::size_t args_sz = arguments.size();
      if(0 < args_sz) {
         std::string cxx_fmt_str{string_kind::value(std::get<LiteralExpression>(arguments[1]).value)};
         fmt::dynamic_format_arg_store<fmt::format_context> store;

         for(std::size_t i = 2; i < args_sz; ++i) {
            Statement const& stmt = arguments[i];
            ArgumentVisitor v{nullptr, std::stringstream{}};
            std::visit(v, stmt);
            store.push_back(v.os.str());
         }

         os << fmt::vformat(cxx_fmt_str, store) << std::endl;
      }
   }
   else if(std::holds_alternative<std::shared_ptr<tuple_kind>>(symbol.kind)) {
      const std::size_t args_sz = arguments.size();
      std::string fn_fmt_str{};
      fmt::dynamic_format_arg_store<fmt::format_context> store;

      for(std::size_t i = 1; i < args_sz; ++i) {
         fn_fmt_str += (i == 1) ? "{}" : ", {}";
         Statement const& stmt = arguments[i];
         ArgumentVisitor v{nullptr, std::stringstream{}};
         std::visit(v, stmt);
         store.push_back(v.os.str());
      }

      os << symbol.identifier  << '(' << fmt::vformat(fn_fmt_str, store) << ')';
   }
   else if(std::holds_alternative<std::shared_ptr<func_kind>>(symbol.kind) ||
           std::holds_alternative<std::shared_ptr<array_kind>>(symbol.kind)) {
      const std::size_t args_sz = arguments.size();
      std::string fn_fmt_str{};
      fmt::dynamic_format_arg_store<fmt::format_context> store;
      if((symbol.identifier) == "[]" && 0 < args_sz) {
          ArgumentVisitor v{nullptr, std::stringstream{}};
          std::visit(v, arguments[0]);

          for(std::size_t i = 1; i < args_sz; ++i) {
             fn_fmt_str += (i == 1) ? "{}" : ", {}";
             Statement const& stmt = arguments[i];
             ArgumentVisitor v{nullptr, std::stringstream{}};
             std::visit(v, stmt);
             store.push_back(v.os.str());
          }

          os << v.os.str()  << '(' << fmt::vformat(fn_fmt_str, store) << ')';
      }
      else {
         for(std::size_t i = 1; i < args_sz; ++i) {
            fn_fmt_str += (i == 1) ? "{}" : ", {}";
            Statement const& stmt = arguments[i];
            ArgumentVisitor v{nullptr, std::stringstream{}};
            std::visit(v, stmt);
            store.push_back(v.os.str());
         }

         auto pos = symbol.identifier.find('|');
         os << symbol.identifier.substr( 0, (pos == std::string::npos) ? symbol.identifier.size() : pos ) << '(' << fmt::vformat(fn_fmt_str, store) << ")";
      }
   }
}

void FunctionDeclarationExpression::emit(std::ostream & os) const {
}

void VariableExpression::emit(std::ostream & os) const {
    os << sym->identifier;
};

void LiteralExpression::emit(std::ostream & os) const {
    ArgumentVisitor v{value, std::stringstream{}};
    std::visit(v, kind);
    os << v.os.str();
};

void UnaryOpExpression::emit(std::ostream & os) const {
    // idx[0] is an operation
    //

    // idx[1] is either a literal or variable
    //
    ArgumentVisitor v{nullptr, std::stringstream{}};
    std::visit(v, statements[1]);

    os << op << v.os.str();
}

void BinaryOpExpression::emit(std::ostream & os) const {
}

void ScalarDeclarationExprExpression::emit(std::ostream & os) const {
   os << "auto " << identifier;
}

void TernaryOpExpression::emit(std::ostream & os) const {
}

void ForLoopExpression::emit(std::ostream & os) const {
}

void RecordDeclarationExpression::emit(std::ostream & os) const {
}

void ClassDeclarationExpression::emit(std::ostream & os) const {
}

} /* namespace hpx */ } /* namespace ast */ } /* namespace chpl */
