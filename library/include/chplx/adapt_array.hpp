//  Copyright (c) 2023-2025 Hartmut Kaiser
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

#include <hpx/iterator_support/iterator_range.hpp>
#include <hpx/type_support/bit_cast.hpp>
#include <hpx/type_support/is_contiguous_iterator.hpp>

#include <iterator>
#include <type_traits>

namespace chplx {

//-----------------------------------------------------------------------------
// 1D iteration support
template <typename T, typename Domain>
hpx::generator<T &, T>
iterate(detail::IteratorGenerator<Array<T, Domain>> a) noexcept {

  auto size = a.size;
  for (auto ilo = a.first; size-- != 0; ++ilo) {
    co_yield a.target[ilo];
  }
}

//-----------------------------------------------------------------------------
template <typename T, typename Domain>
decltype(auto) iterate(Array<T, Domain> const &a) noexcept {

  return iterate(detail::IteratorGenerator(a));
}

template <typename T, typename Domain>
decltype(auto) iterate(Array<T, Domain> &&a) noexcept {

  return iterate(detail::IteratorGenerator(std::move(a)));
}

namespace detail {

template <typename Iter>
  requires(!std::is_pointer_v<Iter>)
HPX_FORCEINLINE constexpr auto get_unwrapped_ptr(Iter it) noexcept {

  static_assert(hpx::traits::is_contiguous_iterator_v<Iter>,
                "optimized merge is possible for contiguous-iterators "
                "only");

  using value_t = typename std::iterator_traits<Iter>::value_type;
  return const_cast<value_t *>(hpx::bit_cast<value_t const volatile *>(&*it));
}

template <typename T>
  requires(std::is_pointer_v<T>)
HPX_FORCEINLINE constexpr auto get_unwrapped_ptr(T ptr) noexcept {
  using value_t = std::remove_pointer_t<T>;
  return const_cast<value_t *>(hpx::bit_cast<value_t const volatile *>(ptr));
}

template <typename Iter> HPX_FORCEINLINE auto get_unwrapped(Iter it) {

  // is_contiguous_iterator_v is true for pointers
  if constexpr (hpx::traits::is_contiguous_iterator_v<Iter>) {
    return get_unwrapped_ptr(it);
  } else {
    return it;
  }
}

} // namespace detail

template <typename T>
decltype(auto) iterate(Array<T, Domain<1>> const &a) noexcept {

  auto begin = detail::get_unwrapped(a.begin());
  auto end = detail::get_unwrapped(a.end());
  return hpx::util::iterator_range(begin, end);
}

template <typename T> decltype(auto) iterate(Array<T, Domain<1>> &&a) noexcept {

  auto begin = detail::get_unwrapped(a.begin());
  auto end = detail::get_unwrapped(a.end());
  return hpx::util::iterator_range(begin, end);
}

} // namespace chplx
