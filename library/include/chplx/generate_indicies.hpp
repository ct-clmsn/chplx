//  Copyright (c) 2015-2017 Bryce Adelstein Lelbach aka wash
//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/detail/range_index_generator.hpp>
#include <chplx/range.hpp>

#include <hpx/config.hpp>

#include <cassert>
#include <exception>

namespace chplx {

template <typename T, BoundedRangeType BoundedType, bool Stridable>
detail::IndexGenerator<T, 1>
generateIndices(Range<T, BoundedType, Stridable> ni) noexcept {
  T const nilo = ni.getFirstIndex();
  T const nihi = ni.getLastIndex();
  assert(nilo >= 0);
  assert(nihi > nilo);
  for (T i = nilo; i != nihi; ++i)
    co_yield detail::Position<T, 1>(i);
}

template <typename IndexType>
detail::IndexGenerator<IndexType, 1> generateIndices(IndexType ni) noexcept {
  return generateIndices(Range<IndexType, BoundedRangeType::bounded>(
      0, ni, detail::BoundsCategoryType::Open));
}

template <typename T, BoundedRangeType BoundedType0, bool Stridable0,
          BoundedRangeType BoundedType1, bool Stridable1>
detail::IndexGenerator<T, 2>
generateIndices(Range<T, BoundedType0, Stridable0> ni,
                Range<T, BoundedType1, Stridable1> nj) noexcept {
  T const nilo = ni.getFirstIndex();
  T const nihi = ni.getLastIndex();
  T const njlo = nj.getFirstIndex();
  T const njhi = nj.getLastIndex();
  assert(njlo >= 0);
  assert(nilo >= 0);
  assert(njhi > njlo);
  assert(nihi > nilo);
  for (T j = njlo; j != njhi; ++j)
    for (T i = nilo; i != nihi; ++i)
      co_yield detail::Position<T, 2>(i, j);
}

template <typename IndexType>
detail::IndexGenerator<IndexType, 2> generateIndices(IndexType ni,
                                                     IndexType nj) noexcept {
  return generateIndices<IndexType>(
      Range<IndexType, BoundedRangeType::bounded>(
          0, ni, detail::BoundsCategoryType::Open),
      Range<IndexType, BoundedRangeType::bounded>(
          0, nj, detail::BoundsCategoryType::Open));
}

#if 0
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
  assert(nklo >= 0);
  assert(njlo >= 0);
  assert(nilo >= 0);
  assert(nkhi > nklo);
  assert(njhi > njlo);
  assert(nihi > nilo);
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
  assert(nllo >= 0);
  assert(nklo >= 0);
  assert(njlo >= 0);
  assert(nilo >= 0);
  assert(nlhi > nllo);
  assert(nkhi > nklo);
  assert(njhi > njlo);
  assert(nihi > nilo);
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
  assert(nmlo >= 0);
  assert(nllo >= 0);
  assert(nklo >= 0);
  assert(njlo >= 0);
  assert(nilo >= 0);
  assert(nmhi > nmlo);
  assert(nlhi > nllo);
  assert(nkhi > nklo);
  assert(njhi > njlo);
  assert(nihi > nilo);
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
  assert(nnlo >= 0);
  assert(nmlo >= 0);
  assert(nllo >= 0);
  assert(nklo >= 0);
  assert(njlo >= 0);
  assert(nilo >= 0);
  assert(nnhi > nnlo);
  assert(nmhi > nmlo);
  assert(nlhi > nllo);
  assert(nkhi > nklo);
  assert(njhi > njlo);
  assert(nihi > nilo);
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
