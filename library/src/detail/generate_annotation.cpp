//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#include <hpx/modules/assertion.hpp>
#include <hpx/modules/format.hpp>

#include <filesystem>
#include <string>

namespace chplx::detail {

    std::string generate_annotation(hpx::source_location const& location)
    {
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
        std::filesystem::path p(location.file_name());
        return hpx::util::format("%s(%d): %s", p.filename(), location.line(),
            location.function_name());
#else
        return {};
#endif
    }
}    // namespace chplx::detail
