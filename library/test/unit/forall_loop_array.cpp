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
void testForallLoopArray(chplx::Array<T, Domain> d) {

  std::set<T> values;
  hpx::mutex mtx;

  std::size_t count = 0;

  chplx::forall(d, [&](auto value) {
    std::lock_guard l(mtx);
    ++count;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  HPX_TEST_EQ(count, static_cast<std::size_t>(d.size()));
  count = 0;

  for (auto const &e : d.these()) {

    ++count;
    HPX_TEST(values.contains(e));
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
void testForallLoopArrays1D(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Is...> s) {

  (testForallLoopArray(initArray1D<T>(std::get<Is>(r))), ...);
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
void testForallLoopArrays2D(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Js...>) {

  (testForallLoopArray(initArray2D<T>(std::get<Js>(r), std::get<N>(r))), ...);
}

template <typename T, typename... Rs, std::size_t... Is>
void testForallLoopArrays2D(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Is...> s) {

  (testForallLoopArrays2D<T, Is>(r, s), ...);
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
void testForallLoopArrays3D_2(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Js...>) {

  (testForallLoopArray(
       initArray3D<T>(std::get<Js>(r), std::get<N>(r), std::get<M>(r))),
   ...);
}

template <typename T, std::size_t N, typename... Rs, std::size_t... Is>
void testForallLoopArrays3D_1(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Is...> s) {

  (testForallLoopArrays3D_2<T, N, Is>(r, s), ...);
}

template <typename T, typename... Rs, std::size_t... Is>
void testForallLoopArrays3D(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Is...> s) {

  (testForallLoopArrays3D_1<T, Is>(r, s), ...);
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
void testForallLoopArrays4D_3(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Js...>) {

  (testForallLoopArray(initArray4D<T>(std::get<Js>(r), std::get<N>(r),
                                      std::get<M>(r), std::get<O>(r))),
   ...);
}

template <typename T, std::size_t N, std::size_t M, typename... Rs,
          std::size_t... Is>
void testForallLoopArrays4D_2(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Is...> s) {

  (testForallLoopArrays4D_3<T, N, M, Is>(r, s), ...);
}

template <typename T, std::size_t N, typename... Rs, std::size_t... Is>
void testForallLoopArrays4D_1(chplx::Tuple<Rs...> const &r,
                              std::index_sequence<Is...> s) {

  (testForallLoopArrays4D_2<T, N, Is>(r, s), ...);
}

template <typename T, typename... Rs, std::size_t... Is>
void testForallLoopArrays4D(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Is...> s) {

  (testForallLoopArrays4D_1<T, Is>(r, s), ...);
}

} // namespace detail

template <typename T, typename... Rs>
void testForallLoopArrays1D(chplx::Tuple<Rs...> r) {

  detail::testForallLoopArrays1D<T>(r,
                                    std::make_index_sequence<sizeof...(Rs)>());
}

template <typename T, typename... Rs>
void testForallLoopArrays2D(chplx::Tuple<Rs...> r) {

  detail::testForallLoopArrays2D<T>(r,
                                    std::make_index_sequence<sizeof...(Rs)>());
}

template <typename T, typename... Rs>
void testForallLoopArrays3D(chplx::Tuple<Rs...> r) {

  detail::testForallLoopArrays3D<T>(r,
                                    std::make_index_sequence<sizeof...(Rs)>());
}

template <typename T, typename... Rs>
void testForallLoopArrays4D(chplx::Tuple<Rs...> r) {

  detail::testForallLoopArrays4D<T>(r,
                                    std::make_index_sequence<sizeof...(Rs)>());
}

int main() {

  {
    auto constexpr r1 = chplx::Range(0, 10);
    auto constexpr r2 = chplx::BoundedRange<int, true>(0, 10, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(0, 10, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(0, 10, -2);

    testForallLoopArrays1D<double>(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopArrays2D<double>(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopArrays3D<double>(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopArrays4D<double>(chplx::Tuple(r1, r2, r3, r4));
  }

  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::BoundedRange<int, true>(1, 9, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(1, 9, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(1, 9, -2);

    testForallLoopArrays1D<int>(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopArrays2D<int>(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopArrays3D<int>(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopArrays4D<int>(chplx::Tuple(r1, r2, r3, r4));
  }

  return hpx::util::report_errors();
}
