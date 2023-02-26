//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

#include <vector>

void test_1d() {
  std::vector<chplx::detail::Position<int, 1>> p = {{0}, {1}, {2},
                                                    {3}, {4}, {5}};

  std::size_t i = 0;
  for (auto pos : chplx::generateIndices(6)) {
    HPX_TEST_EQ(pos[0], p[i][0]);
    ++i;
  }
}

void test_2d() {
  std::vector<chplx::detail::Position<int, 2>> p = {{0, 0}, {1, 0}, {0, 1},
                                                    {1, 1}, {0, 2}, {1, 2}};

  std::size_t i = 0;
  for (auto pos : chplx::generateIndices(2, 3)) {
    HPX_TEST_EQ(pos[0], p[i][0]);
    HPX_TEST_EQ(pos[1], p[i][1]);
    ++i;
  }
}

int main() {

  test_1d();
  test_2d();

  return hpx::util::report_errors();
}
