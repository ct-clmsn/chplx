/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <variant>

namespace chpl { namespace backend { namespace symbols {

using Symbol = std::variant<
std::monostate
>;

} /* namespace symbols */ } /* nameaspace backend */ } /* namespace chpl */
