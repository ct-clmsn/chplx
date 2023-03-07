//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_tuple.hpp>
#include <chplx/range.hpp>
#include <chplx/types.hpp>

#include <hpx/assert.hpp>
#include <hpx/config.hpp>
#include <hpx/modules/type_support.hpp>

#include <cstddef>
#include <functional>
#include <limits>
#include <tuple>

namespace chplx {

template <typename... Ts> struct Tuple : std::tuple<Ts...> {

  using base_type = std::tuple<Ts...>;

  using std::tuple<Ts...>::tuple;

  constexpr std::tuple<Ts...> &base() noexcept {
    return static_cast<std::tuple<Ts...> &>(*this);
  }
  constexpr std::tuple<Ts...> const &base() const noexcept {
    return static_cast<std::tuple<Ts...> const &>(*this);
  }

  static constexpr bool isHomogenous() noexcept {

    if constexpr (size() == 0) {
      return true;
    } else {
      using first_element = detail::FirstElement_t<base_type>;
      return hpx::util::all_of_v<std::is_same<first_element, Ts>...>;
    }
  }

  // Returns the size of the tuple.
  static constexpr std::size_t size() noexcept {
    return std::tuple_size_v<std::tuple<Ts...>>;
  }

  // Returns the range 0..<this.size representing the indices that are legal for
  // indexing into the tuple.
  static constexpr auto indices() noexcept {
    return Range(static_cast<std::size_t>(0), size(),
                 chplx::BoundsCategoryType::Open);
  }

  // iteration support
  [[nodiscard]] constexpr auto begin() const noexcept
    requires(isHomogenous())
  {
    return detail::HomogenousTupleIterator{base()};
  }
  [[nodiscard]] constexpr auto end() const noexcept
    requires(isHomogenous())
  {
    return detail::HomogenousTupleIterator{base(), size()};
  }

  // index operator
  [[nodiscard]] decltype(auto) operator[](std::size_t i)
    requires(isHomogenous())
  {
    return HomogenousTupleRange(base())[i];
  }
  [[nodiscard]] decltype(auto) operator[](std::size_t i) const
    requires(isHomogenous())
  {
    return HomogenousTupleRange(*this)[i];
  }

  // alternative index operator
  [[nodiscard]] decltype(auto) operator()(std::size_t i)
    requires(isHomogenous())
  {
    return HomogenousTupleRange(base())[i];
  }
  [[nodiscard]] decltype(auto) operator()(std::size_t i) const
    requires(isHomogenous())
  {
    return HomogenousTupleRange(*this)[i];
  }
};

template <typename... Ts> Tuple(Ts &&...) -> Tuple<Ts...>;

//-----------------------------------------------------------------------------
// Returns true if t is a tuple; otherwise false.
template <typename T> inline constexpr bool isTupleType = false;

template <typename... Ts>
inline constexpr bool isTupleType<Tuple<Ts...>> = true;

// Returns true if the value T is a range value.
template <typename T> [[nodiscard]] constexpr bool isTupleValue(T) noexcept {
  return false;
}

template <typename... Ts>
[[nodiscard]] constexpr bool isTupleValue(Tuple<Ts...> const &) noexcept {
  return true;
}

//-----------------------------------------------------------------------------
// Returns true if t is a homogeneous tuple; otherwise false.
template <typename Tuple>
[[nodiscard]] constexpr bool isHomogeneousTuple(Tuple const &t) noexcept {

  if constexpr (isTupleType<Tuple>) {
    return Tuple::isHomogenous();
  } else {
    return false;
  }
}

//-----------------------------------------------------------------------------
// Returns a tuple of type t with each component set to max of the type in the
// corresponding component of the argument.
template <typename... Ts> auto(max)(Tuple<Ts...> const &) {

  return Tuple((std::numeric_limits<Ts>::max)()...);
}

// Returns a tuple of type t with each component set to min of the type in the
// corresponding component of the argument.
template <typename... Ts> auto(min)(Tuple<Ts...> const &) {

  return Tuple((std::numeric_limits<Ts>::min)()...);
}

//-----------------------------------------------------------------------------
// The unary operators +, -, ~, and ! are overloaded on tuples by applying the
// operator to each argument component and returning the results as a new tuple.
namespace detail {

template <typename... Ts, typename Op, std::size_t... Is>
constexpr decltype(auto) unaryOperator(Tuple<Ts...> const &t, Op &&op,
                                       std::index_sequence<Is...>) {

  return Tuple(op(std::get<Is>(t))...);
};
} // namespace detail

template <typename... Ts>
constexpr decltype(auto) operator+(Tuple<Ts...> const &t) {

  return t;
}

template <typename... Ts>
constexpr decltype(auto) operator-(Tuple<Ts...> const &t) {

  return detail::unaryOperator(t, std::negate<>{},
                               std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename... Ts>
constexpr decltype(auto) operator~(Tuple<Ts...> const &t) {

  return detail::unaryOperator(t, std::bit_not<>{},
                               std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename... Ts>
constexpr decltype(auto) operator!(Tuple<Ts...> const &t) {

  return detail::unaryOperator(t, std::logical_not<>{},
                               std::make_index_sequence<sizeof...(Ts)>{});
}

//-----------------------------------------------------------------------------
// The binary operators +, -, *, /, %, **, &, |, ^, <<, and >> are overloaded on
// tuples by applying them to pairs of the respective argument components and
// returning the results as a new tuple. The sizes of the two argument tuples
// must be the same. These operators are also defined for homogeneous tuples and
// scalar values of matching type.
namespace detail {

template <typename... Ts1, typename... Ts2, typename Op, std::size_t... Is>
constexpr decltype(auto) binaryOperator(Tuple<Ts1...> const &t1,
                                        Tuple<Ts2...> const &t2, Op &&op,
                                        std::index_sequence<Is...>) {

  static_assert(sizeof...(Ts1) == sizeof...(Ts2),
                "the size of the tuple arguments must be the same");
  return Tuple(op(std::get<Is>(t1), std::get<Is>(t2))...);
};

template <typename T = void> struct shift_left {
  constexpr decltype(auto) operator()(T const &t1, T const &t2) const {
    return t1 << t2;
  }
};

template <> struct shift_left<> {
  using is_transparent = void;

  template <typename T1, typename T2>
  constexpr decltype(auto) operator()(T1 const &t1, T2 const &t2) const {
    return t1 << t2;
  }
};

template <typename T = void> struct shift_right {
  constexpr decltype(auto) operator()(T const &t1, T const &t2) const {
    return t1 >> t2;
  }
};

template <> struct shift_right<> {
  using is_transparent = void;

  template <typename T1, typename T2>
  constexpr decltype(auto) operator()(T1 const &t1, T2 const &t2) const {
    return t1 >> t2;
  }
};
} // namespace detail

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator+(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::plus<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator-(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::minus<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator*(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::multiplies<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator/(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::divides<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator%(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::modulus<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator&(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::bit_and<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator|(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::bit_or<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator^(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::bit_xor<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator<<(Tuple<Ts1...> const &t1,
                                    Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, detail::shift_left<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
constexpr decltype(auto) operator>>(Tuple<Ts1...> const &t1,
                                    Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, detail::shift_right<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

//-----------------------------------------------------------------------------
// The relational operators >, >=, <, <=, ==, and != are defined over tuples of
// matching size. They return a single boolean value indicating whether the two
// arguments satisfy the corresponding relation.
template <typename... Ts1, typename... Ts2>
[[nodiscard]] constexpr auto operator<=>(Tuple<Ts1...> const &t1,
                                         Tuple<Ts2...> const &t2) {

  return t1.base() <=> t2.base();
}

} // namespace chplx
