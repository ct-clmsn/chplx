//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/dmap.hpp>
#include <chplx/locale.hpp>
#include <chplx/tuple.hpp>
#include <chplx/types.hpp>

#include <chplx/domains/base_rectangular.hpp>

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace chplx::domains {

//-----------------------------------------------------------------------------
struct DefaultDist;

namespace detail {
//-----------------------------------------------------------------------------
template <int Ord> struct IndexOrder {

  template <typename Ranges, typename IndexType>
  static constexpr std::int64_t call(Ranges const &rs, IndexType idx) noexcept {
    auto const &r = std::get<Ord>(rs);
    std::int64_t order = IndexOrder<Ord - 1>::call(rs, idx);
    return order * r.size() + r.indexOrder(std::get<Ord>(idx));
  }
};

template <> struct IndexOrder<0> {

  template <typename Ranges, typename IndexType>
  static constexpr std::int64_t call(Ranges const &rs, IndexType idx) noexcept {
    return std::get<0>(rs).indexOrder(std::get<0>(idx));
  }
};

//-----------------------------------------------------------------------------
template <int Ord> struct OrderToIndex {

  template <typename... Ts, typename T0, std::size_t... Is>
  static constexpr auto flatten(T0 idxN, Tuple<Ts...> const &idx,
                                std::index_sequence<Is...>) noexcept {

    return Tuple<T0, std::decay_t<Ts>...>(idxN, std::get<Is>(idx)...);
  };

  template <int Rank, typename Ranges>
  static constexpr std::size_t size(Ranges const &rs) noexcept {

    return std::get<Rank>(rs).size() *
           OrderToIndex<Ord - 1>::template size<Rank - 1>(rs);
  }

  template <int Rank, typename Ranges>
  static constexpr auto call(Ranges const &rs, std::int64_t order) noexcept {

    constexpr int N = Ranges::size() - Ord - 1;

    auto size = OrderToIndex::size<Rank>(rs);
    auto this_order = order / size;
    auto idx0 = std::get<N>(rs).orderToIndex(this_order);
    auto idx = OrderToIndex<Ord - 1>::template call<Rank>(
        rs, order - this_order * size);

    return flatten(idx0, idx, std::make_index_sequence<Ord>());
  }
};

template <> struct OrderToIndex<0> {

  template <int, typename Ranges>
  static constexpr std::size_t size(Ranges const &rs) noexcept {

    return 1;
  }

  template <int, typename Ranges>
  static constexpr auto call(Ranges const &rs, std::int64_t order) noexcept {

    constexpr int N = Ranges::size() - 1;

    auto const &r = std::get<N>(rs);
    return Tuple(r.orderToIndex(order % r.size()));
  }
};
} // namespace detail

//-----------------------------------------------------------------------------
template <int Rank, typename IndexType, bool Stridable>
struct DefaultRectangularDomain final
    : BaseRectangularDomain<Rank, IndexType, Stridable> {

private:
  using baseType = BaseRectangularDomain<Rank, IndexType, Stridable>;

public:
  static constexpr int rank = Rank;
  using indexType = typename baseType::indexType;
  using rangesType = typename baseType::rangesType;
  using rangeType = typename baseType::rangeType;

  explicit DefaultRectangularDomain(
      rangesType ranges, hpx::intrusive_ptr<DefaultDist> &&dist) noexcept
      : ranges(ranges), dist(std::move(dist)) {}

  //---------------------------------------------------------------------------
  [[nodiscard]] rangesType &dsiDims() noexcept override { return ranges; }
  [[nodiscard]] rangesType const &dsiDims() const noexcept override {
    return ranges;
  }
  [[nodiscard]] rangeType dsiDim(int i) const noexcept override {
    return ranges[i];
  }

  [[nodiscard]] rangesType dsiGetIndices() const noexcept override {
    return ranges;
  }
  void dsiSetIndices(rangesType r) noexcept override { ranges = r; }

  [[nodiscard]] bool dsiMember(indexType idx) const noexcept override {
    return chplx::reduce(
        chplx::detail::lift(ranges, idx,
                            [](auto &&r, auto i) { return r.contains(i); }),
        std::logical_and<>{}, true);
  }

  [[nodiscard]] IndexType dsiIndexOrder(indexType idx) const noexcept override {
    return detail::IndexOrder<Rank - 1>::call(ranges, idx);
  }
  [[nodiscard]] constexpr indexType
  dsiOrderToIndex(IndexType idx) const noexcept override {
    return detail::OrderToIndex<Rank - 1>::template call<Rank - 1>(ranges, idx);
  }
  [[nodiscard]] indexType dsiLow() const noexcept override {
    return chplx::detail::lift(ranges, [](auto &&r) { return r.lowBound(); });
  }
  [[nodiscard]] indexType dsiHigh() const noexcept override {
    return chplx::detail::lift(ranges, [](auto &&r) { return r.highBound(); });
  }
  [[nodiscard]] indexType dsiAlignedLow() const noexcept override {
    return chplx::detail::lift(ranges, [](auto &&r) { return r.low(); });
  }
  [[nodiscard]] indexType dsiAlignedHigh() const noexcept override {
    return chplx::detail::lift(ranges, [](auto &&r) { return r.high(); });
  }
  [[nodiscard]] indexType dsiFirst() const noexcept override {
    return chplx::detail::lift(ranges, [](auto &&r) { return r.first(); });
  }
  [[nodiscard]] indexType dsiLast() const noexcept override {
    return chplx::detail::lift(ranges, [](auto &&r) { return r.last(); });
  }
  [[nodiscard]] indexType dsiStride() const noexcept override {
    return chplx::detail::lift(ranges, [](auto &&r) { return r.stride(); });
  }
  [[nodiscard]] indexType dsiAlignment() const noexcept override {
    return chplx::detail::lift(ranges, [](auto &&r) { return r.alignment(); });
  }

  [[nodiscard]] IndexType dsiNumIndices() const noexcept override {
    return chplx::reduce(
        chplx::detail::lift(ranges, [](auto &&r) { return r.size(); }),
        std::multiplies<>(), 1);
  }

  //---------------------------------------------------------------------------
  [[nodiscard]] auto dsiMyDist() const noexcept { return dist; }

private:
  rangesType ranges{};
  hpx::intrusive_ptr<DefaultDist> dist{};
};

//-----------------------------------------------------------------------------
struct DefaultDist : GlobalDistribution<DefaultDist> {

  template <int Rank, typename IndexType, bool Stridable, typename Indices>
  [[nodiscard]] auto *dsiNewRectangularDom(Indices &&inds) {
    return new DefaultRectangularDomain<Rank, IndexType, Stridable>(
        std::forward<Indices>(inds), this);
  }

  // Right now, the default distribution acts like a singleton. So we don't
  // have to copy it when a clone is requested.
  [[nodiscard]] auto dsiClone() { return this; }

  template <typename IndexType>
  constexpr chplx::locale dsiIndexToLocale(IndexType) const noexcept {
    return locale;
  }

  void dsiAssign(DefaultDist const &d) noexcept { locale = d.locale; }

  template <typename Dist>
  [[nodiscard]] static constexpr bool dsiEqualDMaps(Dist const &) noexcept {
    return std::is_same_v<Dist, DefaultDist>;
  }

  [[nodiscard]] static constexpr bool dsiIsLayout() noexcept { return true; }

  [[nodiscard]] static Array<chplx::locale, Domain<1>> const &
  dsiTargetLocales(chplx::locale here) {
    return singleLocaleSingleton(here);
  }

  chplx::locale locale = here;
};

} // namespace chplx::domains

namespace chplx {

// If a dmapped clause starts with the name of a domain map class, it is
// considered to be an initialization expression as if preceded by new. The
// resulting domain map instance is wrapped in a newly-created instance of
// dmap implicitly.
template <int Rank, typename IndexType, bool Stridable>
struct dmapped_t<Domain<Rank, IndexType, Stridable>, domains::DefaultDist> {

  template <typename... Ts> constexpr auto operator()(Ts &&...ts) const {
    return dmap<domains::DefaultDist, Rank, IndexType, Stridable>(
        new domains::DefaultDist(std::forward<Ts>(ts)...));
  }
};

//-----------------------------------------------------------------------------
template <typename Domain> auto const &defaultDist() {
  static auto dist = dmapped<Domain, domains::DefaultDist>();
  return dist;
}

} // namespace chplx
