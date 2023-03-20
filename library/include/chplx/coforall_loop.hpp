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
#include <chplx/types.hpp>
#include <chplx/zip.hpp>

#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>
#include <hpx/future.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace chplx {

//-----------------------------------------------------------------------------
// coforall loop for tuples
namespace detail {
template <typename Tuple, typename F, typename... Args>
void coforall(Tuple &t, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  using base_tuple = typename Tuple::base_type;
  if constexpr (std::tuple_size_v<base_tuple> != 0) {
    if constexpr (Tuple::isHomogenous()) {

      hpx::wait_all(hpx::parallel::execution::bulk_async_execute(
          policy.executor(),
          [&](auto val, auto &&...fargs) {
            f(val, std::forward<decltype(args)>(fargs)...);
          },
          HomogenousTupleRange(t.base()),
          detail::task_intent<std::decay_t<Args>>::call(
              std::forward<Args>(args))...));
    } else {

      using table = detail::forLoopTable<
          Tuple, std::decay_t<F>,
          std::make_index_sequence<std::tuple_size_v<base_tuple>>, Args...>;

      hpx::wait_all(hpx::parallel::execution::bulk_async_execute(
          policy.executor(),
          [&](std::size_t i, auto &&...fargs) {
            table::lookupTable[i](t, f, std::forward<decltype(args)>(fargs)...);
          },
          t.size(),
          detail::task_intent<std::decay_t<Args>>::call(
              std::forward<Args>(args))...));
    }
  }
}
} // namespace detail

template <typename... Ts, typename F, typename... Args>
void coforall(Tuple<Ts...> &t, F &&f, Args &&...args) {

  detail::coforall(t, std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename... Ts, typename F, typename... Args>
void coforall(Tuple<Ts...> const &t, F &&f, Args &&...args) {

  detail::coforall(t, std::forward<F>(f), std::forward<Args>(args)...);
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

  hpx::wait_all(hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, auto &&...fargs) {
        return f(r.orderToIndex(idx), std::forward<decltype(args)>(fargs)...);
      },
      r.size(),
      detail::task_intent<std::decay_t<Args>>::call(
          std::forward<Args>(args))...));
}

//-----------------------------------------------------------------------------
// coforall loop for domain
template <int N, typename T, bool Stridable, typename F, typename... Args>
void coforall(Domain<N, T, Stridable> const &d, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::wait_all(hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, auto &&...fargs) {
        return f(d.orderToIndex(idx), std::forward<decltype(args)>(fargs)...);
      },
      d.size(),
      detail::task_intent<std::decay_t<Args>>::call(
          std::forward<Args>(args))...));
}

//-----------------------------------------------------------------------------
// coforall loop for associative domain
template <typename T, typename F, typename... Args>
void coforall(AssocDomain<T> const &d, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::wait_all(hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, auto &&...fargs) {
        return f(d.orderToIndex(idx), std::forward<decltype(args)>(fargs)...);
      },
      d.size(),
      detail::task_intent<std::decay_t<Args>>::call(
          std::forward<Args>(args))...));
}

//-----------------------------------------------------------------------------
// forall loop for zippered iteration
template <typename... Rs, typename F, typename... Args>
void coforall(detail::ZipRange<Rs...> const &zr, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::wait_all(hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, auto &&...fargs) {
        return f(zr.orderToIndex(idx), std::forward<decltype(args)>(fargs)...);
      },
      zr.size(),
      detail::task_intent<std::decay_t<Args>>::call(
          std::forward<Args>(args))...));
}

//-----------------------------------------------------------------------------
// forall loop for aray iteration
template <typename Domain, typename T, typename F, typename... Args>
void coforall(Array<Domain, T> const &a, F &&f, Args &&...args) {

  auto policy = hpx::parallel::util::adapt_sharing_mode(
      hpx::execution::par,
      hpx::threads::thread_sharing_hint::do_not_combine_tasks);

  hpx::wait_all(hpx::parallel::execution::bulk_async_execute(
      policy.executor(),
      [&](std::size_t idx, auto &&...fargs) {
        return f(a.orderToIndex(idx), std::forward<decltype(args)>(fargs)...);
      },
      a.size(),
      detail::task_intent<std::decay_t<Args>>::call(
          std::forward<Args>(args))...));
}

} // namespace chplx
