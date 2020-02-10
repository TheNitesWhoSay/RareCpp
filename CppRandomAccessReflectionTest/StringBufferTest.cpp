#include <gtest/gtest.h>
#include <regex>
#include "../CppRandomAccessReflectionLib/StringBuffer.h"

TEST(StringBufferTest, ConstructDefault)
{
    StringBuffer sb;
    EXPECT_EQ(0, sb.size());
}

TEST(StringBufferTest, ConstructString)
{
    StringBuffer sbEmptyString("");
    EXPECT_EQ(0, sbEmptyString.size());

    StringBuffer sbOneChar("a");
    EXPECT_EQ(1, sbOneChar.size());
    EXPECT_EQ('a', sbOneChar[0]);

    StringBuffer sbMultiChar("asdf");
    EXPECT_EQ(4, sbMultiChar.size());
    EXPECT_EQ('a', sbMultiChar[0]);
    EXPECT_EQ('s', sbMultiChar[1]);
    EXPECT_EQ('d', sbMultiChar[2]);
    EXPECT_EQ('f', sbMultiChar[3]);

    std::string stringInstance("qwer");
    StringBuffer sbStringInstance(stringInstance);
    EXPECT_EQ(4, sbStringInstance.size());
    EXPECT_EQ('q', sbStringInstance[0]);
    EXPECT_EQ('w', sbStringInstance[1]);
    EXPECT_EQ('e', sbStringInstance[2]);
    EXPECT_EQ('r', sbStringInstance[3]);
}

TEST(StringBufferTest, PlusEqualsCharacter)
{
    StringBuffer sb;
    EXPECT_EQ(0, sb.size());

    sb += 'a';
    EXPECT_EQ(1, sb.size());
    EXPECT_EQ('a', sb[0]);

    sb += 'b';
    EXPECT_EQ(2, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('b', sb[1]);

    sb += 'c';
    EXPECT_EQ(3, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('b', sb[1]);
    EXPECT_EQ('c', sb[2]);
}

TEST(StringBufferTest, PlusEqualsCharArray)
{
    StringBuffer sb;
    EXPECT_EQ(0, sb.size());
    
    sb += "";
    EXPECT_EQ(0, sb.size());

    sb += "asdf";
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('s', sb[1]);
    EXPECT_EQ('d', sb[2]);
    EXPECT_EQ('f', sb[3]);

    char charArrayInstance[] = "qwerty";
    sb += charArrayInstance;
    EXPECT_EQ(10, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('s', sb[1]);
    EXPECT_EQ('d', sb[2]);
    EXPECT_EQ('f', sb[3]);
    EXPECT_EQ('q', sb[4]);
    EXPECT_EQ('w', sb[5]);
    EXPECT_EQ('e', sb[6]);
    EXPECT_EQ('r', sb[7]);
    EXPECT_EQ('t', sb[8]);
    EXPECT_EQ('y', sb[9]);

    sb += "j";
    EXPECT_EQ(11, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('s', sb[1]);
    EXPECT_EQ('d', sb[2]);
    EXPECT_EQ('f', sb[3]);
    EXPECT_EQ('q', sb[4]);
    EXPECT_EQ('w', sb[5]);
    EXPECT_EQ('e', sb[6]);
    EXPECT_EQ('r', sb[7]);
    EXPECT_EQ('t', sb[8]);
    EXPECT_EQ('y', sb[9]);
    EXPECT_EQ('j', sb[10]);
}
