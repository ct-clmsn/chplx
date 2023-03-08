//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_range.hpp>
#include <chplx/adapt_tuple.hpp>
#include <chplx/range.hpp>
#include <chplx/tuple.hpp>

namespace chplx {

namespace detail {

// 'iterate'  over non-homogenous tuple
template <typename... Ts, typename F, std::size_t... Is>
void forLoop(Tuple<Ts...> &t, F &&f, std::index_sequence<Is...>) {

  (f(std::get<Is>(t)), ...);
}
} // namespace detail

// for loop for tuples
template <typename... Ts, typename F> void forLoop(Tuple<Ts...> &t, F &&f) {

  if constexpr (sizeof...(Ts) != 0) {

    if constexpr (Tuple<Ts...>::isHomogenous()) {
      for (auto const &e : t) {
        f(e);
      }
    }
  } else {

    detail::forLoop(t, std::forward<F>(f),
                    std::make_index_sequence<sizeof...(Ts)>{});
  }
}

// for loop for ranges
template <typename T, BoundedRangeType BoundedType, bool Stridable, typename F>
void forLoop(Range<T, BoundedType, Stridable> &r, F &&f) {

  for (auto const &e : chplx::iterate(r)) {
    f(e);
  }
}

} // namespace chplx
