#ifndef MY_SET_H_
#define MY_SET_H_

#include <cassert>
#include <iterator>
#include <limits>
#include <stdexcept>

#include "my_avltree.h"
#include "my_vector.h"

namespace my {
template <typename K, typename Compare = std::less<K>>
class Set : public Tree<K, std::nullptr_t, Compare> {
 public:
  using key_type = K;
  using value_type = key_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = typename Tree<K, std::nullptr_t, Compare>::iterator;
  using const_iterator =
      typename Tree<K, std::nullptr_t, Compare>::const_iterator;
  using reverse_iterator =
      typename Tree<K, std::nullptr_t, Compare>::reverse_iterator;
  using const_reverse_iterator =
      typename Tree<K, std::nullptr_t, Compare>::const_reverse_iterator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using node_type = typename Tree<K, std::nullptr_t, Compare>::node_type;

 public:
  Set() = default;
  Set(std::initializer_list<value_type> const& items) {
    for (const_reference item : items) this->insert_uniq(item);
  }
  // copy/move/assignment just forward to the Tree base (which owns the nodes)
  Set(const Set& other) = default;
  Set(Set&& other) noexcept = default;
  Set& operator=(const Set& other) = default;
  Set& operator=(Set&& other) noexcept = default;
  ~Set() = default;

 public:
  void merge(Set& other) { this->merge_uniq(other); }
  std::pair<iterator, bool> insert(const value_type& value) {
    return this->insert_uniq(value);
  }

  template <typename... Args>
  Vector<std::pair<iterator, bool>> insert_many(Args&&... args) {
    Vector<std::pair<iterator, bool>> results;
    (results.push_back(this->insert_uniq(std::forward<Args>(args))), ...);
    return results;
  }
};
}  // namespace my
#endif  // MY_SET_H_
