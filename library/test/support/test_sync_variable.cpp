//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

// var sy$: sync int=1;  // state = full, value = 1
// var done$: sync bool;
// writeln("Launching new task");
// begin {
//   var sy = sy$.readFE(); // This statement will block until sy$ is full
//   writeln("New task unblocked, sy=", sy);
//   done$.writeEF(true);
// }

void testSyncVariable() {

  chplx::sync<int> sync_sy;
  HPX_TEST(!sync_sy.isFull());
  HPX_TEST_EQ(sync_sy.readXX(), int());

  chplx::sync<bool> sync_done;
  HPX_TEST(!sync_done.isFull());
  HPX_TEST_EQ(sync_sy.readXX(), bool());

  chplx::begin([&]() {
    auto sy = sync_sy.readFE();
    HPX_TEST(!sync_sy.isFull());
    HPX_TEST_EQ(sy, 1);

    HPX_TEST(!sync_done.readXX());
    sync_done.writeEF(true);
    HPX_TEST(sync_done.isFull());
  });

  sync_sy.writeEF(1);
  HPX_TEST(sync_sy.isFull());
  HPX_TEST_EQ(sync_sy.readXX(), 1);

  sync_done.readFE();
  HPX_TEST(!sync_done.isFull());
}

int main() {

  testSyncVariable();

  return hpx::util::report_errors();
}
