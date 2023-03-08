//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

namespace chplx::detail {

template <typename Target> struct IteratorGenerator {

  template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, IteratorGenerator>)
  IteratorGenerator(T &&target)
      : target(std::forward<T>(target)), first(0), size(target.size()) {}

  template <typename T>
  IteratorGenerator(T &&target, std::size_t first, std::size_t size)
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
} // namespace chplx::detail
