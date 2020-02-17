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

TEST(StringBufferTest, PlusEqualsString)
{
    StringBuffer sb;
    sb += std::string("");
    EXPECT_EQ(0, sb.size());

    sb += std::string("1");
    EXPECT_EQ(1, sb.size());
    EXPECT_EQ('1', sb[0]);

    sb += std::string("a34");
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('1', sb[0]);
    EXPECT_EQ('a', sb[1]);
    EXPECT_EQ('3', sb[2]);
    EXPECT_EQ('4', sb[3]);

    std::string stringInst("qq");
    sb += stringInst;
    EXPECT_EQ(6, sb.size());
    EXPECT_EQ('1', sb[0]);
    EXPECT_EQ('a', sb[1]);
    EXPECT_EQ('3', sb[2]);
    EXPECT_EQ('4', sb[3]);
    EXPECT_EQ('q', sb[4]);
    EXPECT_EQ('q', sb[5]);
}

TEST(StringBufferTest, StreamCharacter)
{
    StringBuffer sb;
    sb << 'm' << 'e' << 'm' << 'e';
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('m', sb[0]);
    EXPECT_EQ('e', sb[1]);
    EXPECT_EQ('m', sb[2]);
    EXPECT_EQ('e', sb[3]);

    sb << 's';
    EXPECT_EQ(5, sb.size());
    EXPECT_EQ('s', sb[4]);
}

TEST(StringBufferTest, StreamCharArray)
{
    StringBuffer sb;
    
    sb << "";
    EXPECT_EQ(0, sb.size());
    
    char charArrayInstance[] = "qwerty";
    sb << "asdf" << charArrayInstance << "j";
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

    sb << "k";
    EXPECT_EQ(12, sb.size());
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
    EXPECT_EQ('k', sb[11]);
}

TEST(StringBufferTest, StreamString)
{
    StringBuffer sb;

    sb << std::string("");
    EXPECT_EQ(0, sb.size());
    
    sb << std::string("1") << std::string("a34");
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('1', sb[0]);
    EXPECT_EQ('a', sb[1]);
    EXPECT_EQ('3', sb[2]);
    EXPECT_EQ('4', sb[3]);

    std::string stringInst("qq");
    sb << stringInst;
    EXPECT_EQ(6, sb.size());
    EXPECT_EQ('1', sb[0]);
    EXPECT_EQ('a', sb[1]);
    EXPECT_EQ('3', sb[2]);
    EXPECT_EQ('4', sb[3]);
    EXPECT_EQ('q', sb[4]);
    EXPECT_EQ('q', sb[5]);
}

TEST(StringBufferTest, AppendNumber)
{
    short shortNum = -42;
    unsigned short unsignedShortNum = 42;
    int intNum = 1337;
    unsigned int unsignedIntNum = 1338;
    long longNum = -987654321;
    unsigned long unsignedLongNum = 987654321;
    long long longLongNum = 0xDEADBEEF;
    unsigned long long unsignedLongLongNum = 0xDEADBEEF;
    bool boolNum = false;
    float floatNum = 6.022f;
    double doubleNum = 6.62607;
    long double longDoubleNum = 3.1415926;
    
    StringBuffer sbShort;
    StringBuffer sbUnsignedShort;
    StringBuffer sbInt;
    StringBuffer sbUnsignedInt;
    StringBuffer sbLong;
    StringBuffer sbUnsignedLong;
    StringBuffer sbLongLong;
    StringBuffer sbUnsignedLongLong;
    StringBuffer sbBool;
    StringBuffer sbFloat;
    StringBuffer sbDouble;
    StringBuffer sbLongDouble;
    
    sbShort.appendNumber(shortNum);
    sbUnsignedShort.appendNumber(unsignedShortNum);
    sbInt.appendNumber(intNum);
    sbUnsignedInt.appendNumber(unsignedIntNum);
    sbLong.appendNumber(longNum);
    sbUnsignedLong.appendNumber(unsignedLongNum);
    sbLongLong.appendNumber(longLongNum);
    sbUnsignedLongLong.appendNumber(unsignedLongLongNum);
    sbBool.appendNumber(boolNum);
    sbFloat.appendNumber(floatNum);
    sbDouble.appendNumber(doubleNum);
    sbLongDouble.appendNumber(longDoubleNum);

    EXPECT_STREQ("-42", sbShort.c_str());
    EXPECT_STREQ("42", sbUnsignedShort.c_str());
    EXPECT_STREQ("1337", sbInt.c_str());
    EXPECT_STREQ("1338", sbUnsignedInt.c_str());
    EXPECT_STREQ("-987654321", sbLong.c_str());
    EXPECT_STREQ("987654321", sbUnsignedLong.c_str());
    EXPECT_STREQ("3735928559", sbLongLong.c_str());
    EXPECT_STREQ("3735928559", sbUnsignedLongLong.c_str());
    EXPECT_STREQ("0", sbBool.c_str());
    EXPECT_STREQ("6.022", sbFloat.c_str());
    EXPECT_STREQ("6.62607", sbDouble.c_str());
    EXPECT_STREQ("3.1415926", sbLongDouble.c_str());
}

TEST(StringBufferTest, StreamNumber)
{
    short shortNum = -42;
    unsigned short unsignedShortNum = 42;
    int intNum = 1337;
    unsigned int unsignedIntNum = 1338;
    long longNum = -987654321;
    unsigned long unsignedLongNum = 987654321;
    long long longLongNum = 0xDEADBEEF;
    unsigned long long unsignedLongLongNum = 0xDEADBEEF;
    bool boolNum = false;
    float floatNum = 6.022f;
    double doubleNum = 6.62607;
    long double longDoubleNum = 3.1415926;
    
    StringBuffer sbShort;
    StringBuffer sbUnsignedShort;
    StringBuffer sbInt;
    StringBuffer sbUnsignedInt;
    StringBuffer sbLong;
    StringBuffer sbUnsignedLong;
    StringBuffer sbLongLong;
    StringBuffer sbUnsignedLongLong;
    StringBuffer sbBool;
    StringBuffer sbFloat;
    StringBuffer sbDouble;
    StringBuffer sbLongDouble;
    
    sbShort << shortNum;
    sbUnsignedShort << unsignedShortNum;
    sbInt << intNum;
    sbUnsignedInt << unsignedIntNum;
    sbLong << longNum;
    sbUnsignedLong << unsignedLongNum;
    sbLongLong << longLongNum;
    sbUnsignedLongLong << unsignedLongLongNum;
    sbBool << boolNum;
    sbFloat << floatNum;
    sbDouble << doubleNum;
    sbLongDouble << longDoubleNum;

    EXPECT_STREQ("-42", sbShort.c_str());
    EXPECT_STREQ("42", sbUnsignedShort.c_str());
    EXPECT_STREQ("1337", sbInt.c_str());
    EXPECT_STREQ("1338", sbUnsignedInt.c_str());
    EXPECT_STREQ("-987654321", sbLong.c_str());
    EXPECT_STREQ("987654321", sbUnsignedLong.c_str());
    EXPECT_STREQ("3735928559", sbLongLong.c_str());
    EXPECT_STREQ("3735928559", sbUnsignedLongLong.c_str());
    EXPECT_STREQ("0", sbBool.c_str());
    EXPECT_STREQ("6.022", sbFloat.c_str());
    EXPECT_STREQ("6.62607", sbDouble.c_str());
    EXPECT_STREQ("3.1415926", sbLongDouble.c_str());
}

TEST(StringBufferTest, StreamEndLine)
{
    StringBuffer sb;
    sb << sb.endl;
    EXPECT_EQ(1, sb.size());
    EXPECT_EQ('\n', sb[0]);
    
    sb << sb.endl << sb.endl;
    EXPECT_EQ(3, sb.size());
    EXPECT_EQ('\n', sb[0]);
    EXPECT_EQ('\n', sb[1]);
    EXPECT_EQ('\n', sb[2]);
}

struct SomeObj { int a; };

std::ostream & operator<<(std::ostream & os, const SomeObj & someObj)
{
    return os << someObj.a;
}

TEST(StringBufferTest, StreamOs)
{
    StringBuffer sb;
    sb << sb.os;
    EXPECT_EQ(0, sb.size());
    
    sb << sb.os << std::hex << std::uppercase << 0xABCD;
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('A', sb[0]);
    EXPECT_EQ('B', sb[1]);
    EXPECT_EQ('C', sb[2]);
    EXPECT_EQ('D', sb[3]);

    SomeObj someObj = { 0xBEEF };
    sb << someObj;

    EXPECT_EQ(8, sb.size());
    EXPECT_EQ('A', sb[0]);
    EXPECT_EQ('B', sb[1]);
    EXPECT_EQ('C', sb[2]);
    EXPECT_EQ('D', sb[3]);
    EXPECT_EQ('B', sb[4]);
    EXPECT_EQ('E', sb[5]);
    EXPECT_EQ('E', sb[6]);
    EXPECT_EQ('F', sb[7]);
}

TEST(StringBufferTest, Termination)
{
    StringBuffer sb;
    EXPECT_EQ(0, sb.size());
    
    sb.nullTerminate();
    EXPECT_EQ(1, sb.size());
    EXPECT_EQ('\0', sb[0]);

    sb.unterminate();
    EXPECT_EQ(0, sb.size());

    sb << "asdf";
    EXPECT_EQ(4, sb.size());

    sb.nullTerminate();
    EXPECT_EQ(5, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('s', sb[1]);
    EXPECT_EQ('d', sb[2]);
    EXPECT_EQ('f', sb[3]);
    EXPECT_EQ('\0', sb[4]);
}

TEST(StringBufferTest, Clear)
{
    StringBuffer sb("asdf");
    EXPECT_EQ(4, sb.size());
    sb.clear();
    EXPECT_EQ(0, sb.size());
}

TEST(StringBufferTest, CStr)
{
    StringBuffer sb("asdf");
    const char* unterminated = sb.c_str(false);
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('a', unterminated[0]);
    EXPECT_EQ('s', unterminated[1]);
    EXPECT_EQ('d', unterminated[2]);
    EXPECT_EQ('f', unterminated[3]);

    const char* terminated = sb.c_str(true);
    EXPECT_EQ(5, sb.size());
    EXPECT_STREQ("asdf", terminated);
}

TEST(StringBufferTest, Str)
{
    StringBuffer sb("asdf");
    std::string str = sb.str();
    EXPECT_EQ(4, sb.size());
    EXPECT_STREQ("asdf", str.c_str());
}

TEST(StringBufferTest, OstreamOverload)
{
    StringBuffer sbEmpty;
    std::stringstream ssEmpty;
    ssEmpty << sbEmpty;
    EXPECT_STREQ("", ssEmpty.str().c_str());

    StringBuffer sb("asdf");
    std::stringstream ss;
    ss << sb;
    EXPECT_EQ(4, sb.size());
    EXPECT_STREQ("asdf", ss.str().c_str());
}

TEST(StringBufferTest, Example)
{
    StringBuffer sb;
    sb << "Quick example printing " << 4 << " words" << sb.endl
        << "Then some characte" << 'r' << 's' << sb.endl
        << "And some numbers and stuff (" << size_t(2) << ", " << short(-5) << ")" << sb.endl;
    EXPECT_STREQ("Quick example printing 4 words\nThen some characters\nAnd some numbers and stuff (2, -5)\n", sb.c_str());
}

TEST(StringBufferTest, Performance_1000_StringStream)
{
    std::stringstream ss;
    for ( size_t i=0; i<1000; i++ )
        ss << "asdf" << i << "qwer" << i*5 << '\n';
}

TEST(StringBufferTest, Performance_1000_StringBuffer)
{
    StringBuffer sb;
    for ( size_t i=0; i<1000; i++ )
        sb << "asdf" << i << "qwer" << i*5 << sb.endl;
}

/*TEST(StringBufferTest, Performance_10000_StringStream)
{
    std::stringstream ss;
    for ( size_t i=0; i<10000; i++ )
        ss << "asdf" << i << "qwer" << i*5 << '\n';
}

TEST(StringBufferTest, Performance_10000_StringBuffer)
{
    StringBuffer sb;
    for ( size_t i=0; i<10000; i++ )
        sb << "asdf" << i << "qwer" << i*5 << sb.endl;
}

TEST(StringBufferTest, Performance_100000_StringStream)
{
    std::stringstream ss;
    for ( size_t i=0; i<100000; i++ )
        ss << "asdf" << i << "qwer" << i*5 << '\n';
}

TEST(StringBufferTest, Performance_100000_StringBuffer)
{
    StringBuffer sb;
    for ( size_t i=0; i<100000; i++ )
        sb << "asdf" << i << "qwer" << i*5 << sb.endl;
}

TEST(StringBufferTest, Performance_1000000_StringStream)
{
    std::stringstream ss;
    for ( size_t i=0; i<1000000; i++ )
        ss << "asdf" << i << "qwer" << i*5 << '\n';
}

TEST(StringBufferTest, Performance_1000000_StringBuffer)
{
    StringBuffer sb;
    for ( size_t i=0; i<1000000; i++ )
        sb << "asdf" << i << "qwer" << i*5 << sb.endl;
}*/

size_t getSizeDoubled(StringBufferPtr stringBufferPtr)
{
    return stringBufferPtr->size() + (*stringBufferPtr).size();
}

TEST(StringBufferTest, StringBufferPtrCall)
{
    std::stringstream ss;
    EXPECT_EQ(0, getSizeDoubled(ss));
    EXPECT_EQ(0, getSizeDoubled((std::ostream &)ss));
    EXPECT_EQ(0, getSizeDoubled((std::istream &)ss));
    EXPECT_EQ(0, getSizeDoubled((std::iostream &)ss));

    StringBuffer sb;
    EXPECT_EQ(0, getSizeDoubled(sb));

    StringBufferPtr ssPtr(ss);
    EXPECT_EQ(0, getSizeDoubled(ssPtr));

    StringBufferPtr sbPtr(sb);
    EXPECT_EQ(0, getSizeDoubled(sbPtr));
}

TEST(StringBufferTest, StringBufferPtrIos)
{
    std::stringstream ss;
    StringBufferPtr osPtr(ss);
    osPtr->put('c');
    EXPECT_EQ(1, osPtr->size());
    EXPECT_EQ((*osPtr)[0], 'c');
    osPtr.flush();
    EXPECT_STREQ("c", ss.str().c_str());
    
    char characters[] = { '\0', '\0' };
    osPtr->read(characters, 1);
    EXPECT_EQ('c', characters[0]);

    ss.clear();
    ss.str("asdf");
    StringBufferPtr otherOsPtr(ss);
    EXPECT_EQ(4, otherOsPtr->size());
    EXPECT_STREQ("asdf", otherOsPtr->str().c_str());
    std::string str;
    *otherOsPtr >> str;
    EXPECT_STREQ("asdf", str.c_str());
    ss.putback('q');
    otherOsPtr->syncInput();
    char next = '\0';
    otherOsPtr->get(next);
    EXPECT_EQ('q', next);
}

TEST(StringBufferTest, StringBufferPtrOs)
{

}
