//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_domain.hpp>
#include <chplx/detail/iterator_generator.hpp>
#include <chplx/dmap.hpp>
#include <chplx/domains/default_rectangular.hpp>
#include <chplx/range.hpp>
#include <chplx/tuple.hpp>
#include <chplx/types.hpp>

#include <hpx/assert.hpp>
#include <hpx/config.hpp>
#include <hpx/generator.hpp>
#include <hpx/modules/memory.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace chplx {

//-----------------------------------------------------------------------------
template <int N, typename IndexType, bool Stridable> class Domain {

  static_assert(N > 0, "the domains rank should be positive");

public:
  static constexpr int Rank = N;

  using rangeType = BoundedRange<IndexType, Stridable>;
  using indexType = detail::generate_tuple_type_t<N, IndexType>;
  using indicesType = detail::generate_tuple_type_t<N, rangeType>;

  // Return the type of the indices of this domain
  using idxType = IndexType;

  // The idxType as represented by an integer type. When idxType is an enum
  // type, this evaluates to int. Otherwise, it evaluates to idxType.
  using intIdxType =
      std::conditional_t<std::is_enum_v<idxType>, std::int64_t, idxType>;

  // Return true if this is a stridable domain
  [[nodiscard]] static constexpr bool stridable() noexcept { return Stridable; }

  // Return the number of indices in this domain as an int.
  [[nodiscard]] constexpr std::int64_t size() const noexcept {
    return dom->size();
  }

  // Return the number of indices in this domain as the specified type
  template <typename T>
    requires(std::is_integral_v<T>)
  [[nodiscard]] constexpr T sizeAs() const noexcept {
    return reduce(shape(), std::multiplies<>(), static_cast<T>(1));
  }

  // Returns true if this is a fully bounded domain, false otherwise.
  [[nodiscard]] constexpr bool isBounded() const noexcept {
    return reduce(detail::lift(dims(), [](auto &&r) { return r.isBounded(); }),
                  std::logical_and<>(), true);
  }

  // Returns true if this is an iterable domain, false otherwise.
  [[nodiscard]] constexpr bool isIterable() const noexcept {
    return reduce(detail::lift(dims(), [](auto &&r) { return r.isIterable(); }),
                  std::logical_and<>(), true);
  }

  // Yield the domain indices
  [[nodiscard]] decltype(auto) these() const { return iterate(*this); }

  // Return a tuple of ranges describing the bounds of a rectangular domain. For
  // a sparse domain, return the bounds of the parent domain.
  constexpr indicesType dims() noexcept { return dom->dims(); }
  constexpr indicesType const &dims() const noexcept { return dom->dims(); }

  // Return a range representing the boundary of this domain in a particular
  // dimension.
  [[nodiscard]] constexpr auto dim(int i) const noexcept { return dom->dim(i); }

  // Return a tuple of int values representing the size of each dimension.
  [[nodiscard]] constexpr decltype(auto) shape() const noexcept {
    return detail::lift(dims(), [](auto &&r) { return r.size(); });
  }

  // Returns the domain's 'pure' low bound.
  [[nodiscard]] constexpr decltype(auto) lowBound() const noexcept {
    return dom->low();
  }

  // Return the lowest index represented by a rectangular domain.
  [[nodiscard]] constexpr decltype(auto) low() const noexcept {
    return dom->alignedLow();
  }

  // Return the first index in this domain.
  [[nodiscard]] constexpr decltype(auto) first() const noexcept {
    return dom->first();
  }

  // Returns the domain's 'pure' high bound.
  [[nodiscard]] constexpr decltype(auto) highBound() const noexcept {
    return dom->high();
  }

  // Return the highest index represented by a rectangular domain.
  [[nodiscard]] constexpr decltype(auto) high() const noexcept {
    return dom->alignedHigh();
  }

  // Return the last index in this domain.
  [[nodiscard]] constexpr decltype(auto) last() const noexcept {
    return dom->last();
  }

  // Return the stride of the indices in this domain.
  [[nodiscard]] constexpr decltype(auto) stride() const noexcept {
    return dom->stride();
  }

  // Return the alignment of the indices in this domain
  [[nodiscard]] constexpr decltype(auto) alignment() const noexcept {
    return dom->alignment();
  }

  // Return whether the given index is contained in this domain
  [[nodiscard]] constexpr decltype(auto)
  contains(indexType const &idx) const noexcept {
    return dom->member(idx);
  }

  // Return whether the index of the given domain is contained in this domain
  template <typename IndexType1, bool Stridable1>
  [[nodiscard]] constexpr decltype(auto)
  contains(Domain<N, IndexType1, Stridable1> const &other) const noexcept {
    return dom->member(other.lowBound()) && dom->member(other.highBound());
  }

  // Return true if the domain has no indices.
  [[nodiscard]] constexpr bool isEmpty() const noexcept { return size() == 0; }

  // Return true if this domain is a rectangular. Otherwise, return false.
  [[nodiscard]] static constexpr bool isRectangular() noexcept { return true; }

  // Return true if d is an irregular domain; e.g. is not rectangular.
  // Otherwise, return false.
  [[nodiscard]] static constexpr bool isIrregular() noexcept {
    return !isRectangular();
  }

  // Return true if d is an associative domain. Otherwise, return false.
  [[nodiscard]] static constexpr bool isAssociative() noexcept { return false; }

  // Return true if d is a sparse domain. Otherwise, return false.
  [[nodiscard]] static constexpr bool isSparse() noexcept { return false; }

  [[nodiscard]] static constexpr int rank() noexcept { return N; }

  // Returns an integer representing the zero-based ordinal value of ind within
  // the domain's sequence of values if it is a member of the sequence.
  // Otherwise, returns -1. The indexOrder procedure is the reverse of
  // orderToIndex.
  [[nodiscard]] constexpr std::int64_t
  indexOrder(indexType idx) const noexcept {
    return dom->indexOrder(idx);
  }

  // Returns the zero-based ord-th element of this domain's represented
  // sequence. The orderToIndex procedure is the reverse of indexOrder.
  [[nodiscard]] constexpr indexType
  orderToIndex(std::int64_t order) const noexcept {
    return dom->orderToIndex(order);
  }

  [[nodiscard]] constexpr IndexType operator[](std::int64_t idx) const noexcept
    requires(Rank == 1)
  {
    return std::get<0>(orderToIndex(idx));
  }

  [[nodiscard]] constexpr indexType operator[](std::int64_t idx) const noexcept
    requires(Rank != 1)
  {
    return orderToIndex(idx);
  }

  using baseDomain = domains::BaseRectangularDomain<N, IndexType, Stridable>;

  constexpr Domain() noexcept
      : dom(static_cast<baseDomain *>(
            chplx::defaultDist<Domain>().newRectangularDom(indicesType()))) {}

  explicit constexpr Domain(indicesType const &idx,
                            dmapBase<N, IndexType, Stridable> const &dm =
                                chplx::defaultDist<Domain>())
      : dom(static_cast<baseDomain *>(dm.newRectangularDom(idx))) {}

  template <typename R, typename... Rs>
    requires(isRangeType<R> && (isRangeType<Rs> && ...) &&
             (N == sizeof...(Rs) + 1))
  explicit constexpr Domain(R const &r, Rs const &...rs) noexcept
      : Domain(indicesType(rangeType(r), rangeType(rs)...)) {}

  template <typename T, typename... Ts> auto *buildArray(Ts &&...ts) {
    return dom->template dsiBuildArray<T>(std::forward<Ts>(ts)...);
  }

private:
  hpx::intrusive_ptr<baseDomain> dom;
};

//-----------------------------------------------------------------------------
namespace detail {

template <typename... Rs> struct common_range_index {

  using type = std::common_type_t<typename std::decay_t<Rs>::idxType...>;
};

template <typename... Rs>
using common_range_index_t = typename common_range_index<Rs...>::type;

template <typename... Rs>
inline constexpr bool common_stridable_v =
    (std::decay_t<Rs>::stridable() || ...);

} // namespace detail

template <typename R, typename... Rs>
  requires(isRangeType<R> && (isRangeType<Rs> && ...))
Domain(Tuple<R, Rs...>) -> Domain<static_cast<int>(sizeof...(Rs) + 1),
                                  detail::common_range_index_t<R, Rs...>,
                                  detail::common_stridable_v<R, Rs...>>;

template <typename R, typename... Rs, int N, typename IndexType, bool Stridable>
  requires(isRangeType<R> && (isRangeType<Rs> && ...) && sizeof...(Rs) + 1 == N)
Domain(Tuple<R, Rs...>, dmapBase<N, IndexType, Stridable>) -> Domain<
    N, std::common_type_t<IndexType, detail::common_range_index_t<R, Rs...>>,
    Stridable || detail::common_stridable_v<R, Rs...>>;

template <typename R, typename... Rs>
  requires(isRangeType<R> && (isRangeType<Rs> && ...))
Domain(R, Rs...) -> Domain<static_cast<int>(sizeof...(Rs) + 1),
                           detail::common_range_index_t<R, Rs...>,
                           detail::common_stridable_v<R, Rs...>>;

//-----------------------------------------------------------------------------
// Returns true if the type T is a domain type.
template <typename T> inline constexpr bool isDomainType = false;

template <int N, typename T, bool Stridable>
inline constexpr bool isDomainType<Domain<N, T, Stridable>> = true;

// Returns true if the value T is a domain value.
template <typename T> [[nodiscard]] constexpr bool isDomainValue(T) noexcept {
  return false;
}

template <int N, typename T, bool Stridable>
[[nodiscard]] constexpr bool
isDomainValue(Domain<N, T, Stridable> const &) noexcept {
  return true;
}

//-----------------------------------------------------------------------------
// Equality operators are defined to test if two domains are equivalent or not
template <int N, typename T, bool Stridable>
constexpr bool operator==(Domain<N, T, Stridable> const &lhs,
                          Domain<N, T, Stridable> const &rhs) noexcept {

  return lhs.dims() == rhs.dims();
}

template <int N, typename T, bool Stridable>
constexpr bool operator!=(Domain<N, T, Stridable> const &lhs,
                          Domain<N, T, Stridable> const &rhs) noexcept {

  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
// The by operator can be applied to a rectangular domain value in order to
// create a strided rectangular domain value. The right-hand operand to the by
// operator can either be an integral value or an integral tuple whose size
// matches the domain's rank.
//
// The type of the resulting domain is the same as the original domain but with
// stridable set to true. In the case of an integer stride value, the value of
// the resulting domain is computed by applying the integer value to each range
// in the value using the by operator. In the case of a tuple stride value, the
// resulting domain's value is computed by applying each tuple component to the
// corresponding range using the by operator.
template <int N, typename T, bool Stridable, typename StepType>
  requires(std::is_integral_v<StepType>)
auto by(Domain<N, T, Stridable> const &rhs, StepType step) {

  auto result = Domain<N, T, true>(
      detail::lift(rhs.dims(), [&](auto &&r) { return by(r, step); }));

  return result;
}

template <int N, typename T, bool Stridable, typename... Ts>
  requires(N == sizeof...(Ts) && (std::is_integral_v<Ts> && ...))
auto by(Domain<N, T, Stridable> const &rhs, Tuple<Ts...> const &steps) {

  auto result = Domain<N, T, true>(detail::lift(
      rhs.dims(), steps, [&](auto &&r, auto &&step) { return by(r, step); }));

  return result;
}

//-----------------------------------------------------------------------------
// The align operator can be applied to a rectangular domain value in order to
// change the alignment of a rectangular domain value. The right-hand operand to
// the align operator can either be an integral value or an integral tuple whose
// size matches the domain�s rank.
//
// The type of the resulting domain is the same as the original domain but with
// stridable set to true. In the case of an integer alignment value, the value
// of the resulting domain is computed by applying the integer value to each
// range in the value using the align operator. In the case of a tuple alignment
// value, the resulting domain's value is computed by applying each tuple
// component to the corresponding range using the align operator.
template <int N, typename T, bool Stridable, typename AlignType>
  requires(std::is_integral_v<AlignType>)
auto align(Domain<N, T, Stridable> const &rhs, AlignType alignment) {

  auto result = Domain<N, T, Stridable>(
      detail::lift(rhs.dims(), [&](auto &&r) { return align(r, alignment); }));

  return result;
}

template <int N, typename T, bool Stridable, typename... Ts>
  requires(N == sizeof...(Ts) && (std::is_integral_v<Ts> && ...))
auto align(Domain<N, T, Stridable> const &rhs, Tuple<Ts...> const &alignments) {

  auto result = Domain<N, T, Stridable>(
      detail::lift(rhs.dims(), alignments,
                   [&](auto &&r, auto &&step) { return align(r, step); }));

  return result;
}

//-----------------------------------------------------------------------------
// The # operator can be applied to dense rectangular domains with a tuple
// argument whose size matches the rank of the domain (or optionally an integer
// in the case of a 1D domain). The operator is equivalent to applying the #
// operator to the component ranges of the domain and then using them to slice
// the domain as in Section Range-based Slicing.
template <typename T, bool Stridable, typename Count>
  requires(std::is_integral_v<Count>)
auto count(Domain<1, T, Stridable> const &rhs, Count n) {

  auto result = Domain<1, T, Stridable>(
      detail::lift(rhs.dims(), [&](auto &&r) { return count(r, n); }));

  return result;
}

template <int N, typename T, bool Stridable, typename... Ts>
  requires(N != 1 && N == sizeof...(Ts) && (std::is_integral_v<Ts> && ...))
auto count(Domain<N, T, Stridable> const &rhs, Tuple<Ts...> const &counts) {

  auto result = Domain<N, T, Stridable>(detail::lift(
      rhs.dims(), counts, [&](auto &&r, auto &&n) { return count(r, n); }));

  return result;
}

//-----------------------------------------------------------------------------
// When slicing rectangular domains or arrays, the brackets or parentheses can
// contain a list of rank ranges. These ranges can either be bounded or
// unbounded. When unbounded, they inherit their bounds from the domain or array
// being sliced. The Cartesian product of the ranges' index sets is applied for
// slicing.
template <int N, typename T, bool Stridable, typename... Ts>
  requires(N == sizeof...(Ts) && (isRangeType<Ts> && ...))
auto slice(Domain<N, T, Stridable> const &rhs, Tuple<Ts...> const &ranges) {

  auto result = Domain<N, T, (Ts::stridable() || ...)>(detail::lift(
      rhs.dims(), ranges, [&](auto &&r, auto &&n) { return slice(r, n); }));

  return result;
}

//-----------------------------------------------------------------------------
template <int N, typename T, bool Stridable>
std::ostream &operator<<(std::ostream &os, Domain<N, T, Stridable> const &d) {

  os << "[";
  if constexpr (N != 0) {
    os << std::get<0>(d.dims().base());
    if constexpr (N > 1) {
      detail::writeTuple(os, d.dims().base(),
                         std::make_index_sequence<N - 1>());
    }
  }
  os << "]";
  return os;
}
} // namespace chplx
