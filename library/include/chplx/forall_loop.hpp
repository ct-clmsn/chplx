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

#include <cstddef>
#include <tuple>

namespace chplx {

//-----------------------------------------------------------------------------
// forall loop for tuples
namespace detail {

template <typename Tuple, typename F, typename... Args>
void forall(Tuple &t, F &&f, Args &&...args) {

  using base_tuple = typename Tuple::base_type;
  if constexpr (std::tuple_size_v<base_tuple> != 0) {

    if constexpr (Tuple::isHomogenous()) {

      hpx::ranges::for_each(
          hpx::execution::par, HomogenousTupleRange(t.base()),
          [&, ... fargs = detail::task_intent<std::decay_t<Args>>::call(
                  std::forward<Args>(args))]<typename Arg>(Arg &&value) {
            f(std::forward<Arg>(value),
              hpx::util::decay_unwrap<decltype(fargs)>::call(fargs)...);
          });
    } else {

      using table = detail::forLoopTable<
          Tuple, F, std::make_index_sequence<std::tuple_size_v<base_tuple>>,
          Args...>;

      hpx::experimental::for_loop(
          hpx::execution::par, 0, t.size(),
          [&, ... fargs = detail::task_intent<std::decay_t<Args>>::call(
                  std::forward<Args>(args))](std::size_t i) mutable {
            table::lookupTable[i](
                t, f, hpx::util::decay_unwrap<decltype(fargs)>::call(fargs)...);
          });
    }
  }
}
} // namespace detail

template <typename... Ts, typename F, typename... Args>
void forall(Tuple<Ts...> &t, F &&f, Args &&...args) {

  detail::forall(t, std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename... Ts, typename F, typename... Args>
void forall(Tuple<Ts...> const &t, F &&f, Args &&...args) {

  detail::forall(t, std::forward<F>(f), std::forward<Args>(args)...);
}

//-----------------------------------------------------------------------------
// forall loop for ranges
template <typename T, BoundedRangeType BoundedType, bool Stridable, typename F,
          typename... Args>
void forall(Range<T, BoundedType, Stridable> const &r, F &&f, Args &&...args) {

  hpx::ranges::experimental::for_loop(
      hpx::execution::par, detail::IteratorGenerator(r),
      [&, ... fargs = detail::task_intent<std::decay_t<Args>>::call(
              std::forward<Args>(args))]<typename Arg>(Arg &&value) mutable {
        f(std::forward<Arg>(value),
          hpx::util::decay_unwrap<decltype(fargs)>::call(fargs)...);
      });
}

//-----------------------------------------------------------------------------
// forall loop for domain
template <int N, typename T, bool Stridable, typename F, typename... Args>
void forall(Domain<N, T, Stridable> const &d, F &&f, Args &&...args) {

  hpx::ranges::experimental::for_loop(
      hpx::execution::par, detail::IteratorGenerator(d),
      [&, ... fargs = detail::task_intent<std::decay_t<Args>>::call(
              std::forward<Args>(args))]<typename Arg>(Arg &&value) mutable {
        f(std::forward<Arg>(value),
          hpx::util::decay_unwrap<decltype(fargs)>::call(fargs)...);
      });
}

//-----------------------------------------------------------------------------
// forall loop for associative domains
template <typename T, typename F, typename... Args>
void forall(AssocDomain<T> const &d, F &&f, Args &&...args) {

  hpx::ranges::experimental::for_loop(
      hpx::execution::par, detail::IteratorGenerator(d, 0, d.size()),
      [&, ... fargs = detail::task_intent<std::decay_t<Args>>::call(
              std::forward<Args>(args))]<typename Arg>(Arg &&value) mutable {
        f(std::forward<Arg>(value),
          hpx::util::decay_unwrap<decltype(fargs)>::call(fargs)...);
      });
}

//-----------------------------------------------------------------------------
// forall loop for zippered iteration
template <typename... Rs, typename F, typename... Args>
void forall(detail::ZipRange<Rs...> const &zr, F &&f, Args &&...args) {

  hpx::ranges::experimental::for_loop(
      hpx::execution::par, detail::IteratorGenerator(zr),
      [&, ... fargs = detail::task_intent<std::decay_t<Args>>::call(
              std::forward<Args>(args))]<typename Arg>(Arg &&value) mutable {
        f(std::forward<Arg>(value),
          hpx::util::decay_unwrap<decltype(fargs)>::call(fargs)...);
      });
}

//-----------------------------------------------------------------------------
// forall loop for array iteration
template <typename T, typename Domain, typename F, typename... Args>
void forall(Array<T, Domain> const &a, F &&f, Args &&...args) {

  hpx::ranges::experimental::for_loop(
      hpx::execution::par, detail::IteratorGenerator(a),
      [&, ... fargs = detail::task_intent<std::decay_t<Args>>::call(
              std::forward<Args>(args))]<typename Arg>(Arg &&value) mutable {
        f(std::forward<Arg>(value),
          hpx::util::decay_unwrap<decltype(fargs)>::call(fargs)...);
      });
}

} // namespace chplx
