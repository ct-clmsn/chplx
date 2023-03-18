//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/types.hpp>

#include <hpx/assert.hpp>
#include <hpx/modules/synchronization.hpp>

#include <functional>
#include <mutex>
#include <utility>

namespace chplx {

template <typename T> class Single {

public:
  // construct empty
  constexpr Single() = default;

  // construct full from given value
  explicit Single(T val) : value(std::move(val)), full(true) {}

  // inhibit moving/copying
  Single(Single const &) = delete;
  Single(Single &&) = delete;
  Single &operator=(Single const &) = delete;
  Single &operator=(Single &&) = delete;

  ~Single() = default;

  // Returns true if the single variable is in the full state, false otherwise.
  bool isFull() const {
    std::scoped_lock l(mtx);
    return full;
  }

  // Blocks until the single variable is full. Read the value of the single
  // variable and and leave the variable full.
  T readFF() const {
    std::unique_lock<hpx::spinlock> l(mtx);
    if (!full) {
      // wait for variable to become full
      cv_write.wait(l);
    }
    return value;
  }

  // Read the value of the single variable. For a full single, returns a copy of
  // the value stored. For an empty single, the implementation will return
  // either a new default-initialized value of the value type or the last value
  // stored. Does not change the full/empty state
  T readXX() const {
    std::scoped_lock l(mtx);
    return value;
  }

  // Write the value of the single variable and leave the variable full
  void writeEF(T val) {
    {
      std::scoped_lock l(mtx);
      HPX_ASSERT(!full);
      value = std::move(val);
      full = true;
    }
    cv_write.notify_all();
  }

private:
  T value{};
  bool full = false;
  mutable hpx::spinlock mtx{};
  mutable hpx::condition_variable_any cv_write{};
};

//-----------------------------------------------------------------------------
// Make sure single variables are always passed by reference to tasks
template <typename T> struct detail::task_intent<Single<T>> {

  using type = std::reference_wrapper<T>;

  static constexpr decltype(auto) call(Single<T> &arg) noexcept {
    return std::ref(arg);
  }
};
} // namespace chplx
