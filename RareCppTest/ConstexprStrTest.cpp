#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
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
