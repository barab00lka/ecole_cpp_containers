#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "../include/my_array.h"

using my::Array;
using std::string;

template <typename T>
string PrintArrayElements(const Array<T, 5>& l) {
  string s;

  for (auto i : l) {
    s += i;
    s += " ";
  }
  s += "\n";
  return s;
}

template <typename T>
string PrintStdArrayElements(const std::array<T, 5>& l) {
  string s;

  for (auto i : l) {
    s += i;
    s += " ";
  }
  s += "\n";
  return s;
}

template <typename T>
void CheckEqualByElem(const Array<T, 5>& actual,
                      const std::array<T, 5>& expected) {
  EXPECT_EQ(actual.size(), expected.size());
  auto it = actual.crbegin();
  auto sit = expected.crbegin();
  for (; sit != expected.crend(); ++it, ++sit) EXPECT_EQ(*it, *sit);

  if (testing::Test::HasFailure()) {
    std::cout << "Actual (from my::Array):\n  " << PrintArrayElements(actual)
              << "Expected (from std::array):\n  "
              << PrintStdArrayElements(expected) << std::endl;
  }
}

template <typename T>
void CheckEqualByElem(const Array<T, 100>& actual,
                      const Array<T, 100>& expected) {
  EXPECT_EQ(actual.size(), expected.size());
  auto it = actual.crbegin();
  auto sit = expected.crbegin();
  for (; sit != expected.crend(); ++it, ++sit) EXPECT_EQ(*it, *sit);

  if (testing::Test::HasFailure()) {
    std::cout << "Actual:\n  " << PrintArrayElements(actual) << "Expected:\n  "
              << PrintArrayElements(expected) << std::endl;
  }
}

TEST(Array, UnboundedAccessor) {
  auto ar = Array<int, 20>{};
  for (size_t i = 0; i < ar.size(); i++) {
    ar[i] = i;
    EXPECT_EQ(i, ar[i]);
  }
}

TEST(Array, BoundedAccess) {
  auto ar = Array<int, 20>{};
  EXPECT_THROW(ar.at(20), std::out_of_range);
}

TEST(Array, Empty) {
  auto ar = Array<string, 0>{};
  EXPECT_TRUE(ar.empty());
  auto ar2 = Array<string, 1>{};
  EXPECT_FALSE(ar2.empty());
}

TEST(Array, Fill) {
  auto ar = Array<string, 20>{};
  ar.fill("shrek");
  for (size_t i = 0; i < ar.size(); i++) {
    EXPECT_EQ("shrek", ar[i]);
  }
}

TEST(Array, AggregateInit) {
  auto ar = Array<string, 5>{"My", "name", "is", "Slim", "Shady"};

  auto exp = std::array<string, 5>{"My", "name", "is", "Slim", "Shady"};

  CheckEqualByElem(ar, exp);
}

TEST(Array, AtSuccessAndBounds) {
  Array<int, 3> ar{10, 20, 30};
  EXPECT_EQ(ar.at(0), 10);
  EXPECT_EQ(ar.at(2), 30);
  ar.at(1) = 99;
  EXPECT_EQ(ar[1], 99);
  EXPECT_THROW(ar.at(3), std::out_of_range);
}

TEST(Array, ConstAt) {
  const Array<int, 3> ar{1, 2, 3};
  EXPECT_EQ(ar.at(0), 1);
  EXPECT_EQ(ar.at(2), 3);
  EXPECT_THROW(ar.at(3), std::out_of_range);
}

TEST(Array, FrontBack) {
  Array<int, 4> ar{5, 6, 7, 8};
  EXPECT_EQ(ar.front(), 5);
  EXPECT_EQ(ar.back(), 8);
  ar.front() = 50;
  ar.back() = 80;
  EXPECT_EQ(ar[0], 50);
  EXPECT_EQ(ar[3], 80);

  const Array<int, 4> car{5, 6, 7, 8};
  EXPECT_EQ(car.front(), 5);
  EXPECT_EQ(car.back(), 8);
}

TEST(Array, Data) {
  Array<int, 3> ar{1, 2, 3};
  int* p = ar.data();
  EXPECT_EQ(p[0], 1);
  p[0] = 100;
  EXPECT_EQ(ar[0], 100);

  const Array<int, 3> car{1, 2, 3};
  const int* cp = car.data();
  EXPECT_EQ(cp[2], 3);
}

TEST(Array, MaxSizeEqualsSize) {
  Array<int, 7> ar{};
  EXPECT_EQ(ar.size(), 7u);
  EXPECT_GT(ar.max_size(), 0u);
}

TEST(Array, ForwardIteration) {
  Array<int, 5> ar{1, 2, 3, 4, 5};
  int expected = 1;
  for (auto it = ar.begin(); it != ar.end(); ++it) EXPECT_EQ(*it, expected++);

  const Array<int, 5>& car = ar;
  expected = 1;
  for (auto it = car.begin(); it != car.end(); ++it) EXPECT_EQ(*it, expected++);
}

TEST(Array, ReverseIteration) {
  Array<int, 4> ar{1, 2, 3, 4};
  std::array<int, 4> ref{1, 2, 3, 4};
  auto it = ar.rbegin();
  auto rit = ref.rbegin();
  for (; rit != ref.rend(); ++it, ++rit) EXPECT_EQ(*it, *rit);
  EXPECT_EQ(it, ar.rend());
}

TEST(Array, Swap) {
  Array<int, 3> a{1, 2, 3};
  Array<int, 3> b{4, 5, 6};
  a.swap(b);
  EXPECT_EQ(a[0], 4);
  EXPECT_EQ(a[2], 6);
  EXPECT_EQ(b[0], 1);
  EXPECT_EQ(b[2], 3);
}

TEST(Array, EqualityOperator) {
  Array<int, 3> a{1, 2, 3};
  Array<int, 3> b{1, 2, 3};
  Array<int, 3> c{1, 2, 4};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
}

TEST(Array, ConstIteration) {
  const Array<string, 5> a{"a", "b", "c", "d", "e"};
  string joined;
  for (auto it = a.begin(); it != a.end(); ++it) joined += *it;
  EXPECT_EQ(joined, "abcde");
}
