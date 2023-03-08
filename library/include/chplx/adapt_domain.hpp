//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_range.hpp>
#include <chplx/range.hpp>
#include <chplx/tuple.hpp>
#include <chplx/types.hpp>

#include <hpx/assert.hpp>
#include <hpx/config.hpp>
#include <hpx/generator.hpp>

#include <cstdlib>

namespace chplx {

//-----------------------------------------------------------------------------
// 1D iteration support
template <int N, typename T, bool Stridable>
  requires(N == 1)
hpx::generator<T> iterate(
    detail::IteratorGenerator<Domain<N, T, Stridable>> d) noexcept {

  auto size = d.size;
  for (auto ilo = d.first; size-- != 0; ++ilo) {
    co_yield std::get<0>(d.target.orderToIndex(ilo));
  }
}

//-----------------------------------------------------------------------------
// > 2D iteration support
template <int N, typename T, bool Stridable>
  requires(N != 1)
hpx::generator<typename Domain<N, T, Stridable>::indexType> iterate(
    detail::IteratorGenerator<Domain<N, T, Stridable>> d) noexcept {

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
template <int N, typename T, bool Stridable>
constexpr bool
empty(detail::IteratorGenerator<Domain<N, T, Stridable>> const &d) noexcept {

  return d.size == 0;
}

template <int N, typename T, bool Stridable>
constexpr auto
size(detail::IteratorGenerator<Domain<N, T, Stridable>> const &d) noexcept {

  return d.size;
}

template <int N, typename T, bool Stridable>
auto subrange(detail::IteratorGenerator<Domain<N, T, Stridable>> const &d,
              std::ptrdiff_t first, std::size_t size) {

  auto result = detail::IteratorGenerator<Domain<N, T, Stridable>>(
      d.target, d.first + first, size);

  return result;
}

} // namespace chplx
