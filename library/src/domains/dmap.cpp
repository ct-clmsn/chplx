//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx/dmap.hpp>

namespace chplx {

//-----------------------------------------------------------------------------
void intrusive_ptr_add_ref(BaseDist *p) noexcept { ++p->count; }

void intrusive_ptr_release(BaseDist *p) noexcept {
  if (0 == --p->count)
    delete p;
}

//-----------------------------------------------------------------------------
void intrusive_ptr_add_ref(BaseDomain *p) noexcept { ++p->count; }

void intrusive_ptr_release(BaseDomain *p) noexcept {
  if (0 == --p->count)
    delete p;
}

//-----------------------------------------------------------------------------
void intrusive_ptr_add_ref(BaseArray *p) noexcept { ++p->count; }

void intrusive_ptr_release(BaseArray *p) noexcept {
  if (0 == --p->count)
    delete p;
}
}
