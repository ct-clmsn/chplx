//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/thread.hpp>

#include <cstddef>
#include <mutex>
#include <set>

template <typename T, typename Domain>
void testCoforallLoopArray(chplx::Array<T, Domain> d) {

  std::set<T> values;
  hpx::mutex mtx;

  std::size_t count = 0;

  chplx::coforall(
      d,
      [&](auto &value, int fortytwo) {
        HPX_TEST_EQ(fortytwo, 42);
        std::lock_guard l(mtx);
        ++count;
        auto p = values.insert(value);
        HPX_TEST(p.second);
      },
      42);

  HPX_TEST_EQ(count, static_cast<std::size_t>(d.size()));
  count = 0;

  for (auto val : d.these()) {
    ++count;
    HPX_TEST(values.contains(val));
  }

  HPX_TEST_EQ(count, values.size());
}

namespace detail {

//-----------------------------------------------------------------------------
template <typename T, typename R> auto initArray1D(R &&r) {

  auto array = chplx::Array(chplx::Domain(r), T());
  T value = T();
  chplx::forLoop(array, [&](auto &v) { v = ++value; });
  return array;
}

template <typename T, typename... Rs, std::size_t... Is>
void testCoforallLoopArrays1D(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Is...> s) {

  (testCoforallLoopArray(initArray1D<T>(std::get<Is>(r))), ...);
}

//-----------------------------------------------------------------------------
template <typename T, typename R1, typename R2>
auto initArray2D(R1 &&r1, R2 &&r2) {

  auto array = chplx::Array(chplx::Domain(r1, r2), T());
  T value = T();
  chplx::forLoop(array, [&](auto &v) { v = ++value; });
  return array;
}

template <typename T, std::size_t N, typename... Rs, std::size_t... Js>
void testCoforallLoopArrays2D(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Js...>) {

  (testCoforallLoopArray(initArray2D<T>(std::get<Js>(r), std::get<N>(r))), ...);
}

template <typename T, typename... Rs, std::size_t... Is>
void testCoforallLoopArrays2D(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Is...> s) {

  (testCoforallLoopArrays2D<T, Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <typename T, typename R1, typename R2, typename R3>
auto initArray3D(R1 &&r1, R2 &&r2, R3 &&r3) {

  auto array = chplx::Array(chplx::Domain(r1, r2, r3), T());
  T value = T();
  chplx::forLoop(array, [&](auto &v) { v = ++value; });
  return array;
}

template <typename T, std::size_t N, std::size_t M, typename... Rs,
          std::size_t... Js>
void testCoforallLoopArrays3D_2(chplx::Tuple<Rs...> const &r,
                                std::index_sequence<Js...>) {

  (testCoforallLoopArray(
       initArray3D<T>(std::get<Js>(r), std::get<N>(r), std::get<M>(r))),
   ...);
}

template <typename T, std::size_t N, typename... Rs, std::size_t... Is>
void testCoforallLoopArrays3D_1(chplx::Tuple<Rs...> const &r,
                                std::index_sequence<Is...> s) {

  (testCoforallLoopArrays3D_2<T, N, Is>(r, s), ...);
}

template <typename T, typename... Rs, std::size_t... Is>
void testCoforallLoopArrays3D(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Is...> s) {

  (testCoforallLoopArrays3D_1<T, Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <typename T, typename R1, typename R2, typename R3, typename R4>
auto initArray4D(R1 &&r1, R2 &&r2, R3 &&r3, R4 &&r4) {

  auto array = chplx::Array(chplx::Domain(r1, r2, r3, r4), T());
  T value = T();
  chplx::forLoop(array, [&](auto &v) { v = ++value; });
  return array;
}

template <typename T, std::size_t N, std::size_t M, std::size_t O,
          typename... Rs, std::size_t... Js>
void testCoforallLoopArrays4D_3(chplx::Tuple<Rs...> const &r,
                                std::index_sequence<Js...>) {

  (testCoforallLoopArray(initArray4D<T>(std::get<Js>(r), std::get<N>(r),
                                        std::get<M>(r), std::get<O>(r))),
   ...);
}

template <typename T, std::size_t N, std::size_t M, typename... Rs,
          std::size_t... Is>
void testCoforallLoopArrays4D_2(chplx::Tuple<Rs...> const &r,
                                std::index_sequence<Is...> s) {

  (testCoforallLoopArrays4D_3<T, N, M, Is>(r, s), ...);
}

template <typename T, std::size_t N, typename... Rs, std::size_t... Is>
void testCoforallLoopArrays4D_1(chplx::Tuple<Rs...> const &r,
                                std::index_sequence<Is...> s) {

  (testCoforallLoopArrays4D_2<T, N, Is>(r, s), ...);
}

template <typename T, typename... Rs, std::size_t... Is>
void testCoforallLoopArrays4D(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Is...> s) {

  (testCoforallLoopArrays4D_1<T, Is>(r, s), ...);
}

} // namespace detail

template <typename T, typename... Rs>
void testCoforallLoopArrays1D(chplx::Tuple<Rs...> r) {

  detail::testCoforallLoopArrays1D<T>(
      r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename T, typename... Rs>
void testCoforallLoopArrays2D(chplx::Tuple<Rs...> r) {

  detail::testCoforallLoopArrays2D<T>(
      r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename T, typename... Rs>
void testCoforallLoopArrays3D(chplx::Tuple<Rs...> r) {

  detail::testCoforallLoopArrays3D<T>(
      r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename T, typename... Rs>
void testCoforallLoopArrays4D(chplx::Tuple<Rs...> r) {

  detail::testCoforallLoopArrays4D<T>(
      r, std::make_index_sequence<sizeof...(Rs)>());
}

int main() {

  {
    auto constexpr r1 = chplx::Range(0, 10);
    auto constexpr r2 = chplx::BoundedRange<int, true>(0, 10, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(0, 10, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(0, 10, -2);

    testCoforallLoopArrays1D<double>(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopArrays2D<double>(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopArrays3D<double>(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopArrays4D<double>(chplx::Tuple(r1, r2, r3, r4));
  }

  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::BoundedRange<int, true>(1, 9, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(1, 9, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(1, 9, -2);

    testCoforallLoopArrays1D<int>(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopArrays2D<int>(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopArrays3D<int>(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopArrays4D<int>(chplx::Tuple(r1, r2, r3, r4));
  }

  return hpx::util::report_errors();
}
