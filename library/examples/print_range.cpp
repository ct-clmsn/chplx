//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

using namespace chplx;

template <typename Range>
void testWriteRange(char const *expr, Range const &r) {

  write(expr, " = ");
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
    for (auto i : iterate(r)) {
      if (!first)
        write(", ");
      write(i);
      first = false;
    }

  } else if (r.hasFirst()) {
    // The range is not fully bounded, but its sequence has a starting point
    // - print the first three indices.  Note that in this and the next
    // case the sequence can be either increasing or decreasing.
    for (auto i : iterate(count(r, 3)))
      write(i, ", ");
    write("...");

  } else if (r.hasLast()) {
    // The range is not fully bounded, but its sequence has an ending point.
    // Print the last three indices.
    write("...");
    for (auto i : iterate(count(r, -3)))
      write(", ", i);

  } else if (r.stride() == 1 || r.stride() == -1) {
    // If we are here, the range is fully unbounded.
    write("all integers, ", r.stride() > 0 ? "increasing" : "decreasing");

  } else {
    // We got a more complex range, do not elaborate.
  }
  writeln();
}

void boundedRanges() {

  writeln("Closed interval ranges:");

  testWriteRange("1..10",
                 BoundedRange<int>(1, 10)); // = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
  testWriteRange("-3..3", BoundedRange<int>(-3, 3)); // = -3, -2, -1, 0, 1, 2, 3
  testWriteRange("1..10 # 3", count(BoundedRange<int>(1, 10), 3)); // = 1, 2, 3
  testWriteRange("1..10 # -3",
                 count(BoundedRange<int>(1, 10), -3)); // = 8, 9, 10
  testWriteRange("1..10 by 2",
                 by(BoundedRange<int>(1, 10), 2)); // = 1, 3, 5, 7, 9
  writeln();

  constexpr auto open = BoundsCategoryType::Open;

  writeln("Open interval ranges:");
  testWriteRange("1..<10",
                 BoundedRange<int>(1, 10, open)); // = 1, 2, 3, 4, 5, 6, 7, 8, 9
  testWriteRange("-3..<3",
                 BoundedRange<int>(-3, 3, open)); // = -3, -2, -1, 0, 1, 2
  testWriteRange("1..<10 # 3",
                 count(BoundedRange<int>(1, 10, open), 3)); // = 1, 2, 3
  testWriteRange("1..<10 # -3",
                 count(BoundedRange<int>(1, 10, open), -3)); // = 7, 8, 9
  testWriteRange("1..<10 by 2",
                 by(BoundedRange<int>(1, 10, open), 2)); // = 1, 3, 5, 7, 9
  writeln();

  writeln("Empty ranges:");
  testWriteRange("1..0", BoundedRange<int>(1, 0));   // =
  testWriteRange("0..-1", BoundedRange<int>(0, -1)); // =
  testWriteRange("10..1", BoundedRange<int>(10, 1)); // =
  writeln();

  writeln("Decreasing range");
  testWriteRange("1..10 by -1", by(BoundedRange<int>(1, 10),
                                   -1)); // = 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
  testWriteRange("1..10 by -2",
                 by(BoundedRange<int>(1, 10), -2)); // = 10, 8, 6, 4, 2
  writeln();
}

void unboundedRanges() {

  writeln("Unbounded ranges");
  testWriteRange("1..", BoundedRange<int>(1));     // = 1, 2, 3, ...
  testWriteRange("-3..", BoundedLowRange<int>(1)); // = -3, -2, -1, ...

  testWriteRange("..5", BoundedRange<int>(RangeInit::noValue,
                                          5)); // = ..., 3, 4, 5
  testWriteRange("..", BoundedRange<int>(RangeInit::noValue,
                                         RangeInit::noValue)); // = ...
  testWriteRange("..", BoundedRange<int>());                   // = ...
  writeln();
}

int main() {

  boundedRanges();
  unboundedRanges();

  return 0;
}
