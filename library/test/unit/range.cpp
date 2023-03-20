//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

#include <cstddef>

void testBounded() {

  // default stride
  {
    // 1..10
    chplx::Range<int, chplx::BoundedRangeType::bounded, false> r1(1, 10);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 1);
    HPX_TEST_EQ(r1.last(), 10);
    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), 1);
    HPX_TEST_EQ(r1.size(), 10);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());

    // 1..<10
    chplx::Range<int, chplx::BoundedRangeType::bounded, false> r2(
        1, 10, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), 1);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(r2.hasFirst());
    HPX_TEST(r2.hasLast());
    HPX_TEST(r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.first(), 1);
    HPX_TEST_EQ(r2.last(), 9);
    HPX_TEST_EQ(r2.lowBound(), 1);
    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), 1);
    HPX_TEST_EQ(r2.size(), 9);

    HPX_TEST(r2.isIterable());
    HPX_TEST(r2.isNaturallyAligned());
  }

  {
    // 1..10 by -1
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r1(1, 10, -1);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 10);
    HPX_TEST_EQ(r1.last(), 1);
    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), -1);
    HPX_TEST_EQ(r1.size(), 10);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());

    // 1..<10 by -1
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r2(
        1, 10, -1, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), 1);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(r2.hasFirst());
    HPX_TEST(r2.hasLast());
    HPX_TEST(r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.first(), 9);
    HPX_TEST_EQ(r2.last(), 1);
    HPX_TEST_EQ(r2.lowBound(), 1);
    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), -1);
    HPX_TEST_EQ(r2.size(), 9);

    HPX_TEST(r2.isIterable());
    HPX_TEST(r2.isNaturallyAligned());
  }

  // explicit stride
  {
    // 1..10 by 2
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r1(1, 10, 2);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 1);
    HPX_TEST_EQ(r1.last(), 9);
    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), 2);
    HPX_TEST_EQ(r1.size(), 5);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());

    // 1..<10 by 2
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r2(
        1, 10, 2, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), 1);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(r2.hasFirst());
    HPX_TEST(r2.hasLast());
    HPX_TEST(r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.first(), 1);
    HPX_TEST_EQ(r2.last(), 9);
    HPX_TEST_EQ(r2.lowBound(), 1);
    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), 2);
    HPX_TEST_EQ(r2.size(), 5);

    HPX_TEST(r2.isIterable());
    HPX_TEST(r2.isNaturallyAligned());
  }

  {
    // 1..10 by -2
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r1(1, 10, -2);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 10);
    HPX_TEST_EQ(r1.last(), 2);
    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), -2);
    HPX_TEST_EQ(r1.size(), 5);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());

    // 1..<10 by -2
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r2(
        1, 10, -2, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), 1);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(r2.hasFirst());
    HPX_TEST(r2.hasLast());
    HPX_TEST(r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.first(), 9);
    HPX_TEST_EQ(r2.last(), 1);
    HPX_TEST_EQ(r2.lowBound(), 1);
    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), -2);
    HPX_TEST_EQ(r2.size(), 5);

    HPX_TEST(r2.isIterable());
    HPX_TEST(r2.isNaturallyAligned());
  }

  {
    // 1..10 by 3
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r1(1, 10, 3);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 1);
    HPX_TEST_EQ(r1.last(), 10);
    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), 3);
    HPX_TEST_EQ(r1.size(), 4);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());

    // 1..<10 by 3
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r2(
        1, 10, 3, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), 1);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(r2.hasFirst());
    HPX_TEST(r2.hasLast());
    HPX_TEST(r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.first(), 1);
    HPX_TEST_EQ(r2.last(), 7);
    HPX_TEST_EQ(r2.lowBound(), 1);
    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), 3);
    HPX_TEST_EQ(r2.size(), 3);

    HPX_TEST(r2.isIterable());
    HPX_TEST(r2.isNaturallyAligned());
  }

  {
    // 1..10 by -3
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r1(1, 10, -3);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 10);
    HPX_TEST_EQ(r1.last(), 1);
    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), -3);
    HPX_TEST_EQ(r1.size(), 4);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());

    // 1..<10 by -3
    chplx::Range<int, chplx::BoundedRangeType::bounded, true> r2(
        1, 10, -3, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), 1);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(r2.hasFirst());
    HPX_TEST(r2.hasLast());
    HPX_TEST(r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.first(), 9);
    HPX_TEST_EQ(r2.last(), 3);
    HPX_TEST_EQ(r2.lowBound(), 1);
    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), -3);
    HPX_TEST_EQ(r2.size(), 3);

    HPX_TEST(r2.isIterable());
    HPX_TEST(r2.isNaturallyAligned());
  }
}

void testBoundedLow() {

  // default stride
  {
    // 1..
    chplx::Range<int, chplx::BoundedRangeType::boundedLow, false> r1(1);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), chplx::detail::MaxValue<int>::value);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(!r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(!r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 1);
    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.stride(), 1);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());
  }

  // explicit stride
  {
    // 1.. by 2
    chplx::Range<int, chplx::BoundedRangeType::boundedLow, true> r1(
        1, chplx::RangeInit::noValue, 2);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), chplx::detail::MaxValue<int>::value);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(!r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(!r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 1);
    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.stride(), 2);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());
  }

  {
    // 1.. by -2
    chplx::Range<int, chplx::BoundedRangeType::boundedLow, true> r1(
        1, chplx::RangeInit::noValue, -2);

    HPX_TEST_EQ(r1.getFirstIndex(), 1);
    HPX_TEST_EQ(r1.getLastIndex(), chplx::detail::MaxValue<int>::value);

    HPX_TEST(!r1.hasFirst());
    HPX_TEST(!r1.hasLast());
    HPX_TEST(r1.hasLowBound());
    HPX_TEST(!r1.hasHighBound());

    HPX_TEST_EQ(r1.lowBound(), 1);
    HPX_TEST_EQ(r1.stride(), -2);

    HPX_TEST(!r1.isIterable());
    HPX_TEST(!r1.isNaturallyAligned());
  }
}

void testBoundedHigh() {

  // default stride
  {
    // ..10
    chplx::Range<int, chplx::BoundedRangeType::boundedHigh, false> r1(
        chplx::RangeInit::noValue, 10);

    HPX_TEST_EQ(r1.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(!r1.hasFirst());
    HPX_TEST(r1.hasLast());
    HPX_TEST(!r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.last(), 10);
    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), 1);

    HPX_TEST(!r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());

    // ..<10
    chplx::Range<int, chplx::BoundedRangeType::boundedHigh, false> r2(
        chplx::RangeInit::noValue, 10, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(!r2.hasFirst());
    HPX_TEST(r2.hasLast());
    HPX_TEST(!r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.last(), 9);
    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), 1);

    HPX_TEST(!r2.isIterable());
    HPX_TEST(r2.isNaturallyAligned());
  }

  // explicit stride
  {
    // ..10 by 2
    chplx::Range<int, chplx::BoundedRangeType::boundedHigh, true> r1(
        chplx::RangeInit::noValue, 10, 2);

    HPX_TEST_EQ(r1.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(!r1.hasFirst());
    HPX_TEST(!r1.hasLast());
    HPX_TEST(!r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), 2);

    HPX_TEST(!r1.isIterable());
    HPX_TEST(!r1.isNaturallyAligned());

    // ..<10 by 2
    chplx::Range<int, chplx::BoundedRangeType::boundedHigh, true> r2(
        chplx::RangeInit::noValue, 10, 2, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(!r2.hasFirst());
    HPX_TEST(!r2.hasLast());
    HPX_TEST(!r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), 2);

    HPX_TEST(!r2.isIterable());
    HPX_TEST(!r2.isNaturallyAligned());
  }

  {
    // ..10 by -2
    chplx::Range<int, chplx::BoundedRangeType::boundedHigh, true> r1(
        chplx::RangeInit::noValue, 10, -2);

    HPX_TEST_EQ(r1.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r1.getLastIndex(), 11);

    HPX_TEST(r1.hasFirst());
    HPX_TEST(!r1.hasLast());
    HPX_TEST(!r1.hasLowBound());
    HPX_TEST(r1.hasHighBound());

    HPX_TEST_EQ(r1.first(), 10);
    HPX_TEST_EQ(r1.highBound(), 10);
    HPX_TEST_EQ(r1.stride(), -2);

    HPX_TEST(r1.isIterable());
    HPX_TEST(r1.isNaturallyAligned());

    // ..<10 by -2
    chplx::Range<int, chplx::BoundedRangeType::boundedHigh, true> r2(
        chplx::RangeInit::noValue, 10, -2, chplx::BoundsCategoryType::Open);

    HPX_TEST_EQ(r2.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r2.getLastIndex(), 10);

    HPX_TEST(r2.hasFirst());
    HPX_TEST(!r2.hasLast());
    HPX_TEST(!r2.hasLowBound());
    HPX_TEST(r2.hasHighBound());

    HPX_TEST_EQ(r2.first(), 9);
    HPX_TEST_EQ(r2.highBound(), 9);
    HPX_TEST_EQ(r2.stride(), -2);

    HPX_TEST(r2.isIterable());
    HPX_TEST(r2.isNaturallyAligned());
  }
}

void testUnbounded() {

  // default stride
  {
    // ..
    chplx::Range<int, chplx::BoundedRangeType::boundedNone, false> r1;

    HPX_TEST_EQ(r1.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r1.getLastIndex(), chplx::detail::MaxValue<int>::value);

    HPX_TEST_EQ(r1.stride(), 1);

    HPX_TEST(!r1.isIterable());
    HPX_TEST(!r1.isNaturallyAligned());
  }

  // explicit stride
  {
    // .. by 2
    chplx::Range<int, chplx::BoundedRangeType::boundedNone, true> r1(
        chplx::RangeInit::noValue, chplx::RangeInit::noValue, 2);

    HPX_TEST_EQ(r1.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r1.getLastIndex(), chplx::detail::MaxValue<int>::value);

    HPX_TEST_EQ(r1.stride(), 2);

    HPX_TEST(!r1.isIterable());
    HPX_TEST(!r1.isNaturallyAligned());
  }

  {
    // .. by -2
    chplx::Range<int, chplx::BoundedRangeType::boundedNone, true> r1(
        chplx::RangeInit::noValue, chplx::RangeInit::noValue, -2);

    HPX_TEST_EQ(r1.getFirstIndex(), chplx::detail::MinValue<int>::value);
    HPX_TEST_EQ(r1.getLastIndex(), chplx::detail::MaxValue<int>::value);

    HPX_TEST_EQ(r1.stride(), -2);

    HPX_TEST(!r1.isIterable());
    HPX_TEST(!r1.isNaturallyAligned());
  }
}

template <typename T, chplx::BoundedRangeType BoundedType, bool Stridable>
void testIndexOrder(chplx::Range<T, BoundedType, Stridable> const &r,
                    std::vector<T> const &elements) {

  std::size_t count = 0;
  for (std::size_t i = 0; i != r.size(); ++i) {

    auto idx = r.orderToIndex(i);
    HPX_TEST_EQ(idx, elements[i]);

    std::size_t order = r.indexOrder(idx);
    HPX_TEST_EQ(i, order);
    ++count;
  }
  HPX_TEST_EQ(count, elements.size());
}

void testIndexOrder() {

  testIndexOrder(chplx::Range(0, 5), {0, 1, 2, 3, 4, 5});
  testIndexOrder(chplx::Range(0, 5, 2), {0, 2, 4});
  testIndexOrder(chplx::Range(0, 5, -1), {5, 4, 3, 2, 1, 0});
  testIndexOrder(chplx::Range(0, 5, -2), {5, 3, 1});

  testIndexOrder(chplx::Range(0, 6, 2), {0, 2, 4, 6});
  testIndexOrder(chplx::Range(0, 6, -1), {6, 5, 4, 3, 2, 1, 0});
  testIndexOrder(chplx::Range(0, 6, -2), {6, 4, 2, 0});

  testIndexOrder(chplx::Range(1, 5, 2), {1, 3, 5});
  testIndexOrder(chplx::Range(1, 5, -1), {5, 4, 3, 2, 1});
  testIndexOrder(chplx::Range(1, 5, -2), {5, 3, 1});

  testIndexOrder(chplx::Range(1, 6, 2), {1, 3, 5});
  testIndexOrder(chplx::Range(1, 6, -1), {6, 5, 4, 3, 2, 1});
  testIndexOrder(chplx::Range(1, 6, -2), {6, 4, 2});
}

int main() {

  testBounded();
  testBoundedLow();
  testBoundedHigh();
  testUnbounded();

  testIndexOrder();

  return hpx::util::report_errors();
}
