#ifndef MY_AVLTREE_H_
#define MY_AVLTREE_H_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace my {

template <typename Key, typename Value = std::nullptr_t,
          typename Compare = std::less<Key>>
class Tree {
 public:
  using key_type = Key;
  using value_type = Value;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

 protected:  // accessible to derived classes (Map, Set, Multiset)
  template <typename K, typename V>
  struct TreeNode {
    // compile-time condition
    // if value_type of the Tree class is instantiated as a nullptr_t, means
    // that we have a Set, if not then we have Map
    using data_type =
        std::conditional_t<std::is_same_v<Tree::value_type, std::nullptr_t>,
                           Tree::key_type, std::pair<key_type, value_type>>;

    TreeNode* parent_ = nullptr;
    TreeNode* left_ = nullptr;
    TreeNode* right_ = nullptr;
    bool is_header_ = false;

    data_type data_ = data_type{};  // either a KV pair or a K
    /* Subtree height (leaf = 1, null child = 0). AVL tracks height, not
     * depth-from-root, which rotations would cost O(n) to maintain. */
    size_type height_ = 1;

    key_type key() const noexcept {
      if constexpr (std::is_same_v<Tree::value_type, std::nullptr_t>)
        return data_;
      else
        return data_.first;
    }
  };

 public:
  using node_type = TreeNode<key_type, value_type>;
  using reference = typename node_type::data_type&;
  using const_reference = const typename node_type::data_type&;
  using pointer = typename node_type::data_type*;
  using const_pointer = const typename node_type::data_type*;

 protected:
  static key_type key_of(const typename node_type::data_type& d) {
    if constexpr (std::is_same_v<Tree::value_type, std::nullptr_t>)
      return d;
    else
      return d.first;
  }

 public:
  template <bool IsConst>
  class TreeIterator {
    friend class Tree;
    template <bool>
    friend class TreeIterator;

    node_type* node_ = nullptr;

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    TreeIterator() = default;
    explicit TreeIterator(node_type* n) noexcept : node_(n) {}

    node_type* node() noexcept { return node_; }

    template <bool C, typename = std::enable_if_t<IsConst && !C>>
    TreeIterator(const TreeIterator<C>& other) : node_(other.node_) {}

    reference operator*() const { return node_->data_; }
    pointer operator->() const { return &node_->data_; }

    TreeIterator& operator++() {
      node_ = successor_of(node_);
      return *this;
    }
    TreeIterator operator++(int) {
      TreeIterator tmp = *this;
      ++*this;
      return tmp;
    }

    TreeIterator& operator--() {
      node_ = predecessor_of(node_);
      return *this;
    }
    TreeIterator operator--(int) {
      TreeIterator tmp = *this;
      --*this;
      return tmp;
    }

    friend bool operator==(const TreeIterator& a, const TreeIterator& b) {
      return a.node_ == b.node_;
    }
    friend bool operator!=(const TreeIterator& a, const TreeIterator& b) {
      return a.node_ != b.node_;
    }
  };

  using iterator = TreeIterator<false>;
  using const_iterator = TreeIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // Internal methods
 protected:
  node_type* Root() { return sentinel_.parent_; }

  // get the first node of parent's subtree
  // (leftmost descendant)
  static node_type* subtree_first(node_type* parent) {
    assert(parent);
    auto first = parent;
    while (first->left_ != nullptr) first = first->left_;
    return (first);
  }

  // get the last node of parent's subtree
  // (rightmost descendant)
  static node_type* subtree_last(node_type* parent) noexcept {
    assert(parent);
    auto last = parent;
    while (last->right_ != nullptr) last = (last->right_);
    return (last);
  }
  // returns the node that's next to x in traversal order or end() if not found
  static node_type* successor_of(node_type* x) noexcept {
    // base case: if we have right child
    // then the leftmost node of child subtree
    // is the successor
    if (x->right_ != nullptr) return subtree_first(x->right_);

    // go up one ancestor
    node_type* child = x;
    node_type* par = child->parent_;
    // until the node we're on
    // is a left child of some parent.
    // that parent node is the successor of x
    while (!par->is_header_ && par->left_ != child) {
      child = par;
      par = child->parent_;
    }
    return par;
  }
  // returns the node that preceeds x in the traversal order
  // or end() if not found
  static node_type* predecessor_of(node_type* x) {
    // rightmost node (special case if x is end())
    if (x->is_header_) return x->right_;

    if (x->left_ != nullptr) return subtree_last(x->left_);

    node_type* child = x;
    node_type* par = child->parent_;
    while (!par->is_header_ && par->right_ != child) {
      child = par;
      par = child->parent_;
    }
    return par;
  }

  // returns parent and its child. child is first matched node or end() if none
  // match
  std::pair<node_type*, node_type*> find_by_key(const key_type& key) {
    node_type* parent = &sentinel_;
    node_type* cur = Root();

    while (cur != nullptr) {
      parent = cur;
      if (comp_(key, cur->key())) {
        cur = cur->left_;
      } else if (comp_(cur->key(), key)) {  // data.key() > cur.key
        cur = cur->right_;
      } else {  // found match
        return {cur->parent_, cur};
      }
    }
    return {parent, nullptr};
  }
  /* First node strictly greater than key (upper_bound), or sentinel_ if none.
   * Each node greater than key becomes the best candidate and we look left for
   * a smaller one; otherwise we go right. */
  node_type* find_next(const key_type& key) noexcept {
    node_type* candidate = &sentinel_;
    node_type* cur = Root();
    while (cur != nullptr) {
      if (comp_(key, cur->key())) {
        candidate = cur;
        cur = cur->left_;
      } else {
        cur = cur->right_;
      }
    }
    return candidate;
  }

  // First node not less than key (lower_bound), or sentinel_ if none.
  // Like find_next, but a node == key also counts, so duplicates go leftmost.
  node_type* find_not_less(const key_type& key) noexcept {
    node_type* candidate = &sentinel_;
    node_type* cur = Root();
    while (cur != nullptr) {
      if (comp_(cur->key(), key)) {
        cur = cur->right_;
      } else {
        candidate = cur;
        cur = cur->left_;
      }
    }
    return candidate;
  }
  // ---- AVL balancing --------------------------------------------------------
  // height of a (possibly null) node, null == 0
  static size_type height_of(node_type* n) noexcept {
    return n == nullptr ? 0 : n->height_;
  }
  // recompute height from children (call after changing them)
  static void update_height(node_type* n) noexcept {
    n->height_ = 1 + std::max(height_of(n->left_), height_of(n->right_));
  }
  // >0 left-heavy, <0 right-heavy; AVL keeps it in {-1, 0, +1}
  static long balance_factor(node_type* n) noexcept {
    return static_cast<long>(height_of(n->left_)) -
           static_cast<long>(height_of(n->right_));
  }
  /* Re-point parent's link from old_child to new_child. For the header the
   * "child" is the root, held in sentinel_.parent_, so update that instead. */
  void reattach_child(node_type* parent, node_type* old_child,
                      node_type* new_child) noexcept {
    if (parent->is_header_)
      sentinel_.parent_ = new_child;
    else if (parent->left_ == old_child)
      parent->left_ = new_child;
    else
      parent->right_ = new_child;
  }
  /*        x                 y
   *       / \               / \
   *      a   y     -->      x   c
   *         / \            / \
   *        b   c          a   b
   */
  node_type* rotate_left(node_type* x) noexcept {
    node_type* y = x->right_;
    x->right_ = y->left_;
    if (y->left_ != nullptr) y->left_->parent_ = x;
    y->parent_ = x->parent_;
    reattach_child(x->parent_, x, y);
    y->left_ = x;
    x->parent_ = y;
    update_height(x);  // x sits lower now, recompute it first
    update_height(y);
    return y;
  }
  /*          x             y
   *         / \           / \
   *        y   c   -->   a   x
   *       / \               / \
   *      a   b             b   c
   */
  node_type* rotate_right(node_type* x) noexcept {
    node_type* y = x->left_;
    x->left_ = y->right_;
    if (y->right_ != nullptr) y->right_->parent_ = x;
    y->parent_ = x->parent_;
    reattach_child(x->parent_, x, y);
    y->right_ = x;
    x->parent_ = y;
    update_height(x);
    update_height(y);
    return y;
  }
  /* Walk from n up to the root, updating heights and applying the four AVL
   * rotations (LL/LR/RR/RL) where a node is unbalanced. Runs after insert and
   * erase. */
  void rebalance_from(node_type* n) noexcept {
    while (!n->is_header_) {
      update_height(n);
      long bf = balance_factor(n);
      if (bf > 1) {  // left subtree too tall
        if (balance_factor(n->left_) < 0) rotate_left(n->left_);  // LR case
        n = rotate_right(n);                                      // LL case
      } else if (bf < -1) {  // right subtree too tall
        if (balance_factor(n->right_) > 0) rotate_right(n->right_);  // RL case
        n = rotate_left(n);                                          // RR case
      }
      n = n->parent_;
    }
  }

  void subtree_delete(node_type* node) {
    assert(node != nullptr);
    assert(node != &sentinel_);
    /* Push node down to a leaf, then unlink it. Swap with the in-order
     * neighbour (successor, else predecessor) — an arbitrary child breaks BST
     * order. The value left in `repl` is discarded on delete. */
    while (!is_leaf(node)) {
      node_type* repl = (node->right_ != nullptr) ? subtree_first(node->right_)
                                                  : subtree_last(node->left_);
      std::swap(node->data_, repl->data_);
      node = repl;
    }
    node_type* parent = node->parent_;

    if (parent == &sentinel_) {
      size_ = 0;
      sentinel_.parent_ = nullptr;
      sentinel_.left_ = sentinel_.right_ = &sentinel_;
      delete node;
      return;
    }

    if (sentinel_.left_ == node) sentinel_.left_ = parent;
    if (sentinel_.right_ == node) sentinel_.right_ = parent;
    if (parent->left_ == node)
      parent->left_ = nullptr;
    else
      parent->right_ = nullptr;

    delete node;
    size_ -= 1;
    rebalance_from(parent);
  }

  // // inserts node right after node pos in order
  // iterator subtree_insert_after(iterator pos, node_type* new_node) {
  //   if (size_ == max_size()) throw std::length_error("Tree: max size
  //   reached"); node_type* parent = pos.node();
  //
  //   if (parent == &sentinel_) {
  //     if (Root() == nullptr) {
  //       sentinel_.parent_ = new_node;
  //       new_node->parent_ = &sentinel_;
  //       sentinel_.left_ = sentinel_.right_ = new_node;
  //     } else {
  //       node_type* first = subtree_first(sentinel_.parent_);
  //       first->left_ = new_node;
  //       new_node->parent_ = first;
  //       sentinel_.left_ = new_node;
  //     }
  //     // base case
  //   } else if (parent->right_ == nullptr) {
  //     parent->right_ = new_node;
  //     new_node->parent_ = parent;
  //     if (sentinel_.right_ == parent) sentinel_.right_ = new_node;
  //   } else {
  //     // next inorder after parent
  //     parent = successor_of(parent);
  //     parent->left_ = new_node;
  //     new_node->parent_ = parent;
  //   }
  //   size_ += 1;
  //   rebalance_from(parent);
  //   return iterator(new_node);
  // }

  // for set and map insertions (unique keys). Rejects duplicates.
  std::pair<iterator, bool> insert_uniq(
      const typename node_type::data_type& data) {
    if (size_ == max_size()) throw std::length_error("Tree: max size reached");
    key_type k = key_of(data);
    node_type* parent = &sentinel_;
    node_type* cur = Root();
    bool go_left = true;
    while (cur != nullptr) {
      parent = cur;
      key_type ck = cur->key();
      if (comp_(k, ck)) {  // k < cur.key
        cur = cur->left_;
        go_left = true;
      } else if (comp_(ck, k)) {  // k > cur.key
        cur = cur->right_;
        go_left = false;
      } else {
        return {iterator(cur), false};  // key already present
      }
    }
    node_type* node = new node_type{.data_ = data};  // height_ defaults to 1
    node->parent_ = parent;
    if (Root() == nullptr) {  // empty tree
      sentinel_.parent_ = node;
      sentinel_.left_ = node;
      sentinel_.right_ = node;
    } else if (go_left) {
      parent->left_ = node;
      if (parent == sentinel_.left_) sentinel_.left_ = node;
    } else {
      parent->right_ = node;
      if (parent == sentinel_.right_) sentinel_.right_ = node;
    }
    ++size_;
    rebalance_from(node->parent_);  // node survives rotations, iterator valid
    return {iterator(node), true};
  }
  // multiset and multimap insertions
  // duplicate keys get inserted as successors
  iterator insert(const typename node_type::data_type& data) {
    if (size_ == max_size()) throw std::length_error("Tree: max size reached");
    key_type k = key_of(data);
    node_type* node = new node_type{.data_ = data};  // height_ defaults to 1
    if (Root() == nullptr) {
      node->parent_ = &sentinel_;
      sentinel_.parent_ = sentinel_.left_ = sentinel_.right_ = node;
      size_ = 1;
      return iterator(node);
    }
    node_type* parent = &sentinel_;
    bool go_left = true;
    for (node_type* cur = Root(); cur != nullptr;) {
      parent = cur;
      if (comp_(k, cur->key())) {  // k < cur.key
        cur = cur->left_;
        go_left = true;
      } else {  // k >= cur.key : equal keys go to the right subtree
        cur = cur->right_;
        go_left = false;
      }
    }
    node->parent_ = parent;
    if (go_left) {
      parent->left_ = node;
      if (parent == sentinel_.left_) sentinel_.left_ = node;
    } else {
      parent->right_ = node;
      if (parent == sentinel_.right_) sentinel_.right_ = node;
    }
    size_ += 1;
    rebalance_from(node->parent_);
    return iterator(node);
  }

  void merge_uniq(Tree& other) {
    if (this == &other) return;
    node_type* cur =
        (other.Root() == nullptr) ? nullptr : subtree_first(other.Root());
    while (cur != nullptr) {
      key_type k = cur->key();

      auto [f_parent, f_child] = find_by_key(k);
      node_type* next = other.find_next(k);
      bool has_next = (next != &other.sentinel_);
      key_type next_key = has_next ? next->key() : key_type{};

      if (f_child == nullptr) {
        insert_uniq(cur->data_);
        other.subtree_delete(cur);
      }

      cur = has_next ? other.find_by_key(next_key).second : nullptr;
    }
  }

  void merge_multi(Tree& other) {
    if (this == &other) return;
    while (other.Root() != nullptr) {
      node_type* cur = subtree_first(other.Root());
      insert(cur->data_);
      other.subtree_delete(cur);
    }
  }

 private:
  void delete_children(node_type* parent) {
    if (parent == nullptr || parent->is_header_) return;
    if (parent->left_) delete_children(parent->left_);
    if (parent->right_) delete_children(parent->right_);
    delete parent;
  }

  /* Deep-copy the subtree at src under parent. Cloning the shape (vs
   * re-inserting) keeps the balanced layout and any duplicate keys. */
  node_type* clone_subtree(node_type* src, node_type* parent) {
    if (src == nullptr) return nullptr;
    node_type* n = new node_type{.data_ = src->data_};
    n->parent_ = parent;
    n->height_ = src->height_;
    n->left_ = clone_subtree(src->left_, n);
    n->right_ = clone_subtree(src->right_, n);
    return n;
  }

  bool is_leaf(const node_type* node) const {
    assert(node);
    return ((node->left_ == nullptr) && (node->right_ == nullptr));
  }

  static void exchange_links(node_type* x, node_type* y) {
    assert(x != nullptr);
    assert(y != nullptr);
    /* BEFORE:
     *      P              y.P
     *      |               |
     *      x               y
     *    /   \           /   \
     *  x.L   x.R       y.L   y.R
     *
     * AFTER:
     *     y.P             x.P
     *      |               |
     *      x               y
     *    /   \           /   \
     *  y.L   y.R       x.L    x.R
     */
    node_type* ptr = y->parent_;
    y->parent_ = x->parent_;
    x->parent_ = ptr;

    ptr = y->right_;
    y->right_ = x->right_;
    x->right_ = ptr;

    ptr = y->left_;
    y->left_ = x->left_;
    x->left_ = ptr;
  }

  // STL API methods
 public:
  // Default ctor
  // sentinel_.parent_ points to the Root,
  // but for empty tree it stays equal to nullptr
  Tree() noexcept
      : sentinel_(
            {.left_ = &sentinel_, .right_ = &sentinel_, .is_header_ = true}) {}

  // Copy: deep-clone the structure, then re-derive the min/max pointers
  Tree(const Tree& other)
      : sentinel_(
            {.left_ = &sentinel_, .right_ = &sentinel_, .is_header_ = true}),
        size_(0),
        comp_(other.comp_) {
    if (other.sentinel_.parent_ != nullptr) {
      sentinel_.parent_ = clone_subtree(other.sentinel_.parent_, &sentinel_);
      sentinel_.left_ = subtree_first(sentinel_.parent_);
      sentinel_.right_ = subtree_last(sentinel_.parent_);
      size_ = other.size_;
    }
  }

  /* Move: start empty, then swap. swap() re-anchors each root to its own
   * (by-value) sentinel. */
  Tree(Tree&& other) noexcept
      : sentinel_(
            {.left_ = &sentinel_, .right_ = &sentinel_, .is_header_ = true}) {
    swap(other);
  }

  // copy-and-swap: strong exception safety, self-assignment safe
  Tree& operator=(const Tree& other) {
    if (this != &other) {
      Tree tmp(other);
      swap(tmp);
    }
    return *this;
  }
  Tree& operator=(Tree&& other) noexcept {
    if (this != &other) {
      clear();
      swap(other);
    }
    return *this;
  }

  // dtor
  ~Tree() { clear(); }

  [[nodiscard]] size_type size() const { return size_; }
  [[nodiscard]] size_type max_size() const {
    return std::numeric_limits<size_type>::max() / sizeof(node_type);
  }
  [[nodiscard]] bool empty() const {
    // if (size_ == 0) assert(sentinel_.parent_ == nullptr);
    return size_ == 0;
  }

  [[nodiscard]] iterator begin() noexcept { return iterator(sentinel_.left_); }
  [[nodiscard]] iterator end() noexcept { return iterator(&sentinel_); }

  [[nodiscard]] const_iterator begin() const noexcept {
    return const_iterator(sentinel_.left_);
  }
  [[nodiscard]] const_iterator end() const noexcept {
    return const_iterator(&sentinel_);
  }

  // [[nodiscard]] const_iterator cbegin() const noexcept{ return begin(); }
  // [[nodiscard]] const_iterator cend() const noexcept{ return end(); }

  void erase(iterator pos) { subtree_delete(pos.node()); }

  void clear() {
    if (empty()) return;
    delete_children(Root());
    sentinel_.parent_ = nullptr;
    sentinel_.left_ = sentinel_.right_ = &sentinel_;
    size_ = 0;
  }

  /* Invalidates end() (sentinel address moves); element iterators stay valid,
   * the nodes don't move. */
  void swap(Tree& other) noexcept {
    // sentinel is by-value: swap its three links + bookkeeping...
    std::swap(sentinel_.parent_, other.sentinel_.parent_);
    std::swap(sentinel_.left_, other.sentinel_.left_);
    std::swap(sentinel_.right_, other.sentinel_.right_);
    std::swap(size_, other.size_);
    std::swap(comp_, other.comp_);
    /* ...then re-anchor each root to its own sentinel (address unchanged); an
     * empty tree points begin()/end() at itself. */
    if (sentinel_.parent_ != nullptr)
      sentinel_.parent_->parent_ = &sentinel_;
    else
      sentinel_.left_ = sentinel_.right_ = &sentinel_;

    if (other.sentinel_.parent_ != nullptr)
      other.sentinel_.parent_->parent_ = &other.sentinel_;
    else
      other.sentinel_.left_ = other.sentinel_.right_ = &other.sentinel_;
  }

  iterator upper_bound(const key_type& k) {
    return iterator(this->find_next(k));
  }

  // First element not less than k (leftmost match, so equal_range spans dups).
  iterator lower_bound(const key_type& k) {
    return iterator(this->find_not_less(k));
  }
  // finds an element with a specific key
  iterator find(const key_type& key) {
    auto [_, child] = this->find_by_key(key);
    return child == nullptr ? end() : iterator(child);  // found child or end()
  }
  // checks if the container contains an element with a specific key
  bool contains(const key_type& key) {
    auto [_, child] = this->find_by_key(key);
    return child == nullptr ? false : true;
  }

 protected:
  // data members
  node_type sentinel_;
  size_type size_ = 0;
  Compare comp_;
};

}  // namespace my

#endif
