#include <gtest/gtest.h>
#include <regex>
#include "../RareCppLib/StringBuffer.h"
using namespace BufferedStream;

//#define RUN_PERFORMANCE_TESTS

TEST(StringBufferTest, ConstructDefault)
{
    StringBuffer sb;
    EXPECT_TRUE(sb.source() == nullptr);
    EXPECT_EQ(0, sb.size());
}

TEST(StringBufferTest, ConstructSource)
{
    std::stringstream src;
    StringBuffer sb(src);
    EXPECT_EQ(&src, sb.source());
    EXPECT_EQ(0, sb.size());
}

TEST(StringBufferTest, ConstructString)
{
    StringBuffer sbEmptyString("");
    EXPECT_TRUE(sbEmptyString.source() == nullptr);
    EXPECT_EQ(0, sbEmptyString.size());

    StringBuffer sbOneChar("a");
    EXPECT_TRUE(sbOneChar.source() == nullptr);
    EXPECT_EQ(1, sbOneChar.size());
    EXPECT_EQ('a', sbOneChar[0]);

    StringBuffer sbMultiChar("asdf");
    EXPECT_TRUE(sbMultiChar.source() == nullptr);
    EXPECT_EQ(4, sbMultiChar.size());
    EXPECT_EQ('a', sbMultiChar[0]);
    EXPECT_EQ('s', sbMultiChar[1]);
    EXPECT_EQ('d', sbMultiChar[2]);
    EXPECT_EQ('f', sbMultiChar[3]);

    std::string stringInstance("qwer");
    StringBuffer sbStringInstance(stringInstance);
    EXPECT_TRUE(sbStringInstance.source() == nullptr);
    EXPECT_EQ(4, sbStringInstance.size());
    EXPECT_EQ('q', sbStringInstance[0]);
    EXPECT_EQ('w', sbStringInstance[1]);
    EXPECT_EQ('e', sbStringInstance[2]);
    EXPECT_EQ('r', sbStringInstance[3]);
}

TEST(StringBufferTest, ConstructStringSource)
{
    std::stringstream src;
    StringBuffer sbEmptyStringNull("", src);
    EXPECT_EQ(&src, sbEmptyStringNull.source());
    EXPECT_EQ(0, sbEmptyStringNull.size());

    StringBuffer sbOneCharNull("a", src);
    EXPECT_EQ(&src, sbOneCharNull.source());
    EXPECT_EQ(1, sbOneCharNull.size());
    EXPECT_EQ('a', sbOneCharNull[0]);

    StringBuffer sbMultiCharNull("asdf", src);
    EXPECT_EQ(&src, sbMultiCharNull.source());
    EXPECT_EQ(4, sbMultiCharNull.size());
    EXPECT_EQ('a', sbMultiCharNull[0]);
    EXPECT_EQ('s', sbMultiCharNull[1]);
    EXPECT_EQ('d', sbMultiCharNull[2]);
    EXPECT_EQ('f', sbMultiCharNull[3]);

    std::string stringInstance("qwer");
    StringBuffer sbStringInstanceNull(stringInstance, src);
    EXPECT_EQ(&src, sbStringInstanceNull.source());
    EXPECT_EQ(4, sbStringInstanceNull.size());
    EXPECT_EQ('q', sbStringInstanceNull[0]);
    EXPECT_EQ('w', sbStringInstanceNull[1]);
    EXPECT_EQ('e', sbStringInstanceNull[2]);
    EXPECT_EQ('r', sbStringInstanceNull[3]);
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
    EXPECT_TRUE(sbFloat.str().find_first_of("6.022") == 0);
    EXPECT_TRUE(sbDouble.str().find_first_of("6.62607") == 0);
    EXPECT_TRUE(sbLongDouble.str().find_first_of("3.1415926") == 0);
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
    EXPECT_TRUE(sbFloat.str().find_first_of("6.022") == 0);
    EXPECT_TRUE(sbDouble.str().find_first_of("6.62607") == 0);
    EXPECT_TRUE(sbLongDouble.str().find_first_of("3.1415926") == 0);
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

TEST(StringBufferTest, Vec)
{
    StringBuffer sb("asdf");
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('s', sb[1]);
    EXPECT_EQ('d', sb[2]);
    EXPECT_EQ('f', sb[3]);
    const std::vector<char> & vec = sb.vec();
    EXPECT_EQ(4, vec.size());
    EXPECT_EQ('a', vec[0]);
    EXPECT_EQ('s', vec[1]);
    EXPECT_EQ('d', vec[2]);
    EXPECT_EQ('f', vec[3]);
}

TEST(StringBufferTest, SwapVec)
{
    StringBuffer sb("asdf");
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('s', sb[1]);
    EXPECT_EQ('d', sb[2]);
    EXPECT_EQ('f', sb[3]);

    std::vector<char> vec;
    EXPECT_EQ(0, vec.size());

    sb.swap(vec);
    EXPECT_EQ(0, sb.size());
    EXPECT_EQ(4, vec.size());
    EXPECT_EQ('a', vec[0]);
    EXPECT_EQ('s', vec[1]);
    EXPECT_EQ('d', vec[2]);
    EXPECT_EQ('f', vec[3]);
}

TEST(StringBufferTest, SwapBuffer)
{
    StringBuffer sb("asdf");
    EXPECT_EQ(4, sb.size());
    EXPECT_EQ('a', sb[0]);
    EXPECT_EQ('s', sb[1]);
    EXPECT_EQ('d', sb[2]);
    EXPECT_EQ('f', sb[3]);

    StringBuffer sb2("qwe");
    EXPECT_EQ(3, sb2.size());
    EXPECT_EQ('q', sb2[0]);
    EXPECT_EQ('w', sb2[1]);
    EXPECT_EQ('e', sb2[2]);

    sb.swap(sb2);

    EXPECT_EQ(3, sb.size());
    EXPECT_EQ('q', sb[0]);
    EXPECT_EQ('w', sb[1]);
    EXPECT_EQ('e', sb[2]);

    EXPECT_EQ(4, sb2.size());
    EXPECT_EQ('a', sb2[0]);
    EXPECT_EQ('s', sb2[1]);
    EXPECT_EQ('d', sb2[2]);
    EXPECT_EQ('f', sb2[3]);
}

TEST(StringBufferTest, OstreamPosition)
{
    StringBuffer sb;
    std::ostream & os = (std::ostream &)sb;
    os << "asdf";
    EXPECT_EQ(4, sb.size());
    EXPECT_STREQ("asdf", sb.str().c_str());
    os.seekp(0);
    os << 'q';
    EXPECT_EQ(4, sb.size());
    EXPECT_STREQ("qsdf", sb.str().c_str());
    os.seekp(2);
    os << "erty";
    EXPECT_EQ(6, sb.size());
    EXPECT_STREQ("qserty", sb.str().c_str());
    os.seekp(1);
    os << 'w';
    EXPECT_EQ(6, sb.size());
    EXPECT_STREQ("qwerty", sb.str().c_str());
}

TEST(StringBufferTest, IstreamPosition)
{
    StringBuffer sb("qwerty");
    std::istream & is = (std::istream &)sb;

    char c = '\0';
    is >> c;
    EXPECT_EQ('q', c);

    is >> c;
    EXPECT_EQ('w', c);

    is >> c;
    EXPECT_EQ('e', c);

    is >> c;
    EXPECT_EQ('r', c);

    is >> c;
    EXPECT_EQ('t', c);

    is >> c;
    EXPECT_EQ('y', c);

    is.seekg(1);
    is >> c;
    EXPECT_EQ('w', c);
}

TEST(StringBufferTest, IostreamPosition)
{
    StringBuffer sb;
    std::iostream & ios = (std::iostream &)sb;
    ios << "asdf";
    EXPECT_EQ(4, sb.size());
    EXPECT_STREQ("asdf", sb.str().c_str());
    ios.seekp(0);
    ios << 'q';
    EXPECT_EQ(4, sb.size());
    EXPECT_STREQ("qsdf", sb.str().c_str());

    char c = '\0';
    ios >> c;
    EXPECT_EQ('q', c);
    ios >> c;
    EXPECT_EQ('s', c);
    ios >> c;
    EXPECT_EQ('d', c);
    ios >> c;
    EXPECT_EQ('f', c);
    
    ios.seekp(2);
    ios << "erty";
    ios >> c;
    EXPECT_EQ('t', c);
    ios >> c;
    EXPECT_EQ('y', c);
}

TEST(StringBufferTest, SourceBackedRead)
{
    std::stringstream src;
    src << "qwerty";
    EXPECT_STREQ("qwerty", src.str().c_str());
    StringBuffer sb("asdf", src);

    char asdf[5] = {};
    sb.read(asdf, 4);
    EXPECT_STREQ("asdf", asdf);

    char qwerty[7] = {};
    sb.read(qwerty, 6);
    EXPECT_STREQ("qwerty", qwerty);
    EXPECT_STREQ("asdfqwerty", sb.str().c_str());

    src << "poiuy";
    EXPECT_STREQ("qwertypoiuy", src.str().c_str());

    char poiuy[6] = {};
    sb.read(poiuy, 5);
    EXPECT_STREQ("poiuy", poiuy);
    EXPECT_STREQ("asdfqwertypoiuy", sb.str().c_str());
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
#ifdef RUN_PERFORMANCE_TESTS
    std::stringstream ss;
    for ( size_t i=0; i<1000; i++ )
        ss << "asdf" << i << "qwer" << i*5 << '\n';
#endif
}

TEST(StringBufferTest, Performance_1000_StringBuffer)
{
#ifdef RUN_PERFORMANCE_TESTS
    StringBuffer sb;
    for ( size_t i=0; i<1000; i++ )
        sb << "asdf" << i << "qwer" << i*5 << sb.endl;
#endif
}

TEST(StringBufferTest, Performance_10000_StringStream)
{
#ifdef RUN_PERFORMANCE_TESTS
    std::stringstream ss;
    for ( size_t i=0; i<10000; i++ )
        ss << "asdf" << i << "qwer" << i*5 << '\n';
#endif
}

TEST(StringBufferTest, Performance_10000_StringBuffer)
{
#ifdef RUN_PERFORMANCE_TESTS
    StringBuffer sb;
    for ( size_t i=0; i<10000; i++ )
        sb << "asdf" << i << "qwer" << i*5 << sb.endl;
#endif
}

TEST(StringBufferTest, Performance_100000_StringStream)
{
#ifdef RUN_PERFORMANCE_TESTS
    std::stringstream ss;
    for ( size_t i=0; i<100000; i++ )
        ss << "asdf" << i << "qwer" << i*5 << '\n';
#endif
}

TEST(StringBufferTest, Performance_100000_StringBuffer)
{
#ifdef RUN_PERFORMANCE_TESTS
    StringBuffer sb;
    for ( size_t i=0; i<100000; i++ )
        sb << "asdf" << i << "qwer" << i*5 << sb.endl;
#endif
}

TEST(StringBufferTest, Performance_1000000_StringStream)
{
#ifdef RUN_PERFORMANCE_TESTS
    std::stringstream ss;
    for ( size_t i=0; i<1000000; i++ )
        ss << "asdf" << i << "qwer" << i*5 << '\n';
#endif
}

TEST(StringBufferTest, Performance_1000000_StringBuffer)
{
#ifdef RUN_PERFORMANCE_TESTS
    StringBuffer sb;
    for ( size_t i=0; i<1000000; i++ )
        sb << "asdf" << i << "qwer" << i*5 << sb.endl;
#endif
}

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
    StringBufferPtr iosPtr(ss);
    iosPtr->put('c');
    EXPECT_EQ(1, iosPtr->size());
    EXPECT_EQ((*iosPtr)[0], 'c');
    iosPtr.flush();
    EXPECT_STREQ("c", ss.str().c_str());
    
    char characters[] = { '\0', '\0' };
    iosPtr->read(characters, 1);
    EXPECT_EQ('c', characters[0]);

    ss.clear();
    ss.str("asdf");
    StringBufferPtr otherIosPtr(ss);
    std::string str;
    *otherIosPtr >> str;
    EXPECT_EQ(4, otherIosPtr->size());
    EXPECT_STREQ("asdf", otherIosPtr->str().c_str());
    EXPECT_STREQ("asdf", str.c_str());
    ss.putback('q');
    otherIosPtr->sync();
    char next = '\0';
    otherIosPtr->get(next);
    EXPECT_EQ('q', next);
}

TEST(StringBufferTest, StringBufferPtrOs)
{
    std::stringstream ss;
    StringBufferPtr osPtr((std::ostream &)ss);
    osPtr->put('c');
    EXPECT_EQ(1, osPtr->size());
    EXPECT_EQ((*osPtr)[0], 'c');
    osPtr.flush();
    EXPECT_STREQ("c", ss.str().c_str());
}

TEST(StringBufferTest, StringBufferPtrIs)
{
    std::stringstream ss("asdf");
    StringBufferPtr isPtr((std::istream &)ss);
    char characters[5] = {};
    isPtr->read(characters, 4);
    EXPECT_STREQ("asdf", characters);
}

void ostreamTest(std::ostream & os)
{
    os << "a";
}

char istreamTest(std::istream & is)
{
    char c = '\0';
    is >> c;
    return c;
}

void oStringBufferTest(OStringBufferPtr osb)
{
    *osb << "b";
}

char iStringBufferTest(IStringBufferPtr isb)
{
    char c = '\0';
    *isb >> c;
    return c;
}

char ioStringBufferTest(StringBufferPtr sb)
{
    char c = '\0';
    *sb >> c;
    *sb << 'c';
    return c;
}

TEST(StringBufferTest, IOStringBufferPtrCalls)
{
    std::stringstream os1;
    OStringBuffer osb1;
    StringBuffer sb1o;
    ostreamTest(os1);
    ostreamTest(osb1);
    ostreamTest(sb1o);
    EXPECT_EQ('a', os1.str()[0]);
    EXPECT_EQ('a', osb1.str()[0]);
    EXPECT_EQ('a', sb1o.str()[0]);
    
    std::stringstream is1("d");
    IStringBuffer isb1("e");
    StringBuffer sb1i("f");
    EXPECT_EQ('d', istreamTest(is1));
    EXPECT_EQ('e', istreamTest(isb1));
    EXPECT_EQ('f', istreamTest(sb1i));
    
    std::stringstream os2;
    OStringBuffer osb2;
    StringBuffer sb2o;
    oStringBufferTest(os2);
    oStringBufferTest(osb2);
    oStringBufferTest(sb2o);
    EXPECT_EQ('b', os2.str()[0]);
    EXPECT_EQ('b', osb2.str()[0]);
    EXPECT_EQ('b', sb2o.str()[0]);

    std::stringstream is2("g");
    IStringBuffer isb2("h");
    StringBuffer sb2i("i");
    EXPECT_EQ('g', iStringBufferTest(is2));
    EXPECT_EQ('h', iStringBufferTest(isb2));
    EXPECT_EQ('i', iStringBufferTest(sb2i));

    std::stringstream ios2("j");
    StringBuffer sb2io("l");
    EXPECT_EQ('j', ioStringBufferTest(ios2));
    EXPECT_EQ('l', ioStringBufferTest(sb2io));
    EXPECT_EQ('c', ios2.str()[1]);
    EXPECT_EQ('c', sb2io.str()[1]);
}
