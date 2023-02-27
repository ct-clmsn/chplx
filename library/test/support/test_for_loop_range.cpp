//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

template <typename T, chplx::BoundedRangeType BoundedType, bool Stridable>
void test_for_loop_range(chplx::Range<T, BoundedType, Stridable> r) {

  int called = 0;

  T val = r.first();
  T stride = r.stride();

  chplx::for_loop(r, [&](auto value) {
    ++called;
    HPX_TEST_EQ(value, val);
    val += stride;
  });

  HPX_TEST_EQ(called, r.size());
}

int main() {

  test_for_loop_range(chplx::Range(0, 10));
  test_for_loop_range(chplx::BoundedRange<int, true>(0, 10, 2));
  test_for_loop_range(chplx::Range(0, 10, chplx::BoundsCategoryType::Open));
  test_for_loop_range(chplx::BoundedRange<int, true>(
      0, 10, 2, chplx::BoundsCategoryType::Open));

  test_for_loop_range(chplx::Range(1, 0));

  test_for_loop_range(by(chplx::BoundedRange<int, true>(1, 10), -1));
  test_for_loop_range(by(chplx::BoundedRange<int, true>(1, 10), -2));

  return hpx::util::report_errors();
}
