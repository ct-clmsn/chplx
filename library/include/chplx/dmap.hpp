//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/assert.hpp>
#include <hpx/modules/memory.hpp>
#include <hpx/modules/thread_support.hpp>

#include <chplx/locale.hpp>
#include <chplx/range.hpp>
#include <chplx/types.hpp>

#include <cstddef>
#include <memory>

namespace chplx {

//-----------------------------------------------------------------------------
struct BaseDist;

void intrusive_ptr_add_ref(BaseDist *p) noexcept;
void intrusive_ptr_release(BaseDist *p) noexcept;

struct BaseDist {

  BaseDist() = default;

  BaseDist(BaseDist const &) = delete;
  BaseDist(BaseDist &&) = delete;
  BaseDist &operator=(BaseDist const &) = delete;
  BaseDist &operator=(BaseDist &&) = delete;

  virtual ~BaseDist() = default;

private:
  friend void intrusive_ptr_add_ref(BaseDist *p) noexcept;
  friend void intrusive_ptr_release(BaseDist *p) noexcept;

  hpx::util::atomic_count count{0};
};

//-----------------------------------------------------------------------------
struct BaseDomain;

void intrusive_ptr_add_ref(BaseDomain *p) noexcept;
void intrusive_ptr_release(BaseDomain *p) noexcept;

struct BaseDomain {

  BaseDomain() = default;

  BaseDomain(BaseDomain const &) = delete;
  BaseDomain(BaseDomain &&) = delete;
  BaseDomain &operator=(BaseDomain const &) = delete;
  BaseDomain &operator=(BaseDomain &&) = delete;

  virtual ~BaseDomain() = default;

private:
  friend void intrusive_ptr_add_ref(BaseDomain *p) noexcept;
  friend void intrusive_ptr_release(BaseDomain *p) noexcept;

  hpx::util::atomic_count count{0};
};

//-----------------------------------------------------------------------------
struct BaseArray;

void intrusive_ptr_add_ref(BaseArray *p) noexcept;
void intrusive_ptr_release(BaseArray *p) noexcept;

struct BaseArray {

  BaseArray() = default;

  BaseArray(BaseArray const &) = delete;
  BaseArray(BaseArray &&) = delete;
  BaseArray &operator=(BaseArray const &) = delete;
  BaseArray &operator=(BaseArray &&) = delete;

  virtual ~BaseArray() = default;

private:
  friend void intrusive_ptr_add_ref(BaseArray *p) noexcept;
  friend void intrusive_ptr_release(BaseArray *p) noexcept;

  hpx::util::atomic_count count{0};
};

//-----------------------------------------------------------------------------
// GlobalDistribution defines the Domain map Standard Interface (DSI) This class
// is visible to the users of the domain map: the dmap wrapper in Chapel’s
// dmapped clauses wraps instances of this class. This class must be a subclass
// of BaseDist.
template <typename Derived> struct GlobalDistribution : BaseDist {

private:
  constexpr Derived &derived() noexcept {
    return static_cast<Derived &>(*this);
  }
  constexpr Derived const &derived() const noexcept {
    return static_cast<Derived const &>(*this);
  }

protected:
  template <typename IndexType>
  static constexpr chplx::locale dsiIndexToLocale(IndexType) noexcept {
    return here;
  }

  static hpx::intrusive_ptr<GlobalDistribution> dsiClone() noexcept {
    HPX_ASSERT_MSG(
        false, "The dsiClone() method must be supported by this distribution");
    return {};
  }

  static void dsiAssign(GlobalDistribution const &) noexcept {
    HPX_ASSERT_MSG(
        false, "The dsiAssign() method must be supported by this distribution");
  }

  template <typename Dist>
  static constexpr bool dsiEqualDMaps(Dist const &d) noexcept {
    HPX_ASSERT_MSG(
        false,
        "The dsiEqualDMaps() method must be supported by this distribution");
    return false;
  }

  [[nodiscard]] static constexpr bool dsiTrackDomains() noexcept {
    return false;
  }

  [[nodiscard]] static constexpr bool dsiIsLayout() noexcept { return false; }

  static constexpr void dsiDisplayRepresentation() noexcept {}

  template <int Rank, typename IndexType, bool Stridable, typename Indices>
  static auto dsiNewRectangularDom(Indices &&ind) {
    HPX_ASSERT_MSG(
        false, "Rectangular domains are not supported by this distribution");
  }

  template <typename IndexType, bool parSafe>
  static auto dsiNewAssociativeDom() {
    HPX_ASSERT_MSG(
        false, "associative domains are not supported by this distribution");
  }

  template <int Rank, typename IndexType, typename Domain>
  static auto dsiNewSparseDom(Domain &&dom) {
    HPX_ASSERT_MSG(false,
                   "sparse domains are not supported by this distribution");
  }

  [[nodiscard]] static Array<chplx::locale, Domain<1>> const &
  dsiTargetLocales(chplx::locale here) {
    HPX_ASSERT_MSG(
        false,
        "The targetLocales() method must be supported by this distribution");
    return singleLocaleSingleton(here);
  }

public:
  // Given an index idx, returns the locale that "owns" that index, i.e. on
  // which the corresponding data is located. This is used to implement
  // idxToLocale() on the dmap wrapper.
  template <typename IndexType> chplx::locale indexToLocale(IndexType idx) {
    return derived().dsiIndexToLocale(idx);
  }

  // Returns a duplicate of this.
  hpx::intrusive_ptr<GlobalDistribution> clone() const {
    return derived().dsiClone();
  }

  void assign(GlobalDistribution const &other) {
    return derived().dsiAssign(other);
  }

  bool trackDomains() { return derived().dsiTrackDomains(); }

  bool isLayout() { return derived().dsiIsLayout(); }

  // A debugging method. It implements displayRepresentation() on the dmap
  // wrapper.
  void displayRepresentation() const {
    return derived().dsiDisplayRepresentation();
  }

  // Return whether or not the two domain maps are "equal" (specify the same
  // distribution). This is invoked when == is applied to two domain maps.
  bool equalDMaps(GlobalDistribution const &rhs) {
    return derived().dsiEqualDMaps(rhs.derived());
  }

  // This method is invoked when the Chapel program is creating a domain value
  // of the type domain(rank, idxType, stridable) mapped using the domain map
  // this with initial indices 'inds'.
  //
  // This method returns a new GlobalDomain instance that will correspond to
  // that Chapel domain value, i.e., be that value’s runtime representation. The
  // field 'dist' of the returned GlobalDomain must point to this.
  template <int Rank, typename IndexType, bool Stridable, typename Indices>
  BaseDomain *newRectangularDom(Indices &&inds) {
    return derived().template dsiNewRectangularDom<Rank, IndexType, Stridable>(
        std::forward<Indices>(inds));
  }

  template <typename IndexType, bool ParSafe> auto newAssociativeDom() const {
    return derived().template dsiNewAssociativeDom<IndexType, ParSafe>();
  }

  template <int Rank, typename IndexType, typename Domain>
  auto newSparseDom(Domain &&dom) const {
    return derived().template dsiNewNewSparseDom<Rank, IndexType>(
        std::forward<Domain>(dom));
  }

  Array<chplx::locale, Domain<1>> const &
  targetLocales(chplx::locale here) const {
    return derived().dsiTargetLocales(here);
  }
};

// An instance of this class is created whenever a GlobalDistribution-mapped
// Chapel domain is created (e.g. when a domain variable is declared). There is
// a one-to-one correspondence at run time between a (conceptual) Chapel domain
// value and an instance of GlobalDomain. For presentation simplicity,
// GlobalDomain's properties refer to the properties of the corresponding Chapel
// domain value, when clear from the context.

template <typename Derived> struct GlobalDomain : BaseDomain {

private:
  constexpr Derived &derived() noexcept {
    return static_cast<Derived &>(*this);
  }
  constexpr Derived const &derived() const noexcept {
    return static_cast<Derived const &>(*this);
  }

protected:
  static constexpr auto dsiDim(int) noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiDim() method must be supported by this domain");
    return Range{1, 0};
  }
  static auto const &dsiDims() noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiDims() method must be supported by this domain");
    static auto r = Range{1, 0};
    return r;
  }

  static void dsiMyDist() noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiMyDist() method must be supported by this domain");
  }

  static void dsiGetIndices() noexcept {
    HPX_ASSERT_MSG(
        false, "The dsiGetIndices() method must be supported by this domain");
  }
  template <typename Indices> static void dsiSetIndices(Indices &&ind) {
    HPX_ASSERT_MSG(
        false, "The dsiSetIndices() method must be supported by this domain");
  }

  template <typename Indices> static bool dsiMember(Indices &&) noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiMember() method must be supported by this domain");
    return false;
  }

  template <typename Indices> static void dsiIndexOrder(Indices &&) noexcept {
    HPX_ASSERT_MSG(
        false, "The dsiIndexOrder() method must be supported by this domain");
  }
  template <typename IndexType>
  static void dsiOrderToIndex(IndexType) noexcept {
    HPX_ASSERT_MSG(
        false, "The dsiOrderToIndex() method must be supported by this domain");
  }

  static constexpr bool dsiHasSingleLocalSubdomain() noexcept { return true; }

  static constexpr void dsiLow() noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiLow() method must be supported by this domain");
  }
  static constexpr void dsiHigh() noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiHigh() method must be supported by this domain");
  }
  static constexpr void dsiAlignedLow() noexcept {
    HPX_ASSERT_MSG(
        false, "The dsiAlignedLow() method must be supported by this domain");
  }
  static constexpr void dsiAlignedHigh() noexcept {
    HPX_ASSERT_MSG(
        false, "The dsiAlignedHigh() method must be supported by this domain");
  }
  static constexpr void dsiFirst() noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiFirst() method must be supported by this domain");
  }
  static constexpr void dsiLast() noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiLast() method must be supported by this domain");
  }
  static constexpr void dsiStride() noexcept {
    HPX_ASSERT_MSG(false,
                   "The dsiStride() method must be supported by this domain");
  }
  static constexpr void dsiAlignment() noexcept {
    HPX_ASSERT_MSG(
        false, "The dsiAlignment() method must be supported by this domain");
  }

public:
  constexpr auto &dims() noexcept { return derived().dsiDims(); }
  constexpr auto const &dims() const noexcept { return derived().dsiDims(); }
  constexpr decltype(auto) dim(int i) const noexcept {
    return derived().dsiDim(i);
  }

  auto myDist() const { return derived().dsiMyDist(); }

  auto getIndices() const noexcept { return derived().dsiGetIndices(); }
  template <typename Indices> void setIndices(Indices &&ind) {
    derived().dsiSetIndices(std::forward<Indices>(ind));
  }

  template <typename Indices> bool member(Indices &&ind) const {
    return derived().dsiMember(std::forward<Indices>(ind));
  }

  template <typename Indices> auto indexOrder(Indices &&ind) const {
    return derived().dsiIndexOrder(std::forward<Indices>(ind));
  }
  template <typename IndexType> auto orderToIndex(IndexType idx) const {
    return derived().dsiOrderToIndex(idx);
  }

  constexpr auto low() const noexcept { return derived().dsiLow(); }
  constexpr auto high() const noexcept { return derived().dsiHigh(); }
  constexpr auto alignedLow() const noexcept {
    return derived().dsiAlignedLow();
  }
  constexpr auto alignedHigh() const noexcept {
    return derived().dsiAlignedHigh();
  }
  constexpr auto first() const noexcept { return derived().dsiFirst(); }
  constexpr auto last() const noexcept { return derived().dsiLast(); }
  constexpr auto stride() const noexcept { return derived().dsiStride(); }
  constexpr auto alignment() const noexcept { return derived().dsiAlignment(); }

  constexpr auto size() const noexcept { return derived().dsiNumIndices(); }

  // This is a param function which returns a boolean. If true, then the index
  // set owned by a locale can be represented by a single domain. This function
  // always returns true for non-distributed arrays and domains.
  bool hasSingleLocalSubdomain() const {
    return derived().dsiHasSingleLocalSubdomain();
  }
};

// A dmap value consists of an instance of a domain map class wrapped in an
// instance of the predefined record dmap. The domain map class is chosen and
// instantiated by the user. dmap behaves like a generic record with a single
// generic field, which holds the domain map instance.
template <int Rank, typename IndexType, bool Stridable> struct dmapBase {

  using rangeType = BoundedRange<IndexType, Stridable>;
  using indicesType = detail::generate_tuple_type_t<Rank, rangeType>;

  virtual ~dmapBase() = default;
  virtual locale idxToLocale(IndexType idx) const = 0;
  virtual void displayRepresentation() const = 0;

  virtual BaseDomain *newRectangularDom(indicesType const &inds) const = 0;
};

template <typename Dist, int Rank, typename IndexType, bool Stridable>
struct dmap : dmapBase<Rank, IndexType, Stridable> {

  using baseType = dmapBase<Rank, IndexType, Stridable>;
  using typename baseType::indicesType;
  using typename baseType::rangeType;

  explicit dmap(hpx::intrusive_ptr<Dist> d) noexcept
      : dm(hpx::static_pointer_cast<GlobalDistribution<Dist>>(std::move(d))) {}

  locale idxToLocale(IndexType idx) const override {
    return dm->indexToLocale(idx);
  }

  void displayRepresentation() const override { dm->displayRepresentation(); }

  BaseDomain *newRectangularDom(indicesType const &inds) const override {
    return dm->template newRectangularDom<Rank, IndexType, Stridable>(inds);
  }

private:
  hpx::intrusive_ptr<GlobalDistribution<Dist>> dm;
};

// Compare domain maps. Return whether or not the two domain maps are "equal"
// (specify the same distribution).
template <typename DM1, int Rank1, typename IndexType1, bool Stridable1,
          typename DM2, int Rank2, typename IndexType2, bool Stridable2>
bool operator==(dmap<DM1, Rank1, IndexType1, Stridable1> const &lhs,
                dmap<DM2, Rank2, IndexType2, Stridable2> const &rhs) {

  return lhs.dm->equalDMaps(*rhs.dm);
}

// If a dmapped clause starts with the name of a domain map class, it is
// considered to be an initialization expression as if preceded by new. The
// resulting domain map instance is wrapped in a newly-created instance of dmap
// implicitly.
template <typename Target, typename Dist> struct dmapped_t {
  template <typename... Ts> auto operator()(Ts &&...ts) const = delete;
};

template <typename Target, typename Dist>
inline constexpr dmapped_t<Target, Dist> dmapped = dmapped_t<Target, Dist>{};

//-----------------------------------------------------------------------------
template <typename Derived> struct GlobalArray : BaseArray {};

} // namespace chplx
