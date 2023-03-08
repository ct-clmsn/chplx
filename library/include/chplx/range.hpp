//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_range.hpp>
#include <chplx/detail/iterator_generator.hpp>
#include <chplx/types.hpp>

#include <hpx/assert.hpp>
#include <hpx/config.hpp>
#include <hpx/modules/type_support.hpp>

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <ostream>
#include <type_traits>

namespace chplx {

enum class RangeInit { noValue };

namespace detail {

//-----------------------------------------------------------------------------
template <typename T, typename Enable = std::enable_if_t<!std::is_enum_v<T>>>
struct StrideType : std::make_signed<T> {};

template <typename T>
struct StrideType<T, std::enable_if_t<std::is_enum_v<T>>> {
  using type = std::int64_t;
};

template <> struct StrideType<bool> {
  using type = std::int64_t;
};

template <typename T> using StrideType_t = typename StrideType<T>::type;

//-----------------------------------------------------------------------------
template <typename T, BoundedRangeType BoundedType, bool Stride>
struct Stridable {

  Stridable() = default;

  explicit constexpr Stridable([[maybe_unused]] T s) noexcept {
    HPX_ASSERT(s == StrideType_t<T>(1));
  }

  [[nodiscard]] static constexpr auto getStride() noexcept {
    return StrideType_t<T>(1);
  }

  [[nodiscard]] static constexpr bool hasAmbiguousValue() noexcept {
    return BoundedType == BoundedRangeType::boundedNone;
  }
};

template <typename T, BoundedRangeType BoundedType>
struct Stridable<T, BoundedType, true> {

  Stridable() = default;

  explicit constexpr Stridable(T s) noexcept : stride(StrideType_t<T>(s)) {}

  [[nodiscard]] constexpr auto getStride() const noexcept { return stride; }

  [[nodiscard]] constexpr bool hasAmbiguousValue() const noexcept {
    return stride != 1 && stride != -1;
  }

  StrideType_t<T> stride = StrideType_t<T>(1);
};

template <typename T1, BoundedRangeType BoundedType1, bool Stride1, typename T2,
          BoundedRangeType BoundedType2, bool Stride2>
[[nodiscard]] constexpr bool
operator==(Stridable<T1, BoundedType1, Stride1> const &lhs,
           Stridable<T2, BoundedType2, Stride2> const &rhs) noexcept {

  return lhs.getStride() == rhs.getStride();
}

template <typename T1, BoundedRangeType BoundedType1, bool Stride1, typename T2,
          BoundedRangeType BoundedType2, bool Stride2>
[[nodiscard]] constexpr bool
operator!=(Stridable<T1, BoundedType1, Stride1> const &lhs,
           Stridable<T2, BoundedType2, Stride2> const &rhs) noexcept {

  return !(lhs == rhs);
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

//-----------------------------------------------------------------------------
template <typename T, BoundedRangeType BoundedType> struct Bounds {

  constexpr Bounds(T low, T high,
                   BoundsCategoryType type = BoundsCategoryType::None) noexcept
      : firstIndex(low),
        lastIndex(high + (type == BoundsCategoryType::Closed ? 1 : 0)) {}

  constexpr Bounds(T low, RangeInit) noexcept
      : firstIndex(low), lastIndex(MaxValue_v<T>) {}

  constexpr Bounds(RangeInit, T high,
                   BoundsCategoryType type = BoundsCategoryType::None) noexcept
      : firstIndex(MinValue_v<T>),
        lastIndex(high + (type == BoundsCategoryType::Closed ? 1 : 0)) {}

  constexpr Bounds(RangeInit, RangeInit) noexcept
      : firstIndex(MinValue_v<T>), lastIndex(MaxValue_v<T>) {}

  constexpr Bounds() noexcept : firstIndex(T(1)), lastIndex(T(1)) {}

  [[nodiscard]] constexpr auto getFirstIndex() const noexcept {
    return firstIndex;
  }
  [[nodiscard]] constexpr auto getLastIndex() const noexcept {
    return lastIndex;
  }
  [[nodiscard]] static constexpr bool isIterable() noexcept { return true; }

  [[nodiscard]] constexpr bool hasFirst() const noexcept {
    return firstIndex != MinValue_v<T>;
  }
  [[nodiscard]] constexpr bool hasLast() const noexcept {
    return lastIndex != MaxValue_v<T>;
  }
  [[nodiscard]] constexpr BoundedRangeType boundedType() const noexcept {

    if (firstIndex == MinValue_v<T>) {
      if (lastIndex == MaxValue_v<T>)
        return BoundedRangeType::boundedNone;
      return BoundedRangeType::boundedHigh;
    }
    if (lastIndex == MaxValue_v<T>)
      return BoundedRangeType::boundedLow;
    return BoundedRangeType::bounded;
  }

  T firstIndex;
  T lastIndex;
};

template <typename T> struct Bounds<T, BoundedRangeType::boundedLow> {

  constexpr Bounds(T low, [[maybe_unused]] T high,
                   BoundsCategoryType = BoundsCategoryType::None) noexcept
      : firstIndex(low) {
    HPX_ASSERT(high == MaxValue_v<T>);
  }

  constexpr Bounds(T low, RangeInit) noexcept : firstIndex(low) {}

  constexpr Bounds(RangeInit, T high,
                   BoundsCategoryType type = BoundsCategoryType::None) noexcept
      : firstIndex(MinValue_v<T>) {
    HPX_ASSERT(high == MaxValue_v<T>);
  }

  constexpr Bounds(RangeInit, RangeInit) noexcept : firstIndex(MinValue_v<T>) {}

  constexpr Bounds() noexcept : firstIndex(T(1)) {}

  [[nodiscard]] constexpr auto getFirstIndex() const noexcept {
    return firstIndex;
  }
  [[nodiscard]] static constexpr auto getLastIndex() noexcept {
    return MaxValue_v<T>;
  }
  [[nodiscard]] static constexpr bool isIterable() noexcept { return true; }

  [[nodiscard]] constexpr bool hasFirst() const noexcept {
    return firstIndex != MinValue_v<T>;
  }
  [[nodiscard]] static constexpr bool hasLast() noexcept { return false; }

  [[nodiscard]] constexpr BoundedRangeType boundedType() const noexcept {

    if (firstIndex == MinValue_v<T>)
      return BoundedRangeType::boundedNone;
    return BoundedRangeType::boundedLow;
  }

  T firstIndex;
};

template <typename T> struct Bounds<T, BoundedRangeType::boundedHigh> {

  constexpr Bounds([[maybe_unused]] T low, T high,
                   BoundsCategoryType type = BoundsCategoryType::None) noexcept
      : lastIndex(high + (type == BoundsCategoryType::Closed ? 1 : 0)) {
    HPX_ASSERT(low == MinValue_v<T>);
  }

  constexpr Bounds([[maybe_unused]] T low, RangeInit) noexcept
      : lastIndex(MaxValue_v<T>) {
    HPX_ASSERT(low == MinValue_v<T>);
  }

  constexpr Bounds(RangeInit, T high,
                   BoundsCategoryType type = BoundsCategoryType::None) noexcept
      : lastIndex(high + (type == BoundsCategoryType::Closed ? 1 : 0)) {}

  constexpr Bounds(RangeInit, RangeInit) noexcept : lastIndex(MaxValue_v<T>) {}

  constexpr Bounds() noexcept : lastIndex(T(1)) {}

  [[nodiscard]] static constexpr auto getFirstIndex() noexcept {
    return MinValue_v<T>;
  }
  [[nodiscard]] constexpr auto getLastIndex() const noexcept {
    return lastIndex;
  }
  [[nodiscard]] static constexpr bool isIterable() noexcept { return false; }

  [[nodiscard]] static constexpr bool hasFirst() noexcept { return false; }
  [[nodiscard]] constexpr bool hasLast() const noexcept {
    return lastIndex != MaxValue_v<T>;
  }

  [[nodiscard]] constexpr BoundedRangeType boundedType() const noexcept {

    if (lastIndex == MaxValue_v<T>)
      return BoundedRangeType::boundedNone;
    return BoundedRangeType::boundedHigh;
  }

  T lastIndex;
};

template <typename T> struct Bounds<T, BoundedRangeType::boundedNone> {

  constexpr Bounds([[maybe_unused]] T low, [[maybe_unused]] T high,
                   BoundsCategoryType = BoundsCategoryType::None) noexcept {
    HPX_ASSERT(low == MinValue_v<T>);
    HPX_ASSERT(high == MaxValue_v<T>);
  }

  constexpr Bounds([[maybe_unused]] T low, RangeInit) noexcept {
    HPX_ASSERT(low == MinValue_v<T>);
  }

  constexpr Bounds(
      RangeInit, [[maybe_unused]] T high,
      BoundsCategoryType type = BoundsCategoryType::None) noexcept {
    HPX_ASSERT(high == MaxValue_v<T>);
  }

  constexpr Bounds(RangeInit, RangeInit) noexcept {}

  constexpr Bounds() noexcept = default;

  [[nodiscard]] static constexpr auto getFirstIndex() noexcept {
    return MinValue_v<T>;
  }
  [[nodiscard]] static constexpr auto getLastIndex() noexcept {
    return MaxValue_v<T>;
  }
  [[nodiscard]] static constexpr bool isIterable() noexcept { return false; }

  [[nodiscard]] static constexpr bool hasFirst() noexcept { return false; }
  [[nodiscard]] static constexpr bool hasLast() noexcept { return false; }

  [[nodiscard]] static constexpr BoundedRangeType boundedType() noexcept {
    return BoundedRangeType::boundedNone;
  }
};

template <typename T1, BoundedRangeType BoundedType1, typename T2,
          BoundedRangeType BoundedType2>
[[nodiscard]] constexpr bool
operator==(Bounds<T1, BoundedType1> const &lhs,
           Bounds<T2, BoundedType2> const &rhs) noexcept {

  return lhs.getFirstIndex() == rhs.getFirstIndex() &&
         lhs.getLastIndex() == rhs.getLastIndex();
}

template <typename T1, BoundedRangeType BoundedType1, typename T2,
          BoundedRangeType BoundedType2>
[[nodiscard]] constexpr bool
operator!=(Bounds<T1, BoundedType1> const &lhs,
           Bounds<T2, BoundedType2> const &rhs) noexcept {

  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
template <typename T, BoundedRangeType BoundedType, bool Stridable>
struct Alignment {

  Alignment() = default;
  explicit constexpr Alignment(T a) noexcept : alignment(a) {}

  // The alignment is either a specific index value or is ambiguous.
  [[nodiscard]] constexpr bool isAmbiguous() const noexcept {
    return alignment == MinValue_v<T>;
  }

  [[nodiscard]] constexpr T getAlignment() const noexcept { return alignment; }

  void setAlignment(T value) noexcept { alignment = value; }

  T alignment = MinValue_v<T>;
};

// ranges that are boundedNone are always ambiguous
template <typename T, bool Stridable>
struct Alignment<T, BoundedRangeType::boundedNone, Stridable> {

  Alignment() = default;
  explicit constexpr Alignment(T) noexcept {}

  // The alignment is either a specific index value or is ambiguous.
  [[nodiscard]] static constexpr bool isAmbiguous() noexcept { return true; }

  [[nodiscard]] static constexpr auto getAlignment() noexcept {
    return MinValue_v<T>;
  }

  static constexpr void setAlignment(T) noexcept {}
};

// non-strided ranges that are boundedHigh are always ambiguous
template <typename T>
struct Alignment<T, BoundedRangeType::boundedHigh, false> {

  Alignment() = default;
  explicit constexpr Alignment(T) noexcept {}

  // The alignment is either a specific index value or is ambiguous.
  [[nodiscard]] static constexpr bool isAmbiguous() noexcept { return true; }

  [[nodiscard]] static constexpr auto getAlignment() noexcept {
    return MinValue_v<T>;
  }

  static constexpr void setAlignment(T) noexcept {}
};

template <typename T1, BoundedRangeType BoundedType1, bool Stridable1,
          typename T2, BoundedRangeType BoundedType2, bool Stridable2>
[[nodiscard]] constexpr bool
operator==(Alignment<T1, BoundedType1, Stridable1> const &lhs,
           Alignment<T2, BoundedType2, Stridable2> const &rhs) noexcept {

  return lhs.getAlignment() == rhs.getAlignment();
}

template <typename T1, BoundedRangeType BoundedType1, bool Stridable1,
          typename T2, BoundedRangeType BoundedType2, bool Stridable2>
[[nodiscard]] constexpr bool
operator!=(Alignment<T1, BoundedType1, Stridable1> const &lhs,
           Alignment<T2, BoundedType2, Stridable2> const &rhs) noexcept {

  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
template <typename T, typename S>
constexpr T round_down(T value, T base, S stride) noexcept {

  HPX_ASSERT(stride > 0);
  return (stride == 1) ? value : value - ((value - base) % stride);
}

template <typename T, typename S>
constexpr T round_up(T value, T base, S stride) noexcept {

  HPX_ASSERT(stride > 0);
  return (stride == 1) ? value : value + ((base - value) % stride);
}
} // namespace detail

//-----------------------------------------------------------------------------
template <typename T, BoundedRangeType BoundedType, bool Stridable>
struct Range {

  using idxType = T;
  using indexType = T;

  static_assert(std::is_integral_v<idxType>,
                "the index type of a range must be integral");

  constexpr BoundedRangeType boundedType() const noexcept {
    return bounds_.boundedType();
  }
  static constexpr bool stridable() noexcept { return Stridable; }

  static_assert(std::is_integral_v<idxType> || std::is_enum_v<idxType> ||
                    std::is_same_v<bool, idxType>,
                "idxType must be an integral, boolean, or enumerated type");

  // default constructed range
  explicit constexpr Range() noexcept : bounds_(), stride_(), alignment_() {}

  // ..
  explicit constexpr Range(RangeInit, RangeInit) noexcept
      : bounds_(RangeInit::noValue, RangeInit::noValue), stride_(),
        alignment_() {}

  // low..high
  constexpr Range(T low, T high,
                  BoundsCategoryType type = BoundsCategoryType::Closed) noexcept
      : bounds_(low, high, type), stride_(), alignment_(low) {}

  // low..
  explicit constexpr Range(T low, RangeInit = RangeInit::noValue) noexcept
      : bounds_(low, RangeInit::noValue), stride_(), alignment_(low) {}

  // ..high
  constexpr Range(RangeInit, T high,
                  BoundsCategoryType type = BoundsCategoryType::Closed) noexcept
      : bounds_(RangeInit::noValue, high, type), stride_(), alignment_() {}

  // .., with stride
  constexpr Range(RangeInit, RangeInit, detail::StrideType_t<T> stride) noexcept
      : bounds_(RangeInit::noValue, RangeInit::noValue), stride_(stride),
        alignment_() {}

  // low..high, with stride
  constexpr Range(T low, T high, detail::StrideType_t<T> stride,
                  BoundsCategoryType type = BoundsCategoryType::Closed) noexcept
      : bounds_(low, high, type), stride_(stride),
        alignment_(stride > 0 ? low : high) {}

  // low.., with stride
  constexpr Range(T low, RangeInit, detail::StrideType_t<T> stride) noexcept
      : bounds_(low, RangeInit::noValue), stride_(stride),
        alignment_(stride > 0 ? low : detail::MinValue_v<T>) {}

  // ..high, with stride
  constexpr Range(RangeInit, T high, detail::StrideType_t<T> stride,
                  BoundsCategoryType type = BoundsCategoryType::Closed) noexcept
      : bounds_(RangeInit::noValue, high, type), stride_(stride),
        alignment_(stride < 0 ? high : detail::MinValue_v<T>) {}

  template <typename T1, bool Stridable1>
    requires(std::is_convertible_v<T1, T> && (!Stridable1 || Stridable) &&
             BoundedType == BoundedRangeType::bounded)
  Range(Range<T1, BoundedRangeType::bounded, Stridable1> const &rhs)
      : bounds_(rhs.getFirstIndex(), rhs.getLastIndex(),
                BoundsCategoryType::Open),
        stride_(rhs.stride()), alignment_(rhs.alignment()) {}

  decltype(auto) these() const { return iterate(*this); }

  [[nodiscard]] constexpr auto getFirstIndex() const noexcept {
    return bounds_.getFirstIndex();
  }
  [[nodiscard]] constexpr auto getLastIndex() const noexcept {
    return bounds_.getLastIndex();
  }

  [[nodiscard]] constexpr auto stride() const noexcept {
    return stride_.getStride();
  }

  // A range is non-ambiguously aligned if
  // - its alignment is non-ambiguous or
  // - its stride is either 1 or -1.
  [[nodiscard]] constexpr bool isNaturallyAligned() const noexcept {
    return !alignment_.isAmbiguous() || !stride_.hasAmbiguousValue();
  }

  // A range is ambiguously aligned if
  // - its alignment is ambiguous and
  // - its stride is neither 1 nor -1.
  [[nodiscard]] constexpr bool isAmbiguous() const noexcept {
    return alignment_.isAmbiguous() && stride_.hasAmbiguousValue();
  }

  [[nodiscard]] constexpr auto alignment() const noexcept {
    return alignment_.getAlignment();
  }
  void setAlignment(T value) noexcept { alignment_.setAlignment(value); }

  [[nodiscard]] static constexpr bool isIterable() noexcept {
    return detail::Bounds<T, BoundedType>::isIterable();
  }

  // Returns true if argument r is a fully bounded range, false otherwise.
  [[nodiscard]] static constexpr bool isBounded() noexcept {
    return BoundedType == BoundedRangeType::bounded;
  }

  // Returns true if this range's low bound is not -inf, and false otherwise.
  [[nodiscard]] static constexpr bool hasLowBound() noexcept {
    return BoundedType == BoundedRangeType::bounded ||
           BoundedType == BoundedRangeType::boundedLow;
  }

  // Returns the range's low bound. If the range does not have a low bound
  // (e.g., ..10), the behavior is undefined.
  [[nodiscard]] constexpr auto lowBound() const noexcept {
    HPX_ASSERT(hasLowBound());
    return bounds_.getFirstIndex();
  }

  // Returns the range'’'s aligned low bound. If this bound is undefined (e.g.,
  // ..10 by -2), the behavior is undefined.
  [[nodiscard]] constexpr auto low() const noexcept {
    return bounds_.getFirstIndex();
  }

  // Returns true if this range's high bound is not inf, and false otherwise.
  [[nodiscard]] static constexpr bool hasHighBound() noexcept {
    return BoundedType == BoundedRangeType::bounded ||
           BoundedType == BoundedRangeType::boundedHigh;
  }

  // Return the range's high bound. If the range does not have a high bound
  // (e.g., 1..), the behavior is undefined.
  [[nodiscard]] constexpr auto highBound() const noexcept {
    HPX_ASSERT(hasHighBound());
    return bounds_.getLastIndex() - 1;
  }

  // Return the range's high bound. If the range does not have a high bound
  // (e.g., 1..), the behavior is undefined.
  [[nodiscard]] constexpr auto high() const noexcept {
    return bounds_.getLastIndex() - 1;
  }

  // Returns the number of values represented by this range as an integer.
  [[nodiscard]] constexpr auto size() const noexcept {
    HPX_ASSERT(isBounded());
    auto stride = std::abs(stride_.getStride());
    auto num = highBound() - lowBound() + stride;
    return stride == 1 ? num : num / stride;
  }

  // Returns true if the sequence represented by the range is empty and false
  // otherwise. If the range is ambiguous, the behavior is undefined.
  [[nodiscard]] constexpr bool isEmpty() const noexcept {
    HPX_ASSERT(!isAmbiguous());
    return size() == 0;
  }

  // Returns true if the range has a first index, false otherwise.
  [[nodiscard]] constexpr bool hasFirst() const noexcept {

    if (stride_.getStride() > 0) {
      return bounds_.hasFirst() && isNaturallyAligned();
    }
    return bounds_.hasLast() && isNaturallyAligned();
  }

  [[nodiscard]] constexpr auto first() const noexcept {

    if (stride_.getStride() > 0) {
      HPX_ASSERT(bounds_.hasFirst());
      return bounds_.getFirstIndex();
    }

    HPX_ASSERT(bounds_.hasLast());
    return bounds_.getLastIndex() - 1;
  }

  // Returns true if the range has a first index, false otherwise.
  [[nodiscard]] constexpr bool hasLast() const noexcept {

    if (stride_.getStride() > 0) {
      return bounds_.hasLast() && isNaturallyAligned();
    }
    return bounds_.hasFirst() && isNaturallyAligned();
  }

  [[nodiscard]] constexpr auto last() const noexcept {

    if (stride_.getStride() > 0) {
      HPX_ASSERT(bounds_.hasLast());
      return detail::round_down(bounds_.getLastIndex() - 1,
                                bounds_.getFirstIndex(), stride_.getStride());
    }

    HPX_ASSERT(bounds_.hasFirst());
    return detail::round_up(bounds_.getFirstIndex(), bounds_.getLastIndex() - 1,
                            -stride_.getStride());
  }

  // Returns true if the range's represented sequence contains ind, false
  // otherwise. It is an error to invoke contains if the represented sequence is
  // not defined.
  [[nodiscard]] constexpr bool contains(idxType ind) {
    HPX_ASSERT(hasLowBound() && hasHighBound());
    return low() <= ind && ind <= high();
  }

  // Returns true if the range other is contained within this one, false
  // otherwise.
  template <typename T1, BoundedRangeType Bounded1, bool Stridable1>
  [[nodiscard]] constexpr bool
  contains(Range<T1, Bounded1, Stridable1> const &other) {
    HPX_ASSERT(hasLowBound() && hasHighBound());
    return low() <= other.low() && other.high() <= high();
  }

  // Assigning one range to another results in the target range copying the low
  // and high bounds, stride, and alignment from the source range.
  //
  // Range assignment is legal when:
  // - An implicit conversion is allowed from idxType of the source range to
  //   idxType of the destination range type,
  // - the two range types have the same boundedType, and
  // - either the destination range is stridable or the source range is not
  //   stridable.
  template <typename T1, bool Stridable1,
            typename = std::enable_if_t<std::is_convertible_v<T1, T> &&
                                        (Stridable || !Stridable1)>>
  Range &operator=(Range<T1, BoundedType, Stridable1> const &rhs) {
    if (this != &rhs) {
      bounds_ = detail::Bounds<T, BoundedType>(rhs.getFirstIndex(),
                                               rhs.getLastIndex() + 1);
      stride_ = detail::Stridable<T, BoundedType, Stridable>(rhs.stride());
      alignment_ =
          detail::Alignment<T, BoundedType, Stridable>(rhs.alignment());
    }
    return *this;
  }

  // Returns an integer representing the zero-based ordinal value of ind within
  // the range's sequence of values if it is a member of the sequence.
  // Otherwise, returns -1. It is an error to invoke indexOrder if the
  // represented sequence is not defined or the range does not have a first
  // index. The indexOrder procedure is the reverse of orderToIndex.
  constexpr std::int64_t indexOrder(idxType idx) const noexcept {

    std::int64_t result;
    if (stride() > 0) {
      result = (idx - first() + stride() - 1) / stride();
    } else {
      result = (first() - idx - stride() - 1) / -stride();
    }

    if (result < 0 || result > size()) {
      return -1;
    }
    return result;
  }

  // Returns the zero-based ord-th element of this range's represented sequence.
  // It is an error to invoke orderToIndex if the range is not defined, or if
  // ord is negative or greater than the range's size. The orderToIndex
  // procedure is the reverse of indexOrder.
  constexpr idxType orderToIndex(std::int64_t order) const noexcept {

    HPX_ASSERT(order >= 0 && order <= size());
    auto result = first() + order * stride();

    return result;
  }

private:
  template <typename T1, BoundedRangeType BoundedType1, bool Stridable1,
            typename T2, BoundedRangeType BoundedType2, bool Stridable2>
  friend constexpr bool
  operator==(Range<T1, BoundedType1, Stridable1> const &lhs,
             Range<T2, BoundedType2, Stridable2> const &rhs) noexcept;

  template <typename T1, BoundedRangeType BoundedType1, bool Stridable1,
            typename T2, BoundedRangeType BoundedType2, bool Stridable2>
  friend constexpr bool
  operator!=(Range<T1, BoundedType1, Stridable1> const &lhs,
             Range<T2, BoundedType2, Stridable2> const &rhs) noexcept;

  [[no_unique_address]] detail::Bounds<T, BoundedType> bounds_;
  [[no_unique_address]] detail::Stridable<T, BoundedType, Stridable> stride_;
  [[no_unique_address]] detail::Alignment<T, BoundedType, Stridable> alignment_;
};

//-----------------------------------------------------------------------------
template <typename T> Range(T, T) -> Range<T, BoundedRangeType::bounded, false>;

template <typename T>
Range(T, T, T) -> Range<T, BoundedRangeType::bounded, true>;

//-----------------------------------------------------------------------------
// Returns true if the type T is a range type.
template <typename T> inline constexpr bool isRangeType = false;

template <typename T, BoundedRangeType Bounded, bool Stridable>
inline constexpr bool isRangeType<Range<T, Bounded, Stridable>> = true;

// Returns true if the value T is a range value.
template <typename T> [[nodiscard]] constexpr bool isRangeValue(T) noexcept {
  return false;
}

template <typename T, BoundedRangeType Bounded, bool Stridable>
[[nodiscard]] constexpr bool
isRangeValue(Range<T, Bounded, Stridable> const &) noexcept {
  return true;
}

//-----------------------------------------------------------------------------
template <typename T, bool Stridable = false>
using BoundedRange = Range<T, BoundedRangeType::bounded, Stridable>;

template <typename T, bool Stridable = false>
using BoundedLowRange = Range<T, BoundedRangeType::boundedLow, Stridable>;

template <typename T, bool Stridable = false>
using BoundedHighRange = Range<T, BoundedRangeType::boundedHigh, Stridable>;

template <typename T, bool Stridable = false>
using BoundedNoneRange = Range<T, BoundedRangeType::boundedNone, Stridable>;

template <typename T> inline constexpr bool is_range_v = false;

template <typename T, BoundedRangeType BoundedType, bool Stridable>
inline constexpr bool is_range_v<Range<T, BoundedType, Stridable>> = true;

//-----------------------------------------------------------------------------
// Ranges can be compared using equality and inequality.
//
// Returns true if the two ranges have the same represented sequence or the same
// four primary properties, and false otherwise.
template <typename T1, BoundedRangeType BoundedType1, bool Stridable1,
          typename T2, BoundedRangeType BoundedType2, bool Stridable2>
[[nodiscard]] constexpr bool
operator==(Range<T1, BoundedType1, Stridable1> const &lhs,
           Range<T2, BoundedType2, Stridable2> const &rhs) noexcept {

  return lhs.bounds_ == rhs.bounds_ && lhs.stride_ == rhs.stride_ &&
         lhs.alignment_ == rhs.alignment_;
}

template <typename T1, BoundedRangeType BoundedType1, bool Stridable1,
          typename T2, BoundedRangeType BoundedType2, bool Stridable2>
[[nodiscard]] constexpr bool
operator!=(Range<T1, BoundedType1, Stridable1> const &lhs,
           Range<T2, BoundedType2, Stridable2> const &rhs) noexcept {

  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
// The align operator can be applied to any range, and creates a new range with
// the given alignment.
//
// The resulting range has the same low and high bounds and stride as the source
// range. The alignment equals the align operator's right operand and therefore
// is not ambiguous.
template <typename T, BoundedRangeType BoundedType, bool Stridable,
          typename AlignType>
  requires(std::is_convertible_v<AlignType, T>)
auto align(Range<T, BoundedType, Stridable> const &rhs, AlignType alignment) {

  auto result =
      Range<T, BoundedType, true>(rhs.getFirstIndex(), rhs.getLastIndex(),
                                  rhs.stride(), BoundsCategoryType::Open);

  result.setAlignment(alignment);
  return result;
}

//-----------------------------------------------------------------------------
// The result of the by operator is a range with the following primary
// properties:
// - The low and upper bounds are the same as those of the base range.
// - The stride is the product of the base range's stride and the step, cast to
//   the base range's stride type before multiplying.
// - The alignment is:
//    - the aligned low bound of the base range, if such exists and the stride
//      is positive;
//    - the aligned high bound of the base range, if such exists and the stride
//      is negative;
//    - the same as that of the base range, otherwise.
template <typename T, BoundedRangeType BoundedType, bool Stridable,
          typename StepType>
  requires(std::is_integral_v<StepType>)
auto by(Range<T, BoundedType, Stridable> const &rhs, StepType step) {

  auto result = Range<T, BoundedType, true>(
      rhs.getFirstIndex(), rhs.getLastIndex(),
      rhs.stride() * detail::StrideType_t<T>(step), BoundsCategoryType::Open);

  return result;
}

// The # operator takes a range and an integral count and creates a new range
// containing the specified number of indices. The low or high bound of the left
// operand is preserved, and the other bound adjusted to provide the specified
// number of indices. If the count is positive, indices are taken from the start
// of the range; if the count is negative, indices are taken from the end of the
// range.
template <typename T, BoundedRangeType BoundedType, bool Stridable,
          typename Count>
  requires(std::is_integral_v<Count>)
auto count(Range<T, BoundedType, Stridable> const &rhs, Count n) {

  auto distance = n * rhs.stride();
  auto offset = rhs.stride() > 0 ? rhs.stride() - 1 : rhs.stride() + 2;

  // if the range is bounded, then it must be unambiguous
  HPX_ASSERT(!rhs.isAmbiguous() || (distance > 0 && rhs.hasFirst()) ||
             (distance < 0 && rhs.hasLast()));

  if (distance > 0) {

    // if the range is bounded, then 'n' must be smaller than its size
    HPX_ASSERT(rhs.boundedType() != BoundedRangeType::bounded ||
               n <= rhs.size());
    return Range<T, BoundedRangeType::bounded, Stridable>(
        rhs.getFirstIndex(), rhs.getFirstIndex() + distance - 1, rhs.stride());
  }

  // if the range is bounded, then 'n' must be smaller than its size
  HPX_ASSERT(rhs.boundedType() != BoundedRangeType::bounded || n <= rhs.size());
  return Range<T, BoundedRangeType::bounded, Stridable>(
      rhs.getLastIndex() + distance + offset, rhs.getLastIndex(), rhs.stride(),
      BoundsCategoryType::Open);
}

// The + and - operators apply the scalar via the operator to the range's low
// and high bounds, producing a shifted version of the range. If the operand
// range is unbounded above or below, the missing bounds are ignored. The index
// type of the resulting range is the type of the value that would result from
// an addition between the scalar value and a value with the range’s index type.
// The bounded and stridable parameters for the result range are the same as for
// the input range.
//
// The stride of the resulting range is the same as the stride of the original.
//
// The alignment of the resulting range is shifted by the same amount as the
// high and low bounds. It is permissible to apply the shift operators to a
// range that is ambiguously aligned. In that case, the resulting range is also
// ambiguously aligned.
template <typename T, BoundedRangeType BoundedType, bool Stridable,
          typename Integral>
  requires(std::is_integral_v<Integral>)
constexpr auto operator+(Range<T, BoundedType, Stridable> const &rhs,
                         Integral n) noexcept {

  auto result = Range<T, BoundedType, Stridable>(
      rhs.hasFirst() ? rhs.getFirstIndex() + n : rhs.getFirstIndex(),
      rhs.hasLast() ? rhs.getLastIndex() + n : rhs.getLastIndex(), rhs.stride(),
      BoundsCategoryType::Open);

  if (rhs.isNaturallyAligned())
    result.setAlignment(rhs.alignment() + n);

  return result;
}

template <typename Integral, typename T, BoundedRangeType BoundedType,
          bool Stridable>
  requires(std::is_integral_v<Integral>)
constexpr auto operator+(Integral n,
                         Range<T, BoundedType, Stridable> const &rhs) noexcept {

  auto result = Range<T, BoundedType, Stridable>(
      rhs.hasFirst() ? rhs.getFirstIndex() + n : rhs.getFirstIndex(),
      rhs.hasLast() ? rhs.getLastIndex() + n : rhs.getLastIndex(), rhs.stride(),
      BoundsCategoryType::Open);

  if (rhs.isNaturallyAligned())
    result.setAlignment(rhs.alignment() + n);

  return result;
}

template <typename T, BoundedRangeType BoundedType, bool Stridable,
          typename Integral>
  requires(std::is_integral_v<Integral>)
constexpr auto operator-(Range<T, BoundedType, Stridable> const &rhs,
                         Integral n) noexcept {

  auto result = Range<T, BoundedType, Stridable>(
      rhs.hasFirst() ? rhs.getFirstIndex() - n : rhs.getFirstIndex(),
      rhs.hasLast() ? rhs.getLastIndex() - n : rhs.getLastIndex(), rhs.stride(),
      BoundsCategoryType::Open);

  if (rhs.isNaturallyAligned())
    result.setAlignment(rhs.alignment() - n);

  return result;
}

// Ranges can be sliced using other ranges to create new sub-ranges. The
// resulting range represents the intersection between the two ranges'
// represented sequences. The stride and alignment of the resulting range are
// adjusted as needed to make this true. idxType and the sign of the stride of
// the result are determined by the first operand.
template <typename T1, BoundedRangeType BoundedType1, bool Stridable1,
          typename T2, BoundedRangeType BoundedType2, bool Stridable2>
constexpr auto slice(Range<T1, BoundedType1, Stridable1> const &lhs,
                     Range<T2, BoundedType2, Stridable2> const &rhs) noexcept {

  auto result = Range<T1, BoundedType1, Stridable1>(
      rhs.getFirstIndex(), rhs.getLastIndex(), rhs.stride(),
      BoundsCategoryType::Open);

  // TODO: needs implementation

  return result;
}

//-----------------------------------------------------------------------------
template <typename T, BoundedRangeType BoundedRange, bool Stridable>
std::ostream &operator<<(std::ostream &os,
                         Range<T, BoundedRange, Stridable> const &r) {

  if (r.hasLowBound()) {
    os << r.lowBound();
  }
  os << "..";
  if (r.hasHighBound()) {
    os << r.highBound();
  }
  if (r.stride() != 1) {
    os << " by " << r.stride();
  }
  return os;
}
} // namespace chplx
