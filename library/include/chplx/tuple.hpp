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
#include <ostream>
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

  // index operator
  [[nodiscard]] decltype(auto) operator[](std::size_t i)
    requires(isHomogenous())
  {
    return HomogenousTupleRange(base())[i];
  }
  [[nodiscard]] decltype(auto) operator[](std::size_t i) const
    requires(isHomogenous())
  {
    return HomogenousTupleRange(base())[i];
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
    return HomogenousTupleRange(base())[i];
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
constexpr decltype(auto) unaryOperator(Tuple<Ts...> &&t, Op &&op,
                                       std::index_sequence<Is...>) {

  return Tuple(op(std::get<Is>(std::move(t)))...);
};

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

template <typename T1, typename T2, typename Op>
constexpr decltype(auto) binaryOperator(Tuple<T1> const &t1,
                                        Tuple<T2> const &t2, Op &&op,
                                        std::index_sequence<0>) {
  return op(std::get<0>(t1), std::get<0>(t2));
};

//-----------------------------------------------------------------------------
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
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator+(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::plus<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator-(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::minus<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator*(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::multiplies<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator/(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::divides<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator%(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::modulus<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator&(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::bit_and<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator|(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::bit_or<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator^(Tuple<Ts1...> const &t1,
                                   Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, std::bit_xor<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) operator<<(Tuple<Ts1...> const &t1,
                                    Tuple<Ts2...> const &t2) {

  return detail::binaryOperator(t1, t2, detail::shift_left<>{},
                                std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts1, typename... Ts2>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
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
  requires(sizeof...(Ts1) == sizeof...(Ts2))
[[nodiscard]] constexpr auto operator<=>(Tuple<Ts1...> const &t1,
                                         Tuple<Ts2...> const &t2) {

  return t1.base() <=> t2.base();
}

//-----------------------------------------------------------------------------
// General purpose lifting
namespace detail {

template <typename... Ts, typename F>
constexpr decltype(auto) lift(Tuple<Ts...> &&t, F &&f) {
  return unaryOperator(std::move(t), std::forward<F>(f),
                       std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename... Ts1, typename... Ts2, typename F>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto) lift(Tuple<Ts1...> &&t1, Tuple<Ts2...> &&t2, F &&f) {
  return binaryOperator(std::move(t1), std::move(t2), std::forward<F>(f),
                        std::make_index_sequence<sizeof...(Ts1)>{});
}

template <typename... Ts, typename F>
constexpr decltype(auto) lift(Tuple<Ts...> const &t, F &&f) {
  return unaryOperator(t, std::forward<F>(f),
                       std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename... Ts1, typename... Ts2, typename F>
  requires(sizeof...(Ts1) == sizeof...(Ts2))
constexpr decltype(auto)
    lift(Tuple<Ts1...> const &t1, Tuple<Ts2...> const &t2, F &&f) {
  return binaryOperator(t1, t2, std::forward<F>(f),
                        std::make_index_sequence<sizeof...(Ts1)>{});
}

//-----------------------------------------------------------------------------
template <typename... Ts, typename Op, typename T, std::size_t... Is>
constexpr decltype(auto) reduce(Tuple<Ts...> const &t, Op &&op, T init,
                                std::index_sequence<Is...>) {

  T result = init;
  auto f = [&](auto next) { result = op(result, static_cast<T>(next)); };

  (f(std::get<Is>(t)), ...);

  return result;
}
} // namespace detail

//-----------------------------------------------------------------------------
// Reduction over tuple elements
template <typename... Ts, typename F, typename T>
constexpr decltype(auto) reduce(Tuple<Ts...> const &t, F &&f, T init) {

  return detail::reduce(t, std::forward<F>(f), init,
                        std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename T1, typename F, typename T2>
  requires(!isTupleType<T1>)
constexpr decltype(auto) reduce(T1 const &t, F &&f, T2 init) {

  return f(init, static_cast<T2>(t));
}

//-----------------------------------------------------------------------------
namespace detail {

template <typename Tuple, std::size_t... Is>
std::ostream &writeTuple(std::ostream &os, Tuple const &t,
                         std::index_sequence<Is...>) {

  auto p = [&](auto val) { os << "," << val; };
  (p(std::get<Is + 1>(t)), ...);

  return os;
}
} // namespace detail

template <typename... Ts>
std::ostream &operator<<(std::ostream &os, Tuple<Ts...> const &t) {

  os << "(";
  if constexpr (sizeof...(Ts) != 0) {
    os << std::get<0>(t.base());
    if constexpr (sizeof...(Ts) > 1) {
      detail::writeTuple(os, t.base(),
                         std::make_index_sequence<sizeof...(Ts) - 1>());
    }
  }
  os << ")";
  return os;
}

} // namespace chplx
