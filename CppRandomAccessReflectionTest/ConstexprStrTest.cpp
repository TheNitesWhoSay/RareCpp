#include <gtest/gtest.h>
#include "../CppRandomAccessReflectionLib/Reflect.h"
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <type_traits>

TEST(ConstexprStrTest, ConstexprStrSubstr)
{
    EXPECT_STREQ("a", ConstexprStr::substr<1>("abcdef").value);
    EXPECT_STREQ("ab", ConstexprStr::substr<2>("abcdef").value);
    EXPECT_STREQ("abc", ConstexprStr::substr<3>("abcdef").value);
    EXPECT_STREQ("abcd", ConstexprStr::substr<4>("abcdef").value);
    EXPECT_STREQ("abcde", ConstexprStr::substr<5>("abcdef").value);
    EXPECT_STREQ("abcdef", ConstexprStr::substr<6>("abcdef").value);
    
    EXPECT_STREQ("b", ConstexprStr::substr<1>("abcdef"+1).value);
    EXPECT_STREQ("bc", ConstexprStr::substr<2>("abcdef"+1).value);
    EXPECT_STREQ("bcd", ConstexprStr::substr<3>("abcdef"+1).value);
    EXPECT_STREQ("bcde", ConstexprStr::substr<4>("abcdef"+1).value);
    EXPECT_STREQ("bcdef", ConstexprStr::substr<5>("abcdef"+1).value);
    
    EXPECT_STREQ("c", ConstexprStr::substr<1>("abcdef"+2).value);
    EXPECT_STREQ("cd", ConstexprStr::substr<2>("abcdef"+2).value);
    EXPECT_STREQ("cde", ConstexprStr::substr<3>("abcdef"+2).value);
    EXPECT_STREQ("cdef", ConstexprStr::substr<4>("abcdef"+2).value);
    
    EXPECT_STREQ("d", ConstexprStr::substr<1>("abcdef"+3).value);
    EXPECT_STREQ("de", ConstexprStr::substr<2>("abcdef"+3).value);
    EXPECT_STREQ("def", ConstexprStr::substr<3>("abcdef"+3).value);

    EXPECT_STREQ("e", ConstexprStr::substr<1>("abcdef"+4).value);
    EXPECT_STREQ("ef", ConstexprStr::substr<2>("abcdef"+4).value);

    EXPECT_STREQ("f", ConstexprStr::substr<1>("abcdef"+5).value);
}

TEST(ConstexprStrTest, ConstexprStrLengthBetween)
{
    EXPECT_EQ(0, ConstexprStr::length_between("<>", '<', '>'));
    EXPECT_EQ(1, ConstexprStr::length_between("<1>", '<', '>'));
    EXPECT_EQ(2, ConstexprStr::length_between("<12>", '<', '>'));
    EXPECT_EQ(3, ConstexprStr::length_between("<123>", '<', '>'));
    EXPECT_EQ(4, ConstexprStr::length_between("<1234>", '<', '>'));

    EXPECT_EQ(2, ConstexprStr::length_between("<<>>", '<', '>'));
    EXPECT_EQ(3, ConstexprStr::length_between("<<1>>", '<', '>'));
    EXPECT_EQ(4, ConstexprStr::length_between("<<12>>", '<', '>'));
    EXPECT_EQ(5, ConstexprStr::length_between("<<123>>", '<', '>'));
    EXPECT_EQ(6, ConstexprStr::length_between("<<1234>>", '<', '>'));
}

TEST(ConstexprStrTest, ConstexprStrLengthAfterLast)
{
    EXPECT_EQ(0, ConstexprStr::length_after_last("a b ", ' '));
    EXPECT_EQ(1, ConstexprStr::length_after_last("a b 1", ' '));
    EXPECT_EQ(2, ConstexprStr::length_after_last("a b 12", ' '));
    EXPECT_EQ(3, ConstexprStr::length_after_last("a b 123", ' '));
}

TEST(ConstexprStrTest, ConstexprStrFind)
{
    EXPECT_EQ(0, ConstexprStr::find("abcdefabcdef", 'a'));
    EXPECT_EQ(1, ConstexprStr::find("abcdefabcdef", 'b'));
    EXPECT_EQ(2, ConstexprStr::find("abcdefabcdef", 'c'));
    EXPECT_EQ(3, ConstexprStr::find("abcdefabcdef", 'd'));
    EXPECT_EQ(4, ConstexprStr::find("abcdefabcdef", 'e'));
    EXPECT_EQ(5, ConstexprStr::find("abcdefabcdef", 'f'));
}

TEST(ConstexprStrTest, ConstexprStrFindLastOf)
{
    EXPECT_EQ(6, ConstexprStr::find_last_of("abcdefabcdef", 'a'));
    EXPECT_EQ(7, ConstexprStr::find_last_of("abcdefabcdef", 'b'));
    EXPECT_EQ(8, ConstexprStr::find_last_of("abcdefabcdef", 'c'));
    EXPECT_EQ(9, ConstexprStr::find_last_of("abcdefabcdef", 'd'));
    EXPECT_EQ(10, ConstexprStr::find_last_of("abcdefabcdef", 'e'));
    EXPECT_EQ(11, ConstexprStr::find_last_of("abcdefabcdef", 'f'));
}
