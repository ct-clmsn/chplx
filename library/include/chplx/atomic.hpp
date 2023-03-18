//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <atomic>

namespace chplx {

//-----------------------------------------------------------------------------
enum class memoryOrder {

  relaxed = static_cast<int>(std::memory_order_relaxed),
  acquire = static_cast<int>(std::memory_order_acquire),
  release = static_cast<int>(std::memory_order_release),
  acqRel = static_cast<int>(std::memory_order_acq_rel),
  seqCst = static_cast<int>(std::memory_order_seq_cst)
};

constexpr std::memory_order to_memorder(memoryOrder order) noexcept {
  return static_cast<std::memory_order>(order);
}

//-----------------------------------------------------------------------------
template <typename T> class Atomic : std::atomic<T> {

  using base_type = std::atomic<T>;

public:
  constexpr Atomic() noexcept = default;
  constexpr Atomic(T value) noexcept : base_type(value) {}

  // Returns the stored value
  T read(memoryOrder order = memoryOrder::seqCst) const noexcept {
    return this->base_type::load(to_memorder(order));
  }

  // Stores value as the new value.
  void write(T value, memoryOrder order = memoryOrder::seqCst) noexcept {
    this->base_type::store(value, to_memorder(order));
  }

  // Stores value as the new value and returns the original value.
  T exchange(T value, memoryOrder order = memoryOrder::seqCst) noexcept {
    return this->base_type::exchange(value, to_memorder(order));
  }

  // Stores desired as the new value, if and only if the original value is equal
  // to expected. Returns true if desired was stored, otherwise updates expected
  // to the original value.
  bool compareExchange(T &expected, T desired,
                       memoryOrder order = memoryOrder::seqCst) noexcept {
    return this->base_type::compare_exchange_strong(expected, desired,
                                                    to_memorder(order));
  }

  bool compareExchange(T &expected, T desired, memoryOrder success,
                       memoryOrder failure) noexcept {
    return this->base_type::compare_exchange_strong(
        expected, desired, to_memorder(success), to_memorder(failure));
  }

  // Similar to compareExchange, except that this function may return false even
  // if the original value was equal to expected. This may happen if the value
  // could not be updated atomically. This weak version is allowed to spuriously
  // fail, but when compareExchange is already in a loop, it can offer better
  // performance on some platforms.
  bool compareExchangeWeak(T &expected, T desired,
                           memoryOrder order = memoryOrder::seqCst) noexcept {
    return this->base_type::compare_exchange_weak(expected, desired,
                                                  to_memorder(order));
  }

  bool compareExchangeWeak(T &expected, T desired, memoryOrder success,
                           memoryOrder failure) noexcept {
    return this->base_type::compare_exchange_weak(
        expected, desired, to_memorder(success), to_memorder(failure));
  }

  // Stores desired as the new value, if and only if the original value is equal
  // to expected. Returns true if desired was stored.
  bool compareAndSwap(T expected, T desired,
                      memoryOrder order = memoryOrder::seqCst) noexcept {

    return this->base_type::compare_exchange_strong(expected, desired,
                                                    to_memorder(order));
  }

  // Adds value to the original value and stores the result. Defined for integer
  // and real atomic types. Returns the original value.
  T fetchAdd(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_arithmetic_v<T>)
  {
    return this->base_type::fetch_add(value, to_memorder(order));
  }

  // Adds value to the original value and stores the result. Defined for integer
  // and real atomic types.
  void add(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_arithmetic_v<T>)
  {
    this->base_type::fetch_add(value, to_memorder(order));
  }

  // Subtracts value from the original value and stores the result. Defined for
  // integer and real atomic types. Returns the original value.
  T fetchSub(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_arithmetic_v<T>)
  {
    return this->base_type::fetch_sub(value, to_memorder(order));
  }

  // Subtracts value from the original value and stores the result. Defined for
  // integer and real atomic types.
  void sub(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_arithmetic_v<T>)
  {
    this->base_type::fetch_sub(value, to_memorder(order));
  }

  // Applies the | operator to value and the original value, then stores the
  // result. Defined for integer and real atomic types. Returns the original
  // value.
  T fetchOr(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_integral_v<T>)
  {
    return this->base_type::fetch_or(value, to_memorder(order));
  }

  // Applies the | operator to value and the original value, then stores the
  // result. Defined for integer and real atomic types.
  void Or(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_integral_v<T>)
  {
    this->base_type::fetch_or(value, to_memorder(order));
  }

  // Applies the & operator to value and the original value, then stores the
  // result. Defined for integer and real atomic types. Returns the original
  // value.
  T fetchAnd(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_integral_v<T>)
  {
    return this->base_type::fetch_and(value, to_memorder(order));
  }

  // Applies the & operator to value and the original value, then stores the
  // result. Defined for integer and real atomic types.
  void And(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_integral_v<T>)
  {
    this->base_type::fetch_and(value, to_memorder(order));
  }

  // Applies the ^ operator to value and the original value, then stores the
  // result. Defined for integer and real atomic types. Returns the original
  // value.
  T fetchXor(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_integral_v<T>)
  {
    return this->base_type::fetch_xor(value, to_memorder(order));
  }

  // Applies the ^ operator to value and the original value, then stores the
  // result. Defined for integer and real atomic types.
  void Xor(T value, memoryOrder order = memoryOrder::seqCst) noexcept
    requires(std::is_integral_v<T>)
  {
    this->base_type::fetch_xor(value, to_memorder(order));
  }

  // Waits until the stored value is equal to value. The implementation may
  // yield the running task while waiting.
  void waitFor(T compare_with,
               memoryOrder order = memoryOrder::seqCst) const noexcept {
    this->base_type::wait(compare_with, to_memorder(order));
  }
};

//-----------------------------------------------------------------------------
template <> class Atomic<bool> : std::atomic<bool> {

  using base_type = std::atomic<bool>;

public:
  constexpr Atomic() noexcept = default;
  constexpr Atomic(bool value) noexcept : base_type(value) {}

  // Returns the stored value
  bool read(memoryOrder order = memoryOrder::seqCst) const noexcept {
    return this->base_type::load(to_memorder(order));
  }

  // Stores value as the new value.
  void write(bool value, memoryOrder order = memoryOrder::seqCst) noexcept {
    this->base_type::store(value, to_memorder(order));
  }

  // Stores value as the new value and returns the original value.
  bool exchange(bool value, memoryOrder order = memoryOrder::seqCst) noexcept {
    return this->base_type::exchange(value, to_memorder(order));
  }

  // Stores desired as the new value, if and only if the original value is equal
  // to expected. Returns true if desired was stored, otherwise updates expected
  // to the original value.
  bool compareExchange(bool &expected, bool desired,
                       memoryOrder order = memoryOrder::seqCst) noexcept {
    return this->base_type::compare_exchange_strong(expected, desired,
                                                    to_memorder(order));
  }

  bool compareExchange(bool &expected, bool desired, memoryOrder success,
                       memoryOrder failure) noexcept {
    return this->base_type::compare_exchange_strong(
        expected, desired, to_memorder(success), to_memorder(failure));
  }

  // Similar to compareExchange, except that this function may return false even
  // if the original value was equal to expected. This may happen if the value
  // could not be updated atomically. This weak version is allowed to spuriously
  // fail, but when compareExchange is already in a loop, it can offer better
  // performance on some platforms.
  bool compareExchangeWeak(bool &expected, bool desired,
                           memoryOrder order = memoryOrder::seqCst) noexcept {
    return this->base_type::compare_exchange_weak(expected, desired,
                                                  to_memorder(order));
  }

  bool compareExchangeWeak(bool &expected, bool desired, memoryOrder success,
                           memoryOrder failure) noexcept {
    return this->base_type::compare_exchange_weak(
        expected, desired, to_memorder(success), to_memorder(failure));
  }

  // Stores desired as the new value, if and only if the original value is equal
  // to expected. Returns true if desired was stored.
  bool compareAndSwap(bool expected, bool desired,
                      memoryOrder order = memoryOrder::seqCst) noexcept {
    return this->base_type::compare_exchange_strong(expected, desired,
                                                    to_memorder(order));
  }

  // Stores true as the new value and returns the old value.
  bool testAndSet(memoryOrder order = memoryOrder::seqCst) noexcept {
    return this->base_type::exchange(true, to_memorder(order));
  }

  // Stores false as the new value.
  void clear(memoryOrder order = memoryOrder::seqCst) noexcept {
    this->base_type::store(false, to_memorder(order));
  }

  // Waits until the stored value is equal to value. The implementation may
  // yield the running task while waiting.
  void waitFor(bool compare_with,
               memoryOrder order = memoryOrder::seqCst) const noexcept {
    this->base_type::wait(compare_with, to_memorder(order));
  }
};

//-----------------------------------------------------------------------------
// An atomic fence that establishes an ordering of non-atomic and relaxed atomic
// operations.
inline void atomicFence(memoryOrder order = memoryOrder::seqCst) noexcept {
  std::atomic_thread_fence(to_memorder(order));
}

//-----------------------------------------------------------------------------
// Make sure atomic variables are always passed by reference to tasks
template <typename T> struct detail::task_intent<Atomic<T>> {

  using type = std::reference_wrapper<T>;

  static constexpr decltype(auto) call(Atomic<T> &arg) noexcept {
    return std::ref(arg);
  }
};
} // namespace chplx
