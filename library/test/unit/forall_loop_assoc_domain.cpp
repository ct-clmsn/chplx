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

bool chplx_fork_join_executor = true;
hpx::execution::experimental::fork_join_executor *exec = nullptr;

template <typename T>
void testForallLoopDomain(chplx::AssocDomain<T> const &d) {

  std::set<typename chplx::AssocDomain<T>::idxType> values;
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

template <typename T> void testForallLoopDomain(std::vector<T> const &values) {

  chplx::AssocDomain<T> d;
  for (auto const &elem : values) {
    d.add(elem);
  }
  testForallLoopDomain(d);
}

int main() {

  chplx_fork_join_executor = true;
  exec = new hpx::execution::experimental::fork_join_executor();

  {
    std::vector<int> const v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    testForallLoopDomain(v);
  }

  {
    std::vector<std::string> const v = {"0", "1", "2", "3", "4",
                                        "5", "6", "7", "8", "9"};
    testForallLoopDomain(v);
  }

  delete exec;

  chplx_fork_join_executor = false;

  {
    std::vector<int> const v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    testForallLoopDomain(v);
  }

  {
    std::vector<std::string> const v = {"0", "1", "2", "3", "4",
                                        "5", "6", "7", "8", "9"};
    testForallLoopDomain(v);
  }


  return hpx::util::report_errors();
}
