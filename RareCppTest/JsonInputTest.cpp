#ifdef GET_RUNJSONINPUTTESTSRC_INCLUDES
#include <gtest/gtest.h>
#include <regex>
#include "JsonInputTest.h"
#endif
#ifdef GET_RUNJSONINPUTTESTSRC_CPP

using namespace Reflect;

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
    bool isSpecialized = Json::Input::Customize<CustomizeUnspecialized, int, CustomizeUnspecialized::Class::IndexOf::firstField, NoAnnotation, CustomizeUnspecialized::Class::firstField_::Field>
        ::As(std::cin, Json::context, customizeUnspecialized, customizeUnspecialized.firstField);
    EXPECT_FALSE(isSpecialized);

    isSpecialized = Json::Input::HaveSpecialization<CustomizeUnspecialized, int>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeFullySpecialized)
{
    CustomizeFullySpecialized customizeFullySpecialized = { 1, 2, 'a' };
    bool isSpecialized = Json::Input::Customize<CustomizeFullySpecialized, int, CustomizeFullySpecialized::Class::IndexOf::firstField, NoAnnotation, CustomizeFullySpecialized::Class::firstField_::Field>
        ::As(std::cin, Json::context, customizeFullySpecialized, customizeFullySpecialized.firstField);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::HaveSpecialization<CustomizeFullySpecialized, int, CustomizeFullySpecialized::Class::IndexOf::firstField, NoAnnotation, CustomizeFullySpecialized::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize5Args_OpAnnotationsDefaulted)
{
    Customize5Args_OpAnnotationsDefaulted customize5Args_OpAnnotationsDefaulted;
    bool isSpecialized = Json::Input::Customize<Customize5Args_OpAnnotationsDefaulted, int, Customize5Args_OpAnnotationsDefaulted::Class::IndexOf::firstField, NoAnnotation, Customize5Args_OpAnnotationsDefaulted::Class::firstField_::Field>
        ::As(std::cin, Json::context, customize5Args_OpAnnotationsDefaulted, customize5Args_OpAnnotationsDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::HaveSpecialization<Customize5Args_OpAnnotationsDefaulted, int, Customize5Args_OpAnnotationsDefaulted::Class::IndexOf::firstField, NoAnnotation, Customize5Args_OpAnnotationsDefaulted::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize5Args_FieldIndexDefaulted)
{
    Customize5Args_FieldIndexDefaulted customize5Args_FieldIndexDefaulted;
    bool isSpecialized = Json::Input::Customize<Customize5Args_FieldIndexDefaulted, int, Json::NoFieldIndex, NoAnnotation, Customize5Args_FieldIndexDefaulted::Class::firstField_::Field>
        ::As(std::cin, Json::context, customize5Args_FieldIndexDefaulted, customize5Args_FieldIndexDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::HaveSpecialization<Customize5Args_FieldIndexDefaulted, int, Customize5Args_FieldIndexDefaulted::Class::IndexOf::firstField, NoAnnotation, Customize5Args_FieldIndexDefaulted::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize5Args_BothDefaulted)
{
    Customize5Args_BothDefaulted customize5Args_BothDefaulted;
    bool isSpecialized = Json::Input::Customize<Customize5Args_BothDefaulted, int, Json::NoFieldIndex, NoAnnotation, Customize5Args_BothDefaulted::Class::firstField_::Field>
        ::As(std::cin, Json::context, customize5Args_BothDefaulted, customize5Args_BothDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::HaveSpecialization<Customize5Args_BothDefaulted, int, Customize5Args_BothDefaulted::Class::IndexOf::firstField, NoAnnotation, Customize5Args_BothDefaulted::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize4Args)
{
    Customize4Args customize4Args;
    bool isSpecialized = Json::Input::Customize<Customize4Args, int, Customize4Args::Class::IndexOf::firstField, NoAnnotation>
        ::As(std::cin, Json::context, customize4Args, customize4Args.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::HaveSpecialization<Customize4Args, int, Customize4Args::Class::IndexOf::firstField, NoAnnotation, Customize4Args::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize4Args_FieldIndexDefaulted)
{
    Customize4Args_FieldIndexDefaulted customize4Args_FieldIndexDefaulted;
    bool isSpecialized = Json::Input::Customize<Customize4Args_FieldIndexDefaulted, int, Json::NoFieldIndex, NoAnnotation>
        ::As(std::cin, Json::context, customize4Args_FieldIndexDefaulted, customize4Args_FieldIndexDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::HaveSpecialization<Customize4Args_FieldIndexDefaulted, int, Customize4Args_FieldIndexDefaulted::Class::IndexOf::firstField, NoAnnotation, Customize4Args_FieldIndexDefaulted::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize3Args)
{
    Customize3Args customize3Args;
    bool isSpecialized = Json::Input::Customize<Customize3Args, int, Customize3Args::Class::IndexOf::firstField>
        ::As(std::cin, Json::context, customize3Args, customize3Args.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::HaveSpecialization<Customize3Args, int, Customize3Args::Class::IndexOf::firstField, NoAnnotation, Customize3Args::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, Customize2Args)
{
    Customize2Args customize2Args;
    bool isSpecialized = Json::Input::Customize<Customize2Args, int>
        ::As(std::cin, Json::context, customize2Args, customize2Args.firstField);
    EXPECT_TRUE(isSpecialized);
    
    isSpecialized = Json::Input::HaveSpecialization<Customize2Args, int, Customize2Args::Class::IndexOf::firstField, NoAnnotation, Customize2Args::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeTypeFullySpecialized)
{
    std::stringstream input("0");
    CustomizeTypeFullySpecialized customizeTypeFullySpecialized;
    bool isSpecialized = Json::Input::CustomizeType<CustomizeTypeFullySpecialized, NoAnnotation, CustomizeTypeFullySpecialized::Class::firstField_::Field>
        ::As(input, Json::context, customizeTypeFullySpecialized);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::HaveSpecialization<CustomizeTypeFullySpecialized, CustomizeTypeFullySpecialized, CustomizeTypeFullySpecialized::Class::IndexOf::firstField, NoAnnotation, CustomizeTypeFullySpecialized::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeType3Args_OpAnnotationsDefaulted)
{
    CustomizeType3Args_OpAnnotationsDefaulted customizeType3Args_OpAnnotationsDefaulted;
    bool isSpecialized = Json::Input::CustomizeType<CustomizeType3Args_OpAnnotationsDefaulted, NoAnnotation, CustomizeType3Args_OpAnnotationsDefaulted::Class::firstField_::Field>
        ::As(std::cin, Json::context, customizeType3Args_OpAnnotationsDefaulted);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::HaveSpecialization<CustomizeType3Args_OpAnnotationsDefaulted, CustomizeType3Args_OpAnnotationsDefaulted, CustomizeType3Args_OpAnnotationsDefaulted::Class::IndexOf::firstField, NoAnnotation, CustomizeType3Args_OpAnnotationsDefaulted::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeType2Args)
{
    CustomizeType2Args customizeType2Args;
    bool isSpecialized = Json::Input::CustomizeType<CustomizeType2Args, NoAnnotation>
        ::As(std::cin, Json::context, customizeType2Args);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::HaveSpecialization<CustomizeType2Args, CustomizeType2Args, CustomizeType2Args::Class::IndexOf::firstField, NoAnnotation, CustomizeType2Args::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonInputCustomizersTest, CustomizeType1Arg)
{
    CustomizeType1Arg customizeType1Arg;
    bool isSpecialized = Json::Input::CustomizeType<CustomizeType1Arg>
        ::As(std::cin, Json::context, customizeType1Arg);
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Input::HaveSpecialization<CustomizeType1Arg, CustomizeType1Arg, CustomizeType1Arg::Class::IndexOf::firstField, NoAnnotation, CustomizeType1Arg::Class::firstField_::Field>;
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
    Cacheable cacheable;
    std::multimap<size_t, Json::JsonField> & fieldNameToJsonField =
        Json::Input::getClassFieldCache<Cacheable>(cacheable);

    bool foundField = false;
    auto aMatches = fieldNameToJsonField.equal_range(Json::Input::Cache::strHash("a"));
    for ( auto it = aMatches.first; it != aMatches.second; ++it )
    {
        if ( it->second.name.compare("a") == 0 )
        {
            foundField = true;
            EXPECT_EQ(0, it->second.index);
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
            EXPECT_EQ(1, it->second.index);
            EXPECT_EQ(Json::JsonField::Type::Regular, it->second.type);
        }
    }
    EXPECT_TRUE(foundField);
}

TEST_HEADER(JsonInputCacheTest, GetJsonField)
{
    Cacheable cacheable;
    Json::JsonField* jsonField = Json::Input::getJsonField(cacheable, "b");
    EXPECT_FALSE(jsonField == nullptr);
    EXPECT_EQ(1, jsonField->index);
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
    EXPECT_NO_THROW(Json::Consume::Null<true>(validNull, c));

    std::stringstream incompleteNull("nu");
    EXPECT_THROW(Json::Consume::Null<true>(incompleteNull, c), Json::UnexpectedInputEnd);
}

TEST_HEADER(JsonInputConsumeTest, NullToStream)
{
    char c = '\0';
    std::stringstream source("null,");
    std::stringstream receiver;
    Json::Consume::Null<true>(source, c, receiver);
    EXPECT_STREQ("null", receiver.str().c_str());
}

TEST_HEADER(JsonInputConsumeTest, TryNull)
{
    char c = '\0';
    std::stringstream nonNull("true,");
    std::stringstream partialNull("nula,");
    std::stringstream fullNull("null,");

    EXPECT_FALSE(Json::Consume::TryNull<true>(nonNull, c));
    EXPECT_THROW(Json::Consume::TryNull<true>(partialNull, c), Json::Exception);
    EXPECT_TRUE(Json::Consume::TryNull<true>(fullNull, c));
}

TEST_HEADER(JsonInputConsume, True)
{
    char c = '\0';
    std::stringstream nonTrue("false,");
    std::stringstream partialTrue("tru,");
    std::stringstream fullTrue("true,");

    EXPECT_THROW(Json::Consume::True<true>(nonTrue, c), Json::Exception);
    EXPECT_THROW(Json::Consume::True<true>(partialTrue, c), Json::Exception);
    EXPECT_NO_THROW(Json::Consume::True<true>(fullTrue, c));
}

TEST_HEADER(JsonInputConsume, TrueToStream)
{
    char c = '\0';
    std::stringstream fullTrue("true,");
    std::stringstream receiver;

    EXPECT_NO_THROW(Json::Consume::True<true>(fullTrue, c, receiver));

    EXPECT_STREQ("true", receiver.str().c_str());
}


TEST_HEADER(JsonInputConsume, False)
{
    char c = '\0';
    std::stringstream nonFalse("true,");
    std::stringstream partialFalse("fals,");
    std::stringstream fullFalse("false,");

    EXPECT_THROW(Json::Consume::False<true>(nonFalse, c), Json::Exception);
    EXPECT_THROW(Json::Consume::False<true>(partialFalse, c), Json::Exception);
    EXPECT_NO_THROW(Json::Consume::False<true>(fullFalse, c));
}

TEST_HEADER(JsonInputConsume, FalseToStream)
{
    char c = '\0';
    std::stringstream fullFalse("false,");
    std::stringstream receiver;

    EXPECT_NO_THROW(Json::Consume::False<true>(fullFalse, c, receiver));

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
    
    EXPECT_THROW(Json::Consume::Number<true>(nonNumber, c), Json::InvalidNumericCharacter);
    EXPECT_THROW(Json::Consume::Number<true>(doubleDecimalNumber, c), Json::InvalidSecondDecimal);
    EXPECT_THROW(Json::Consume::Number<true>(doubleNegativeNumber, c), Json::InvalidNumericCharacter);
    EXPECT_NO_THROW(Json::Consume::Number<true>(negativeInteger, c));
    EXPECT_NO_THROW(Json::Consume::Number<true>(validInteger, c));
    EXPECT_NO_THROW(Json::Consume::Number<true>(validFloat, c));
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
    
    EXPECT_THROW(Json::Consume::Number<true>(nonNumber, c, nonNumberReceiver), Json::InvalidNumericCharacter);
    EXPECT_THROW(Json::Consume::Number<true>(doubleDecimalNumber, c, doubleDecimalNumberReceiver), Json::InvalidSecondDecimal);
    EXPECT_THROW(Json::Consume::Number<true>(doubleNegativeNumber, c, doubleNegativeNumberReceiver), Json::InvalidNumericCharacter);
    EXPECT_NO_THROW(Json::Consume::Number<true>(negativeInteger, c, negativeIntegerReceiver));
    EXPECT_NO_THROW(Json::Consume::Number<true>(validInteger, c, validIntegerReceiver));
    EXPECT_NO_THROW(Json::Consume::Number<true>(validFloat, c, validFloatReceiver));
    
    EXPECT_STREQ("-1234", negativeIntegerReceiver.str().c_str());
    EXPECT_STREQ("2345", validIntegerReceiver.str().c_str());
    EXPECT_STREQ("123.456", validFloatReceiver.str().c_str());
}

TEST_HEADER(JsonInputConsume, String)
{
    char c = '\0';
    std::stringstream input("\"asdf\\\"\\r\\n\\fqwer\"");
    EXPECT_NO_THROW(Json::Consume::String(input, c));
}

TEST_HEADER(JsonInputConsume, StringToStream)
{
    char c = '\0';
    std::stringstream input("\"asdf\\\"\\r\\n\\fqwer\"");
    std::stringstream receiver;
    EXPECT_NO_THROW(Json::Consume::String(input, c, receiver));
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
    
    EXPECT_NO_THROW(Json::Consume::Value<true>(stringStream, c));
    EXPECT_NO_THROW(Json::Consume::Value<true>(numberStream, c));
    EXPECT_NO_THROW(Json::Consume::Value<true>(objectStream, c));
    EXPECT_NO_THROW(Json::Consume::Value<true>(arrayStream, c));
    EXPECT_NO_THROW(Json::Consume::Value<true>(trueStream, c));
    EXPECT_NO_THROW(Json::Consume::Value<true>(falseStream, c));
    EXPECT_NO_THROW(Json::Consume::Value<true>(nullStream, c));
    EXPECT_THROW(Json::Consume::Value<true>(invalidStream, c), Json::InvalidUnknownFieldValue);
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
    
    EXPECT_NO_THROW(Json::Consume::Value<true>(stringStream, c, stringReceiver));
    EXPECT_NO_THROW(Json::Consume::Value<true>(numberStream, c, numberReceiver));
    EXPECT_NO_THROW(Json::Consume::Value<true>(objectStream, c, objectReceiver));
    EXPECT_NO_THROW(Json::Consume::Value<true>(arrayStream, c, arrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Value<true>(trueStream, c, trueReceiver));
    EXPECT_NO_THROW(Json::Consume::Value<true>(falseStream, c, falseReceiver));
    EXPECT_NO_THROW(Json::Consume::Value<true>(nullStream, c, nullReceiver));
    EXPECT_THROW(Json::Consume::Value<true>(invalidStream, c, invalidReceiver), Json::InvalidUnknownFieldValue);
    
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
    
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(stringArray, c));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(numberArray, c));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(objectArray, c));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(arrayArray, c));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(boolArray, c));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(nullArray, c));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(mixedArray, c));
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
    
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(emptyArray, c, emptyArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(stringArray, c, stringArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(numberArray, c, numberArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(objectArray, c, objectArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(arrayArray, c, arrayArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(boolArray, c, boolArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(nullArray, c, nullArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<true>(mixedArray, c, mixedArrayReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<false>(emptyObj, c, emptyObjReceiver));
    EXPECT_NO_THROW(Json::Consume::Iterable<false>(obj, c, objReceiver));
    
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
    
    EXPECT_NO_THROW(Json::Read::ObjectPrefix(objPrefix, c));
    EXPECT_THROW(Json::Read::ObjectPrefix(arrayPrefix, c), Json::Exception);
    EXPECT_THROW(Json::Read::ObjectPrefix(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, TryObjectSuffix)
{
    std::stringstream objSuffix("}");
    std::stringstream arraySuffix("]");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::TryObjectSuffix(objSuffix));
    EXPECT_FALSE(Json::Read::TryObjectSuffix(arraySuffix));
    EXPECT_FALSE(Json::Read::TryObjectSuffix(letter));
}

TEST_HEADER(JsonInputReadAffix, FieldSeparator)
{
    std::stringstream fieldSeparator(",");
    std::stringstream objSuffix("}");
    std::stringstream arraySuffix("]");
    std::stringstream letter("a");

    EXPECT_TRUE(Json::Read::FieldSeparator(fieldSeparator));
    EXPECT_FALSE(Json::Read::FieldSeparator(objSuffix));
    EXPECT_THROW(Json::Read::FieldSeparator(arraySuffix), Json::Exception);
    EXPECT_THROW(Json::Read::FieldSeparator(letter), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, FieldNameValueSeparator)
{
    char c = '\0';
    std::stringstream fieldNameValueSeparator(":");
    std::stringstream letter("a");
    EXPECT_NO_THROW(Json::Read::FieldNameValueSeparator(fieldNameValueSeparator, c));
    EXPECT_THROW(Json::Read::FieldNameValueSeparator(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, ArrayPrefix)
{
    char c = '\0';
    std::stringstream arrayPrefix("[");
    std::stringstream objectPrefix("{");
    std::stringstream letter("a");
    EXPECT_NO_THROW(Json::Read::ArrayPrefix(arrayPrefix, c));
    EXPECT_THROW(Json::Read::ArrayPrefix(objectPrefix, c), Json::Exception);
    EXPECT_THROW(Json::Read::ArrayPrefix(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, TrySingularTupleArrayPrefix)
{
    char c = '\0';
    std::stringstream arrayPrefix("[");
    std::stringstream objectPrefix("{");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::TrySingularTupleArrayPrefix(arrayPrefix, c));
    EXPECT_FALSE(Json::Read::TrySingularTupleArrayPrefix(objectPrefix, c));
    EXPECT_FALSE(Json::Read::TrySingularTupleArrayPrefix(letter, c));
}

TEST_HEADER(JsonInputReadAffix, TryArraySuffix)
{
    std::stringstream arraySuffix("]");
    std::stringstream objectSuffix("}");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::TryArraySuffix(arraySuffix));
    EXPECT_FALSE(Json::Read::TryArraySuffix(objectSuffix));
    EXPECT_FALSE(Json::Read::TryArraySuffix(letter));
}

TEST_HEADER(JsonInputReadAffix, EitherIterablePrefix)
{
    char c = '\0';
    std::stringstream objectPrefix("{");
    std::stringstream arrayPrefix("[");
    std::stringstream objectSuffix("}");
    std::stringstream arraySuffix("}");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::IterablePrefix(objectPrefix, c));
    EXPECT_FALSE(Json::Read::IterablePrefix(arrayPrefix, c));
    EXPECT_THROW(Json::Read::IterablePrefix(objectSuffix, c), Json::Exception);
    EXPECT_THROW(Json::Read::IterablePrefix(arraySuffix, c), Json::Exception);
    EXPECT_THROW(Json::Read::IterablePrefix(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, PeekIterablePrefix)
{
    char c = '\0';
    std::stringstream objectPrefix("{");
    std::stringstream arrayPrefix("[");
    std::stringstream objectSuffix("}");
    std::stringstream arraySuffix("}");
    std::stringstream letter("a");
    
    EXPECT_TRUE(Json::Read::PeekIterablePrefix(objectPrefix, c));
    EXPECT_FALSE(Json::Read::PeekIterablePrefix(arrayPrefix, c));
    EXPECT_THROW(Json::Read::PeekIterablePrefix(objectSuffix, c), Json::Exception);
    EXPECT_THROW(Json::Read::PeekIterablePrefix(arraySuffix, c), Json::Exception);
    EXPECT_THROW(Json::Read::PeekIterablePrefix(letter, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, SpecificIterablePrefix)
{
    char c = '\0';
    std::stringstream objectPrefixExpected("{");
    std::stringstream objectPrefixUnexpected("{");
    std::stringstream arrayPrefixExpected("[");
    std::stringstream arrayPrefixUnexpected("[");
    
    EXPECT_NO_THROW(Json::Read::IterablePrefix<true>(objectPrefixExpected, c));
    EXPECT_THROW(Json::Read::IterablePrefix<false>(objectPrefixUnexpected, c), Json::Exception);
    EXPECT_NO_THROW(Json::Read::IterablePrefix<false>(arrayPrefixExpected, c));
    EXPECT_THROW(Json::Read::IterablePrefix<true>(arrayPrefixUnexpected, c), Json::Exception);
}

TEST_HEADER(JsonInputReadAffix, TryIterableSuffix)
{
    std::stringstream objectSuffixExpected("}");
    std::stringstream objectSuffixUnexpected("}");
    std::stringstream arraySuffixExpected("]");
    std::stringstream arraySuffixUnexpected("]");
    
    EXPECT_TRUE(Json::Read::TryIterableSuffix<true>(objectSuffixExpected));
    EXPECT_FALSE(Json::Read::TryIterableSuffix<false>(objectSuffixUnexpected));
    EXPECT_TRUE(Json::Read::TryIterableSuffix<false>(arraySuffixExpected));
    EXPECT_FALSE(Json::Read::TryIterableSuffix<true>(arraySuffixUnexpected));
}

TEST_HEADER(JsonInputReadAffix, TryIterableSuffixInstanced)
{
    std::stringstream objectSuffixExpected("}");
    std::stringstream objectSuffixUnexpected("}");
    std::stringstream arraySuffixExpected("]");
    std::stringstream arraySuffixUnexpected("]");
    
    EXPECT_TRUE(Json::Read::TryIterableSuffix(objectSuffixExpected, true));
    EXPECT_FALSE(Json::Read::TryIterableSuffix(objectSuffixUnexpected, false));
    EXPECT_TRUE(Json::Read::TryIterableSuffix(arraySuffixExpected, false));
    EXPECT_FALSE(Json::Read::TryIterableSuffix(arraySuffixUnexpected, true));
}

TEST_HEADER(JsonInputReadAffix, IterableElementSeparator)
{
    std::stringstream iterableElementSeparator(",");
    std::stringstream objectTerminator("}");
    std::stringstream objectTerminatorUnexpected("}");
    std::stringstream arrayTerminator("]");
    std::stringstream arrayTerminatorUnexpected("]");
    
    EXPECT_TRUE(Json::Read::IterableElementSeparator<true>(iterableElementSeparator));
    EXPECT_FALSE(Json::Read::IterableElementSeparator<true>(objectTerminator));
    EXPECT_THROW(Json::Read::IterableElementSeparator<false>(objectTerminatorUnexpected), Json::Exception);
    EXPECT_FALSE(Json::Read::IterableElementSeparator<false>(arrayTerminator));
    EXPECT_THROW(Json::Read::IterableElementSeparator<true>(arrayTerminatorUnexpected), Json::Exception);
}

TEST_HEADER(JsonInputRead, Customization)
{
    CustomizeUnspecialized customizeUnspecialized;
    CustomizeFullySpecialized customizeFullySpecialized;
    Customize5Args_OpAnnotationsDefaulted customize5Args_OpAnnotationsDefaulted;
    Customize5Args_FieldIndexDefaulted customize5Args_FieldIndexDefaulted;
    Customize5Args_BothDefaulted customize5Args_BothDefaulted;
    Customize4Args customize4Args;
    Customize4Args_FieldIndexDefaulted customize4Args_FieldIndexDefaulted;
    Customize3Args customize3Args;
    Customize2Args customize2Args;
    CustomizeTypeFullySpecialized customizeTypeFullySpecialized;
    CustomizeType3Args_OpAnnotationsDefaulted customizeType3Args_OpAnnotationsDefaulted;
    CustomizeType2Args customizeType2Args;
    CustomizeType1Arg customizeType1Arg;

    using AField = Fields::Field<>;

    bool customized = Json::Read::Customization<CustomizeUnspecialized, int, 0, NoAnnotation, AField>(std::cin, Json::context, customizeUnspecialized, customizeUnspecialized.firstField);
    EXPECT_FALSE(customized);
    customized = Json::Read::Customization<CustomizeFullySpecialized, int, 0, NoAnnotation, AField>(std::cin, Json::context, customizeFullySpecialized, customizeFullySpecialized.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<Customize5Args_OpAnnotationsDefaulted, int, 0, NoAnnotation, AField>(std::cin, Json::context, customize5Args_OpAnnotationsDefaulted, customize5Args_OpAnnotationsDefaulted.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<Customize5Args_FieldIndexDefaulted, int, 0, NoAnnotation, AField>(std::cin, Json::context, customize5Args_FieldIndexDefaulted, customize5Args_FieldIndexDefaulted.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<Customize5Args_BothDefaulted, int, 0, NoAnnotation, AField>(std::cin, Json::context, customize5Args_BothDefaulted, customize5Args_BothDefaulted.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<Customize4Args, int, 0, NoAnnotation, AField>(std::cin, Json::context, customize4Args, customize4Args.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<Customize4Args_FieldIndexDefaulted, int, 0, NoAnnotation, AField>(std::cin, Json::context, customize4Args_FieldIndexDefaulted, customize4Args_FieldIndexDefaulted.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<Customize3Args, int, 0, NoAnnotation, AField>(std::cin, Json::context, customize3Args, customize3Args.firstField);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<Customize2Args, int, 0, NoAnnotation, AField>(std::cin, Json::context, customize2Args, customize2Args.firstField);
    EXPECT_TRUE(customized);
    std::stringstream input("0");
    customized = Json::Read::Customization<CustomizeTypeFullySpecialized, CustomizeTypeFullySpecialized, 0, NoAnnotation, AField>(input, Json::context, customizeTypeFullySpecialized, customizeTypeFullySpecialized);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<CustomizeType3Args_OpAnnotationsDefaulted, CustomizeType3Args_OpAnnotationsDefaulted, 0, NoAnnotation, AField>(std::cin, Json::context, customizeType3Args_OpAnnotationsDefaulted, customizeType3Args_OpAnnotationsDefaulted);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<CustomizeType2Args, CustomizeType2Args, 0, NoAnnotation, AField>(std::cin, Json::context, customizeType2Args, customizeType2Args);
    EXPECT_TRUE(customized);
    customized = Json::Read::Customization<CustomizeType1Arg, CustomizeType1Arg, 0, NoAnnotation, AField>(std::cin, Json::context, customizeType1Arg, customizeType1Arg);
    EXPECT_TRUE(customized);
}

TEST_HEADER(JsonInputRead, True)
{
    char c = '\0';
    std::stringstream trueStream("true,");
    std::stringstream falseStream("false,");
    EXPECT_TRUE(Json::Read::True<true>(trueStream, c));
    EXPECT_THROW(Json::Read::True<true>(falseStream, c), Json::Exception);
}

TEST_HEADER(JsonInputRead, False)
{
    char c = '\0';
    std::stringstream falseStream("false,");
    std::stringstream trueStream("true,");
    EXPECT_FALSE(Json::Read::False<true>(falseStream, c));
    EXPECT_THROW(Json::Read::False<true>(trueStream, c), Json::Exception);
}

TEST_HEADER(JsonInputRead, BoolReference)
{
    char c = '\0';
    bool value = false;

    std::stringstream trueStream("true,");
    Json::Read::Bool<true>(trueStream, c, value);
    EXPECT_TRUE(value);

    std::stringstream falseStream("false,");
    Json::Read::Bool<true>(falseStream, c, value);
    EXPECT_FALSE(value);

    std::stringstream invalidStream("asdf");
    EXPECT_THROW(Json::Read::Bool<true>(invalidStream, c, value), Json::Exception);
}

TEST_HEADER(JsonInputRead, BoolReturn)
{
    char c = '\0';
    
    std::stringstream trueStream("true,");
    EXPECT_TRUE(Json::Read::Bool<true>(trueStream, c));

    std::stringstream falseStream("false,");
    EXPECT_FALSE(Json::Read::Bool<true>(falseStream, c));

    std::stringstream invalidStream("asdf");
    EXPECT_THROW(Json::Read::Bool<true>(invalidStream, c), Json::Exception);
}

TEST_HEADER(JsonInputRead, Number)
{
    char c = '\0';
    std::stringstream validNumber("1234,");
    std::string result = Json::Input::Read::Number<true>(validNumber, c);
    EXPECT_STREQ("1234", result.c_str());

    std::stringstream invalidNumber("asdf,");
    EXPECT_THROW(Json::Input::Read::Number<true>(invalidNumber, c), Json::Exception);
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
    
    Json::Read::String(emptyStr, c, emptyStrReceiver);
    Json::Read::String(basicStr, c, basicStrReceiver);
    Json::Read::String(quoteStr, c, quoteStrReceiver);
    Json::Read::String(backslashStr, c, backslashStrReceiver);
    Json::Read::String(forwardslashStr, c, forwardslashStrReceiver);
    Json::Read::String(backspaceStr, c, backspaceStrReceiver);
    Json::Read::String(formFeedStr, c, formFeedStrReceiver);
    Json::Read::String(lineFeedStr, c, lineFeedStrReceiver);
    Json::Read::String(carriageReturnStr, c, carriageReturnStrReceiver);
    Json::Read::String(tabStr, c, tabStrReceiver);
    Json::Read::String(unicodeCharacterStr, c, unicodeCharacterStrReceiver);
    Json::Read::String(everything, c, everythingReceiver);
    
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
    Json::Read::String(everything, c, result);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

TEST_HEADER(JsonInputRead, StringTemplate)
{
    char c = '\0';
    std::stringstream integerStream("\"1234\"");
    int result = 0;
    Json::Read::String(integerStream, c, result);
    EXPECT_EQ(1234, result);
}

TEST_HEADER(JsonInputRead, StringCharReturned)
{
    char c = '\0';
    std::stringstream everything("\" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 \"");
    std::string result = Json::Read::String(everything, c);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

TEST_HEADER(JsonInputRead, StringReturned)
{
    std::stringstream everything("\" asdf \\\"\\\\\\/\\b\\f\\n\\r\\t\\u0030 \"");
    std::string result = Json::Read::String(everything);
    EXPECT_STREQ(" asdf \"\\/\b\f\n\r\t0 ", result.c_str());
}

enum_t(EnumIntEnum, uint32_t, {
    none = 0,
    leet = 1337,
    overNineThousand = 9001
});

TEST_HEADER(JsonInputRead, EnumInt)
{
    std::stringstream leet("1337");
    std::stringstream overNineThousand("9001");
    EnumIntEnum result = EnumIntEnum::none;
    Json::Read::EnumInt(leet, result);
    EXPECT_EQ(EnumIntEnum::leet, result);
    Json::Read::EnumInt(overNineThousand, result);
    EXPECT_EQ(EnumIntEnum::overNineThousand, result);
}

TEST_HEADER(JsonInputRead, FieldName)
{
    char c = '\0';
    std::stringstream fieldNameStream("\"asdf\"");
    std::string fieldName = Json::Read::FieldName(fieldNameStream, c);
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
    
    assigner = Json::Read::GenericValue<true>(stringStream, Json::context, c);
    EXPECT_EQ(Json::Value::Type::String, assigner->get()->type());
    assigner = Json::Read::GenericValue<true>(objectStream, Json::context, c);
    EXPECT_EQ(Json::Value::Type::Object, assigner->get()->type());
    assigner = Json::Read::GenericValue<true>(arrayStream, Json::context, c);
    EXPECT_EQ(Json::Value::Type::NullArray, assigner->get()->type());
    assigner = Json::Read::GenericValue<true>(boolStream, Json::context, c);
    EXPECT_EQ(Json::Value::Type::Boolean, assigner->get()->type());
    assigner = Json::Read::GenericValue<true>(numberStream, Json::context, c);
    EXPECT_EQ(Json::Value::Type::Number, assigner->get()->type());
    assigner = Json::Read::GenericValue<true>(nullStream, Json::context, c);
    EXPECT_TRUE(assigner->get() == nullptr);

    EXPECT_THROW(Json::Read::GenericValue<true>(invalidStream, Json::context, c), Json::InvalidUnknownFieldValue);
}

TEST_HEADER(JsonInputRead, ValueType)
{
    char c = '\"';
    EXPECT_EQ(Json::Value::Type::String, Json::Read::ValueType(c));

    c = '{';
    EXPECT_EQ(Json::Value::Type::Object, Json::Read::ValueType(c));

    c = '[';
    EXPECT_EQ(Json::Value::Type::Array, Json::Read::ValueType(c));

    c = 't';
    EXPECT_EQ(Json::Value::Type::Boolean, Json::Read::ValueType(c));
    c = 'f';
    EXPECT_EQ(Json::Value::Type::Boolean, Json::Read::ValueType(c));

    c = '-';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '0';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '1';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '2';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '3';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '4';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '5';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '6';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '7';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '8';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));
    c = '9';
    EXPECT_EQ(Json::Value::Type::Number, Json::Read::ValueType(c));

    c = 'n';
    EXPECT_EQ(Json::Value::Type::Null, Json::Read::ValueType(c));

    c = 'q';
    EXPECT_THROW(Json::Read::ValueType(c), Json::InvalidUnknownFieldValue);
}

TEST_HEADER(JsonInputRead, GenericArray)
{
    char c = '\0';
    std::stringstream emptyArray("[]");
    std::stringstream nullArray("[null, null, null]");
    std::stringstream boolArray("[true, false, false, true]");
    std::stringstream numberArray("[12, 13, 13.5, 15, -12.322, -5, 6]");
    std::stringstream stringArray("[\"asdf\",\"qwerty\"]");
    std::stringstream objectArray("[{}, {\"field\":true}, {\"str\":\"hello world\",\"num\":42}]");
    std::stringstream mixedArray("[null, false]");
    std::stringstream nestedArrays("[ [[true, false], [null, null]], [[\"as\"], [{\"df\":\"qw\"}]] ]");

    std::shared_ptr<Json::Value::Assigner> assigner = nullptr;

    assigner = Json::Read::GenericArray<true>(emptyArray, Json::context, c);
    EXPECT_EQ(Json::Value::Type::NullArray, assigner->get()->type());

    assigner = Json::Read::GenericArray<true>(nullArray, Json::context, c);
    EXPECT_EQ(Json::Value::Type::NullArray, assigner->get()->type());
    EXPECT_EQ(3, assigner->get()->nullArray());

    assigner = Json::Read::GenericArray<true>(boolArray, Json::context, c);
    EXPECT_EQ(Json::Value::Type::BoolArray, assigner->get()->type());
    EXPECT_TRUE(assigner->get()->boolArray()[0]);
    EXPECT_FALSE(assigner->get()->boolArray()[1]);
    EXPECT_FALSE(assigner->get()->boolArray()[2]);
    EXPECT_TRUE(assigner->get()->boolArray()[3]);
    
    assigner = Json::Read::GenericArray<true>(numberArray, Json::context, c);
    EXPECT_EQ(Json::Value::Type::NumberArray, assigner->get()->type());
    EXPECT_EQ(7, assigner->get()->arraySize());
    EXPECT_STREQ("12", assigner->get()->numberArray()[0].c_str());
    EXPECT_STREQ("13", assigner->get()->numberArray()[1].c_str());
    EXPECT_STREQ("13.5", assigner->get()->numberArray()[2].c_str());
    EXPECT_STREQ("15", assigner->get()->numberArray()[3].c_str());
    EXPECT_STREQ("-12.322", assigner->get()->numberArray()[4].c_str());
    EXPECT_STREQ("-5", assigner->get()->numberArray()[5].c_str());
    EXPECT_STREQ("6", assigner->get()->numberArray()[6].c_str());

    assigner = Json::Read::GenericArray<true>(stringArray, Json::context, c);
    EXPECT_EQ(Json::Value::Type::StringArray, assigner->get()->type());
    EXPECT_EQ(2, assigner->get()->arraySize());
    EXPECT_STREQ("asdf", assigner->get()->stringArray()[0].c_str());
    EXPECT_STREQ("qwerty", assigner->get()->stringArray()[1].c_str());

    assigner = Json::Read::GenericArray<true>(objectArray, Json::context, c);
    EXPECT_EQ(Json::Value::Type::ObjectArray, assigner->get()->type());
    EXPECT_EQ(3, assigner->get()->arraySize());
    EXPECT_TRUE(assigner->get()->objectArray()[0].empty());
    EXPECT_TRUE(assigner->get()->objectArray()[1].find("field")->second->boolean());
    EXPECT_STREQ("hello world", assigner->get()->objectArray()[2].find("str")->second->string().c_str());
    EXPECT_STREQ("42", assigner->get()->objectArray()[2].find("num")->second->number().c_str());

    assigner = Json::Read::GenericArray<true>(mixedArray, Json::context, c);
    EXPECT_EQ(Json::Value::Type::MixedArray, assigner->get()->type());
    EXPECT_EQ(2, assigner->get()->arraySize());
    EXPECT_TRUE(assigner->get()->mixedArray()[0] == nullptr);
    EXPECT_TRUE(assigner->get()->mixedArray()[1]->boolean() == false);

    assigner = Json::Read::GenericArray<true>(nestedArrays, Json::context, c);
    EXPECT_EQ(Json::Value::Type::MixedArray, assigner->get()->type());
    EXPECT_EQ(2, assigner->get()->arraySize());
    EXPECT_TRUE(assigner->get()->mixedArray()[0]->mixedArray()[0]->boolArray()[0]);
    EXPECT_FALSE(assigner->get()->mixedArray()[0]->mixedArray()[0]->boolArray()[1]);
    EXPECT_EQ(2, assigner->get()->mixedArray()[0]->mixedArray()[1]->nullArray());
    EXPECT_STREQ("as", assigner->get()->mixedArray()[1]->mixedArray()[0]->stringArray()[0].c_str());
    EXPECT_STREQ("qw", assigner->get()->mixedArray()[1]->mixedArray()[1]->objectArray()[0].find("df")->second->string().c_str());
}

TEST_HEADER(JsonInputRead, GenericObject)
{
    char c = '\0';
    std::shared_ptr<Json::Value::Assigner> assigner = nullptr;

    std::stringstream emptyObject("{}");
    std::stringstream basicObject("{\"one\":1,\"two\":2}");
    
    assigner = Json::Read::GenericObject(emptyObject, Json::context, c);
    EXPECT_TRUE(assigner->get()->object().empty());

    assigner = Json::Read::GenericObject(basicObject, Json::context, c);
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

struct VariousValues
{
    CustomizeTypeFullySpecialized customized;
    
    std::shared_ptr<Json::Value> genericNonNull;
    std::shared_ptr<Json::Value> genericNull;

    std::shared_ptr<int> sharedPointerNull;
    std::unique_ptr<int> uniquePointerNull;
    int* regularPointerNull;

    int* regularPointerToBecomeNull;
    int* regularPointerValue;

    Json::Object genericValue;
    std::vector<int> intVector;
    ComposedObj composedObj;
    NOTE(integerString, Json::Stringify)
    int integerString;
    NOTE(enumInt, Json::EnumInt)
    EnumIntEnum enumInt;
    bool boolean;
    static constexpr const int constant = 0;
    std::string str;
    
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
        genericValue, intVector, composedObj, integerString, enumInt, boolean, constant, str,
        emptyTuple, singleTuple, doubleTuple, tripleTuple, pair, primitiveKey, complexKey, intIntArrayTuple,
        intIntKeyableTuple, intIntTupleTuple)
};

TEST_HEADER(JsonInputRead, Value)
{
    char c = '\0';
    int anInt = 0;

    VariousValues v = {};
    v.genericNonNull = Json::Bool::Make(false);
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

    std::stringstream customizedStream("1234,");
    Json::Read::Value<true, VariousValues::Class::customized_::Field, CustomizeTypeFullySpecialized, VariousValues>(
        customizedStream, Json::context, c, v, v.customized);
    EXPECT_EQ(1234, v.customized.firstField);

    std::stringstream genericNonNullNull("null,");
    Json::Read::Value<true, VariousValues::Class::genericNonNull_::Field, std::shared_ptr<Json::Value>, VariousValues>(
        genericNonNullNull, Json::context, c, v, v.genericNonNull);
    EXPECT_TRUE(v.genericNonNull == nullptr);

    std::stringstream genericNullNonNull("true,");
    Json::Read::Value<true, VariousValues::Class::genericNull_::Field, std::shared_ptr<Json::Value>, VariousValues>(
        genericNullNonNull, Json::context, c, v, v.genericNull);
    EXPECT_TRUE(v.genericNull != nullptr);
    EXPECT_EQ(Json::Value::Type::Boolean, v.genericNull->type());

    std::stringstream sharedPtrNullptr("111,");
    Json::Read::Value<true, VariousValues::Class::sharedPointerNull_::Field, std::shared_ptr<int>, VariousValues>(
        sharedPtrNullptr, Json::context, c, v, v.sharedPointerNull);
    EXPECT_TRUE(v.sharedPointerNull != nullptr);
    EXPECT_EQ(111, *v.sharedPointerNull);

    std::stringstream uniquePtrNullptr("222,");
    Json::Read::Value<true, VariousValues::Class::uniquePointerNull_::Field, std::unique_ptr<int>, VariousValues>(
        uniquePtrNullptr, Json::context, c, v, v.uniquePointerNull);
    EXPECT_TRUE(v.uniquePointerNull != nullptr);
    EXPECT_EQ(222, *v.uniquePointerNull);

    std::stringstream regularPtrNullptr("null,");
    Json::Read::Value<true, VariousValues::Class::regularPointerNull_::Field, int*, VariousValues>(
        regularPtrNullptr, Json::context, c, v, v.regularPointerNull);
    EXPECT_TRUE(v.regularPointerNull == nullptr);

    std::stringstream regularPtrToBecomeNull("null,");
    Json::Read::Value<true, VariousValues::Class::regularPointerToBecomeNull_::Field, int*, VariousValues>(
        regularPtrToBecomeNull, Json::context, c, v, v.regularPointerToBecomeNull);
    EXPECT_TRUE(v.regularPointerToBecomeNull == nullptr);

    std::stringstream regularPtrValue("999,");
    Json::Read::Value<true, VariousValues::Class::regularPointerValue_::Field, int*, VariousValues>(
        regularPtrValue, Json::context, c, v, v.regularPointerValue);
    EXPECT_EQ(999, anInt);

    std::stringstream composedObjStream("{\"a\": 1}");
    Json::Read::Value<true, VariousValues::Class::composedObj_::Field, ComposedObj, VariousValues>(
        composedObjStream, Json::context, c, v, v.composedObj);
    EXPECT_EQ(1, v.composedObj.a);

    std::stringstream integerStringStream("\"4567\"");
    Json::Read::Value<true, VariousValues::Class::integerString_::Field, int, VariousValues>(
        integerStringStream, Json::context, c, v, v.integerString);
    EXPECT_EQ(4567, v.integerString);

    std::stringstream enumIntStream("1337,");
    Json::Read::Value<true, VariousValues::Class::enumInt_::Field, EnumIntEnum, VariousValues>(
        enumIntStream, Json::context, c, v, v.enumInt);
    EXPECT_EQ(EnumIntEnum::leet, v.enumInt);

    std::stringstream booleanStream("true,");
    Json::Read::Value<true, VariousValues::Class::boolean_::Field, bool, VariousValues>(
        booleanStream, Json::context, c, v, v.boolean);
    EXPECT_TRUE(v.boolean);
    
    std::stringstream constInt("9001,");
    Json::Read::Value<true, VariousValues::Class::constant_::Field, const int, VariousValues>(
        constInt, Json::context, c, v, v.constant);
    EXPECT_EQ(0, v.constant);

    std::stringstream strStream("\"asdf\",");
    Json::Read::Value<true, VariousValues::Class::str_::Field, std::string, VariousValues>(
        strStream, Json::context, c, v, v.str);
    EXPECT_STREQ("asdf", v.str.c_str());

    std::stringstream nullTupleStream("null,");
    Json::Read::Value<true, VariousValues::Class::emptyTuple_::Field, std::tuple<>, VariousValues>(
        nullTupleStream, Json::context, c, v, v.emptyTuple);
    
    std::stringstream emptyArrayTupleStream("[],");
    Json::Read::Value<true, VariousValues::Class::emptyTuple_::Field, std::tuple<>, VariousValues>(
        emptyArrayTupleStream, Json::context, c, v, v.emptyTuple);

    std::stringstream singleTupleValueStream("1,");
    Json::Read::Value<true, VariousValues::Class::singleTuple_::Field, std::tuple<int>, VariousValues>(
        singleTupleValueStream, Json::context, c, v, v.singleTuple);
    EXPECT_EQ(1, std::get<0>(v.singleTuple));

    std::stringstream singleTupleArrayStream("[2],");
    Json::Read::Value<true, VariousValues::Class::singleTuple_::Field, std::tuple<int>, VariousValues>(
        singleTupleArrayStream, Json::context, c, v, v.singleTuple);
    EXPECT_EQ(2, std::get<0>(v.singleTuple));

    std::stringstream doubleTupleArrayStream("[1, 2],");
    Json::Read::Value<true, VariousValues::Class::doubleTuple_::Field, std::tuple<int, int>, VariousValues>(
        doubleTupleArrayStream, Json::context, c, v, v.doubleTuple);
    EXPECT_EQ(1, std::get<0>(v.doubleTuple));
    EXPECT_EQ(2, std::get<1>(v.doubleTuple));

    std::stringstream tripleTupleArrayStream("[1, 2, 3],");
    Json::Read::Value<true, VariousValues::Class::tripleTuple_::Field, std::tuple<int, int, int>, VariousValues>(
        tripleTupleArrayStream, Json::context, c, v, v.tripleTuple);
    EXPECT_EQ(1, std::get<0>(v.tripleTuple));
    EXPECT_EQ(2, std::get<1>(v.tripleTuple));
    EXPECT_EQ(3, std::get<2>(v.tripleTuple));

    std::stringstream pairStream("[1, 2],");
    Json::Read::Value<true, VariousValues::Class::pair_::Field, std::pair<int, int>, VariousValues>(
        pairStream, Json::context, c, v, v.pair);
    EXPECT_EQ(1, std::get<0>(v.pair));
    EXPECT_EQ(2, std::get<1>(v.pair));

    std::stringstream primitiveKeyStream("{\"1\": {\"a\":2}},");
    Json::Read::Value<true, VariousValues::Class::primitiveKey_::Field, std::map<int, Keyable>, VariousValues>(
        primitiveKeyStream, Json::context, c, v, v.primitiveKey);
    auto found = v.primitiveKey.find(1);
    EXPECT_TRUE(found != v.primitiveKey.end());
    EXPECT_EQ(2, found->second.a);

    std::stringstream primitiveKeyArrayStream("[[3, {\"a\":4}]],");
    Json::Read::Value<true, VariousValues::Class::primitiveKey_::Field, std::map<int, Keyable>, VariousValues>(
        primitiveKeyArrayStream, Json::context, c, v, v.primitiveKey);
    auto primitiveKeyArrayFound = v.primitiveKey.find(3);
    EXPECT_TRUE(primitiveKeyArrayFound != v.primitiveKey.end());
    EXPECT_EQ(4, primitiveKeyArrayFound->second.a);

    std::stringstream complexKeyStream("[{\"key\":{\"a\":1},\"value\":2}],");
    Json::Read::Value<true, VariousValues::Class::complexKey_::Field, std::map<Keyable, int>, VariousValues>(
        complexKeyStream, Json::context, c, v, v.complexKey);
    Keyable keyable { 1 };
    auto complexKeyFound = v.complexKey.find(keyable);
    EXPECT_TRUE(complexKeyFound != v.complexKey.end());
    EXPECT_EQ(2, complexKeyFound->second);

    std::stringstream complexKeyArrayStream("[[{\"a\":3},4]],");
    Json::Read::Value<true, VariousValues::Class::complexKey_::Field, std::map<Keyable, int>, VariousValues>(
        complexKeyArrayStream, Json::context, c, v, v.complexKey);
    Keyable arrayKeyable { 3 };
    auto complexKeyArrayFound = v.complexKey.find(arrayKeyable);
    EXPECT_TRUE(complexKeyArrayFound != v.complexKey.end());
    EXPECT_EQ(4, complexKeyArrayFound->second);

    std::stringstream intIntArrayTupleStream("[1, 2, [3,4]],");
    Json::Read::Tuple<VariousValues::Class::intIntArrayTuple_::Field, 0>(
        intIntArrayTupleStream, Json::context, c, v, v.intIntArrayTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntArrayTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntArrayTuple));
    EXPECT_EQ(3, std::get<2>(v.intIntArrayTuple)[0]);
    EXPECT_EQ(4, std::get<2>(v.intIntArrayTuple)[1]);

    std::stringstream intIntKeyableTupleStream("[1, 2, {\"a\":3}],");
    Json::Read::Tuple<VariousValues::Class::intIntKeyableTuple_::Field, 0>(
        intIntKeyableTupleStream, Json::context, c, v, v.intIntKeyableTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntKeyableTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntKeyableTuple));
    EXPECT_EQ(3, std::get<2>(v.intIntKeyableTuple).a);

    std::stringstream intIntTupleTupleStream("[1, 2, [3, 4]],");
    Json::Read::Tuple<VariousValues::Class::intIntTupleTuple_::Field, 0>(
        intIntTupleTupleStream, Json::context, c, v, v.intIntTupleTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntTupleTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntTupleTuple));
    EXPECT_EQ(3, std::get<0>(std::get<2>(v.intIntTupleTuple)));
    EXPECT_EQ(4, std::get<1>(std::get<2>(v.intIntTupleTuple)));
}

TEST_HEADER(JsonInputRead, Tuple)
{
    char c = '\0';
    int anInt = 0;

    VariousValues v = {};
    v.genericNonNull = Json::Bool::Make(false);
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
    Json::Read::Tuple<VariousValues::Class::doubleTuple_::Field, 0>(
        doubleTupleArrayStream, Json::context, c, v, v.doubleTuple);
    EXPECT_EQ(1, std::get<0>(v.doubleTuple));
    EXPECT_EQ(2, std::get<1>(v.doubleTuple));

    std::stringstream tripleTupleArrayStream("[1, 2, 3],");
    Json::Read::Tuple<VariousValues::Class::tripleTuple_::Field, 0>(
        tripleTupleArrayStream, Json::context, c, v, v.tripleTuple);
    EXPECT_EQ(1, std::get<0>(v.tripleTuple));
    EXPECT_EQ(2, std::get<1>(v.tripleTuple));
    EXPECT_EQ(3, std::get<2>(v.tripleTuple));

    std::stringstream intIntArrayTupleStream("[1, 2, [3,4]],");
    Json::Read::Tuple<VariousValues::Class::intIntArrayTuple_::Field, 0>(
        intIntArrayTupleStream, Json::context, c, v, v.intIntArrayTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntArrayTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntArrayTuple));
    EXPECT_EQ(3, std::get<2>(v.intIntArrayTuple)[0]);
    EXPECT_EQ(4, std::get<2>(v.intIntArrayTuple)[1]);

    std::stringstream intIntKeyableTupleStream("[1, 2, {\"a\":3}],");
    Json::Read::Tuple<VariousValues::Class::intIntKeyableTuple_::Field, 0>(
        intIntKeyableTupleStream, Json::context, c, v, v.intIntKeyableTuple);
    EXPECT_EQ(1, std::get<0>(v.intIntKeyableTuple));
    EXPECT_EQ(2, std::get<1>(v.intIntKeyableTuple));
    EXPECT_EQ(3, std::get<2>(v.intIntKeyableTuple).a);

    std::stringstream intIntTupleTupleStream("[1, 2, [3, 4]],");
    Json::Read::Tuple<VariousValues::Class::intIntTupleTuple_::Field, 0>(
        intIntTupleTupleStream, Json::context, c, v, v.intIntTupleTuple);
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
    Json::Read::Pair<VariousPairs::Class::intIntPair_::Field, VariousPairs>(
        intIntPairStream, Json::context, c, v, v.intIntPair);
    EXPECT_EQ(1, std::get<0>(v.intIntPair));
    EXPECT_EQ(2, std::get<1>(v.intIntPair));

    std::stringstream intObjPairStream("[1, {\"a\":2}],");
    Json::Read::Pair<VariousPairs::Class::intObjPair_::Field, VariousPairs>(
        intObjPairStream, Json::context, c, v, v.intObjPair);
    EXPECT_EQ(1, std::get<0>(v.intObjPair));
    EXPECT_EQ(2, std::get<1>(v.intObjPair).a);

    std::stringstream objIntPairStream("[{\"a\":1}, 2],");
    Json::Read::Pair<VariousPairs::Class::objIntPair_::Field, VariousPairs>(
        objIntPairStream, Json::context, c, v, v.objIntPair);
    EXPECT_EQ(1, std::get<0>(v.objIntPair).a);
    EXPECT_EQ(2, std::get<1>(v.objIntPair));

    std::stringstream intPairPairStream("[1, [{\"a\":2}, {\"a\":3}]],");
    Json::Read::Pair<VariousPairs::Class::intPairPair_::Field, VariousPairs>(
        intPairPairStream, Json::context, c, v, v.intPairPair);
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

    REFLECT(VariousMaps, intIntMap, intObjMap, objIntMap, objObjMap)
};

TEST_HEADER(JsonInputRead, KeyValueObject)
{
    char c = '\0';

    VariousMaps v;
    v.intIntMap = { {0, 0} };
    v.intObjMap = { {0, {0}} };
    v.objIntMap = { {{0}, 0} };
    v.objObjMap = { {{0}, {0}} };
    
    std::stringstream intIntMapStream("{\"key\":1,\"value\":2},");
    std::pair<int, int> value = { 0, 0 };
    Json::Read::KeyValueObject<VariousMaps::Class::intIntMap_::Field, VariousMaps>(
        intIntMapStream, Json::context, c, v, value);
    EXPECT_EQ(1, std::get<0>(value));
    EXPECT_EQ(2, std::get<1>(value));
    
    std::stringstream intObjMapStream("{\"key\":1,\"value\":{\"a\":2}},");
    std::pair<int, Keyable> intObjElement = { 0, {0} };
    Json::Read::KeyValueObject<VariousMaps::Class::intObjMap_::Field, VariousMaps>(
        intObjMapStream, Json::context, c, v, intObjElement);
    EXPECT_EQ(1, std::get<0>(intObjElement));
    EXPECT_EQ(2, std::get<1>(intObjElement).a);
    
    std::stringstream objIntMapStream("{\"key\":{\"a\":1},\"value\":2},");
    std::pair<Keyable, int> objIntElement = { {0}, 0 };
    Json::Read::KeyValueObject<VariousMaps::Class::objIntMap_::Field, VariousMaps>(
        objIntMapStream, Json::context, c, v, objIntElement);
    EXPECT_EQ(1, std::get<0>(objIntElement).a);
    EXPECT_EQ(2, std::get<1>(objIntElement));
    
    std::stringstream objObjMapStream("{\"key\":{\"a\":1},\"value\":{\"a\":2}},");
    std::pair<Keyable, Keyable> objObjElement = { {0}, {0} };
    Json::Read::KeyValueObject<VariousMaps::Class::objObjMap_::Field, VariousMaps>(
        objObjMapStream, Json::context, c, v, objObjElement);
    EXPECT_EQ(1, std::get<0>(objObjElement).a);
    EXPECT_EQ(2, std::get<1>(objObjElement).a);
}

TEST_HEADER(JsonInputRead, FieldPair)
{
    char c = '\0';

    VariousMaps v;
    v.intIntMap = { {0, 0} };
    v.intObjMap = { {0, {0}} };
    v.objIntMap = { {{0}, 0} };
    v.objObjMap = { {{0}, {0}} };

    std::stringstream intIntFieldPairStream("\"1\":2");
    std::pair<int, int> value;
    Json::Read::FieldPair<VariousMaps::Class::intIntMap_::Field, VariousMaps>(
        intIntFieldPairStream, Json::context, c, v, value.first, value.second);
    EXPECT_EQ(1, std::get<0>(value));
    EXPECT_EQ(2, std::get<1>(value));

    std::stringstream intObjFieldPairStream("\"1\":{\"a\":2}");
    std::pair<int, Keyable> intObjValue;
    Json::Read::FieldPair<VariousMaps::Class::intObjMap_::Field, VariousMaps>(
        intObjFieldPairStream, Json::context, c, v, intObjValue.first, intObjValue.second);
    EXPECT_EQ(1, std::get<0>(intObjValue));
    EXPECT_EQ(2, std::get<1>(intObjValue).a);
}

TEST_HEADER(JsonInputRead, Iterable)
{
    char c = '\0';
    int placeholderObj = 0;

    int staticArray[4] = { 0, 0, 0, 0 };
    std::stringstream staticArrayStream("[1,2,3,4]");
    using StaticArrayField = Fields::Field<decltype(staticArray)>;
    Json::Read::Iterable<StaticArrayField>(staticArrayStream, Json::context, c, placeholderObj, staticArray);
    EXPECT_EQ(1, staticArray[0]);
    EXPECT_EQ(2, staticArray[1]);
    EXPECT_EQ(3, staticArray[2]);
    EXPECT_EQ(4, staticArray[3]);

    std::vector<int> intVector;
    std::stringstream vectorStream("[2,3,4,5]");
    using VectorField = Fields::Field<decltype(intVector)>;
    Json::Read::Iterable<VectorField>(vectorStream, Json::context, c, placeholderObj, intVector);
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
    REFLECT_EMPTY(SuperClassField)
};

TEST_HEADER(JsonInputRead, Field)
{
    RegularField regularField = {};
    regularField.a = 444;
    SuperClassField superClassField = {};
    superClassField.a = 555;

    char c = '\0';
    std::stringstream regularFieldStream(":777,");
    std::stringstream superClassFieldStream(":{\"a\":888}");

    Json::Read::Field<RegularField>(regularFieldStream, Json::context, c, regularField, "a");
    Json::Read::Field<SuperClassField>(superClassFieldStream, Json::context, c, superClassField, Json::Shared::superTypeToJsonFieldName<RegularField>().c_str());

    EXPECT_EQ(regularField.a, 777);
    EXPECT_EQ(superClassField.a, 888);

    std::stringstream unknownFieldStream(":222,");
    EXPECT_NO_THROW(Json::Read::Field<RegularField>(unknownFieldStream, Json::context, c, regularField, "b"));
}

struct ReadableObject
{
    int a;

    REFLECT(ReadableObject, a)
};

TEST_HEADER(JsonInputRead, Object)
{
    char c = '\0';
    std::stringstream objectStream("{\"a\":5}");
    ReadableObject object;
    object.a = 0;

    Json::Read::Object<ReadableObject>(objectStream, Json::context, c, object);
    EXPECT_EQ(5, object.a);
}

TEST_HEADER(JsonInput, ReflectedObject)
{
    std::stringstream objectStream("{\"a\":5}");
    ReadableObject object;
    object.a = 0;

    Json::Input::ReflectedObject<ReadableObject> reflectedObject(object, nullptr);
    objectStream >> reflectedObject;

    EXPECT_EQ(5, object.a);
}

struct JsonReferences
{
    JsonReferences() : primitive(1), primitiveReference(primitive) {}

    int primitive;
    int & primitiveReference;
    static int staticPrimitive;
    static int & staticPrimitiveReference;

    REFLECT(JsonReferences, primitive, primitiveReference, staticPrimitive, staticPrimitiveReference)
};

int JsonReferences::staticPrimitive = 2;
int & JsonReferences::staticPrimitiveReference = JsonReferences::staticPrimitive;

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
    REFLECT_EMPTY(EmptyIn)
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
    EXPECT_EQ(3, complexStruct.fieldCluster.object().find("someOtherUnknown")->second->arraySize());
    EXPECT_STREQ("4", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[0].c_str());
    EXPECT_STREQ("5", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[1].c_str());
    EXPECT_STREQ("6", complexStruct.fieldCluster.object().find("someOtherUnknown")->second->numberArray()[2].c_str());
}

#endif