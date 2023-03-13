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

    hpx::execution::experimental::static_chunk_size scs(1);
    auto policy = hpx::parallel::util::adapt_sharing_mode(
        hpx::execution::par,
        hpx::threads::thread_sharing_hint::do_not_combine_tasks);

    if constexpr (Tuple<Ts...>::isHomogenous()) {

      hpx::ranges::for_each(policy.with(scs), HomogenousTupleRange(t.base()),
                            std::forward<F>(f));
    } else {

      using table =
          detail::forLoopTable<Tuple<Ts...>, std::decay_t<F>,
                               std::make_index_sequence<sizeof...(Ts)>>;

      hpx::experimental::for_loop(
          policy.with(scs), 0, t.size(),
          [&](std::size_t i) { table::lookupTable[i](t, f); });
    }
  }
}

//-----------------------------------------------------------------------------
// coforall loop for ranges
template <typename T, BoundedRangeType BoundedType, bool Stridable, typename F>
void coforallLoop(Range<T, BoundedType, Stridable> const &r, F &&f) {

  hpx::execution::experimental::static_chunk_size scs(1);
  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::ranges::experimental::for_loop(
      policy.with(scs), detail::IteratorGenerator(r), std::forward<F>(f));
}

//-----------------------------------------------------------------------------
// coforall loop for domain
template <int N, typename T, bool Stridable, typename F>
void coforallLoop(Domain<N, T, Stridable> const &d, F &&f) {

  hpx::execution::experimental::static_chunk_size scs(1);
  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::ranges::experimental::for_loop(
      policy.with(scs), detail::IteratorGenerator(d), std::forward<F>(f));
}

//-----------------------------------------------------------------------------
// forall loop for zippered iteration
template <typename... Rs, typename F>
void coforallLoop(detail::ZipRange<Rs...> const &zr, F &&f) {

  hpx::execution::experimental::static_chunk_size scs(1);
  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::ranges::experimental::for_loop(
      policy.with(scs), detail::IteratorGenerator(zr), std::forward<F>(f));
}

} // namespace chplx
