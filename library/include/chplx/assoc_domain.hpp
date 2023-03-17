//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <chplx/adapt_domain.hpp>
#include <chplx/detail/iterator_generator.hpp>
#include <chplx/domain.hpp>
#include <chplx/types.hpp>

#include <hpx/datastructures/detail/flat_set.hpp>

#include <iterator>
#include <vector>

namespace chplx {

//-----------------------------------------------------------------------------
template <typename T> class IndexBuffer;

//-----------------------------------------------------------------------------
template <typename IndexType> class AssocDomain {

public:
  using idxType = IndexType;

  // Return the number of indices in this domain as an int.
  [[nodiscard]] constexpr std::int64_t size() const noexcept {
    return indices.size();
  }

  // Return the number of indices in this domain as the specified type
  template <typename T>
    requires(std::is_integral_v<T>)
  [[nodiscard]] constexpr T sizeAs() const noexcept {
    return static_cast<T>(indices.size());
  }

  // Yield the domain indices
  [[nodiscard]] decltype(auto) these() const { return iterate(*this); }

  // Yield the domain indices in sorted order
  [[nodiscard]] decltype(auto) sorted() const { return these(); }

  // Add index idx to this domain. This method is also available as the +=
  // operator.
  void add(IndexType idx) { indices.insert(idx); }

  // Remove index idx from this domain.
  void remove(IndexType idx) { indices.erase(idx); }

  // Remove all indices from this domain, leaving it empty
  void clear() { indices.clear(); }

  // Request space for a particular number of values in an domain.
  static constexpr void requestCapacity(std::int64_t) {} // not implemented

  // Return true if this domain contains idx. Otherwise return false.
  bool contains(IndexType idx) { return indices.contains(idx); }

  // Returns true if this domain contains all the indices in the other domain.
  bool contains(AssocDomain const &d) {

    for (auto const &idx : d.values()) {
      if (!contains(idx)) {
        return false;
      }
    }
    return true;
  }

  // Returns an integer representing the zero-based ordinal value of ind within
  // the domain's sequence of values if it is a member of the sequence.
  // Otherwise, returns -1. The indexOrder procedure is the reverse of
  // orderToIndex.
  [[nodiscard]] constexpr std::int64_t indexOrder(idxType idx) const noexcept {
    auto it = indices.find(idx);
    return it != indices.end() ? std::distance(indices.begin(), it) : -1;
  }

  // Returns the zero-based ord-th element of this domain's represented
  // sequence. The orderToIndex procedure is the reverse of indexOrder.
  [[nodiscard]] constexpr idxType
  orderToIndex(std::int64_t order) const noexcept {
    return *std::next(indices.begin(), order);
  }

  // Return true if the domain has no indices.
  [[nodiscard]] constexpr bool isEmpty() const noexcept {
    return indices.empty();
  }

  // Returns true if this is a fully bounded domain, false otherwise.
  [[nodiscard]] static constexpr bool isBounded() noexcept { return true; }

  // Return true if d is an irregular domain; e.g. is not rectangular. Otherwise
  // return false.
  [[nodiscard]] static constexpr bool isIrregular() noexcept { return true; }

  // Return true if d is an associative domain. Otherwise return false.
  [[nodiscard]] static constexpr bool isAssociative() noexcept { return true; }

  // Return true if d is a sparse domain. Otherwise return false.
  [[nodiscard]] static constexpr bool isSparse() noexcept { return false; }

  // Return true if this domain is a rectangular. Otherwise return false.
  [[nodiscard]] static constexpr bool isRectangular() noexcept { return false; }

  // Creates an index buffer which can be used for faster index addition.
  inline IndexBuffer<IndexType> createIndexBuffer(std::int64_t size);

  // Helper function for iteration
  [[nodiscard]] auto &values() { return indices; }
  [[nodiscard]] auto const &values() const { return indices; }

private:
  hpx::detail::flat_set<IndexType> indices{};
};

//-----------------------------------------------------------------------------
template <typename T> inline constexpr bool isDomainType<AssocDomain<T>> = true;

template <typename T>
[[nodiscard]] constexpr bool isDomainValue(AssocDomain<T> const &) noexcept {
  return true;
}

//-----------------------------------------------------------------------------
// Equality operators are defined to test if two domains are equivalent or not
template <typename T>
constexpr bool operator==(AssocDomain<T> const &lhs,
                          AssocDomain<T> const &rhs) noexcept {

  return lhs.values() == rhs.values();
}

template <typename T>
constexpr bool operator!=(AssocDomain<T> const &lhs,
                          AssocDomain<T> const &rhs) noexcept {

  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
template <typename T>
std::ostream &operator<<(std::ostream &os, AssocDomain<T> const &d) {

  os << "{";
  std::size_t size = d.size();
  for (auto &&elem : d.these()) {
    os << elem;
    if (size-- != 0) {
      os << ", ";
    }
  }
  os << "}";
  return os;
}

//-----------------------------------------------------------------------------
template <typename T> class IndexBuffer {

public:
  IndexBuffer(AssocDomain<T> &target, std::int64_t size)
      : target(target), maxSize(size) {
    buffer.reserve(size);
  }

  IndexBuffer(IndexBuffer const &) = default;
  IndexBuffer(IndexBuffer &&) = default;
  IndexBuffer &operator=(IndexBuffer const &) = default;
  IndexBuffer &operator=(IndexBuffer &&) = default;

  ~IndexBuffer() { commit(); }

  void add(T val) {

    if (buffer.size() >= maxSize) {
      commit();
    }
    buffer.push_back(val);
  }

  void commit() {

    target.values().insert(std::make_move_iterator(buffer.begin()),
                           std::make_move_iterator(buffer.end()));
    buffer.clear();
  }

private:
  AssocDomain<T> &target;
  std::int64_t maxSize;
  std::vector<T> buffer;
};

template <typename T> IndexBuffer(AssocDomain<T>) -> IndexBuffer<T>;

//-----------------------------------------------------------------------------
template <typename T>
[[nodiscard]] IndexBuffer<T>
AssocDomain<T>::createIndexBuffer(std::int64_t size) {

  return IndexBuffer(*this, size);
}

} // namespace chplx
