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
      os << "> " << identifier << "(" << range_size << ");" << std::endl;
   }
   else {
      os << "> " << identifier << "{};" << std::endl;
   }
}

void ArrayDeclarationLiteralExpression::emit(std::ostream & os) const {
}

} /* namespace hpx */ } /* namespace ast */ } /* namespace chpl */
