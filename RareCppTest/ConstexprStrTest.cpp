#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
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
using namespace ConstexprStr;

TEST(ConstexprStrTest, ConstexprStrSubstr)
{
    EXPECT_STREQ("a", substr<1>("abcdef").value);
    EXPECT_STREQ("ab", substr<2>("abcdef").value);
    EXPECT_STREQ("abc", substr<3>("abcdef").value);
    EXPECT_STREQ("abcd", substr<4>("abcdef").value);
    EXPECT_STREQ("abcde", substr<5>("abcdef").value);
    EXPECT_STREQ("abcdef", substr<6>("abcdef").value);
    
    EXPECT_STREQ("b", substr<1>(&"abcdef"[0]+1).value);
    EXPECT_STREQ("bc", substr<2>(&"abcdef"[0]+1).value);
    EXPECT_STREQ("bcd", substr<3>(&"abcdef"[0]+1).value);
    EXPECT_STREQ("bcde", substr<4>(&"abcdef"[0]+1).value);
    EXPECT_STREQ("bcdef", substr<5>(&"abcdef"[0]+1).value);
    
    EXPECT_STREQ("c", substr<1>(&"abcdef"[0]+2).value);
    EXPECT_STREQ("cd", substr<2>(&"abcdef"[0]+2).value);
    EXPECT_STREQ("cde", substr<3>(&"abcdef"[0]+2).value);
    EXPECT_STREQ("cdef", substr<4>(&"abcdef"[0]+2).value);
    
    EXPECT_STREQ("d", substr<1>(&"abcdef"[0]+3).value);
    EXPECT_STREQ("de", substr<2>(&"abcdef"[0]+3).value);
    EXPECT_STREQ("def", substr<3>(&"abcdef"[0]+3).value);

    EXPECT_STREQ("e", substr<1>(&"abcdef"[0]+4).value);
    EXPECT_STREQ("ef", substr<2>(&"abcdef"[0]+4).value);

    EXPECT_STREQ("f", substr<1>(&"abcdef"[0]+5).value);
}

TEST(ConstexprStrTest, ConstexprStrLengthAfterLast)
{
    EXPECT_EQ(0, length_after_last("a b ", ' '));
    EXPECT_EQ(1, length_after_last("a b 1", ' '));
    EXPECT_EQ(2, length_after_last("a b 12", ' '));
    EXPECT_EQ(3, length_after_last("a b 123", ' '));
}

TEST(ConstexprStrTest, ConstexprStrFindLastOf)
{
    EXPECT_EQ(6, find_last_of("abcdefabcdef", 'a'));
    EXPECT_EQ(7, find_last_of("abcdefabcdef", 'b'));
    EXPECT_EQ(8, find_last_of("abcdefabcdef", 'c'));
    EXPECT_EQ(9, find_last_of("abcdefabcdef", 'd'));
    EXPECT_EQ(10, find_last_of("abcdefabcdef", 'e'));
    EXPECT_EQ(11, find_last_of("abcdefabcdef", 'f'));
}
