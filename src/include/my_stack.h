#ifndef MY_STACK_H_
#define MY_STACK_H_

#include "../include/my_vector.h"

namespace my {
// Assigns specified base Container by default
template <typename T, typename Container = Vector<T>>
class Stack final {
 public:
  using value_type = typename Container::value_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using size_type = typename Container::size_type;
  using container_type = Container;

 public:
  Stack() = default;
  Stack(std::initializer_list<value_type> il) : c(il) {}
  Stack(const Stack& other) : c(other.c) {}
  Stack(Stack&& other) noexcept {
    if (this != &other) swap(other);
  }
  Stack& operator=(Stack temp) noexcept {
    swap(temp);
    return *this;
  }
  ~Stack() = default;  // does not need to be declared explicitly, only for
                       // documentation purposes
  explicit Stack(const container_type& cont) : c(cont) {}
  explicit Stack(Container&& cont) : c(std::move(cont)) {}

 public:
  [[nodiscard]] constexpr bool empty() const { return c.empty(); }
  [[nodiscard]] constexpr size_type size() const { return c.size(); }
  [[nodiscard]] constexpr reference top() {
    assert(this->empty() == false);
    return c.back();
  }
  [[nodiscard]] constexpr const_reference top() const {
    assert(this->empty() == false);
    return c.back();
  }
  constexpr void push(const value_type& x) { c.push_back(x); }
  constexpr void push(value_type&& x) { c.push_back(std::move(x)); }
  constexpr void pop() {
    assert(this->empty() == false);
    c.pop_back();
  }
  constexpr void swap(Stack& other) noexcept(noexcept(std::swap(c, other.c))) {
    std::swap(c, other.c);
  }
  template <typename... Args>
  void insert_many_back(Args&&... args) {
    c.insert_many_back(args...);
  }

 protected:
  Container c;
};  // namespace my
}  // namespace my
#endif  // MY_STACK_H_
