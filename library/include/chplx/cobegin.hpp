//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/detail/generate_annotation.hpp>

#include <hpx/config.hpp>
#include <hpx/execution.hpp>
#include <hpx/modules/assertion.hpp>
#include <hpx/parallel/task_group.hpp>

namespace chplx {

    template <typename F, typename... Fs>
    void cobegin(hpx::source_location const& location, F&& f, Fs&&... fs)
    {
        auto exec = hpx::execution::par.executor();
        hpx::experimental::task_group g;

        auto annotation = detail::generate_annotation(location);

        g.run(exec, hpx::annotated_function(std::forward<F>(f), annotation));
        (g.run(exec, hpx::annotated_function(std::forward<Fs>(fs), annotation)),
            ...);

        g.wait();
    }

    template <typename F, typename... Fs>
        requires(!std::is_same_v<std::decay_t<F>, hpx::source_location>)
    void cobegin(F&& f, Fs&&... fs)
    {
#if defined(CHPLX_NO_SOURCE_LOCATION)
        auto exec = hpx::execution::par.executor();
        hpx::experimental::task_group g;
        g.run(exec, std::forward<F>(f));
        (g.run(exec, std::forward<F>(fs)), ...);
        g.wait();
#else
        cobegin(HPX_CURRENT_SOURCE_LOCATION(), std::forward<F>(f),
            std::forward<Fs>(fs)...);
#endif
    }
}    // namespace chplx
