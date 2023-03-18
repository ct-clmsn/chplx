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

template <typename Tuple, typename T>
void testCoforallLoopHomogenousTuple(Tuple t, T value) {

  called = 0;
  chplx::coforall(
      t,
      [&](auto elem, T val) {
        ++called;
        HPX_TEST_EQ(elem, val);
      },
      value);

  HPX_TEST_EQ(called.load(), t.size());
}

template <typename T> void testCoforallLoopHomogenousTuples(T val) {

  testCoforallLoopHomogenousTuple(chplx::Tuple<>(), val);
  testCoforallLoopHomogenousTuple(chplx::Tuple<T>(val), val);
  testCoforallLoopHomogenousTuple(chplx::Tuple<T, T>(val, val), val);
  testCoforallLoopHomogenousTuple(
      chplx::Tuple<T, T, T, T, T, T, T, T, T, T>(val, val, val, val, val, val,
                                                 val, val, val, val),
      val);
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
  chplx::coforall(t, [&](auto value) {
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

  testCoforallLoopHomogenousTuples(42);
  testCoforallLoopHomogenousTuples(42.0);
  testCoforallLoopHomogenousTuples(std::string("42"));

  testCoforallLoopTuple(42);
  testCoforallLoopTuple(42, 43L);
  testCoforallLoopTuple(42, 43L, std::string("42"));

  return hpx::util::report_errors();
}
