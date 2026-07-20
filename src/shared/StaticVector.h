#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

/**
 * StaticVector - A fixed-size, pre-allocated vector-like container for embedded systems.
 * 
 * Features:
 * - No dynamic allocation: all memory is reserved at compile time
 * - STL-like interface: push_back, pop_back, erase, size, operator[]
 * - Efficient removal: uses swap-remove for O(1) erase operations
 * - Suitable for embedded/real-time systems with deterministic behavior
 */
template <typename T, size_t MAX_SIZE>
class StaticVector {
public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using size_type = size_t;

  StaticVector() : count_(0) {}

  /**
   * Add an element to the end of the vector.
   * Throws if the vector is at capacity.
   */
  void push_back(const T& value) {
    if (count_ >= MAX_SIZE) {
      throw std::overflow_error("StaticVector: push_back overflow");
    }
    items_[count_] = value;
    ++count_;
  }

  /**
   * Move-construct an element at the end of the vector.
   */
  void push_back(T&& value) {
    if (count_ >= MAX_SIZE) {
      throw std::overflow_error("StaticVector: push_back overflow");
    }
    items_[count_] = std::move(value);
    ++count_;
  }

  /**
   * Remove the last element.
   * Does nothing if the vector is empty.
   */
  void pop_back() {
    if (count_ > 0) {
      --count_;
    }
  }

  /**
   * Remove element at index and preserve order.
   * Elements after the removed index are shifted left.
   * Throws if index is out of bounds.
   */
  void erase(size_t index) {
    if (index >= count_) {
      throw std::out_of_range("StaticVector: erase index out of range");
    }
    for (size_t moveIndex = index; moveIndex + 1 < count_; ++moveIndex) {
      items_[moveIndex] = std::move(items_[moveIndex + 1]);
    }
    --count_;
  }

  /**
   * Remove all elements.
   */
  void clear() {
    count_ = 0;
  }

  /**
   * Get the number of elements in the vector.
   */
  size_type size() const {
    return count_;
  }

  /**
   * Get the maximum capacity of the vector.
   */
  static constexpr size_type capacity() {
    return MAX_SIZE;
  }

  /**
   * Check if the vector is empty.
   */
  bool empty() const {
    return count_ == 0;
  }

  /**
   * Check if the vector is at maximum capacity.
   */
  bool full() const {
    return count_ >= MAX_SIZE;
  }

  /**
   * Access element at index.
   * Throws if index is out of bounds.
   */
  reference at(size_t index) {
    if (index >= count_) {
      throw std::out_of_range("StaticVector: at() index out of range");
    }
    return items_[index];
  }

  /**
   * Access element at index (const).
   * Throws if index is out of bounds.
   */
  const_reference at(size_t index) const {
    if (index >= count_) {
      throw std::out_of_range("StaticVector: at() index out of range");
    }
    return items_[index];
  }

  /**
   * Access element at index (no bounds checking).
   */
  reference operator[](size_t index) {
    return items_[index];
  }

  /**
   * Access element at index (const, no bounds checking).
   */
  const_reference operator[](size_t index) const {
    return items_[index];
  }

  /**
   * Get reference to the last element.
   * Undefined behavior if the vector is empty.
   */
  reference back() {
    return items_[count_ - 1];
  }

  /**
   * Get const reference to the last element.
   * Undefined behavior if the vector is empty.
   */
  const_reference back() const {
    return items_[count_ - 1];
  }

  /**
   * Get reference to the first element.
   * Undefined behavior if the vector is empty.
   */
  reference front() {
    return items_[0];
  }

  /**
   * Get const reference to the first element.
   * Undefined behavior if the vector is empty.
   */
  const_reference front() const {
    return items_[0];
  }

  /**
   * Pointer to underlying data for compatibility.
   */
  T* data() {
    return items_.data();
  }

  /**
   * Const pointer to underlying data.
   */
  const T* data() const {
    return items_.data();
  }

  // Iterator support
  using iterator = typename std::array<T, MAX_SIZE>::iterator;
  using const_iterator = typename std::array<T, MAX_SIZE>::const_iterator;

  iterator begin() {
    return items_.begin();
  }

  const_iterator begin() const {
    return items_.begin();
  }

  iterator end() {
    return items_.begin() + count_;
  }

  const_iterator end() const {
    return items_.begin() + count_;
  }

private:
  std::array<T, MAX_SIZE> items_;
  size_t count_;
};
