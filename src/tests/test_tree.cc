#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../include/my_avltree.h"

using my::Tree;
using std::string;

class TestTree : public Tree<int> {
 public:
  using Tree::find_by_key;
  using Tree::find_next;
  // using Tree::find_prev;
  using Tree::insert_uniq;
};

TestTree t;

TEST(BaseBST, Insert) {
  t.insert_uniq(7);
  t.insert_uniq(3);
  t.insert_uniq(5);
  t.insert_uniq(20);
  t.insert_uniq(2);
  EXPECT_EQ(*(t.begin()), 2);
  EXPECT_EQ(*(++t.begin()), 3);
}

TEST(BaseBST, FindByKey) {
  EXPECT_EQ(t.find_by_key(3).second->key(), *++t.begin());
  EXPECT_EQ(t.find_next(5)->key(), 7);
  EXPECT_EQ(t.find_next(3)->key(), 5);
  EXPECT_EQ(t.find_next(2)->key(), 3);
  EXPECT_EQ(t.find_next(7)->key(), 20);
}

TEST(BaseBST, EmptyAndSizeOnFresh) {
  TestTree tr;
  EXPECT_TRUE(tr.empty());
  EXPECT_EQ(tr.size(), 0u);
  EXPECT_EQ(tr.begin(), tr.end());
}

TEST(BaseBST, SizeGrowsOnInsert) {
  TestTree tr;
  tr.insert_uniq(10);
  tr.insert_uniq(20);
  tr.insert_uniq(30);
  EXPECT_EQ(tr.size(), 3u);
  EXPECT_FALSE(tr.empty());
}

TEST(BaseBST, MaxSizePositive) {
  TestTree tr;
  EXPECT_GT(tr.max_size(), 0u);
}

TEST(BaseBST, InsertReturnValue) {
  TestTree tr;
  auto r1 = tr.insert_uniq(42);
  EXPECT_TRUE(r1.second);
  EXPECT_EQ(*r1.first, 42);

  auto r2 = tr.insert_uniq(42);
  EXPECT_FALSE(r2.second);
  EXPECT_EQ(*r2.first, 42);
  EXPECT_EQ(tr.size(), 1u);
}

TEST(BaseBST, InorderTraversalSorted) {
  TestTree tr;
  for (int v : {50, 30, 70, 20, 40, 60, 80, 10}) tr.insert_uniq(v);
  std::vector<int> got;
  for (auto it = tr.begin(); it != tr.end(); ++it) got.push_back(*it);
  std::vector<int> expected{10, 20, 30, 40, 50, 60, 70, 80};
  EXPECT_EQ(got, expected);
}

TEST(BaseBSTIterator, PrefixPostfixIncrement) {
  TestTree tr;
  tr.insert_uniq(1);
  tr.insert_uniq(2);
  tr.insert_uniq(3);
  auto it = tr.begin();
  EXPECT_EQ(*it, 1);
  auto old = it++;
  EXPECT_EQ(*old, 1);
  EXPECT_EQ(*it, 2);
  EXPECT_EQ(*++it, 3);
}

TEST(BaseBSTIterator, DecrementFromEnd) {
  TestTree tr;
  for (int v : {5, 1, 9, 3, 7}) tr.insert_uniq(v);
  auto it = tr.end();
  --it;
  EXPECT_EQ(*it, 9);
}

TEST(BaseBSTIterator, FullBackwardWalk) {
  TestTree tr;
  for (int v : {5, 1, 9, 3, 7}) tr.insert_uniq(v);
  std::vector<int> got;
  auto it = tr.end();
  do {
    --it;
    got.push_back(*it);
  } while (it != tr.begin());
  std::vector<int> expected{9, 7, 5, 3, 1};
  EXPECT_EQ(got, expected);
}

TEST(BaseBSTIterator, PostfixDecrement) {
  TestTree tr;
  tr.insert_uniq(1);
  tr.insert_uniq(2);
  auto it = --tr.end();
  auto old = it--;
  EXPECT_EQ(*old, 2);
  EXPECT_EQ(*it, 1);
}

TEST(BaseBSTLookup, FindByKeyMissReturnsNULL) {
  TestTree tr;
  for (int v : {5, 3, 8}) tr.insert_uniq(v);
  EXPECT_EQ(tr.find_by_key(99).second, nullptr);
  EXPECT_EQ(tr.find_by_key(5).second->key(), 5);
}

TEST(BaseBSTLookup, FindNextChain) {
  TestTree tr;
  for (int v : {40, 20, 60, 10, 30, 50, 70}) tr.insert_uniq(v);
  EXPECT_EQ(tr.find_next(10)->key(), 20);
  EXPECT_EQ(tr.find_next(40)->key(), 50);
  EXPECT_EQ(tr.find_next(60)->key(), 70);

  EXPECT_EQ(tr.find_next(70), tr.end().node());
}

TEST(BaseBSTLookup, FindPrevChain) {
  TestTree tr;
  for (int v : {40, 20, 60, 10, 30, 50, 70}) tr.insert_uniq(v);

  // EXPECT_EQ(tr.find_prev(30)->key(), 30);
  // EXPECT_EQ(tr.find_prev(50)->key(), 50);
}

class TestTreeStrings : public Tree<string> {
 public:
  using Tree::find_by_key;
  using Tree::find_next;
  // using Tree::find_prev;
  using Tree::insert_uniq;
};
TEST(BaseBST, StringKeysSorted) {
  TestTreeStrings tr;
  for (const char* v : {"pear", "apple", "kiwi", "banana"}) tr.insert_uniq(v);
  std::vector<string> got;
  for (auto it = tr.begin(); it != tr.end(); ++it) got.push_back(*it);
  std::vector<string> expected{"apple", "banana", "kiwi", "pear"};
  EXPECT_EQ(got, expected);
}
