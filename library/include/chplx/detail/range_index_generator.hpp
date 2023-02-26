//  Copyright (c) 2015-2017 Bryce Adelstein Lelbach aka wash
//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#include <array>
#include <cassert>
#include <exception>

namespace chplx::detail {

template <typename IndexType, std::size_t N> struct Position {
  constexpr Position() noexcept = default;

  template <typename... Indices>
  constexpr Position(Indices... idxs_) noexcept
      : idxs{{static_cast<IndexType>(idxs_)...}} {
    static_assert(sizeof...(Indices) == N,
                  "Insufficient index parameters passed to constructor.");
  }

  constexpr Position(Position const &) noexcept = default;
  constexpr Position(Position &&) noexcept = default;
  constexpr Position &operator=(Position const &) noexcept = default;
  constexpr Position &operator=(Position &&) noexcept = default;

  ~Position() = default;

  constexpr IndexType &operator[](IndexType i) noexcept { return idxs[i]; }
  constexpr IndexType const &operator[](IndexType i) const noexcept {
    return idxs[i];
  }

private:
  std::array<IndexType, N> idxs;
};

template <typename IndexType> struct Dimension {
  constexpr Dimension() noexcept = default;

  constexpr Dimension(IndexType extent_, IndexType idx_) noexcept
      : extent(extent_), idx(idx_) {}

  constexpr Dimension(Dimension const &) = default;
  constexpr Dimension(Dimension &&) = default;

  constexpr Dimension &operator=(Dimension const &) = default;
  constexpr Dimension &operator=(Dimension &&) noexcept = default;

  ~Dimension() = default;

  IndexType const extent = 0;
  IndexType idx = 0;
};

template <typename IndexType, std::size_t N> struct IndexGenerator {
  static_assert(N != 0, "N must be greater than 0.");

  struct promise_type {
    using return_type = IndexGenerator;

    Position<IndexType, N> pos;
    std::exception_ptr exception_;

    constexpr hpx::coro::suspend_always
    yield_value(Position<IndexType, N> pos_) noexcept {
      pos = pos_;
      return {};
    }

    constexpr hpx::coro::suspend_always initial_suspend() const noexcept {
      return {};
    }

    constexpr hpx::coro::suspend_always final_suspend() const noexcept {
      return {};
    }

    IndexGenerator get_return_object() noexcept { return IndexGenerator(this); }

    static constexpr void return_void() noexcept {}

    void unhandled_exception() { exception_ = std::current_exception(); }
  };
  using handle_type = hpx::coro::coroutine_handle<promise_type>;

  struct iterator {
    handle_type coro;
    bool done;

    constexpr iterator(handle_type coro_, bool done_)
        : coro(coro_), done(done_) {}

    iterator &operator++() {
      coro.resume();
      done = coro.done();
      return *this;
    }

    Position<IndexType, N> operator*() const { return coro.promise().pos; }

    constexpr bool operator==(iterator const &rhs) const noexcept {
      return done == rhs.done;
    }
    constexpr bool operator!=(iterator const &rhs) const noexcept {
      return !(*this == rhs);
    }
  };

  iterator begin() {
    p.resume();
    return iterator(p, p.done());
  }

  constexpr iterator end() { return iterator(p, true); }

  constexpr IndexGenerator(IndexGenerator const &rhs) = delete;
  constexpr IndexGenerator(IndexGenerator &&rhs) noexcept : p(rhs.p) {
    rhs.p = nullptr;
  }

  constexpr IndexGenerator &operator=(IndexGenerator const &rhs) = delete;
  constexpr IndexGenerator &operator=(IndexGenerator &&rhs) noexcept {
    p = rhs.p;
    rhs.p = nullptr;
    return *this;
  }

  ~IndexGenerator() {
    if (p)
      p.destroy();
  }

private:
  explicit IndexGenerator(promise_type *p) noexcept
      : p(handle_type::from_promise(*p)) {}

  handle_type p{};
};

} // namespace chplx::detail
