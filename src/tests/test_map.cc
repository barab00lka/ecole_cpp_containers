#include <gtest/gtest.h>

#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../include/my_map.h"

using my::Map;
using std::string;

// Walks both containers in order and checks that every key/value pair matches.
template <typename K, typename V>
void ExpectSameOrder(Map<K, V>& m, const std::map<K, V>& ref) {
  ASSERT_EQ(m.size(), ref.size());
  auto it = m.begin();
  auto rit = ref.begin();
  for (; rit != ref.end(); ++it, ++rit) {
    EXPECT_EQ(it->first, rit->first);
    EXPECT_EQ(it->second, rit->second);
  }
  EXPECT_EQ(it, m.end());
}

// ---- construction ---------------------------------------------------------
TEST(Map, DefaultEmpty) {
  Map<int, int> m;
  EXPECT_TRUE(m.empty());
  EXPECT_EQ(m.size(), 0u);
  EXPECT_EQ(m.begin(), m.end());
}

TEST(Map, InitializerListKeepsSortedUnique) {
  Map<int, string> m{{3, "c"}, {1, "a"}, {2, "b"}, {1, "dup"}};
  std::map<int, string> ref{{3, "c"}, {1, "a"}, {2, "b"}, {1, "dup"}};
  ExpectSameOrder(m, ref);
  EXPECT_EQ(m.size(), 3u);
  EXPECT_EQ(m.at(1), "a");  // first wins
}

TEST(Map, CopyConstructorIsIndependent) {
  Map<int, int> a{{1, 10}, {2, 20}, {3, 30}};
  Map<int, int> b(a);
  EXPECT_EQ(b.size(), 3u);
  b[2] = 99;
  EXPECT_EQ(a.at(2), 20);  // original untouched
  EXPECT_EQ(b.at(2), 99);
}

TEST(Map, MoveConstructorTransfersOwnership) {
  Map<int, int> a{{1, 10}, {2, 20}};
  Map<int, int> b(std::move(a));
  EXPECT_EQ(b.size(), 2u);
  EXPECT_EQ(b.at(1), 10);
  EXPECT_TRUE(a.empty());
}

TEST(Map, CopyAssignmentIsIndependent) {
  Map<int, int> a{{1, 1}, {2, 2}};
  Map<int, int> b{{9, 9}};
  b = a;
  EXPECT_EQ(b.size(), 2u);
  b[1] = 111;
  EXPECT_EQ(a.at(1), 1);
}

TEST(Map, MoveAssignmentTransfersOwnership) {
  Map<int, int> a{{1, 1}, {2, 2}};
  Map<int, int> b{{9, 9}};
  b = std::move(a);
  EXPECT_EQ(b.size(), 2u);
  EXPECT_EQ(b.at(2), 2);
  EXPECT_TRUE(a.empty());
}

// ---- element access -------------------------------------------------------
TEST(Map, AtReturnsAndModifies) {
  Map<int, int> m{{1, 10}, {2, 20}};
  EXPECT_EQ(m.at(1), 10);
  m.at(1) = 15;
  EXPECT_EQ(m.at(1), 15);
}

TEST(Map, AtThrowsOnMissingKey) {
  Map<int, int> m{{1, 10}};
  EXPECT_THROW(m.at(42), std::out_of_range);
}

TEST(Map, AtConstReadsAndThrows) {
  Map<int, int> m{{1, 10}, {2, 20}};
  const Map<int, int>& cm = m;
  EXPECT_EQ(cm.at(2), 20);
  EXPECT_THROW(cm.at(99), std::out_of_range);
}

TEST(Map, SubscriptInsertsDefaultThenReads) {
  Map<int, string> m;
  EXPECT_EQ(m[5], string{});  // inserts default value
  EXPECT_EQ(m.size(), 1u);
  m[5] = "five";
  EXPECT_EQ(m[5], "five");  // reaches existing element
  EXPECT_EQ(m.size(), 1u);
}

TEST(Map, SubscriptMatchesStdMap) {
  Map<string, int> m;
  std::map<string, int> ref;
  for (const char* const& w : {"pear", "apple", "pear", "kiwi", "apple"}) {
    ++m[w];
    ++ref[w];
  }
  ExpectSameOrder(m, ref);
  EXPECT_EQ(m["apple"], 2);
  EXPECT_EQ(m["pear"], 2);
  EXPECT_EQ(m["kiwi"], 1);
}

// ---- modifiers ------------------------------------------------------------
TEST(Map, InsertPairReturnsIteratorAndFlag) {
  Map<int, int> m;
  auto r1 = m.insert({1, 100});
  EXPECT_TRUE(r1.second);
  EXPECT_EQ(r1.first->second, 100);

  auto r2 = m.insert({1, 200});  // key exists -> no overwrite
  EXPECT_FALSE(r2.second);
  EXPECT_EQ(r2.first->second, 100);
  EXPECT_EQ(m.at(1), 100);
}

TEST(Map, InsertKeyValueOverload) {
  Map<int, string> m;
  auto r = m.insert(7, "seven");
  EXPECT_TRUE(r.second);
  EXPECT_EQ(m.at(7), "seven");
  EXPECT_FALSE(m.insert(7, "other").second);
  EXPECT_EQ(m.at(7), "seven");
}

TEST(Map, InsertOrAssignInsertsThenOverwrites) {
  Map<int, string> m;
  auto r1 = m.insert_or_assign(1, "a");
  EXPECT_TRUE(r1.second);
  EXPECT_EQ(m.at(1), "a");

  auto r2 = m.insert_or_assign(1, "b");  // existing -> overwrite
  EXPECT_FALSE(r2.second);
  EXPECT_EQ(m.at(1), "b");
  EXPECT_EQ(m.size(), 1u);
}

// ---- lookup ---------------------------------------------------------------
TEST(Map, ContainsAndFind) {
  Map<int, int> m{{1, 1}, {3, 3}, {5, 5}};
  EXPECT_TRUE(m.contains(3));
  EXPECT_FALSE(m.contains(4));

  auto it = m.find(5);
  ASSERT_NE(it, m.end());
  EXPECT_EQ(it->second, 5);
  EXPECT_EQ(m.find(2), m.end());
}

TEST(Map, LowerUpperBoundMatchStd) {
  Map<int, int> m;
  std::map<int, int> ref;
  for (int k : {2, 4, 6, 8, 10}) {
    m.insert({k, k});
    ref.insert({k, k});
  }
  for (int q : {1, 2, 3, 6, 10, 11}) {
    auto lb = m.lower_bound(q);
    auto rlb = ref.lower_bound(q);
    if (rlb == ref.end())
      EXPECT_EQ(lb, m.end()) << "lower_bound q=" << q;
    else
      EXPECT_EQ(lb->first, rlb->first) << "lower_bound q=" << q;

    auto ub = m.upper_bound(q);
    auto rub = ref.upper_bound(q);
    if (rub == ref.end())
      EXPECT_EQ(ub, m.end()) << "upper_bound q=" << q;
    else
      EXPECT_EQ(ub->first, rub->first) << "upper_bound q=" << q;
  }
}

// ---- iteration & bulk ops -------------------------------------------------
TEST(Map, ForwardAndBackwardIteration) {
  Map<int, int> m;
  std::map<int, int> ref;
  for (int k : {40, 10, 30, 20, 50}) {
    m.insert({k, k * 2});
    ref.insert({k, k * 2});
  }
  ExpectSameOrder(m, ref);

  std::vector<int> got;
  auto it = m.end();
  do {
    --it;
    got.push_back(it->first);
  } while (it != m.begin());
  std::vector<int> expected;
  for (auto rit = ref.rbegin(); rit != ref.rend(); ++rit)
    expected.push_back(rit->first);
  EXPECT_EQ(got, expected);
}

TEST(Map, EraseAndClear) {
  Map<int, int> m{{1, 1}, {2, 2}, {3, 3}};
  m.erase(m.find(2));
  EXPECT_EQ(m.size(), 2u);
  EXPECT_FALSE(m.contains(2));
  EXPECT_TRUE(m.contains(1));

  m.clear();
  EXPECT_TRUE(m.empty());
  EXPECT_EQ(m.begin(), m.end());
}

TEST(Map, SwapExchangesContents) {
  Map<int, int> a{{1, 1}, {2, 2}};
  Map<int, int> b{{9, 90}};
  a.swap(b);
  EXPECT_EQ(a.size(), 1u);
  EXPECT_EQ(a.at(9), 90);
  EXPECT_EQ(b.size(), 2u);
  EXPECT_EQ(b.at(1), 1);
}

TEST(Map, MaxSizePositive) {
  Map<int, int> m;
  EXPECT_GT(m.max_size(), 0u);
}
