//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/thread.hpp>

#include <atomic>
#include <mutex>
#include <set>
#include <string>

std::atomic<int> called(0);

template <typename T> void testCoforallLoopHomogenousTuple(T val) {

  {
    chplx::Tuple<> t;

    called = 0;
    chplx::coforallLoop(t, [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), t.size());
  }

  {
    chplx::Tuple<T> t{val};

    called = 0;
    chplx::coforallLoop(t, [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), t.size());
  }

  {
    chplx::Tuple<T, T> t{val, val};

    called = 0;
    chplx::coforallLoop(t, [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), t.size());
  }

  {
    chplx::Tuple<T, T, T, T, T, T, T, T, T, T> t{val, val, val, val, val,
                                                 val, val, val, val, val};

    called = 0;
    chplx::coforallLoop(t, [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), t.size());
  }
}

template <std::size_t N, typename... Ts, std::size_t... Is>
void testValue(std::set<std::variant<Ts...>> const &values,
               chplx::Tuple<Ts...> const &t) {

  HPX_TEST(values.contains(std::variant<Ts...>(std::get<N>(t))));
}

template <typename... Ts, std::size_t... Is>
int testValues(std::set<std::variant<Ts...>> const &values,
               chplx::Tuple<Ts...> const &t, std::index_sequence<Is...>) {

  int count = 0;
  ((testValue<Is>(values, t), ++count), ...);
  return count;
}

template <typename... Ts> void testCoforallLoopTuple(Ts... ts) {

  chplx::Tuple<Ts...> t(ts...);

  int called = 0;

  std::set<std::variant<Ts...>> values;
  hpx::mutex mtx;

  called = 0;
  chplx::coforallLoop(t, [&](auto value) {
    std::lock_guard l(mtx);
    ++called;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  HPX_TEST_EQ(testValues(values, t, std::make_index_sequence<sizeof...(Ts)>()),
              t.size());
  HPX_TEST_EQ(called, t.size());
}

int main() {

  testCoforallLoopHomogenousTuple(42);
  testCoforallLoopHomogenousTuple(42.0);
  testCoforallLoopHomogenousTuple(std::string("42"));

  testCoforallLoopTuple(42);
  testCoforallLoopTuple(42, 43L);
  testCoforallLoopTuple(42, 43L, std::string("42"));

  return hpx::util::report_errors();
}
