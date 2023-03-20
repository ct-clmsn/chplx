//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_range.hpp>
#include <chplx/array.hpp>
#include <chplx/types.hpp>

#include <hpx/config.hpp>
#include <hpx/generator.hpp>

#include <iterator>

namespace chplx {

//-----------------------------------------------------------------------------
// 1D iteration support
template <typename Domain, typename T>
hpx::generator<T> iterate(
    detail::IteratorGenerator<Array<Domain, T>> a) noexcept {

  auto size = a.size;
  for (auto ilo = a.first; size-- != 0; ++ilo) {
    co_yield a.target(ilo);
  }
}

//-----------------------------------------------------------------------------
template <typename Domain, typename T>
decltype(auto) iterate(Array<Domain, T> const &a) noexcept {

  return iterate(detail::IteratorGenerator(a));
}

} // namespace chplx
