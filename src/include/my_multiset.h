#ifndef MY_MULTISET_H_
#define MY_MULTISET_H_

#include <cassert>
#include <iterator>
#include <limits>
#include <stdexcept>

#include "my_avltree.h"
#include "my_vector.h"

namespace my {
template <typename K, typename Compare = std::less<K>>
class Multiset : public Tree<K, std::nullptr_t, Compare> {
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
  Multiset() = default;
  Multiset(std::initializer_list<value_type> const& items) {
    for (const_reference item : items) this->insert(item);
  }
  Multiset(const Multiset& other) = default;
  Multiset(Multiset&& other) noexcept = default;
  Multiset& operator=(const Multiset& other) = default;
  Multiset& operator=(Multiset&& other) noexcept = default;
  ~Multiset() = default;

 public:
  // Tree::insert keeps duplicates (equal keys become right-descendants)
  using Tree<K, std::nullptr_t, Compare>::insert;
  void merge(Multiset& other) { this->merge_multi(other); }

  template <typename... Args>
  Vector<std::pair<iterator, bool>> insert_many(Args&&... args) {
    Vector<std::pair<iterator, bool>> results;
    // API requested pair even for multiset
    (results.push_back({insert(std::forward<Args>(args)), true}), ...);
    return results;
  }
};
}  // namespace my
#endif  // MY_MULTISET_H_
