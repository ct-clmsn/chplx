//  Copyright (c) 2023-2025 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/detail/iterator_generator.hpp>
#include <chplx/types.hpp>

#include <hpx/assert.hpp>
#include <hpx/config.hpp>
#include <hpx/generator.hpp>

#include <hpx/iterator_support/counting_shape.hpp>

#include <cstddef>

namespace chplx {

//-----------------------------------------------------------------------------
// 1D iteration support
template <typename T, BoundedRangeType BoundedType, bool Stridable>
hpx::generator<T> iterate(
    detail::IteratorGenerator<Range<T, BoundedType, Stridable>> ni) noexcept {

  HPX_ASSERT(ni.target.isIterable());

  auto size = ni.size;
  for (auto ilo = ni.first; size-- != 0; ++ilo) {
    co_yield ni.target[ilo];
  }
}

//-----------------------------------------------------------------------------
template <typename T, BoundedRangeType BoundedType, bool Stridable>
decltype(auto) iterate(Range<T, BoundedType, Stridable> const &r) noexcept {

  return iterate(detail::IteratorGenerator(r));
}

//-----------------------------------------------------------------------------
template <typename T>
decltype(auto)
iterate(Range<T, BoundedRangeType::bounded, false> const &r) noexcept {

  return hpx::util::counting_shape(r.low(), r.high());
}

} // namespace chplx
