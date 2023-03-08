//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// this is inspired by: https://www.foonathan.net/2017/03/tuple-iterator/

#pragma once

#include <chplx/types.hpp>

#include <hpx/assert.hpp>
#include <hpx/modules/iterator_support.hpp>
#include <hpx/modules/type_support.hpp>

#include <array>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <variant>

namespace chplx {
namespace detail {

//-----------------------------------------------------------------------------
template <typename Tuple, typename F, typename Pack> struct forLoopTable;

template <typename Tuple, typename F, std::size_t... Is>
struct forLoopTable<Tuple, F, std::index_sequence<Is...>> {

  template <std::size_t N>
  static constexpr void accessFunc(Tuple &t, F &f) noexcept {
    f(std::get<N>(t));
  }

  using accessFuncType = void (*)(Tuple &t, F &f) noexcept;

  static constexpr std::array<accessFuncType, sizeof...(Is)> lookupTable = {
      &accessFunc<Is>...};
};

//-----------------------------------------------------------------------------
template <typename R, typename Tuple, typename F, std::size_t... Is>
struct AccessTable {

  using tuple_type = Tuple;
  using return_type = R;

  template <std::size_t N>
  [[nodiscard]] static return_type accessTuple(tuple_type &t, F &f) noexcept {

    return f(std::get<N>(t));
  }

  using accessor_fun_ptr = return_type (*)(tuple_type &, F &) noexcept;
  static constexpr std::size_t table_size = sizeof...(Is);

  static constexpr std::array<accessor_fun_ptr, table_size> lookupTable = {
      {&accessTuple<Is>...}};
};

//-----------------------------------------------------------------------------
template <typename R, typename Tuple, typename F, std::size_t... Is>
[[nodiscard]] constexpr decltype(auto)
callAccessFunction(Tuple &t, std::size_t i, F &&f,
                   std::index_sequence<Is...>) noexcept {

  HPX_ASSERT_MSG(i < sizeof...(Is), "index must be smaller than tuple size");

  auto &table = AccessTable<R, Tuple, F, Is...>::lookupTable;
  return table[i](t, f);
}

//-----------------------------------------------------------------------------
template <typename Tuple>
using FirstElement_t = std::tuple_element_t<0, std::remove_reference_t<Tuple>>;

//-----------------------------------------------------------------------------
template <typename Tuple>
[[nodiscard]] constexpr decltype(auto)
homogenousRuntimeGet(Tuple &t, std::size_t i) noexcept {

  return callAccessFunction<FirstElement_t<Tuple> &>(
      t, i, hpx::identity{},
      std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
}

//-----------------------------------------------------------------------------
template <typename Tuple>
class HomogenousTupleIterator
    : public hpx::util::iterator_facade<HomogenousTupleIterator<Tuple>,
                                        FirstElement_t<Tuple>,
                                        std::random_access_iterator_tag> {

  using base_type =
      hpx::util::iterator_facade<HomogenousTupleIterator, FirstElement_t<Tuple>,
                                 std::random_access_iterator_tag>;

public:
  using iterator_category = typename base_type::iterator_category;
  using value_type = typename base_type::value_type;
  using difference_type = typename base_type::difference_type;
  using pointer = typename base_type::pointer;
  using reference = typename base_type::reference;

  explicit constexpr HomogenousTupleIterator(Tuple &t,
                                             std::size_t idx = 0) noexcept
      : t{std::addressof(t)}, i{idx} {}

private:
  Tuple *t;
  std::size_t i;

  friend hpx::util::iterator_core_access;

  constexpr reference dereference() const noexcept {
    return homogenousRuntimeGet(*t, i);
  }

  void increment() noexcept { ++i; }
  void decrement() noexcept { --i; }

  void advance(std::ptrdiff_t n) noexcept { i += n; }

  [[nodiscard]] constexpr bool equal(HomogenousTupleIterator const &rhs) const {
    return rhs.t == t && rhs.i == i;
  }

  [[nodiscard]] constexpr std::ptrdiff_t
  distance_to(HomogenousTupleIterator const &rhs) const noexcept {
    return rhs.i - i;
  }
};

//-----------------------------------------------------------------------------
// Generate variant that uniquely holds all of the tuple types
template <typename Tuple> struct VariantFromTuple;

template <typename... Ts> struct VariantFromTuple<std::tuple<Ts...>> {

  using type =
      hpx::meta::invoke<hpx::meta::unique<hpx::meta::func<std::variant>>,
                        std::reference_wrapper<Ts>...>;
};

template <typename Tuple>
using VariantFromTuple_t = typename VariantFromTuple<Tuple>::type;

//-----------------------------------------------------------------------------
template <typename Tuple>
[[nodiscard]] constexpr decltype(auto) runtimeGet(Tuple &t,
                                                  std::size_t i) noexcept {

  return callAccessFunction<VariantFromTuple_t<std::decay_t<Tuple>>>(
      t, i, [](auto &element) { return std::ref(element); },
      std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
}

//-----------------------------------------------------------------------------
template <typename Tuple>
class TupleIterator
    : public hpx::util::iterator_facade<
          TupleIterator<Tuple>, VariantFromTuple_t<Tuple>,
          std::random_access_iterator_tag, VariantFromTuple_t<Tuple>> {

  using base_type =
      hpx::util::iterator_facade<TupleIterator, VariantFromTuple_t<Tuple>,
                                 std::random_access_iterator_tag,
                                 VariantFromTuple_t<Tuple>>;

public:
  using iterator_category = typename base_type::iterator_category;
  using value_type = typename base_type::value_type;
  using difference_type = typename base_type::difference_type;
  using pointer = typename base_type::pointer;
  using reference = typename base_type::reference;

  explicit constexpr TupleIterator(Tuple &t, std::size_t idx = 0) noexcept
      : t{std::addressof(t)}, i{idx} {}

private:
  Tuple *t;
  std::size_t i;

  friend hpx::util::iterator_core_access;

  constexpr reference dereference() const noexcept { return runtimeGet(*t, i); }

  void increment() noexcept { ++i; }
  void decrement() noexcept { --i; }

  void advance(std::ptrdiff_t n) noexcept { i += n; }

  [[nodiscard]] constexpr bool equal(TupleIterator const &rhs) const {
    return rhs.t == t && rhs.i == i;
  }

  [[nodiscard]] constexpr std::ptrdiff_t
  distance_to(TupleIterator const &rhs) const noexcept {
    return rhs.i - i;
  }
};

} // namespace detail

//-----------------------------------------------------------------------------
// Access elements of homogenous tuple by index
template <typename Tuple> class HomogenousTupleRange {

  Tuple *t;

public:
  explicit constexpr HomogenousTupleRange(Tuple &t) noexcept
      : t(std::addressof(t)) {}

  // iteration support
  [[nodiscard]] constexpr auto begin() const noexcept {

    return detail::HomogenousTupleIterator{*t};
  }
  [[nodiscard]] constexpr auto end() const noexcept {

    return detail::HomogenousTupleIterator{
        *t, std::tuple_size_v<std::decay_t<Tuple>>};
  }

  // index operator
  [[nodiscard]] decltype(auto) operator[](std::size_t i) {

    return detail::homogenousRuntimeGet(*t, i);
  }
  [[nodiscard]] decltype(auto) operator[](std::size_t i) const {

    return detail::homogenousRuntimeGet(*t, i);
  }
};

//-----------------------------------------------------------------------------
// Access elements of non-homogenous tuple by index
template <typename Tuple> class TupleRange {

  Tuple *t;

public:
  explicit constexpr TupleRange(Tuple &t) noexcept : t(std::addressof(t)) {}

  // iteration support
  [[nodiscard]] constexpr auto begin() const noexcept {

    return detail::TupleIterator{*t};
  }
  [[nodiscard]] constexpr auto end() const noexcept {

    return detail::TupleIterator{*t, std::tuple_size_v<std::decay_t<Tuple>>};
  }
};

} // namespace chplx
