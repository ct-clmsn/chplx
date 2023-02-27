//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_range.hpp>
#include <chplx/adapt_tuple.hpp>

#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>

namespace chplx {

// forall loop for tuples
template <typename... Ts, typename F>
void coforall_loop(std::tuple<Ts...> &t, F &&f) {

  if constexpr (sizeof...(Ts) != 0) {

    hpx::execution::experimental::static_chunk_size scs(1);
    auto policy = hpx::parallel::util::adapt_sharing_mode(
        hpx::execution::par,
        hpx::threads::thread_sharing_hint::do_not_combine_tasks);
    hpx::ranges::for_each(policy.with(scs), chplx::tuple_range(t),
                          std::forward<F>(f));
  }
}

// forall loop for ranges
template <typename T, BoundedRangeType BoundedType, bool Stridable, typename F>
void coforall_loop(Range<T, BoundedType, Stridable> &r, F &&f) {

  hpx::execution::experimental::static_chunk_size scs(1);
  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);
  hpx::ranges::experimental::for_loop(policy.with(scs), r, std::forward<F>(f));
}

} // namespace chplx
