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

#include <chplx/domains/default_rectangular.hpp>

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

namespace chplx {

// An array is a map from a domain's indices to a collection of variables of
// homogeneous type.
template <typename T, typename Domain> class Array {

public:
  static constexpr int rank = Domain::rank();
  static constexpr bool Stridable = Domain::stridable();
  using idxType = typename Domain::idxType;

private:
  using arrayHandle =
      domains::BaseRectangularArray<T, rank, idxType, Stridable>;

  using indexType = typename Domain::indexType;
  using indicesType = typename Domain::indicesType;

public:
  Array() = default;

  explicit Array(Domain domain) : array(domain.template buildArray<T>()) {}
  Array(Domain domain, T init) : array(domain.template buildArray<T>(init)) {}

  Array(Domain domain, std::vector<T> &&data)
    requires(rank == 1)
      : array(domain.template buildArray<T>(std::move(data))) {}
  Array(Domain domain, std::initializer_list<T> &&data)
    requires(rank == 1)
      : array(domain.template buildArray<T>(std::move(data))) {}

  Array(Domain domain, std::vector<std::vector<T>> &&data)
    requires(rank == 2)
      : array(domain.template buildArray<T>(std::move(data))) {}
  Array(Domain domain, std::initializer_list<std::initializer_list<T>> &&data)
    requires(rank == 2)
      : array(domain.template buildArray<T>(std::move(data))) {}

  Array(Domain domain, std::vector<std::vector<std::vector<T>>> &&data)
    requires(rank == 3)
      : array(domain.template buildArray<T>(std::move(data))) {}
  Array(Domain domain,
        std::initializer_list<std::initializer_list<std::initializer_list<T>>>
            &&data)
    requires(rank == 3)
      : array(domain.template buildArray<T>(std::move(data))) {}

  Array(Array const &) = default;
  Array(Array &&) = default;

  Array &operator=(Array const &) = default;
  Array &operator=(Array &&) = default;

  ~Array() = default;

  constexpr T &This(indexType idx)
    requires(isTupleType<indexType>)
  {
    return array->dsiAccess(array->dsiGetBaseDomain()->dsiIndexOrder(idx));
  }
  constexpr T const &This(indexType idx) const
    requires(isTupleType<indexType>)
  {
    return array->dsiAccess(array->dsiGetBaseDomain()->dsiIndexOrder(idx));
  }

  constexpr T &operator()(indexType idx) noexcept
    requires(isTupleType<indexType>)
  {
    return array->dsiAccess(array->dsiGetBaseDomain()->dsiIndexOrder(idx));
  }
  constexpr T const &operator()(indexType idx) const noexcept
    requires(isTupleType<indexType>)
  {
    return array->dsiAccess(array->dsiGetBaseDomain()->dsiIndexOrder(idx));
  }

  constexpr T &operator()(idxType idx) noexcept {
    return array->dsiAccess(idx);
  }
  constexpr T const &operator()(idxType idx) const noexcept {
    return array->dsiAccess(idx);
  }

  constexpr T &operator[](idxType idx) noexcept {
    return array->dsiAccess(idx);
  }
  constexpr T const &operator[](idxType idx) const noexcept {
    return array->dsiAccess(idx);
  }

  template <typename... Ts>
    requires(sizeof...(Ts) == rank && sizeof...(Ts) > 1 &&
             (std::is_convertible_v<Ts, std::size_t> && ...))
  constexpr T &operator()(Ts... idxs) noexcept {
    return array->dsiAccess(array->dsiGetBaseDomain()->dsiIndexOrder(
        detail::generate_tuple_type_t<rank, std::common_type_t<Ts...>>(
            idxs...)));
  }
  template <typename... Ts>
    requires(sizeof...(Ts) == rank && sizeof...(Ts) > 1 &&
             (std::is_convertible_v<Ts, std::size_t> && ...))
  constexpr T const &operator()(Ts... idxs) const noexcept {
    return array->dsiAccess(array->dsiGetBaseDomain()->dsiIndexOrder(
        detail::generate_tuple_type_t<rank, std::common_type_t<Ts...>>(
            idxs...)));
  }

  // Yield the array values
  [[nodiscard]] decltype(auto) these() const { return iterate(*this); }

  // Return the number of indices in this array as an int.
  [[nodiscard]] constexpr std::int64_t size() const noexcept {
    return array->dsiGetBaseDomain()->dsiNumIndices();
  }

  // Returns true if the embedded domain is fully bounded, false otherwise.
  [[nodiscard]] constexpr bool isBounded() const noexcept {
    return reduce(detail::lift(dims(), [](auto &&r) { return r.isBounded(); }),
                  std::logical_and<>(), true);
  }

  // Return a tuple of ranges describing the bounds of a rectangular domain. For
  // a sparse domain, return the bounds of the parent domain.
  constexpr indicesType dims() noexcept {
    return array->dsiGetBaseDomain()->dsiDims();
  }
  constexpr indicesType const &dims() const noexcept {
    return array->dsiGetBaseDomain()->dsiDims();
  }

  // Return a range representing the boundary of this domain in a particular
  // dimension.
  [[nodiscard]] constexpr auto dim(int i) const noexcept {
    return array->dsiGetBaseDomain()->dsiDim(i);
  }

  // return the location of this array instance
  chplx::locale locale = chplx::here;

private:
  hpx::intrusive_ptr<arrayHandle> array;
};

template <typename Domain, typename T>
  requires(isDomainType<Domain>)
Array(Domain, T) -> Array<T, Domain>;

template <typename Domain, typename T>
  requires(isDomainType<Domain>)
Array(Domain, std::vector<T> &&) -> Array<T, Domain>;

template <typename Domain, typename T>
  requires(isDomainType<Domain>)
Array(Domain, std::initializer_list<T> &&) -> Array<T, Domain>;

//-----------------------------------------------------------------------------
namespace detail {

enum class eol { none, space, newline };

template <int N> struct writeArray {
  template <typename T, typename Domain, typename... IndexTypes>
    requires(sizeof...(IndexTypes) == Domain::rank() - N &&
             (std::is_convertible_v<IndexTypes, typename Domain::idxType> &&
              ...) &&
             (std::is_nothrow_constructible_v<typename Domain::idxType,
                                              IndexTypes> &&
              ...))
  static void call(std::ostream &os, Array<T, Domain> const &arr, eol prev_last,
                   IndexTypes... indices) {
    auto d = arr.dim(Domain::rank() - N);

    for (typename Domain::idxType i = d.low(); i <= d.high(); ++i) {
      writeArray<N - 1>::call(
          os, arr,
          i == d.high() ? (N != Domain::rank() ? prev_last : eol::none)
                        : (N == 1 ? eol::space : eol::newline),
          indices..., i);
    }
    if (N > 1 && prev_last == eol::newline) {
      os << "\n";
    }
  }
};

template <> struct writeArray<0> {
  template <typename T, typename Domain, typename... IndexTypes>
    requires(sizeof...(IndexTypes) == Domain::rank() &&
             (std::is_convertible_v<IndexTypes, typename Domain::idxType> &&
              ...) &&
             (std::is_nothrow_constructible_v<typename Domain::idxType,
                                              IndexTypes> &&
              ...))
  static void call(std::ostream &os, Array<T, Domain> const &arr, eol last,
                   IndexTypes... indices) {

    os << arr(Tuple(indices...))
       << (last == eol::newline ? "\n"
           : last == eol::space ? " "
                                : "");
  }
};

} // namespace detail

template <typename T, typename Domain>
std::ostream &operator<<(std::ostream &os, Array<T, Domain> const &arr) {
  if (arr.size() != 0) {
    detail::writeArray<Domain::rank()>::call(os, arr, detail::eol::none);
  }
  return os;
}
} // namespace chplx
