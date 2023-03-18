//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/types.hpp>

#include <hpx/modules/synchronization.hpp>

#include <functional>
#include <mutex>
#include <utility>

namespace chplx {

template <typename T> class sync {

public:
  // construct empty
  constexpr sync() = default;

  // construct full from given value
  explicit sync(T val) : value(std::move(val)), full(true) {}

  // inhibit moving/copying
  sync(sync const &) = delete;
  sync(sync &&) = delete;
  sync &operator=(sync const &) = delete;
  sync &operator=(sync &&) = delete;

  ~sync() = default;

  // Returns true if the sync or single variable is in the full state, false
  // otherwise.
  bool isFull() const {
    std::scoped_lock l(mtx);
    return full;
  }

  // Resets the value of this sync variable to the default value of its type.
  // This method is non-blocking and the state of the sync variable is set to
  // empty when this method completes.
  void reset() {
    std::scoped_lock l(mtx);
    full = false;
    cv_read.notify_one();
  }

  // Blocks until the sync variable is full. Read the value of the sync variable
  // and set the variable to empty.
  T readFE() const {
    std::unique_lock<hpx::spinlock> l(mtx);
    while (!full) {
      // wait for variable to become full
      cv_write.wait(l);
    }

    T result = std::move(value);
    full = false;
    cv_read.notify_one();
    return result;
  }

  // Blocks until the sync variable is full. Read the value of the sync variable
  // and and leave the variable full.
  T readFF() const {
    std::unique_lock<hpx::spinlock> l(mtx);
    while (!full) {
      // wait for variable to become full
      cv_write.wait(l);
    }
    return value;
  }

  // Read the value of the sync variable. For a full sync, returns a copy of the
  // value stored. For an empty sync, the implementation will return either a
  // new default-initialized value of the value type or the last value stored.
  // Does not change the full/empty state
  T readXX() const {
    std::scoped_lock l(mtx);
    return value;
  }

  // Block until the sync variable is empty. Write the value of the sync
  // variable and leave the variable full
  void writeEF(T val) {
    std::unique_lock<hpx::spinlock> l(mtx);
    while (full) {
      // wait for variable to become empty
      cv_read.wait(l);
    }
    value = std::move(val);
    full = true;
    cv_write.notify_one();
  }

  // Block until the sync variable is full. Write the value of the sync
  // variable and leave the variable full.
  void writeFF(T val) {
    std::unique_lock<hpx::spinlock> l(mtx);
    while (!full) {
      // wait for variable to become full
      cv_write.wait(l);
    }
    value = std::move(val);
  }

  // Write the value of the sync variable and leave the variable full.
  void writeXF(T val) {
    std::unique_lock<hpx::spinlock> l(mtx);
    value = std::move(val);
  }

private:
  T value{};

  // all members below are mutable to be able to make the read functions const
  mutable bool full = false;
  mutable hpx::spinlock mtx{};
  mutable hpx::condition_variable_any cv_read{};
  mutable hpx::condition_variable_any cv_write{};
};

//-----------------------------------------------------------------------------
// Make sure sync variables are always passed by reference to tasks
template <typename T> struct detail::task_intent<sync<T>> {

  using type = std::reference_wrapper<T>;

  static constexpr decltype(auto) call(sync<T> &arg) noexcept {
    return std::ref(arg);
  }
};
} // namespace chplx
