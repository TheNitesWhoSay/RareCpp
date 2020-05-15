#ifdef GET_RUNJSONTESTSRC_INCLUDES
#include <gtest/gtest.h>
#include <regex>
#include "JsonTest.h"
#endif
#ifdef GET_RUNJSONTESTSRC_CPP

#ifdef USE_BUFFERED_STREAMS
using TestStreamType = StringBuffer;
StringBuffer os;
static constexpr EndL osEndl = {};
#else
using TestStreamType = std::stringstream;
std::ostream & os = std::cout;
#define osEndl std::endl
#endif

#ifdef USE_BUFFERED_STREAMS
#undef TEST_HEADER
#define TEST_HEADER(group, name) TEST(group##Buffered, name)
#else
#undef TEST_HEADER
#define TEST_HEADER(group, name) TEST(group##Unbuffered, name)
#endif

#define RUN_PERFORMANCE_TESTS

namespace TestDefinitions
{
Json::OutStreamType & operator <<(Json::OutStreamType & os, const CustomizeFullySpecialized &)
{
    os << "Unspecialized!";
    return os;
}

struct IsSpecializedTest {};

struct IsUnspecializedTest : Json::Unspecialized {};

TEST_HEADER(JsonAnnotationsTest, IsSpecialized)
{
    EXPECT_TRUE(Json::is_specialized<IsSpecializedTest>::value);
    EXPECT_FALSE(Json::is_specialized<IsUnspecializedTest>::value);
}

TEST_HEADER(JsonSharedTest, MatchesStatics)
{
    bool matches = Json::matches_statics<false, Statics::Excluded>::value;
    EXPECT_TRUE(matches);
    matches = Json::matches_statics<false, Statics::Included>::value;
    EXPECT_TRUE(matches);
    matches = Json::matches_statics<false, Statics::Only>::value;
    EXPECT_FALSE(matches);
    matches = Json::matches_statics<true, Statics::Excluded>::value;
    EXPECT_FALSE(matches);
    matches = Json::matches_statics<true, Statics::Included>::value;
    EXPECT_TRUE(matches);
    matches = Json::matches_statics<true, Statics::Only>::value;
    EXPECT_TRUE(matches);
}

struct NoFields { REFLECT_EMPTY(() NoFields) };
struct InstanceField { int a; REFLECT(() InstanceField, () a) };
struct StaticField { static int a; REFLECT(() StaticField, () a) };
struct BothFields { int a; static int b; REFLECT(() BothFields, () a, () b) };
struct MoreInstance { int a; int b; static int c; REFLECT(() MoreInstance, () a, () b, () c) };
struct MoreStatic { int a; static int b; static int c; REFLECT(() MoreStatic, () a, () b, () c) };
int StaticField::a = 0;
int BothFields::b = 0;
int MoreInstance::c = 0;
int MoreStatic::b = 0;
int MoreStatic::c = 0;

struct IgnoreInstanceField {
    int a;
    REFLECT(() IgnoreInstanceField, (Json::Ignore) a)
};
struct IgnoreStaticField {
    static int a;
    REFLECT(() IgnoreStaticField, (Json::Ignore) a)
};
struct IgnoreBothField {
    int a;
    static int b;
    REFLECT(() IgnoreBothField, (Json::Ignore) a, (Json::Ignore) b)
};
struct IgnoreMixed {
    int a;
    int b;
    int c;
    static int d;
    static int e;
    static int f;
    REFLECT(() IgnoreMixed,
        (Json::Ignore) a,
        () b,
        (Json::Ignore) c,
        () d,
        () e,
        (Json::Ignore) f)
};
struct OnlyIgnoreStatics {
    int a;
    static int b;
    REFLECT(() OnlyIgnoreStatics, () a, (Json::Ignore) b)
};
struct OnlyIgnoreInstance {
    int a;
    static int b;
    REFLECT(() OnlyIgnoreInstance, (Json::Ignore) a, () b)
};
int IgnoreStaticField::a = 0;
int IgnoreBothField::b = 0;
int IgnoreMixed::d = 0;
int IgnoreMixed::e = 0;
int IgnoreMixed::f = 0;
int OnlyIgnoreStatics::b = 0;
int OnlyIgnoreInstance::b= 0;

TEST_HEADER(JsonSharedTest, IgnoredFieldCount)
{
    size_t ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, NoFields>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, InstanceField>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, StaticField>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, BothFields>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, MoreInstance>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, MoreStatic>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, IgnoreInstanceField>();
    EXPECT_EQ(1, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, IgnoreStaticField>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, IgnoreBothField>();
    EXPECT_EQ(1, ignoredFieldCount);
    
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Excluded, IgnoreMixed>();
    EXPECT_EQ(2, ignoredFieldCount);

    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, NoFields>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, InstanceField>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, StaticField>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, BothFields>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, MoreInstance>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, MoreStatic>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, IgnoreInstanceField>();
    EXPECT_EQ(1, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, IgnoreStaticField>();
    EXPECT_EQ(1, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, IgnoreBothField>();
    EXPECT_EQ(2, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Included, IgnoreMixed>();
    EXPECT_EQ(3, ignoredFieldCount);

    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, NoFields>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, InstanceField>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, StaticField>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, BothFields>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, MoreInstance>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, MoreStatic>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, IgnoreInstanceField>();
    EXPECT_EQ(0, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, IgnoreStaticField>();
    EXPECT_EQ(1, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, IgnoreBothField>();
    EXPECT_EQ(1, ignoredFieldCount);
    ignoredFieldCount = Json::IgnoredFieldCount<Statics::Only, IgnoreMixed>();
    EXPECT_EQ(1, ignoredFieldCount);
}

struct EmptySuper { REFLECT_EMPTY(() EmptySuper) };
struct OtherEmptySuper { REFLECT_EMPTY(() OtherEmptySuper) };
struct FieldedSuper { int a; REFLECT(() FieldedSuper, () a) };
struct NestedEmptySuper : EmptySuper { REFLECT_EMPTY((EmptySuper) NestedEmptySuper) };
struct NestedFieldedSuper : FieldedSuper { REFLECT_EMPTY((FieldedSuper) NestedFieldedSuper) };
struct DoublyNestedEmptySuper : NestedEmptySuper { REFLECT_EMPTY((NestedEmptySuper) DoublyNestedEmptySuper) };
struct DoublyNestedFieldedSuper : NestedFieldedSuper { REFLECT_EMPTY((NestedFieldedSuper) DoublyNestedFieldedSuper) };
struct DoubleSuper : EmptySuper, FieldedSuper {
    using TwoSupers = Inherit<EmptySuper, FieldedSuper>;
    REFLECT_EMPTY((TwoSupers) DoubleSuper)
};
struct TripleSuper : EmptySuper, OtherEmptySuper, FieldedSuper {
    using ThreeSupers = Inherit<EmptySuper, OtherEmptySuper, FieldedSuper>;
    REFLECT_EMPTY((ThreeSupers) TripleSuper)
};

TEST_HEADER(JsonSharedTest, HasFields)
{
    bool hasFields = Json::HasFields<Statics::Excluded, NoFields>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, InstanceField>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, StaticField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, BothFields>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, MoreInstance>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, MoreStatic>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, IgnoreInstanceField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, IgnoreStaticField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, IgnoreBothField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, IgnoreMixed>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, OnlyIgnoreStatics>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Excluded, OnlyIgnoreInstance>();
    EXPECT_FALSE(hasFields);

    hasFields = Json::HasFields<Statics::Included, NoFields>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Included, InstanceField>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, StaticField>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, BothFields>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, MoreInstance>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, MoreStatic>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, IgnoreInstanceField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Included, IgnoreStaticField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Included, IgnoreBothField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Included, IgnoreMixed>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, OnlyIgnoreStatics>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, OnlyIgnoreInstance>();
    EXPECT_TRUE(hasFields);

    hasFields = Json::HasFields<Statics::Only, NoFields>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Only, InstanceField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Only, StaticField>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Only, BothFields>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Only, MoreInstance>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Only, MoreStatic>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Only, IgnoreInstanceField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Only, IgnoreStaticField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Only, IgnoreBothField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Only, IgnoreMixed>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Only, OnlyIgnoreStatics>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Only, OnlyIgnoreInstance>();
    EXPECT_TRUE(hasFields);
    
    hasFields = Json::HasFields<Statics::Included, EmptySuper>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Included, FieldedSuper>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, NestedEmptySuper>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Included, NestedFieldedSuper>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::HasFields<Statics::Included, DoublyNestedEmptySuper>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::HasFields<Statics::Included, DoublyNestedFieldedSuper>();
}

TEST_HEADER(JsonSharedTest, FirstIndex)
{
    size_t firstIndex = Json::FirstIndex<Statics::Excluded, NoFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, InstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, StaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, BothFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, MoreInstance>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, MoreStatic>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, IgnoreInstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, IgnoreStaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, IgnoreBothField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, IgnoreMixed>();
    EXPECT_EQ(1, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, OnlyIgnoreStatics>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Excluded, OnlyIgnoreInstance>();
    EXPECT_EQ(0, firstIndex);

    firstIndex = Json::FirstIndex<Statics::Included, NoFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, InstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, StaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, BothFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, MoreInstance>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, MoreStatic>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, IgnoreInstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, IgnoreStaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, IgnoreBothField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, IgnoreMixed>();
    EXPECT_EQ(1, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, OnlyIgnoreStatics>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Included, OnlyIgnoreInstance>();
    EXPECT_EQ(1, firstIndex);

    firstIndex = Json::FirstIndex<Statics::Only, NoFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, InstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, StaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, BothFields>();
    EXPECT_EQ(1, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, MoreInstance>();
    EXPECT_EQ(2, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, MoreStatic>();
    EXPECT_EQ(1, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, IgnoreInstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, IgnoreStaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, IgnoreBothField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, IgnoreMixed>();
    EXPECT_EQ(3, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, OnlyIgnoreStatics>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::FirstIndex<Statics::Only, OnlyIgnoreInstance>();
    EXPECT_EQ(1, firstIndex);
}

TEST_HEADER(JsonSharedTest, FirstSuperIndex)
{
    size_t firstSuperIndex = Json::FirstSuperIndex<Json::Statics::Included, EmptySuper>();
    EXPECT_EQ(0, firstSuperIndex);
    firstSuperIndex = Json::FirstSuperIndex<Json::Statics::Included, DoubleSuper>();
    EXPECT_EQ(1, firstSuperIndex);
    firstSuperIndex = Json::FirstSuperIndex<Json::Statics::Included, TripleSuper>();
    EXPECT_EQ(2, firstSuperIndex);
}

TEST_HEADER(JsonSharedTest, Exception)
{
    const char test[] = "test";
    
    try {
        throw Json::Exception(test);
        EXPECT_TRUE(false);
    } catch ( Json::Exception & e ) {
        EXPECT_STREQ(test, e.what());
    }
}

TEST_HEADER(JsonSharedTest, NullUnassignable)
{
    bool visited = false;
    try {
        throw Json::NullUnassignable();
    } catch ( Json::NullUnassignable & ) {
        visited = true;
    }
    EXPECT_TRUE(visited);

    visited = false;
    try {
        throw Json::NullUnassignable();
    } catch ( Json::Exception & ) {
        visited = true;
    }
    EXPECT_TRUE(visited);
}

TEST_HEADER(JsonSharedTest, SimplifyTypeStr)
{
    EXPECT_STREQ("MyStruct", Json::simplifyTypeStr("struct My struct").c_str());
    EXPECT_STREQ("MyClass", Json::simplifyTypeStr("class My class").c_str());
}

struct MyStruct {};
class MyClass {};

TEST_HEADER(JsonSharedTest, SuperTypeToJsonFieldName)
{
    std::string myStructStr = Json::superTypeToJsonFieldName<MyStruct>();
    std::string myClassStr = Json::superTypeToJsonFieldName<MyClass>();

    size_t lastStructScope = myStructStr.find_last_of(':');
    size_t lastClassScope = myClassStr.find_last_of(':');
    
    EXPECT_STREQ("__", myStructStr.substr(0, 2).c_str());
    EXPECT_STREQ("__", myClassStr.substr(0, 2).c_str());
    EXPECT_STREQ(lastStructScope == std::string::npos ? "__MyStruct" : "MyStruct",
        lastStructScope == std::string::npos ? myStructStr.c_str() : myStructStr.substr(lastStructScope+1).c_str());
    EXPECT_STREQ(lastClassScope == std::string::npos ? "__MyClass" : "MyClass",
        lastClassScope == std::string::npos ? myClassStr.c_str() : myClassStr.substr(lastClassScope+1).c_str());
}

TEST_HEADER(JsonSharedTest, FieldClusterToJsonFieldName)
{
    EXPECT_STREQ("____fieldCluster", Json::fieldClusterToJsonFieldName().c_str());
}

TEST_HEADER(JsonGenericTest, JsonField)
{
    Json::JsonField ctor = Json::JsonField();
    EXPECT_EQ(0, ctor.index);
    EXPECT_EQ(Json::JsonField::Type::Regular, ctor.type);
    EXPECT_STREQ("", ctor.name.c_str());

    size_t fieldIndex = 1337;
    Json::JsonField::Type type = Json::JsonField::Type::SuperClass;
    std::string fieldName = "asdf";

    Json::JsonField other = Json::JsonField(fieldIndex, type, fieldName);
    EXPECT_EQ(fieldIndex, other.index);
    EXPECT_EQ(type, other.type);
    EXPECT_STREQ(fieldName.c_str(), other.name.c_str());
}

TEST_HEADER(JsonGenericTest, TypeMismatch)
{
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::None).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::Boolean).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::Number).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::String).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::Object).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::Array).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::BoolArray).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::NumberArray).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::StringArray).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::ObjectArray).empty());
    EXPECT_FALSE(Json::Value::TypeMismatch::getTypeStr(Json::Value::Type::MixedArray).empty());

    Json::Value::TypeMismatch functionTypeMismatch(Json::Value::Type::NullArray, Json::Value::Type::Number, "nullArray");
    EXPECT_EQ(Json::Value::Type::NullArray, functionTypeMismatch.valueType);
    EXPECT_EQ(Json::Value::Type::None, functionTypeMismatch.rValueType);
    EXPECT_EQ(Json::Value::Type::Number, functionTypeMismatch.functionType);

    Json::Value::TypeMismatch assignTypeMismatch(Json::Value::Type::BoolArray, Json::Value::Type::Boolean);
    EXPECT_EQ(Json::Value::Type::BoolArray, assignTypeMismatch.valueType);
    EXPECT_EQ(Json::Value::Type::Boolean, assignTypeMismatch.rValueType);
    EXPECT_EQ(Json::Value::Type::None, assignTypeMismatch.functionType);
}

TEST_HEADER(JsonGenericTest, Bool)
{
    Json::Bool ctor;
    EXPECT_FALSE(ctor.boolean());

    Json::Bool valueConstruct(true);
    EXPECT_TRUE(valueConstruct.boolean());

    Json::Bool copyConstruct(valueConstruct);
    EXPECT_TRUE(copyConstruct.boolean());

    auto ctorMake = Json::Bool::Make();
    EXPECT_FALSE(ctorMake->boolean());

    auto valueMake = Json::Bool::Make(true);
    EXPECT_TRUE(valueMake->boolean());

    auto copyMake = Json::Bool::Make(valueConstruct);
    EXPECT_TRUE(copyMake->boolean());

    Json::Bool other(false);
    EXPECT_FALSE(other.boolean());
    other = valueConstruct;
    EXPECT_TRUE(other.boolean());

    EXPECT_EQ(Json::Value::Type::Boolean, ctor.type());
    EXPECT_THROW(ctor.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::Value::TypeMismatch);

    const Json::Bool & constCtor = ctor;

    EXPECT_EQ(false, constCtor.boolean());
    EXPECT_THROW(constCtor.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.arraySize(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constCtor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, Number)
{
    Json::Number ctor;
    EXPECT_STREQ("0", ctor.number().c_str());

    std::string numericValue("9000.1337");
    Json::Number valueConstruct(numericValue);
    EXPECT_STREQ(numericValue.c_str(), valueConstruct.number().c_str());

    Json::Number copyConstruct(valueConstruct);
    EXPECT_STREQ(numericValue.c_str(), copyConstruct.number().c_str());

    auto ctorMake = Json::Number::Make();
    EXPECT_STREQ("0", ctorMake->number().c_str());

    auto constCharMake = Json::Number::Make("66.7");
    EXPECT_STREQ("66.7", constCharMake->number().c_str());

    auto valueMake = Json::Number::Make(numericValue);
    EXPECT_STREQ(numericValue.c_str(), valueMake->number().c_str());

    auto copyMake = Json::Number::Make(valueConstruct);
    EXPECT_STREQ(numericValue.c_str(), copyMake->number().c_str());

    short number = 553;
    auto numberCastMake = Json::Number::Make(number);
    EXPECT_STREQ(std::to_string(number).c_str(), numberCastMake->number().c_str());

    Json::Number other = valueConstruct;
    EXPECT_STREQ(numericValue.c_str(), other.number().c_str());

    EXPECT_EQ(Json::Value::Type::Number, ctor.type());
    
    EXPECT_THROW(ctor.boolean(), Json::Value::TypeMismatch);
    EXPECT_STREQ("0", ctor.number().c_str());
    EXPECT_THROW(ctor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::Value::TypeMismatch);

    const Json::Number & constCtor = ctor;
    
    EXPECT_THROW(constCtor.boolean(), Json::Value::TypeMismatch);
    EXPECT_STREQ("0", constCtor.number().c_str());
    EXPECT_THROW(constCtor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.arraySize(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constCtor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, String)
{
    Json::String ctor;
    EXPECT_STREQ("", ctor.string().c_str());

    std::string stringValue("asdf jkl");
    Json::String valueConstruct(stringValue);
    EXPECT_STREQ(stringValue.c_str(), valueConstruct.string().c_str());

    Json::String copyConstruct(valueConstruct);
    EXPECT_STREQ(stringValue.c_str(), copyConstruct.string().c_str());

    auto ctorMake = Json::String::Make();
    EXPECT_STREQ("", ctorMake->string().c_str());

    auto valueMake = Json::String::Make(stringValue);
    EXPECT_STREQ(stringValue.c_str(), valueMake->string().c_str());

    auto copyMake = Json::String::Make(valueConstruct);
    EXPECT_STREQ(stringValue.c_str(), copyMake->string().c_str());

    Json::String other = valueConstruct;
    EXPECT_STREQ(stringValue.c_str(), other.string().c_str());

    EXPECT_EQ(Json::Value::Type::String, ctor.type());
    
    EXPECT_THROW(ctor.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.number(), Json::Value::TypeMismatch);
    EXPECT_STREQ("", ctor.string().c_str());
    EXPECT_THROW(ctor.object(), Json::Value::TypeMismatch);

    const Json::String & constCtor = ctor;
    
    EXPECT_THROW(constCtor.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.number(), Json::Value::TypeMismatch);
    EXPECT_STREQ("", constCtor.string().c_str());
    EXPECT_THROW(constCtor.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.arraySize(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constCtor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constCtor.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, Object)
{
    Json::Object obj;
    EXPECT_TRUE(obj.object().empty());

    std::map<std::string, std::shared_ptr<Json::Value>> value;
    value.insert(std::pair("first", Json::Bool::Make(false)));
    value.insert(std::pair("second", Json::String::Make("asdf")));
    Json::Object objectValue(value);
    auto first = objectValue.object().find("first");
    auto second = objectValue.object().find("second");
    EXPECT_STREQ("first", first->first.c_str());
    EXPECT_STREQ("second", second->first.c_str());
    EXPECT_FALSE(first->second->boolean());
    EXPECT_STREQ("asdf", second->second->string().c_str());

    auto ctorMake = Json::Object::Make();
    EXPECT_TRUE(ctorMake->object().empty());

    auto valueMake = Json::Object::Make(value);
    first = valueMake->object().find("first");
    second = valueMake->object().find("second");
    EXPECT_STREQ("first", first->first.c_str());
    EXPECT_STREQ("second", second->first.c_str());
    EXPECT_FALSE(first->second->boolean());
    EXPECT_STREQ("asdf", second->second->string().c_str());

    obj = value;
    first = obj.object().find("first");
    second = obj.object().find("second");
    EXPECT_STREQ("first", first->first.c_str());
    EXPECT_STREQ("second", second->first.c_str());
    EXPECT_FALSE(first->second->boolean());
    EXPECT_STREQ("asdf", second->second->string().c_str());

    EXPECT_EQ(Json::Value::Type::Object, obj.type());
    
    EXPECT_THROW(obj.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(obj.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(obj.string(), Json::Value::TypeMismatch);

    const Json::Object & constObj = obj;
    
    EXPECT_THROW(constObj.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObj.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObj.string(), Json::Value::TypeMismatch);

    EXPECT_THROW(obj.arraySize(), Json::Value::TypeMismatch);

    EXPECT_THROW(obj.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(obj.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(obj.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(obj.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(obj.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(obj.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constObj.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObj.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObj.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObj.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObj.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObj.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, NullArray)
{
    Json::NullArray nullArray;
    EXPECT_EQ(0, nullArray.arraySize());
    EXPECT_EQ(0, nullArray.nullArray());

    nullArray.nullArray()++;
    EXPECT_EQ(1, nullArray.arraySize());
    EXPECT_EQ(1, nullArray.nullArray());
    nullArray.nullArray()++;
    EXPECT_EQ(2, nullArray.arraySize());
    EXPECT_EQ(2, nullArray.nullArray());

    Json::NullArray valueConstruct(5);
    EXPECT_EQ(5, valueConstruct.arraySize());
    EXPECT_EQ(5, valueConstruct.nullArray());

    Json::NullArray copyConstruct(nullArray);
    EXPECT_EQ(2, copyConstruct.arraySize());
    EXPECT_EQ(2, copyConstruct.nullArray());

    auto ctorMake = Json::NullArray::Make();
    EXPECT_EQ(0, ctorMake->arraySize());
    EXPECT_EQ(0, ctorMake->nullArray());

    auto valueMake = Json::NullArray::Make(5);
    EXPECT_EQ(5, valueMake->arraySize());
    EXPECT_EQ(5, valueMake->nullArray());

    auto copyMake = Json::NullArray::Make(nullArray);
    EXPECT_EQ(2, copyMake->arraySize());
    EXPECT_EQ(2, copyMake->nullArray());

    nullArray = valueConstruct;
    EXPECT_EQ(5, nullArray.arraySize());
    EXPECT_EQ(5, nullArray.nullArray());

    EXPECT_EQ(Json::Value::Type::NullArray, nullArray.type());
    
    EXPECT_THROW(nullArray.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(nullArray.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(nullArray.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(nullArray.object(), Json::Value::TypeMismatch);

    const Json::NullArray & constNullArray = nullArray;
    
    EXPECT_THROW(constNullArray.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constNullArray.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constNullArray.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(constNullArray.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(nullArray.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(nullArray.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(nullArray.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(nullArray.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(nullArray.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constNullArray.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constNullArray.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constNullArray.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constNullArray.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constNullArray.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, BoolArray)
{
    Json::BoolArray ctor;
    EXPECT_EQ(0, ctor.arraySize());
    EXPECT_TRUE(ctor.boolArray().empty());

    std::vector<bool> values;
    values.push_back(true);
    values.push_back(true);
    values.push_back(false);
    Json::BoolArray valueConstruct(values);
    EXPECT_EQ(3, valueConstruct.arraySize());
    EXPECT_EQ(true, valueConstruct.boolArray()[0]);
    EXPECT_EQ(true, valueConstruct.boolArray()[1]);
    EXPECT_EQ(false, valueConstruct.boolArray()[2]);

    Json::BoolArray copyConstruct(values);
    EXPECT_EQ(3, copyConstruct.arraySize());
    EXPECT_EQ(true, copyConstruct.boolArray()[0]);
    EXPECT_EQ(true, copyConstruct.boolArray()[1]);
    EXPECT_EQ(false, copyConstruct.boolArray()[2]);

    auto ctorMake = Json::BoolArray::Make();
    EXPECT_EQ(0, ctorMake->arraySize());
    EXPECT_TRUE(ctorMake->boolArray().empty());

    auto valueMake = Json::BoolArray::Make(values);
    EXPECT_EQ(3, valueMake->arraySize());
    EXPECT_EQ(true, valueMake->boolArray()[0]);
    EXPECT_EQ(true, valueMake->boolArray()[1]);
    EXPECT_EQ(false, valueMake->boolArray()[2]);

    auto copyMake = Json::BoolArray::Make(valueConstruct);
    EXPECT_EQ(3, copyMake->arraySize());
    EXPECT_EQ(true, copyMake->boolArray()[0]);
    EXPECT_EQ(true, copyMake->boolArray()[1]);
    EXPECT_EQ(false, copyMake->boolArray()[2]);

    Json::BoolArray other;
    other = valueConstruct;
    EXPECT_EQ(3, other.arraySize());
    EXPECT_EQ(true, other.boolArray()[0]);
    EXPECT_EQ(true, other.boolArray()[1]);
    EXPECT_EQ(false, other.boolArray()[2]);

    EXPECT_EQ(Json::Value::Type::BoolArray, ctor.type());
    
    EXPECT_THROW(ctor.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::Value::TypeMismatch);

    const Json::BoolArray & constBoolArray = ctor;
    
    EXPECT_THROW(constBoolArray.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constBoolArray.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, NumberArray)
{
    Json::NumberArray ctor;
    EXPECT_TRUE(ctor.numberArray().empty());
    EXPECT_EQ(0, ctor.arraySize());

    std::vector<std::string> values;
    values.push_back("0");
    values.push_back("1");
    values.push_back("45");
    Json::NumberArray valueConstruct(values);
    EXPECT_EQ(3, valueConstruct.arraySize());
    EXPECT_STREQ("0", valueConstruct.numberArray()[0].c_str());
    EXPECT_STREQ("1", valueConstruct.numberArray()[1].c_str());
    EXPECT_STREQ("45", valueConstruct.numberArray()[2].c_str());

    Json::NumberArray copyConstruct(valueConstruct);
    EXPECT_EQ(3, copyConstruct.arraySize());
    EXPECT_STREQ("0", copyConstruct.numberArray()[0].c_str());
    EXPECT_STREQ("1", copyConstruct.numberArray()[1].c_str());
    EXPECT_STREQ("45", copyConstruct.numberArray()[2].c_str());

    auto ctorMake = Json::NumberArray::Make();
    EXPECT_TRUE(ctorMake->numberArray().empty());
    EXPECT_EQ(0, ctorMake->arraySize());

    auto valueMake = Json::NumberArray::Make(values);
    EXPECT_EQ(3, valueMake->arraySize());
    EXPECT_STREQ("0", valueMake->numberArray()[0].c_str());
    EXPECT_STREQ("1", valueMake->numberArray()[1].c_str());
    EXPECT_STREQ("45", valueMake->numberArray()[2].c_str());

    auto copyMake = Json::NumberArray::Make(values);
    EXPECT_EQ(3, copyMake->arraySize());
    EXPECT_STREQ("0", copyMake->numberArray()[0].c_str());
    EXPECT_STREQ("1", copyMake->numberArray()[1].c_str());
    EXPECT_STREQ("45", copyMake->numberArray()[2].c_str());

    Json::NumberArray other = values;
    EXPECT_EQ(3, other.arraySize());
    EXPECT_STREQ("0", other.numberArray()[0].c_str());
    EXPECT_STREQ("1", other.numberArray()[1].c_str());
    EXPECT_STREQ("45", other.numberArray()[2].c_str());

    EXPECT_EQ(Json::Value::Type::NumberArray, ctor.type());

    EXPECT_THROW(ctor.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::Value::TypeMismatch);

    const Json::NumberArray & constBoolArray = ctor;
    
    EXPECT_THROW(constBoolArray.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constBoolArray.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constBoolArray.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, StringArray)
{
    Json::StringArray ctor;
    EXPECT_EQ(0, ctor.arraySize());
    EXPECT_TRUE(ctor.stringArray().empty());

    std::vector<std::string> values;
    values.push_back("asdf");
    values.push_back("some string");
    values.push_back("1234");
    Json::StringArray valueConstruct(values);
    EXPECT_EQ(3, valueConstruct.arraySize());
    EXPECT_STREQ("asdf", valueConstruct.stringArray()[0].c_str());
    EXPECT_STREQ("some string", valueConstruct.stringArray()[1].c_str());
    EXPECT_STREQ("1234", valueConstruct.stringArray()[2].c_str());

    Json::StringArray copyConstruct(valueConstruct);
    EXPECT_EQ(3, copyConstruct.arraySize());
    EXPECT_STREQ("asdf", copyConstruct.stringArray()[0].c_str());
    EXPECT_STREQ("some string", copyConstruct.stringArray()[1].c_str());
    EXPECT_STREQ("1234", copyConstruct.stringArray()[2].c_str());

    auto ctorMake = Json::StringArray::Make();
    EXPECT_EQ(0, ctorMake->arraySize());
    EXPECT_TRUE(ctorMake->stringArray().empty());

    auto valueMake = Json::StringArray::Make(values);
    EXPECT_EQ(3, valueMake->arraySize());
    EXPECT_STREQ("asdf", valueMake->stringArray()[0].c_str());
    EXPECT_STREQ("some string", valueMake->stringArray()[1].c_str());
    EXPECT_STREQ("1234", valueMake->stringArray()[2].c_str());

    auto copyMake = Json::StringArray::Make(valueConstruct);
    EXPECT_EQ(3, copyMake->arraySize());
    EXPECT_STREQ("asdf", copyMake->stringArray()[0].c_str());
    EXPECT_STREQ("some string", copyMake->stringArray()[1].c_str());
    EXPECT_STREQ("1234", copyMake->stringArray()[2].c_str());

    Json::StringArray other = valueConstruct;
    EXPECT_EQ(Json::Value::Type::StringArray, other.type());
    
    EXPECT_THROW(ctor.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::Value::TypeMismatch);

    const Json::StringArray & constStringArray = ctor;
    
    EXPECT_THROW(constStringArray.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constStringArray.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constStringArray.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(constStringArray.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constStringArray.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constStringArray.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constStringArray.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constStringArray.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constStringArray.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, ObjectArray)
{
    Json::ObjectArray ctor;
    EXPECT_EQ(0, ctor.arraySize());
    EXPECT_TRUE(ctor.objectArray().empty());

    std::vector<std::map<std::string, std::shared_ptr<Json::Value>>> objects;
    std::map<std::string, std::shared_ptr<Json::Value>> firstObj;
    std::map<std::string, std::shared_ptr<Json::Value>> secondObj;
    std::map<std::string, std::shared_ptr<Json::Value>> thirdObj;
    firstObj.insert(std::pair("firstOne", Json::Bool::Make(false)));
    firstObj.insert(std::pair("firstTwo", Json::Number::Make("1234")));
    firstObj.insert(std::pair("firstThree", Json::String::Make("asdf")));
    thirdObj.insert(std::pair("thirdThree", Json::Object::Make()));
    objects.push_back(firstObj);
    objects.push_back(secondObj);
    objects.push_back(thirdObj);
    
    Json::ObjectArray valueConstruct(objects);
    EXPECT_EQ(3, valueConstruct.arraySize());
    EXPECT_EQ(3, valueConstruct.objectArray().size());
    auto first = valueConstruct.objectArray()[0];
    EXPECT_STREQ("firstOne", first.find("firstOne")->first.c_str());
    EXPECT_STREQ("firstTwo", first.find("firstTwo")->first.c_str());
    EXPECT_STREQ("firstThree", first.find("firstThree")->first.c_str());
    EXPECT_EQ(false, first.find("firstOne")->second->boolean());
    EXPECT_STREQ("1234", first.find("firstTwo")->second->number().c_str());
    EXPECT_STREQ("asdf", first.find("firstThree")->second->string().c_str());
    auto second = valueConstruct.objectArray()[1];
    EXPECT_TRUE(second.empty());
    auto third = valueConstruct.objectArray()[2];
    EXPECT_EQ(Json::Value::Type::Object, third.find("thirdThree")->second->type());

    Json::ObjectArray copyConstruct(valueConstruct);
    EXPECT_EQ(3, copyConstruct.arraySize());
    EXPECT_EQ(3, copyConstruct.objectArray().size());
    first = copyConstruct.objectArray()[0];
    EXPECT_STREQ("firstOne", first.find("firstOne")->first.c_str());
    EXPECT_STREQ("firstTwo", first.find("firstTwo")->first.c_str());
    EXPECT_STREQ("firstThree", first.find("firstThree")->first.c_str());
    EXPECT_EQ(false, first.find("firstOne")->second->boolean());
    EXPECT_STREQ("1234", first.find("firstTwo")->second->number().c_str());
    EXPECT_STREQ("asdf", first.find("firstThree")->second->string().c_str());
    second = copyConstruct.objectArray()[1];
    EXPECT_TRUE(second.empty());
    third = copyConstruct.objectArray()[2];
    EXPECT_EQ(Json::Value::Type::Object, third.find("thirdThree")->second->type());

    auto ctorMake = Json::ObjectArray::Make();
    EXPECT_EQ(0, ctorMake->arraySize());
    EXPECT_TRUE(ctorMake->objectArray().empty());

    auto valueMake = Json::ObjectArray::Make(objects);
    EXPECT_EQ(3, valueMake->arraySize());
    EXPECT_EQ(3, valueMake->objectArray().size());
    first = valueMake->objectArray()[0];
    EXPECT_STREQ("firstOne", first.find("firstOne")->first.c_str());
    EXPECT_STREQ("firstTwo", first.find("firstTwo")->first.c_str());
    EXPECT_STREQ("firstThree", first.find("firstThree")->first.c_str());
    EXPECT_EQ(false, first.find("firstOne")->second->boolean());
    EXPECT_STREQ("1234", first.find("firstTwo")->second->number().c_str());
    EXPECT_STREQ("asdf", first.find("firstThree")->second->string().c_str());
    second = valueMake->objectArray()[1];
    EXPECT_TRUE(second.empty());
    third = valueMake->objectArray()[2];
    EXPECT_EQ(Json::Value::Type::Object, third.find("thirdThree")->second->type());

    auto copyMake = Json::ObjectArray::Make(valueConstruct);
    EXPECT_EQ(3, copyMake->arraySize());
    EXPECT_EQ(3, copyMake->objectArray().size());
    first = copyMake->objectArray()[0];
    EXPECT_STREQ("firstOne", first.find("firstOne")->first.c_str());
    EXPECT_STREQ("firstTwo", first.find("firstTwo")->first.c_str());
    EXPECT_STREQ("firstThree", first.find("firstThree")->first.c_str());
    EXPECT_EQ(false, first.find("firstOne")->second->boolean());
    EXPECT_STREQ("1234", first.find("firstTwo")->second->number().c_str());
    EXPECT_STREQ("asdf", first.find("firstThree")->second->string().c_str());
    second = copyMake->objectArray()[1];
    EXPECT_TRUE(second.empty());
    third = copyMake->objectArray()[2];
    EXPECT_EQ(Json::Value::Type::Object, third.find("thirdThree")->second->type());

    Json::ObjectArray other = valueConstruct;
    EXPECT_EQ(3, other.arraySize());
    EXPECT_EQ(3, other.objectArray().size());
    first = other.objectArray()[0];
    EXPECT_STREQ("firstOne", first.find("firstOne")->first.c_str());
    EXPECT_STREQ("firstTwo", first.find("firstTwo")->first.c_str());
    EXPECT_STREQ("firstThree", first.find("firstThree")->first.c_str());
    EXPECT_EQ(false, first.find("firstOne")->second->boolean());
    EXPECT_STREQ("1234", first.find("firstTwo")->second->number().c_str());
    EXPECT_STREQ("asdf", first.find("firstThree")->second->string().c_str());
    second = other.objectArray()[1];
    EXPECT_TRUE(second.empty());
    third = other.objectArray()[2];
    EXPECT_EQ(Json::Value::Type::Object, third.find("thirdThree")->second->type());

    EXPECT_EQ(Json::Value::Type::ObjectArray, ctor.type());

    EXPECT_THROW(ctor.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::Value::TypeMismatch);

    const Json::ObjectArray & constObjectArray = ctor;
    
    EXPECT_THROW(constObjectArray.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObjectArray.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObjectArray.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObjectArray.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constObjectArray.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObjectArray.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObjectArray.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObjectArray.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constObjectArray.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, MixedArray)
{
    Json::MixedArray ctor;
    EXPECT_EQ(0, ctor.arraySize());
    EXPECT_TRUE(ctor.mixedArray().empty());

    std::vector<std::shared_ptr<Json::Value>> nestedMixedArrayValues = { nullptr, Json::Number::Make("1234") };
    std::shared_ptr<Json::Value> nestedMixedArrayValue = std::shared_ptr<Json::Value>(new Json::MixedArray(nestedMixedArrayValues));

    std::vector<bool> nestedBoolArrayValues = { true, false, true };
    std::shared_ptr<Json::Value> nestedBoolArrayValue = std::shared_ptr<Json::Value>(new Json::BoolArray(nestedBoolArrayValues));

    std::shared_ptr<Json::Object> objValue = std::shared_ptr<Json::Object>(new Json::Object());
    objValue->put("some", Json::String::Make("field"));
    objValue->put("someother", Json::String::Make("field"));
    
    std::vector<std::shared_ptr<Json::Value>> values = { nestedMixedArrayValue, nestedBoolArrayValue, objValue };
    Json::MixedArray valueConstruct(values);
    EXPECT_EQ(3, valueConstruct.arraySize());
    EXPECT_EQ(3, valueConstruct.mixedArray().size());
    auto nestedMixedArray = valueConstruct.mixedArray()[0];
    auto nestedBoolArray = valueConstruct.mixedArray()[1];
    auto nestedObj = valueConstruct.mixedArray()[2];
    EXPECT_EQ(Json::Value::Type::MixedArray, nestedMixedArray->type());
    EXPECT_EQ(Json::Value::Type::BoolArray, nestedBoolArray->type());
    EXPECT_EQ(Json::Value::Type::Object, nestedObj->type());
    auto nestedNestedNullptr = nestedMixedArray->mixedArray()[0];
    auto nestedNestedNumber = nestedMixedArray->mixedArray()[1];
    EXPECT_TRUE(nullptr == nestedNestedNullptr);
    EXPECT_STREQ("1234", nestedNestedNumber->number().c_str());
    EXPECT_TRUE(nestedBoolArray->boolArray()[0]);
    EXPECT_FALSE(nestedBoolArray->boolArray()[1]);
    EXPECT_TRUE(nestedBoolArray->boolArray()[2]);
    EXPECT_STREQ("some", nestedObj->object().find("some")->first.c_str());
    EXPECT_STREQ("someother", nestedObj->object().find("someother")->first.c_str());
    EXPECT_STREQ("field", nestedObj->object().find("some")->second->string().c_str());
    EXPECT_STREQ("field", nestedObj->object().find("someother")->second->string().c_str());
    
    Json::MixedArray copyConstruct(valueConstruct);
    EXPECT_EQ(3, copyConstruct.arraySize());
    EXPECT_EQ(3, copyConstruct.mixedArray().size());
    nestedMixedArray = copyConstruct.mixedArray()[0];
    nestedBoolArray = copyConstruct.mixedArray()[1];
    nestedObj = copyConstruct.mixedArray()[2];
    EXPECT_EQ(Json::Value::Type::MixedArray, nestedMixedArray->type());
    EXPECT_EQ(Json::Value::Type::BoolArray, nestedBoolArray->type());
    EXPECT_EQ(Json::Value::Type::Object, nestedObj->type());
    nestedNestedNullptr = nestedMixedArray->mixedArray()[0];
    nestedNestedNumber = nestedMixedArray->mixedArray()[1];
    EXPECT_TRUE(nullptr == nestedNestedNullptr);
    EXPECT_STREQ("1234", nestedNestedNumber->number().c_str());
    EXPECT_TRUE(nestedBoolArray->boolArray()[0]);
    EXPECT_FALSE(nestedBoolArray->boolArray()[1]);
    EXPECT_TRUE(nestedBoolArray->boolArray()[2]);
    EXPECT_STREQ("some", nestedObj->object().find("some")->first.c_str());
    EXPECT_STREQ("someother", nestedObj->object().find("someother")->first.c_str());
    EXPECT_STREQ("field", nestedObj->object().find("some")->second->string().c_str());
    EXPECT_STREQ("field", nestedObj->object().find("someother")->second->string().c_str());

    auto ctorMake = Json::MixedArray::Make();
    EXPECT_EQ(0, ctorMake->arraySize());
    EXPECT_TRUE(ctorMake->mixedArray().empty());
    
    auto valueMake = Json::MixedArray::Make(values);
    EXPECT_EQ(3, valueMake->arraySize());
    EXPECT_EQ(3, valueMake->mixedArray().size());
    nestedMixedArray = valueMake->mixedArray()[0];
    nestedBoolArray = valueMake->mixedArray()[1];
    nestedObj = valueMake->mixedArray()[2];
    EXPECT_EQ(Json::Value::Type::MixedArray, nestedMixedArray->type());
    EXPECT_EQ(Json::Value::Type::BoolArray, nestedBoolArray->type());
    EXPECT_EQ(Json::Value::Type::Object, nestedObj->type());
    nestedNestedNullptr = nestedMixedArray->mixedArray()[0];
    nestedNestedNumber = nestedMixedArray->mixedArray()[1];
    EXPECT_TRUE(nullptr == nestedNestedNullptr);
    EXPECT_STREQ("1234", nestedNestedNumber->number().c_str());
    EXPECT_TRUE(nestedBoolArray->boolArray()[0]);
    EXPECT_FALSE(nestedBoolArray->boolArray()[1]);
    EXPECT_TRUE(nestedBoolArray->boolArray()[2]);
    EXPECT_STREQ("some", nestedObj->object().find("some")->first.c_str());
    EXPECT_STREQ("someother", nestedObj->object().find("someother")->first.c_str());
    EXPECT_STREQ("field", nestedObj->object().find("some")->second->string().c_str());
    EXPECT_STREQ("field", nestedObj->object().find("someother")->second->string().c_str());

    auto copyMake = Json::MixedArray::Make(valueConstruct);
    EXPECT_EQ(3, copyMake->arraySize());
    EXPECT_EQ(3, copyMake->mixedArray().size());
    nestedMixedArray = copyMake->mixedArray()[0];
    nestedBoolArray = copyMake->mixedArray()[1];
    nestedObj = copyMake->mixedArray()[2];
    EXPECT_EQ(Json::Value::Type::MixedArray, nestedMixedArray->type());
    EXPECT_EQ(Json::Value::Type::BoolArray, nestedBoolArray->type());
    EXPECT_EQ(Json::Value::Type::Object, nestedObj->type());
    nestedNestedNullptr = nestedMixedArray->mixedArray()[0];
    nestedNestedNumber = nestedMixedArray->mixedArray()[1];
    EXPECT_TRUE(nullptr == nestedNestedNullptr);
    EXPECT_STREQ("1234", nestedNestedNumber->number().c_str());
    EXPECT_TRUE(nestedBoolArray->boolArray()[0]);
    EXPECT_FALSE(nestedBoolArray->boolArray()[1]);
    EXPECT_TRUE(nestedBoolArray->boolArray()[2]);
    EXPECT_STREQ("some", nestedObj->object().find("some")->first.c_str());
    EXPECT_STREQ("someother", nestedObj->object().find("someother")->first.c_str());
    EXPECT_STREQ("field", nestedObj->object().find("some")->second->string().c_str());
    EXPECT_STREQ("field", nestedObj->object().find("someother")->second->string().c_str());

    Json::MixedArray other = valueConstruct;
    EXPECT_EQ(3, other.arraySize());
    EXPECT_EQ(3, other.mixedArray().size());
    nestedMixedArray = other.mixedArray()[0];
    nestedBoolArray = other.mixedArray()[1];
    nestedObj = other.mixedArray()[2];
    EXPECT_EQ(Json::Value::Type::MixedArray, nestedMixedArray->type());
    EXPECT_EQ(Json::Value::Type::BoolArray, nestedBoolArray->type());
    EXPECT_EQ(Json::Value::Type::Object, nestedObj->type());
    nestedNestedNullptr = nestedMixedArray->mixedArray()[0];
    nestedNestedNumber = nestedMixedArray->mixedArray()[1];
    EXPECT_TRUE(nullptr == nestedNestedNullptr);
    EXPECT_STREQ("1234", nestedNestedNumber->number().c_str());
    EXPECT_TRUE(nestedBoolArray->boolArray()[0]);
    EXPECT_FALSE(nestedBoolArray->boolArray()[1]);
    EXPECT_TRUE(nestedBoolArray->boolArray()[2]);
    EXPECT_STREQ("some", nestedObj->object().find("some")->first.c_str());
    EXPECT_STREQ("someother", nestedObj->object().find("someother")->first.c_str());
    EXPECT_STREQ("field", nestedObj->object().find("some")->second->string().c_str());
    EXPECT_STREQ("field", nestedObj->object().find("someother")->second->string().c_str());

    EXPECT_EQ(Json::Value::Type::MixedArray, ctor.type());

    EXPECT_THROW(ctor.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::Value::TypeMismatch);

    const Json::MixedArray & constMixedArray = ctor;
    
    EXPECT_THROW(constMixedArray.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constMixedArray.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constMixedArray.string(), Json::Value::TypeMismatch);
    EXPECT_THROW(constMixedArray.object(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constMixedArray.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constMixedArray.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constMixedArray.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constMixedArray.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constMixedArray.objectArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, FieldCluster)
{
    Json::FieldCluster ctor;
    EXPECT_TRUE(ctor.object().empty());

    std::map<std::string, std::shared_ptr<Json::Value>> value;
    value.insert(std::pair("first", Json::Bool::Make(false)));
    value.insert(std::pair("second", Json::String::Make("asdf")));
    Json::FieldCluster fieldClusterValue(value);
    auto first = fieldClusterValue.object().find("first");
    auto second = fieldClusterValue.object().find("second");
    EXPECT_STREQ("first", first->first.c_str());
    EXPECT_STREQ("second", second->first.c_str());
    EXPECT_FALSE(first->second->boolean());
    EXPECT_STREQ("asdf", second->second->string().c_str());

    auto ctorMake = Json::FieldCluster::Make();
    EXPECT_TRUE(ctorMake->object().empty());

    auto valueMake = Json::FieldCluster::Make(value);
    first = valueMake->object().find("first");
    second = valueMake->object().find("second");
    EXPECT_STREQ("first", first->first.c_str());
    EXPECT_STREQ("second", second->first.c_str());
    EXPECT_FALSE(first->second->boolean());
    EXPECT_STREQ("asdf", second->second->string().c_str());

    ctor = value;
    first = ctor.object().find("first");
    second = ctor.object().find("second");
    EXPECT_STREQ("first", first->first.c_str());
    EXPECT_STREQ("second", second->first.c_str());
    EXPECT_FALSE(first->second->boolean());
    EXPECT_STREQ("asdf", second->second->string().c_str());

    EXPECT_EQ(Json::Value::Type::FieldCluster, ctor.type());
    
    EXPECT_THROW(ctor.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.string(), Json::Value::TypeMismatch);

    const Json::FieldCluster & constFieldCluster = ctor;
    
    EXPECT_THROW(constFieldCluster.boolean(), Json::Value::TypeMismatch);
    EXPECT_THROW(constFieldCluster.number(), Json::Value::TypeMismatch);
    EXPECT_THROW(constFieldCluster.string(), Json::Value::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::Value::TypeMismatch);

    EXPECT_THROW(constFieldCluster.nullArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constFieldCluster.boolArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constFieldCluster.numberArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constFieldCluster.stringArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constFieldCluster.objectArray(), Json::Value::TypeMismatch);
    EXPECT_THROW(constFieldCluster.mixedArray(), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonGenericTest, GenericValueAssigner)
{
    // All cases where allocatedValue == nullptr
    Json::Value::Assigner nullAssigner(nullptr);
    EXPECT_EQ(nullptr, nullAssigner.get());

    Json::Value::Assigner falseBool(new Json::Bool(false));
    EXPECT_FALSE(falseBool.get()->boolean());
    Json::Value::Assigner trueBool = Json::Value::Assigner(new Json::Bool(true));
    EXPECT_TRUE(trueBool.get()->boolean());
    
    int genericValueInstance = 0;
    EXPECT_THROW(nullAssigner.into(genericValueInstance), Json::Exception);

    Json::Bool someValue = Json::Bool(true);
    EXPECT_THROW(nullAssigner.into(someValue), Json::NullUnassignable);

    Json::Value* genericValuePtr = (Json::Value*)&someValue;
    nullAssigner.into(genericValuePtr);
    EXPECT_TRUE(genericValuePtr == nullptr);

    std::unique_ptr<Json::Value> genericValueUniquePtr = std::unique_ptr<Json::Value>(new Json::Bool(false));
    nullAssigner.into(genericValueUniquePtr);
    EXPECT_TRUE(genericValueUniquePtr == nullptr);

    std::shared_ptr<Json::Value> genericValueSharedPtr = std::shared_ptr<Json::Value>(new Json::Bool(true));
    nullAssigner.into(genericValueSharedPtr);
    EXPECT_TRUE(genericValueSharedPtr == nullptr);

    // All cases where allocatedValue != nullptr and type matches
    Json::Value::Assigner boolInstanceAssigner(new Json::Bool(true));
    Json::Bool boolInstanceReceivable;
    boolInstanceAssigner.into(boolInstanceReceivable);
    EXPECT_TRUE(boolInstanceReceivable.boolean());

    Json::Value::Assigner boolPtrAssigner(new Json::Bool(true));
    Json::Bool* boolPtrUnreceivable = nullptr;
    EXPECT_THROW(boolPtrAssigner.into(boolPtrUnreceivable), Json::Exception);

    Json::Value::Assigner boolUniqueAssigner(new Json::Bool(true));
    std::unique_ptr<Json::Bool> boolUniqueRecievable = nullptr;
    boolUniqueAssigner.into(boolUniqueRecievable);
    EXPECT_TRUE(boolUniqueRecievable->boolean());

    Json::Value::Assigner boolSharedAssigner(new Json::Bool(true));
    std::shared_ptr<Json::Bool> boolSharedReceivable = nullptr;
    boolSharedAssigner.into(boolSharedReceivable);
    EXPECT_TRUE(boolSharedReceivable->boolean());

    // All cases where allocatedValue != nullptr and type mismatches
    Json::Value::Assigner numberNonAssigner(new Json::Number("1"));
    Json::String stringUnassignable;
    EXPECT_THROW(numberNonAssigner.into(stringUnassignable), Json::Value::TypeMismatch);

    Json::Value::Assigner numberNullptrAssigner(new Json::Number("2"));
    Json::String* stringNullptr = nullptr;
    EXPECT_THROW(numberNullptrAssigner.into(stringNullptr), Json::Exception);

    Json::Value::Assigner numberPtrAssigner(new Json::Number("3"));
    Json::String* stringPtrUnassignable = &stringUnassignable;
    EXPECT_THROW(numberPtrAssigner.into(stringPtrUnassignable), Json::Value::TypeMismatch);

    Json::Value::Assigner numberUniqueNullAssigner(new Json::Number("4"));
    std::unique_ptr<Json::String> stringUniqueNullptr = nullptr;
    EXPECT_THROW(numberUniqueNullAssigner.into(stringUniqueNullptr), Json::Exception);

    Json::Value::Assigner numberUniquePtrAssigner(new Json::Number("5"));
    std::unique_ptr<Json::String> stringUniquePtr = std::unique_ptr<Json::String>(new Json::String("asdf"));
    EXPECT_THROW(numberUniquePtrAssigner.into(stringUniquePtr), Json::Value::TypeMismatch);

    Json::Value::Assigner numberSharedNullAssigner(new Json::Number("6"));
    std::shared_ptr<Json::String> stringSharedNullptr = nullptr;
    EXPECT_THROW(numberSharedNullAssigner.into(stringSharedNullptr), Json::Exception);

    Json::Value::Assigner numberSharedPtrAssigner(new Json::Number("7"));
    std::shared_ptr<Json::String> stringSharedPtr = std::unique_ptr<Json::String>(new Json::String("asdf"));
    EXPECT_THROW(numberSharedPtrAssigner.into(stringSharedPtr), Json::Value::TypeMismatch);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeNoSpecialization)
{
    CustomizeNoSpecialization noSpecialization;

    bool isSpecialized = Json::Output::Customize<CustomizeNoSpecialization, int, CustomizeNoSpecialization::Class::IndexOf::integer,
        Annotate<>, CustomizeNoSpecialization::Class::integer_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>
        ::As(os, Json::context, noSpecialization, noSpecialization.integer);
    EXPECT_FALSE(isSpecialized);

    isSpecialized = Json::Output::Customize<CustomizeNoSpecialization, char, CustomizeNoSpecialization::Class::IndexOf::character,
        Annotate<>, CustomizeNoSpecialization::Class::character_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>
        ::As(os, Json::context, noSpecialization, noSpecialization.character);
    EXPECT_FALSE(isSpecialized);

    isSpecialized = Json::Output::HaveSpecialization<CustomizeNoSpecialization, int, CustomizeNoSpecialization::Class::IndexOf::integer,
        Annotate<>, CustomizeNoSpecialization::Class::integer_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeFullySpecialized)
{
    CustomizeFullySpecialized fullySpecialized;

    bool isSpecialized = Json::Output::Customize<CustomizeFullySpecialized, int, CustomizeFullySpecialized::Class::IndexOf::firstField,
        Annotate<>, CustomizeFullySpecialized::Class::firstField_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>
        ::As(os, Json::context, fullySpecialized, fullySpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<CustomizeFullySpecialized, int, CustomizeFullySpecialized::Class::IndexOf::firstField,
        Annotate<>, CustomizeFullySpecialized::Class::firstField_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<CustomizeFullySpecialized, int, CustomizeFullySpecialized::Class::IndexOf::secondField,
        Annotate<>, CustomizeFullySpecialized::Class::secondField_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>
        ::As(os, Json::context, fullySpecialized, fullySpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<CustomizeFullySpecialized, int, CustomizeFullySpecialized::Class::IndexOf::secondField,
        Annotate<>, CustomizeFullySpecialized::Class::secondField_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<CustomizeFullySpecialized, char, CustomizeFullySpecialized::Class::IndexOf::unspecialized,
        Annotate<>, CustomizeFullySpecialized::Class::unspecialized_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>
        ::As(os, Json::context, fullySpecialized, fullySpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<CustomizeFullySpecialized, char, CustomizeFullySpecialized::Class::IndexOf::unspecialized,
        Annotate<>, CustomizeFullySpecialized::Class::unspecialized_::Field, Json::Statics::Included, true, 0, 0, Json::twoSpaces>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize5ArgSpecialized)
{
    Customize5ArgSpecialized fiveArgSpecialized;

    bool isSpecialized = Json::Output::Customize<Customize5ArgSpecialized, int, Customize5ArgSpecialized::Class::IndexOf::firstField,
        Annotate<>, Customize5ArgSpecialized::Class::firstField_::Field>::As(os, Json::context, fiveArgSpecialized, fiveArgSpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized, int, Customize5ArgSpecialized::Class::IndexOf::firstField,
        Annotate<>, Customize5ArgSpecialized::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized, int, Customize5ArgSpecialized::Class::IndexOf::secondField,
        Annotate<>, Customize5ArgSpecialized::Class::secondField_::Field>::As(os, Json::context, fiveArgSpecialized, fiveArgSpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized, int, Customize5ArgSpecialized::Class::IndexOf::secondField,
        Annotate<>, Customize5ArgSpecialized::Class::secondField_::Field>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized, char, Customize5ArgSpecialized::Class::IndexOf::unspecialized,
        Annotate<>, Customize5ArgSpecialized::Class::unspecialized_::Field>::As(os, Json::context, fiveArgSpecialized, fiveArgSpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized, char, Customize5ArgSpecialized::Class::IndexOf::unspecialized,
        Annotate<>, Customize5ArgSpecialized::Class::unspecialized_::Field>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize4ArgSpecialized)
{
    Customize4ArgSpecialized fourArgSpecialized;

    bool isSpecialized = Json::Output::Customize<Customize4ArgSpecialized, int, Customize4ArgSpecialized::Class::IndexOf::firstField,
        Annotate<>>::As(os, Json::context, fourArgSpecialized, fourArgSpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize4ArgSpecialized, int, Customize4ArgSpecialized::Class::IndexOf::firstField,
        Annotate<>>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize4ArgSpecialized, int, Customize4ArgSpecialized::Class::IndexOf::secondField,
        Annotate<>>::As(os, Json::context, fourArgSpecialized, fourArgSpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize4ArgSpecialized, int, Customize4ArgSpecialized::Class::IndexOf::secondField,
        Annotate<>>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize4ArgSpecialized, char, Customize4ArgSpecialized::Class::IndexOf::unspecialized,
        Annotate<>>::As(os, Json::context, fourArgSpecialized, fourArgSpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize4ArgSpecialized, char, Customize4ArgSpecialized::Class::IndexOf::unspecialized,
        Annotate<>>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize3ArgSpecialized)
{
    Customize3ArgSpecialized threeArgSpecialized;

    bool isSpecialized = Json::Output::Customize<Customize3ArgSpecialized, int, Customize3ArgSpecialized::Class::IndexOf::firstField>
        ::As(os, Json::context, threeArgSpecialized, threeArgSpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize3ArgSpecialized, int, Customize3ArgSpecialized::Class::IndexOf::firstField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize3ArgSpecialized, int, Customize3ArgSpecialized::Class::IndexOf::secondField>
        ::As(os, Json::context, threeArgSpecialized, threeArgSpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize3ArgSpecialized, int, Customize3ArgSpecialized::Class::IndexOf::secondField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize3ArgSpecialized, char, Customize3ArgSpecialized::Class::IndexOf::unspecialized>
        ::As(os, Json::context, threeArgSpecialized, threeArgSpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize3ArgSpecialized, char, Customize3ArgSpecialized::Class::IndexOf::unspecialized>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize2ArgSpecialized)
{
    Customize2ArgSpecialized twoArgSpecialized;

    bool isSpecialized = Json::Output::Customize<Customize2ArgSpecialized, int>
        ::As(os, Json::context, twoArgSpecialized, twoArgSpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize2ArgSpecialized, int>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize2ArgSpecialized, int>
        ::As(os, Json::context, twoArgSpecialized, twoArgSpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize2ArgSpecialized, int>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize2ArgSpecialized, char>
        ::As(os, Json::context, twoArgSpecialized, twoArgSpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize2ArgSpecialized, char>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize5ArgSpecialized_OpAnnotationsDefaulted)
{
    Customize5ArgSpecialized_OpAnnotationsDefaulted fiveArgSpecialized_OpAnnotationsDefaulted;

    bool isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::IndexOf::firstField,
        Annotate<>, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::firstField_::Field>
        ::As(os, Json::context, fiveArgSpecialized_OpAnnotationsDefaulted, fiveArgSpecialized_OpAnnotationsDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::IndexOf::firstField,
        Annotate<>, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::IndexOf::secondField,
        Annotate<>, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::secondField_::Field>
        ::As(os, Json::context, fiveArgSpecialized_OpAnnotationsDefaulted, fiveArgSpecialized_OpAnnotationsDefaulted.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::IndexOf::secondField,
        Annotate<>, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::secondField_::Field>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_OpAnnotationsDefaulted, char, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::IndexOf::unspecialized,
        Annotate<>, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::unspecialized_::Field>
        ::As(os, Json::context, fiveArgSpecialized_OpAnnotationsDefaulted, fiveArgSpecialized_OpAnnotationsDefaulted.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_OpAnnotationsDefaulted, char, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::IndexOf::unspecialized,
        Annotate<>, Customize5ArgSpecialized_OpAnnotationsDefaulted::Class::unspecialized_::Field>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize5ArgSpecialized_FieldIndexDefaulted)
{
    Customize5ArgSpecialized_FieldIndexDefaulted fiveArgSpecialized_FieldIndexDefaulted;

    bool isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_FieldIndexDefaulted::Class::firstField_::Field>
        ::As(os, Json::context, fiveArgSpecialized_FieldIndexDefaulted, fiveArgSpecialized_FieldIndexDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_FieldIndexDefaulted::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_FieldIndexDefaulted::Class::secondField_::Field>
        ::As(os, Json::context, fiveArgSpecialized_FieldIndexDefaulted, fiveArgSpecialized_FieldIndexDefaulted.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_FieldIndexDefaulted::Class::secondField_::Field>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_FieldIndexDefaulted, char, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_FieldIndexDefaulted::Class::unspecialized_::Field>
        ::As(os, Json::context, fiveArgSpecialized_FieldIndexDefaulted, fiveArgSpecialized_FieldIndexDefaulted.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_FieldIndexDefaulted, char, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_FieldIndexDefaulted::Class::unspecialized_::Field>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize5ArgSpecialized_BothDefaulted)
{
    Customize5ArgSpecialized_BothDefaulted fiveArgSpecialized_BothDefaulted;

    bool isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_BothDefaulted, int, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_BothDefaulted::Class::firstField_::Field>
        ::As(os, Json::context, fiveArgSpecialized_BothDefaulted, fiveArgSpecialized_BothDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_BothDefaulted, int, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_BothDefaulted::Class::firstField_::Field>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_BothDefaulted, int, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_BothDefaulted::Class::secondField_::Field>
        ::As(os, Json::context, fiveArgSpecialized_BothDefaulted, fiveArgSpecialized_BothDefaulted.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_BothDefaulted, int, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_BothDefaulted::Class::secondField_::Field>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_BothDefaulted, char, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_BothDefaulted::Class::unspecialized_::Field>
        ::As(os, Json::context, fiveArgSpecialized_BothDefaulted, fiveArgSpecialized_BothDefaulted.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize5ArgSpecialized_BothDefaulted, char, Json::NoFieldIndex,
        Annotate<>, Customize5ArgSpecialized_BothDefaulted::Class::unspecialized_::Field>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize4ArgSpecialized_FieldIndexDefaulted)
{
    Customize4ArgSpecialized_FieldIndexDefaulted fourArgSpecialized_FieldIndexDefaulted;

    bool isSpecialized = Json::Output::Customize<Customize4ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex, Annotate<>>
        ::As(os, Json::context, fourArgSpecialized_FieldIndexDefaulted, fourArgSpecialized_FieldIndexDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize4ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex, Annotate<>>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize4ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex, Annotate<>>
        ::As(os, Json::context, fourArgSpecialized_FieldIndexDefaulted, fourArgSpecialized_FieldIndexDefaulted.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize4ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex, Annotate<>>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize4ArgSpecialized_FieldIndexDefaulted, char, Json::NoFieldIndex, Annotate<>>
        ::As(os, Json::context, fourArgSpecialized_FieldIndexDefaulted, fourArgSpecialized_FieldIndexDefaulted.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<Customize4ArgSpecialized_FieldIndexDefaulted, char, Json::NoFieldIndex, Annotate<>>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeUnspecialized)
{
    ContainsUnspecialized containsUnspecialized;

    bool isSpecialized = Json::Output::CustomizeType<UnspecializedType, Annotate<>, ContainsUnspecialized::Class::unspecializedType_::Field,
        Json::Statics::Included, true, 0, 0, Json::twoSpaces>::As(os, Json::context, containsUnspecialized.unspecializedType);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<ContainsUnspecialized, UnspecializedType, ContainsUnspecialized::Class::IndexOf::unspecializedType, Annotate<>, ContainsUnspecialized::Class::unspecializedType_::Field,
        Json::Statics::Included, true, 0, 0, Json::twoSpaces>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeFullySpecialized)
{
    ContainsFullySpecialized containsFullySpecialized;

    bool isSpecialized = Json::Output::CustomizeType<FullySpecializedType, Annotate<>, ContainsFullySpecialized::Class::fullySpecializedType_::Field,
        Json::Statics::Included, true, 0, 0, Json::twoSpaces>::As(os, Json::context, containsFullySpecialized.fullySpecializedType);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<ContainsFullySpecialized, FullySpecializedType, ContainsFullySpecialized::Class::IndexOf::fullySpecializedType, Annotate<>, ContainsFullySpecialized::Class::fullySpecializedType_::Field,
        Json::Statics::Included, true, 0, 0, Json::twoSpaces>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeThreeArgSpecialized)
{
    ContainsThreeArgSpecialized containsThreeArgSpecialized;

    bool isSpecialized = Json::Output::CustomizeType<ThreeArgSpecializedType, Annotate<>, ContainsThreeArgSpecialized::Class::threeArgSpecializedType_::Field>
        ::As(os, Json::context, containsThreeArgSpecialized.threeArgSpecializedType);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<ContainsThreeArgSpecialized, ThreeArgSpecializedType, ContainsThreeArgSpecialized::Class::IndexOf::threeArgSpecializedType, Annotate<>, ContainsThreeArgSpecialized::Class::threeArgSpecializedType_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeTwoArgSpecialized)
{
    ContainsTwoArgSpecialized containsTwoArgSpecialized;

    bool isSpecialized = Json::Output::CustomizeType<TwoArgSpecializedType, Annotate<>>
        ::As(os, Json::context, containsTwoArgSpecialized.twoArgSpecializedType);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<ContainsTwoArgSpecialized, TwoArgSpecializedType, ContainsTwoArgSpecialized::Class::IndexOf::twoArgSpecializedType, Annotate<>>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeOneArgSpecialized)
{
    ContainsOneArgSpecialized containsOneArgSpecialized;

    bool isSpecialized = Json::Output::CustomizeType<OneArgSpecializedType>
        ::As(os, Json::context, containsOneArgSpecialized.oneArgSpecializedType);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<ContainsOneArgSpecialized, OneArgSpecializedType>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeThreeArgSpecialized_OpAnnotationsDefaulted)
{
    ContainsThreeArgSpecializedType_OpAnnotationsDefaulted containsThreeArgSpecializedType_OpAnnotationsDefaulted;

    bool isSpecialized = Json::Output::CustomizeType<ThreeArgSpecializedType_OpAnnotationsDefaulted, Annotate<>,
        ContainsThreeArgSpecializedType_OpAnnotationsDefaulted::Class::threeArgSpecializedType_OpAnnotationsDefaulted_::Field>
        ::As(os, Json::context, containsThreeArgSpecializedType_OpAnnotationsDefaulted.threeArgSpecializedType_OpAnnotationsDefaulted);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::HaveSpecialization<ContainsThreeArgSpecializedType_OpAnnotationsDefaulted, ThreeArgSpecializedType_OpAnnotationsDefaulted,
        ContainsThreeArgSpecializedType_OpAnnotationsDefaulted::Class::IndexOf::threeArgSpecializedType_OpAnnotationsDefaulted,
        Annotate<>, ContainsThreeArgSpecializedType_OpAnnotationsDefaulted::Class::threeArgSpecializedType_OpAnnotationsDefaulted_::Field>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputStaticAffixTest, StaticIndent)
{
    TestStreamType zeroIndents;
    zeroIndents << Json::Output::Indent<true, 0, Json::twoSpaces>;
    EXPECT_STREQ("", zeroIndents.str().c_str());

    TestStreamType oneIndent;
    oneIndent << Json::Output::Indent<true, 1, Json::twoSpaces>;
    EXPECT_STREQ("  ", oneIndent.str().c_str());

    TestStreamType twoIndents;
    twoIndents << Json::Output::Indent<true, 2, Json::twoSpaces>;
    EXPECT_STREQ("    ", twoIndents.str().c_str());

    TestStreamType threeIndents;
    threeIndents << Json::Output::Indent<true, 3, Json::twoSpaces>;
    EXPECT_STREQ("      ", threeIndents.str().c_str());

    TestStreamType zeroIndentsUnpretty;
    zeroIndentsUnpretty << Json::Output::Indent<false, 0, Json::twoSpaces>;
    EXPECT_STREQ("", zeroIndentsUnpretty.str().c_str());

    TestStreamType oneIndentUnpretty;
    oneIndentUnpretty << Json::Output::Indent<false, 1, Json::twoSpaces>;
    EXPECT_STREQ("", oneIndentUnpretty.str().c_str());

    TestStreamType twoIndentsUnpretty;
    twoIndentsUnpretty << Json::Output::Indent<false, 2, Json::twoSpaces>;
    EXPECT_STREQ("", twoIndentsUnpretty.str().c_str());

    TestStreamType threeIndentsUnpretty;
    threeIndentsUnpretty << Json::Output::Indent<false, 3, Json::twoSpaces>;
    EXPECT_STREQ("", threeIndentsUnpretty.str().c_str());
}

TEST_HEADER(JsonOutputStaticAffixTest, StaticArrayPrefix)
{
    TestStreamType noPrettyPrint;
    noPrettyPrint << Json::Output::ArrayPrefix<false, false, 0, Json::twoSpaces>;
    EXPECT_STREQ("[", noPrettyPrint.str().c_str());

    TestStreamType primitivePrettyPrint;
    primitivePrettyPrint << Json::Output::ArrayPrefix<true, true, 0, Json::twoSpaces>;
    EXPECT_STREQ("[ ", primitivePrettyPrint.str().c_str());

    TestStreamType nonPrimitivePrettyCompare;
    nonPrimitivePrettyCompare << "[" << osEndl << Json::Indent<true, 3, Json::twoSpaces>;
    TestStreamType nonPrimitivePretty;
    nonPrimitivePretty << Json::ArrayPrefix<true, false, 3, Json::twoSpaces>;
    EXPECT_STREQ(nonPrimitivePrettyCompare.str().c_str(), nonPrimitivePretty.str().c_str());
}

TEST_HEADER(JsonOutputStaticAffixTest, StaticArraySuffix)
{
    TestStreamType noPrettyPrint;
    noPrettyPrint << Json::Output::ArraySuffix<false, false, 0, Json::twoSpaces>;
    EXPECT_STREQ("]", noPrettyPrint.str().c_str());

    TestStreamType primitivePrettyPrint;
    primitivePrettyPrint << Json::Output::ArraySuffix<true, true, 0, Json::twoSpaces>;
    EXPECT_STREQ(" ]", primitivePrettyPrint.str().c_str());

    TestStreamType nonPrimitivePrettyCompare;
    nonPrimitivePrettyCompare << osEndl << Json::Output::Indent<true, 3, Json::twoSpaces> << "]";
    TestStreamType nonPrimitivePretty;
    nonPrimitivePretty << Json::Output::ArraySuffix<true, false, 3, Json::twoSpaces>;
    EXPECT_STREQ(nonPrimitivePrettyCompare.str().c_str(), nonPrimitivePretty.str().c_str());
}

TEST_HEADER(JsonOutputStaticAffixTest, StaticObjectPrefix)
{
    TestStreamType objectPrefix;
    objectPrefix << Json::Output::ObjectPrefix<false, 0, Json::twoSpaces, Json::Statics::Included, int>;
    EXPECT_STREQ("{", objectPrefix.str().c_str());
}

struct TrulyEmpty
{
    REFLECT_EMPTY(() TrulyEmpty)
};

struct NoSuperOrStatic
{
    int instanceField;

    REFLECT(() NoSuperOrStatic, () instanceField)
};

struct NoSuperOrInstance
{
    static int staticField;

    REFLECT(() NoSuperOrInstance, () staticField)
};
int NoSuperOrInstance::staticField = 0;

struct NoSuper
{
    int instanceField;
    static int staticField;

    REFLECT(() NoSuper, () instanceField, () staticField)
};
int NoSuper::staticField = 0;

struct Super
{
    int instanceField;
    static int staticField;

    REFLECT(() Super, () instanceField, () staticField)
};

struct NoStaticOrInstance : public Super
{
    REFLECT_EMPTY((Super) NoStaticOrInstance)
};

struct NoStatic : public Super
{
    int instanceField;
    
    REFLECT((Super) NoStatic, () instanceField)
};

struct NoInstance : public Super
{
    static int staticField;

    REFLECT((Super) NoInstance, () staticField)
};
int NoInstance::staticField = 0;

struct Everything : public Super
{
    int instanceField;
    static int staticField;

    REFLECT((Super) Everything, () instanceField, () staticField)
};
int Everything::staticField = 0;

TEST_HEADER(JsonOutputStaticAffixTest, StaticObjectSuffix)
{
    TestStreamType voidPrettyCompare;
    voidPrettyCompare << osEndl << Json::Output::Indent<true, 3, Json::twoSpaces> << "}";
    TestStreamType voidPretty;
    voidPretty << Json::Output::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, void>;
    EXPECT_STREQ(voidPrettyCompare.str().c_str(), voidPretty.str().c_str());

    TestStreamType voidNonPretty;
    voidNonPretty << Json::Output::ObjectSuffix<false, 3, Json::twoSpaces, Json::Statics::Included, void>;
    EXPECT_STREQ("}", voidNonPretty.str().c_str());

    TestStreamType nonVoidNonPretty;
    nonVoidNonPretty << Json::Output::ObjectSuffix<false, 3, Json::twoSpaces, Json::Statics::Included, Everything>;
    EXPECT_STREQ("}", nonVoidNonPretty.str().c_str());
    
    TestStreamType trulyEmptyExcluded;
    TestStreamType trulyEmptyIncluded;
    TestStreamType trulyEmptyOnly;
    
    TestStreamType noSuperOrStaticExcluded;
    TestStreamType noSuperOrStaticIncluded;
    TestStreamType noSuperOrStaticOnly;
    
    TestStreamType noSuperOrInstanceExcluded;
    TestStreamType noSuperOrInstanceIncluded;
    TestStreamType noSuperOrInstanceOnly;
    
    TestStreamType noSuperExcluded;
    TestStreamType noSuperIncluded;
    TestStreamType noSuperOnly;
    
    TestStreamType noStaticOrInstanceExcluded;
    TestStreamType noStaticOrInstanceIncluded;
    TestStreamType noStaticOrInstanceOnly;
    
    TestStreamType noStaticExcluded;
    TestStreamType noStaticIncluded;
    TestStreamType noStaticOnly;
    
    TestStreamType noInstanceExcluded;
    TestStreamType noInstanceIncluded;
    TestStreamType noInstanceOnly;
    
    TestStreamType everythingExcluded;
    TestStreamType everythingIncluded;
    TestStreamType everythingOnly;
    
    trulyEmptyExcluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Excluded, TrulyEmpty>;
    trulyEmptyIncluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, TrulyEmpty>;
    trulyEmptyOnly << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Only, TrulyEmpty>;
    
    noSuperOrStaticExcluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Excluded, NoSuperOrStatic>;
    noSuperOrStaticIncluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, NoSuperOrStatic>;
    noSuperOrStaticOnly << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Only, NoSuperOrStatic>;
    
    noSuperOrInstanceExcluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Excluded, NoSuperOrInstance>;
    noSuperOrInstanceIncluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, NoSuperOrInstance>;
    noSuperOrInstanceOnly << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Only, NoSuperOrInstance>;
    
    noSuperExcluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Excluded, NoSuper>;
    noSuperIncluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, NoSuper>;
    noSuperOnly << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Only, NoSuper>;
    
    noStaticOrInstanceExcluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Excluded, NoStaticOrInstance>;
    noStaticOrInstanceIncluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, NoStaticOrInstance>;
    noStaticOrInstanceOnly << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Only, NoStaticOrInstance>;
    
    noStaticExcluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Excluded, NoStatic>;
    noStaticIncluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, NoStatic>;
    noStaticOnly << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Only, NoStatic>;
    
    noInstanceExcluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Excluded, NoInstance>;
    noInstanceIncluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, NoInstance>;
    noInstanceOnly << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Only, NoInstance>;
    
    everythingExcluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Excluded, Everything>;
    everythingIncluded << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Included, Everything>;
    everythingOnly << Json::ObjectSuffix<true, 3, Json::twoSpaces, Json::Statics::Only, Everything>;

    TestStreamType prettyNotEmptyCompareStream;
    prettyNotEmptyCompareStream << osEndl << Json::Indent<true, 3, Json::twoSpaces> << "}";
    std::string prettyNotEmptyCompareStr = prettyNotEmptyCompareStream.str();
    const char* prettyNotEmptyCompare = prettyNotEmptyCompareStr.c_str();
    
    EXPECT_STREQ("}", trulyEmptyExcluded.str().c_str());
    EXPECT_STREQ("}", trulyEmptyIncluded.str().c_str());
    EXPECT_STREQ("}", trulyEmptyOnly.str().c_str());
    
    EXPECT_STREQ(prettyNotEmptyCompare, noSuperOrStaticExcluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noSuperOrStaticIncluded.str().c_str());
    EXPECT_STREQ("}", noSuperOrStaticOnly.str().c_str());
    
    EXPECT_STREQ("}", noSuperOrInstanceExcluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noSuperOrInstanceIncluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noSuperOrInstanceOnly.str().c_str());
    
    EXPECT_STREQ(prettyNotEmptyCompare, noSuperExcluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noSuperIncluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noSuperOnly.str().c_str());
    
    EXPECT_STREQ(prettyNotEmptyCompare, noStaticOrInstanceExcluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noStaticOrInstanceIncluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noStaticOrInstanceOnly.str().c_str());
    
    EXPECT_STREQ(prettyNotEmptyCompare, noStaticExcluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noStaticIncluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noStaticOnly.str().c_str());
    
    EXPECT_STREQ(prettyNotEmptyCompare, noInstanceExcluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noInstanceIncluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, noInstanceOnly.str().c_str());
    
    EXPECT_STREQ(prettyNotEmptyCompare, everythingExcluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, everythingIncluded.str().c_str());
    EXPECT_STREQ(prettyNotEmptyCompare, everythingOnly.str().c_str());
}

TEST_HEADER(JsonOutputStaticAffixTest, StaticFieldPrefix)
{
    TestStreamType nonPrettyFirstField;
    nonPrettyFirstField << Json::Output::FieldPrefix<true, false, 3, Json::twoSpaces>;
    EXPECT_STREQ("", nonPrettyFirstField.str().c_str());

    TestStreamType nonPrettyNonFirst;
    nonPrettyNonFirst << Json::Output::FieldPrefix<false, false, 3, Json::twoSpaces>;
    EXPECT_STREQ(",", nonPrettyNonFirst.str().c_str());

    TestStreamType prettyFirstCompare;
    prettyFirstCompare << osEndl << Json::Output::Indent<true, 3, Json::twoSpaces>;
    TestStreamType prettyFirst;
    prettyFirst << Json::Output::FieldPrefix<true, true, 3, Json::twoSpaces>;
    EXPECT_STREQ(prettyFirstCompare.str().c_str(), prettyFirst.str().c_str());

    TestStreamType prettyNonFirstCompare;
    prettyNonFirstCompare << "," << osEndl << Json::Output::Indent<true, 3, Json::twoSpaces>;
    TestStreamType prettyNonFirst;
    prettyNonFirst << Json::Output::FieldPrefix<false, true, 3, Json::twoSpaces>;
    EXPECT_STREQ(prettyNonFirstCompare.str().c_str(), prettyNonFirst.str().c_str());
}

TEST_HEADER(JsonOutputStaticAffixTest, StaticFieldNameValueSeparator)
{
    TestStreamType pretty;
    pretty << Json::Output::FieldNameValueSeparator<true>;
    EXPECT_STREQ(": ", pretty.str().c_str());

    TestStreamType nonPretty;
    nonPretty << Json::Output::FieldNameValueSeparator<false>;
    EXPECT_STREQ(":", nonPretty.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, Indent)
{
    TestStreamType prettyIndent;
    Json::Put::Indent<true, Json::twoSpaces>(prettyIndent, 3);
    EXPECT_STREQ("      ", prettyIndent.str().c_str());

    TestStreamType nonPrettyIndent;
    Json::Put::Indent<false, Json::twoSpaces>(nonPrettyIndent, 3);
    EXPECT_STREQ("", nonPrettyIndent.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, ArrayPrefix)
{
    TestStreamType nonPretty;
    Json::Put::ArrayPrefix<false, false, Json::twoSpaces>(nonPretty, 3);
    EXPECT_STREQ("[", nonPretty.str().c_str());

    TestStreamType prettyPrimitives;
    Json::Put::ArrayPrefix<true, true, Json::twoSpaces>(prettyPrimitives, 3);
    EXPECT_STREQ("[ ", prettyPrimitives.str().c_str());

    TestStreamType prettyNonPrimitivesCompare;
    prettyNonPrimitivesCompare << "[" << osEndl;
    Json::Put::Indent<true, Json::twoSpaces>(prettyNonPrimitivesCompare, 3);

    TestStreamType prettyNonPrimitives;
    Json::Put::ArrayPrefix<true, false, Json::twoSpaces>(prettyNonPrimitives, 3);
    EXPECT_STREQ(prettyNonPrimitivesCompare.str().c_str(), prettyNonPrimitives.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, ArraySuffix)
{
    TestStreamType nonPretty;
    Json::Put::ArraySuffix<false, false, Json::twoSpaces>(nonPretty, 3);
    EXPECT_STREQ("]", nonPretty.str().c_str());

    TestStreamType prettyPrimitives;
    Json::Put::ArraySuffix<true, true, Json::twoSpaces>(prettyPrimitives, 3);
    EXPECT_STREQ(" ]", prettyPrimitives.str().c_str());

    TestStreamType prettyNonPrimitivesCompare;
    prettyNonPrimitivesCompare << osEndl;
    Json::Put::Indent<true, Json::twoSpaces>(prettyNonPrimitivesCompare, 3);
    prettyNonPrimitivesCompare << "]";

    TestStreamType prettyNonPrimitives;
    Json::Put::ArraySuffix<true, false, Json::twoSpaces>(prettyNonPrimitives, 3);
    EXPECT_STREQ(prettyNonPrimitivesCompare.str().c_str(), prettyNonPrimitives.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, ObjectPrefix)
{
    TestStreamType objectPrefix;
    Json::Put::ObjectPrefix<true, Json::twoSpaces>(objectPrefix, 3);
    EXPECT_STREQ("{", objectPrefix.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, ObjectSuffix)
{
    TestStreamType nonPretty;
    Json::Put::ObjectSuffix<false, Json::twoSpaces>(nonPretty, false, 3);
    EXPECT_STREQ("}", nonPretty.str().c_str());

    TestStreamType prettyEmpty;
    Json::Put::ObjectSuffix<true, Json::twoSpaces>(prettyEmpty, true, 3);
    EXPECT_STREQ("}", prettyEmpty.str().c_str());

    TestStreamType prettyNonEmptyCompare;
    prettyNonEmptyCompare << osEndl;
    Json::Put::Indent<true, Json::twoSpaces>(prettyNonEmptyCompare, 3);
    prettyNonEmptyCompare << "}";

    TestStreamType prettyNonEmpty;
    Json::Put::ObjectSuffix<true, Json::twoSpaces>(prettyNonEmpty, false, 3);
    EXPECT_STREQ(prettyNonEmptyCompare.str().c_str(), prettyNonEmpty.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, FieldPrefix)
{
    TestStreamType prettyFirstCompare;
    prettyFirstCompare << osEndl;
    Json::Put::Indent<true, Json::twoSpaces>(prettyFirstCompare, 3);
    TestStreamType prettyFirst;
    Json::Put::FieldPrefix<true, Json::twoSpaces>(prettyFirst, true, 3);
    EXPECT_STREQ(prettyFirstCompare.str().c_str(), prettyFirst.str().c_str());

    TestStreamType prettyNonFirstCompare;
    prettyNonFirstCompare << "," << osEndl;
    Json::Put::Indent<true, Json::twoSpaces>(prettyNonFirstCompare, 3);
    TestStreamType prettyNonFirst;
    Json::Put::FieldPrefix<true, Json::twoSpaces>(prettyNonFirst, false, 3);
    EXPECT_STREQ(prettyNonFirstCompare.str().c_str(), prettyNonFirst.str().c_str());

    TestStreamType nonPrettyNonFirst;
    Json::Put::FieldPrefix<false, Json::twoSpaces>(nonPrettyNonFirst, false, 3);
    EXPECT_STREQ(",", nonPrettyNonFirst.str().c_str());

    TestStreamType nonPrettyFirst;
    Json::Put::FieldPrefix<false, Json::twoSpaces>(nonPrettyFirst, true, 3);
    EXPECT_STREQ("", nonPrettyFirst.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, PartiallyStaticNestedPrefix)
{
    TestStreamType emptyArrayCompare;
    emptyArrayCompare << Json::ArrayPrefix<false, false, 3, Json::twoSpaces>;
    TestStreamType emptyArray;
    Json::Put::NestedPrefix<true, true, false, 3, Json::twoSpaces>(emptyArray, true);
    EXPECT_STREQ(emptyArrayCompare.str().c_str(), emptyArray.str().c_str());
    
    TestStreamType nonEmptyArrayCompare;
    nonEmptyArrayCompare << Json::ArrayPrefix<true, false, 3, Json::twoSpaces>;
    TestStreamType nonEmptyArray;
    Json::Put::NestedPrefix<true, true, false, 3, Json::twoSpaces>(nonEmptyArray, false);
    EXPECT_STREQ(nonEmptyArrayCompare.str().c_str(), nonEmptyArray.str().c_str());

    TestStreamType emptyObjectCompare;
    emptyObjectCompare << Json::ObjectPrefix<false, 3, Json::twoSpaces>;
    TestStreamType emptyObject;
    Json::Put::NestedPrefix<true, false, false, 3, Json::twoSpaces>(emptyObject, true);
    EXPECT_STREQ(emptyObjectCompare.str().c_str(), emptyObject.str().c_str());

    TestStreamType nonEmptyObjectCompare;
    nonEmptyObjectCompare << Json::ObjectPrefix<true, 3, Json::twoSpaces>;
    TestStreamType nonEmptyObject;
    Json::Put::NestedPrefix<true, false, false, 3, Json::twoSpaces>(nonEmptyObject, false);
    EXPECT_STREQ(nonEmptyObjectCompare.str().c_str(), nonEmptyObject.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, NestedPrefix)
{
    TestStreamType emptyArrayPrimitivesCompare;
    Json::Put::ArrayPrefix<false, true, Json::twoSpaces>(emptyArrayPrimitivesCompare, 3);
    TestStreamType emptyArrayPrimitives;
    Json::Put::NestedPrefix<true, Json::twoSpaces>(emptyArrayPrimitives, true, true, true, 3);
    EXPECT_STREQ(emptyArrayPrimitivesCompare.str().c_str(), emptyArrayPrimitives.str().c_str());

    TestStreamType emptyArrayNonPrimitivesCompare;
    Json::Put::ArrayPrefix<false, false, Json::twoSpaces>(emptyArrayNonPrimitivesCompare, 3);
    TestStreamType emptyArrayNonPrimitives;
    Json::Put::NestedPrefix<true, Json::twoSpaces>(emptyArrayNonPrimitives, true, false, true, 3);
    EXPECT_STREQ(emptyArrayNonPrimitivesCompare.str().c_str(), emptyArrayNonPrimitives.str().c_str());
    
    TestStreamType nonEmptyArrayPrimitivesCompare;
    Json::Put::ArrayPrefix<true, true, Json::twoSpaces>(nonEmptyArrayPrimitivesCompare, 3);
    TestStreamType nonEmptyArrayPrimitives;
    Json::Put::NestedPrefix<true, Json::twoSpaces>(nonEmptyArrayPrimitives, true, true, false, 3);
    EXPECT_STREQ(nonEmptyArrayPrimitivesCompare.str().c_str(), nonEmptyArrayPrimitives.str().c_str());

    TestStreamType nonEmptyArrayNonPrimitivesCompare;
    Json::Put::ArrayPrefix<true, false, Json::twoSpaces>(nonEmptyArrayNonPrimitivesCompare, 3);
    TestStreamType nonEmptyArrayNonPrimitives;
    Json::Put::NestedPrefix<true, Json::twoSpaces>(nonEmptyArrayNonPrimitives, true, false, false, 3);
    EXPECT_STREQ(nonEmptyArrayNonPrimitivesCompare.str().c_str(), nonEmptyArrayNonPrimitives.str().c_str());
    
    TestStreamType emptyObjectCompare;
    Json::Put::ObjectPrefix<false, Json::twoSpaces>(emptyObjectCompare, 3);
    TestStreamType emptyObject;
    Json::Put::NestedPrefix<true, Json::twoSpaces>(emptyObject, false, true, true, 3);
    EXPECT_STREQ(emptyObjectCompare.str().c_str(), emptyObject.str().c_str());

    TestStreamType nonEmptyObjectCompare;
    Json::Put::ObjectPrefix<true, Json::twoSpaces>(nonEmptyObjectCompare, 3);
    TestStreamType nonEmptyObject;
    Json::Put::NestedPrefix<true, Json::twoSpaces>(nonEmptyObject, false, true, false, 3);
    EXPECT_STREQ(nonEmptyObjectCompare.str().c_str(), nonEmptyObject.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, PartiallyStaticNestedSuffix)
{
    TestStreamType emptyArrayCompare;
    emptyArrayCompare << Json::ArraySuffix<false, false, 3, Json::twoSpaces>;
    TestStreamType emptyArray;
    Json::Put::NestedSuffix<true, true, false, 3, Json::twoSpaces>(emptyArray, true);
    EXPECT_STREQ(emptyArrayCompare.str().c_str(), emptyArray.str().c_str());

    TestStreamType nonEmptyArrayCompare;
    nonEmptyArrayCompare << Json::ArraySuffix<true, false, 3, Json::twoSpaces>;
    TestStreamType nonEmptyArray;
    Json::Put::NestedSuffix<true, true, false, 3, Json::twoSpaces>(nonEmptyArray, false);
    EXPECT_STREQ(nonEmptyArrayCompare.str().c_str(), nonEmptyArray.str().c_str());

    TestStreamType emptyObjectCompare;
    emptyObjectCompare << Json::ObjectSuffix<false, 3, Json::twoSpaces>;
    TestStreamType emptyObject;
    Json::Put::NestedSuffix<true, false, false, 3, Json::twoSpaces>(emptyObject, true);
    EXPECT_STREQ(emptyObjectCompare.str().c_str(), emptyObject.str().c_str());

    TestStreamType nonEmptyObjectCompare;
    nonEmptyObjectCompare << Json::ObjectSuffix<true, 3, Json::twoSpaces>;
    TestStreamType nonEmptyObject;
    Json::Put::NestedSuffix<true, false, false, 3, Json::twoSpaces>(nonEmptyObject, false);
    EXPECT_STREQ(nonEmptyObjectCompare.str().c_str(), nonEmptyObject.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, NestedSuffix)
{
    TestStreamType emptyArrayPrimitivesCompare;
    Json::Put::ArraySuffix<false, true, Json::twoSpaces>(emptyArrayPrimitivesCompare, 3);
    TestStreamType emptyArrayPrimitives;
    Json::Put::NestedSuffix<true, Json::twoSpaces>(emptyArrayPrimitives, true, true, true, 3);
    EXPECT_STREQ(emptyArrayPrimitivesCompare.str().c_str(), emptyArrayPrimitives.str().c_str());
    
    TestStreamType emptyArrayNonPrimitivesCompare;
    Json::Put::ArraySuffix<false, false, Json::twoSpaces>(emptyArrayNonPrimitivesCompare, 3);
    TestStreamType emptyArrayNonPrimitives;
    Json::Put::NestedSuffix<true, Json::twoSpaces>(emptyArrayNonPrimitives, true, false, true, 3);
    EXPECT_STREQ(emptyArrayNonPrimitivesCompare.str().c_str(), emptyArrayNonPrimitives.str().c_str());
    
    TestStreamType nonEmptyArrayPrimitivesCompare;
    Json::Put::ArraySuffix<true, true, Json::twoSpaces>(nonEmptyArrayPrimitivesCompare, 3);
    TestStreamType nonEmptyArrayPrimitives;
    Json::Put::NestedSuffix<true, Json::twoSpaces>(nonEmptyArrayPrimitives, true, true, false, 3);
    EXPECT_STREQ(nonEmptyArrayPrimitivesCompare.str().c_str(), nonEmptyArrayPrimitives.str().c_str());
    
    TestStreamType nonEmptyArrayNonPrimitivesCompare;
    Json::Put::ArraySuffix<true, false, Json::twoSpaces>(nonEmptyArrayNonPrimitivesCompare, 3);
    TestStreamType nonEmptyArrayNonPrimitives;
    Json::Put::NestedSuffix<true, Json::twoSpaces>(nonEmptyArrayNonPrimitives, true, false, false, 3);
    EXPECT_STREQ(nonEmptyArrayNonPrimitivesCompare.str().c_str(), nonEmptyArrayNonPrimitives.str().c_str());
    
    TestStreamType emptyObjectCompare;
    Json::Put::ObjectSuffix<false, Json::twoSpaces>(emptyObjectCompare, true, 3);
    TestStreamType emptyObject;
    Json::Put::NestedSuffix<true, Json::twoSpaces>(emptyObject, false, false, true, 3);
    EXPECT_STREQ(emptyObjectCompare.str().c_str(), emptyObject.str().c_str());
    
    TestStreamType nonEmptyObjectCompare;
    Json::Put::ObjectSuffix<true, Json::twoSpaces>(nonEmptyObjectCompare, false, 3);
    TestStreamType nonEmptyObject;
    Json::Put::NestedSuffix<true, Json::twoSpaces>(nonEmptyObject, false, false, false, 3);
    EXPECT_STREQ(nonEmptyObjectCompare.str().c_str(), nonEmptyObject.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, PartiallyStaticSeparator)
{
    TestStreamType jsonFirstPrettyCompare;
    jsonFirstPrettyCompare << osEndl << Json::Indent<true, 3, Json::twoSpaces>;
    TestStreamType jsonFirstPretty;
    Json::Put::Separator<true, true, false, 3, Json::twoSpaces>(jsonFirstPretty, true);
    EXPECT_STREQ(jsonFirstPrettyCompare.str().c_str(), jsonFirstPretty.str().c_str());
    
    TestStreamType jsonNonFirstPrettyCompare;
    jsonNonFirstPrettyCompare << "," << osEndl << Json::Indent<true, 3, Json::twoSpaces>;
    TestStreamType jsonNonFirstPretty;
    Json::Put::Separator<true, true, false, 3, Json::twoSpaces>(jsonNonFirstPretty, false);
    EXPECT_STREQ(jsonNonFirstPrettyCompare.str().c_str(), jsonNonFirstPretty.str().c_str());

    TestStreamType jsonFirstNonPretty;
    Json::Put::Separator<false, true, false, 3, Json::twoSpaces>(jsonFirstNonPretty, true);
    EXPECT_STREQ("", jsonFirstNonPretty.str().c_str());

    TestStreamType jsonNonFirstNonPretty;
    Json::Put::Separator<false, true, false, 3, Json::twoSpaces>(jsonNonFirstNonPretty, false);
    EXPECT_STREQ(",", jsonNonFirstNonPretty.str().c_str());
    
    TestStreamType nonJsonFieldFirst;
    Json::Put::Separator<true, false, false, 3, Json::twoSpaces>(nonJsonFieldFirst, true);
    EXPECT_STREQ("", nonJsonFieldFirst.str().c_str());

    TestStreamType nonJsonFieldNestedPrettyCompare;
    nonJsonFieldNestedPrettyCompare << "," << osEndl << Json::Indent<true, 3, Json::twoSpaces>;
    TestStreamType nonJsonFieldNestedPretty;
    Json::Put::Separator<true, false, true, 3, Json::twoSpaces>(nonJsonFieldNestedPretty, false);
    EXPECT_STREQ(nonJsonFieldNestedPrettyCompare.str().c_str(), nonJsonFieldNestedPretty.str().c_str());

    TestStreamType nonJsonFieldNonNestedPretty;
    Json::Put::Separator<true, false, false, 3, Json::twoSpaces>(nonJsonFieldNonNestedPretty, false);
    EXPECT_STREQ(", ", nonJsonFieldNonNestedPretty.str().c_str());

    TestStreamType nonJsonFieldNestedNonPretty;
    Json::Put::Separator<false, false, true, 3, Json::twoSpaces>(nonJsonFieldNestedNonPretty, false);
    EXPECT_STREQ(",", nonJsonFieldNestedNonPretty.str().c_str());

    TestStreamType nonJsonFieldNonNestedNonPretty;
    Json::Put::Separator<false, false, false, 3, Json::twoSpaces>(nonJsonFieldNonNestedNonPretty, false);
    EXPECT_STREQ(",", nonJsonFieldNonNestedNonPretty.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, Separator)
{
    TestStreamType jsonFirstPrettyCompare;
    jsonFirstPrettyCompare << osEndl;
    Json::Put::Indent<true, Json::twoSpaces>(jsonFirstPrettyCompare, 3);
    TestStreamType jsonFirstPretty;
    Json::Put::Separator<true, true, false, Json::twoSpaces>(jsonFirstPretty, true, 3);
    EXPECT_STREQ(jsonFirstPrettyCompare.str().c_str(), jsonFirstPretty.str().c_str());

    TestStreamType jsonFirstNonPretty;
    Json::Put::Separator<false, true, false, Json::twoSpaces>(jsonFirstNonPretty, true, 3);
    EXPECT_STREQ("", jsonFirstNonPretty.str().c_str());

    TestStreamType jsonNonFirstPrettyCompare;
    jsonNonFirstPrettyCompare << "," << osEndl;
    Json::Put::Indent<true, Json::twoSpaces>(jsonNonFirstPrettyCompare, 3);
    TestStreamType jsonNonFirstPretty;
    Json::Put::Separator<true, true, false, Json::twoSpaces>(jsonNonFirstPretty, false, 3);
    EXPECT_STREQ(jsonNonFirstPrettyCompare.str().c_str(), jsonNonFirstPretty.str().c_str());

    TestStreamType jsonNonFirstNonPretty;
    Json::Put::Separator<false, true, false, Json::twoSpaces>(jsonNonFirstNonPretty, false, 3);
    EXPECT_STREQ(",", jsonNonFirstNonPretty.str().c_str());
    
    TestStreamType nonJsonNonFirstNestedPrettyCompare;
    nonJsonNonFirstNestedPrettyCompare << "," << osEndl;
    Json::Put::Indent<true, Json::twoSpaces>(nonJsonNonFirstNestedPrettyCompare, 3);
    TestStreamType nonJsonNonFirstNestedPretty;
    Json::Put::Separator<true, false, true, Json::twoSpaces>(nonJsonNonFirstNestedPretty, false, 3);
    EXPECT_STREQ(nonJsonNonFirstNestedPrettyCompare.str().c_str(), nonJsonNonFirstNestedPretty.str().c_str());

    TestStreamType nonJsonNonFirstNestedNonPretty;
    Json::Put::Separator<false, false, true, Json::twoSpaces>(nonJsonNonFirstNestedNonPretty, false, 3);
    EXPECT_STREQ(",", nonJsonNonFirstNestedNonPretty.str().c_str());

    TestStreamType nonJsonNonFirstNonNestedPretty;
    Json::Put::Separator<true, false, false, Json::twoSpaces>(nonJsonNonFirstNonNestedPretty, false, 3);
    EXPECT_STREQ(", ", nonJsonNonFirstNonNestedPretty.str().c_str());

    TestStreamType nonJsonNonFirstNonNestedNonPretty;
    Json::Put::Separator<false, false, false, Json::twoSpaces>(nonJsonNonFirstNonNestedNonPretty, false, 3);
    EXPECT_STREQ(",", nonJsonNonFirstNonNestedNonPretty.str().c_str());
}

struct PlaceholderAnnotations {};

TEST_HEADER(JsonOutputPut, Customization)
{
    using Notate = Annotate<PlaceholderAnnotations>;
    int objectPlaceholder;
    using Field = CustomizeNoSpecialization::Class::integer_::Field;
    CustomizeNoSpecialization customizeNoSpecialization;
    bool customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customizeNoSpecialization, objectPlaceholder);
    EXPECT_FALSE(customized);

    CustomizeFullySpecialized customizeFullySpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customizeFullySpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize5ArgSpecialized customize5ArgSpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customize5ArgSpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize4ArgSpecialized customize4ArgSpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customize4ArgSpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize3ArgSpecialized customize3ArgSpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customize3ArgSpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize2ArgSpecialized customize2ArgSpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customize2ArgSpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize5ArgSpecialized_OpAnnotationsDefaulted customize5ArgSpecialized_opAnnotationsDefaulted;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customize5ArgSpecialized_opAnnotationsDefaulted, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize5ArgSpecialized_FieldIndexDefaulted customize5ArgSpecialized_fieldIndexDefaulted;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customize5ArgSpecialized_fieldIndexDefaulted, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize5ArgSpecialized_BothDefaulted customize5ArgSpecialized_bothDefaulted;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customize5ArgSpecialized_bothDefaulted, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize4ArgSpecialized_FieldIndexDefaulted customize4ArgSpecialized_fieldIndexDefaulted;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, customize4ArgSpecialized_fieldIndexDefaulted, objectPlaceholder);
    EXPECT_TRUE(customized);

    UnspecializedType unspecializedType;
    ContainsUnspecialized containsUnspecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, containsUnspecialized, unspecializedType);
    EXPECT_FALSE(customized);

    FullySpecializedType fullySpecializedType;
    ContainsFullySpecialized containsFullySpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, containsFullySpecialized, fullySpecializedType);
    EXPECT_TRUE(customized);

    ThreeArgSpecializedType threeArgSpecializedType;
    ContainsThreeArgSpecialized containsThreeArgSpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, containsThreeArgSpecialized, threeArgSpecializedType);
    EXPECT_TRUE(customized);

    TwoArgSpecializedType twoArgSpecializedType;
    ContainsTwoArgSpecialized containsTwoArgSpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, containsTwoArgSpecialized, twoArgSpecializedType);
    EXPECT_TRUE(customized);

    OneArgSpecializedType oneArgSpecializedType;
    ContainsOneArgSpecialized containsOneArgSpecialized;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, containsOneArgSpecialized, oneArgSpecializedType);
    EXPECT_TRUE(customized);

    ThreeArgSpecializedType_OpAnnotationsDefaulted threeArgSpecializedType_opAnnotationsDefaulted;
    ContainsThreeArgSpecializedType_OpAnnotationsDefaulted containsThreeArgSpecializedType_opAnnotationDefaulted;
    customized = Json::Put::Customization<Notate, Field, Json::Statics::Included, true, 3, 3, Json::twoSpaces>(
        os, Json::context, containsThreeArgSpecializedType_opAnnotationDefaulted, threeArgSpecializedType_opAnnotationsDefaulted);
    EXPECT_TRUE(customized);
}

TEST_HEADER(JsonOutputPut, String)
{
    TestStreamType one, two, three, four, five, six, seven, eight, nine, ten, eleven;

    Json::Output::Put::String(one, "asdf");
    EXPECT_STREQ("\"asdf\"", one.str().c_str());
    
    Json::Output::Put::String(two, "\"");
    EXPECT_STREQ("\"\\\"\"", two.str().c_str());
    Json::Output::Put::String(three, "\\");
    EXPECT_STREQ("\"\\\\\"", three.str().c_str());
    Json::Output::Put::String(four, "/");
    EXPECT_STREQ("\"\\/\"", four.str().c_str());
    Json::Output::Put::String(five, "\b");
    EXPECT_STREQ("\"\\b\"", five.str().c_str());
    Json::Output::Put::String(six, "\f");
    EXPECT_STREQ("\"\\f\"", six.str().c_str());
    Json::Output::Put::String(seven, "\n");
    EXPECT_STREQ("\"\\n\"", seven.str().c_str());
    Json::Output::Put::String(eight, "\r");
    EXPECT_STREQ("\"\\r\"", eight.str().c_str());
    Json::Output::Put::String(nine, "\t");
    EXPECT_STREQ("\"\\t\"", nine.str().c_str());

    Json::Output::Put::String(ten, "");
    EXPECT_STREQ("\"\"", ten.str().c_str());

    Json::Output::Put::String(eleven, "asdf\"\\/\b\f\n\r\tjkl;");
    EXPECT_STREQ("\"asdf\\\"\\\\\\/\\b\\f\\n\\r\\tjkl;\"", eleven.str().c_str());
}

struct OstreamOverloaded {};

Json::OutStreamType & operator<<(Json::OutStreamType & os, const OstreamOverloaded &)
{
    os << "OstreamOverloaded";
    return os;
}

TEST_HEADER(JsonOutputPut, StringTemplate)
{
    TestStreamType one, two;

    int integer = 1337;
    OstreamOverloaded ostreamOverloaded;
    
    Json::Output::Put::String(one, integer);
    EXPECT_STREQ("\"1337\"", one.str().c_str());

    Json::Output::Put::String(two, ostreamOverloaded);
    EXPECT_STREQ("\"OstreamOverloaded\"", two.str().c_str());
}

TEST_HEADER(JsonOutputPut, GenericValue)
{
    TestStreamType booleanStream,
        numberStream,
        strStream,
        objStream,
        nullArrayStream,
        boolArrayStream,
        numberArrayStream,
        strArrayStream,
        objArrayStream,
        mixedArrayStream,
        fieldClusterStream;

    Json::Bool boolean(true);
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(booleanStream, Json::context, 0, 0, boolean);
    EXPECT_STREQ("true", booleanStream.str().c_str());

    Json::Number number("123.456");
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(numberStream, Json::context, 0, 0, number);
    EXPECT_STREQ("123.456", numberStream.str().c_str());

    Json::String str("asd\nf");
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(strStream, Json::context, 0, 0, str);
    EXPECT_STREQ("\"asd\\nf\"", strStream.str().c_str());

    Json::Object obj;
    obj.put("astr", Json::String::Make("asdf"));
    obj.put("number", Json::Number::Make("1234"));
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(objStream, Json::context, 0, 0, obj);
    EXPECT_STREQ("{\"astr\":\"asdf\",\"number\":1234}", objStream.str().c_str());

    Json::NullArray nullArray(3);
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(nullArrayStream, Json::context, 0, 0, nullArray);
    EXPECT_STREQ("[null,null,null]", nullArrayStream.str().c_str());

    Json::BoolArray boolArray;
    boolArray.boolArray().push_back(true);
    boolArray.boolArray().push_back(true);
    boolArray.boolArray().push_back(false);
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(boolArrayStream, Json::context, 0, 0, boolArray);
    EXPECT_STREQ("[true,true,false]", boolArrayStream.str().c_str());

    Json::NumberArray numberArray;
    numberArray.numberArray().push_back("2");
    numberArray.numberArray().push_back("1");
    numberArray.numberArray().push_back("0.5");
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(numberArrayStream, Json::context, 0, 0, numberArray);
    EXPECT_STREQ("[2,1,0.5]", numberArrayStream.str().c_str());

    Json::StringArray strArray;
    strArray.stringArray().push_back("as");
    strArray.stringArray().push_back("df");
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(strArrayStream, Json::context, 0, 0, strArray);
    EXPECT_STREQ("[\"as\",\"df\"]", strArrayStream.str().c_str());

    Json::ObjectArray objArray;
    Json::ObjectValue objElement;
    objElement.insert(std::pair("astr", Json::String::Make("asdf")));
    objElement.insert(std::pair("number", Json::Number::Make("1234")));
    objArray.objectArray().push_back(objElement);
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(objArrayStream, Json::context, 0, 0, objArray);
    EXPECT_STREQ("[{\"astr\":\"asdf\",\"number\":1234}]", objArrayStream.str().c_str());

    Json::MixedArray mixedArray;
    mixedArray.mixedArray().push_back(Json::Bool::Make(true));
    mixedArray.mixedArray().push_back(nullptr);
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(mixedArrayStream, Json::context, 0, 0, mixedArray);
    EXPECT_STREQ("[true,null]", mixedArrayStream.str().c_str());

    Json::FieldCluster fieldCluster;
    fieldCluster.put("astr", Json::String::Make("asdf"));
    fieldCluster.put("number", Json::Number::Make("1234"));
    Json::Put::GenericValue<Annotate<>, false, Json::twoSpaces, true>(fieldClusterStream, Json::context, 0, 0, fieldCluster);
    EXPECT_STREQ("\"astr\":\"asdf\",\"number\":1234", fieldClusterStream.str().c_str());
}

TEST_HEADER(JsonOutputPut, GenericIterable)
{
    TestStreamType objStream,
        nullArrayStream,
        boolArrayStream,
        numberArrayStream,
        strArrayStream,
        objArrayStream,
        mixedArrayStream;

    Json::Object obj;
    obj.put("astr", Json::String::Make("asdf"));
    obj.put("number", Json::Number::Make("1234"));
    Json::Put::GenericIterable<Annotate<>, false, Json::twoSpaces>(objStream, Json::context, 3, 3, obj);
    EXPECT_STREQ("{\"astr\":\"asdf\",\"number\":1234}", objStream.str().c_str());

    Json::NullArray nullArray(3);
    Json::Put::GenericIterable<Annotate<>, false, Json::twoSpaces>(nullArrayStream, Json::context, 3, 3, nullArray);
    EXPECT_STREQ("[null,null,null]", nullArrayStream.str().c_str());

    Json::BoolArray boolArray;
    boolArray.boolArray().push_back(true);
    boolArray.boolArray().push_back(true);
    boolArray.boolArray().push_back(false);
    Json::Put::GenericIterable<Annotate<>, false, Json::twoSpaces>(boolArrayStream, Json::context, 3, 3, boolArray);
    EXPECT_STREQ("[true,true,false]", boolArrayStream.str().c_str());

    Json::NumberArray numberArray;
    numberArray.numberArray().push_back("2");
    numberArray.numberArray().push_back("1");
    numberArray.numberArray().push_back("0.5");
    Json::Put::GenericIterable<Annotate<>, false, Json::twoSpaces>(numberArrayStream, Json::context, 3, 3, numberArray);
    EXPECT_STREQ("[2,1,0.5]", numberArrayStream.str().c_str());

    Json::StringArray stringArray;
    stringArray.stringArray().push_back("as");
    stringArray.stringArray().push_back("df");
    Json::Put::GenericIterable<Annotate<>, false, Json::twoSpaces>(strArrayStream, Json::context, 3, 3, stringArray);
    EXPECT_STREQ("[\"as\",\"df\"]", strArrayStream.str().c_str());

    Json::ObjectArray objArray;
    Json::ObjectValue objElement;
    objElement.insert(std::pair("astr", Json::String::Make("asdf")));
    objElement.insert(std::pair("number", Json::Number::Make("1234")));
    objArray.objectArray().push_back(objElement);
    Json::Put::GenericIterable<Annotate<>, false, Json::twoSpaces>(objArrayStream, Json::context, 3, 3, objArray);
    EXPECT_STREQ("[{\"astr\":\"asdf\",\"number\":1234}]", objArrayStream.str().c_str());

    Json::MixedArray mixedArray;
    mixedArray.mixedArray().push_back(Json::Bool::Make(true));
    mixedArray.mixedArray().push_back(nullptr);
    Json::Put::GenericIterable<Annotate<>, false, Json::twoSpaces>(mixedArrayStream, Json::context, 3, 3, mixedArray);
    EXPECT_STREQ("[true,null]", mixedArrayStream.str().c_str());
}

enum_t(AnEnum, uint16_t, {
    first,
    second
});

struct AnObject
{
    int first;
    std::string second;
    char third;
};

TEST_HEADER(JsonOutputPut, Value)
{
    TestStreamType customizedStream,
        nullPointerStream,
        intPointerStream,
        genericNumberStream,
        intVectorStream,
        intMapStream,
        rootObjStream,
        reflectedObjStream,
        strStream,
        enumIntStream,
        booleanStream,
        shortIntStream;

    using AField = Fields::Field<>;

    int placeholderObj = 0;
    CustomizeFullySpecialized customized;
    customized.firstField = 1337;
    Json::Put::Value<Annotate<>, CustomizeFullySpecialized::Class::firstField_::Field,
        Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, CustomizeFullySpecialized, true>
        (customizedStream, Json::context, customized, customized.firstField);
    EXPECT_STREQ("\"Customized1337\"", customizedStream.str().c_str());

    int* nullPointer = nullptr;
    Json::Put::Value<Annotate<>, AField, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(nullPointerStream, Json::context, placeholderObj, nullPointer);
    EXPECT_STREQ("null", nullPointerStream.str().c_str());

    int anInt = 555;
    int* intPointer = &anInt;
    Json::Put::Value<Annotate<>, AField, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(intPointerStream, Json::context, placeholderObj, intPointer);
    EXPECT_STREQ("555", intPointerStream.str().c_str());

    Json::Number number("123.456");
    Json::Put::Value<Annotate<>, AField, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(genericNumberStream, Json::context, placeholderObj, number);
    EXPECT_STREQ("123.456", genericNumberStream.str().c_str());

    std::vector<int> intVector = { 1, 2, 3 };
    Json::Put::Value<Annotate<>, AField, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(intVectorStream, Json::context, placeholderObj, intVector);
    EXPECT_STREQ("[1,2,3]", intVectorStream.str().c_str());

    std::map<int, int> intMap = { {1, 2}, {3, 4}, {5, 6} };
    Json::Put::Value<Annotate<>, AField, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(intMapStream, Json::context, placeholderObj, intMap);
    EXPECT_STREQ("{\"1\":2,\"3\":4,\"5\":6}", intMapStream.str().c_str());

    InstanceField rootObj;
    rootObj.a = 5;
    Json::Put::Value<Annotate<>, Fields::Field<InstanceField, void*, 0, Reflected>, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(rootObjStream, Json::context, placeholderObj, rootObj);
    EXPECT_STREQ("{\"a\":5}", rootObjStream.str().c_str());

    InstanceField obj;
    obj.a = 6;
    Json::Put::Value<Annotate<>, Fields::Field<InstanceField, void*, 0, Reflected>, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(reflectedObjStream, Json::context, placeholderObj, obj);
    EXPECT_STREQ("{\"a\":6}", reflectedObjStream.str().c_str());

    std::string str = "asdf";
    Json::Put::Value<Annotate<>, AField, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(strStream, Json::context, placeholderObj, str);
    EXPECT_STREQ("\"asdf\"", strStream.str().c_str());

    AnEnum enumInt = AnEnum::first;
    Json::Put::Value<Annotate<>, Fields::Field<AnEnum, void*, 0, Json::EnumInt>, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(enumIntStream, Json::context, placeholderObj, enumInt);
    EXPECT_STREQ("0", enumIntStream.str().c_str());

    bool boolean = false;
    Json::Put::Value<Annotate<>, AField, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(booleanStream, Json::context, placeholderObj, boolean);
    EXPECT_STREQ("false", booleanStream.str().c_str());

    short shortInt = 22222;
    Json::Put::Value<Annotate<>, AField, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(shortIntStream, Json::context, placeholderObj, shortInt);
    EXPECT_STREQ("22222", shortIntStream.str().c_str());
}

TEST_HEADER(JsonOutputPut, PairValue)
{
    TestStreamType pairValueStream;
    std::pair<std::string, int> pair = std::pair("integer", 555);
    int placeholderObj = 0;
    Json::Put::Value<Annotate<>, Fields::Field<>, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, int, true>(pairValueStream, Json::context, placeholderObj, pair);
    EXPECT_STREQ("\"integer\":555", pairValueStream.str().c_str());
}

struct ContainsIterables
{
    std::vector<int> intVector;
    int intArray[3];
    std::queue<int> intQueue;

    REFLECT(() ContainsIterables, () intVector, () intQueue, () intArray)
};

TEST_HEADER(JsonOutputPut, Iterable)
{
    TestStreamType intVectorStream,
        intQueueStream,
        intArrayStream;

    ContainsIterables containsIterables = {
        { 1, 2, 3 },
        { 3, 4, 5 }
    };
    containsIterables.intQueue.push(2);
    containsIterables.intQueue.push(3);
    containsIterables.intQueue.push(4);

    Json::Put::Iterable<Annotate<>, ContainsIterables::Class::intVector_::Field, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, ContainsIterables>(
        intVectorStream, Json::context, containsIterables, containsIterables.intVector);
    EXPECT_STREQ("[1,2,3]", intVectorStream.str().c_str());

    Json::Put::Iterable<Annotate<>, ContainsIterables::Class::intQueue_::Field, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, ContainsIterables>(
        intQueueStream, Json::context, containsIterables, containsIterables.intQueue);
    EXPECT_STREQ("[2,3,4]", intQueueStream.str().c_str());

    Json::Put::Iterable<Annotate<>, ContainsIterables::Class::intArray_::Field, Json::Statics::Excluded, false, 0, 0, Json::twoSpaces, ContainsIterables>(
        intArrayStream, Json::context, containsIterables, containsIterables.intArray);
    EXPECT_STREQ("[3,4,5]", intArrayStream.str().c_str());
}

struct RegularFields
{
    int regular;
    int regularIgnored;
    Json::FieldCluster regularFieldCluster;

    REFLECT(() RegularFields, () regular, (Json::Ignore) regularIgnored, () regularFieldCluster)
};

struct FieldClusterOnly
{
    Json::FieldCluster onlyFieldCluster;

    REFLECT(() FieldClusterOnly, () onlyFieldCluster)
};

struct FieldClusterFirst
{
    Json::FieldCluster firstFieldCluster;
    int regular;

    REFLECT(() FieldClusterFirst, () firstFieldCluster, () regular)
};

struct FieldClusterPointer
{
    int regular;
    std::unique_ptr<Json::FieldCluster> fieldClusterPointer;
    int otherRegular;

    REFLECT(() FieldClusterPointer, () regular, () fieldClusterPointer, () otherRegular)
};

TEST_HEADER(JsonOutputPut, Field)
{
    TestStreamType putRegular;
    RegularFields regularFields = {1, 2};
    Json::Put::Field<Annotate<>, RegularFields::Class::regular_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegular, Json::context, regularFields, "regular", regularFields.regular);
    EXPECT_STREQ("\"regular\":1", putRegular.str().c_str());

    TestStreamType putRegularIgnored;
    Json::Put::Field<Annotate<>, RegularFields::Class::regularIgnored_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularIgnored, Json::context, regularFields, "regularIgnored", regularFields.regularIgnored);
    EXPECT_STREQ("", putRegularIgnored.str().c_str());

    TestStreamType putRegularEmptyCluster;
    Json::Put::Field<Annotate<>, RegularFields::Class::regularFieldCluster_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularEmptyCluster, Json::context, regularFields, "regularFieldCluster", regularFields.regularFieldCluster);
    EXPECT_STREQ("", putRegularEmptyCluster.str().c_str());

    TestStreamType putRegularCluster;
    regularFields.regularFieldCluster.put("aField", Json::String::Make("aString"));
    Json::Put::Field<Annotate<>, RegularFields::Class::regularFieldCluster_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularCluster, Json::context, regularFields, "regularFieldCluster", regularFields.regularFieldCluster);
    EXPECT_STREQ(",\"aField\":\"aString\"", putRegularCluster.str().c_str());

    TestStreamType putOnlyCluster;
    FieldClusterOnly fieldClusterOnly;
    bool visited = false;
    try {
        Json::Put::Field<Annotate<>, FieldClusterOnly::Class::onlyFieldCluster_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterOnly>(
            putOnlyCluster, Json::context, fieldClusterOnly, "onlyFieldCluster", fieldClusterOnly.onlyFieldCluster);
    } catch ( Json::Exception & ) {
        visited = true;
    }
    EXPECT_TRUE(visited);

    TestStreamType putClusterFirst;
    FieldClusterFirst fieldClusterFirst = {};
    visited = false;
    try {
        Json::Put::Field<Annotate<>, FieldClusterFirst::Class::firstFieldCluster_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterFirst>(
            putOnlyCluster, Json::context, fieldClusterFirst, "firstFieldCluster", fieldClusterFirst.firstFieldCluster);
    } catch ( Json::Exception & ) {
        visited = true;
    }
    EXPECT_TRUE(visited);

    TestStreamType putClusterNullPointer;
    FieldClusterPointer fieldClusterPointer = {};
    fieldClusterPointer.fieldClusterPointer = nullptr;
    Json::Put::Field<Annotate<>, FieldClusterPointer::Class::fieldClusterPointer_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterPointer>(
        putClusterNullPointer, Json::context, fieldClusterPointer, "fieldClusterPointer", fieldClusterPointer.fieldClusterPointer);
    EXPECT_STREQ("", putClusterNullPointer.str().c_str());

    TestStreamType putClusterPointerEmpty;
    fieldClusterPointer.fieldClusterPointer = std::unique_ptr<Json::FieldCluster>(new Json::FieldCluster());
    Json::Put::Field<Annotate<>, FieldClusterPointer::Class::fieldClusterPointer_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterPointer>(
        putClusterPointerEmpty, Json::context, fieldClusterPointer, "fieldClusterPointer", fieldClusterPointer.fieldClusterPointer);
    EXPECT_STREQ("", putClusterPointerEmpty.str().c_str());

    TestStreamType putClusterPointerNonEmpty;
    fieldClusterPointer.fieldClusterPointer->put("one", nullptr);
    fieldClusterPointer.fieldClusterPointer->put("two", Json::Bool::Make(true));
    Json::Put::Field<Annotate<>, FieldClusterPointer::Class::fieldClusterPointer_::Field, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterPointer>(
        putClusterPointerNonEmpty, Json::context, fieldClusterPointer, "fieldClusterPointer", fieldClusterPointer.fieldClusterPointer);
    EXPECT_STREQ(",\"one\":null,\"two\":true", putClusterPointerNonEmpty.str().c_str());
}

TEST_HEADER(JsonOutputPut, Fields)
{
    TestStreamType putRegularFields;
    RegularFields regularFields = { 1, 2 };
    Json::Put::Fields<Annotate<>, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularFields, Json::context, regularFields);
    EXPECT_STREQ("\"regular\":1", putRegularFields.str().c_str());

    TestStreamType putMultipleRegularFields;
    regularFields.regularFieldCluster.put("unknown", Json::String::Make("field"));
    Json::Put::Fields<Annotate<>, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putMultipleRegularFields, Json::context, regularFields);
    EXPECT_STREQ("\"regular\":1,\"unknown\":\"field\"", putMultipleRegularFields.str().c_str());
}

TEST_HEADER(JsonOutputPut, SuperTest)
{
    TestStreamType putNoSupers;
    NestedEmptySuper emptySuper;
    Json::Put::Super<Annotate<>, 0, EmptySuper, Json::Statics::Excluded, false, 0, Json::twoSpaces, NestedEmptySuper>(
        putNoSupers, Json::context, emptySuper, "__EmptySuper");
    EXPECT_STREQ("", putNoSupers.str().c_str());

    TestStreamType putFieldedSuper;
    NestedFieldedSuper nestedFieldedSuper;
    nestedFieldedSuper.a = 555;
    Json::Put::Super<Annotate<>, 0, FieldedSuper, Json::Statics::Excluded, false, 0, Json::twoSpaces, NestedFieldedSuper>(
        putFieldedSuper, Json::context, nestedFieldedSuper, "__FieldedSuper");
    EXPECT_TRUE(std::regex_match(putFieldedSuper.str().c_str(), std::regex("\"__.*FieldedSuper\":\\{\"a\":555\\}")));

    TestStreamType putDoublyNestedSuper;
    DoublyNestedFieldedSuper doublyNestedFieldedSuper;
    doublyNestedFieldedSuper.a = 444;
    Json::Put::Super<Annotate<>, 0, NestedFieldedSuper, Json::Statics::Excluded, false, 0, Json::twoSpaces, DoublyNestedFieldedSuper>(
        putDoublyNestedSuper, Json::context, doublyNestedFieldedSuper, "__NestedFieldedSuper");
    EXPECT_TRUE(std::regex_match(putDoublyNestedSuper.str().c_str(), std::regex("\"__.*NestedFieldedSuper\":\\{\"__.*FieldedSuper\":\\{\"a\":444\\}\\}")));
}

TEST_HEADER(JsonOutputPut, SupersTest)
{
    TestStreamType putNoSupers;
    EmptySuper emptySuper;
    Json::Put::Supers<Annotate<>, Json::Statics::Included, false, 0, Json::twoSpaces, EmptySuper>(putNoSupers, Json::context, emptySuper);
    EXPECT_STREQ("", putNoSupers.str().c_str());

    TestStreamType putEmptySupers;
    NestedEmptySuper nestedEmptySuper;
    Json::Put::Supers<Annotate<>, Json::Statics::Included, false, 0, Json::twoSpaces, NestedEmptySuper>(putEmptySupers, Json::context, nestedEmptySuper);
    EXPECT_STREQ("", putEmptySupers.str().c_str());

    TestStreamType putDoubleSuper;
    DoubleSuper doubleSuper;
    doubleSuper.a = 111;
    Json::Put::Supers<Annotate<>, Json::Statics::Included, false, 0, Json::twoSpaces, DoubleSuper>(putDoubleSuper, Json::context, doubleSuper);
    EXPECT_TRUE(std::regex_match(putDoubleSuper.str().c_str(), std::regex("\"__.*FieldedSuper\":\\{\"a\":111\\}")));

    TestStreamType putTripleSuper;
    TripleSuper tripleSuper;
    tripleSuper.a = 222;
    Json::Put::Supers<Annotate<>, Json::Statics::Included, false, 0, Json::twoSpaces, TripleSuper>(putTripleSuper, Json::context, tripleSuper);
    EXPECT_TRUE(std::regex_match(putTripleSuper.str().c_str(), std::regex("\"__.*FieldedSuper\":\\{\"a\":222\\}")));
}

struct NestedObj
{
    bool boolean;
    int ray[3];

    REFLECT(() NestedObj, () boolean, () ray)
};

struct AnObjectTest
{
    int integer;
    std::string str;
    NestedObj nestedObj;

    REFLECT(() AnObjectTest, () integer, () str, (Reflected) nestedObj)
};

TEST_HEADER(JsonOutputPut, Object)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    TestStreamType objStream;
    Json::Put::Object<Annotate<>, Json::Statics::Included, false, 0, Json::twoSpaces, AnObjectTest>(objStream, Json::context, anObject);
    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"boolean\":false,\"ray\":[1,2,3]}}", objStream.str().c_str());
}

TEST_HEADER(JsonOutputTest, JsonOutputReflectedObject)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };
    Json::Output::ReflectedObject<Annotate<>, Statics::Included, false, 0, Json::twoSpaces, AnObjectTest> reflectedObj(anObject, nullptr);
    TestStreamType objStream;
    reflectedObj.put(objStream);

    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"boolean\":false,\"ray\":[1,2,3]}}", objStream.str().c_str());
}

TEST_HEADER(JsonOutputTest, JsonOut)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };
    auto reflectedObj = Json::out(anObject);
    TestStreamType objStream;
    reflectedObj.put(objStream);
    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"boolean\":false,\"ray\":[1,2,3]}}", objStream.str().c_str());

    TestStreamType finalObjStream;
    finalObjStream << Json::out(anObject);
    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"boolean\":false,\"ray\":[1,2,3]}}", finalObjStream.str().c_str());
}

struct JsonReferences
{
    JsonReferences() : primitive(11), primitiveReference(primitive) {}

    int primitive;
    int & primitiveReference;
    static int staticPrimitive;
    static int & staticPrimitiveReference;

    REFLECT(() JsonReferences, () primitive, () primitiveReference, () staticPrimitive, () staticPrimitiveReference)
};

int JsonReferences::staticPrimitive = 33;
int & JsonReferences::staticPrimitiveReference = JsonReferences::staticPrimitive;

TEST_HEADER(JsonOutputTest, JsonOutReferences)
{
    JsonReferences jsonReferences;
    TestStreamType objStream;
    objStream << Json::out<Statics::Included>(jsonReferences);
    EXPECT_STREQ("{\"primitive\":11,\"primitiveReference\":11,\"staticPrimitive\":33,\"staticPrimitiveReference\":33}", objStream.str().c_str());
}

TEST_HEADER(JsonOutputTest, JsonPretty)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };
    TestStreamType objStreamCompare;
    objStreamCompare
        << "{" << osEndl
        << "  \"integer\": 4," << osEndl
        << "  \"str\": \"aString\"," << osEndl
        << "  \"nestedObj\": {" << osEndl
        << "    \"boolean\": false," << osEndl
        << "    \"ray\": [ 1, 2, 3 ]" << osEndl
        << "  }" << osEndl
        << "}";

    auto reflectedObj = Json::pretty(anObject);
    TestStreamType objStream;
    reflectedObj.put(objStream);
    EXPECT_STREQ(objStreamCompare.str().c_str(), objStream.str().c_str());

    TestStreamType finalObjStream;
    finalObjStream << Json::pretty(anObject);
    EXPECT_STREQ(objStreamCompare.str().c_str(), finalObjStream.str().c_str());
}

TEST_HEADER(JsonOutputTest, Performance_1000_StringStream)
{
#ifdef RUN_PERFORMANCE_TESTS
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    TestStreamType ss;
    for ( size_t i=0; i<1000; i++ )
        ss << Json::out(anObject);
#endif
}

TEST_HEADER(JsonOutputTest, Performance_1000_StringBuffer)
{
#ifdef RUN_PERFORMANCE_TESTS
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    StringBuffer sb;
    for ( size_t i=0; i<1000; i++ )
        sb << Json::out(anObject);
#endif
}

TEST_HEADER(JsonOutputTest, Performance_10000_StringStream)
{
#ifdef RUN_PERFORMANCE_TESTS
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    TestStreamType ss;
    for ( size_t i=0; i<10000; i++ )
        ss << Json::out(anObject);
#endif
}

TEST_HEADER(JsonOutputTest, Performance_10000_StringBuffer)
{
#ifdef RUN_PERFORMANCE_TESTS
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    StringBuffer sb;
    for ( size_t i=0; i<10000; i++ )
        sb << Json::out(anObject);
#endif
}

TEST_HEADER(JsonOutputTest, Performance_100000_StringStream)
{
#ifdef RUN_PERFORMANCE_TESTS
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    TestStreamType ss;
    for ( size_t i=0; i<100000; i++ )
        ss << Json::out(anObject);
#endif
}

TEST_HEADER(JsonOutputTest, Performance_100000_StringBuffer)
{
#ifdef RUN_PERFORMANCE_TESTS
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    StringBuffer sb;
    for ( size_t i=0; i<100000; i++ )
        sb << Json::out(anObject);
#endif
}

TEST_HEADER(JsonOutputTest, Performance_1000000_StringStream)
{
#ifdef RUN_PERFORMANCE_TESTS
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    TestStreamType ss;
    for ( size_t i=0; i<1000000; i++ )
        ss << Json::out(anObject);
#endif
}

TEST_HEADER(JsonOutputTest, Performance_1000000_StringBuffer)
{
#ifdef RUN_PERFORMANCE_TESTS
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    StringBuffer sb;
    for ( size_t i=0; i<1000000; i++ )
        sb << Json::out(anObject);
#endif
}
}

#undef osEndl

#endif