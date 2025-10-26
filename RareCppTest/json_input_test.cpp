#ifdef GET_RUNJSONINPUTTESTSRC_INCLUDES
#include "json_input_test.h"
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <regex>
#endif
#ifdef GET_RUNJSONINPUTTESTSRC_CPP

using namespace Reflection;

#ifdef USE_BUFFERED_STREAMS
#undef TEST_HEADER
#define TEST_HEADER(group, name) TEST(group##Buffered, name)
#else
#undef TEST_HEADER
#define TEST_HEADER(group, name) TEST(group##Unbuffered, name)
#endif

TEST_HEADER(JsonInputCustomizersTest, CustomizeUnspecialized)
{
    CustomizeUnspecialized customizeUnspecialized = { 1, 2, 'a' };
    bool isSpecialized = Json::Input::Customize<CustomizeUnspecialized, int, Reflect<CustomizeUnspecialized>::IndexOf::firstField, NoNote, Reflect<CustomizeUnspecialized>::MemberType::firstField>
        ::as(std::cin, Json::defaultContext, customizeUnspecialized, customizeUnspecialized.firstField);
    EXPECT_FALSE(isSpecialized);

    isSpecialized = Json::Input::haveSpecialization<CustomizeUnspecialized, int>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeFullySpecialized)
{
    CustomizeFullySpecialized customizeFullySpecialized = { 1, 2, 'a' };
    bool isSpecialized = Json::Input::Customize<CustomizeFullySpecialized, int, Reflect<CustomizeFullySpecialized>::IndexOf::firstField, NoNote, Reflect<CustomizeFullySpecialized>::MemberType::firstField>
        ::as(std::cin, Json::defaultContext, customizeFullySpecialized, customizeFullySpecialized.firstField);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::haveSpecialization<CustomizeFullySpecialized, int, Reflect<CustomizeFullySpecialized>::IndexOf::firstField, NoNote, Reflect<CustomizeFullySpecialized>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize5Args_OpAnnotationsDefaulted)
{
    Customize5Args_OpAnnotationsDefaulted customize5Args_OpAnnotationsDefaulted;
    bool isSpecialized = Json::Input::Customize<Customize5Args_OpAnnotationsDefaulted, int, Reflect<Customize5Args_OpAnnotationsDefaulted>::IndexOf::firstField, NoNote, Reflect<Customize5Args_OpAnnotationsDefaulted>::MemberType::firstField>
        ::as(std::cin, Json::defaultContext, customize5Args_OpAnnotationsDefaulted, customize5Args_OpAnnotationsDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::haveSpecialization<Customize5Args_OpAnnotationsDefaulted, int, Reflect<Customize5Args_OpAnnotationsDefaulted>::IndexOf::firstField, NoNote, Reflect<Customize5Args_OpAnnotationsDefaulted>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize5Args_MemberIndexDefaulted)
{
    Customize5Args_MemberIndexDefaulted customize5Args_MemberIndexDefaulted;
    bool isSpecialized = Json::Input::Customize<Customize5Args_MemberIndexDefaulted, int, Json::noMemberIndex, NoNote, Reflect<Customize5Args_MemberIndexDefaulted>::MemberType::firstField>
        ::as(std::cin, Json::defaultContext, customize5Args_MemberIndexDefaulted, customize5Args_MemberIndexDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::haveSpecialization<Customize5Args_MemberIndexDefaulted, int, Reflect<Customize5Args_MemberIndexDefaulted>::IndexOf::firstField, NoNote, Reflect<Customize5Args_MemberIndexDefaulted>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize5Args_BothDefaulted)
{
    Customize5Args_BothDefaulted customize5Args_BothDefaulted;
    bool isSpecialized = Json::Input::Customize<Customize5Args_BothDefaulted, int, Json::noMemberIndex, NoNote, Reflect<Customize5Args_BothDefaulted>::MemberType::firstField>
        ::as(std::cin, Json::defaultContext, customize5Args_BothDefaulted, customize5Args_BothDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::haveSpecialization<Customize5Args_BothDefaulted, int, Reflect<Customize5Args_BothDefaulted>::IndexOf::firstField, NoNote, Reflect<Customize5Args_BothDefaulted>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize4Args)
{
    Customize4Args customize4Args;
    bool isSpecialized = Json::Input::Customize<Customize4Args, int, Reflect<Customize4Args>::IndexOf::firstField, NoNote>
        ::as(std::cin, Json::defaultContext, customize4Args, customize4Args.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::haveSpecialization<Customize4Args, int, Reflect<Customize4Args>::IndexOf::firstField, NoNote, Reflect<Customize4Args>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize4Args_MemberIndexDefaulted)
{
    Customize4Args_MemberIndexDefaulted customize4Args_MemberIndexDefaulted;
    bool isSpecialized = Json::Input::Customize<Customize4Args_MemberIndexDefaulted, int, Json::noMemberIndex, NoNote>
        ::as(std::cin, Json::defaultContext, customize4Args_MemberIndexDefaulted, customize4Args_MemberIndexDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::haveSpecialization<Customize4Args_MemberIndexDefaulted, int, Reflect<Customize4Args_MemberIndexDefaulted>::IndexOf::firstField, NoNote, Reflect<Customize4Args_MemberIndexDefaulted>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize3Args)
{
    Customize3Args customize3Args;
    bool isSpecialized = Json::Input::Customize<Customize3Args, int, Reflect<Customize3Args>::IndexOf::firstField>
        ::as(std::cin, Json::defaultContext, customize3Args, customize3Args.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::haveSpecialization<Customize3Args, int, Reflect<Customize3Args>::IndexOf::firstField, NoNote, Reflect<Customize3Args>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize2Args)
{
    Customize2Args customize2Args;
    bool isSpecialized = Json::Input::Customize<Customize2Args, int>
        ::as(std::cin, Json::defaultContext, customize2Args, customize2Args.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::haveSpecialization<Customize2Args, int, Reflect<Customize2Args>::IndexOf::firstField, NoNote, Reflect<Customize2Args>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeTypeFullySpecialized)
{
    std::stringstream input("0");
    CustomizeTypeFullySpecialized customizeTypeFullySpecialized;
    bool isSpecialized = Json::Input::CustomizeType<CustomizeTypeFullySpecialized, NoNote, Reflect<CustomizeTypeFullySpecialized>::MemberType::firstField>
        ::as(input, Json::defaultContext, customizeTypeFullySpecialized);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::haveSpecialization<CustomizeTypeFullySpecialized, CustomizeTypeFullySpecialized, Reflect<CustomizeTypeFullySpecialized>::IndexOf::firstField, NoNote, Reflect<CustomizeTypeFullySpecialized>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeType3Args_OpAnnotationsDefaulted)
{
    CustomizeType3Args_OpAnnotationsDefaulted customizeType3Args_OpAnnotationsDefaulted;
    bool isSpecialized = Json::Input::CustomizeType<CustomizeType3Args_OpAnnotationsDefaulted, NoNote, Reflect<CustomizeType3Args_OpAnnotationsDefaulted>::MemberType::firstField>
        ::as(std::cin, Json::defaultContext, customizeType3Args_OpAnnotationsDefaulted);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::haveSpecialization<CustomizeType3Args_OpAnnotationsDefaulted, CustomizeType3Args_OpAnnotationsDefaulted, Reflect<CustomizeType3Args_OpAnnotationsDefaulted>::IndexOf::firstField, NoNote, Reflect<CustomizeType3Args_OpAnnotationsDefaulted>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeType2Args)
{
    CustomizeType2Args customizeType2Args;
    bool isSpecialized = Json::Input::CustomizeType<CustomizeType2Args, NoNote>
        ::as(std::cin, Json::defaultContext, customizeType2Args);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::haveSpecialization<CustomizeType2Args, CustomizeType2Args, Reflect<CustomizeType2Args>::IndexOf::firstField, NoNote, Reflect<CustomizeType2Args>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeType1Arg)
{
    CustomizeType1Arg customizeType1Arg;
    bool isSpecialized = Json::Input::CustomizeType<CustomizeType1Arg>
        ::as(std::cin, Json::defaultContext, customizeType1Arg);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::haveSpecialization<CustomizeType1Arg, CustomizeType1Arg, Reflect<CustomizeType1Arg>::IndexOf::firstField, NoNote, Reflect<CustomizeType1Arg>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);
}

struct Cacheable
{
    int a;
    short b;

    REFLECT(Cacheable, a, b)
};

TEST_HEADER(JsonInputCacheTest, GetClassFieldCache)
{
    std::multimap<size_t, Json::JsonField> & fieldNameToJsonField =
        Json::Input::getClassFieldCache<Cacheable>();

    bool foundField = false;
    auto aMatches = fieldNameToJsonField.equal_range(Json::Input::Cache::strHash("a"));
    for ( auto it = aMatches.first; it != aMatches.second; ++it )
    {
        if ( it->second.name.compare("a") == 0 )
        {
            foundField = true;
            EXPECT_EQ(size_t(0), it->second.index);
            EXPECT_EQ(Json::JsonField::Type::Regular, it->second.type);
        }
    }
    EXPECT_TRUE(foundField);

    foundField = false;
    auto bMatches = fieldNameToJsonField.equal_range(Json::Input::Cache::strHash("b"));
    for ( auto it = bMatches.first; it != bMatches.second; ++it )
    {
        if ( it->second.name.compare("b") == 0 )
        {
            foundField = true;
            EXPECT_EQ(size_t(1), it->second.index);
            EXPECT_EQ(Json::JsonField::Type::Regular, it->second.type);
        }
    }
    EXPECT_TRUE(foundField);
}

TEST_HEADER(JsonInputCacheTest, GetJsonField)
{
    Json::JsonField* jsonField = Json::Input::getJsonField<Cacheable>("b");
    EXPECT_FALSE(jsonField == nullptr);
    EXPECT_EQ(size_t(1), jsonField->index);
    EXPECT_STREQ("b", jsonField->name.c_str());
    EXPECT_EQ(Json::JsonField::Type::Regular, jsonField->type);
}

TEST_HEADER(JsonInputCacheTest, PutClassFieldCache)
{
    EXPECT_NO_THROW(Json::putClassFieldCache(std::cout));
}

TEST_HEADER(JsonInputCheckedTest, Peek)
{
    char c = '\0';
    std::stringstream empty("");
    bool visited = false;
    try {
        Json::Checked::peek(empty, c, "exception");
    } catch ( Json::UnexpectedInputEnd & ) {
        visited = true;
    }
    EXPECT_TRUE(visited);

    std::stringstream nonEmpty("a");
    Json::Checked::peek(nonEmpty, c, "a");
    EXPECT_EQ('a', c);
}

TEST_HEADER(JsonInputCheckedTest, TryGet)
{
    std::stringstream empty("");
    bool visited = false;
    try {
        Json::Checked::tryGet(empty, 'a', "exception");
    } catch ( Json::UnexpectedInputEnd & ) {
        visited = true;
    }
    EXPECT_TRUE(visited);

    std::stringstream nonEmptyMatch("   \n\t  a");
    EXPECT_TRUE(Json::Checked::tryGet(nonEmptyMatch, 'a', "a"));

    std::stringstream nonEmptyNonMatch("   \n\t  b");
    EXPECT_FALSE(Json::Checked::tryGet(nonEmptyNonMatch, 'a', "a"));
}

TEST_HEADER(JsonInputCheckedTest, TryGetPrimarySecondary)
{
    std::stringstream primaryMatch("a");
    EXPECT_TRUE(Json::Checked::tryGet<true>(primaryMatch, 'a', 'b', "a", "b"));

    std::stringstream secondaryMatch("b");
    EXPECT_TRUE(Json::Checked::tryGet<false>(secondaryMatch, 'a', 'b', "a", "b"));

    std::stringstream primaryNonMatch("b");
    EXPECT_FALSE(Json::Checked::tryGet<true>(primaryNonMatch, 'a', 'b', "a", "b"));

    std::stringstream secondaryNonMatch("a");
    EXPECT_FALSE(Json::Checked::tryGet<false>(secondaryNonMatch, 'a', 'b', "a", "b"));
}

TEST_HEADER(JsonInputCheckedTest, GetTrueFalse)
{
    std::stringstream ssTrueMatch("t");
    EXPECT_TRUE(Json::Checked::getTrueFalse(ssTrueMatch, 't', 'f', "t"));
    std::stringstream ssFalseMatch("f");
    EXPECT_FALSE(Json::Checked::getTrueFalse(ssFalseMatch, 't', 'f', "t"));
    std::stringstream ssNoMatch("n");
    EXPECT_THROW(Json::Checked::getTrueFalse(ssNoMatch, 't', 'f', "t"), Json::Exception);
}

TEST_HEADER(JsonInputCheckedTest, GetTrueFalseSecondaryFalseChar)
{
    std::stringstream primary("b");
    EXPECT_FALSE(Json::Checked::getTrueFalse<true>(primary, 'a', 'b', 'c', "b", "c"));

    std::stringstream secondary("c");
    EXPECT_FALSE(Json::Checked::getTrueFalse<false>(secondary, 'a', 'b', 'c', "b", "c"));
}

TEST_HEADER(JsonInputCheckedTest, GetExpectedChar)
{
    char c = '\0';
    std::stringstream expected("a");
    EXPECT_NO_THROW(Json::Checked::get(expected, c, 'a', "a"));

    std::stringstream unexpected("b");
    EXPECT_THROW(Json::Checked::get(expected, c, 'c', "c"), Json::Exception);
}

TEST_HEADER(JsonInputCheckedTest, GetChar)
{
    char c = '\0';

    std::stringstream whitespaceIgnoredEmpty("  \t\n  ");
    EXPECT_THROW(Json::Checked::get(whitespaceIgnoredEmpty, c, "exception"), Json::UnexpectedInputEnd);

    std::stringstream whitespaceIgnoredNonEmpty("  \t\n  a");
    Json::Checked::get(whitespaceIgnoredNonEmpty, c, "a");
    EXPECT_EQ('a', c);

    std::stringstream empty("");
    EXPECT_THROW(Json::Checked::get(empty, c, "exception"), Json::UnexpectedInputEnd);

    std::stringstream nonEmpty("b");
    Json::Checked::get(nonEmpty, c, "b");
    EXPECT_EQ('b', c);
}

TEST_HEADER(JsonInputCheckedTest, GetSecondaryDescription)
{
    char c = '\0';
    std::stringstream primary;
    EXPECT_THROW(Json::Checked::get<true>(primary, c, "expected", "secondary expected"), Json::UnexpectedInputEnd);

    std::stringstream secondary;
    EXPECT_THROW(Json::Checked::get<false>(primary, c, "expected", "secondary expected"), Json::UnexpectedInputEnd);
}

TEST_HEADER(JsonInputCheckedTest, GetSecondary)
{
    char c = '\0';
    std::stringstream primaryExpected("a");
    EXPECT_NO_THROW(Json::Checked::get<true>(primaryExpected, c, 'a', 'b', "a", "b"));

    std::stringstream secondaryExpected("d");
    EXPECT_NO_THROW(Json::Checked::get<false>(secondaryExpected, c, 'c', 'd', "c", "d"));

    std::stringstream primaryUnexpected("b");
    EXPECT_THROW(Json::Checked::get<true>(primaryExpected, c, 'a', 'b', "a", "b"), Json::Exception);
    
    std::stringstream secondaryUnexpected("c");
    EXPECT_THROW(Json::Checked::get<false>(secondaryExpected, c, 'c', 'd', "c", "d"), Json::Exception);
}

TEST_HEADER(JsonInputCheckedTest, Unget)
{
    char c = '\0';
    std::stringstream unget("asdf");
    Json::Checked::get(unget, c, "a");
    EXPECT_EQ('a', c);
    EXPECT_TRUE(Json::Checked::unget(unget, 'a'));
    
    Json::Checked::get(unget, c, "a");
    EXPECT_EQ('a', c);
    Json::Checked::get(unget, c, "s");
    EXPECT_EQ('s', c);
}

TEST_HEADER(JsonInputCheckedTest, EscapeSequenceGet)
{
    char c = '\0';
    std::stringstream goodSequence("0");
    Json::Checked::escapeSequenceGet(goodSequence, c, "u003");
    EXPECT_EQ('0', c);

    std::stringstream badSequence("");
    EXPECT_THROW(Json::Checked::escapeSequenceGet(badSequence, c, "u003"), Json::InvalidEscapeSequence);
}

TEST_HEADER(JsonInputCheckedTest, ConsumeWhitespace)
{
    std::stringstream empty("");
    EXPECT_THROW(Json::Checked::consumeWhitespace(empty, "exception"), Json::UnexpectedInputEnd);

    std::stringstream onlyWhitespace("  \n\t  ");
    EXPECT_THROW(Json::Checked::consumeWhitespace(onlyWhitespace, "exception"), Json::UnexpectedInputEnd);

    std::stringstream prefixWhitespace("\t\ta");
    EXPECT_NO_THROW(Json::Checked::consumeWhitespace(prefixWhitespace, "noThrow"));
}

TEST_HEADER(JsonInputCheckedTest, ConsumeWhitespacePrimarySecondary)
{
    std::stringstream primary("  a");
    EXPECT_NO_THROW(Json::Checked::consumeWhitespace<true>(primary, "primary", "secondary"));

    std::stringstream secondary("  b");
    EXPECT_NO_THROW(Json::Checked::consumeWhitespace<false>(secondary, "secondary", "secondary"));
}

TEST_HEADER(JsonInputConsumeTest, Null)
{
    char c = '\0';
    std::stringstream validNull("\n null,");
    EXPECT_NO_THROW(Json::Consume::null<true>(validNull, c));

    std::stringstream incompleteNull("nu");
    EXPECT_THROW(Json::Consume::null<true>(incompleteNull, c), Json::UnexpectedInputEnd);
}

TEST_HEADER(JsonInputConsumeTest, NullToStream)
{
    char c = '\0';
    std::stringstream source("null,");
    std::stringstream receiver;
    Json::Consume::null<true>(source, c, receiver);
    EXPECT_STREQ("null", receiver.str().c_str());
}

TEST_HEADER(JsonInputConsumeTest, TryNull)
{
    char c = '\0';
    std::stringstream nonNull("true,");
    std::stringstream partialNull("nula,");
    std::stringstream fullNull("null,");

    EXPECT_FALSE(Json::Consume::tryNull<true>(nonNull, c));
    EXPECT_THROW(Json::Consume::tryNull<true>(partialNull, c), Json::Exception);
    EXPECT_TRUE(Json::Consume::tryNull<true>(fullNull, c));
}

TEST_HEADER(JsonInputConsume, True)
{
    char c = '\0';
    std::stringstream nonTrue("false,");
    std::stringstream partialTrue("tru,");
    std::stringstream fullTrue("true,");

    EXPECT_THROW(Json::Consume::boolTrue<true>(nonTrue, c), Json::Exception);
    EXPECT_THROW(Json::Consume::boolTrue<true>(partialTrue, c), Json::Exception);
    EXPECT_NO_THROW(Json::Consume::boolTrue<true>(fullTrue, c));
}

TEST_HEADER(JsonInputConsume, TrueToStream)
{
    char c = '\0';
    std::stringstream fullTrue("true,");
    std::stringstream receiver;

    EXPECT_NO_THROW(Json::Consume::boolTrue<true>(fullTrue, c, receiver));

    EXPECT_STREQ("true", receiver.str().c_str());
}


TEST_HEADER(JsonInputConsume, False)
{
    char c = '\0';
    std::stringstream nonFalse("true,");
    std::stringstream partialFalse("fals,");
    std::stringstream fullFalse("false,");

    EXPECT_THROW(Json::Consume::boolFalse<true>(nonFalse, c), Json::Exception);
    EXPECT_THROW(Json::Consume::boolFalse<true>(partialFalse, c), Json::Exception);
    EXPECT_NO_THROW(Json::Consume::boolFalse<true>(fullFalse, c));
}

TEST_HEADER(JsonInputConsume, FalseToStream)
{
    char c = '\0';
    std::stringstream fullFalse("false,");
    std::stringstream receiver;

    EXPECT_NO_THROW(Json::Consume::boolFalse<true>(fullFalse, c, receiver));

    EXPECT_STREQ("false", receiver.str().c_str());
}

TEST_HEADER(JsonInputConsume, Number)
{
    char c = '\0';

    std::stringstream nonNumber("asdf,");
    std::stringstream doubleDecimalNumber("123.456.789,");
    std::stringstream doubleNegativeNumber("--5,");
    std::stringstream negativeInteger("-1234,");
    std::stringstream validInteger("2345,");
    std::stringstream validFloat("123.456,");
    
    EXPECT_THROW(Json::Consume::number<true>(nonNumber, c), Json::InvalidNumericCharacter);
    EXPECT_THROW(Json::Consume::number<true>(doubleDecimalNumber, c), Json::InvalidSecondDecimal);
    EXPECT_THROW(Json::Consume::number<true>(doubleNegativeNumber, c), Json::InvalidNumericCharacter);
    EXPECT_NO_THROW(Json::Consume::number<true>(negativeInteger, c));
    EXPECT_NO_THROW(Json::Consume::number<true>(validInteger, c));
    EXPECT_NO_THROW(Json::Consume::number<true>(validFloat, c));
}


TEST_HEADER(JsonInputConsume, NumberToStream)
{
    char c = '\0';

    std::stringstream nonNumber("asdf,");
    std::stringstream doubleDecimalNumber("123.456.789,");
    std::stringstream doubleNegativeNumber("--5,");
    std::stringstream negativeInteger("-1234,");
    std::stringstream validInteger("2345,");
    std::stringstream validFloat("123.456,");

    std::stringstream nonNumberReceiver, doubleDecimalNumberReceiver, doubleNegativeNumberReceiver,
        negativeIntegerReceiver, validIntegerReceiver, validFloatReceiver;
    
    EXPECT_THROW(Json::Consume::number<true>(nonNumber, c, nonNumberReceiver), Json::InvalidNumericCharacter);
    EXPECT_THROW(Json::Consume::number<true>(doubleDecimalNumber, c, doubleDecimalNumberReceiver), Json::InvalidSecondDecimal);
    EXPECT_THROW(Json::Consume::number<true>(doubleNegativeNumber, c, doubleNegativeNumberReceiver), Json::InvalidNumericCharacter);
    EXPECT_NO_THROW(Json::Consume::number<true>(negativeInteger, c, negativeIntegerReceiver));
    EXPECT_NO_THROW(Json::Consume::number<true>(validInteger, c, validIntegerReceiver));
    EXPECT_NO_THROW(Json::Consume::number<true>(validFloat, c, validFloatReceiver));
    
    EXPECT_STREQ("-1234", negativeIntegerReceiver.str().c_str());
    EXPECT_STREQ("2345", validIntegerReceiver.str().c_str());
    EXPECT_STREQ("123.456", validFloatReceiver.str().c_str());
}

TEST_HEADER(JsonInputConsume, String)
{
    char c = '\0';
    std::stringstream input("\"asdf\\\"\\r\\n\\fqwer\"");
    EXPECT_NO_THROW(Json::Consume::string(input, c));
}

TEST_HEADER(JsonInputConsume, StringToStream)
{
    char c = '\0';
    std::stringstream input("\"asdf\\\"\\r\\n\\fqwer\"");
    std::stringstream receiver;
    EXPECT_NO_THROW(Json::Consume::string(input, c, receiver));
    EXPECT_STREQ("\"asdf\\\"\\r\\n\\fqwer\"", receiver.str().c_str());
}

TEST_HEADER(JsonInputConsume, Value)
{
    char c = '\0';
    std::stringstream stringStream("\"asdf\"");
    std::stringstream numberStream("1234,");
    std::stringstream objectStream("{\"one\":1,\"two\":2}");
    std::stringstream arrayStream("[1,2,3]");
    std::stringstream trueStream("true,");
    std::stringstream falseStream("false,");
    std::stringstream nullStream("null,");
    std::stringstream invalidStream("qqqq,");
    
    EXPECT_NO_THROW(Json::Consume::value<true>(stringStream, c));
    EXPECT_NO_THROW(Json::Consume::value<true>(numberStream, c));
    EXPECT_NO_THROW(Json::Consume::value<true>(objectStream, c));
    EXPECT_NO_THROW(Json::Consume::value<true>(arrayStream, c));
    EXPECT_NO_THROW(Json::Consume::value<true>(trueStream, c));
    EXPECT_NO_THROW(Json::Consume::value<true>(falseStream, c));
    EXPECT_NO_THROW(Json::Consume::value<true>(nullStream, c));
    EXPECT_THROW(Json::Consume::value<true>(invalidStream, c), Json::InvalidUnknownFieldValue);
}

TEST_HEADER(JsonInputConsume, ValueToStream)
{
    char c = '\0';
    std::stringstream stringStream("\"asdf\"");
    std::stringstream numberStream("1234,");
    std::stringstream objectStream("{\"one\":1,\"two\":2}");
    std::stringstream arrayStream("[1,2,3]");
    std::stringstream trueStream("true,");
    std::stringstream falseStream("false,");
    std::stringstream nullStream("null,");
    std::stringstream invalidStream("qqqq,");
    
    std::stringstream stringReceiver;
    std::stringstream numberReceiver;
    std::stringstream objectReceiver;
    std::stringstream arrayReceiver;
    std::stringstream trueReceiver;
    std::stringstream falseReceiver;
    std::stringstream nullReceiver;
    std::stringstream invalidReceiver;
    
    EXPECT_NO_THROW(Json::Consume::value<true>(stringStream, c, stringReceiver));
    EXPECT_NO_THROW(Json::Consume::value<true>(numberStream, c, numberReceiver));
    EXPECT_NO_THROW(Json::Consume::value<true>(objectStream, c, objectReceiver));
    EXPECT_NO_THROW(Json::Consume::value<true>(arrayStream, c, arrayReceiver));
    EXPECT_NO_THROW(Json::Consume::value<true>(trueStream, c, trueReceiver));
    EXPECT_NO_THROW(Json::Consume::value<true>(falseStream, c, falseReceiver));
    EXPECT_NO_THROW(Json::Consume::value<true>(nullStream, c, nullReceiver));
    EXPECT_THROW(Json::Consume::value<true>(invalidStream, c, invalidReceiver), Json::InvalidUnknownFieldValue);
    
    EXPECT_STREQ("\"asdf\"", stringReceiver.str().c_str());
    EXPECT_STREQ("1234", numberReceiver.str().c_str());
    EXPECT_STREQ("{\"one\":1,\"two\":2}", objectReceiver.str().c_str());
    EXPECT_STREQ("[1,2,3]", arrayReceiver.str().c_str());
    EXPECT_STREQ("true", trueReceiver.str().c_str());
    EXPECT_STREQ("false", falseReceiver.str().c_str());
    EXPECT_STREQ("null", nullReceiver.str().c_str());
}

TEST_HEADER(JsonInputConsume, Iterable)
{
    char c = '\0';
    std::stringstream stringArray("[\"asdf\",\"qwer\"]");
    std::stringstream numberArray("[1,-2,-2.3,5.5,62312]");
    std::stringstream objectArray("[{\"one\":1,\"two\":2},{\"three\":3},{}]");
    std::stringstream arrayArray("[[1,2,3],[],[4,5,6]]");
    std::stringstream boolArray("[true,false,true,true,false]");
    std::stringstream nullArray("[null,null,null]");
    std::stringstream mixedArray("[null,1,\"two\",{},[]]");
    
    EXPECT_NO_THROW(Json::Consume::iterable<true>(stringArray, c));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(numberArray, c));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(objectArray, c));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(arrayArray, c));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(boolArray, c));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(nullArray, c));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(mixedArray, c));
}

TEST_HEADER(JsonInputConsume, IterableToStream)
{
    char c = '\0';
    std::stringstream emptyArray("[]");
    std::stringstream stringArray("[\"asdf\",\"qwer\"]");
    std::stringstream numberArray("[1,-2,-2.3,5.5,62312]");
    std::stringstream objectArray("[{\"one\":1,\"two\":2},{\"three\":3},{}]");
    std::stringstream arrayArray("[[1,2,3],[],[4,5,6]]");
    std::stringstream boolArray("[true,false,true,true,false]");
    std::stringstream nullArray("[null,null,null]");
    std::stringstream mixedArray("[null,1,\"two\",{},[]]");
    std::stringstream emptyObj("{}");
    std::stringstream obj("{\"one\":\"str\",\"two\":2,\"three\":{},\"four\":[],\"five\":true,\"six\":null}");
    
    std::stringstream emptyArrayReceiver;
    std::stringstream stringArrayReceiver;
    std::stringstream numberArrayReceiver;
    std::stringstream objectArrayReceiver;
    std::stringstream arrayArrayReceiver;
    std::stringstream boolArrayReceiver;
    std::stringstream nullArrayReceiver;
    std::stringstream mixedArrayReceiver;
    std::stringstream emptyObjReceiver;
    std::stringstream objReceiver;
    
    EXPECT_NO_THROW(Json::Consume::iterable<true>(emptyArray, c, emptyArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(stringArray, c, stringArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(numberArray, c, numberArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(objectArray, c, objectArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(arrayArray, c, arrayArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(boolArray, c, boolArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(nullArray, c, nullArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<true>(mixedArray, c, mixedArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<false>(emptyObj, c, emptyObjReceiver));
    EXPECT_NO_THROW(Json::Consume::iterable<false>(obj, c, objReceiver));
    
    EXPECT_STREQ("[]", emptyArrayReceiver.str().c_str());
    EXPECT_STREQ("[\"asdf\",\"qwer\"]", stringArrayReceiver.str().c_str());
    EXPECT_STREQ("[1,-2,-2.3,5.5,62312]", numberArrayReceiver.str().c_str());
    EXPECT_STREQ("[{\"one\":1,\"two\":2},{\"three\":3},{}]", objectArrayReceiver.str().c_str());
    EXPECT_STREQ("[[1,2,3],[],[4,5,6]]", arrayArrayReceiver.str().c_str());
    EXPECT_STREQ("[true,false,true,true,false]", boolArrayReceiver.str().c_str());
    EXPECT_STREQ("[null,null,null]", nullArrayReceiver.str().c_str());
    EXPECT_STREQ("[null,1,\"two\",{},[]]", mixedArrayReceiver.str().c_str());
    EXPECT_STREQ("{}", emptyObjReceiver.str().c_str());
    EXPECT_STREQ("{\"one\":\"str\",\"two\":2,\"three\":{},\"four\":[],\"five\":true,\"six\":null}", objReceiver.str().c_str());
}

TEST_HEADER(JsonInputReadAffix, ObjectPrefix)
{
    char c = '\0';
    std::stringstream objPrefix("{");
    std::stringstream arrayPrefix("[");
    std::stringstream letter("a");
    
    EXPECT_NO_THROW(Json::Read::objectPrefix(objPrefix, c));
    EXPECT_THROW(Json::Read::objectPrefix(arrayPrefix, c), Json::Exception);
    EXPECT_THROW(Json::Read::objectPrefix(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, TryObjectSuffix)
{
    std::stringstream objSuffix("}");
    std::stringstream arraySuffix("]");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::tryObjectSuffix(objSuffix));
    EXPECT_FALSE(Json::Read::tryObjectSuffix(arraySuffix));
    EXPECT_FALSE(Json::Read::tryObjectSuffix(letter));
}

TEST_HEADER(JsonInputReadAffix, FieldSeparator)
{
    std::stringstream fieldSeparator(",");
    std::stringstream objSuffix("}");
    std::stringstream arraySuffix("]");
    std::stringstream letter("a");

    EXPECT_TRUE(Json::Read::fieldSeparator(fieldSeparator));
    EXPECT_FALSE(Json::Read::fieldSeparator(objSuffix));
    EXPECT_THROW(Json::Read::fieldSeparator(arraySuffix), Json::Exception);
    EXPECT_THROW(Json::Read::fieldSeparator(letter), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, FieldNameValueSeparator)
{
    char c = '\0';
    std::stringstream fieldNameValueSeparator(":");
    std::stringstream letter("a");
    EXPECT_NO_THROW(Json::Read::fieldNameValueSeparator(fieldNameValueSeparator, c));
    EXPECT_THROW(Json::Read::fieldNameValueSeparator(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, ArrayPrefix)
{
    char c = '\0';
    std::stringstream arrayPrefix("[");
    std::stringstream objectPrefix("{");
    std::stringstream letter("a");
    EXPECT_NO_THROW(Json::Read::arrayPrefix(arrayPrefix, c));
    EXPECT_THROW(Json::Read::arrayPrefix(objectPrefix, c), Json::Exception);
    EXPECT_THROW(Json::Read::arrayPrefix(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, TrySingularTupleArrayPrefix)
{
    char c = '\0';
    std::stringstream arrayPrefix("[");
    std::stringstream objectPrefix("{");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::trySingularTupleArrayPrefix(arrayPrefix, c));
    EXPECT_FALSE(Json::Read::trySingularTupleArrayPrefix(objectPrefix, c));
    EXPECT_FALSE(Json::Read::trySingularTupleArrayPrefix(letter, c));
}

TEST_HEADER(JsonInputReadAffix, TryArraySuffix)
{
    std::stringstream arraySuffix("]");
    std::stringstream objectSuffix("}");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::tryArraySuffix(arraySuffix));
    EXPECT_FALSE(Json::Read::tryArraySuffix(objectSuffix));
    EXPECT_FALSE(Json::Read::tryArraySuffix(letter));
}

TEST_HEADER(JsonInputReadAffix, EitherIterablePrefix)
{
    char c = '\0';
    std::stringstream objectPrefix("{");
    std::stringstream arrayPrefix("[");
    std::stringstream objectSuffix("}");
    std::stringstream arraySuffix("}");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::iterablePrefix(objectPrefix, c));
    EXPECT_FALSE(Json::Read::iterablePrefix(arrayPrefix, c));
    EXPECT_THROW(Json::Read::iterablePrefix(objectSuffix, c), Json::Exception);
    EXPECT_THROW(Json::Read::iterablePrefix(arraySuffix, c), Json::Exception);
    EXPECT_THROW(Json::Read::iterablePrefix(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, PeekIterablePrefix)
{
    char c = '\0';
    std::stringstream objectPrefix("{");
    std::stringstream arrayPrefix("[");
    std::stringstream objectSuffix("}");
    std::stringstream arraySuffix("}");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::peekIterablePrefix(objectPrefix, c));
    EXPECT_FALSE(Json::Read::peekIterablePrefix(arrayPrefix, c));
    EXPECT_THROW(Json::Read::peekIterablePrefix(objectSuffix, c), Json::Exception);
    EXPECT_THROW(Json::Read::peekIterablePrefix(arraySuffix, c), Json::Exception);
    EXPECT_THROW(Json::Read::peekIterablePrefix(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, SpecificIterablePrefix)
{
    char c = '\0';
    std::stringstream objectPrefixExpected("{");
    std::stringstream objectPrefixUnexpected("{");
    std::stringstream arrayPrefixExpected("[");
    std::stringstream arrayPrefixUnexpected("[");
    
    EXPECT_NO_THROW(Json::Read::iterablePrefix<true>(objectPrefixExpected, c));
    EXPECT_THROW(Json::Read::iterablePrefix<false>(objectPrefixUnexpected, c), Json::Exception);
    EXPECT_NO_THROW(Json::Read::iterablePrefix<false>(arrayPrefixExpected, c));
    EXPECT_THROW(Json::Read::iterablePrefix<true>(arrayPrefixUnexpected, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, TryIterableSuffix)
{
    std::stringstream objectSuffixExpected("}");
    std::stringstream objectSuffixUnexpected("}");
    std::stringstream arraySuffixExpected("]");
    std::stringstream arraySuffixUnexpected("]");
    
    EXPECT_TRUE(Json::Read::tryIterableSuffix<true>(objectSuffixExpected));
    EXPECT_FALSE(Json::Read::tryIterableSuffix<false>(objectSuffixUnexpected));
    EXPECT_TRUE(Json::Read::tryIterableSuffix<false>(arraySuffixExpected));
    EXPECT_FALSE(Json::Read::tryIterableSuffix<true>(arraySuffixUnexpected));
}

TEST_HEADER(JsonInputReadAffix, TryIterableSuffixInstanced)
{
    std::stringstream objectSuffixExpected("}");
    std::stringstream objectSuffixUnexpected("}");
    std::stringstream arraySuffixExpected("]");
    std::stringstream arraySuffixUnexpected("]");
    
    EXPECT_TRUE(Json::Read::tryIterableSuffix(objectSuffixExpected, true));
    EXPECT_FALSE(Json::Read::tryIterableSuffix(objectSuffixUnexpected, false));
    EXPECT_TRUE(Json::Read::tryIterableSuffix(arraySuffixExpected, false));
    EXPECT_FALSE(Json::Read::tryIterableSuffix(arraySuffixUnexpected, true));
}

TEST_HEADER(JsonInputReadAffix, IterableElementSeparator)
{
    std::stringstream iterableElementSeparator(",");
    std::stringstream objectTerminator("}");
    std::stringstream objectTerminatorUnexpected("}");
    std::stringstream arrayTerminator("]");
    std::stringstream arrayTerminatorUnexpected("]");
    
    EXPECT_TRUE(Json::Read::iterableElementSeparator<true>(iterableElementSeparator));
    EXPECT_FALSE(Json::Read::iterableElementSeparator<true>(objectTerminator));
    EXPECT_THROW(Json::Read::iterableElementSeparator<false>(objectTerminatorUnexpected), Json::Exception);
    EXPECT_FALSE(Json::Read::iterableElementSeparator<false>(arrayTerminator));
    EXPECT_THROW(Json::Read::iterableElementSeparator<true>(arrayTerminatorUnexpected), Json::Exception);
}

TEST_HEADER(JsonInputRead, Customization)
{
    CustomizeUnspecialized customizeUnspecialized;
    CustomizeFullySpecialized customizeFullySpecialized;
    Customize5Args_OpAnnotationsDefaulted customize5Args_OpAnnotationsDefaulted;
    Customize5Args_MemberIndexDefaulted customize5Args_MemberIndexDefaulted;
    Customize5Args_BothDefaulted customize5Args_BothDefaulted;
    Customize4Args customize4Args;
    Customize4Args_MemberIndexDefaulted customize4Args_MemberIndexDefaulted;
    Customize3Args customize3Args;
    Customize2Args customize2Args;
    CustomizeTypeFullySpecialized customizeTypeFullySpecialized;
    CustomizeType3Args_OpAnnotationsDefaulted customizeType3Args_OpAnnotationsDefaulted;
    CustomizeType2Args customizeType2Args;
    CustomizeType1Arg customizeType1Arg;

    using AMember = Json::MockMember<int>;

    bool customized = Json::Read::customization<CustomizeUnspecialized, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customizeUnspecialized, customizeUnspecialized.firstField);
    EXPECT_FALSE(customized);
    customized = Json::Read::customization<CustomizeFullySpecialized, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customizeFullySpecialized, customizeFullySpecialized.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<Customize5Args_OpAnnotationsDefaulted, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customize5Args_OpAnnotationsDefaulted, customize5Args_OpAnnotationsDefaulted.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<Customize5Args_MemberIndexDefaulted, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customize5Args_MemberIndexDefaulted, customize5Args_MemberIndexDefaulted.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<Customize5Args_BothDefaulted, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customize5Args_BothDefaulted, customize5Args_BothDefaulted.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<Customize4Args, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customize4Args, customize4Args.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<Customize4Args_MemberIndexDefaulted, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customize4Args_MemberIndexDefaulted, customize4Args_MemberIndexDefaulted.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<Customize3Args, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customize3Args, customize3Args.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<Customize2Args, int, 0, NoNote, AMember>(std::cin, Json::defaultContext, customize2Args, customize2Args.firstField);
    EXPECT_TRUE(customized);
    std::stringstream input("0");
    customized = Json::Read::customization<CustomizeTypeFullySpecialized, CustomizeTypeFullySpecialized, 0, NoNote, AMember>(input, Json::defaultContext, customizeTypeFullySpecialized, customizeTypeFullySpecialized);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<CustomizeType3Args_OpAnnotationsDefaulted, CustomizeType3Args_OpAnnotationsDefaulted, 0, NoNote, AMember>(std::cin, Json::defaultContext, customizeType3Args_OpAnnotationsDefaulted, customizeType3Args_OpAnnotationsDefaulted);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<CustomizeType2Args, CustomizeType2Args, 0, NoNote, AMember>(std::cin, Json::defaultContext, customizeType2Args, customizeType2Args);
    EXPECT_TRUE(customized);
    customized = Json::Read::customization<CustomizeType1Arg, CustomizeType1Arg, 0, NoNote, AMember>(std::cin, Json::defaultContext, customizeType1Arg, customizeType1Arg);
    EXPECT_TRUE(customized);
}

TEST_HEADER(JsonInputRead, True)
{
    char c = '\0';
    std::stringstream trueStream("true,");
    std::stringstream falseStream("false,");
    EXPECT_TRUE(Json::Read::boolTrue<true>(trueStream, c));
    EXPECT_THROW(Json::Read::boolTrue<true>(falseStream, c), Json::Exception);
}

TEST_HEADER(JsonInputRead, False)
{
    char c = '\0';
    std::stringstream falseStream("false,");
    std::stringstream trueStream("true,");
    EXPECT_FALSE(Json::Read::boolFalse<true>(falseStream, c));
    EXPECT_THROW(Json::Read::boolFalse<true>(trueStream, c), Json::Exception);
}

TEST_HEADER(JsonInputRead, BoolReference)
{
    char c = '\0';
    bool value = false;

    std::stringstream trueStream("true,");
    Json::Read::boolean<true>(trueStream, c, value);
    EXPECT_TRUE(value);

    std::stringstream falseStream("false,");
    Json::Read::boolean<true>(falseStream, c, value);
    EXPECT_FALSE(value);

    std::stringstream invalidStream("asdf");
    EXPECT_THROW(Json::Read::boolean<true>(invalidStream, c, value), Json::Exception);
}

TEST_HEADER(JsonInputRead, BoolReturn)
{
    char c = '\0';
    
    std::stringstream trueStream("true,");
    EXPECT_TRUE(Json::Read::boolean<true>(trueStream, c));

    std::stringstream falseStream("false,");
    EXPECT_FALSE(Json::Read::boolean<true>(falseStream, c));

    std::stringstream invalidStream("asdf");
    EXPECT_THROW(Json::Read::boolean<true>(invalidStream, c), Json::Exception);
}

TEST_HEADER(JsonInputRead, Number)
{
    char c = '\0';
    std::stringstream validNumber("1234,");
    std::string result = Json::Input::Read::number<true>(validNumber, c);
    EXPECT_STREQ("1234", result.c_str());

    std::stringstream invalidNumber("asdf,");
    EXPECT_THROW(Json::Input::Read::number<true>(invalidNumber, c), Json::Exception);
}

TEST_HEADER(JsonInputRead, StringToStream)
{
    char c = '\0';
    std::stringstream emptyStr("\"\"\"");
    std::stringstream basicStr("\"asdf\"");
    std::stringstream quoteStr("\"\\\"\"");
    std::stringstream backslashStr("\"\\\\\"");
    std::stringstream forwardslashStr("\"\\/\"");
    std::stringstream backspaceStr("\"\\b\"");
    std::stringstream formFeedStr("\"\\f\"");
    std::stringstream lineFeedStr("\"\\n\"");
    std::stringstream carriageReturnStr("\"\\r\"");
    std::stringstream tabStr("\"\\t\"");
    std::stringstream unicodeCharacterStr("\"\\u0030\"");
    std::stringstream everything("\" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 \"");

    std::stringstream emptyStrReceiver;
    std::stringstream basicStrReceiver;
    std::stringstream quoteStrReceiver;
    std::stringstream backslashStrReceiver;
    std::stringstream forwardslashStrReceiver;
    std::stringstream backspaceStrReceiver;
    std::stringstream formFeedStrReceiver;
    std::stringstream lineFeedStrReceiver;
    std::stringstream carriageReturnStrReceiver;
    std::stringstream tabStrReceiver;
    std::stringstream unicodeCharacterStrReceiver;
    std::stringstream everythingReceiver;

    Json::Read::string<>(emptyStr, c, emptyStrReceiver);
    Json::Read::string<>(basicStr, c, basicStrReceiver);
    Json::Read::string<>(quoteStr, c, quoteStrReceiver);
    Json::Read::string<>(backslashStr, c, backslashStrReceiver);
    Json::Read::string<>(forwardslashStr, c, forwardslashStrReceiver);
    Json::Read::string<>(backspaceStr, c, backspaceStrReceiver);
    Json::Read::string<>(formFeedStr, c, formFeedStrReceiver);
    Json::Read::string<>(lineFeedStr, c, lineFeedStrReceiver);
    Json::Read::string<>(carriageReturnStr, c, carriageReturnStrReceiver);
    Json::Read::string<>(tabStr, c, tabStrReceiver);
    Json::Read::string<>(unicodeCharacterStr, c, unicodeCharacterStrReceiver);
    Json::Read::string<>(everything, c, everythingReceiver);

    EXPECT_STREQ("", emptyStrReceiver.str().c_str());
    EXPECT_STREQ("asdf", basicStrReceiver.str().c_str());
    EXPECT_STREQ("\"", quoteStrReceiver.str().c_str());
    EXPECT_STREQ("\\", backslashStrReceiver.str().c_str());
    EXPECT_STREQ("/", forwardslashStrReceiver.str().c_str());
    EXPECT_STREQ("\b", backspaceStrReceiver.str().c_str());
    EXPECT_STREQ("\f", formFeedStrReceiver.str().c_str());
    EXPECT_STREQ("\n", lineFeedStrReceiver.str().c_str());
    EXPECT_STREQ("\r", carriageReturnStrReceiver.str().c_str());
    EXPECT_STREQ("\t", tabStrReceiver.str().c_str());
    EXPECT_STREQ("0", unicodeCharacterStrReceiver.str().c_str());
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", everythingReceiver.str().c_str());
}

TEST_HEADER(JsonInputRead, UnquotedStringToStream)
{
    char c = '\0';
    std::stringstream emptyStr("");
    std::stringstream basicStr("asdf");
    std::stringstream quoteStr("\\\"");
    std::stringstream backslashStr("\\\\");
    std::stringstream forwardslashStr("\\/");
    std::stringstream backspaceStr("\\b");
    std::stringstream formFeedStr("\\f");
    std::stringstream lineFeedStr("\\n");
    std::stringstream carriageReturnStr("\\r");
    std::stringstream tabStr("\\t");
    std::stringstream unicodeCharacterStr("\\u0030");
    std::stringstream everything(" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 ");

    std::stringstream emptyStrReceiver;
    std::stringstream basicStrReceiver;
    std::stringstream quoteStrReceiver;
    std::stringstream backslashStrReceiver;
    std::stringstream forwardslashStrReceiver;
    std::stringstream backspaceStrReceiver;
    std::stringstream formFeedStrReceiver;
    std::stringstream lineFeedStrReceiver;
    std::stringstream carriageReturnStrReceiver;
    std::stringstream tabStrReceiver;
    std::stringstream unicodeCharacterStrReceiver;
    std::stringstream everythingReceiver;

    Json::Read::string<false>(emptyStr, c, emptyStrReceiver);
    Json::Read::string<false>(basicStr, c, basicStrReceiver);
    Json::Read::string<false>(quoteStr, c, quoteStrReceiver);
    Json::Read::string<false>(backslashStr, c, backslashStrReceiver);
    Json::Read::string<false>(forwardslashStr, c, forwardslashStrReceiver);
    Json::Read::string<false>(backspaceStr, c, backspaceStrReceiver);
    Json::Read::string<false>(formFeedStr, c, formFeedStrReceiver);
    Json::Read::string<false>(lineFeedStr, c, lineFeedStrReceiver);
    Json::Read::string<false>(carriageReturnStr, c, carriageReturnStrReceiver);
    Json::Read::string<false>(tabStr, c, tabStrReceiver);
    Json::Read::string<false>(unicodeCharacterStr, c, unicodeCharacterStrReceiver);
    Json::Read::string<false>(everything, c, everythingReceiver);

    EXPECT_STREQ("", emptyStrReceiver.str().c_str());
    EXPECT_STREQ("asdf", basicStrReceiver.str().c_str());
    EXPECT_STREQ("\"", quoteStrReceiver.str().c_str());
    EXPECT_STREQ("\\", backslashStrReceiver.str().c_str());
    EXPECT_STREQ("/", forwardslashStrReceiver.str().c_str());
    EXPECT_STREQ("\b", backspaceStrReceiver.str().c_str());
    EXPECT_STREQ("\f", formFeedStrReceiver.str().c_str());
    EXPECT_STREQ("\n", lineFeedStrReceiver.str().c_str());
    EXPECT_STREQ("\r", carriageReturnStrReceiver.str().c_str());
    EXPECT_STREQ("\t", tabStrReceiver.str().c_str());
    EXPECT_STREQ("0", unicodeCharacterStrReceiver.str().c_str());
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", everythingReceiver.str().c_str());
}

TEST_HEADER(JsonInputRead, StringReference)
{
    char c = '\0';
    std::stringstream everything("\" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 \"");
    std::string result;
    Json::Read::string<>(everything, c, result);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

TEST_HEADER(JsonInputRead, StringReferenceUnquoted)
{
    char c = '\0';
    std::stringstream everything(" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 ");
    std::string result;
    Json::Read::string<false>(everything, c, result);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

TEST_HEADER(JsonInputRead, StringTemplate)
{
    char c = '\0';
    std::stringstream integerStream("\"1234\"");
    int result = 0;
    Json::Read::string<>(integerStream, c, result);
    EXPECT_EQ(1234, result);
}

TEST_HEADER(JsonInputRead, StringTemplateUnquoted)
{
    char c = '\0';
    std::stringstream integerStream("1234");
    int result = 0;
    Json::Read::string<int, false>(integerStream, c, result);
    EXPECT_EQ(1234, result);
}

TEST_HEADER(JsonInputRead, StringCharReturned)
{
    char c = '\0';
    std::stringstream everything("\" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 \"");
    std::string result = Json::Read::string<>(everything, c);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

TEST_HEADER(JsonInputRead, StringCharReturnedUnquoted)
{
    char c = '\0';
    std::stringstream everything(" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 ");
    std::string result = Json::Read::string<false>(everything, c);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

TEST_HEADER(JsonInputRead, StringReturned)
{
    std::stringstream everything("\" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 \"");
    std::string result = Json::Read::string<>(everything);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

TEST_HEADER(JsonInputRead, StringReturnedUnquoted)
{
    std::stringstream everything(" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 ");
    std::string result = Json::Read::string<false>(everything);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

enum class EnumIntEnum : uint32_t {
    none = 0,
    leet = 1337,
    overNineThousand = 9001
};

TEST_HEADER(JsonInputRead, EnumInt)
{
    std::stringstream leet("1337");
    std::stringstream overNineThousand("9001");
    EnumIntEnum result = EnumIntEnum::none;
    Json::Read::enumInt(leet, result);
    EXPECT_EQ(EnumIntEnum::leet, result);
    Json::Read::enumInt(overNineThousand, result);
    EXPECT_EQ(EnumIntEnum::overNineThousand, result);
}

TEST_HEADER(JsonInputRead, FieldName)
{
    char c = '\0';
    std::stringstream fieldNameStream("\"asdf\"");
    std::string fieldName = Json::Read::fieldName(fieldNameStream, c);
    EXPECT_STREQ("asdf", fieldName.c_str());
}

TEST_HEADER(JsonInputRead, GenericValue)
{
    char c = '\0';
    std::shared_ptr<Json::Value::Assigner> assigner;
    
    std::stringstream stringStream("\"string\"");
    std::stringstream objectStream("{}");
    std::stringstream arrayStream("[]");
    std::stringstream boolStream("true,");
    std::stringstream numberStream("1234,");
    std::stringstream nullStream("null,");
    std::stringstream invalidStream("qqq,");
    
    assigner = Json::Read::genericValue<true, false>(stringStream, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::String, assigner->get()->type());
    assigner = Json::Read::genericValue<true, false>(objectStream, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::Object, assigner->get()->type());
    assigner = Json::Read::genericValue<true, false>(arrayStream, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::NullArray, assigner->get()->type());
    assigner = Json::Read::genericValue<true, false>(boolStream, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::Boolean, assigner->get()->type());
    assigner = Json::Read::genericValue<true, false>(numberStream, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::Number, assigner->get()->type());
    assigner = Json::Read::genericValue<true, false>(nullStream, Json::defaultContext, c);
    EXPECT_TRUE(assigner->get() == nullptr);

    bool throwedAsExpected = false;
    try {
        Json::Read::genericValue<true, false>(invalidStream, Json::defaultContext, c);
    } catch ( Json::InvalidUnknownFieldValue ) {
        throwedAsExpected = true;
    }
    EXPECT_TRUE(throwedAsExpected);
}

TEST_HEADER(JsonInputRead, ValueType)
{
    char c = '\"';
    EXPECT_EQ(Json::Value::Type::String, Json::Read::valueType<false>(c));

    c = '{';
    EXPECT_EQ(Json::Value::Type::Object, Json::Read::valueType<false>(c));

    c = '[';
    EXPECT_EQ(Json::Value::Type::Array, Json::Read::valueType<false>(c));

    c = 't';
    EXPECT_EQ(Json::Value::Type::Boolean, Json::Read::valueType<false>(c));
    c = 'f';
    EXPECT_EQ(Json::Value::Type::Boolean, Json::Read::valueType<false>(c));

    c = '-';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '0';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '1';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '2';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '3';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '4';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '5';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '6';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '7';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '8';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));
    c = '9';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::valueType<false>(c));

    c = 'n';
    EXPECT_EQ(Json::Value::Type::Null, Json::Read::valueType<false>(c));

    c = 'q';
    EXPECT_THROW(Json::Read::valueType<false>(c), Json::InvalidUnknownFieldValue);
}

TEST_HEADER(JsonInputRead, GenericArray)
{
    char c = '\0';
    std::stringstream emptyArray("[]");
    std::stringstream nullArray("[null, null, null]");
    std::stringstream boolArray("[true, false, false, true]");
    std::stringstream numberArray("[12, 13, 13.5, 15, -12.322, -5, 6]");
    std::stringstream stringArray("[\"asdf\",\"qwerty\"]");
    std::stringstream objectArray("[{}, {\"field\":true}, {\"str\":\"hello world\",\"null\":null,\"num\":42}]");
    std::stringstream mixedArray("[null, false]");
    std::stringstream nestedArrays("[ [[true, false], [null, null]], [[\"as\"], [{\"df\":\"qw\"}]] ]");

    std::shared_ptr<Json::Value::Assigner> assigner = nullptr;

    assigner = Json::Read::genericArray<true, false>(emptyArray, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::NullArray, assigner->get()->type());

    assigner = Json::Read::genericArray<true, false>(nullArray, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::NullArray, assigner->get()->type());
    EXPECT_EQ(size_t(3), assigner->get()->nullArray());

    assigner = Json::Read::genericArray<true, false>(boolArray, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::BoolArray, assigner->get()->type());
    EXPECT_TRUE(assigner->get()->boolArray()[0]);
    EXPECT_FALSE(assigner->get()->boolArray()[1]);
    EXPECT_FALSE(assigner->get()->boolArray()[2]);
    EXPECT_TRUE(assigner->get()->boolArray()[3]);
    
    assigner = Json::Read::genericArray<true, false>(numberArray, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::NumberArray, assigner->get()->type());
    EXPECT_EQ(size_t(7), assigner->get()->arraySize());
    EXPECT_STREQ("12", assigner->get()->numberArray()[0].c_str());
    EXPECT_STREQ("13", assigner->get()->numberArray()[1].c_str());
    EXPECT_STREQ("13.5", assigner->get()->numberArray()[2].c_str());
    EXPECT_STREQ("15", assigner->get()->numberArray()[3].c_str());
    EXPECT_STREQ("-12.322", assigner->get()->numberArray()[4].c_str());
    EXPECT_STREQ("-5", assigner->get()->numberArray()[5].c_str());
    EXPECT_STREQ("6", assigner->get()->numberArray()[6].c_str());

    assigner = Json::Read::genericArray<true, false>(stringArray, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::StringArray, assigner->get()->type());
    EXPECT_EQ(size_t(2), assigner->get()->arraySize());
    EXPECT_STREQ("asdf", assigner->get()->stringArray()[0].c_str());
    EXPECT_STREQ("qwerty", assigner->get()->stringArray()[1].c_str());

    assigner = Json::Read::genericArray<true, false>(objectArray, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::ObjectArray, assigner->get()->type());
    EXPECT_EQ(size_t(3), assigner->get()->arraySize());
    EXPECT_TRUE(assigner->get()->objectArray()[0].empty());
    EXPECT_TRUE(assigner->get()->objectArray()[1].find("field")->second->boolean());
    EXPECT_STREQ("hello world", assigner->get()->objectArray()[2].find("str")->second->string().c_str());
    EXPECT_TRUE(assigner->get()->objectArray()[2].find("null")->second == nullptr);
    EXPECT_STREQ("42", assigner->get()->objectArray()[2].find("num")->second->number().c_str());

    assigner = Json::Read::genericArray<true, false>(mixedArray, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::MixedArray, assigner->get()->type());
    EXPECT_EQ(size_t(2), assigner->get()->arraySize());
    EXPECT_TRUE(assigner->get()->mixedArray()[0] == nullptr);
    EXPECT_TRUE(assigner->get()->mixedArray()[1]->boolean() == false);

    assigner = Json::Read::genericArray<true, false>(nestedArrays, Json::defaultContext, c);
    EXPECT_EQ(Json::Value::Type::MixedArray, assigner->get()->type());
    EXPECT_EQ(size_t(2), assigner->get()->arraySize());
    EXPECT_TRUE(assigner->get()->mixedArray()[0]->mixedArray()[0]->boolArray()[0]);
    EXPECT_FALSE(assigner->get()->mixedArray()[0]->mixedArray()[0]->boolArray()[1]);
    EXPECT_EQ(size_t(2), assigner->get()->mixedArray()[0]->mixedArray()[1]->nullArray());
    EXPECT_STREQ("as", assigner->get()->mixedArray()[1]->mixedArray()[0]->stringArray()[0].c_str());
    EXPECT_STREQ("qw", assigner->get()->mixedArray()[1]->mixedArray()[1]->objectArray()[0].find("df")->second->string().c_str());
}

TEST_HEADER(JsonInputRead, GenericObject)
{
    char c = '\0';
    std::shared_ptr<Json::Value::Assigner> assigner = nullptr;

    std::stringstream emptyObject("{}");
    std::stringstream basicObject("{\"null\":null,\"one\":1,\"two\":2}");
    
    assigner = Json::Read::genericObject<false>(emptyObject, Json::defaultContext, c);
    EXPECT_TRUE(assigner->get()->object().empty());

    assigner = Json::Read::genericObject<false>(basicObject, Json::defaultContext, c);
    EXPECT_TRUE(assigner->get()->object().find("null")->second == nullptr);
    EXPECT_STREQ("1", assigner->get()->object().find("one")->second->number().c_str());
    EXPECT_STREQ("2", assigner->get()->object().find("two")->second->number().c_str());
}

struct Keyable
{
    int a;

    REFLECT(Keyable, a)
};

bool operator<(const Keyable & lhs, const Keyable & rhs)
{
    return lhs.a < rhs.a;
}

struct ComposedObj
{
    int a;

    REFLECT(ComposedObj, a)
};

struct MappedFrom
{
    int a;

    REFLECT(MappedFrom, a)
};

struct MappedTo
{
    int a;

    REFLECT(MappedTo, a)

    NOTE(ObjectMappings, RareMapper::createMapping<MappedFrom, MappedTo>().a->a().unidirectional())
};

struct VariousValues
{
    VariousValues(int & integer) : customized({}), genericNonNull(nullptr), genericNull(nullptr),
        sharedPointerNull(nullptr), uniquePointerNull(nullptr),
        regularPointerNull(nullptr), regularPointerToBecomeNull(nullptr), regularPointerValue(nullptr),
        constIntPointerNull(nullptr), constIntPointerToBecomeNull(&integer), constIntPointerValue(&integer),
        intConstPointerNull(nullptr), intConstPointerToBecomeNull(&integer), intConstPointerValue(&integer),
        constIntConstPointerNull(nullptr), constIntConstPointerToBecomeNull(&integer), constIntConstPointerValue(&integer),
        genericValue(), genericObjectArray(), intVector(), composedObj({}), integerString(0), enumInt(EnumIntEnum::none), replaceableInt(0), boolean(false),
        str(), mappedTo({}), emptyTuple({}), singleTuple({}), doubleTuple({}), tripleTuple({}), pair({}),
        primitiveKey(), complexKey(), intIntArrayTuple({}), intIntKeyableTuple({}), intIntTupleTuple({}) {}
        

    CustomizeTypeFullySpecialized customized;
    
    std::shared_ptr<Json::Value> genericNonNull;
    std::shared_ptr<Json::Value> genericNull;

    std::shared_ptr<int> sharedPointerNull;
    std::unique_ptr<int> uniquePointerNull;
    int* regularPointerNull;

    int* regularPointerToBecomeNull;
    int* regularPointerValue;
    
    const int* constIntPointerNull;
    const int* constIntPointerToBecomeNull;
    const int* constIntPointerValue;
    
    int* const intConstPointerNull;
    int* const intConstPointerToBecomeNull;
    int* const intConstPointerValue;
    
    const int* const constIntConstPointerNull;
    const int* const constIntConstPointerToBecomeNull;
    const int* const constIntConstPointerValue;

    Json::Object genericValue;
    Json::ObjectArray genericObjectArray;
    std::vector<int> intVector;
    ComposedObj composedObj;
    NOTE(integerString, Json::Stringify)
    int integerString;
    NOTE(enumInt, Json::EnumInt)
    EnumIntEnum enumInt;
    NOTE(replaceableInt, Json::Replace{"\"\"", -222})
    int replaceableInt;
    bool boolean;
    static constexpr const int constant = 0;
    std::string str;
    MappedTo mappedTo;
    
    std::tuple<> emptyTuple;
    std::tuple<int> singleTuple;
    std::tuple<int, int> doubleTuple;
    std::tuple<int, int, int> tripleTuple;
    std::pair<int, int> pair;
    std::map<int, Keyable> primitiveKey;
    std::map<Keyable, int> complexKey;
    std::tuple<int, int, int[2]> intIntArrayTuple;
    std::tuple<int, int, Keyable> intIntKeyableTuple;
    std::tuple<int, int, std::tuple<int, int>> intIntTupleTuple;

    REFLECT(VariousValues, customized, genericNonNull, genericNull, sharedPointerNull, uniquePointerNull,
        regularPointerNull, regularPointerToBecomeNull, regularPointerValue,
        constIntPointerNull, constIntPointerToBecomeNull, constIntPointerValue,
        intConstPointerNull, intConstPointerToBecomeNull, intConstPointerValue,
        constIntConstPointerNull, constIntConstPointerToBecomeNull, constIntConstPointerValue,
        genericValue, genericObjectArray, intVector, composedObj, integerString, enumInt, replaceableInt, boolean, constant, str, mappedTo,
        emptyTuple, singleTuple, doubleTuple, tripleTuple, pair, primitiveKey, complexKey, intIntArrayTuple,
        intIntKeyableTuple, intIntTupleTuple)
};

TEST_HEADER(JsonInputRead, Value)
{
    char c = '\0';
    int anInt = 0;

    VariousValues v(anInt);
    v.genericNonNull = std::make_shared<Json::Bool>(false);
    v.genericNull = nullptr;
    v.sharedPointerNull = nullptr;
    v.uniquePointerNull = nullptr;
    v.regularPointerNull = nullptr;
    v.regularPointerToBecomeNull = &anInt;
    v.regularPointerValue = &anInt;
    
    v.constIntPointerNull = nullptr;
    v.constIntPointerToBecomeNull = &anInt;
    v.constIntPointerValue = &anInt;

    v.genericObjectArray.objectArray().assign(2, Json::ObjectValue{});

    v.composedObj.a = 0;
    v.integerString = 0;
    v.enumInt = EnumIntEnum::none;
    v.replaceableInt = 0;
    v.boolean = false;
    v.str = "";
    v.mappedTo = { 0 };

    v.emptyTuple = {};
    v.singleTuple = { 0 };
    v.doubleTuple = { 0, 0 };
    v.tripleTuple = { 0, 0, 0 };
    v.pair = { 0, 0 };
    std::get<0>(v.intIntArrayTuple) = 0;
    std::get<1>(v.intIntArrayTuple) = 0;
    std::get<2>(v.intIntArrayTuple)[0] = 0;
    std::get<2>(v.intIntArrayTuple)[1] = 0;
    v.intIntKeyableTuple = { 0, 0, {0} };
    std::get<0>(v.intIntTupleTuple) = 0;
    std::get<1>(v.intIntTupleTuple) = 0;
    std::get<0>(std::get<2>(v.intIntTupleTuple)) = 0;
    std::get<1>(std::get<2>(v.intIntTupleTuple)) = 0;

    std::stringstream customizedStream("1234,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::customized, CustomizeTypeFullySpecialized, VariousValues>(
        customizedStream, Json::defaultContext, c, v, v.customized);
    EXPECT_EQ(1234, v.customized.firstField);

    std::stringstream genericNonNullNull("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::genericNonNull, std::shared_ptr<Json::Value>, VariousValues>(
        genericNonNullNull, Json::defaultContext, c, v, v.genericNonNull);
    EXPECT_TRUE(v.genericNonNull == nullptr);

    std::stringstream genericNullNonNull("true,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::genericNull, std::shared_ptr<Json::Value>, VariousValues>(
        genericNullNonNull, Json::defaultContext, c, v, v.genericNull);
    EXPECT_TRUE(v.genericNull != nullptr);
    EXPECT_EQ(Json::Value::Type::Boolean, v.genericNull->type());

    std::stringstream sharedPtrNullptr("111,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::sharedPointerNull, std::shared_ptr<int>, VariousValues>(
        sharedPtrNullptr, Json::defaultContext, c, v, v.sharedPointerNull);
    EXPECT_TRUE(v.sharedPointerNull != nullptr);
    EXPECT_EQ(111, *v.sharedPointerNull);

    std::stringstream uniquePtrNullptr("222,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::uniquePointerNull, std::unique_ptr<int>, VariousValues>(
        uniquePtrNullptr, Json::defaultContext, c, v, v.uniquePointerNull);
    EXPECT_TRUE(v.uniquePointerNull != nullptr);
    EXPECT_EQ(222, *v.uniquePointerNull);

    std::stringstream regularPtrNullptr("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::regularPointerNull, int*, VariousValues>(
        regularPtrNullptr, Json::defaultContext, c, v, v.regularPointerNull);
    EXPECT_TRUE(v.regularPointerNull == nullptr);

    std::stringstream regularPtrToBecomeNull("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::regularPointerToBecomeNull, int*, VariousValues>(
        regularPtrToBecomeNull, Json::defaultContext, c, v, v.regularPointerToBecomeNull);
    EXPECT_TRUE(v.regularPointerToBecomeNull == nullptr);

    std::stringstream regularPtrValue("999,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::regularPointerValue, int*, VariousValues>(
        regularPtrValue, Json::defaultContext, c, v, v.regularPointerValue);
    EXPECT_EQ(999, anInt);



    std::stringstream constIntPtrNullptr("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::constIntPointerNull, const int*, VariousValues>(
        constIntPtrNullptr, Json::defaultContext, c, v, v.constIntPointerNull);
    EXPECT_TRUE(v.constIntPointerNull == nullptr);

    std::stringstream constIntPtrToBecomeNull("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::constIntPointerToBecomeNull, const int*, VariousValues>(
        constIntPtrToBecomeNull, Json::defaultContext, c, v, v.constIntPointerToBecomeNull);
    EXPECT_TRUE(v.constIntPointerToBecomeNull == nullptr);

    std::stringstream constIntPtrValue("998,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::constIntPointerValue, const int*, VariousValues>(
        constIntPtrValue, Json::defaultContext, c, v, v.constIntPointerValue);
    EXPECT_EQ(999, anInt); // Value should not change

    std::stringstream emptyArrayValue("[],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::genericObjectArray, Json::ObjectArray, VariousValues>(
        emptyArrayValue, Json::defaultContext, c, v, v.genericObjectArray);


    std::stringstream intConstPtrNullptr("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::intConstPointerNull, int* const, VariousValues>(
        intConstPtrNullptr, Json::defaultContext, c, v, v.intConstPointerNull);
    EXPECT_TRUE(v.intConstPointerNull == nullptr);

    std::stringstream intConstPtrToBecomeNull("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::intConstPointerToBecomeNull, int* const, VariousValues>(
        intConstPtrToBecomeNull, Json::defaultContext, c, v, v.intConstPointerToBecomeNull);
    EXPECT_FALSE(v.intConstPointerToBecomeNull == nullptr); // Pointer should not change

    std::stringstream intConstPtrValue("997,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::intConstPointerValue, int* const, VariousValues>(
        intConstPtrValue, Json::defaultContext, c, v, v.intConstPointerValue);
    EXPECT_EQ(997, anInt); // Value should change


    std::stringstream constIntConstPtrNullptr("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::constIntConstPointerNull, const int* const, VariousValues>(
        constIntConstPtrNullptr, Json::defaultContext, c, v, v.constIntConstPointerNull);
    EXPECT_TRUE(v.constIntConstPointerNull == nullptr);

    std::stringstream constIntConstPtrToBecomeNull("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::constIntConstPointerToBecomeNull, const int* const, VariousValues>(
        constIntConstPtrToBecomeNull, Json::defaultContext, c, v, v.constIntConstPointerToBecomeNull);
    EXPECT_FALSE(v.constIntConstPointerToBecomeNull == nullptr); // Pointer should not change

    std::stringstream constIntConstPtrValue("996,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::constIntConstPointerValue, const int* const, VariousValues>(
        constIntConstPtrValue, Json::defaultContext, c, v, v.constIntConstPointerValue);
    EXPECT_EQ(997, anInt); // Value should not change


    std::stringstream composedObjStream("{\"a\": 1}");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::composedObj, ComposedObj, VariousValues>(
        composedObjStream, Json::defaultContext, c, v, v.composedObj);
    EXPECT_EQ(1, v.composedObj.a);

    std::stringstream integerStringStream("\"4567\"");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::integerString, int, VariousValues>(
        integerStringStream, Json::defaultContext, c, v, v.integerString);
    EXPECT_EQ(4567, v.integerString);

    std::stringstream enumIntStream("1337,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::enumInt, EnumIntEnum, VariousValues>(
        enumIntStream, Json::defaultContext, c, v, v.enumInt);
    EXPECT_EQ(EnumIntEnum::leet, v.enumInt);

    std::stringstream unreplacedReplaceableIntStream("1337,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::replaceableInt, int, VariousValues>(
        unreplacedReplaceableIntStream, Json::defaultContext, c, v, v.replaceableInt);
    EXPECT_EQ(1337, v.replaceableInt);

    std::stringstream replacedReplaceableIntStream("\"\",");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::replaceableInt, int, VariousValues>(
        replacedReplaceableIntStream, Json::defaultContext, c, v, v.replaceableInt);
    EXPECT_EQ(-222, v.replaceableInt);

    std::stringstream booleanStream("true,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::boolean, bool, VariousValues>(
        booleanStream, Json::defaultContext, c, v, v.boolean);
    EXPECT_TRUE(v.boolean);
    
    std::stringstream constInt("9001,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::constant, const int, VariousValues>(
        constInt, Json::defaultContext, c, v, v.constant);
    EXPECT_EQ(0, v.constant);

    std::stringstream strStream("\"asdf\",");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::str, std::string, VariousValues>(
        strStream, Json::defaultContext, c, v, v.str);
    EXPECT_STREQ("asdf", v.str.c_str());

    std::stringstream mappedToStream("{\"a\":1},");
    using OpNoteWithMapping = std::tuple<RareMapper::UseMapping<MappedTo, MappedFrom>>;
    Json::Read::value<OpNoteWithMapping, true, Reflect<VariousValues>::MemberType::mappedTo, MappedTo, VariousValues>(
        mappedToStream, Json::defaultContext, c, v, v.mappedTo);
    EXPECT_EQ(1, v.mappedTo.a);

    std::stringstream nullTupleStream("null,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::emptyTuple, std::tuple<>, VariousValues>(
        nullTupleStream, Json::defaultContext, c, v, v.emptyTuple);
    
    std::stringstream emptyArrayTupleStream("[],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::emptyTuple, std::tuple<>, VariousValues>(
        emptyArrayTupleStream, Json::defaultContext, c, v, v.emptyTuple);

    std::stringstream singleTupleValueStream("1,");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::singleTuple, std::tuple<int>, VariousValues>(
        singleTupleValueStream, Json::defaultContext, c, v, v.singleTuple);
    EXPECT_EQ(1, std::get<0>(v.singleTuple));

    std::stringstream singleTupleArrayStream("[2],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::singleTuple, std::tuple<int>, VariousValues>(
        singleTupleArrayStream, Json::defaultContext, c, v, v.singleTuple);
    EXPECT_EQ(2, std::get<0>(v.singleTuple));

    std::stringstream doubleTupleArrayStream("[1, 2],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::doubleTuple, std::tuple<int, int>, VariousValues>(
        doubleTupleArrayStream, Json::defaultContext, c, v, v.doubleTuple);
    EXPECT_EQ(1, std::get<0>(v.doubleTuple));
    EXPECT_EQ(2, std::get<1>(v.doubleTuple));

    std::stringstream tripleTupleArrayStream("[1, 2, 3],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::tripleTuple, std::tuple<int, int, int>, VariousValues>(
        tripleTupleArrayStream, Json::defaultContext, c, v, v.tripleTuple);
    EXPECT_EQ(1, std::get<0>(v.tripleTuple));
    EXPECT_EQ(2, std::get<1>(v.tripleTuple));
    EXPECT_EQ(3, std::get<2>(v.tripleTuple));

    std::stringstream pairStream("[1, 2],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::pair, std::pair<int, int>, VariousValues>(
        pairStream, Json::defaultContext, c, v, v.pair);
    EXPECT_EQ(1, std::get<0>(v.pair));
    EXPECT_EQ(2, std::get<1>(v.pair));

    std::stringstream primitiveKeyStream("{\"1\": {\"a\":2}},");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::primitiveKey, std::map<int, Keyable>, VariousValues>(
        primitiveKeyStream, Json::defaultContext, c, v, v.primitiveKey);
    auto found = v.primitiveKey.find(1);
    EXPECT_TRUE(found != v.primitiveKey.end());
    EXPECT_EQ(2, found->second.a);

    std::stringstream primitiveKeyArrayStream("[[3, {\"a\":4}]],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::primitiveKey, std::map<int, Keyable>, VariousValues>(
        primitiveKeyArrayStream, Json::defaultContext, c, v, v.primitiveKey);
    auto primitiveKeyArrayFound = v.primitiveKey.find(3);
    EXPECT_TRUE(primitiveKeyArrayFound != v.primitiveKey.end());
    EXPECT_EQ(4, primitiveKeyArrayFound->second.a);

    std::stringstream complexKeyStream("[{\"key\":{\"a\":1},\"value\":2}],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::complexKey, std::map<Keyable, int>, VariousValues>(
        complexKeyStream, Json::defaultContext, c, v, v.complexKey);
    Keyable keyable { 1 };
    auto complexKeyFound = v.complexKey.find(keyable);
    EXPECT_TRUE(complexKeyFound != v.complexKey.end());
    EXPECT_EQ(2, complexKeyFound->second);

    std::stringstream complexKeyArrayStream("[[{\"a\":3},4]],");
    Json::Read::value<NoNote, true, Reflect<VariousValues>::MemberType::complexKey, std::map<Keyable, int>, VariousValues>(
        complexKeyArrayStream, Json::defaultContext, c, v, v.complexKey);
    Keyable arrayKeyable { 3 };
    auto complexKeyArrayFound = v.complexKey.find(arrayKeyable);
    EXPECT_TRUE(complexKeyArrayFound != v.complexKey.end());
    EXPECT_EQ(4, complexKeyArrayFound->second);

    std::stringstream intIntArrayTupleStream("[1, 2, [3,4]],");
    Json::Read::tuple<NoNote, Reflect<VariousValues>::MemberType::intIntArrayTuple, 0>(
        intIntArrayTupleStream, Json::defaultContext, c, v, v.intIntArrayTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntArrayTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntArrayTuple));
    EXPECT_EQ(3, std::get<2>(v.intIntArrayTuple)[0]);
    EXPECT_EQ(4, std::get<2>(v.intIntArrayTuple)[1]);

    std::stringstream intIntKeyableTupleStream("[1, 2, {\"a\":3}],");
    Json::Read::tuple<NoNote, Reflect<VariousValues>::MemberType::intIntKeyableTuple, 0>(
        intIntKeyableTupleStream, Json::defaultContext, c, v, v.intIntKeyableTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntKeyableTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntKeyableTuple));
    EXPECT_EQ(3, std::get<2>(v.intIntKeyableTuple).a);

    std::stringstream intIntTupleTupleStream("[1, 2, [3, 4]],");
    Json::Read::tuple<NoNote, Reflect<VariousValues>::MemberType::intIntTupleTuple, 0>(
        intIntTupleTupleStream, Json::defaultContext, c, v, v.intIntTupleTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntTupleTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntTupleTuple));
    EXPECT_EQ(3, std::get<0>(std::get<2>(v.intIntTupleTuple)));
    EXPECT_EQ(4, std::get<1>(std::get<2>(v.intIntTupleTuple)));
}

TEST_HEADER(JsonInputRead, Tuple)
{
    char c = '\0';
    int anInt = 0;

    VariousValues v(anInt);
    v.genericNonNull = std::make_shared<Json::Bool>(false);
    v.genericNull = nullptr;
    v.sharedPointerNull = nullptr;
    v.uniquePointerNull = nullptr;
    v.regularPointerNull = nullptr;
    v.regularPointerToBecomeNull = &anInt;
    v.regularPointerValue = &anInt;

    v.composedObj.a = 0;
    v.integerString = 0;
    v.enumInt = EnumIntEnum::none;
    v.boolean = false;
    v.str = "";

    v.emptyTuple = {};
    v.singleTuple = { 0 };
    v.doubleTuple = { 0, 0 };
    v.tripleTuple = { 0, 0, 0 };
    v.pair = { 0, 0 };
    std::get<0>(v.intIntArrayTuple) = 0;
    std::get<1>(v.intIntArrayTuple) = 0;
    std::get<2>(v.intIntArrayTuple)[0] = 0;
    std::get<2>(v.intIntArrayTuple)[1] = 0;
    v.intIntKeyableTuple = { 0, 0, {0} };
    std::get<0>(v.intIntTupleTuple) = 0;
    std::get<1>(v.intIntTupleTuple) = 0;
    std::get<0>(std::get<2>(v.intIntTupleTuple)) = 0;
    std::get<1>(std::get<2>(v.intIntTupleTuple)) = 0;

    std::stringstream doubleTupleArrayStream("[1, 2],");
    Json::Read::tuple<NoNote, Reflect<VariousValues>::MemberType::doubleTuple, 0>(
        doubleTupleArrayStream, Json::defaultContext, c, v, v.doubleTuple);
    EXPECT_EQ(1, std::get<0>(v.doubleTuple));
    EXPECT_EQ(2, std::get<1>(v.doubleTuple));

    std::stringstream tripleTupleArrayStream("[1, 2, 3],");
    Json::Read::tuple<NoNote, Reflect<VariousValues>::MemberType::tripleTuple, 0>(
        tripleTupleArrayStream, Json::defaultContext, c, v, v.tripleTuple);
    EXPECT_EQ(1, std::get<0>(v.tripleTuple));
    EXPECT_EQ(2, std::get<1>(v.tripleTuple));
    EXPECT_EQ(3, std::get<2>(v.tripleTuple));

    std::stringstream intIntArrayTupleStream("[1, 2, [3,4]],");
    Json::Read::tuple<NoNote, Reflect<VariousValues>::MemberType::intIntArrayTuple, 0>(
        intIntArrayTupleStream, Json::defaultContext, c, v, v.intIntArrayTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntArrayTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntArrayTuple));
    EXPECT_EQ(3, std::get<2>(v.intIntArrayTuple)[0]);
    EXPECT_EQ(4, std::get<2>(v.intIntArrayTuple)[1]);

    std::stringstream intIntKeyableTupleStream("[1, 2, {\"a\":3}],");
    Json::Read::tuple<NoNote, Reflect<VariousValues>::MemberType::intIntKeyableTuple, 0>(
        intIntKeyableTupleStream, Json::defaultContext, c, v, v.intIntKeyableTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntKeyableTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntKeyableTuple));
    EXPECT_EQ(3, std::get<2>(v.intIntKeyableTuple).a);

    std::stringstream intIntTupleTupleStream("[1, 2, [3, 4]],");
    Json::Read::tuple<NoNote, Reflect<VariousValues>::MemberType::intIntTupleTuple, 0>(
        intIntTupleTupleStream, Json::defaultContext, c, v, v.intIntTupleTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntTupleTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntTupleTuple));
    EXPECT_EQ(3, std::get<0>(std::get<2>(v.intIntTupleTuple)));
    EXPECT_EQ(4, std::get<1>(std::get<2>(v.intIntTupleTuple)));
}

struct VariousPairs
{
    std::pair<int, int> intIntPair;
    std::pair<int, Keyable> intObjPair;
    std::pair<Keyable, int> objIntPair;
    std::pair<int, std::pair<Keyable, Keyable>> intPairPair;

    REFLECT(VariousPairs, intIntPair, intObjPair, objIntPair, intPairPair)
};

TEST_HEADER(JsonInputRead, Pair)
{
    char c = '\0';

    VariousPairs v;
    v.intIntPair = { 0, 0 };
    v.intObjPair = { 0, {0} };
    v.objIntPair = { {0}, 0 };
    v.intPairPair = { 0, {{0},{0}} };

    std::stringstream intIntPairStream("[1, 2],");
    Json::Read::pair<NoNote, Reflect<VariousPairs>::MemberType::intIntPair, VariousPairs>(
        intIntPairStream, Json::defaultContext, c, v, v.intIntPair);
    EXPECT_EQ(1, std::get<0>(v.intIntPair));
    EXPECT_EQ(2, std::get<1>(v.intIntPair));

    std::stringstream intObjPairStream("[1, {\"a\":2}],");
    Json::Read::pair<NoNote, Reflect<VariousPairs>::MemberType::intObjPair, VariousPairs>(
        intObjPairStream, Json::defaultContext, c, v, v.intObjPair);
    EXPECT_EQ(1, std::get<0>(v.intObjPair));
    EXPECT_EQ(2, std::get<1>(v.intObjPair).a);

    std::stringstream objIntPairStream("[{\"a\":1}, 2],");
    Json::Read::pair<NoNote, Reflect<VariousPairs>::MemberType::objIntPair, VariousPairs>(
        objIntPairStream, Json::defaultContext, c, v, v.objIntPair);
    EXPECT_EQ(1, std::get<0>(v.objIntPair).a);
    EXPECT_EQ(2, std::get<1>(v.objIntPair));

    std::stringstream intPairPairStream("[1, [{\"a\":2}, {\"a\":3}]],");
    Json::Read::pair<NoNote, Reflect<VariousPairs>::MemberType::intPairPair, VariousPairs>(
        intPairPairStream, Json::defaultContext, c, v, v.intPairPair);
    EXPECT_EQ(1, std::get<0>(v.intPairPair));
    EXPECT_EQ(2, std::get<0>(std::get<1>(v.intPairPair)).a);
    EXPECT_EQ(3, std::get<1>(std::get<1>(v.intPairPair)).a);
}

struct VariousMaps
{
    std::map<int, int> intIntMap;
    std::map<int, Keyable> intObjMap;
    std::map<Keyable, int> objIntMap;
    std::map<Keyable, Keyable> objObjMap;
    std::map<std::string, std::string> stringStringMap;

    REFLECT(VariousMaps, intIntMap, intObjMap, objIntMap, objObjMap, stringStringMap)
};

TEST_HEADER(JsonInputRead, KeyValueObject)
{
    char c = '\0';

    VariousMaps v;
    v.intIntMap = { {0, 0} };
    v.intObjMap = { {0, {0}} };
    v.objIntMap = { {{0}, 0} };
    v.objObjMap = { {{0}, {0}} };
    v.stringStringMap = { {"", ""}};
    
    std::stringstream intIntMapStream("{\"key\":1,\"value\":2},");
    std::pair<int, int> value = { 0, 0 };
    Json::Read::keyValueObject<NoNote, Reflect<VariousMaps>::MemberType::intIntMap, VariousMaps>(
        intIntMapStream, Json::defaultContext, c, v, value);
    EXPECT_EQ(1, std::get<0>(value));
    EXPECT_EQ(2, std::get<1>(value));
    
    std::stringstream intObjMapStream("{\"key\":1,\"value\":{\"a\":2}},");
    std::pair<int, Keyable> intObjElement = { 0, {0} };
    Json::Read::keyValueObject<NoNote, Reflect<VariousMaps>::MemberType::intObjMap, VariousMaps>(
        intObjMapStream, Json::defaultContext, c, v, intObjElement);
    EXPECT_EQ(1, std::get<0>(intObjElement));
    EXPECT_EQ(2, std::get<1>(intObjElement).a);
    
    std::stringstream objIntMapStream("{\"key\":{\"a\":1},\"value\":2},");
    std::pair<Keyable, int> objIntElement = { {0}, 0 };
    Json::Read::keyValueObject<NoNote, Reflect<VariousMaps>::MemberType::objIntMap, VariousMaps>(
        objIntMapStream, Json::defaultContext, c, v, objIntElement);
    EXPECT_EQ(1, std::get<0>(objIntElement).a);
    EXPECT_EQ(2, std::get<1>(objIntElement));
    
    std::stringstream objObjMapStream("{\"key\":{\"a\":1},\"value\":{\"a\":2}},");
    std::pair<Keyable, Keyable> objObjElement = { {0}, {0} };
    Json::Read::keyValueObject<NoNote, Reflect<VariousMaps>::MemberType::objObjMap, VariousMaps>(
        objObjMapStream, Json::defaultContext, c, v, objObjElement);
    EXPECT_EQ(1, std::get<0>(objObjElement).a);
    EXPECT_EQ(2, std::get<1>(objObjElement).a);

    std::stringstream stringStringMapStream("{\"key\":\"1\",\"value\":\"2\"},");
    std::pair<std::string, std::string> stringStringValue;
    Json::Read::keyValueObject<NoNote, Reflect<VariousMaps>::MemberType::stringStringMap, VariousMaps>(
        stringStringMapStream, Json::defaultContext, c, v, stringStringValue);
    EXPECT_STREQ(std::get<0>(stringStringValue).c_str(), "1");
    EXPECT_STREQ(std::get<1>(stringStringValue).c_str(), "2");
}

TEST_HEADER(JsonInputRead, FieldPair)
{
    char c = '\0';

    VariousMaps v;
    v.intIntMap = { {0, 0} };
    v.intObjMap = { {0, {0}} };
    v.objIntMap = { {{0}, 0} };
    v.objObjMap = { {{0}, {0}} };
    v.stringStringMap = { {"", ""}};

    std::stringstream intIntFieldPairStream("\"1\":2");
    std::pair<int, int> value;
    Json::Read::fieldPair<NoNote, Reflect<VariousMaps>::MemberType::intIntMap, VariousMaps>(
        intIntFieldPairStream, Json::defaultContext, c, v, value.first, value.second);
    EXPECT_EQ(1, std::get<0>(value));
    EXPECT_EQ(2, std::get<1>(value));

    std::stringstream intObjFieldPairStream("\"1\":{\"a\":2}");
    std::pair<int, Keyable> intObjValue;
    Json::Read::fieldPair<NoNote, Reflect<VariousMaps>::MemberType::intObjMap, VariousMaps>(
        intObjFieldPairStream, Json::defaultContext, c, v, intObjValue.first, intObjValue.second);
    EXPECT_EQ(1, std::get<0>(intObjValue));
    EXPECT_EQ(2, std::get<1>(intObjValue).a);

    std::stringstream stringStringFieldPairStream("\"1\":\"2\"");
    std::pair<std::string, std::string> stringStringValue;
    Json::Read::fieldPair<NoNote, Reflect<VariousMaps>::MemberType::stringStringMap, VariousMaps>(
        stringStringFieldPairStream, Json::defaultContext, c, v, stringStringValue.first, stringStringValue.second);
    EXPECT_STREQ(std::get<0>(stringStringValue).c_str(), "1");
    EXPECT_STREQ(std::get<1>(stringStringValue).c_str(), "2");
}

TEST_HEADER(JsonInputRead, Iterable)
{
    char c = '\0';
    int placeholderObj = 0;

    int staticArray[4] = { 0, 0, 0, 0 };
    std::stringstream staticArrayStream("[1,2,3,4]");
    using StaticArrayField = Json::MockMember<decltype(staticArray)>;
    Json::Read::iterable<NoNote, StaticArrayField>(staticArrayStream, Json::defaultContext, c, placeholderObj, staticArray);
    EXPECT_EQ(1, staticArray[0]);
    EXPECT_EQ(2, staticArray[1]);
    EXPECT_EQ(3, staticArray[2]);
    EXPECT_EQ(4, staticArray[3]);

    std::vector<int> intVector;
    std::stringstream vectorStream("[2,3,4,5]");
    using VectorField = Json::MockMember<decltype(intVector)>;
    Json::Read::iterable<NoNote, VectorField>(vectorStream, Json::defaultContext, c, placeholderObj, intVector);
    EXPECT_EQ(2, intVector[0]);
    EXPECT_EQ(3, intVector[1]);
    EXPECT_EQ(4, intVector[2]);
    EXPECT_EQ(5, intVector[3]);
}

struct RegularField
{
    int a;

    REFLECT(RegularField, a)
};

NOTE(SuperClassField, Super<RegularField>)
struct SuperClassField : RegularField
{
    REFLECT_NOTED(SuperClassField)
};

struct RenamedField
{
    NOTE(renamed, Json::Name{"Custom Name"})
    int renamed;

    REFLECT(RenamedField, renamed)
};

TEST_HEADER(JsonInputRead, Field)
{
    RegularField regularField = {};
    regularField.a = 444;
    SuperClassField superClassField = {};
    superClassField.a = 555;
    RenamedField renamedField = {};
    renamedField.renamed = 666;

    char c = '\0';
    std::stringstream regularFieldStream(":777,");
    std::stringstream superClassFieldStream(":{\"a\":888}");
    std::stringstream renamedFieldStream(":999");

    Json::Read::field<NoNote, RegularField>(regularFieldStream, Json::defaultContext, c, regularField, "a");
    Json::Read::field<NoNote, SuperClassField>(superClassFieldStream, Json::defaultContext, c, superClassField, Json::Shared::superTypeToJsonFieldName<RegularField>().c_str());
    Json::Read::field<NoNote, RenamedField>(renamedFieldStream, Json::defaultContext, c, renamedField, "Custom Name");

    EXPECT_EQ(regularField.a, 777);
    EXPECT_EQ(superClassField.a, 888);
    EXPECT_EQ(renamedField.renamed, 999);

    std::stringstream unknownFieldStream(":222,");
    Json::Read::field<NoNote, RegularField>(unknownFieldStream, Json::defaultContext, c, regularField, "b");
}

struct ReadableObject
{
    int a;

    NOTE(b, Json::Name{"c"})
    int b;

    REFLECT(ReadableObject, a, b)
};

TEST_HEADER(JsonInputRead, Object)
{
    char c = '\0';
    std::stringstream objectStream("{\"a\":5,\"c\":6}");
    ReadableObject object;
    object.a = 0;
    object.b = 0;

    Json::Read::object<NoNote, ReadableObject>(objectStream, Json::defaultContext, c, object);
    EXPECT_EQ(5, object.a);
    EXPECT_EQ(6, object.b);
}

TEST_HEADER(JsonInput, ReflectedObject)
{
    std::stringstream objectStream("{\"a\":5,\"b\":6}");
    ReadableObject object;
    object.a = 0;
    object.b = 0;

    Json::Input::ReflectedObject<NoNote, ReadableObject> reflectedObject(object, nullptr);
    objectStream >> reflectedObject;
    
    EXPECT_EQ(5, object.a);
    EXPECT_EQ(0, object.b);
}

struct JsonReferences
{
    JsonReferences() : primitive(1), primitiveReference(primitive) {}

    int primitive;
    int & primitiveReference;
    static int staticPrimitive;
    static constexpr int & staticPrimitiveReference = staticPrimitive;

    REFLECT(JsonReferences, primitive, primitiveReference, staticPrimitive, staticPrimitiveReference)
};

int JsonReferences::staticPrimitive = 2;

TEST_HEADER(JsonInput, InReferences)
{
    JsonReferences jsonReferences;
    EXPECT_EQ(1, jsonReferences.primitive);
    EXPECT_EQ(1, jsonReferences.primitiveReference);
    EXPECT_EQ(2, jsonReferences.staticPrimitive);
    EXPECT_EQ(2, jsonReferences.staticPrimitiveReference);

    std::stringstream inputPrimitivesStream(
        "{\"primitive\":11,\"staticPrimitive\":22}");
    inputPrimitivesStream >> Json::in(jsonReferences);
    EXPECT_EQ(11, jsonReferences.primitive);
    EXPECT_EQ(11, jsonReferences.primitiveReference);
    EXPECT_EQ(22, jsonReferences.staticPrimitive);
    EXPECT_EQ(22, jsonReferences.staticPrimitiveReference);

    std::stringstream inputReferencesStream(
        "{\"primitiveReference\":111,\"staticPrimitiveReference\":222}");
    inputReferencesStream >> Json::in(jsonReferences);
    EXPECT_EQ(111, jsonReferences.primitive);
    EXPECT_EQ(111, jsonReferences.primitiveReference);
    EXPECT_EQ(222, jsonReferences.staticPrimitive);
    EXPECT_EQ(222, jsonReferences.staticPrimitiveReference);
}

NOTE(EmptyIn)
struct EmptyIn
{
    REFLECT_NOTED(EmptyIn)
};

struct SingleFieldIn
{
    int a;

    REFLECT(SingleFieldIn, a)
};

NOTE(ComplexStruct, Super<EmptyIn>, Super<SingleFieldIn>)
struct ComplexStruct : EmptyIn, SingleFieldIn
{
    int a;
    std::string b;
    std::shared_ptr<std::string> dynamicString;
    SingleFieldIn singleFieldIn;
    std::map<int, std::string> intStringMap;
    std::vector<int> intVector;
    Json::FieldCluster fieldCluster;
    
    REFLECT_NOTED(ComplexStruct, a, b, dynamicString, singleFieldIn, intStringMap, intVector, fieldCluster)
};

TEST_HEADER(JsonInput, In)
{
    EmptyIn emptyIn = {};
    std::stringstream emptyObjStream("{}");
    EXPECT_NO_THROW(emptyObjStream >> Json::in(emptyIn));

    SingleFieldIn singleFieldIn = { 0 };
    std::stringstream singleFieldEmptyObjStream("{}");
    EXPECT_NO_THROW(singleFieldEmptyObjStream >> Json::in(singleFieldIn));
    std::stringstream singleFieldObjStream("{\"a\":1234}");
    singleFieldObjStream >> Json::in(singleFieldIn);
    EXPECT_EQ(1234, singleFieldIn.a);

    ComplexStruct complexStruct = {};
    std::stringstream complexStructStream;
    complexStructStream
        << "{" << std::endl
        << "  \"a\" : 1 , " << std::endl
        << "  \"b\" : \"asdf\" , " << std::endl
        << "  \"dynamicString\" : \"qwerty\" , " << std::endl
        << "  \"singleFieldIn\" : {" << std::endl
        << "    \"a\" : 3 " << std::endl
        << "  } , " << std::endl
        << "  \"intStringMap\" : {" << std::endl
        << "    \"2\": \"ab\" , " << std::endl
        << "    \"3\": \"c\"   " << std::endl
        << "  } , " << std::endl
        << "  \"intVector\" : [ 1 , 2, 3]," << std::endl
        << "  \"someUnknown\": null," << std::endl
        << "  \"someOtherUnknown\": [4, 5, 6]" << std::endl
        << "}";

    complexStructStream >> Json::in(complexStruct);
    EXPECT_EQ(1, complexStruct.a);
    EXPECT_STREQ("asdf", complexStruct.b.c_str());
    EXPECT_STREQ("qwerty", complexStruct.dynamicString->c_str());
    EXPECT_EQ(3, complexStruct.singleFieldIn.a);
    EXPECT_STREQ("ab", complexStruct.intStringMap.find(2)->second.c_str());
    EXPECT_STREQ("c", complexStruct.intStringMap.find(3)->second.c_str());
    EXPECT_EQ(1, complexStruct.intVector[0]);
    EXPECT_EQ(2, complexStruct.intVector[1]);
    EXPECT_EQ(3, complexStruct.intVector[2]);
    EXPECT_TRUE(complexStruct.fieldCluster.object().find("someUnknown")->second == nullptr);
    EXPECT_EQ(size_t(3), complexStruct.fieldCluster.object().find("someOtherUnknown")->second->arraySize());
    EXPECT_STREQ("4", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[0].c_str());
    EXPECT_STREQ("5", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[1].c_str());
    EXPECT_STREQ("6", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[2].c_str());
}

TEST_HEADER(JsonInput, ReadRef)
{
    EmptyIn emptyIn = {};
    std::stringstream emptyObjStream("{}");
    EXPECT_NO_THROW(emptyObjStream >> Json::in(emptyIn));

    SingleFieldIn singleFieldIn = { 0 };
    std::stringstream singleFieldEmptyObjStream("{}");
    EXPECT_NO_THROW(singleFieldEmptyObjStream >> Json::in(singleFieldIn));
    std::stringstream singleFieldObjStream("{\"a\":1234}");
    singleFieldObjStream >> Json::in(singleFieldIn);
    EXPECT_EQ(1234, singleFieldIn.a);

    ComplexStruct complexStruct = {};
    std::stringstream complexStructStream;
    complexStructStream
        << "{" << std::endl
        << "  \"a\" : 1 , " << std::endl
        << "  \"b\" : \"asdf\" , " << std::endl
        << "  \"dynamicString\" : \"qwerty\" , " << std::endl
        << "  \"singleFieldIn\" : {" << std::endl
        << "    \"a\" : 3 " << std::endl
        << "  } , " << std::endl
        << "  \"intStringMap\" : {" << std::endl
        << "    \"2\": \"ab\" , " << std::endl
        << "    \"3\": \"c\"   " << std::endl
        << "  } , " << std::endl
        << "  \"intVector\" : [ 1 , 2, 3]," << std::endl
        << "  \"someUnknown\": null," << std::endl
        << "  \"someOtherUnknown\": [4, 5, 6]" << std::endl
        << "}";

    Json::read(complexStructStream.str(), complexStruct);
    EXPECT_EQ(1, complexStruct.a);
    EXPECT_STREQ("asdf", complexStruct.b.c_str());
    EXPECT_STREQ("qwerty", complexStruct.dynamicString->c_str());
    EXPECT_EQ(3, complexStruct.singleFieldIn.a);
    EXPECT_STREQ("ab", complexStruct.intStringMap.find(2)->second.c_str());
    EXPECT_STREQ("c", complexStruct.intStringMap.find(3)->second.c_str());
    EXPECT_EQ(1, complexStruct.intVector[0]);
    EXPECT_EQ(2, complexStruct.intVector[1]);
    EXPECT_EQ(3, complexStruct.intVector[2]);
    EXPECT_TRUE(complexStruct.fieldCluster.object().find("someUnknown")->second == nullptr);
    EXPECT_EQ(size_t(3), complexStruct.fieldCluster.object().find("someOtherUnknown")->second->arraySize());
    EXPECT_STREQ("4", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[0].c_str());
    EXPECT_STREQ("5", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[1].c_str());
    EXPECT_STREQ("6", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[2].c_str());
}

TEST_HEADER(JsonInput, ReadType)
{
    EmptyIn emptyIn = {};
    std::stringstream emptyObjStream("{}");
    EXPECT_NO_THROW(emptyObjStream >> Json::in(emptyIn));

    SingleFieldIn singleFieldIn = { 0 };
    std::stringstream singleFieldEmptyObjStream("{}");
    EXPECT_NO_THROW(singleFieldEmptyObjStream >> Json::in(singleFieldIn));
    std::stringstream singleFieldObjStream("{\"a\":1234}");
    singleFieldObjStream >> Json::in(singleFieldIn);
    EXPECT_EQ(1234, singleFieldIn.a);

    ComplexStruct complexStruct = {};
    std::stringstream complexStructStream;
    complexStructStream
        << "{" << std::endl
        << "  \"a\" : 1 , " << std::endl
        << "  \"b\" : \"asdf\" , " << std::endl
        << "  \"dynamicString\" : \"qwerty\" , " << std::endl
        << "  \"singleFieldIn\" : {" << std::endl
        << "    \"a\" : 3 " << std::endl
        << "  } , " << std::endl
        << "  \"intStringMap\" : {" << std::endl
        << "    \"2\": \"ab\" , " << std::endl
        << "    \"3\": \"c\"   " << std::endl
        << "  } , " << std::endl
        << "  \"intVector\" : [ 1 , 2, 3]," << std::endl
        << "  \"someUnknown\": null," << std::endl
        << "  \"someOtherUnknown\": [4, 5, 6]" << std::endl
        << "}";

    complexStruct = Json::read<ComplexStruct>(complexStructStream.str());
    EXPECT_EQ(1, complexStruct.a);
    EXPECT_STREQ("asdf", complexStruct.b.c_str());
    EXPECT_STREQ("qwerty", complexStruct.dynamicString->c_str());
    EXPECT_EQ(3, complexStruct.singleFieldIn.a);
    EXPECT_STREQ("ab", complexStruct.intStringMap.find(2)->second.c_str());
    EXPECT_STREQ("c", complexStruct.intStringMap.find(3)->second.c_str());
    EXPECT_EQ(1, complexStruct.intVector[0]);
    EXPECT_EQ(2, complexStruct.intVector[1]);
    EXPECT_EQ(3, complexStruct.intVector[2]);
    EXPECT_TRUE(complexStruct.fieldCluster.object().find("someUnknown")->second == nullptr);
    EXPECT_EQ(size_t(3), complexStruct.fieldCluster.object().find("someOtherUnknown")->second->arraySize());
    EXPECT_STREQ("4", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[0].c_str());
    EXPECT_STREQ("5", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[1].c_str());
    EXPECT_STREQ("6", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[2].c_str());
}

TEST_HEADER(JsonInput, InProxyReflected)
{
    std::stringstream objectStream("{\"a\":5}");
    ProxyReflectedObject object;
    object.a = 0;

    objectStream >> Json::in(object);

    EXPECT_EQ(5, object.a);
}

struct CharacterLikeTypes
{
    char a = '\0';
    signed char b = '\0';
    unsigned char c = '\0';
    std::int8_t d = 0;
    std::uint8_t e = 0;

    REFLECT(CharacterLikeTypes, a, b, c, d, e)
};

TEST_HEADER(JsonInput, InCharacterLikeTypes)
{
    std::stringstream smallInput("{\"a\":1,\"b\":2,\"c\":3,\"d\":4,\"e\":5}");
    CharacterLikeTypes small {};
    smallInput >> Json::in(small);
    EXPECT_EQ(1, small.a);
    EXPECT_EQ(2, small.b);
    EXPECT_EQ(3, small.c);
    EXPECT_EQ(4, small.d);
    EXPECT_EQ(5, small.e);

    std::stringstream largeInput("{\"a\":100,\"b\":101,\"c\":102,\"d\":103,\"e\":104}");
    CharacterLikeTypes large {};
    largeInput >> Json::in(large);
    EXPECT_EQ(100, large.a);
    EXPECT_EQ(101, large.b);
    EXPECT_EQ(102, large.c);
    EXPECT_EQ(103, large.d);
    EXPECT_EQ(104, large.e);
    
    std::stringstream negativeInput("{\"b\":-101,\"d\":-102}");
    CharacterLikeTypes negative {};
    negativeInput >> Json::in(negative);
    EXPECT_EQ(-101, negative.b);
    EXPECT_EQ(-102, negative.d);
}

struct VariousArrays
{
    int a[3] {};
    int b[2][3] {};
    int c[2][3][1] {};
    std::array<int, 2> d {};

    REFLECT(VariousArrays, a, b, c, d)
};

TEST_HEADER(JsonInput, InVariousArrays)
{
    std::stringstream input("{\"a\":[1,2,3],\"b\":[[4,5,6],[7,8,9]],\"c\":[[[4],[5],[6]],[[7],[8],[9]]],\"d\":[4,5]}");
    VariousArrays read {};
    input >> Json::in(read);
    
    EXPECT_EQ(1, read.a[0]);
    EXPECT_EQ(2, read.a[1]);
    EXPECT_EQ(3, read.a[2]);
    
    EXPECT_EQ(4, read.b[0][0]);
    EXPECT_EQ(5, read.b[0][1]);
    EXPECT_EQ(6, read.b[0][2]);
    EXPECT_EQ(7, read.b[1][0]);
    EXPECT_EQ(8, read.b[1][1]);
    EXPECT_EQ(9, read.b[1][2]);
    
    EXPECT_EQ(4, read.c[0][0][0]);
    EXPECT_EQ(5, read.c[0][1][0]);
    EXPECT_EQ(6, read.c[0][2][0]);
    EXPECT_EQ(7, read.c[1][0][0]);
    EXPECT_EQ(8, read.c[1][1][0]);
    EXPECT_EQ(9, read.c[1][2][0]);
    
    EXPECT_EQ(4, read.d[0]);
    EXPECT_EQ(5, read.d[1]);
}

struct InOptObj
{
    int a;

    REFLECT(InOptObj, a)
};

struct InOptional
{
    std::optional<int> a = std::nullopt;
    std::optional<InOptObj> b = std::nullopt;

    REFLECT(InOptional, a, b)
};

TEST_HEADER(JsonInput, InOptionals)
{
    std::stringstream nullOptInput("{\"a\":null,\"b\":null}");
    InOptional inNullOpt {};
    nullOptInput >> Json::in(inNullOpt);
    EXPECT_FALSE(inNullOpt.a);
    EXPECT_FALSE(inNullOpt.b);

    std::stringstream valueOptInput("{\"a\":1,\"b\":{\"a\":2}}");
    InOptional inValueOpt {};
    valueOptInput >> Json::in(inValueOpt);
    EXPECT_EQ(1, *(inValueOpt.a));
    EXPECT_EQ(2, inValueOpt.b->a);
}

#endif