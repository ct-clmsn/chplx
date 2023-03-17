//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_domain.hpp>
#include <chplx/adapt_range.hpp>
#include <chplx/adapt_tuple.hpp>
#include <chplx/assoc_domain.hpp>
#include <chplx/detail/iterator_generator.hpp>
#include <chplx/domain.hpp>
#include <chplx/range.hpp>
#include <chplx/tuple.hpp>
#include <chplx/zip.hpp>

#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>

namespace chplx {

//-----------------------------------------------------------------------------
// coforall loop for tuples
template <typename... Ts, typename F, typename... Args>
void coforall(Tuple<Ts...> &t, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  if constexpr (sizeof...(Ts) != 0) {
    if constexpr (Tuple<Ts...>::isHomogenous()) {

      hpx::parallel::execution::bulk_async_execute(
          policy.executor(),
          [&](auto val, Args &&...fargs) {
            f(val, std::forward<Args>(fargs)...);
          },
          HomogenousTupleRange(t.base()), std::forward<Args>(args)...)
          .get();
    } else {

      using table =
          detail::forLoopTable<Tuple<Ts...>, std::decay_t<F>,
                               std::make_index_sequence<sizeof...(Ts)>,
                               Args...>;

      hpx::parallel::execution::bulk_async_execute(
          policy.executor(),
          [&](std::size_t i, Args &&...fargs) {
            table::lookupTable[i](t, f, std::forward<Args>(fargs)...);
          },
          t.size(), std::forward<Args>(args)...)
          .get();
    }
  }
}

//-----------------------------------------------------------------------------
// coforall loop for ranges
template <typename T, BoundedRangeType BoundedType, bool Stridable, typename F,
          typename... Args>
void coforall(Range<T, BoundedType, Stridable> const &r, F &&f,
              Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, Args &&...fargs) {
        return f(r.orderToIndex(idx), std::forward<Args>(fargs)...);
      },
      r.size(), std::forward<Args>(args)...)
      .get();
}

//-----------------------------------------------------------------------------
// coforall loop for domain
template <int N, typename T, bool Stridable, typename F, typename... Args>
void coforall(Domain<N, T, Stridable> const &d, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, Args &&...fargs) {
        return f(d.orderToIndex(idx), std::forward<Args>(fargs)...);
      },
      d.size(), std::forward<Args>(args)...)
      .get();
}

//-----------------------------------------------------------------------------
// coforall loop for associative domain
template <typename T, typename F, typename... Args>
void coforall(AssocDomain<T> const &d, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, Args &&...fargs) {
        return f(d.orderToIndex(idx), std::forward<Args>(fargs)...);
      },
      d.size(), std::forward<Args>(args)...)
      .get();
}

//-----------------------------------------------------------------------------
// forall loop for zippered iteration
template <typename... Rs, typename F, typename... Args>
void coforall(detail::ZipRange<Rs...> const &zr, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, Args &&...fargs) {
        return f(zr.orderToIndex(idx), std::forward<Args>(fargs)...);
      },
      zr.size(), std::forward<Args>(args)...)
      .get();
}

} // namespace chplx
