//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

#include <atomic>
#include <cstddef>
#include <string>
#include <tuple>

template <typename Tuple, typename T>
void testForLoopHomogenousTuple(Tuple t, T val) {

  std::atomic<int> called = 0;

  {
    chplx::forLoop(const_cast<Tuple const &>(t), [&](auto value) {
      ++called;
      HPX_TEST_EQ(value, val);
    });

    HPX_TEST_EQ(called.load(), t.size());
  }

  // modify tuple elements
  {
    called = 0;

    chplx::forLoop(t, [&]<typename Elem>(Elem &value) {
      ++called;
      HPX_TEST_EQ(value, val);

      value = Elem();
      HPX_TEST_EQ(value, Elem());
    });

    HPX_TEST_EQ(called.load(), t.size());
  }
}

template <typename T> void testForLoopHomogenousTuples(T val) {

  testForLoopHomogenousTuple(chplx::Tuple<>(), val);
  testForLoopHomogenousTuple(chplx::Tuple<T>(val), val);
  testForLoopHomogenousTuple(chplx::Tuple<T, T>(val, val), val);
  testForLoopHomogenousTuple(
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

template <typename... Ts> void testForLoopTuple(Ts... ts) {

  chplx::Tuple<Ts...> t(ts...);

  {
    int called = 0;

    std::set<std::variant<Ts...>> values;
    hpx::mutex mtx;

    called = 0;
    chplx::coforall(const_cast<chplx::Tuple<Ts...> const &>(t),
                    [&](auto value) {
                      std::lock_guard l(mtx);
                      ++called;
                      auto p = values.insert(value);
                      HPX_TEST(p.second);
                    });

    HPX_TEST_EQ(
        testValues(values, t, std::make_index_sequence<sizeof...(Ts)>()),
        t.size());
    HPX_TEST_EQ(called, t.size());
  }

  // modify tuple elements
  {
    int called = 0;

    std::set<std::variant<Ts...>> values;
    hpx::mutex mtx;

    called = 0;
    chplx::coforall(t, [&]<typename Elem>(Elem &value) {
      std::lock_guard l(mtx);
      ++called;
      auto p = values.insert(value);
      HPX_TEST(p.second);

      value = Elem();
      HPX_TEST_EQ(value, Elem());
    });

    chplx::Tuple<Ts...> expected(ts...);
    HPX_TEST_EQ(
        testValues(values, expected, std::make_index_sequence<sizeof...(Ts)>()),
        t.size());
    HPX_TEST_EQ(called, expected.size());
  }
}

int main() {

  testForLoopHomogenousTuples<int>(42);
  testForLoopHomogenousTuples<double>(42.0);
  testForLoopHomogenousTuples<std::string>("42.0");

  testForLoopTuple(42);
  testForLoopTuple(42, 43L);
  testForLoopTuple(42, 43L, std::string("42"));

  return hpx::util::report_errors();
}
