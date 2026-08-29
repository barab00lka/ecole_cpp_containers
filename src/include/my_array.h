#ifndef MY_ARRAY_H_
#define MY_ARRAY_H_
#include <iterator>

namespace my {
template <typename T, std::size_t n>
struct Array final {
  T start_[n];
  // member types
  using value_type = T;
  using reference = T&;  // same as typedef T& reference;
  using const_reference = const T&;
  using iterator = T*;  // (its just a raw pointer)
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using pointer = T*;
  using const_pointer = const T*;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;

  [[nodiscard]] constexpr size_type size() const noexcept { return n; }
  [[nodiscard]] constexpr size_type max_size() const noexcept {
    return std::numeric_limits<size_type>::max() / sizeof(T);
  }
  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

  // element accessor
  constexpr reference operator[](size_type i) noexcept { return *(start_ + i); }
  constexpr reference at(size_type i) {
    if (i >= size()) {
      throw std::out_of_range("...");
    }
    return operator[](i);
  }
  constexpr const_reference operator[](size_type i) const noexcept {
    return *(start_ + i);
  }
  constexpr const_reference at(size_type i) const {
    if (i >= size()) {
      throw std::out_of_range("...");
    }
    return operator[](i);
  }
  [[nodiscard]] constexpr reference front() noexcept { return *start_; }
  [[nodiscard]] constexpr reference back() noexcept { return *(end() - 1); }
  [[nodiscard]] constexpr const_reference front() const noexcept {
    return *start_;
  }
  [[nodiscard]] constexpr const_reference back() const noexcept {
    return *(end() - 1);
  }
  [[nodiscard]] constexpr pointer data() noexcept { return start_; }
  [[nodiscard]] constexpr const_pointer data() const noexcept { return start_; }

  // iterators
  [[nodiscard]] constexpr iterator begin() noexcept { return start_; }
  [[nodiscard]] constexpr iterator end() noexcept { return start_ + size(); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return start_;
  }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return start_;
  }
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return start_ + size();
  }
  [[nodiscard]] constexpr const_iterator cend() const noexcept {
    return start_ + size();
  }
  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  [[nodiscard]] constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(cbegin());
  }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  constexpr void swap(Array& other) noexcept {
    for (size_type i = 0; i < size(); ++i)
      std::swap(other.start_[i], start_[i]);
  }
  constexpr void fill(const_reference value) {
    for (size_type i = 0; i < size(); ++i) start_[i] = value;
  }

  friend constexpr bool operator==(const Array& lhs, const Array& rhs) {
    auto lsize = lhs.size(), rsize = rhs.size();
    if (lsize != rsize) return false;
    for (size_type i = 0; i < lsize; ++i)
      if (lhs[i] != rhs[i]) return false;
    return true;
  }
};
}  // namespace my
#endif  // MY_ARRAY_H_
