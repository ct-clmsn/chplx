//  Copyright (c) 2023-2025 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_range.hpp>
#include <chplx/range.hpp>
#include <chplx/tuple.hpp>
#include <chplx/types.hpp>

#include <hpx/config.hpp>
#include <hpx/generator.hpp>

#include <hpx/iterator_support/counting_shape.hpp>

#include <iterator>

namespace chplx {

//-----------------------------------------------------------------------------
// 1D iteration support
template <int N, typename T, bool Stridable>
  requires(N == 1)
hpx::generator<T>
iterate(detail::IteratorGenerator<Domain<N, T, Stridable>> d) noexcept {

  auto size = d.size;
  for (auto ilo = d.first; size-- != 0; ++ilo) {
    co_yield std::get<0>(d.target.orderToIndex(ilo));
  }
}

//-----------------------------------------------------------------------------
// > 2D iteration support
template <int N, typename T, bool Stridable>
  requires(N != 1)
hpx::generator<typename Domain<N, T, Stridable>::indexType>
iterate(detail::IteratorGenerator<Domain<N, T, Stridable>> d) noexcept {

  auto size = d.size;
  for (auto ilo = d.first; size-- != 0; ++ilo) {
    co_yield d.target.orderToIndex(ilo);
  }
}

//-----------------------------------------------------------------------------
template <int N, typename T, bool Stridable>
decltype(auto) iterate(Domain<N, T, Stridable> const &d) noexcept {

  return iterate(detail::IteratorGenerator(d));
}

//-----------------------------------------------------------------------------
template <typename Idx>
decltype(auto) iterate(Domain<1, Idx> const &d) noexcept {

  auto low = std::get<0>(d.low());
  auto high = std::get<0>(d.high());
  if (low <= high)
    return hpx::util::counting_shape(low, high + 1);
  return hpx::util::counting_shape(low, low);
}

//-----------------------------------------------------------------------------
// iteration support for associative domain
template <typename T>
hpx::generator<T>
iterate(detail::IteratorGenerator<AssocDomain<T>> d) noexcept {

  auto size = d.size;
  for (auto it = std::next(d.target.values().begin(), d.first); size-- != 0;
       ++it) {
    co_yield *it;
  }
}

//-----------------------------------------------------------------------------
template <typename T> decltype(auto) iterate(AssocDomain<T> const &d) noexcept {

  return iterate(detail::IteratorGenerator(d, 0, d.size()));
}

} // namespace chplx
