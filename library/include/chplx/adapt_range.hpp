//  Copyright (c) 2015-2017 Bryce Adelstein Lelbach aka wash
//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/detail/range_index_generator.hpp>
#include <chplx/range.hpp>

#include <hpx/assert.hpp>
#include <hpx/config.hpp>
#include <hpx/generator.hpp>

#include <cstdlib>

namespace chplx {

//-----------------------------------------------------------------------------
// 1D iteration support
template <typename T, BoundedRangeType BoundedType, bool Stridable>
hpx::generator<T &, T>
iterate(Range<T, BoundedType, Stridable> const &ni) noexcept {

  HPX_ASSERT(ni.isIterable());

  auto const nilo = ni.first();
  auto const nihi = ni.last();
  auto const nistride = ni.stride();

  HPX_ASSERT(nistride != 0);

  if (nistride > 0) {
    for (auto i = nilo; i <= nihi; i += nistride) {
      co_yield i;
    }

  } else {
    for (auto i = nilo; i >= nihi; i += nistride) {
      co_yield i;
    }
  }
}

template <typename IndexType,
          typename = std::enable_if_t<std::is_integral_v<IndexType>>>
hpx::generator<IndexType &, IndexType> iterate(IndexType ni) noexcept {
  return iterate(Range<IndexType, BoundedRangeType::bounded>(
      0, ni, BoundsCategoryType::Open));
}

template <typename T, BoundedRangeType BoundedType, bool Stridable>
constexpr bool empty(Range<T, BoundedType, Stridable> const &r) noexcept {

  if (r.isAmbiguous()) {

    if (!r.hasFirst() || !r.hasLast()) {
      return false;
    }
    return r.last() - r.first() == 0;
  }
  return r.isEmpty();
}

template <typename T1, BoundedRangeType BoundedType1, bool Stridable1,
          typename T2, BoundedRangeType BoundedType2, bool Stridable2>
constexpr std::ptrdiff_t
offset(Range<T1, BoundedType1, Stridable1> const &r1,
       Range<T2, BoundedType2, Stridable2> const &r2) noexcept {

  HPX_ASSERT(r1.hasLowBound() && r2.hasLowBound());
  HPX_ASSERT(r1.stride() == r2.stride());
  auto stride = std::abs(r1.stride());
  return static_cast<std::ptrdiff_t>(r1.lowBound() - r2.lowBound()) / stride;
}

template <typename T, BoundedRangeType BoundedType, bool Stridable>
auto subrange(Range<T, BoundedType, Stridable> const &r, std::ptrdiff_t first,
              std::size_t size) {

  HPX_ASSERT(r.hasLowBound());
  auto stride = std::abs(r.stride());
  Range<T, BoundedType, Stridable> result(
      r.lowBound() + first * stride, r.lowBound() + (first + size) * stride,
      r.stride(), chplx::BoundsCategoryType::Open);
  return result;
}

#if 0
//-----------------------------------------------------------------------------
// 2D iteration support
template <typename T, BoundedRangeType BoundedType0, bool Stridable0,
          BoundedRangeType BoundedType1, bool Stridable1>
hpx::generator<detail::Position<T, 2>>
iterate(Range<T, BoundedType0, Stridable0> ni,
        Range<T, BoundedType1, Stridable1> nj) noexcept {
  auto const nilo = ni.getFirstIndex();
  auto const nihi = ni.getLastIndex();
  auto const nistride = ni.stride();

  auto const njlo = nj.getFirstIndex();
  auto const njhi = nj.getLastIndex();
  auto const njstride = nj.stride();

  if (nistride > 0 && njstride > 0) {
    HPX_ASSERT(nihi >= nilo);
    HPX_ASSERT(njhi >= njlo);
    for (auto j = njlo; j < njhi; j += njstride)
      for (auto i = nilo; i < nihi; i += nistride)
        co_yield detail::Position<T, 2>(i, j);

  } else if (nistride < 0 && njstride > 0) {
    HPX_ASSERT(nihi <= nilo);
    HPX_ASSERT(njhi >= njlo);
    for (auto j = njlo; j < njhi; j += njstride)
      for (auto i = nilo; i > nihi; i += nistride)
        co_yield detail::Position<T, 2>(i, j);

  } else if (nistride > 0 && njstride < 0) {
    HPX_ASSERT(nihi >= nilo);
    HPX_ASSERT(njhi <= njlo);
    for (auto j = njlo; j > njhi; j += njstride)
      for (auto i = nilo; i < nihi; i += nistride)
        co_yield detail::Position<T, 2>(i, j);

  } else if (nistride < 0 && njstride < 0) {
    HPX_ASSERT(nihi <= nilo);
    HPX_ASSERT(njhi <= njlo);
    for (auto j = njlo; j > njhi; j += njstride)
      for (auto i = nilo; i > nihi; i += nistride)
        co_yield detail::Position<T, 2>(i, j);
  }
}

template <typename IndexType,
          typename = std::enable_if_t<std::is_integral_v<IndexType>>>
hpx::generator<detail::Position<IndexType, 2>> iterate(IndexType ni,
                                                       IndexType nj) noexcept {
  return iterate(Range<IndexType, BoundedRangeType::bounded>(
                     0, ni, BoundsCategoryType::Open),
                 Range<IndexType, BoundedRangeType::bounded>(
                     0, nj, BoundsCategoryType::Open));
}

template <typename IndexType>
IndexGenerator<IndexType, 3>
generateIndices(Position<IndexType, 2> ni, Position<IndexType, 2> nj,
                Position<IndexType, 2> nk) noexcept {
  IndexType const nilo = ni[0];
  IndexType const nihi = ni[1];
  IndexType const njlo = nj[0];
  IndexType const njhi = nj[1];
  IndexType const nklo = nk[0];
  IndexType const nkhi = nk[1];
  HPX_ASSERT(nklo >= 0);
  HPX_ASSERT(njlo >= 0);
  HPX_ASSERT(nilo >= 0);
  HPX_ASSERT(nkhi > nklo);
  HPX_ASSERT(njhi > njlo);
  HPX_ASSERT(nihi > nilo);
  for (IndexType k = nklo; k != nkhi; ++k)
    for (IndexType j = njlo; j != njhi; ++j)
      for (IndexType i = nilo; i != nihi; ++i)
        co_yield Position<IndexType, 3>(i, j, k);
}

template <typename IndexType>
IndexGenerator<IndexType, 3> generateIndices(IndexType ni, IndexType nj,
                                             IndexType nk) noexcept {
  return generateIndices<IndexType>({0, ni}, {0, nj}, {0, nk});
}

template <typename IndexType>
IndexGenerator<IndexType, 4>
generateIndices(Position<IndexType, 2> ni, Position<IndexType, 2> nj,
                Position<IndexType, 2> nk, Position<IndexType, 2> nl) noexcept {
  IndexType const nilo = ni[0];
  IndexType const nihi = ni[1];
  IndexType const njlo = nj[0];
  IndexType const njhi = nj[1];
  IndexType const nklo = nk[0];
  IndexType const nkhi = nk[1];
  IndexType const nllo = nl[0];
  IndexType const nlhi = nl[1];
  HPX_ASSERT(nllo >= 0);
  HPX_ASSERT(nklo >= 0);
  HPX_ASSERT(njlo >= 0);
  HPX_ASSERT(nilo >= 0);
  HPX_ASSERT(nlhi > nllo);
  HPX_ASSERT(nkhi > nklo);
  HPX_ASSERT(njhi > njlo);
  HPX_ASSERT(nihi > nilo);
  for (IndexType l = nllo; l != nlhi; ++l)
    for (IndexType k = nklo; k != nkhi; ++k)
      for (IndexType j = njlo; j != njhi; ++j)
        for (IndexType i = nilo; i != nihi; ++i)
          co_yield Position<IndexType, 4>(i, j, k, l);
}

template <typename IndexType>
IndexGenerator<IndexType, 4> generateIndices(IndexType ni, IndexType nj,
                                             IndexType nk,
                                             IndexType nl) noexcept {
  return generateIndices<IndexType>({0, ni}, {0, nj}, {0, nk}, {0, nl});
}

template <typename IndexType>
IndexGenerator<IndexType, 5>
generateIndices(Position<IndexType, 2> ni, Position<IndexType, 2> nj,
                Position<IndexType, 2> nk, Position<IndexType, 2> nl,
                Position<IndexType, 2> nm) noexcept {
  IndexType const nilo = ni[0];
  IndexType const nihi = ni[1];
  IndexType const njlo = nj[0];
  IndexType const njhi = nj[1];
  IndexType const nklo = nk[0];
  IndexType const nkhi = nk[1];
  IndexType const nllo = nl[0];
  IndexType const nlhi = nl[1];
  IndexType const nmlo = nm[0];
  IndexType const nmhi = nm[1];
  HPX_ASSERT(nmlo >= 0);
  HPX_ASSERT(nllo >= 0);
  HPX_ASSERT(nklo >= 0);
  HPX_ASSERT(njlo >= 0);
  HPX_ASSERT(nilo >= 0);
  HPX_ASSERT(nmhi > nmlo);
  HPX_ASSERT(nlhi > nllo);
  HPX_ASSERT(nkhi > nklo);
  HPX_ASSERT(njhi > njlo);
  HPX_ASSERT(nihi > nilo);
  for (IndexType m = nmlo; m != nmhi; ++m)
    for (IndexType l = nllo; l != nlhi; ++l)
      for (IndexType k = nklo; k != nkhi; ++k)
        for (IndexType j = njlo; j != njhi; ++j)
          for (IndexType i = nilo; i != nihi; ++i)
            co_yield Position<IndexType, 5>(i, j, k, l, m);
}

template <typename IndexType>
IndexGenerator<IndexType, 5> generateIndices(IndexType ni, IndexType nj,
                                             IndexType nk, IndexType nl,
                                             IndexType nm) noexcept {
  return generateIndices<IndexType>({0, ni}, {0, nj}, {0, nk}, {0, nl},
                                    {0, nm});
}

template <typename IndexType>
IndexGenerator<IndexType, 6>
generateIndices(Position<IndexType, 2> ni, Position<IndexType, 2> nj,
                Position<IndexType, 2> nk, Position<IndexType, 2> nl,
                Position<IndexType, 2> nm, Position<IndexType, 2> nn) noexcept {
  IndexType const nilo = ni[0];
  IndexType const nihi = ni[1];
  IndexType const njlo = nj[0];
  IndexType const njhi = nj[1];
  IndexType const nklo = nk[0];
  IndexType const nkhi = nk[1];
  IndexType const nllo = nl[0];
  IndexType const nlhi = nl[1];
  IndexType const nmlo = nm[0];
  IndexType const nmhi = nm[1];
  IndexType const nnlo = nn[0];
  IndexType const nnhi = nn[1];
  HPX_ASSERT(nnlo >= 0);
  HPX_ASSERT(nmlo >= 0);
  HPX_ASSERT(nllo >= 0);
  HPX_ASSERT(nklo >= 0);
  HPX_ASSERT(njlo >= 0);
  HPX_ASSERT(nilo >= 0);
  HPX_ASSERT(nnhi > nnlo);
  HPX_ASSERT(nmhi > nmlo);
  HPX_ASSERT(nlhi > nllo);
  HPX_ASSERT(nkhi > nklo);
  HPX_ASSERT(njhi > njlo);
  HPX_ASSERT(nihi > nilo);
  for (IndexType n = nnlo; n != nnhi; ++n)
    for (IndexType m = nmlo; m != nmhi; ++m)
      for (IndexType l = nllo; l != nlhi; ++l)
        for (IndexType k = nklo; k != nkhi; ++k)
          for (IndexType j = njlo; j != njhi; ++j)
            for (IndexType i = nilo; i != nihi; ++i)
              co_yield Position<IndexType, 6>(i, j, k, l, m, n);
}

template <typename IndexType>
static IndexGenerator<IndexType, 6>
generateIndices(IndexType ni, IndexType nj, IndexType nk, IndexType nl,
                IndexType nm, IndexType nn) noexcept {
  return generateIndices<IndexType>({0, ni}, {0, nj}, {0, nk}, {0, nl}, {0, nm},
                                    {0, nn});
}
#endif

} // namespace chplx
