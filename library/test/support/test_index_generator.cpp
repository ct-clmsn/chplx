//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

#include <iostream>
#include <vector>

void test_1d() {
  std::vector<int> p = {0, 1, 2, 3, 4, 5, 6};

  std::size_t i = 0;
  for (auto pos :
       chplx::iterate(chplx::Domain(chplx::BoundedRange<int>(0, 6)))) {
    HPX_TEST_EQ(pos, p[i]);
    ++i;
  }
  HPX_TEST_EQ(i, p.size());
}

void test_2d() {
  std::vector<chplx::Tuple<int, int>> p = {{0, 0}, {0, 1}, {0, 2}, {0, 3},
                                           {1, 0}, {1, 1}, {1, 2}, {1, 3},
                                           {2, 0}, {2, 1}, {2, 2}, {2, 3}};

  std::size_t i = 0;
  for (auto pos : chplx::iterate(chplx::Domain(
           chplx::BoundedRange<int>(0, 2), chplx::BoundedRange<int>(0, 3)))) {
    HPX_TEST_EQ(std::get<0>(pos), std::get<0>(p[i]));
    HPX_TEST_EQ(std::get<1>(pos), std::get<1>(p[i]));
    ++i;
  }
  HPX_TEST_EQ(i, p.size());
}

void test_3d() {
  std::vector<chplx::Tuple<int, int, int>> p = {
      {0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 0, 3}, {0, 1, 0}, {0, 1, 1},
      {0, 1, 2}, {0, 1, 3}, {0, 2, 0}, {0, 2, 1}, {0, 2, 2}, {0, 2, 3},
      {1, 0, 0}, {1, 0, 1}, {1, 0, 2}, {1, 0, 3}, {1, 1, 0}, {1, 1, 1},
      {1, 1, 2}, {1, 1, 3}, {1, 2, 0}, {1, 2, 1}, {1, 2, 2}, {1, 2, 3}};

  std::size_t i = 0;
  for (auto pos : chplx::iterate(chplx::Domain(
           chplx::BoundedRange<int>(0, 1), chplx::BoundedRange<int>(0, 2),
           chplx::BoundedRange<int>(0, 3)))) {
    HPX_TEST_EQ(std::get<0>(pos), std::get<0>(p[i]));
    HPX_TEST_EQ(std::get<1>(pos), std::get<1>(p[i]));
    HPX_TEST_EQ(std::get<2>(pos), std::get<2>(p[i]));
    ++i;
  }
  HPX_TEST_EQ(i, p.size());
}

int main() {

//  test_1d();
//  test_2d();
  test_3d();

  return hpx::util::report_errors();
}
