#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

#include "../include/my_vector.h"

template <typename T>
void PrintVector(const char* name, const auto& vec) {
  std::cout << name << ": [";
  for (size_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i];
    if (i + 1 < vec.size()) std::cout << ", ";
  }
  std::cout << "]" << std::endl;
}
template <typename T>
void ExpectVectorsEqual(const my::Vector<T>& actual,
                        const std::vector<T>& expected) {
  ASSERT_EQ(actual.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    if (actual[i] != expected[i]) {
      PrintVector<T>("actual", actual);
      PrintVector<T>("expected", expected);
    }
    EXPECT_EQ(actual[i], expected[i]) << "Mismatch at index " << i;
  }
}

template <typename T>
void ExpectVectorElements(const my::Vector<T>& actual,
                          const std::vector<T>& expected) {
  auto it_act = actual.cbegin();
  auto it_exp = expected.cbegin();
  for (; it_exp != expected.cend(); ++it_act, ++it_exp) {
    EXPECT_EQ(*it_act, *it_exp);
  }
  EXPECT_EQ(it_act, actual.cend());
}

TEST(VectorCompareTest, DefaultConstructor) {
  my::Vector<int> actual;
  std::vector<int> expected;
  EXPECT_EQ(actual.size(), expected.size());
  EXPECT_EQ(actual.capacity(), expected.capacity());
  EXPECT_TRUE(actual.empty());
}

TEST(VectorCompareTest, SizeConstructor) {
  size_t n = 5;
  my::Vector<int> actual(n);
  std::vector<int> expected(n);
  ExpectVectorsEqual(actual, expected);
  EXPECT_GE(actual.capacity(), actual.size());
}

TEST(VectorCompareTest, SizeConstructorWithZero) {
  my::Vector<int> actual(0);
  std::vector<int> expected(0);
  EXPECT_EQ(actual.size(), expected.size());
  EXPECT_EQ(actual.capacity(), expected.capacity());
}

TEST(VectorCompareTest, InitializerListConstructor) {
  my::Vector<int> actual{1, 2, 3, 4, 5};
  std::vector<int> expected{1, 2, 3, 4, 5};
  ExpectVectorsEqual(actual, expected);
}

TEST(VectorCompareTest, CopyConstructor) {
  my::Vector<int> original{1, 2, 3};
  std::vector<int> orig_std{1, 2, 3};
  my::Vector<int> actual(original);
  std::vector<int> expected(orig_std);
  ExpectVectorsEqual(actual, expected);

  ExpectVectorsEqual(original, orig_std);
  EXPECT_NE(actual.begin(), original.begin());
}

TEST(VectorCompareTest, MoveConstructor) {
  my::Vector<int> original{1, 2, 3};
  std::vector<int> orig_std{1, 2, 3};
  my::Vector<int> actual(std::move(original));
  std::vector<int> expected(std::move(orig_std));
  ExpectVectorsEqual(actual, expected);

  EXPECT_TRUE(original.empty());
  EXPECT_EQ(original.size(), 0u);

  EXPECT_TRUE(orig_std.empty());
}

TEST(VectorCompareTest, CopyAssignment) {
  my::Vector<int> original{10, 20, 30};
  std::vector<int> orig_std{10, 20, 30};
  my::Vector<int> actual;
  std::vector<int> expected;
  actual = original;
  expected = orig_std;
  ExpectVectorsEqual(actual, expected);
  ExpectVectorsEqual(original, orig_std);
}

TEST(VectorCompareTest, MoveAssignment) {
  my::Vector<int> original{10, 20, 30};
  std::vector<int> orig_std{10, 20, 30};
  my::Vector<int> actual;
  std::vector<int> expected;
  actual = std::move(original);
  expected = std::move(orig_std);
  ExpectVectorsEqual(actual, expected);
  EXPECT_TRUE(original.empty())
      << "Start_ address: " << original.data()
      << ", Fin_ address: " << (original.data() + original.size())
      << ", End_of_memory: " << (original.data() + original.capacity());
  EXPECT_TRUE(orig_std.empty());
}

TEST(VectorCompareTest, SubscriptOperator) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(actual[i], expected[i]);

    actual[i] = 100 + i;
    expected[i] = 100 + i;
  }
  ExpectVectorsEqual(actual, expected);
}

TEST(VectorCompareTest, AtMethod) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(actual.at(i), expected.at(i));
  }

  EXPECT_THROW(actual.at(3), std::out_of_range);
  EXPECT_THROW(void(expected.at(3)), std::out_of_range);
}

TEST(VectorCompareTest, FrontBack) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  EXPECT_EQ(actual.front(), expected.front());
  EXPECT_EQ(actual.back(), expected.back());

  actual.front() = 10;
  actual.back() = 30;
  expected.front() = 10;
  expected.back() = 30;
  ExpectVectorsEqual(actual, expected);
}

TEST(VectorCompareTest, Data) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  int* act_data = actual.data();
  int* exp_data = expected.data();
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(act_data[i], exp_data[i]);
  }
}

TEST(VectorCompareTest, Iterators) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  auto act_it = actual.begin();
  auto exp_it = expected.begin();
  for (; exp_it != expected.end(); ++act_it, ++exp_it) {
    EXPECT_EQ(*act_it, *exp_it);
  }
  EXPECT_EQ(act_it, actual.end());
}

TEST(VectorCompareTest, ReverseIterators) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  auto act_rit = actual.rbegin();
  auto exp_rit = expected.rbegin();
  for (; exp_rit != expected.rend(); ++act_rit, ++exp_rit) {
    EXPECT_EQ(*act_rit, *exp_rit);
  }
  EXPECT_EQ(act_rit, actual.rend());
}

TEST(VectorCompareTest, RangeBasedFor) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  size_t i = 0;
  for (int val : actual) {
    EXPECT_EQ(val, expected[i++]);
  }
  EXPECT_EQ(i, expected.size());
}

TEST(VectorCompareTest, Reserve) {
  my::Vector<int> actual;
  std::vector<int> expected;
  actual.reserve(10);
  expected.reserve(10);
  EXPECT_GE(actual.capacity(), 10u);
  EXPECT_GE(expected.capacity(), 10u);
  EXPECT_EQ(actual.size(), expected.size());
}

TEST(VectorCompareTest, ShrinkToFit) {
  my::Vector<int> actual;
  std::vector<int> expected;
  actual.reserve(100);
  expected.reserve(100);
  for (int i = 0; i < 10; ++i) {
    actual.push_back(i);
    expected.push_back(i);
  }
  actual.shrink_to_fit();
  expected.shrink_to_fit();
  EXPECT_EQ(actual.size(), expected.size());
  EXPECT_EQ(actual.capacity(), actual.size());
  ExpectVectorsEqual(actual, expected);
}

TEST(VectorCompareTest, PushBack) {
  my::Vector<int> actual;
  my::Vector<int> insert_back;
  std::vector<int> expected;
  for (int i = 0; i < 5; ++i) {
    actual.push_back(i);
    expected.push_back(i);
  }
  insert_back.insert_many_back(0, 1, 2, 3, 4);
  ExpectVectorsEqual(actual, expected);
  ExpectVectorsEqual(insert_back, expected);
}

TEST(VectorCompareTest, PopBack) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  actual.pop_back();
  expected.pop_back();
  ExpectVectorsEqual(actual, expected);
  actual.pop_back();
  expected.pop_back();
  actual.pop_back();
  expected.pop_back();
  EXPECT_TRUE(actual.empty());
  EXPECT_TRUE(expected.empty());
}

TEST(VectorCompareTest, InsertSingle) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};

  auto act_pos = actual.cbegin() + 1;
  auto exp_pos = expected.cbegin() + 1;
  actual.insert(act_pos, 10);
  expected.insert(exp_pos, 10);
  ExpectVectorsEqual(actual, expected);

  actual.insert(actual.cbegin(), 20);
  expected.insert(expected.cbegin(), 20);
  ExpectVectorsEqual(actual, expected);

  actual.insert(actual.cend(), 30);
  expected.insert(expected.cend(), 30);
  ExpectVectorsEqual(actual, expected);
}
TEST(VectorCompareTest, InsertMany) {
  int numbers[] = {67, 228, 69, 420};
  my::Vector<int> many{1, 2, 3};
  std::vector<int> expected{1, 67, 228, 69, 420, 2, 3};
  many.insert_many(many.cbegin() + 1, numbers[0], numbers[1], numbers[2],
                   numbers[3]);
  ExpectVectorsEqual(many, expected);
  expected.insert(expected.begin(), 11);
  expected.insert(expected.begin(), 10);
  expected.insert(expected.begin(), 9);
  many.insert_many(many.begin(), 9, 10, 11);
  ExpectVectorsEqual(many, expected);
}

TEST(VectorCompareTest, EraseSingle) {
  my::Vector<int> actual{1, 2, 3, 4};
  std::vector<int> expected{1, 2, 3, 4};

  auto act_pos = actual.begin() + 2;
  auto exp_pos = expected.begin() + 2;
  actual.erase(act_pos);
  expected.erase(exp_pos);
  ExpectVectorsEqual(actual, expected);

  actual.erase(actual.begin());
  expected.erase(expected.begin());
  ExpectVectorsEqual(actual, expected);

  actual.erase(actual.end() - 1);
  expected.erase(expected.end() - 1);
  ExpectVectorsEqual(actual, expected);
}

TEST(VectorCompareTest, Clear) {
  my::Vector<int> actual{1, 2, 3};
  std::vector<int> expected{1, 2, 3};
  actual.clear();
  expected.clear();
  EXPECT_EQ(actual.size(), expected.size());
  EXPECT_TRUE(actual.empty());
  EXPECT_TRUE(expected.empty());

  EXPECT_GE(actual.capacity(), 0u);
}

TEST(VectorCompareTest, SwapMethod) {
  my::Vector<int> v1{1, 2, 3};
  my::Vector<int> v2{4, 5};
  std::vector<int> w1{1, 2, 3};
  std::vector<int> w2{4, 5};
  v1.swap(v2);
  w1.swap(w2);
  ExpectVectorsEqual(v1, w1);
  ExpectVectorsEqual(v2, w2);
}

TEST(VectorCompareTest, StdSwapFunction) {
  my::Vector<int> v1{1, 2, 3};
  my::Vector<int> v2{4, 5};
  std::vector<int> w1{1, 2, 3};
  std::vector<int> w2{4, 5};
  std::swap(v1, v2);
  std::swap(w1, w2);
  ExpectVectorsEqual(v1, w1);
  ExpectVectorsEqual(v2, w2);
}

TEST(VectorCompareTest, SizeConstructorThrows) {
  size_t too_big = static_cast<size_t>(-1) / sizeof(int) + 1;
  EXPECT_THROW(my::Vector<int> v(too_big), std::length_error);

  EXPECT_THROW(std::vector<int> v(too_big), std::length_error);
}

TEST(VectorBenchmark, SpeedVsStd) {
  const size_t N = 1'000'000;

  auto start_std = std::chrono::high_resolution_clock::now();
  std::vector<int> std_vec;
  std_vec.reserve(N);
  for (size_t i = 0; i < N; ++i) {
    std_vec.push_back(i);
  }
  auto end_std = std::chrono::high_resolution_clock::now();
  auto dur_std =
      std::chrono::duration_cast<std::chrono::milliseconds>(end_std - start_std)
          .count();

  auto start_my = std::chrono::high_resolution_clock::now();
  my::Vector<int> my_vec;
  my_vec.reserve(N);
  for (size_t i = 0; i < N; ++i) {
    my_vec.push_back(i);
  }
  auto end_my = std::chrono::high_resolution_clock::now();
  auto dur_my =
      std::chrono::duration_cast<std::chrono::milliseconds>(end_my - start_my)
          .count();

  std::cout << "\n========== VECTOR BENCHMARK ==========\n";
  std::cout << "Elements: " << N << "\n";
  std::cout << "std::vector  : " << dur_std << " ms\n";
  std::cout << "my::Vector  : " << dur_my << " ms\n";
  std::cout << "Ratio (my/std): " << (double)dur_my / dur_std << "\n";
  std::cout << "=======================================\n";

  EXPECT_EQ(my_vec.size(), std_vec.size());
  for (size_t i = 0; i < N; ++i) {
    EXPECT_EQ(my_vec[i], std_vec[i]);
  }
}

TEST(VectorCapacity, ReserveSizeMax) {
  my::Vector<int> v(100);

  EXPECT_THROW(v.reserve(SIZE_MAX), std::length_error);
}

TEST(VectorCompareTest, AtConstThrowsOutOfRange) {
  const my::Vector<int> v{1, 2, 3};
  EXPECT_EQ(v.at(2), 3);  // const at() read path
  EXPECT_THROW(v.at(3), std::out_of_range);
}

TEST(VectorCompareTest, EraseInvalidIteratorThrows) {
  my::Vector<int> v{1, 2, 3};
  EXPECT_THROW(v.erase(v.end()), std::out_of_range);
}

TEST(VectorCompareTest, InsertManyWithNoArgsIsNoop) {
  my::Vector<int> v{1, 2, 3};
  auto it = v.insert_many(v.cbegin() + 1);  // zero args -> nothing inserted
  EXPECT_EQ(v.size(), 3u);
  EXPECT_EQ(*it, 2);  // iterator at requested pos
}

TEST(VectorCompareTest, ConstructFromEmptyInitializerList) {
  my::Vector<int> v({});  // empty init-list: early-return branch
  EXPECT_TRUE(v.empty());
  EXPECT_EQ(v.size(), 0u);
}

TEST(VectorCompareTest, CopyConstructEmptyVector) {
  my::Vector<int> empty;
  my::Vector<int> copy(empty);  // empty copy: early-return branch
  EXPECT_TRUE(copy.empty());
  EXPECT_EQ(copy.size(), 0u);
}
