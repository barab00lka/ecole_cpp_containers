#ifndef MY_MAP_H_
#define MY_MAP_H_

#include <cassert>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>

#include "my_avltree.h"
#include "my_vector.h"

namespace my {
template <typename K, typename V, typename Compare = std::less<K>>
class Map : public Tree<K, V, Compare> {
 public:
  using key_type = K;
  using mapped_type = V;
  using value_type = std::pair<const key_type, mapped_type>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = typename Tree<K, V, Compare>::iterator;
  using const_iterator = typename Tree<K, V, Compare>::const_iterator;
  using size_type = std::size_t;
  using node_type = typename Tree<K, V, Compare>::node_type;

 public:
  Map() = default;
  Map(std::initializer_list<value_type> const& items) {
    for (const value_type& v : items) insert(v);
  }
  Map(const Map& other) = default;
  Map(Map&& other) noexcept = default;
  Map& operator=(const Map& other) = default;
  Map& operator=(Map&& other) noexcept = default;
  ~Map() = default;

  mapped_type& at(const key_type& key) {
    iterator it = this->find(key);
    if (it == this->end()) throw std::out_of_range("Map::at: key not found");
    return it->second;
  }
  const mapped_type& at(const key_type& key) const {
    return const_cast<Map*>(this)->at(key);
  }

  mapped_type& operator[](const key_type& key) {
    iterator it = this->find(key);
    if (it == this->end()) it = this->insert_uniq({key, mapped_type{}}).first;
    return it->second;
  }

  std::pair<iterator, bool> insert(const value_type& value) {
    return this->insert_uniq({value.first, value.second});
  }
  std::pair<iterator, bool> insert(const key_type& key,
                                   const mapped_type& obj) {
    return this->insert_uniq({key, obj});
  }

  std::pair<iterator, bool> insert_or_assign(const key_type& key,
                                             const mapped_type& obj) {
    iterator it = this->find(key);
    if (it != this->end()) {
      it->second = obj;
      return {it, false};
    }
    return this->insert_uniq({key, obj});
  }

  template <typename... Args>
  Vector<std::pair<iterator, bool>> insert_many(Args&&... args) {
    Vector<std::pair<iterator, bool>> results;
    (results.push_back(this->insert_uniq(std::forward<Args>(args))), ...);
    return results;
  }

  void merge(Map& other) { this->merge_uniq(other); }
};
}  // namespace my
#endif  // MY_MAP_H_
