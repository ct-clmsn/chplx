//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/tuple.hpp>
#include <chplx/types.hpp>

#include <hpx/generator.hpp>
#include <hpx/modules/datastructures.hpp>
#include <hpx/modules/iterator_support.hpp>
#include <hpx/modules/type_support.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace hpx::util {

namespace detail {

template <typename... Ts, std::size_t... Is>
constexpr hpx::tuple<Ts...> toHpxTuple(chplx::Tuple<Ts...> &&t,
                                       std::index_sequence<Is...>) noexcept {

  return hpx::tuple<Ts...>(std::get<Is>(std::move(t))...);
}

template <typename... Ts>
constexpr hpx::tuple<Ts...> toHpxTuple(chplx::Tuple<Ts...> &&t) noexcept {

  return toHpxTuple(std::move(t), std::make_index_sequence<sizeof...(Ts)>());
}

template <typename... Ts, std::size_t... Is>
constexpr chplx::Tuple<Ts...> toTuple(hpx::tuple<Ts...> &&t,
                                      std::index_sequence<Is...>) noexcept {

  return chplx::Tuple<Ts...>(hpx::get<Is>(std::move(t))...);
}

template <typename... Ts>
constexpr chplx::Tuple<Ts...> toTuple(hpx::tuple<Ts...> &&t) noexcept {

  return toTuple(std::move(t), std::make_index_sequence<sizeof...(Ts)>());
}

template <typename... Ts>
using ToTuple_t = decltype(toTuple(std::declval<hpx::tuple<Ts...>>()));

} // namespace detail

//-----------------------------------------------------------------------------
// Add a zip_iterator specialization that is initialized from a chplx::Tuple
template <typename... Ts>
class zip_iterator<chplx::Tuple<Ts...>>
    : public hpx::util::detail::zip_iterator_base<
          hpx::tuple<Ts...>, zip_iterator<chplx::Tuple<Ts...>>> {

  static_assert(sizeof...(Ts) != 0,
                "zip_iterator must wrap at least one iterator");

  using base_type =
      hpx::util::detail::zip_iterator_base<hpx::tuple<Ts...>,
                                           zip_iterator<chplx::Tuple<Ts...>>>;

  friend class hpx::util::iterator_core_access;

  // special dereference to return a chplx::Tuple
  constexpr decltype(auto) dereference() const {

    return detail::toTuple(base_type::dereference());
  }

public:
  using reference = detail::ToTuple_t<typename base_type::reference>;

  constexpr zip_iterator() noexcept : base_type() {}

  explicit constexpr zip_iterator(Ts const &...ts) noexcept
      : base_type(hpx::tie(ts...)) {}

  explicit constexpr zip_iterator(chplx::Tuple<Ts...> &&t) noexcept
      : base_type(detail::toHpxTuple(std::move(t))) {}

  constexpr zip_iterator(zip_iterator const &other) = default;
  constexpr zip_iterator(zip_iterator &&other) noexcept = default;

  zip_iterator &operator=(zip_iterator const &other) = default;
  zip_iterator &operator=(zip_iterator &&other) noexcept = default;

  template <typename... Ts_>
    requires(std::is_assignable_v<
             typename zip_iterator::iterator_tuple_type &,
             typename zip_iterator<Ts_...>::iterator_tuple_type &&>)
  zip_iterator &operator=(zip_iterator<Ts_...> const &other) {
    base_type::operator=(base_type(other.get_iterator_tuple()));
    return *this;
  }

  template <typename... Ts_>
    requires(std::is_assignable_v<
             typename zip_iterator::iterator_tuple_type &,
             typename zip_iterator<Ts_...>::iterator_tuple_type &&>)
  zip_iterator &operator=(zip_iterator<Ts_...> &&other) noexcept {
    base_type::operator=(base_type(HPX_MOVE(other).get_iterator_tuple()));
    return *this;
  }
};

template <typename... Ts>
zip_iterator(chplx::Tuple<Ts...> &&) -> zip_iterator<chplx::Tuple<Ts...>>;

} // namespace hpx::util

namespace chplx {

namespace detail {

//-----------------------------------------------------------------------------
template <typename... Rs> struct ZipRange : Tuple<Rs...> {

  using base_type = Tuple<Rs...>;

  using idxType = std::common_type_t<typename std::decay_t<Rs>::idxType...>;
  using indexType = Tuple<typename std::decay_t<Rs>::indexType...>;

  base_type &base() { return static_cast<base_type &>(*this); }
  base_type const &base() const {
    return static_cast<base_type const &>(*this);
  }

  template <typename... Rs_>
    requires(sizeof...(Rs) == sizeof...(Rs_))
  constexpr ZipRange(Rs_ &&...rs) : base_type(std::forward<Rs_>(rs)...) {}

  // The zip operation is iterable only if all zippered objects are iterable
  [[nodiscard]] constexpr bool isIterable() const noexcept {
    return reduce(lift(base(), [](auto &&r) { return r.isIterable(); }),
                  std::logical_and<>(), true);
  }

  // The zip operation is bounded if at least one of the zippered objects is
  // bounded.
  [[nodiscard]] constexpr bool isBounded() const noexcept {
    return reduce(lift(base(), [](auto &&r) { return r.isBounded(); }),
                  std::logical_or<>(), false);
  }

  // Compute the size of the zippered object. It is the size of the shortest
  // iteration sequence exposed by all of the zippered objects.
  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return reduce(
        lift(base(),
             [](auto &&r) {
               return r.isBounded() ? r.size() : MaxValue_v<std::size_t>;
             }),
        [](std::size_t init, std::size_t val) { return (std::min)(init, val); },
        MaxValue_v<std::size_t>);
  }

  // Return a Tuple of all index values corresponding to the given sequence
  // order.
  [[nodiscard]] constexpr indexType
  orderToIndex(std::size_t order) const noexcept {
    return lift(base(), [order](auto &&r) { return r.orderToIndex(order); });
  }

  // Yield the zipped indices
  [[nodiscard]] decltype(auto) these() const { return iterate(*this); }

  constexpr auto operator[](std::size_t idx) noexcept {
    return lift(base(), [idx](auto &&r) -> decltype(auto) { return r[idx]; });
  }
  constexpr auto operator[](std::size_t idx) const noexcept {
    return lift(base(), [idx](auto &&r) -> decltype(auto) { return r[idx]; });
  }

  using resultType = decltype(std::declval<ZipRange>()[0]);
};

template <typename... Rs>
hpx::generator<typename ZipRange<Rs...>::resultType>
iterate(IteratorGenerator<ZipRange<Rs...>> zr) noexcept {

  HPX_ASSERT(zr.target.isIterable());

  auto size = zr.size;
  for (auto ilo = zr.first; size-- != 0; ++ilo) {
    co_yield zr.target[ilo];
  }
}

//-----------------------------------------------------------------------------
template <typename... Rs>
decltype(auto) iterate(ZipRange<Rs...> const &zr) noexcept {

  return iterate(detail::IteratorGenerator(zr));
}

} // namespace detail

//-----------------------------------------------------------------------------
template <typename R, typename... Rs>
constexpr decltype(auto) zip(R &&r, Rs &&...rs) {

  HPX_ASSERT_MSG(r.isIterable() && (rs.isIterable() && ...),
                 "all zippered objects need to be iterable");

  return detail::ZipRange<R, Rs...>(std::forward<R>(r),
                                    std::forward<Rs>(rs)...);
}
} // namespace chplx
