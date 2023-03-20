//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx/array.hpp>
#include <chplx/domain.hpp>
#include <chplx/locale.hpp>
#include <chplx/range.hpp>

#include <hpx/modules/actions.hpp>
#include <hpx/modules/naming_base.hpp>
#include <hpx/modules/runtime_distributed.hpp>
#include <hpx/modules/threadmanager.hpp>
#include <hpx/modules/topology.hpp>

#include <thread>
#include <vector>

namespace chplx::detail {

std::string get_locale_hostname() { return hpx::get_locality_name(); }

std::int64_t get_locale_max_task_par() { return hpx::get_num_worker_threads(); }

std::int64_t get_locale_numpus(bool logical, bool accessible) {

  // number of cores available to application on this locality
  if (accessible) {
    return hpx::get_num_worker_threads();
  }

  // number of cores/pus available on node this locality is running on
  auto const &topo = hpx::threads::create_topology();
  if (logical) {
    return topo.get_number_of_pus();
  }
  return topo.get_number_of_cores();
}

std::int64_t get_locale_running_tasks() {

  auto &tm = hpx::threads::get_thread_manager();
  return tm.get_thread_count() -
         static_cast<std::int64_t>(tm.get_background_thread_count() + 1);
}

} // namespace chplx::detail

HPX_PLAIN_ACTION(chplx::detail::get_locale_hostname,
                 get_locale_hostname_action);
HPX_PLAIN_ACTION(chplx::detail::get_locale_max_task_par,
                 get_locale_max_task_par_action);
HPX_PLAIN_ACTION(chplx::detail::get_locale_numpus, get_locale_numpus_action);
HPX_PLAIN_ACTION(chplx::detail::get_locale_running_tasks,
                 get_locale_running_tasks_action);

namespace chplx {

//-----------------------------------------------------------------------------
String locale::hostname() const {

  HPX_ASSERT_MSG(locality_id != hpx::naming::invalid_locality_id,
                 "this locale instance was not properly initialized");

  return hpx::async(::get_locale_hostname_action{},
                    hpx::naming::get_id_from_locality_id(locality_id))
      .get();
}

std::int64_t locale::maxTaskPar() const {

  HPX_ASSERT_MSG(locality_id != hpx::naming::invalid_locality_id,
                 "this locale instance was not properly initialized");

  return hpx::async(::get_locale_max_task_par_action{},
                    hpx::naming::get_id_from_locality_id(locality_id))
      .get();
}

std::int64_t locale::numPUs(bool logical, bool accessible) const {

  HPX_ASSERT_MSG(locality_id != hpx::naming::invalid_locality_id,
                 "this locale instance was not properly initialized");

  return hpx::async(::get_locale_numpus_action{},
                    hpx::naming::get_id_from_locality_id(locality_id), logical,
                    accessible)
      .get();
}

std::int64_t locale::runningTasks() const {

  HPX_ASSERT_MSG(locality_id != hpx::naming::invalid_locality_id,
                 "this locale instance was not properly initialized");

  return hpx::async(::get_locale_running_tasks_action{},
                    hpx::naming::get_id_from_locality_id(locality_id))
      .get();
}

//-----------------------------------------------------------------------------
// the number of locales
std::int64_t numLocales = -1;

// a domain for the Locales array
Domain<1> LocaleSpace;

// The array of locales on which the program is executing.
Array<Domain<1>, locale> Locales;

// For any given task, this variable resolves to the locale value on which the
// task is running.
locale here;

// Initialize data structures related to locale's at runtime startup
void startup() {

  numLocales = hpx::get_num_localities(hpx::launch::sync);
  LocaleSpace = Domain(Range(0, numLocales, BoundsCategoryType::Open));

  std::vector<locale> locales;
  locales.reserve(numLocales);
  for (auto id : LocaleSpace.these()) {
    locales.push_back(id);
  }

  Locales = Array(LocaleSpace, std::move(locales));

  here = locale(hpx::get_locality_id());
}

// De-initialize data structures related to locale's at runtime shutdown
void shutdown() {

  Locales = Array<Domain<1>, locale>();
  LocaleSpace = Domain<1>();
  numLocales = -1;
}

struct register_startup {
  register_startup() {
    hpx::register_pre_startup_function(&startup);
    hpx::register_shutdown_function(&shutdown);
  }
};

register_startup init;
} // namespace chplx
