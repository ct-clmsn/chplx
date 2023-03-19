//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_array.hpp>
#include <chplx/detail/iterator_generator.hpp>
#include <chplx/domain.hpp>
#include <chplx/locale.hpp>
#include <chplx/types.hpp>

#include <utility>
#include <vector>

namespace chplx {

// An array is a map from a domain’s indices to a collection of variables of
// homogeneous type.
template <typename Domain, typename T> class Array {

public:
  using idxType = typename Domain::idxType;
  using indexType = typename Domain::indexType;

  Array() = default;

  Array(Domain domain) : domain(std::move(domain)), data(domain.size()) {}
  Array(Domain domain, std::vector<T> &&data)
      : domain(std::move(domain)), data(std::move(data)) {}

  Array(Array const &) = default;
  Array(Array &&) = default;

  Array &operator=(Array const &) = default;
  Array &operator=(Array &&) = default;

  ~Array() = default;

  constexpr T &This(indexType idx)
    requires(isTupleType<indexType>)
  {
    return data[domain.indexOrder(idx)];
  }
  constexpr T const &This(indexType idx) const
    requires(isTupleType<indexType>)
  {
    return data[domain.indexOrder(idx)];
  }

  constexpr T &operator()(indexType idx) noexcept
    requires(isTupleType<indexType>)
  {
    return data[domain.indexOrder(idx)];
  }
  constexpr T const &operator()(indexType idx) const noexcept
    requires(isTupleType<indexType>)
  {
    return data[domain.indexOrder(idx)];
  }

  constexpr T &operator()(std::size_t idx) noexcept { return data[idx]; }
  constexpr T const &operator()(T idx) const noexcept { return data[idx]; }

  template <typename... Ts>
    requires(sizeof...(Ts) == Domain::Rank && sizeof...(Ts) > 1 &&
             (std::is_convertible_v<Ts, std::size_t> && ...))
  constexpr T &operator()(Ts... idxs) noexcept {
    return data[domain.indexOrder(Tuple<std::common_type_t<Ts...>>(idxs...))];
  }
  template <typename... Ts>
    requires(sizeof...(Ts) == Domain::Rank && sizeof...(Ts) > 1 &&
             (std::is_convertible_v<Ts, std::size_t> && ...))
  constexpr T const &operator()(Ts... idxs) const noexcept {
    return data[domain.indexOrder(Tuple<std::common_type_t<Ts...>>(idxs...))];
  }

  // Yield the array values
  [[nodiscard]] decltype(auto) these() const { return iterate(*this); }

  // Return the number of indices in this array as an int.
  [[nodiscard]] constexpr std::int64_t size() const noexcept {
    return domain.size();
  }

  // Returns true if the embedded domain is fully bounded, false otherwise.
  [[nodiscard]] constexpr bool isBounded() const noexcept {
    return domain.isBounded();
  }

  // return the location of this array instance
  chplx::locale locale = chplx::here;

private:
  Domain domain{};
  std::vector<T> data{};
};

} // namespace chplx
