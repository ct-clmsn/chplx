//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

#include <atomic>

std::atomic<int> global(0);

// send value back to original locality
void sendBack(chplx::locale loc, int val) {

  chplx::on(
      loc,
      [](int value) {
        HPX_TEST_EQ(value, global.load());
        global.store(0);
      },
      val);
}

int main() {

  HPX_TEST_EQ(chplx::numLocales, hpx::get_num_localities(hpx::launch::sync));

  std::size_t i = 0;
  for (auto loc : chplx::Locales.these()) {
    HPX_TEST_EQ(chplx::Locales(i).id(), loc.id());
    ++i;
  }

  auto const here = chplx::Locales(0);
  HPX_TEST_EQ(here.id(), hpx::get_locality_id());
  HPX_TEST_EQ(here.hostname(), hpx::get_locality_name());
  HPX_TEST_EQ(here.maxTaskPar(), hpx::get_num_worker_threads());

  HPX_TEST_EQ(here.numPUs(false, true), hpx::get_num_worker_threads());

  auto &topo = hpx::threads::create_topology();
  HPX_TEST_EQ(here.numPUs(true, false), topo.get_number_of_pus());
  HPX_TEST_EQ(here.numPUs(false, false), topo.get_number_of_cores());

  HPX_TEST(chplx::here == chplx::Locales(0));

  for (auto loc : chplx::Locales.these()) {
    // store a unique value here
    global.store(42);

    // tell other locality to send back a message that resets the global
    chplx::on(
        loc, [](chplx::locale here, int var) { sendBack(here, var); },
        chplx::here, global.load());

    // should have been reset by the sendBack above
    HPX_TEST_EQ(global.load(), 0);
  }

  return hpx::util::report_errors();
}
