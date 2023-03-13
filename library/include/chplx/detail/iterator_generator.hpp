//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/types.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace chplx::detail {

template <typename Target> struct IteratorGenerator {

  template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, IteratorGenerator>)
  explicit constexpr IteratorGenerator(T &&target)
      : target(std::forward<T>(target)), first(0),
        size(target.isBounded()
                 ? target.size()
                 : MaxValue_v<typename std::decay_t<T>::idxType>) {}

  template <typename T>
  constexpr IteratorGenerator(T &&target, std::size_t first, std::size_t size)
      : target(std::forward<T>(target)), first(first), size(size) {}

  constexpr bool operator==(IteratorGenerator const &rhs) noexcept {
    return target == rhs.target && first == rhs.first && size == rhs.size;
  }

  Target target;
  std::size_t first;
  std::size_t size;
};

template <typename Target>
IteratorGenerator(Target &&target) -> IteratorGenerator<std::decay_t<Target>>;

template <typename Target>
IteratorGenerator(Target &&target, std::size_t, std::size_t)
    -> IteratorGenerator<std::decay_t<Target>>;

//-----------------------------------------------------------------------------
template <typename R>
constexpr auto size(IteratorGenerator<R> const &r) noexcept {

  return r.size;
}

template <typename R>
constexpr bool empty(IteratorGenerator<R> const &r) noexcept {

  return r.size == 0;
}

template <typename R>
auto subrange(IteratorGenerator<R> const &r, std::ptrdiff_t first,
              std::size_t size) {

  auto result = IteratorGenerator(r.target, r.first + first, size);

  return result;
}

} // namespace chplx::detail
