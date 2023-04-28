//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iostream>
#include <type_traits>

namespace chplx {

//-----------------------------------------------------------------------------
template <typename T> void write_one(std::ostream &os, T &&t) { os << t; }

template <typename... Ts> void write(std::ostream &os, Ts &&...ts) {
  (write_one(os, std::forward<Ts>(ts)), ...);
}

template <typename... Ts> void writeln(std::ostream &os, Ts &&...ts) {
  write(os, std::forward<Ts>(ts)...);
  os << "\n";
}

inline void writeln(std::ostream &os) { os << "\n"; }

//-----------------------------------------------------------------------------
template <typename T, typename... Ts>
  requires(!std::is_convertible_v<std::decay_t<T> &, std::ostream &>)
void write(T &&t, Ts &&...ts) {
  write(std::cout, std::forward<T>(t), std::forward<Ts>(ts)...);
}

template <typename T, typename... Ts>
  requires(!std::is_convertible_v<std::decay_t<T> &, std::ostream &>)
void writeln(T &&t, Ts &&...ts) {
  writeln(std::cout, std::forward<T>(t), std::forward<Ts>(ts)...);
}

inline void writeln() { writeln(std::cout); }

} // namespace chplx
