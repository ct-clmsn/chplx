//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/nothing.hpp>

#include <complex>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>

namespace chplx {

//-----------------------------------------------------------------------------
// Tuples are a way of grouping several values together. The number of
// components in each tuple must be known at compile-time. However, unlike an
// array, a tuple can store components of different types.
template <typename... Ts> struct Tuple;

// The value of boundedType determines which bounds of the range are specified
// (making the range "bounded", as opposed to infinite, in the corresponding
// direction(s)).
enum class BoundedRangeType {
  bounded,     ///< both bounds are specified.
  boundedLow,  ///< the low bound is specified(the high bound is +inf)
  boundedHigh, ///< the high bound is specified(the low bound is -inf)
  boundedNone  ///< neither bound is specified (both bounds are inf)
};

// encode range type
enum class BoundsCategoryType {
  None,   // default: not specified
  Closed, // closed range
  Open    // open range
};

// A range is a first-class, constant-space representation of a regular sequence
// of values. These values are typically integers, though ranges over booleans,
// enums, and other types are also supported. Ranges support serial and parallel
// iteration over the sequence of values they represent, as well as operations
// such as counting, striding, intersection, shifting, and comparisons. Ranges
// form the basis for defining rectangular domains (Domains) and arrays (Arrays)
// in Chapel.
template <typename T = std::int64_t,
          BoundedRangeType BoundedType = BoundedRangeType::bounded,
          bool Stridable = false>
struct Range;

// A domain is a first-class representation of an index set. Domains are used to
// specify iteration spaces, to define the size and shape of arrays (Arrays),
// and to specify aggregate operations like slicing. A domain can specify a
// single- or multi-dimensional rectangular iteration space or represent a set
// of indices of a given type. Domains can also represent a subset of another
// domain's index set, using either a dense or sparse representation. A domain's
// indices may potentially be distributed across multiple locales as described
// in Domain Maps, thus supporting global-view data structures.
template <int N, typename IndexType = std::int64_t, bool Stridable = false>
class Domain;

//-----------------------------------------------------------------------------
// Returns true if the type T is one the following types, of any width: int,
// uint, real, imag, complex.
template <typename T>
inline constexpr bool isNumericType = std::is_arithmetic_v<T>;

// Returns true if the type T is one the following types, of any width: int,
// uint.
template <typename T>
inline constexpr bool isIntegralType = std::is_integral_v<T>;

// Returns true if the type T is a real type, of any width.
template <typename T>
inline constexpr bool isRealType = std::is_floating_point_v<T>;

// Returns true if the type T is an imag type, of any width.
template <typename T>
inline constexpr bool isImagType = std::is_floating_point_v<T>;

// Returns true if the type T is an imag type, of any width.
template <typename T> inline constexpr bool isComplexType = false;

template <typename T>
inline constexpr bool isComplexType<std::complex<T>> = true;

// Returns true if the type T is the nothing type.
template <typename T> inline constexpr bool isNothingType = false;

template <> inline constexpr bool isNothingType<nothing> = true;

// Returns true if the type T is a bool type, of any width.
template <typename T> inline constexpr bool isBoolType = false;

template <> inline constexpr bool isBoolType<bool> = true;

// Returns true if the type T is an int type, of any width.
template <typename T>
inline constexpr bool isIntType = std::is_integral_v<T> && std::is_signed_v<T>;

// Returns true if the type T is an uint type, of any width.
template <typename T>
inline constexpr bool isUIntType =
    std::is_integral_v<T> && std::is_unsigned_v<T>;

// Returns true if the type T is a string type.
template <typename T> inline constexpr bool isStringType = false;

template <> inline constexpr bool isStringType<std::string> = true;

// Returns true if the type t is an enum type.
template <typename T> inline constexpr bool isEnumType = std::is_enum_v<T>;

// Returns true if the value T is one the following types, of any width: int,
// uint, real, imag, complex.
template <typename T> [[nodiscard]] constexpr bool isNumericValue(T) noexcept {
  return std::is_arithmetic_v<T>;
}

// Returns true if the value T is one the following types, of any width: int,
// uint.
template <typename T> [[nodiscard]] constexpr bool isIntegralValue(T) noexcept {
  return std::is_integral_v<T>;
}

// Returns true if the value T is a real value, of any width.
template <typename T> [[nodiscard]] constexpr bool isRealValue(T) noexcept {
  return std::is_floating_point_v<T>;
}

// Returns true if the value T is an imag value, of any width.
template <typename T> [[nodiscard]] constexpr bool isImagValue(T) noexcept {
  return std::is_floating_point_v<T>;
}

// Returns true if the value T is an complex value, of any width.
template <typename T> [[nodiscard]] constexpr bool isComplexValue(T) noexcept {
  return false;
}

template <typename T>
[[nodiscard]] constexpr bool isComplexValue(std::complex<T>) noexcept {
  return true;
}

// Returns true if the value T is the nothing value.
template <typename T> [[nodiscard]] constexpr bool isNothingValue(T) noexcept {
  return false;
}

[[nodiscard]] constexpr bool isNothingValue(nothing) noexcept { return true; }

// Returns true if the value T is a bool value, of any width.
template <typename T> [[nodiscard]] constexpr bool isBoolValue(T) noexcept {
  return false;
}

[[nodiscard]] constexpr bool isBoolValue(bool) noexcept { return true; }

// Returns true if the value T is an int value, of any width.
template <typename T> [[nodiscard]] constexpr bool isIntValue(T) noexcept {
  return std::is_integral_v<T> && std::is_signed_v<T>;
}

// Returns true if the value T is an uint value, of any width.
template <typename T> [[nodiscard]] constexpr bool isUIntValue(T) noexcept {
  return std::is_integral_v<T> && std::is_unsigned_v<T>;
}

// Returns true if the value T is a string value.
template <typename T> [[nodiscard]] constexpr bool isStringValue(T) noexcept {
  return false;
}

[[nodiscard]] constexpr bool isStringValue(std::string const &) noexcept {
  return true;
}

// Returns true if the value t is an enum value.
template <typename T> [[nodiscard]] constexpr bool isEnumValue(T) noexcept {
  return std::is_enum_v<T>;
}

namespace detail {

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

template <typename T> inline constexpr auto MinValue_v = MinValue<T>::value;

//-----------------------------------------------------------------------------
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

template <typename T> inline constexpr auto MaxValue_v = MaxValue<T>::value;

} // namespace detail
} // namespace chplx
