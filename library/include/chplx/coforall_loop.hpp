//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

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
// coforall loop for tuples
template <typename... Ts, typename F>
void coforallLoop(Tuple<Ts...> &t, F &&f) {

  if constexpr (sizeof...(Ts) != 0) {
    if constexpr (Tuple<Ts...>::isHomogenous()) {

      hpx::parallel::execution::bulk_async_execute(
          hpx::execution::par.executor(), [&](auto val) { f(val); },
          HomogenousTupleRange(t.base()))
          .get();
    } else {

      using table =
          detail::forLoopTable<Tuple<Ts...>, std::decay_t<F>,
                               std::make_index_sequence<sizeof...(Ts)>>;

      hpx::parallel::execution::bulk_async_execute(
          hpx::execution::par.executor(),
          [&](std::size_t i) { table::lookupTable[i](t, f); }, t.size())
          .get();
    }
  }
}

//-----------------------------------------------------------------------------
// coforall loop for ranges
template <typename T, BoundedRangeType BoundedType, bool Stridable, typename F>
void coforallLoop(Range<T, BoundedType, Stridable> const &r, F &&f) {

  hpx::parallel::execution::bulk_async_execute(
      hpx::execution::par.executor(),
      [&](std::size_t idx) { return f(r.orderToIndex(idx)); }, r.size())
      .get();
}

//-----------------------------------------------------------------------------
// coforall loop for domain
template <int N, typename T, bool Stridable, typename F>
void coforallLoop(Domain<N, T, Stridable> const &d, F &&f) {

  hpx::parallel::execution::bulk_async_execute(
      hpx::execution::par.executor(),
      [&](std::size_t idx) { return f(d.orderToIndex(idx)); }, d.size())
      .get();
}

//-----------------------------------------------------------------------------
// forall loop for zippered iteration
template <typename... Rs, typename F>
void coforallLoop(detail::ZipRange<Rs...> const &zr, F &&f) {

  hpx::parallel::execution::bulk_async_execute(
      hpx::execution::par.executor(),
      [&](std::size_t idx) { return f(zr.orderToIndex(idx)); }, zr.size())
      .get();
}

} // namespace chplx
