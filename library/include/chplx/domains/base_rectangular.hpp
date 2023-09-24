//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/dmap.hpp>
#include <chplx/tuple.hpp>
#include <chplx/types.hpp>

#include <hpx/assert.hpp>
#include <hpx/modules/memory.hpp>

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace chplx::domains {

namespace detail {

template <typename T> std::size_t size(std::vector<std::vector<T>> const &d) {

  if (d.empty()) {
    return 0;
  }
  return d.size() * d[0].size();
}

template <typename T> auto shape(std::vector<std::vector<T>> const &d) {

  if (d.empty()) {
    return chplx::Tuple(chplx::BoundedRange<std::int64_t>(),
                        chplx::BoundedRange<std::int64_t>());
  }

  return chplx::Tuple(chplx::BoundedRange<std::int64_t>(0, d.size() - 1),
                      chplx::BoundedRange<std::int64_t>(0, d[0].size() - 1));
}

template <typename T>
std::size_t size(std::vector<std::vector<std::vector<T>>> const &d) {

  if (d.empty() || d[0].empty()) {
    return 0;
  }
  return d[0][0].size() * d[0].size() * d.size();
}

template <typename T>
auto shape(std::vector<std::vector<std::vector<T>>> const &d) {

  if (d.empty() || d[0].empty()) {
    return chplx::Tuple(chplx::BoundedRange<std::int64_t>(),
                        chplx::BoundedRange<std::int64_t>(),
                        chplx::BoundedRange<std::int64_t>());
  }

  return chplx::Tuple(chplx::BoundedRange<std::int64_t>(0, d.size() - 1),
                      chplx::BoundedRange<std::int64_t>(0, d[0].size() - 1),
                      chplx::BoundedRange<std::int64_t>(0, d[0][0].size() - 1));
}

template <typename Domain, typename T>
bool shapesMatch(hpx::intrusive_ptr<Domain> const &dom,
                 std::vector<std::vector<T>> const &d) {

  Tuple dims =
      chplx::detail::lift(dom->dsiDims(), [](auto &&r) { return r.size(); });

  if (dims.size() != 2) {
    return false;
  }
  if (d.empty()) {
    return dims[0] == 0;
  }

  return dims == Tuple(d.size(), d[0].size());
}

template <typename Domain, typename T>
bool shapesMatch(hpx::intrusive_ptr<Domain> const &dom,
                 std::vector<std::vector<std::vector<T>>> const &d) {

  Tuple dims =
      chplx::detail::lift(dom->dsiDims(), [](auto &&r) { return r.size(); });

  if (dims.size() != 3) {
    return false;
  }
  if (d.empty()) {
    return dims[0] == 0;
  }
  if (d[0].empty()) {
    return dims[1] == 0;
  }

  return dims == Tuple(d.size(), d[0].size(), d[0][0].size());
}

template <typename Domain, typename T>
std::vector<T> flatten(hpx::intrusive_ptr<Domain> const &dom,
                       std::vector<std::vector<T>> &&d) {

  HPX_ASSERT_MSG(
      shapesMatch(dom, d),
      "The shapes of the initializer and the target array must match");

  std::vector<T> result;
  result.reserve(detail::size(d));

  for (auto &&vec : d) {
    for (auto &&v : vec) {
      result.push_back(std::move(v));
    }
  }

  return result;
}

template <typename Domain, typename T>
std::vector<T> flatten(hpx::intrusive_ptr<Domain> const &dom,
                       std::vector<std::vector<std::vector<T>>> &&d) {

  HPX_ASSERT_MSG(
      shapesMatch(dom, d),
      "The shapes of the initializer and the target array must match");

  std::vector<T> result;
  result.reserve(detail::size(d));

  for (auto &&v0 : d) {
    for (auto &&v1 : v0) {
      for (auto &&v : v1) {
        result.push_back(std::move(v));
      }
    }
  }

  return result;
}

template <typename T>
std::size_t size(std::initializer_list<std::initializer_list<T>> const &il) {

  if (il.size() == 0) {
    return 0;
  }
  return il.begin()->size() * il.size();
}

template <typename T>
auto shape(std::initializer_list<std::initializer_list<T>> const &il) {

  if (il.size() == 0) {
    return chplx::Tuple(chplx::BoundedRange<std::int64_t>(),
                        chplx::BoundedRange<std::int64_t>());
  }

  return chplx::Tuple(
      chplx::BoundedRange<std::int64_t>(0, il.size() - 1),
      chplx::BoundedRange<std::int64_t>(0, il.begin()->size() - 1));
}

template <typename T>
std::size_t size(
    std::initializer_list<std::initializer_list<std::initializer_list<T>>> const
        &il) {

  if (il.size() == 0 || il.begin()->size() == 0) {
    return 0;
  }
  return il.begin()->begin()->size() * il.begin()->size() * il.size();
}

template <typename T>
auto shape(
    std::initializer_list<std::initializer_list<std::initializer_list<T>>> const
        &il) {

  if (il.size() == 0 || il.begin()->size() == 0) {
    return chplx::Tuple(chplx::BoundedRange<std::int64_t>(),
                        chplx::BoundedRange<std::int64_t>(),
                        chplx::BoundedRange<std::int64_t>());
  }

  return chplx::Tuple(
      chplx::BoundedRange<std::int64_t>(0, il.size() - 1),
      chplx::BoundedRange<std::int64_t>(0, il.begin()->size() - 1),
      chplx::BoundedRange<std::int64_t>(0, il.begin()->begin()->size() - 1));
}

template <typename Domain, typename T>
bool shapesMatch(hpx::intrusive_ptr<Domain> const &dom,
                 std::initializer_list<std::initializer_list<T>> const &il) {

  Tuple dims =
      chplx::detail::lift(dom->dsiDims(), [](auto &&r) { return r.size(); });

  if (dims.size() != 2) {
    return false;
  }
  if (il.size() == 0) {
    return dims[0] == 0;
  }

  return dims == Tuple(il.size(), il.begin()->size());
}

template <typename Domain, typename T>
bool shapesMatch(
    hpx::intrusive_ptr<Domain> const &dom,
    std::initializer_list<std::initializer_list<std::initializer_list<T>>> const
        &il) {

  Tuple dims =
      chplx::detail::lift(dom->dsiDims(), [](auto &&r) { return r.size(); });

  if (dims.size() != 3) {
    return false;
  }
  if (il.size() == 0) {
    return dims[0] == 0;
  }
  if (il.begin()->size() == 0) {
    return dims[1] == 0;
  }

  return dims ==
         Tuple(il.size(), il.begin()->size(), il.begin()->begin()->size());
}

template <typename Domain, typename T>
std::vector<T> flatten(hpx::intrusive_ptr<Domain> const &dom,
                       std::initializer_list<std::initializer_list<T>> &&il) {

  HPX_ASSERT_MSG(
      detail::shapesMatch(dom, il),
      "The shapes of the initializer and the target array must match");

  std::vector<T> result;
  result.reserve(detail::size(il));

  for (auto &&il0 : il) {
    for (auto &&v : il0) {
      result.push_back(std::move(v));
    }
  }

  return result;
}

template <typename Domain, typename T>
std::vector<T>
flatten(hpx::intrusive_ptr<Domain> const &dom,
        std::initializer_list<std::initializer_list<std::initializer_list<T>>>
            &&il) {

  HPX_ASSERT_MSG(
      detail::shapesMatch(dom, il),
      "The shapes of the initializer and the target array must match");

  std::vector<T> result;
  result.reserve(detail::size(il));

  for (auto &&il0 : il) {
    for (auto &&il1 : il0) {
      for (auto &&v : il1) {
        result.push_back(std::move(v));
      }
    }
  }

  return result;
}
} // namespace detail

template <int Rank, typename IndexType, bool Stridable>
struct BaseRectangularDomain;

//-----------------------------------------------------------------------------
template <typename T, int Rank, typename IndexType, bool Stridable>
struct BaseRectangularArray : BaseArray {

  using defaultDomain = BaseRectangularDomain<Rank, IndexType, Stridable>;

  explicit BaseRectangularArray(defaultDomain *dom)
      : domain(dom), data(domain->dsiNumIndices()) {}
  BaseRectangularArray(defaultDomain *dom, T init)
      : domain(dom), data(domain->dsiNumIndices(), std::move(init)) {}

  BaseRectangularArray(defaultDomain *dom, std::vector<T> &&d)
    requires(Rank == 1)
      : domain(dom), data(std::move(d)) {
    HPX_ASSERT_MSG(domain->dsiNumIndices() == data.size(),
                   "Data used for initialization must have same size as "
                   "underlying domain");
  }
  BaseRectangularArray(defaultDomain *dom, std::initializer_list<T> &&d)
    requires(Rank == 1)
      : domain(dom), data(std::move(d)) {
    HPX_ASSERT_MSG(domain->dsiNumIndices() == data.size(),
                   "Initializer list must have same size as underlying domain");
  }

  BaseRectangularArray(defaultDomain *dom, std::vector<std::vector<T>> &&d)
    requires(Rank == 2)
      : domain(dom), data(detail::flatten(domain, std::move(d))) {
    HPX_ASSERT_MSG(domain->dsiNumIndices() == data.size(),
                   "Data used for initialization must have same size as "
                   "underlying domain");
  }
  BaseRectangularArray(defaultDomain *dom,
                       std::initializer_list<std::initializer_list<T>> &&il)
    requires(Rank == 2)
      : domain(dom), data(detail::flatten(domain, std::move(il))) {
    HPX_ASSERT_MSG(domain->dsiNumIndices() == data.size(),
                   "Initializer list must have same size as underlying domain");
  }

  BaseRectangularArray(defaultDomain *dom,
                       std::vector<std::vector<std::vector<T>>> &&d)
    requires(Rank == 3)
      : domain(dom), data(detail::flatten(domain, std::move(d))) {
    HPX_ASSERT_MSG(domain->dsiNumIndices() == data.size(),
                   "Data used for initialization must have same size as "
                   "underlying domain");
  }
  BaseRectangularArray(
      defaultDomain *dom,
      std::initializer_list<std::initializer_list<std::initializer_list<T>>>
          &&il)
    requires(Rank == 3)
      : domain(dom), data(detail::flatten(domain, std::move(il))) {
    HPX_ASSERT_MSG(domain->dsiNumIndices() == data.size(),
                   "Initializer list must have same size as underlying domain");
  }

  auto dsiGetBaseDomain() const { return domain; }

  constexpr inline T &dsiAccess(IndexType idx) { return data[idx]; }
  constexpr inline T const &dsiAccess(IndexType idx) const { return data[idx]; }

  hpx::intrusive_ptr<defaultDomain> const domain{};
  std::vector<T> data{};
};

//-----------------------------------------------------------------------------
template <int Rank, typename IndexType, bool Stridable>
struct BaseRectangularDomain
    : GlobalDomain<BaseRectangularDomain<Rank, IndexType, Stridable>> {

  using rangeType = Range<IndexType, BoundedRangeType::bounded, Stridable>;
  using rangesType = chplx::detail::generate_tuple_type_t<Rank, rangeType>;
  using indexType = chplx::detail::generate_tuple_type_t<Rank, IndexType>;

  virtual ~BaseRectangularDomain() = default;

  virtual void dsiAdd(IndexType) {
    HPX_ASSERT_MSG(false, "Cannot add indices from a rectangular domain");
  }
  virtual void dsiRemove(IndexType) {
    HPX_ASSERT_MSG(false, "Cannot remove indices from a rectangular domain");
  }
  virtual void dsiClear() {
    HPX_ASSERT_MSG(false, "Cannot clear indices from a rectangular domain");
  }

  [[nodiscard]] virtual rangesType &dsiDims() noexcept = 0;
  [[nodiscard]] virtual rangesType const &dsiDims() const noexcept = 0;
  [[nodiscard]] virtual rangeType dsiDim(int i) const noexcept = 0;

  [[nodiscard]] virtual rangesType dsiGetIndices() const noexcept = 0;
  virtual void dsiSetIndices(rangesType r) noexcept = 0;

  [[nodiscard]] virtual bool dsiMember(indexType idx) const noexcept = 0;

  [[nodiscard]] virtual IndexType
  dsiIndexOrder(indexType idx) const noexcept = 0;
  [[nodiscard]] virtual indexType
  dsiOrderToIndex(IndexType idx) const noexcept = 0;

  [[nodiscard]] virtual indexType dsiLow() const noexcept = 0;
  [[nodiscard]] virtual indexType dsiHigh() const noexcept = 0;
  [[nodiscard]] virtual indexType dsiAlignedLow() const noexcept = 0;
  [[nodiscard]] virtual indexType dsiAlignedHigh() const noexcept = 0;
  [[nodiscard]] virtual indexType dsiFirst() const noexcept = 0;
  [[nodiscard]] virtual indexType dsiLast() const noexcept = 0;
  [[nodiscard]] virtual indexType dsiStride() const noexcept = 0;
  [[nodiscard]] virtual indexType dsiAlignment() const noexcept = 0;

  [[nodiscard]] virtual IndexType dsiNumIndices() const noexcept = 0;

  template <typename T, typename... Ts>
  BaseRectangularArray<T, Rank, IndexType, Stridable> *
  dsiBuildArray(Ts &&...ts) {
    return new BaseRectangularArray<T, Rank, IndexType, Stridable>{
        this, std::forward<Ts>(ts)...};
  }
};

} // namespace chplx::domains
