//  Copyright (c) 2015-2017 Bryce Adelstein Lelbach aka wash
//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#include <array>
#include <cstddef>
#include <exception>
#include <ostream>

namespace chplx::detail {

template <typename IndexType, std::size_t N> struct Position {
  constexpr Position() noexcept = default;

  template <typename... Indices>
  constexpr Position(Indices... idxs_) noexcept
      : idxs{{static_cast<IndexType>(idxs_)...}} {
    static_assert(sizeof...(Indices) == N,
                  "Insufficient index parameters passed to constructor.");
  }

  constexpr Position(Position const &) noexcept = default;
  constexpr Position(Position &&) noexcept = default;
  constexpr Position &operator=(Position const &) noexcept = default;
  constexpr Position &operator=(Position &&) noexcept = default;

  ~Position() = default;

  constexpr IndexType &operator[](IndexType i) noexcept { return idxs[i]; }
  constexpr IndexType const &operator[](IndexType i) const noexcept {
    return idxs[i];
  }

private:
  std::array<IndexType, N> idxs;
};

namespace detail {

template <typename IndexType, std::size_t N, std::size_t... Is>
std::ostream &output(std::ostream &os, Position<IndexType, N> pos,
                     std::integer_sequence<std::size_t, Is...>) {

  os << pos[0];
  ((os << ',' << pos[Is + 1]), ...);
  return os;
}

} // namespace detail

template <typename IndexType, std::size_t N>
std::ostream &operator<<(std::ostream &os, Position<IndexType, N> pos) {
  return detail::output(os, pos, std::make_index_sequence<N - 1>{});
}

template <typename IndexType> struct Dimension {
  constexpr Dimension() noexcept = default;

  constexpr Dimension(IndexType extent_, IndexType idx_) noexcept
      : extent(extent_), idx(idx_) {}

  constexpr Dimension(Dimension const &) = default;
  constexpr Dimension(Dimension &&) = default;

  constexpr Dimension &operator=(Dimension const &) = default;
  constexpr Dimension &operator=(Dimension &&) noexcept = default;

  ~Dimension() = default;

  IndexType const extent = 0;
  IndexType idx = 0;
};

} // namespace chplx::detail
