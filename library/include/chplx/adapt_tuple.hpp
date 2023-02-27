//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// this is inspired by: https://www.foonathan.net/2017/03/tuple-iterator/

#pragma once

#include <hpx/assert.hpp>
#include <hpx/modules/iterator_support.hpp>
#include <hpx/modules/type_support.hpp>

#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>

namespace chplx {
namespace detail {

//-----------------------------------------------------------------------------
template <typename Tuple, typename R, std::size_t... Is>
struct tuple_runtime_access_table {

  using tuple_type = Tuple;
  using return_type = std::conditional_t<std::is_reference_v<R>, R, R &>;

  template <std::size_t N>
  [[nodiscard]] static return_type access_tuple(tuple_type &t) noexcept {

    return std::get<N>(t);
  }

  using accessor_fun_ptr = return_type (*)(tuple_type &) noexcept;
  static constexpr std::size_t table_size = sizeof...(Is);

  static constexpr std::array<accessor_fun_ptr, table_size> lookup_table = {
      {&access_tuple<Is>...}};
};

//-----------------------------------------------------------------------------
template <typename Tuple> struct first_element;

template <typename T, typename... Ts>
struct first_element<std::tuple<T, Ts...>> {

  static_assert(hpx::util::all_of_v<std::is_same<T, Ts>...>,
                "the tuple type must be homogeneous");

  using type = T;
};

template <typename Tuple>
using first_element_t = typename first_element<Tuple>::type;

//-----------------------------------------------------------------------------
template <typename Tuple, std::size_t... Is>
[[nodiscard]] constexpr decltype(auto)
call_access_function(Tuple &t, std::size_t i,
                     std::index_sequence<Is...>) noexcept {

  HPX_ASSERT_MSG(i < sizeof...(Is), "index must be smaller than tuple size");

  auto &table =
      tuple_runtime_access_table<Tuple, first_element_t<std::decay_t<Tuple>>,
                                 Is...>::lookup_table;
  return table[i](t);
}

//-----------------------------------------------------------------------------
template <typename Tuple>
[[nodiscard]] constexpr decltype(auto) runtime_get(Tuple &t,
                                                   std::size_t i) noexcept {

  return call_access_function(
      t, i, std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
}

//-----------------------------------------------------------------------------
template <typename Tuple>
class tuple_iterator
    : public hpx::util::iterator_facade<tuple_iterator<Tuple>,
                                        typename first_element<Tuple>::type,
                                        std::random_access_iterator_tag> {

  using base_type =
      hpx::util::iterator_facade<tuple_iterator,
                                 typename first_element<Tuple>::type,
                                 std::random_access_iterator_tag>;

public:
  using iterator_category = typename base_type::iterator_category;
  using value_type = typename base_type::value_type;
  using difference_type = typename base_type::difference_type;
  using pointer = typename base_type::pointer;
  using reference = typename base_type::reference;

  explicit constexpr tuple_iterator(Tuple &t, std::size_t idx = 0) noexcept
      : t{std::addressof(t)}, i{idx} {}

private:
  Tuple *t;
  std::size_t i;

  friend hpx::util::iterator_core_access;

  reference dereference() const noexcept { return runtime_get(*t, i); }

  void increment() noexcept { ++i; }
  void decrement() noexcept { --i; }

  void advance(std::ptrdiff_t n) noexcept { i += n; }

  [[nodiscard]] constexpr bool equal(tuple_iterator const &rhs) const {

    return rhs.t == t && rhs.i == i;
  }

  [[nodiscard]] constexpr std::ptrdiff_t
  distance_to(tuple_iterator const &rhs) const noexcept {

    return rhs.i - i;
  }
};

} // namespace detail

//-----------------------------------------------------------------------------
template <typename Tuple> class tuple_range {

  Tuple &t;

public:
  explicit constexpr tuple_range(Tuple &t) noexcept : t{t} {}

  [[nodiscard]] constexpr auto begin() const noexcept {

    return detail::tuple_iterator{t};
  }

  [[nodiscard]] constexpr auto end() const noexcept {

    return detail::tuple_iterator{t, std::tuple_size_v<std::decay_t<Tuple>>};
  }
};

} // namespace chplx
