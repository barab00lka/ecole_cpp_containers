#include <gtest/gtest.h>

#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../include/my_multiset.h"

using my::Multiset;
using std::string;

template <typename T>
void FillBoth(Multiset<T>& s, std::multiset<T>& ref,
              std::initializer_list<T> const& items) {
  for (const T& v : items) {
    s.insert(v);
    ref.insert(v);
  }
}

template <typename T>
void ExpectSameOrder(Multiset<T>& s, const std::multiset<T>& ref) {
  ASSERT_EQ(s.size(), ref.size());
  auto it = s.begin();
  auto rit = ref.begin();
  for (; rit != ref.end(); ++it, ++rit) EXPECT_EQ(*it, *rit);
  EXPECT_EQ(it, s.end());
}

TEST(Multiset, InsertKeepsSortedOrder) {
  Multiset<int> s;
  std::multiset<int> ref;
  FillBoth(s, ref, {2, 3, 4, 2, 3, 4, 2, 3, 4});
  ExpectSameOrder(s, ref);
}

TEST(Multiset, DefaultEmpty) {
  Multiset<int> s;
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
  EXPECT_EQ(s.begin(), s.end());
}

TEST(Multiset, InitializerListKeepsDuplicates) {
  Multiset<int> s{5, 1, 5, 3, 1, 5};
  std::multiset<int> ref{5, 1, 5, 3, 1, 5};
  ExpectSameOrder(s, ref);
  EXPECT_EQ(s.size(), 6u);
}

TEST(Multiset, SizeCountsDuplicates) {
  Multiset<int> s;
  std::multiset<int> ref;
  FillBoth(s, ref, {1, 1, 1, 2, 2});
  EXPECT_EQ(s.size(), 5u);
  EXPECT_EQ(s.size(), ref.size());
}

TEST(Multiset, ContainsAndFind) {
  Multiset<int> s;
  std::multiset<int> ref;
  FillBoth(s, ref, {4, 4, 2, 8});
  EXPECT_TRUE(s.contains(4));
  EXPECT_FALSE(s.contains(5));

  auto it = s.find(4);
  ASSERT_NE(it, s.end());
  EXPECT_EQ(*it, 4);
  EXPECT_EQ(s.find(7), s.end());
}

TEST(Multiset, LowerUpperBoundOnDuplicates) {
  Multiset<int> s;
  std::multiset<int> ref;
  FillBoth(s, ref, {1, 2, 2, 2, 3, 5});

  for (int q : {0, 1, 2, 3, 4, 5, 6}) {
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

TEST(Multiset, EraseOneOfDuplicatesLeavesRest) {
  Multiset<int> s;
  std::multiset<int> ref;
  FillBoth(s, ref, {2, 2, 2, 5});
  s.erase(s.find(2));  // remove a single 2, two must remain
  EXPECT_EQ(s.size(), 3u);
  int twos = 0;
  for (auto it = s.begin(); it != s.end(); ++it)
    if (*it == 2) ++twos;
  EXPECT_EQ(twos, 2);
}

TEST(Multiset, BackwardIteration) {
  Multiset<int> s;
  std::multiset<int> ref;
  FillBoth(s, ref, {7, 3, 3, 20, 3, 12});

  std::vector<int> got;
  auto it = s.end();
  do {
    --it;
    got.push_back(*it);
  } while (it != s.begin());

  std::vector<int> expected(ref.rbegin(), ref.rend());
  EXPECT_EQ(got, expected);
}

TEST(Multiset, CopyConstructorIsIndependent) {
  Multiset<int> a{1, 1, 2, 3};
  Multiset<int> b(a);
  EXPECT_EQ(b.size(), 4u);
  b.insert(9);
  EXPECT_EQ(a.size(), 4u);  // original untouched
  EXPECT_EQ(b.size(), 5u);
}

TEST(Multiset, MoveConstructorTransfersOwnership) {
  Multiset<int> a{1, 1, 2};
  Multiset<int> b(std::move(a));
  EXPECT_EQ(b.size(), 3u);
  EXPECT_TRUE(a.empty());
}

TEST(Multiset, CopyAndMoveAssignment) {
  Multiset<int> a{1, 2, 2};
  Multiset<int> b;
  b = a;
  EXPECT_EQ(b.size(), 3u);

  Multiset<int> c;
  c = std::move(a);
  EXPECT_EQ(c.size(), 3u);
  EXPECT_TRUE(a.empty());
}

TEST(Multiset, ClearEmptiesContainer) {
  Multiset<int> s{1, 1, 2, 3};
  s.clear();
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.begin(), s.end());
}

TEST(Multiset, StringKeysWithDuplicates) {
  Multiset<string> s;
  std::multiset<string> ref;
  FillBoth<string>(s, ref, {"b", "a", "b", "c", "a", "b"});
  EXPECT_EQ(s.size(), 6u);
  EXPECT_TRUE(s.contains("b"));
  EXPECT_FALSE(s.contains("z"));
  ExpectSameOrder(s, ref);
}

// multiset merge: unconditional move, source always ends up empty
TEST(Multiset, MergeMovesAllElements) {
  Multiset<int> s1, s2;
  std::multiset<int> ref1, ref2;
  FillBoth(s1, ref1, {1, 3, 5, 7, 9});
  FillBoth(s2, ref2, {2, 3, 6, 7, 10});

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  ExpectSameOrder(s1, ref1);

  EXPECT_TRUE(s2.empty());
  EXPECT_EQ(s2.size(), 0u);
  EXPECT_EQ(s2.begin(), s2.end());
}

TEST(Multiset, MergeKeepsDuplicateCounts) {
  Multiset<int> s1, s2;
  std::multiset<int> ref1, ref2;
  FillBoth(s1, ref1, {2, 2, 3});
  FillBoth(s2, ref2, {2, 2, 4});

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), 6u);
  EXPECT_EQ(s1.size(), ref1.size());
  ExpectSameOrder(s1, ref1);

  int twos = 0;
  for (auto it = s1.begin(); it != s1.end(); ++it)
    if (*it == 2) ++twos;
  EXPECT_EQ(twos, 4);

  EXPECT_TRUE(s2.empty());
}

TEST(Multiset, MergeEmptySource) {
  Multiset<int> s1{1, 2, 2, 3};
  Multiset<int> s2;
  std::multiset<int> ref1{1, 2, 2, 3};
  std::multiset<int> ref2;

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  ExpectSameOrder(s1, ref1);
  EXPECT_TRUE(s2.empty());
}

TEST(Multiset, MergeIntoEmpty) {
  Multiset<int> s1;
  Multiset<int> s2{4, 4, 1, 9};
  std::multiset<int> ref1;
  std::multiset<int> ref2{4, 4, 1, 9};

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  ExpectSameOrder(s1, ref1);
  EXPECT_TRUE(s2.empty());
}

TEST(Multiset, MergeBothEmpty) {
  Multiset<int> s1, s2;
  s1.merge(s2);
  EXPECT_TRUE(s1.empty());
  EXPECT_TRUE(s2.empty());
}

TEST(Multiset, MergeAllSameKey) {
  Multiset<int> s1{5, 5, 5};
  Multiset<int> s2{5, 5};
  std::multiset<int> ref1{5, 5, 5};
  std::multiset<int> ref2{5, 5};

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), 5u);
  ExpectSameOrder(s1, ref1);
  EXPECT_TRUE(s2.empty());
}

TEST(Multiset, MergeStringKeys) {
  Multiset<string> s1, s2;
  std::multiset<string> ref1, ref2;
  FillBoth<string>(s1, ref1, {"a", "b", "b"});
  FillBoth<string>(s2, ref2, {"b", "c", "a"});

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  ExpectSameOrder(s1, ref1);
  EXPECT_TRUE(s2.empty());
}

// self-merge: s.merge(s) — other is empty, this is destination, should
// be a no-op, not corrupt or infinite loop
TEST(Multiset, MergeSelfIsNoop) {
  Multiset<int> s{1, 2, 2, 3};
  std::multiset<int> ref{1, 2, 2, 3};
  s.merge(s);
  EXPECT_EQ(s.size(), 4u);
  ExpectSameOrder(s, ref);
}

TEST(Multiset, MergeRandomLarge) {
  Multiset<int> s1, s2;
  std::multiset<int> ref1, ref2;
  const int N = 5000;
  for (int i = 0; i < N; ++i) {
    int v1 = rand() % 500;
    int v2 = rand() % 500;
    s1.insert(v1);
    ref1.insert(v1);
    s2.insert(v2);
    ref2.insert(v2);
  }

  s1.merge(s2);
  ref1.merge(ref2);

  EXPECT_EQ(s1.size(), ref1.size());
  ExpectSameOrder(s1, ref1);
  EXPECT_TRUE(s2.empty());
  EXPECT_EQ(s2.size(), 0u);
}
