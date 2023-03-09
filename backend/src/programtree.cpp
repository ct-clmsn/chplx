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

#include <variant>
#include <fstream>
#include <sstream>
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

void ScalarDeclarationExpression::emit(std::ostream & os) const {
   std::visit(ScalarDeclarationExpressionVisitor{os}, kind);
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

void ScalarDeclarationLiteralExpression::emit(std::ostream & os) const {
   std::visit(ScalarDeclarationExpressionVisitor{os}, kind);
   os << " " << identifier;
}

void ArrayDeclarationExpression::emit(std::ostream & os) const {
   std::shared_ptr<array_kind> const& akref =
      std::get<std::shared_ptr<array_kind>>(kind);


   os << "std::vector<";

   std::visit(ScalarDeclarationExpressionVisitor{os}, akref->kind);

   int range_size = 1;
   const auto & rngs = akref->dom->ranges;
   for(const auto & rng : rngs) {
      if(rng.points.size() == 2) {
         range_size *= ( (rng.points[1] - rng.points[0]) + rng.points[0] );
      }
      else if(rng.points.size() == 1) {
         range_size *= rng.points[0];
      }
   }

   if(range_size > 1) {
      os << "> " << identifier << "(" << range_size << ");" << std::endl;
   }
   else {
      os << "> " << identifier << "{};" << std::endl;
   }
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
    void operator()(kind_node_type const& n) {
       // does not terminate vector declaration
       // does not process next element in sequence
       //
       os << "std::vector<";
    }

    std::ostream & os;
};

void ArrayDeclarationLiteralExpression::emit(std::ostream & os) const {
   std::shared_ptr<array_kind> const& akref =
      std::get<std::shared_ptr<array_kind>>(kind);

   std::stringstream typelist{}, literallist{};
   std::shared_ptr<kind_node_type> & symref =
      std::get<std::shared_ptr<kind_node_type>>(akref->kind);

   std::vector<kind_types> & children =
      std::get<std::shared_ptr<kind_node_type>>(akref->kind)->children;

   const std::size_t children_sz = children.size();
   std::size_t vec_count = 0;

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
   typelist << "std::vector<";

   for(std::size_t i = 0; i < children_sz; ++i) {
      const bool knt =
         std::holds_alternative<std::shared_ptr<kind_node_type>>(children[i]);

      if(knt) {
         typelist << "std::vector<";
         ++vec_count;
      }
      else {
         std::visit(ArrayDeclarationLiteralExpressionVisitor{typelist}, children[i]);
         break;
      }
   }

   for(std::size_t i = 0; i < vec_count; ++i) {
      typelist << ">";
   }

   typelist << ">";

   // this implementation needs work
   //
   // the next loop cycles through the literal list
   // and generates the correct C++ initializer
   // list for the vector type generated in the loop
   // above
   //
   std::size_t lit = 0;

   literallist << "{";
   for(std::size_t i = 0; i < children_sz; ++i) {
      const bool knt =
         std::holds_alternative<std::shared_ptr<kind_node_type>>(children[i]);
      const bool kntend =
         std::holds_alternative<kind_node_term_type>(children[i]);
      const bool kntbeg =
         (i <= children_sz) ? std::holds_alternative<std::shared_ptr<kind_node_type>>(children[i+1]) : false;
      const bool kntendnxt =
         (i <= children_sz) ? std::holds_alternative<kind_node_term_type>(children[i+1]) : false;

     if(knt) { literallist << "{"; }
     else {

        if(!knt && !kntend) {
           std::visit(ScalarDeclarationLiteralExpressionVisitor{literalValues[lit], literallist}, children[i]);
           ++lit;
        }

        if(!knt && !kntend && !kntbeg && !kntendnxt) {
           literallist << ",";
        }
        else if(!knt && kntend && kntbeg) {
           literallist << "},";
        }
        else if(kntend){
           literallist << "}";
        }
     }
   }

   os << typelist.str() << " " << identifier << " = " << literallist.str() << ";" << std::endl;
}

} /* namespace hpx */ } /* namespace ast */ } /* namespace chpl */
