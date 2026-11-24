#ifndef MY_QUEUE_H_
#define MY_QUEUE_H_

#include "../include/my_list.h"

namespace my {
template <typename T, typename Container = List<T>>
class Queue final {
 public:
  using value_type = typename Container::value_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using size_type = typename Container::size_type;
  using container_type = Container;

 public:
  Queue() = default;
  Queue(std::initializer_list<value_type> il) : c(il) {}
  Queue(const Queue& other) : c(other.c) {}
  Queue(Queue&& other) noexcept { swap(other); }
  Queue& operator=(Queue temp) noexcept {
    swap(temp);
    return *this;
  }
  explicit Queue(const container_type& cont) : c(cont) {}
  explicit Queue(container_type&& cont) : c(std::move(cont)) {}

 public:
  [[nodiscard]] constexpr bool empty() const { return c.empty(); }
  [[nodiscard]] constexpr size_type size() const { return c.size(); }
  [[nodiscard]] constexpr reference front() {
    assert(this->empty() == false);
    return c.front();
  }
  [[nodiscard]] constexpr const_reference front() const {
    assert(this->empty() == false);
    return c.front();
  }
  [[nodiscard]] constexpr reference back() {
    assert(this->empty() == false);
    return c.back();
  }
  [[nodiscard]] constexpr const_reference back() const {
    assert(this->empty() == false);
    return c.back();
  }
  constexpr void push(const value_type& x) { c.push_back(x); }
  constexpr void push(value_type&& x) { c.push_back(std::move(x)); }
  constexpr void pop() {
    assert(this->empty() == false);
    c.pop_front();
  }
  constexpr void swap(Queue& other) noexcept(noexcept(std::swap(c, other.c))) {
    std::swap(c, other.c);
  }
  template <typename... Args>
  void insert_many_back(Args&&... args) {
    c.insert_many_back(args...);
  }

 protected:
  Container c;
};
}  // namespace my
#endif  // MY_QUEUE_H_
