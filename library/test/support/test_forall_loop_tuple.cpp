//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

#include <atomic>
#include <string>
#include <tuple>

std::atomic<int> called(0);

template <typename T> void test_forall_loop_tuple(T val) {

  {
    std::tuple<> t;

    called = 0;
    chplx::for_loop(t, [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), std::tuple_size_v<decltype(t)>);
  }

  {
    std::tuple<T> t{val};

    called = 0;
    chplx::forall_loop(t, [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), std::tuple_size_v<decltype(t)>);
  }

  {
    std::tuple<T, T> t{val, val};

    called = 0;
    chplx::forall_loop(t, [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), std::tuple_size_v<decltype(t)>);
  }

  {
    std::tuple<T, T, T, T, T, T, T, T, T, T> t{val, val, val, val, val,
                                               val, val, val, val, val};

    called = 0;
    chplx::forall_loop(t, [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), std::tuple_size_v<decltype(t)>);
  }
}

int main() {

  test_forall_loop_tuple<int>(42);
  test_forall_loop_tuple<double>(42.0);
  test_forall_loop_tuple<std::string>("42.0");

  return hpx::util::report_errors();
}
