//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/detail/generate_annotation.hpp>
#include <chplx/types.hpp>

#include <hpx/config.hpp>
#include <hpx/execution.hpp>
#include <hpx/modules/assertion.hpp>
#include <hpx/modules/threading_base.hpp>

#include <type_traits>
#include <utility>

namespace chplx {

#if defined(CHPLX_NO_SOURCE_LOCATION)
    template <typename F, typename... Args>
    void begin(F&& f, Args&&... args)
    {
        hpx::parallel::execution::post(hpx::execution::par.executor(),
            std::forward<F>(f),
            detail::task_intent<std::decay_t<Args>>::call(
                std::forward<Args>(args))...);
    }
#else
    template <typename F, typename... Args>
    void begin(hpx::source_location const& location, F&& f, Args&&... args)
    {
        hpx::parallel::execution::post(hpx::execution::par.executor(),
            hpx::annotated_function(
                std::forward<F>(f), detail::generate_annotation(location)),
            detail::task_intent<std::decay_t<Args>>::call(
                std::forward<Args>(args))...);
    }
#endif
}    // namespace chplx
