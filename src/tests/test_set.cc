#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

#include "../include/my_set.h"

using my::Set;
using std::string;

template <typename T>
void FillBoth(Set<T>& s, std::set<T>& ref,
              std::initializer_list<T> const& items) {
  for (const T& v : items) {
    s.insert(v);
    ref.insert(v);
  }
}

template <typename T>
void ExpectSameOrder(Set<T>& s, const std::set<T>& ref) {
  ASSERT_EQ(s.size(), ref.size());
  auto it = s.begin();
  auto rit = ref.begin();
  for (; rit != ref.end(); ++it, ++rit) EXPECT_EQ(*it, *rit);
  EXPECT_EQ(it, s.end());
}

TEST(Set, DefaultEmpty) {
  Set<int> s;
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
  EXPECT_EQ(s.begin(), s.end());
}

TEST(Set, MaxSizePositive) {
  Set<int> s;
  EXPECT_GT(s.max_size(), 0u);
}

TEST(Set, InsertReturnsIteratorAndFlag) {
  Set<int> s;
  auto r1 = s.insert(5);
  EXPECT_TRUE(r1.second);
  EXPECT_EQ(*r1.first, 5);
  EXPECT_EQ(s.size(), 1u);

  auto r2 = s.insert(5);
  EXPECT_FALSE(r2.second);
  EXPECT_EQ(*r2.first, 5);
  EXPECT_EQ(s.size(), 1u);
}

TEST(Set, InsertKeepsSortedOrder) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {7, 3, 5, 20, 2, 12, 15, 8, 1});
  ExpectSameOrder(s, ref);
}

TEST(Set, InsertManyDuplicates) {
  Set<int> s;
  for (int i = 0; i < 100; ++i) s.insert(i % 10);
  EXPECT_EQ(s.size(), 10u);
  int expected = 0;
  for (auto it = s.begin(); it != s.end(); ++it) EXPECT_EQ(*it, expected++);
}

TEST(Set, Contains) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {1, 4, 3});
  EXPECT_FALSE(s.contains(5));
  EXPECT_TRUE(s.contains(3));
  EXPECT_TRUE(s.contains(1));
  EXPECT_EQ(s.size(), 3u);
}

TEST(Set, ContainsOnEmpty) {
  Set<int> s;
  EXPECT_FALSE(s.contains(42));
}

TEST(Set, Find) {
  Set<int> s;
  s.insert(1);
  s.insert(3);
  s.insert(4);

  EXPECT_EQ(s.find(2), s.end());

  auto it = s.find(4);
  ASSERT_NE(it, s.end());
  EXPECT_EQ(*it, 4);
}

TEST(Set, FindOnEmpty) {
  Set<int> s;
  EXPECT_EQ(s.find(1), s.end());
}

TEST(Set, ForwardIteration) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {7, 3, 5, 20, 2, 12, 15});
  ExpectSameOrder(s, ref);
}

TEST(Set, BackwardIteration) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {7, 3, 5, 20, 2, 12, 15});

  std::vector<int> got;
  auto it = s.end();
  do {
    --it;
    got.push_back(*it);
  } while (it != s.begin());

  std::vector<int> expected(ref.rbegin(), ref.rend());
  EXPECT_EQ(got, expected);
}

TEST(Set, LastElementViaDecrementEnd) {
  Set<int> s;
  s.insert(2);
  s.insert(9);
  s.insert(5);
  EXPECT_EQ(*(--s.end()), 9);
}

TEST(Set, PostfixIncrement) {
  Set<int> s;
  s.insert(1);
  s.insert(2);
  auto it = s.begin();
  auto old = it++;
  EXPECT_EQ(*old, 1);
  EXPECT_EQ(*it, 2);
}

TEST(Set, LowerUpperBoundMatchStd) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {2, 3, 5, 7, 12, 15, 20});

  for (int q : {0, 1, 2, 3, 4, 5, 11, 12, 16, 20, 21}) {
    auto lb = s.lower_bound(q);
    auto rlb = ref.lower_bound(q);
    if (rlb == ref.end())
      EXPECT_EQ(lb, s.end()) << "lower_bound q=" << q;
    else
      EXPECT_EQ(*lb, *rlb) << "lower_bound q=" << q;

    auto ub = s.upper_bound(q);
    auto rub = ref.upper_bound(q);
    if (rub == ref.end())
      EXPECT_EQ(ub, s.end()) << "upper_bound q=" << q;
    else
      EXPECT_EQ(*ub, *rub) << "upper_bound q=" << q;
  }
}

TEST(Set, UpperBoundAtMaxIsEnd) {
  Set<int> s;
  s.insert(1);
  s.insert(4);
  s.insert(3);
  EXPECT_EQ(s.upper_bound(4), s.end());
  EXPECT_EQ(s.upper_bound(1), ++s.begin());
}

TEST(Set, LowerBoundEqualAndGreater) {
  Set<int> s;
  s.insert(5);
  s.insert(12);
  s.insert(15);

  EXPECT_EQ(*s.lower_bound(5), 5);

  EXPECT_EQ(*s.lower_bound(14), 15);

  EXPECT_EQ(s.lower_bound(17), s.end());
}

TEST(Set, StringKeys) {
  Set<string> s;
  std::set<string> ref;
  FillBoth<string>(s, ref, {"banana", "apple", "cherry", "apple", "date"});
  EXPECT_EQ(s.size(), 4u);
  EXPECT_TRUE(s.contains("cherry"));
  EXPECT_FALSE(s.contains("fig"));
  ExpectSameOrder(s, ref);
}

TEST(Set, EraseMiddle) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {2, 3, 5, 7, 12, 15, 20});
  s.erase(++(++s.begin()));
  ref.erase(++(++ref.begin()));

  std::vector<int> got;
  auto it = s.end();
  do {
    --it;
    got.push_back(*it);
  } while (it != s.begin());

  std::vector<int> expected(ref.rbegin(), ref.rend());
  EXPECT_EQ(got, expected);
}

TEST(Set, Merge) {
  Set<int> s1, s2;
  std::set<int> ref1, ref2;

  std::vector<int> data1 = {1, 3, 5, 7, 9};
  std::vector<int> data2 = {2, 3, 6, 7, 10};

  for (int x : data1) {
    s1.insert(x);
    ref1.insert(x);
  }
  for (int x : data2) {
    s2.insert(x);
    ref2.insert(x);
  }

  // Выполняем слияние
  s1.merge(s2);
  ref1.merge(ref2);

  // Проверяем, что s1 теперь содержит все уникальные элементы из обоих
  EXPECT_EQ(s1.size(), ref1.size());
  auto it1 = s1.begin();
  auto it_ref1 = ref1.begin();
  while (it1 != s1.end() && it_ref1 != ref1.end()) {
    EXPECT_EQ(*it1, *it_ref1);
    ++it1;
    ++it_ref1;
  }
  EXPECT_EQ(it1, s1.end());
  EXPECT_EQ(it_ref1, ref1.end());

  // Проверяем, что в s2 остались только дубликаты (те, что были в s1)
  EXPECT_EQ(s2.size(), ref2.size());
  auto it2 = s2.begin();
  auto it_ref2 = ref2.begin();
  while (it2 != s2.end() && it_ref2 != ref2.end()) {
    EXPECT_EQ(*it2, *it_ref2);
    ++it2;
    ++it_ref2;
  }
  EXPECT_EQ(it2, s2.end());
  EXPECT_EQ(it_ref2, ref2.end());
}

TEST(Set, MergeEmpty) {
  Set<int> s1 = {1, 2, 3};
  Set<int> s2;
  std::set<int> ref1 = {1, 2, 3};
  std::set<int> ref2;

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  EXPECT_TRUE(std::equal(s1.begin(), s1.end(), ref1.begin()));
  EXPECT_EQ(s2.size(), ref2.size());
  EXPECT_TRUE(s2.empty());
}

TEST(Set, MergeAllDuplicates) {
  Set<int> s1 = {1, 2, 3};
  Set<int> s2 = {1, 2, 3};
  std::set<int> ref1 = {1, 2, 3};
  std::set<int> ref2 = {1, 2, 3};

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  EXPECT_TRUE(std::equal(s1.begin(), s1.end(), ref1.begin()));
  EXPECT_EQ(s2.size(), ref2.size());
  EXPECT_TRUE(std::equal(s2.begin(), s2.end(), ref2.begin()));
}
// Тест: слияние двух непересекающихся множеств
TEST(Set, MergeDisjoint) {
  Set<int> s1 = {1, 3, 5};
  Set<int> s2 = {2, 4, 6};
  std::set<int> ref1 = {1, 3, 5};
  std::set<int> ref2 = {2, 4, 6};

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  EXPECT_TRUE(std::equal(s1.begin(), s1.end(), ref1.begin()));
  EXPECT_TRUE(s2.empty());
  EXPECT_EQ(s2.size(), 0u);
}

// Тест: слияние, где s2 является подмножеством s1
TEST(Set, MergeSubset) {
  Set<int> s1 = {1, 2, 3, 4, 5};
  Set<int> s2 = {2, 4};
  std::set<int> ref1 = {1, 2, 3, 4, 5};
  std::set<int> ref2 = {2, 4};

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  EXPECT_TRUE(std::equal(s1.begin(), s1.end(), ref1.begin()));
  EXPECT_EQ(s2.size(), ref2.size());
  EXPECT_TRUE(std::equal(s2.begin(), s2.end(), ref2.begin()));
  // s2 должен остаться неизменным, т.к. все его элементы уже есть в s1
}

// Тест: слияние с большим количеством случайных элементов (проверка
// устойчивости)
TEST(Set, MergeRandomLarge) {
  Set<int> s1, s2;
  std::set<int> ref1, ref2;

  const int N = 10000;
  std::vector<int> vals1(N), vals2(N);
  std::generate(vals1.begin(), vals1.end(), []() { return rand() % 100000; });
  std::generate(vals2.begin(), vals2.end(), []() { return rand() % 100000; });

  for (int v : vals1) {
    s1.insert(v);
    ref1.insert(v);
  }
  for (int v : vals2) {
    s2.insert(v);
    ref2.insert(v);
  }

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  EXPECT_TRUE(std::equal(s1.begin(), s1.end(), ref1.begin()));

  // В s2 должны остаться только дубликаты (те, что были в ref1)
  EXPECT_EQ(s2.size(), ref2.size());
  EXPECT_TRUE(std::equal(s2.begin(), s2.end(), ref2.begin()));
}

TEST(Set, EraseLeaf) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {5, 3, 8, 1});
  s.erase(s.find(1));  // 1 is a leaf
  ref.erase(1);
  EXPECT_FALSE(s.contains(1));
  ExpectSameOrder(s, ref);
}

TEST(Set, EraseNodeWithTwoChildren) {
  // two children -> successor-swap branch
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {5, 3, 8, 2, 4, 7, 9});
  s.erase(s.find(5));
  ref.erase(5);
  EXPECT_FALSE(s.contains(5));
  EXPECT_EQ(s.size(), 6u);
  ExpectSameOrder(s, ref);
}

TEST(Set, EraseNodeWithOneChild) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {5, 3, 8, 9});  // 8 has a single right child (9)
  s.erase(s.find(8));
  ref.erase(8);
  EXPECT_FALSE(s.contains(8));
  EXPECT_TRUE(s.contains(9));
  ExpectSameOrder(s, ref);
}

TEST(Set, EraseSingleNodeMakesEmpty) {
  Set<int> s;
  s.insert(42);
  s.erase(s.begin());  // only node -> tree resets to empty
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
  EXPECT_EQ(s.begin(), s.end());
}

TEST(Set, EraseEveryElementDownToEmpty) {
  Set<int> s;
  std::set<int> ref;
  FillBoth(s, ref, {5, 3, 8, 2, 4, 7, 9, 1, 6});
  while (!s.empty()) {
    s.erase(s.begin());
    ref.erase(ref.begin());
    ExpectSameOrder(s, ref);
  }
  EXPECT_TRUE(s.empty());
}
