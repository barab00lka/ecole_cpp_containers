#include <gtest/gtest.h>

#include <queue>
#include <string>

#include "../include/my_queue.h"

using my::Queue;
using std::string;

template <typename T>
void ExpectQueuesEqual(Queue<T> actual, std::queue<T> expected) {
  ASSERT_EQ(actual.size(), expected.size());
  while (!expected.empty()) {
    ASSERT_FALSE(actual.empty());
    EXPECT_EQ(actual.front(), expected.front());
    EXPECT_EQ(actual.back(), expected.back());
    actual.pop();
    expected.pop();
  }
  EXPECT_TRUE(actual.empty());
}

TEST(QueueConstructor, Default) {
  Queue<int> q;
  EXPECT_TRUE(q.empty());
  EXPECT_EQ(q.size(), 0u);
}

TEST(QueueConstructor, InitializerList) {
  Queue<int> q{1, 2, 3, 4};
  EXPECT_EQ(q.size(), 4u);

  EXPECT_EQ(q.front(), 1);
  EXPECT_EQ(q.back(), 4);
}

TEST(QueueConstructor, InitializerListOrder) {
  Queue<int> q{10, 20, 30};
  std::queue<int> ref;
  ref.push(10);
  ref.push(20);
  ref.push(30);
  ExpectQueuesEqual(q, ref);
}

TEST(QueueConstructor, Copy) {
  Queue<int> original{1, 2, 3};
  Queue<int> copy(original);

  EXPECT_EQ(copy.size(), original.size());
  EXPECT_EQ(copy.front(), 1);
  EXPECT_EQ(copy.back(), 3);

  original.pop();
  EXPECT_EQ(original.front(), 2);
  EXPECT_EQ(copy.front(), 1);
}

TEST(QueueConstructor, Move) {
  Queue<string> original{"hello", "world"};
  Queue<string> moved(std::move(original));

  EXPECT_TRUE(original.empty());
  EXPECT_EQ(moved.size(), 2u);
  EXPECT_EQ(moved.front(), "hello");
  EXPECT_EQ(moved.back(), "world");
}

TEST(QueueConstructor, FromContainerCopy) {
  my::List<int> lst{1, 2, 3};
  Queue<int> q(lst);
  EXPECT_EQ(q.size(), 3u);
  EXPECT_EQ(q.front(), 1);
  EXPECT_EQ(q.back(), 3);

  EXPECT_EQ(lst.size(), 3u);
}

TEST(QueueConstructor, FromContainerMove) {
  my::List<int> lst{4, 5, 6};
  Queue<int> q(std::move(lst));
  EXPECT_EQ(q.size(), 3u);
  EXPECT_EQ(q.front(), 4);
  EXPECT_EQ(q.back(), 6);
}

TEST(QueueAssignment, CopyAssignment) {
  Queue<int> q1{1, 2, 3};
  Queue<int> q2;
  q2 = q1;

  EXPECT_EQ(q2.size(), 3u);
  EXPECT_EQ(q2.front(), 1);
  EXPECT_EQ(q2.back(), 3);

  q1.pop();
  EXPECT_EQ(q2.front(), 1);
}

TEST(QueueAssignment, MoveAssignment) {
  Queue<int> q1{10, 20, 30};
  Queue<int> q2;
  q2 = std::move(q1);

  EXPECT_EQ(q2.size(), 3u);
  EXPECT_EQ(q2.front(), 10);
  EXPECT_EQ(q2.back(), 30);
}

TEST(QueueAccess, FrontBackNonConst) {
  Queue<int> q{1, 2, 3};
  EXPECT_EQ(q.front(), 1);
  EXPECT_EQ(q.back(), 3);

  q.front() = 100;
  q.back() = 300;
  EXPECT_EQ(q.front(), 100);
  EXPECT_EQ(q.back(), 300);
}

TEST(QueueAccess, FrontBackConst) {
  const Queue<int> q{7, 8, 9};
  EXPECT_EQ(q.front(), 7);
  EXPECT_EQ(q.back(), 9);
}

TEST(QueueAccess, SingleElementFrontEqualsBack) {
  Queue<int> q;
  q.push(42);
  EXPECT_EQ(q.front(), q.back());
  EXPECT_EQ(q.front(), 42);
}

TEST(QueueCapacity, Empty) {
  Queue<double> q;
  EXPECT_TRUE(q.empty());
  q.push(3.14);
  EXPECT_FALSE(q.empty());
  q.pop();
  EXPECT_TRUE(q.empty());
}

TEST(QueueCapacity, Size) {
  Queue<char> q;
  EXPECT_EQ(q.size(), 0u);
  q.push('a');
  q.push('b');
  q.push('c');
  EXPECT_EQ(q.size(), 3u);
  q.pop();
  EXPECT_EQ(q.size(), 2u);
}

TEST(QueueModifiers, PushLvalue) {
  Queue<string> q;
  const string hello = "Hello";
  q.push(hello);
  EXPECT_EQ(q.front(), "Hello");
  EXPECT_EQ(q.back(), "Hello");
  EXPECT_EQ(q.size(), 1u);
}

TEST(QueueModifiers, PushRvalue) {
  Queue<string> q;
  q.push(string("World"));
  EXPECT_EQ(q.back(), "World");
  EXPECT_EQ(q.size(), 1u);
}

TEST(QueueModifiers, FifoOrder) {
  Queue<int> q;
  for (int i = 0; i < 5; ++i) q.push(i);

  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(q.front(), i);
    q.pop();
  }
  EXPECT_TRUE(q.empty());
}

TEST(QueueModifiers, PushPopInterleaved) {
  Queue<int> q;
  std::queue<int> ref;
  q.push(1), ref.push(1);
  q.push(2), ref.push(2);
  q.pop(), ref.pop();
  q.push(3), ref.push(3);
  ExpectQueuesEqual(q, ref);
  EXPECT_EQ(q.front(), 2);
  EXPECT_EQ(q.back(), 3);
}

TEST(QueueSwap, MemberSwap) {
  Queue<int> a{1, 2, 3};
  Queue<int> b{4, 5};

  a.swap(b);

  EXPECT_EQ(a.size(), 2u);
  EXPECT_EQ(a.front(), 4);
  EXPECT_EQ(a.back(), 5);

  EXPECT_EQ(b.size(), 3u);
  EXPECT_EQ(b.front(), 1);
  EXPECT_EQ(b.back(), 3);
}

TEST(QueueSwap, StdSwap) {
  Queue<int> a{10, 20};
  Queue<int> b{30, 40, 50};

  using std::swap;
  swap(a, b);

  EXPECT_EQ(a.size(), 3u);
  EXPECT_EQ(a.front(), 30);
  EXPECT_EQ(b.size(), 2u);
  EXPECT_EQ(b.front(), 10);
}

TEST(QueueModifiers, InsertManyBack) {
  Queue<int> q;
  q.insert_many_back(10, 20, 30);
  EXPECT_EQ(q.size(), 3u);
  EXPECT_EQ(q.front(), 10);
  EXPECT_EQ(q.back(), 30);
  q.pop();
  EXPECT_EQ(q.front(), 20);
}

TEST(QueueModifiers, InsertManyBackOntoExisting) {
  Queue<int> q{1, 2};
  q.insert_many_back(3, 4, 5);
  std::queue<int> ref;
  for (int i = 1; i <= 5; ++i) ref.push(i);
  ExpectQueuesEqual(q, ref);
}

TEST(QueueStress, ManyPushPop) {
  constexpr int N = 10000;
  Queue<int> q;
  for (int i = 0; i < N; ++i) q.push(i);
  EXPECT_EQ(q.size(), static_cast<size_t>(N));
  EXPECT_EQ(q.front(), 0);
  EXPECT_EQ(q.back(), N - 1);

  for (int i = 0; i < N; ++i) {
    EXPECT_EQ(q.front(), i);
    q.pop();
  }
  EXPECT_TRUE(q.empty());
}
