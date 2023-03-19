//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/modules/runtime_local.hpp>

namespace chplx {

// register startup/shutdown code for a module
template <typename Module> struct registerModule {

  registerModule() {
    hpx::register_pre_startup_function(&Module::__construct);
    hpx::register_pre_shutdown_function(&Module::__destruct);
  }
};
} // namespace chplx
