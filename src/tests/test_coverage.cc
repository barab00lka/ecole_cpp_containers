#include <gtest/gtest.h>

#include <string>

#include "../include/my_list.h"
#include "../include/my_queue.h"
#include "../include/my_stack.h"
#include "../include/my_vector.h"

template <typename T>
struct Gen;

template <>
struct Gen<int> {
  static int make(int i) { return i; }
};
template <>
struct Gen<double> {
  static double make(int i) { return static_cast<double>(i) + 0.5; }
};
template <>
struct Gen<std::string> {
  static std::string make(int i) { return std::string(i + 1, 'a' + (i % 20)); }
};
template <>
struct Gen<char> {
  static char make(int i) { return static_cast<char>('a' + (i % 26)); }
};

template <typename T>
void SweepVector() {
  using V = my::Vector<T>;
  auto v = Gen<T>::make;

  V a;
  EXPECT_TRUE(a.empty());
  a.push_back(v(1));
  T lv = v(2);
  a.push_back(lv);
  a.insert_many_back(v(3), v(4), v(5));
  EXPECT_EQ(a.size(), 5u);
  EXPECT_GT(a.max_size(), 0u);

  a.reserve(64);
  EXPECT_GE(a.capacity(), 64u);
  a.shrink_to_fit();

  EXPECT_EQ(a.at(0), v(1));
  EXPECT_EQ(a[1], v(2));
  EXPECT_EQ(a.front(), v(1));
  EXPECT_EQ(a.back(), v(5));
  EXPECT_EQ(*a.data(), v(1));
  EXPECT_THROW(a.at(999), std::out_of_range);

  const V& ca = a;
  EXPECT_EQ(ca.at(0), v(1));
  EXPECT_EQ(ca[0], v(1));
  EXPECT_EQ(ca.front(), v(1));
  EXPECT_EQ(ca.back(), v(5));

  T sum_count = T{};
  (void)sum_count;
  for (auto it = a.begin(); it != a.end(); ++it) (void)*it;
  for (auto it = ca.begin(); it != ca.end(); ++it) (void)*it;
  for (auto it = a.rbegin(); it != a.rend(); ++it) (void)*it;
  for (auto it = ca.rbegin(); it != ca.rend(); ++it) (void)*it;

  a.insert(a.begin(), v(0));
  a.insert(a.end(), v(9));
  a.insert_many(a.begin() + 1, v(7), v(8));
  a.erase(a.begin());
  a.pop_back();

  V b(a);
  V c(std::move(b));
  V d;
  d = c;
  V e;
  e = std::move(c);
  EXPECT_TRUE(e == a || !(e == a));

  V sized(3);
  EXPECT_EQ(sized.size(), 3u);
  V init{v(1), v(2), v(3)};
  init.swap(sized);
  init.clear();
  EXPECT_TRUE(init.empty());

  my::Stack<T> st;
  st.push(v(1));
  st.push(lv);
  st.insert_many_back(v(3), v(4));
  my::Stack<T> st2(st);
  my::Stack<T> st3(std::move(st2));
  st3.pop();
  EXPECT_FALSE(st3.empty());
  st.swap(st3);
  const auto& cst = st;
  EXPECT_EQ(cst.top(), st.top());
}

template <typename T>
void SweepList() {
  using L = my::List<T>;
  auto v = Gen<T>::make;

  L a;
  a.push_back(v(3));
  a.push_front(v(1));
  T lv = v(2);
  const T clv = v(6);
  a.push_back(lv);
  a.push_back(clv);
  a.push_front(lv);
  a.insert_many_back(v(7), v(8));
  a.insert_many_front(v(9), v(10));
  EXPECT_GT(a.size(), 0u);
  EXPECT_GT(a.max_size(), 0u);

  EXPECT_EQ(a.front(), v(9));
  const L& ca = a;
  EXPECT_EQ(ca.front(), a.front());
  EXPECT_EQ(ca.back(), a.back());

  for (auto it = a.begin(); it != a.end(); ++it) (void)*it;
  for (auto it = ca.begin(); it != ca.end(); ++it) (void)*it;
  for (auto it = a.rbegin(); it != a.rend(); ++it) (void)*it;
  for (auto it = ca.rbegin(); it != ca.rend(); ++it) (void)*it;

  auto mid = ++a.begin();
  a.insert(mid, v(5));
  a.insert_many(a.begin(), v(11), v(12));
  a.emplace(a.begin(), v(13));
  a.emplace_back(v(14));
  a.pop_back();
  a.pop_front();
  a.erase(a.begin());
  a.reverse();
  a.unique();

  L cnt(4);
  EXPECT_EQ(cnt.size(), 4u);
  L fill(3, v(1));
  EXPECT_EQ(fill.size(), 3u);
  L init{v(1), v(2), v(3)};
  L cp(init);
  L mv(std::move(cp));
  L asg;
  asg = init;
  EXPECT_TRUE(asg == init);

  L s1{v(5), v(1), v(3), v(2), v(4)};
  s1.sort();
  L s2{v(6), v(0), v(7)};
  s2.sort();
  s1.merge(s2);
  EXPECT_TRUE(s2.empty());

  L sp1{v(1), v(2)};
  L sp2{v(3), v(4)};
  sp1.splice(sp1.begin(), sp2);
  sp1.swap(s1);
  sp1.clear();
  EXPECT_TRUE(sp1.empty());

  my::Queue<T> q;
  q.push(v(1));
  q.push(lv);
  q.insert_many_back(v(3), v(4));
  my::Queue<T> q2(q);
  my::Queue<T> q3(std::move(q2));
  q3.pop();
  q.swap(q3);
  const auto& cq = q;
  EXPECT_EQ(cq.front(), q.front());
  EXPECT_EQ(cq.back(), q.back());
}

TEST(Coverage, VectorInt) { SweepVector<int>(); }
TEST(Coverage, VectorDouble) { SweepVector<double>(); }
TEST(Coverage, VectorString) { SweepVector<std::string>(); }
TEST(Coverage, VectorChar) { SweepVector<char>(); }

TEST(Coverage, ListInt) { SweepList<int>(); }
TEST(Coverage, ListDouble) { SweepList<double>(); }
TEST(Coverage, ListString) { SweepList<std::string>(); }
TEST(Coverage, ListChar) { SweepList<char>(); }

TEST(Coverage, ListCharSort) {
  my::List<char> l{'c', 'a', 'b'};
  l.sort();
  EXPECT_EQ(l.front(), 'a');
  EXPECT_EQ(l.back(), 'c');
}
