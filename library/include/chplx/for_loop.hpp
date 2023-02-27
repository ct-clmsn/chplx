//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_range.hpp>
#include <chplx/adapt_tuple.hpp>

namespace chplx {

// for loop for tuples
template <typename... Ts, typename F>
void for_loop(std::tuple<Ts...> &t, F &&f) {

  if constexpr (sizeof...(Ts) != 0) {

    for (auto const &e : chplx::tuple_range(t)) {
      f(e);
    }
  }
}

// for loop for ranges
template <typename T, BoundedRangeType BoundedType, bool Stridable, typename F>
void for_loop(Range<T, BoundedType, Stridable> &r, F &&f) {

  for (auto const &e : chplx::iterate(r)) {
    f(e);
  }
}

} // namespace chplx
