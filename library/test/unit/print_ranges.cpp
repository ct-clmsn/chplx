//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

#include <string>
#include <strstream>

using namespace chplx;

template <typename... Ts> void writeStream(std::ostream &os, Ts &&...ts) {
  (os << ... << ts);
}

template <typename Range>
void testWriteRange(char const *expr, Range const &r, char const *expected) {

  std::stringstream strm;
  writeStream(strm, expr, " = ");
  // clang-format off
  if (r.boundedType() == BoundedRangeType::bounded ||
      ((isBoolType<typename Range::idxType> ||
        isEnumType<typename Range::idxType>) &&
        r.hasFirst())) {
    // clang-format on

    // The range is fully bounded, either because it is a bounded range or
    // because it is unbounded but defined on booleans or an enum, so - print
    // its entire sequence.
    bool first = true;
    for (auto i : chplx::iterate(r)) {
      if (!first)
        writeStream(strm, ", ");
      writeStream(strm, i);
      first = false;
    }

  } else if (r.hasFirst()) {
    // The range is not fully bounded, but its sequence has a starting point
    // - print the first three indices.  Note that in this and the next
    // case the sequence can be either increasing or decreasing.
    for (auto i : iterate(count(r, 3)))
      writeStream(strm, i, ", ");
    writeStream(strm, "...");

  } else if (r.hasLast()) {
    // The range is not fully bounded, but its sequence has an ending point.
    // Print the last three indices.
    writeStream(strm, "...");
    for (auto i : iterate(count(r, -3)))
      writeStream(strm, ", ", i);

  } else if (r.stride() == 1 || r.stride() == -1) {
    // If we are here, the range is fully unbounded.
    writeStream(strm, "all integers, ", r.stride() > 0 ? "increasing" : "decreasing");

  } else {
    // We got a more complex range, do not elaborate.
  }

  HPX_TEST_EQ(std::string(expected), strm.str());
}

constexpr auto Open = BoundsCategoryType::Open;

void boundedRanges() {

  testWriteRange("1..10", BoundedRange<int>(1, 10),
                 "1..10 = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10");
  testWriteRange("-3..3", BoundedRange<int>(-3, 3),
                 "-3..3 = -3, -2, -1, 0, 1, 2, 3");
  testWriteRange("1..10 # 3", count(BoundedRange<int>(1, 10), 3),
                 "1..10 # 3 = 1, 2, 3");
  testWriteRange("1..10 # -3", count(BoundedRange<int>(1, 10), -3),
                 "1..10 # -3 = 8, 9, 10");
  testWriteRange("1..10 by 2", by(BoundedRange<int>(1, 10), 2),
                 "1..10 by 2 = 1, 3, 5, 7, 9");

  testWriteRange("1..<10", BoundedRange<int>(1, 10, Open),
                 "1..<10 = 1, 2, 3, 4, 5, 6, 7, 8, 9");
  testWriteRange("-3..<3", BoundedRange<int>(-3, 3, Open),
                 "-3..<3 = -3, -2, -1, 0, 1, 2");
  testWriteRange("1..<10 # 3", count(BoundedRange<int>(1, 10, Open), 3),
                 "1..<10 # 3 = 1, 2, 3");
  testWriteRange("1..<10 # -3", count(BoundedRange<int>(1, 10, Open), -3),
                 "1..<10 # -3 = 7, 8, 9");
  testWriteRange("1..<10 by 2", by(BoundedRange<int>(1, 10, Open), 2),
                 "1..<10 by 2 = 1, 3, 5, 7, 9");

  testWriteRange("1..0", BoundedRange<int>(1, 0), "1..0 = ");
  testWriteRange("0..-1", BoundedRange<int>(0, -1), "0..-1 = ");
  testWriteRange("10..1", BoundedRange<int>(10, 1), "10..1 = ");

  testWriteRange("1..0 # 3", BoundedRange<int>(1, 0), "1..0 # 3 = ");
  testWriteRange("1..0 by 2", BoundedRange<int>(1, 0), "1..0 by 2 = ");

  testWriteRange("1..10 by -1", by(BoundedRange<int>(1, 10), -1),
                 "1..10 by -1 = 10, 9, 8, 7, 6, 5, 4, 3, 2, 1");
  testWriteRange("1..10 by -2", by(BoundedRange<int>(1, 10), -2),
                 "1..10 by -2 = 10, 8, 6, 4, 2");

  testWriteRange("1..0", BoundedRange<int>(), "1..0 = ");
  testWriteRange("1..", BoundedLowRange<int>(), "1.. = 1, 2, 3, ...");
  testWriteRange("..0", BoundedHighRange<int>(), "..0 = ..., -2, -1, 0");
  testWriteRange("..", BoundedNoneRange<int>(),
                 ".. = all integers, increasing");

  testWriteRange("(1..5) + 3", BoundedRange<int>(1, 5) + 3,
                 "(1..5) + 3 = 4, 5, 6, 7, 8");
  testWriteRange("3 + (1..5)", 3 + BoundedRange<int>(1, 5),
                 "3 + (1..5) = 4, 5, 6, 7, 8");
  testWriteRange("(1..5) - 3", BoundedRange<int>(1, 5) - 3,
                 "(1..5) - 3 = -2, -1, 0, 1, 2");
}

template <template <typename T, bool Stridable> typename R>
void boundedLowRanges() {

  testWriteRange("1..", R<int, false>(1), "1.. = 1, 2, 3, ...");
  testWriteRange("-3..", R<int, false>(-3), "-3.. = -3, -2, -1, ...");

  testWriteRange("1.. # 3", count(R<int, false>(1), 3), "1.. # 3 = 1, 2, 3");

  testWriteRange("1.. by 2", by(R<int, false>(1), 2),
                 "1.. by 2 = 1, 3, 5, ...");

  testWriteRange("(1..) + 3", BoundedRange<int>(1) + 3,
                 "(1..) + 3 = 4, 5, 6, ...");
  testWriteRange("3 + (1..)", 3 + BoundedRange<int>(1),
                 "3 + (1..) = 4, 5, 6, ...");
  testWriteRange("(1..) - 3", BoundedRange<int>(1) - 3,
                 "(1..) - 3 = -2, -1, 0, ...");
}

template <template <typename T, bool Stridable> typename R>
void boundedHighRanges() {

  testWriteRange("..5", R<int, false>(RangeInit::noValue, 5),
                 "..5 = ..., 3, 4, 5");
  testWriteRange("..-5", R<int, false>(RangeInit::noValue, -5),
                 "..-5 = ..., -7, -6, -5");

  testWriteRange("..5 # -3",
                 count(BoundedRange<int>(RangeInit::noValue, 5), -3),
                 "..5 # -3 = 3, 4, 5");
  testWriteRange("..5 by -2", by(BoundedRange<int>(RangeInit::noValue, 5), -2),
                 "..5 by -2 = 5, 3, 1, ...");

  testWriteRange("..<5", R<int, false>(RangeInit::noValue, 5, Open),
                 "..<5 = ..., 2, 3, 4");
  testWriteRange("..<-5", R<int, false>(RangeInit::noValue, -5, Open),
                 "..<-5 = ..., -8, -7, -6");

  testWriteRange("(..5) + 3", BoundedRange<int>(RangeInit::noValue, 5) + 3,
                 "(..5) + 3 = ..., 6, 7, 8");
  testWriteRange("3 + (..5)", 3 + BoundedRange<int>(RangeInit::noValue, 5),
                 "3 + (..5) = ..., 6, 7, 8");
  testWriteRange("(..5) - 3", BoundedRange<int>(RangeInit::noValue, 5) - 3,
                 "(..5) - 3 = ..., 0, 1, 2");
}

template <template <typename T, bool Stridable> typename R>
void boundedNoneRanges() {

  testWriteRange("..", R<int, false>(RangeInit::noValue, RangeInit::noValue),
                 ".. = all integers, increasing");

  testWriteRange(".. by -1",
                 by(R<int, false>(RangeInit::noValue, RangeInit::noValue), -1),
                 ".. by -1 = all integers, decreasing");

  testWriteRange("(..) + 3",
                 BoundedRange<int>(RangeInit::noValue, RangeInit::noValue) + 3,
                 "(..) + 3 = all integers, increasing");
  testWriteRange("3 + (..)",
                 3 + BoundedRange<int>(RangeInit::noValue, RangeInit::noValue),
                 "3 + (..) = all integers, increasing");
  testWriteRange("(..) - 3",
                 BoundedRange<int>(RangeInit::noValue, RangeInit::noValue) - 3,
                 "(..) - 3 = all integers, increasing");
}

int main() {

  boundedRanges();

  boundedLowRanges<BoundedRange>();
  boundedLowRanges<BoundedLowRange>();

  boundedHighRanges<BoundedRange>();
  boundedHighRanges<BoundedHighRange>();

  boundedNoneRanges<BoundedRange>();
  boundedNoneRanges<BoundedLowRange>();
  boundedNoneRanges<BoundedHighRange>();
  boundedNoneRanges<BoundedNoneRange>();

  return hpx::util::report_errors();
}
