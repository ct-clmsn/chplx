//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/array.hpp>
#include <chplx/domain.hpp>
#include <chplx/string.hpp>
#include <chplx/types.hpp>

#include <hpx/modules/actions.hpp>
#include <hpx/modules/async_combinators.hpp>
#include <hpx/modules/async_distributed.hpp>
#include <hpx/modules/naming_base.hpp>
#include <hpx/modules/serialization.hpp>

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace chplx {

// A locale is a Chapel abstraction for a piece of a target architecture that
// has processing and storage capabilities. Generally speaking, the tasks
// running within a locale have roughly uniform access to values stored in the
// locale's local memory and longer latencies for accessing the memories of
// other locales.
struct locale {

  locale() = default;

  constexpr locale(std::uint32_t id) noexcept : locality_id(id) {}

  locale(locale const &) = default;
  locale(locale &&rhs) = default;

  locale &operator=(locale const &) = default;
  locale &operator=(locale &&) = default;

  ~locale() = default;

  // Get the hostname of this locale. Returns the hostname of the compute node
  // associated with the locale.
  String hostname() const;

  // Get the name of this locale. In general, this method returns the same
  // string as locale.hostname; however, it can differ when the program is
  // executed in an oversubscribed manner.
  String name() const { return hostname(); }

  // Get the unique integer identifier for this locale. Returns index of this
  // locale in the range 0..numLocales-1.
  constexpr std::int64_t id() const noexcept { return locality_id; }

  // Get the maximum task concurrency that one can expect to achieve on this
  // locale. Returns the maximum number of tasks that can run in parallel on
  // this locale.
  //
  // Note that the value is an estimate by the runtime tasking layer.
  // Typically it is the number of physical processor cores available to the
  // program. Executing a data-parallel construct with more tasks than this
  // is unlikely to improve performance.
  std::int64_t maxTaskPar() const;

  // Get the number of processing units available on this locale.
  //
  // logical : bool – Count logical PUs (hyperthreads and the like), or
  // physical ones (cores)? Defaults to false, for cores.
  //
  // accessible : bool – Count only PUs that can be reached, or all of them?
  // Defaults to true, for accessible PUs.
  std::int64_t numPUs(bool logical = false, bool accessible = true) const;

  // Get the number of tasks running on this locale.
  //
  // This method is intended to guide task creation during a parallel section.
  // If the number of running tasks is greater than or equal to the locale's
  // maximum task parallelism (queried via locale.maxTaskPar), then creating
  // more tasks is unlikely to decrease wall time.
  std::int64_t runningTasks() const;

  constexpr bool operator==(locale const &rhs) const noexcept {
    return locality_id == rhs.id();
  }
  constexpr bool operator!=(locale const &rhs) const noexcept {
    return !(*this == rhs);
  }

private:
  // same as hpx::naming::invalid_locality_id
  std::uint32_t locality_id = ~static_cast<std::uint32_t>(0);
};

//-----------------------------------------------------------------------------
// The number of locales.
extern std::int64_t numLocales;

// A domain for the Locales array.
extern Domain<1> LocaleSpace;

// The array of locales on which the program is executing.
//
// Note that the Locales array is typically defined such that distinct elements
// refer to distinct resources on the target parallel architecture. In
// particular, the Locales array itself should not be used in an oversubscribed
// manner in which a single processor resource is represented by multiple locale
// values (except during development). Over-subscription should instead be
// handled by creating an aggregate of locale values and referring to it in
// place of the Locales array.
extern Array<Domain<1>, locale> Locales;

// For any given task, this variable resolves to the locale value on which the
// task is running.
extern locale here;

//-----------------------------------------------------------------------------
namespace detail {

// Wrap a function object (e.g. any lambda) such that it can be sent over the
// wire. Note that this will support only lambdas that have an empty capture
// clause.
template <typename F>
  requires(std::is_class_v<F> && std::is_empty_v<F>)
struct OnWrapper {

  OnWrapper() = default;

  template <typename F_, typename = std::enable_if_t<
                             !std::is_same_v<std::decay_t<F_>, OnWrapper>>>
  explicit OnWrapper(F_ &&f) : f(std::forward<F_>(f)) {}

  template <typename... Ts> auto operator()(Ts &&...ts) {
    return f(std::forward<Ts>(ts)...);
  }

  template <typename... Ts> auto operator()(Ts &&...ts) const {
    return f(std::forward<Ts>(ts)...);
  }

  F f;
};

template <typename F>
  requires(std::is_class_v<F> && std::is_empty_v<F>)
OnWrapper(F) -> OnWrapper<std::decay_t<F>>;

template <typename F, typename... Ts>
void invokeOnWrapper(OnWrapper<F> f, Ts... ts) {
  f(std::move(ts)...);
}

} // namespace detail
} // namespace chplx

// locales and OnWrappers are bitwise serializable
template <>
struct hpx::traits::is_bitwise_serializable<chplx::locale> : std::true_type {};

template <typename F>
struct hpx::traits::is_bitwise_serializable<chplx::detail::OnWrapper<F>>
    : std::true_type {};

template <typename F, typename... Ts>
struct invokeOnWrapperAction
    : hpx::actions::make_action_t<
          decltype(&chplx::detail::invokeOnWrapper<F, Ts...>),
          &chplx::detail::invokeOnWrapper<F, Ts...>,
          invokeOnWrapperAction<F, Ts...>> {};

//-----------------------------------------------------------------------------
namespace chplx {

// The on statement controls on which locale a block of code should be
// executed or data should be placed.
template <typename F, typename... Ts> void on(locale loc, F &&f, Ts &&...ts) {

  hpx::wait_all(hpx::async(
      invokeOnWrapperAction<std::decay_t<F>, std::decay_t<Ts>...>{},
      hpx::naming::get_id_from_locality_id(loc.id()),
      chplx::detail::OnWrapper(std::forward<F>(f)), std::forward<Ts>(ts)...));
}

} // namespace chplx
