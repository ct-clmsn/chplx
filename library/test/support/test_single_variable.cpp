//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

// Example: simple split-phase barrier for tasks
//
// var count$: sync int = n;  // counter which also serves as a lock
// var release$: single bool; // barrier release
//
// coforall t in 1..n {
//   var myc = count$.readFE(); // read the count, grab the lock (state = empty)
//   if myc!=1 {                // still waiting for others
//     write(".");
//     count$.writeEF(myc-1); // update the count, release the lock (state =
//     full)
//                              // we could do some work while waiting
//     release$.readFF();       // wait for everyone
//   } else {                   // last one here
//     release$.writeEF(true);  // release everyone first (state = full)
//     writeln("done");
//   }
// }
//
// sy$.reset();

constexpr int n = 7;

void testSingleVariable() {

  chplx::sync<int> count(n);
  HPX_TEST(count.isFull());
  HPX_TEST_EQ(count.readXX(), n);

  chplx::single<bool> release;
  HPX_TEST(!release.isFull());

  chplx::coforall(chplx::Range(1, n), [&count, &release](int) {
    int myc = count.readFE();
    if (myc != 1) {
      count.writeEF(myc - 1);
      (void)release.readFF();
    } else {
      release.writeEF(true);
    }
  });

  HPX_TEST_EQ(count.readXX(), 1);
}

int main() {

  testSingleVariable();

  return hpx::util::report_errors();
}
