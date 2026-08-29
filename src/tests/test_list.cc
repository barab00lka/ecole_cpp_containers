#include <gtest/gtest.h>

#include <cstddef>
#include <limits>
#include <list>
#include <stdexcept>
#include <string>

#include "../include/my_list.h"

using my::List;
using std::string;

template <typename T>
string PrintListElements(const List<T>& l) {
  string s;

  for (auto i : l) {
    s += i;
    s += " ";
  }
  s += "\n";
  return s;
}

template <typename T>
string PrintStdListElements(const std::list<T>& l) {
  string s;

  for (auto i : l) {
    s += i;
    s += " ";
  }
  s += "\n";
  return s;
}

template <typename T>
void CheckEqualByElem(const List<T>& actual, const std::list<T>& expected) {
  EXPECT_EQ(actual.size(), expected.size());
  auto it = actual.rbegin();
  auto sit = expected.rbegin();
  for (; sit != expected.rend(); ++it, ++sit) EXPECT_EQ(*it, *sit);

  if (testing::Test::HasFailure()) {
    std::cout << "Actual (from my::List):\n  " << PrintListElements(actual)
              << "Expected (from std::list):\n  "
              << PrintStdListElements(expected) << std::endl;
  }
}

template <typename T>
void CheckEqualByElem(const List<T>& actual, const List<T>& expected) {
  EXPECT_EQ(actual.size(), expected.size());
  auto it = actual.rbegin();
  auto sit = expected.rbegin();
  for (; sit != expected.rend(); ++it, ++sit) EXPECT_EQ(*it, *sit);

  if (testing::Test::HasFailure()) {
    std::cout << "Actual:\n  " << PrintListElements(actual) << "Expected:\n  "
              << PrintListElements(expected) << std::endl;
  }
}

TEST(List, SizeConstructor) {
  List<int> l;
  EXPECT_TRUE(l.empty());
  EXPECT_THROW(l = List<int>(-1), std::length_error);

  List<string> l1(29);
  EXPECT_EQ(l1.size(), 29);
}

TEST(List, FillCtor) {
  auto l = List<string>(50, "aboba");
  EXPECT_EQ(l.size(), 50);
  EXPECT_THROW(
      l = List<string>(std::numeric_limits<size_t>::max() / sizeof(string) + 1,
                       "too many strings");
      , std::length_error);
}

TEST(List, CopyAndSwapCtor) {
  auto l = List<string>(50, "aboba");
  auto l2 = l;
  EXPECT_EQ(l.size(), l2.size());
  l = {};
  EXPECT_EQ(l2.size(), 50);
  EXPECT_EQ(l.size(), 0);
}

TEST(List, MoveCtor) {
  auto l = List<string>(50, "aboba");
  auto l2 = std::move(l);
  EXPECT_EQ(l2.size(), 50);
  EXPECT_EQ(l.size(), 0);
}

TEST(List, InitializerListCtor) {
  auto l = List<string>{"foo", "bar", "baz"};
  EXPECT_EQ(l.back(), "baz");
  EXPECT_EQ(l.front(), "foo");
}

TEST(List, EmplaceBackPushBack) {
  auto l = List<string>{};
  auto el = std::list<string>{};
  auto old_s = l.size();
  auto str = "new";
  l.push_back(str);
  EXPECT_EQ(l.size(), old_s + 1);
  EXPECT_EQ(l.back(), str);
  l.emplace_back(str), el.emplace_back(str);
  EXPECT_EQ(l.front(), el.back());
}

TEST(List, EmplaceAndPush) {
  auto l = List<string>{};
  auto el = std::list<string>{};

  auto str = "new_head";
  l.push_front(str);
  el.push_front(str);
  EXPECT_EQ(l.front(), el.front());

  str = "new tail";
  l.push_back(str);
  el.push_back(str);
  EXPECT_EQ(l.back(), el.back());

  str = "middle node";
  auto second = ++l.begin();
  auto e_second = ++el.begin();
  EXPECT_EQ(*second, *e_second);

  l.emplace(second, str);
  el.emplace(e_second, str);
  EXPECT_EQ(*(++l.begin()), *(++el.begin()))
      << "my::List: " << PrintListElements(l) << std::endl;
}

TEST(List, PopFront) {
  const std::initializer_list<string> tl = {"foo", "bar", "baz", "tsu",
                                            "ye",  "fa",  "kek"};
  std::list<string> el{tl};
  List<string> l{tl};
  el.pop_front();
  l.pop_front();
  EXPECT_EQ(*(l.begin()), *(el.begin()))
      << "my::List: " << PrintListElements(l) << std::endl;
  EXPECT_EQ(*l.begin(), "bar");
}

TEST(List, PopBack) {
  const std::initializer_list<string> tl = {"foo", "bar", "baz", "tsu",
                                            "ye",  "fa",  "kek"};
  std::list<string> el{tl};
  List<string> l{tl};

  el.pop_back();
  l.pop_back();
  EXPECT_EQ(l.back(), el.back())
      << "my::List: " << PrintListElements(l) << std::endl;
  EXPECT_EQ(l.back(), "fa");
}

TEST(List, Inserts) {
  auto tdata = {
      "hello",
      ",",
  };
  std::list<const char*> el{tdata};
  List<const char*> l{tdata};

  l.insert_many_back("world", "!");
  el.insert(el.end(), "world");
  el.insert(el.end(), "!");

  CheckEqualByElem(l, el);

  l.insert_many_front("1st string", "2nd string");
  el.insert(el.begin(), "2nd string");
  el.insert(el.begin(), "1st string");

  CheckEqualByElem(l, el);

  List<int> act = {3, 4, 5};
  List<int> exp = {2, 3, 4, 5};
  auto it = act.insert(act.begin(), 2);

  EXPECT_EQ(*it, 2);
  CheckEqualByElem(act, exp);

  it = act.insert_many(++act.begin(), 22, 23, 24);
  List<int> expected = {2, 22, 23, 24, 3, 4, 5};
  CheckEqualByElem(act, expected);
  EXPECT_EQ(*it, 22);
}

TEST(List, Erase) {
  const std::initializer_list<string> tdata = {"foo", "bar", "baz", "tsu",
                                               "ye",  "fa",  "kek"};
  std::list<string> el{tdata};
  List<string> l{tdata};

  el.erase(++el.begin());
  l.erase(++l.begin());
  EXPECT_EQ(*(++l.begin()), *(++el.begin()))
      << "my::List: " << PrintListElements(l) << std::endl;
  EXPECT_EQ(*(++l.begin()), "baz");

  List<string> tl{tdata};
  tl.erase(++tl.begin());
  tl.pop_back();
  l.erase(--l.end());
  EXPECT_EQ(l.back(), tl.back())
      << "my::List: " << PrintListElements(l) << std::endl;
  EXPECT_EQ(l.back(), "fa");

  l.erase(l.begin());
  tl.pop_front();
  EXPECT_EQ(l.front(), tl.front())
      << "my::List: " << PrintListElements(l) << std::endl;
  EXPECT_EQ(l.front(), "baz");
}

TEST(ListIterator, Operators) {
  const std::initializer_list<string> tl = {"foo", "bar", "baz",
                                            "tsu", "ye",  "fa"};
  auto l = List<string>{tl};
  auto sl = std::list<string>{tl};

  auto last = ++l.begin();
  auto s_last = ++sl.begin();
  EXPECT_EQ(*last, *s_last);
  last--, s_last--;
  EXPECT_EQ(*last, *s_last);
}

TEST(List, ReverseEven) {
  const std::initializer_list<string> tl = {"foo", "bar", "baz",
                                            "tsu", "ye",  "fa"};
  std::list<string> expected{tl};
  List<string> actual{tl};
  actual.reverse(), expected.reverse();

  CheckEqualByElem(actual, expected);
}

TEST(List, ReverseOdd) {
  const std::initializer_list<string> tl = {"foo", "bar", "baz", "tsu",
                                            "ye",  "fa",  "kek"};
  std::list<string> expected{tl};
  List<string> actual{tl};
  actual.reverse(), expected.reverse();

  CheckEqualByElem(actual, expected);
}

TEST(List, SpliceAtTop) {
  const std::initializer_list<string> tdata = {"one", "four", "five"},
                                      tdata2 = {"two", "three"};
  List<string> act{tdata}, am{tdata2};
  std::list<string> exp{tdata}, em{tdata2};
  exp.splice(exp.begin(), em);
  act.splice(act.begin(), am);

  CheckEqualByElem(act, exp);
}
TEST(List, SpliceInMiddle) {
  const std::initializer_list<string> tdata = {"one", "four", "five"},
                                      tdata2 = {"two", "three"};
  List<string> act{tdata}, am{tdata2};
  std::list<string> exp{tdata}, em{tdata2};
  exp.splice(++exp.begin(), em);
  act.splice(++act.begin(), am);

  CheckEqualByElem(act, exp);
}

TEST(List, SortForInts) {
  const std::initializer_list<int> tdata = {1, 3, 2, 9, 5, 6, 4},
                                   expected_data = {1, 2, 3, 4, 5, 6, 9};

  std::list<int> aa{tdata}, ab{expected_data};
  List<int> a{tdata}, b{expected_data};

  a.sort();
  aa.sort();

  CheckEqualByElem(a, aa);

  auto it = aa.rbegin();
  auto sit = ab.rbegin();
  for (; sit != ab.rend(); ++it, ++sit) EXPECT_EQ(*it, *sit);
}

TEST(List, MergeForInts) {
  const std::initializer_list<int> tdata = {1, 3, 2, 9, 4},
                                   tdata2 = {7, 8, 9, 10, 5, 6, 1};

  std::list<int> aa{tdata}, ab{tdata2};
  List<int> a{tdata}, b{tdata2};
  aa.merge(ab);
  a.merge(b);

  CheckEqualByElem(a, aa);
}

TEST(List, MergeForStrings) {
  const std::initializer_list<string> tdata = {"one",  "three", "seven",
                                               "five", "two",   "six"},
                                      tdata2 = {"seven", "eight", "nine", "ten",
                                                "five",  "six",   "one"};

  std::list<string> aa{tdata}, ab{tdata2};
  List<string> a{tdata}, b{tdata2};
  aa.merge(ab);
  a.merge(b);

  CheckEqualByElem(a, aa);
}

TEST(List, Unique) {
  const std::initializer_list<string> tdata = {"one", "three", "three",
                                               "one", "one",   "one"},
                                      exp_data = {"one", "three", "one"};
  List<string> act{tdata}, exp{exp_data};
  act.unique();
  CheckEqualByElem(act, exp);

  exp.pop_back();
  act = exp;
  act.unique();
  CheckEqualByElem(act, exp);
}

TEST(List, UniqueDouble) {
  const std::initializer_list<double> tdata = {1.213231,
                                               8.312312312312312312,
                                               1.213231,
                                               8.312312312312312312,
                                               8.312312312312312312,
                                               8.312312312312312312,
                                               8.312312312312312312},
                                      exp_data = {
                                          1.213231, 8.312312312312312312,
                                          1.213231, 8.312312312312312312};
  List<double> act{tdata}, exp{exp_data};
  act.unique();
  CheckEqualByElem(act, exp);

  exp.pop_back();
  act = exp;
  act.unique();
  CheckEqualByElem(act, exp);

  List<double> act2{1.139128739128731928}, exp2{1.139128739128731928};
  act.unique();
  CheckEqualByElem(act, exp);
}

// --- iterator convenience methods (operator->, postfix--, const conversion) --
TEST(ListIterator, ArrowOperatorReachesMembers) {
  List<string> l{"abc", "de"};
  auto it = l.begin();
  EXPECT_EQ(it->size(), 3u);  // non-const operator->
  const List<string>& cl = l;
  auto cit = cl.begin();
  EXPECT_EQ(cit->size(), 3u);  // const operator->
}

TEST(ListIterator, PostfixDecrement) {
  List<int> l{1, 2, 3};
  auto it = l.end();
  --it;             // now at 3
  auto old = it--;  // postfix: copy at 3, move to 2
  EXPECT_EQ(*old, 3);
  EXPECT_EQ(*it, 2);

  const List<int>& cl = l;
  auto cit = cl.end();
  --cit;
  auto cold = cit--;  // const postfix decrement
  EXPECT_EQ(*cold, 3);
  EXPECT_EQ(*cit, 2);
}

TEST(ListIterator, ExplicitConversionToConst) {
  List<int> l{7, 8};
  List<int>::iterator it = l.begin();
  auto cit = static_cast<List<int>::const_iterator>(it);  // explicit operator
  EXPECT_EQ(*cit, 7);
  const List<int>& cl = l;
  EXPECT_EQ(cit, cl.begin());
}
