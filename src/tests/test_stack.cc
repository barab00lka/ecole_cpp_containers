#include <gtest/gtest.h>

#include <stack>

#include "../include/my_stack.h"

using my::Stack;
using std::string;

TEST(StackConstructor, Default) {
  Stack<int> s;
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
}

TEST(StackConstructor, InitializerList) {
  Stack<int> s{1, 2, 3, 4};
  EXPECT_EQ(s.size(), 4u);

  EXPECT_EQ(s.top(), 4);
  s.pop();
  EXPECT_EQ(s.top(), 3);
  s.pop();
  EXPECT_EQ(s.top(), 2);
  s.pop();
  EXPECT_EQ(s.top(), 1);
  s.pop();
  EXPECT_TRUE(s.empty());
}

TEST(StackConstructor, Copy) {
  Stack<int> original{10, 20, 30};
  Stack<int> copy(original);

  EXPECT_EQ(copy.size(), original.size());
  EXPECT_EQ(copy.top(), 30);

  original.pop();
  EXPECT_EQ(original.top(), 20);
  EXPECT_EQ(copy.top(), 30);
}

TEST(StackConstructor, Move) {
  Stack<string> original{"hello", "world"};
  Stack<string> moved(std::move(original));

  EXPECT_TRUE(original.empty());
  EXPECT_EQ(moved.size(), 2u);
  EXPECT_EQ(moved.top(), "world");
  moved.pop();
  EXPECT_EQ(moved.top(), "hello");
}

TEST(StackAssignment, CopyAssignment) {
  Stack<int> s1{1, 2, 3};
  Stack<int> s2;
  s2 = s1;

  EXPECT_EQ(s2.size(), 3u);
  EXPECT_EQ(s2.top(), 3);

  s1.pop();
  EXPECT_EQ(s2.top(), 3);

  s2 = s2;
  EXPECT_EQ(s2.size(), 3u);
  EXPECT_EQ(s2.top(), 3);
}

TEST(StackAssignment, MoveAssignment) {
  Stack<int> s1{10, 20, 30};
  Stack<int> s2;
  s2 = std::move(s1);

  EXPECT_TRUE(s1.empty());
  EXPECT_EQ(s2.size(), 3u);
  EXPECT_EQ(s2.top(), 30);

  s2 = std::move(s2);
  EXPECT_EQ(s2.size(), 3u);
  EXPECT_EQ(s2.top(), 30);
}

TEST(StackAccess, TopNonConst) {
  Stack<int> s{42};
  EXPECT_EQ(s.top(), 42);
  s.top() = 100;
  EXPECT_EQ(s.top(), 100);
}

TEST(StackAccess, TopConst) {
  const Stack<int> s{1, 2, 3};
  EXPECT_EQ(s.top(), 3);
}

TEST(StackCapacity, Empty) {
  Stack<double> s;
  EXPECT_TRUE(s.empty());
  s.push(3.14);
  EXPECT_FALSE(s.empty());
  s.pop();
  EXPECT_TRUE(s.empty());
}

TEST(StackCapacity, Size) {
  Stack<char> s;
  EXPECT_EQ(s.size(), 0u);
  s.push('a');
  s.push('b');
  s.push('c');
  EXPECT_EQ(s.size(), 3u);
  s.pop();
  EXPECT_EQ(s.size(), 2u);
}

TEST(StackModifiers, PushLvalue) {
  Stack<string> s;
  const string hello = "Hello";
  s.push(hello);
  EXPECT_EQ(s.top(), "Hello");
  EXPECT_EQ(s.size(), 1u);
}

TEST(StackModifiers, PushRvalue) {
  Stack<string> s;
  s.push(string("World"));
  EXPECT_EQ(s.top(), "World");
  EXPECT_EQ(s.size(), 1u);
}

TEST(StackModifiers, Pop) {
  Stack<int> s{5, 10, 15};
  s.pop();
  EXPECT_EQ(s.top(), 10);
  s.pop();
  EXPECT_EQ(s.top(), 5);
  s.pop();
  EXPECT_TRUE(s.empty());
}

TEST(StackSwap, MemberSwap) {
  Stack<int> a{1, 2, 3};
  Stack<int> b{4, 5};

  a.swap(b);

  EXPECT_EQ(a.size(), 2u);
  EXPECT_EQ(a.top(), 5);
  a.pop();
  EXPECT_EQ(a.top(), 4);

  EXPECT_EQ(b.size(), 3u);
  EXPECT_EQ(b.top(), 3);
  b.pop();
  EXPECT_EQ(b.top(), 2);
}

TEST(StackSwap, NonMemberSwap) {
  Stack<int> a{10, 20};
  Stack<int> b{30, 40, 50};

  using std::swap;
  swap(a, b);

  EXPECT_EQ(a.size(), 3u);
  EXPECT_EQ(a.top(), 50);
  EXPECT_EQ(b.size(), 2u);
  EXPECT_EQ(b.top(), 20);
}

TEST(StackStress, ManyPushPop) {
  constexpr int N = 10000;
  Stack<int> s;
  for (int i = 0; i < N; ++i) {
    s.push(i);
  }
  EXPECT_EQ(s.size(), static_cast<size_t>(N));
  EXPECT_EQ(s.top(), N - 1);

  for (int i = N - 1; i >= 0; --i) {
    EXPECT_EQ(s.top(), i);
    s.pop();
  }
  EXPECT_TRUE(s.empty());
}

TEST(StackString, CopyAndMoveSemantics) {
  Stack<string> original{"short", "longer string"};
  Stack<string> copy(original);
  Stack<string> moved(std::move(original));

  EXPECT_EQ(copy.size(), 2u);
  EXPECT_EQ(copy.top(), "longer string");

  EXPECT_EQ(moved.size(), 2u);
  EXPECT_EQ(moved.top(), "longer string");
  EXPECT_TRUE(original.empty());
}

TEST(StackModifiers, InsertManyBack) {
  Stack<int> s;
  s.insert_many_back(10, 20, 30);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s.top(), 30);
  s.pop();
  EXPECT_EQ(s.top(), 20);
  s.pop();
  EXPECT_EQ(s.top(), 10);
  s.pop();
  EXPECT_TRUE(s.empty());
}
