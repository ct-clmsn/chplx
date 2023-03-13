//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_domain.hpp>
#include <chplx/adapt_range.hpp>
#include <chplx/adapt_tuple.hpp>
#include <chplx/detail/iterator_generator.hpp>
#include <chplx/range.hpp>
#include <chplx/tuple.hpp>
#include <chplx/zip.hpp>

#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>

namespace chplx {

//-----------------------------------------------------------------------------
// forall loop for tuples
template <typename... Ts, typename F> void forallLoop(Tuple<Ts...> &t, F &&f) {

  if constexpr (sizeof...(Ts) != 0) {

    if constexpr (Tuple<Ts...>::isHomogenous()) {

      hpx::ranges::for_each(hpx::execution::par, HomogenousTupleRange(t.base()),
                            std::forward<F>(f));
    } else {

      using table =
          detail::forLoopTable<Tuple<Ts...>, F,
                               std::make_index_sequence<sizeof...(Ts)>>;

      hpx::experimental::for_loop(
          hpx::execution::par, 0, t.size(),
          [&](std::size_t i) { table::lookupTable[i](t, f); });
    }
  }
}

//-----------------------------------------------------------------------------
// forall loop for ranges
template <typename T, BoundedRangeType BoundedType, bool Stridable, typename F>
void forallLoop(Range<T, BoundedType, Stridable> const &r, F &&f) {

  hpx::ranges::experimental::for_loop(
      hpx::execution::par, detail::IteratorGenerator(r), std::forward<F>(f));
}

//-----------------------------------------------------------------------------
// forall loop for domain
template <int N, typename T, bool Stridable, typename F>
void forallLoop(Domain<N, T, Stridable> const &d, F &&f) {

  hpx::ranges::experimental::for_loop(
      hpx::execution::par, detail::IteratorGenerator(d), std::forward<F>(f));
}

//-----------------------------------------------------------------------------
// forall loop for zippered iteration
template <typename... Rs, typename F>
void forallLoop(detail::ZipRange<Rs...> const &zr, F &&f) {

  hpx::ranges::experimental::for_loop(
      hpx::execution::par, detail::IteratorGenerator(zr), std::forward<F>(f));
}

} // namespace chplx
