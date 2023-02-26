//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/detail/range_index_generator.hpp>

#include <cassert>
#include <limits>
#include <type_traits>

namespace chplx {

/// The value of boundedType determines which bounds of the range are specified
/// (making the range "bounded", as opposed to infinite, in the corresponding
/// direction(s)).
enum class BoundedRangeType {
  bounded,     ///< both bounds are specified.
  boundedLow,  ///< the low bound is specified(the high bound is +inf)
  boundedHigh, ///< the high bound is specified(the low bound is -inf)
  boundedNone  ///< neither bound is specified (both bounds are inf)
};

namespace detail {

//-----------------------------------------------------------------------------
template <typename T, typename Enable = std::enable_if_t<!std::is_enum_v<T>>>
struct StrideType : std::make_signed_t<T> {};

template <typename T>
struct StrideType<T, std::enable_if_t<std::is_enum_v<T>>> {
  using type = int;
};

template <> struct StrideType<bool> {
  using type = int;
};

//-----------------------------------------------------------------------------
template <typename T, bool Stride> struct Stridable {

  explicit constexpr Stridable(T) noexcept {}

  [[nodiscard]] static constexpr auto getStride() noexcept {
    return StrideType<T>(1);
  }
};

template <typename T> struct Stridable<T, true> {

  explicit constexpr Stridable(T s) noexcept : stride(StrideType<T>(s)) {}

  [[nodiscard]] constexpr auto getStride() const noexcept { return stride; }

  StrideType<T> stride{};
};

template <typename T, bool Stride>
[[nodiscard]] constexpr auto
operator==(Stridable<T, Stride> const &lhs,
           Stridable<T, Stride> const &rhs) noexcept {
  return lhs.getStride() == rhs.getStride();
}

//-----------------------------------------------------------------------------
template <typename T, typename Enable = std::enable_if_t<!std::is_enum_v<T>>>
struct MinValue {
  static constexpr T value = (std::numeric_limits<T>::min)();
};

template <typename T> struct MinValue<T, std::enable_if_t<std::is_enum_v<T>>> {
  static constexpr T value = T::minValue;
};

template <> struct MinValue<bool> {
  static constexpr bool value = false;
};

template <typename T, typename Enable = std::enable_if_t<!std::is_enum_v<T>>>
struct MaxValue {
  static constexpr T value = (std::numeric_limits<T>::max)();
};

template <typename T> struct MaxValue<T, std::enable_if_t<std::is_enum_v<T>>> {
  static constexpr T value = T::maxValue;
};

template <> struct MaxValue<bool> {
  static constexpr bool value = true;
};

//-----------------------------------------------------------------------------
enum class BoundsCategoryType { None, Closed, Open };

template <typename T, BoundedRangeType BoundedType> struct Bounds {

  Bounds(T low, T high, BoundsCategoryType type = BoundsCategoryType::None)
      : firstIndex(low),
        lastIndex(high + (type == BoundsCategoryType::Closed ? 1 : 0)) {}

  [[nodiscard]] constexpr auto getFirstIndex() const noexcept {
    return firstIndex;
  }
  [[nodiscard]] constexpr auto getLastIndex() const noexcept {
    return lastIndex;
  }
  [[nodiscard]] static constexpr bool isIterable() noexcept { return true; }

  T firstIndex;
  T lastIndex;
};

template <typename T> struct Bounds<T, BoundedRangeType::boundedLow> {

  Bounds(T low, [[maybe_unused]] T high,
         BoundsCategoryType = BoundsCategoryType::None)
      : firstIndex(low) {
    assert(high == MaxValue<T>::value);
  }

  [[nodiscard]] constexpr auto getFirstIndex() const noexcept {
    return firstIndex;
  }
  [[nodiscard]] static constexpr auto getLastIndex() noexcept {
    return MaxValue<T>::value;
  }
  [[nodiscard]] static constexpr bool isIterable() noexcept { return true; }

  T firstIndex;
};

template <typename T> struct Bounds<T, BoundedRangeType::boundedHigh> {

  Bounds([[maybe_unused]] T low, T high,
         BoundsCategoryType type = BoundsCategoryType::None)
      : lastIndex(high + (type == BoundsCategoryType::Closed ? 1 : 0)) {
    assert(low == MinValue<T>::value);
  }

  [[nodiscard]] static constexpr auto getFirstIndex() noexcept {
    return MinValue<T>::value;
  }
  [[nodiscard]] constexpr auto getLastIndex() const noexcept {
    return lastIndex;
  }
  [[nodiscard]] static constexpr bool isIterable() noexcept { return false; }

  T lastIndex;
};

template <typename T> struct Bounds<T, BoundedRangeType::boundedNone> {

  Bounds([[maybe_unused]] T low, [[maybe_unused]] T high,
         BoundsCategoryType = BoundsCategoryType::None) {
    assert(low == MinValue<T>::value);
    assert(high == MaxValue<T>::value);
  }

  [[nodiscard]] static constexpr auto getFirstIndex() noexcept {
    return MinValue<T>::value;
  }
  [[nodiscard]] static constexpr auto getLastIndex() noexcept {
    return MaxValue<T>::value;
  }
  [[nodiscard]] static constexpr bool isIterable() noexcept { return false; }
};

template <typename T, BoundedRangeType BoundedType>
[[nodiscard]] constexpr auto
operator==(Bounds<T, BoundedType> const &lhs,
           Bounds<T, BoundedType> const &rhs) noexcept {
  return lhs.getFirstIndex() == rhs.getFirstIndex() &&
         lhs.getLastIndex() == rhs.getLastIndex();
}

//-----------------------------------------------------------------------------
template <typename T, BoundedRangeType BoundedType> struct Alignment {

  explicit constexpr Alignment(T) noexcept {}

  [[nodiscard]] static constexpr auto getAlignment() noexcept {
    return MinValue<T>::value;
  } // ambiguous alignment

  [[nodiscard]] static constexpr bool hasAmbiguousValue() noexcept {
    return true;
  }
};

template <typename T> struct Alignment<T, BoundedRangeType::bounded> {

  explicit constexpr Alignment(T a) noexcept : alignment(a) {}

  [[nodiscard]] constexpr T getAlignment() const noexcept { return alignment; }

  [[nodiscard]] constexpr bool hasAmbiguousValue() const noexcept {
    return alignment != 1 && alignment != -1;
  }

  T alignment;
};

template <typename T, BoundedRangeType BoundedType>
[[nodiscard]] constexpr auto
operator==(Alignment<T, BoundedType> const &lhs,
           Alignment<T, BoundedType> const &rhs) noexcept {
  return lhs.getAlignment() == rhs.getAlignment();
}

} // namespace detail

//-----------------------------------------------------------------------------
template <typename T = int,
          BoundedRangeType BoundedType = BoundedRangeType::bounded,
          bool Stridable = false>
struct Range {

  using idxType = T;
  static constexpr BoundedRangeType boundedType = BoundedType;
  static constexpr bool stridable = Stridable;

  static_assert(std::is_integral_v<idxType> || std::is_enum_v<idxType> ||
                    std::is_same_v<bool, idxType>,
                "idxType must be an integral, boolean, or enumerated type");

  enum class Init { noValue };

  Range()
      : bounds(detail::MinValue<T>::value, detail::MaxValue<T>::value),
        stride(false), alignment(1) {}

  Range(T low, T high,
        detail::BoundsCategoryType type = detail::BoundsCategoryType::Closed)
      : bounds(low, high, type), stride(false), alignment(1) {}

  Range(T low, Init = Init::noValue)
      : bounds(low, detail::MaxValue<T>::value), stride(false), alignment(1) {}

  Range(Init, T high,
        detail::BoundsCategoryType type = detail::BoundsCategoryType::Closed)
      : bounds(detail::MinValue<T>::value, high, type), stride(false),
        alignment(1) {}

  [[nodiscard]] bool isAmbiguouslyAligned() const noexcept {
    return alignment.hasAmbiguousValue();
  }

  [[nodiscard]] constexpr auto getFirstIndex() const noexcept {
    return bounds.getFirstIndex();
  }
  [[nodiscard]] constexpr auto getLastIndex() const noexcept {
    return bounds.getLastIndex();
  }

  [[nodiscard]] constexpr auto getStride() const noexcept {
    return stride.getStride();
  }

  [[nodiscard]] constexpr auto getAlignedLowBound() const noexcept {
    return alignment.getAlignment(); // TODO: needs implementation
  }
  [[nodiscard]] constexpr auto getAlignedHighBound() const noexcept {
    return alignment.getAlignment(); // TODO: needs implementation
  }

  [[nodiscard]] static constexpr bool isIterable() noexcept {
    return detail::Bounds<T, BoundedType>::isIterable();
  }

  [[no_unique_address]] detail::Bounds<T, BoundedType> bounds;
  [[no_unique_address]] detail::Stridable<T, Stridable> stride;
  [[no_unique_address]] detail::Alignment<T, BoundedType> alignment;
};

template <typename T, BoundedRangeType BoundedType, bool Stridable>
[[nodiscard]] constexpr auto
operator==(Range<T, BoundedType, Stridable> const &lhs,
           Range<T, BoundedType, Stridable> const &rhs) noexcept {
  return lhs.bounds == rhs.bounds && lhs.stride == rhs.stride &&
         lhs.alignment == rhs.alignment;
}

template <typename T, BoundedRangeType BoundedType, bool Stridable>
[[nodiscard]] constexpr auto
operator!=(Range<T, BoundedType, Stridable> const &lhs,
           Range<T, BoundedType, Stridable> const &rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace chplx
