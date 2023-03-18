//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/synchronization.hpp>
#include <hpx/modules/testing.hpp>

#include <atomic>

void testNullary() {

  std::atomic<int> count(0);

  hpx::latch l(5);

  auto f = [&]() {
    ++count;
    l.count_down(1);
  };

  chplx::begin(f);
  chplx::begin(f);
  chplx::begin(f);
  chplx::begin(f);

  l.arrive_and_wait();

  HPX_TEST_EQ(count.load(), 4);
}

void testOneArg() {

  std::atomic<int> count(0);

  hpx::latch l(5);

  auto f = [&](int arg) {
    count += arg;
    l.count_down(1);
  };

  chplx::begin(f, 1);
  chplx::begin(f, 2);
  chplx::begin(f, 3);
  chplx::begin(f, 4);

  l.arrive_and_wait();

  HPX_TEST_EQ(count.load(), 10);
}

void testSyncVariable() {

  std::atomic<int> count(0);

  chplx::sync<int> s(42);

  hpx::latch l(5);

  auto f = [&](chplx::sync<int>& arg) {
    ++count;
    HPX_TEST_EQ(arg.readFF(), 42);
    l.count_down(1);
  };

  chplx::begin(f, s);
  chplx::begin(f, s);
  chplx::begin(f, s);
  chplx::begin(f, s);

  l.arrive_and_wait();

  HPX_TEST_EQ(count.load(), 4);
}

void testSingleVariable() {

  std::atomic<int> count(0);

  chplx::single<int> s(42);

  hpx::latch l(5);

  auto f = [&](chplx::single<int>& arg) {
    ++count;
    HPX_TEST_EQ(arg.readFF(), 42);
    l.count_down(1);
  };

  chplx::begin(f, s);
  chplx::begin(f, s);
  chplx::begin(f, s);
  chplx::begin(f, s);

  l.arrive_and_wait();

  HPX_TEST_EQ(count.load(), 4);
}

int main() {

  testNullary();
  testOneArg();
  testSyncVariable();
  testSingleVariable();

  return hpx::util::report_errors();
}
