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
#include <cstddef>
#include <mutex>
#include <set>
#include <string>

//-----------------------------------------------------------------------------
// helper for testing below
namespace chplx {

template <typename T> bool operator==(Sync<T> const &lhs, Sync<T> const &rhs) {
  return lhs.readFF() == rhs.readFF();
}

template <typename T>
std::ostream &operator<<(std::ostream &os, Sync<T> const &s) {
  return os << s.readFF();
}

template <typename T>
bool operator==(Single<T> const &lhs, Single<T> const &rhs) {
  return lhs.readFF() == rhs.readFF();
}

template <typename T>
std::ostream &operator<<(std::ostream &os, Single<T> const &s) {
  return os << s.readFF();
}

} // namespace chplx

//-----------------------------------------------------------------------------
template <typename Tuple, typename T, typename T1>
void testCoforallLoopHomogenousTuple(Tuple t, T value, T1 &pass) {

  std::atomic<int> called = 0;

  {
    chplx::coforall(const_cast<Tuple const &>(t), [&](auto elem) {
      ++called;
      HPX_TEST_EQ(elem, value);
    });

    HPX_TEST_EQ(called.load(), t.size());
  }

  {
    called = 0;
    chplx::coforall(
        t,
        [&](auto elem, auto &&val) {
          ++called;
          HPX_TEST_EQ(elem, value);
          HPX_TEST_EQ(pass, val);
        },
        pass);

    HPX_TEST_EQ(called.load(), t.size());
  }

  // modify tuple elements
  {
    called = 0;
    chplx::coforall(
        t,
        [&]<typename Elem>(Elem &elem, auto &&val) {
          ++called;
          HPX_TEST_EQ(elem, value);
          HPX_TEST_EQ(pass, val);

          elem = Elem();
          HPX_TEST_EQ(elem, Elem());
        },
        pass);

    HPX_TEST_EQ(called.load(), t.size());
  }
}

template <typename T, typename T1>
void testCoforallLoopHomogenousTuples(T val, T1 &&pass) {

  testCoforallLoopHomogenousTuple(chplx::Tuple<>(), val, pass);
  testCoforallLoopHomogenousTuple(chplx::Tuple<T>(val), val, pass);
  testCoforallLoopHomogenousTuple(chplx::Tuple<T, T>(val, val), val, pass);
  testCoforallLoopHomogenousTuple(
      chplx::Tuple<T, T, T, T, T, T, T, T, T, T>(val, val, val, val, val, val,
                                                 val, val, val, val),
      val, pass);
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

template <typename T, typename... Ts>
void testCoforallLoopTuple(T &&value, Ts... ts) {

  chplx::Tuple<Ts...> t(ts...);

  {
    int called = 0;

    std::set<std::variant<Ts...>> values;
    hpx::mutex mtx;

    called = 0;
    chplx::coforall(const_cast<chplx::Tuple<Ts...> const &>(t), [&](auto val) {
      std::lock_guard l(mtx);
      ++called;
      auto p = values.insert(val);
      HPX_TEST(p.second);
    });

    HPX_TEST_EQ(
        testValues(values, t, std::make_index_sequence<sizeof...(Ts)>()),
        t.size());
    HPX_TEST_EQ(called, t.size());
  }

  {
    int called = 0;

    std::set<std::variant<Ts...>> values;
    hpx::mutex mtx;

    called = 0;
    chplx::coforall(
        t,
        [&](auto elem, auto &&val) {
          HPX_TEST_EQ(value, val);
          std::lock_guard l(mtx);
          ++called;
          auto p = values.insert(elem);
          HPX_TEST(p.second);
        },
        value);

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
    chplx::coforall(t, [&]<typename Elem>(Elem &val) {
      std::lock_guard l(mtx);
      ++called;
      auto p = values.insert(val);
      HPX_TEST(p.second);

      val = Elem();
      HPX_TEST_EQ(val, Elem());
    });

    chplx::Tuple<Ts...> expected(ts...);
    HPX_TEST_EQ(
        testValues(values, expected, std::make_index_sequence<sizeof...(Ts)>()),
        t.size());
    HPX_TEST_EQ(called, expected.size());
  }
}

int main() {

  {
    testCoforallLoopHomogenousTuples(42, 42);
    testCoforallLoopHomogenousTuples(42.0, 42.0);
    testCoforallLoopHomogenousTuples(std::string("42"), std::string("42"));

    chplx::Sync<int> sy(42);
    testCoforallLoopHomogenousTuples(42, sy);
    testCoforallLoopHomogenousTuples(42.0, sy);
    testCoforallLoopHomogenousTuples(std::string("42"), sy);
  }

  {
    testCoforallLoopTuple(10, 42);
    testCoforallLoopTuple(10.0, 42, 43L);
    testCoforallLoopTuple(std::string("10"), 42, 43L, std::string("42"));

    chplx::Sync<int> sy(42);
    testCoforallLoopTuple(sy, 42);
    testCoforallLoopTuple(sy, 42, 43L);
    testCoforallLoopTuple(sy, 42, 43L, std::string("42"));
  }

  return hpx::util::report_errors();
}
