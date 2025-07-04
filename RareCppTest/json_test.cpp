#ifdef GET_RUNJSONTESTSRC_INCLUDES
#include "json_test.h"
#include <gtest/gtest.h>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <queue>
#include <regex>
#include <stack>
#include <unordered_set>
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

//#define RUN_PERFORMANCE_TESTS

namespace TestDefinitions
{
Json::OutStreamType & operator <<(Json::OutStreamType & oss, const CustomizeFullySpecialized &)
{
    oss << "Unspecialized!";
    return oss;
}

TEST_HEADER(JsonSharedTest, MatchesStatics)
{
    bool matches = Json::matches_statics<false, Json::StaticType<Statics::Excluded>>::value;
    EXPECT_TRUE(matches);
    matches = Json::matches_statics<false, Json::StaticType<Statics::Included>>::value;
    EXPECT_TRUE(matches);
    matches = Json::matches_statics<false, Json::StaticType<Statics::Only>>::value;
    EXPECT_FALSE(matches);
    matches = Json::matches_statics<true, Json::StaticType<Statics::Excluded>>::value;
    EXPECT_FALSE(matches);
    matches = Json::matches_statics<true, Json::StaticType<Statics::Included>>::value;
    EXPECT_TRUE(matches);
    matches = Json::matches_statics<true, Json::StaticType<Statics::Only>>::value;
    EXPECT_TRUE(matches);
}

struct NoFields { REFLECT(NoFields) };
struct InstanceField { int a; REFLECT(InstanceField, a) };
struct StaticField { static int a; REFLECT(StaticField, a) };
struct BothFields { int a; static int b; REFLECT(BothFields, a, b) };
struct Functions {
    void a() {}
    static void b() {}
    REFLECT(Functions, a, b)
};
struct MoreInstance { int a; int b; static int c; REFLECT(MoreInstance, a, b, c) };
struct MoreStatic { int a; static int b; static int c; REFLECT(MoreStatic, a, b, c) };
int StaticField::a = 0;
int BothFields::b = 0;
int MoreInstance::c = 0;
int MoreStatic::b = 0;
int MoreStatic::c = 0;

struct IgnoreInstanceField {
    NOTE(a, Json::Ignore)
    int a;
    REFLECT(IgnoreInstanceField, a)
};
struct IgnoreStaticField {
    NOTE(a, Json::Ignore)
    static int a;
    REFLECT(IgnoreStaticField, a)
};
struct IgnoreBothField {
    NOTE(a, Json::Ignore)
    int a;
    NOTE(b, Json::Ignore)
    static int b;
    REFLECT(IgnoreBothField, a, b)
};
struct IgnoreMixed {
    NOTE(a, Json::Ignore)
    int a;
    int b;
    NOTE(c, Json::Ignore)
    int c;
    static int d;
    static int e;
    NOTE(f, Json::Ignore)
    static int f;
    void g() {}
    static void h() {}
    REFLECT(IgnoreMixed, a, b, c, d, e, f, g, h)
};
struct OnlyIgnoreStatics {
    int a;
    NOTE(b, Json::Ignore)
    static int b;
    REFLECT(OnlyIgnoreStatics, a, b)
};
struct OnlyIgnoreInstance {
    NOTE(a, Json::Ignore)
    int a;
    static int b;
    REFLECT(OnlyIgnoreInstance, a, b)
};
int IgnoreStaticField::a = 0;
int IgnoreBothField::b = 0;
int IgnoreMixed::d = 0;
int IgnoreMixed::e = 0;
int IgnoreMixed::f = 0;
int OnlyIgnoreStatics::b = 0;
int OnlyIgnoreInstance::b= 0;

NOTE(EmptySuper)
struct EmptySuper { REFLECT_NOTED(EmptySuper) };
NOTE(OtherEmptySuper)
struct OtherEmptySuper { REFLECT_NOTED(OtherEmptySuper) };
struct FieldedSuper { int a; REFLECT(FieldedSuper, a) };
NOTE(NestedEmptySuper, Super<EmptySuper>)
struct NestedEmptySuper : EmptySuper { REFLECT_NOTED(NestedEmptySuper) };
NOTE(NestedFieldedSuper, Super<FieldedSuper>)
struct NestedFieldedSuper : FieldedSuper { REFLECT_NOTED(NestedFieldedSuper) };
NOTE(DoublyNestedEmptySuper, Super<NestedEmptySuper>)
struct DoublyNestedEmptySuper : NestedEmptySuper { REFLECT_NOTED(DoublyNestedEmptySuper) };
NOTE(DoublyNestedFieldedSuper, Super<NestedFieldedSuper>)
struct DoublyNestedFieldedSuper : NestedFieldedSuper { REFLECT_NOTED(DoublyNestedFieldedSuper) };
NOTE(DoubleSuper, Super<EmptySuper>, Super<FieldedSuper>(Json::Name{"Fielded"}))
struct DoubleSuper : EmptySuper, FieldedSuper { REFLECT_NOTED(DoubleSuper) };
NOTE(TripleSuper, Super<EmptySuper>, Super<OtherEmptySuper>, Super<FieldedSuper>)
struct TripleSuper : EmptySuper, OtherEmptySuper, FieldedSuper { REFLECT_NOTED(TripleSuper) };

TEST_HEADER(JsonSharedTest, HasFields)
{
    bool hasFields = Json::hasFields<Statics::Excluded, NoFields>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, InstanceField>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, StaticField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, BothFields>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, Functions>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, MoreInstance>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, MoreStatic>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, IgnoreInstanceField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, IgnoreStaticField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, IgnoreBothField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, IgnoreMixed>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, OnlyIgnoreStatics>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Excluded, OnlyIgnoreInstance>();
    EXPECT_FALSE(hasFields);

    hasFields = Json::hasFields<Statics::Included, NoFields>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Included, InstanceField>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, StaticField>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, BothFields>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, Functions>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Included, MoreInstance>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, MoreStatic>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, IgnoreInstanceField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Included, IgnoreStaticField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Included, IgnoreBothField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Included, IgnoreMixed>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, OnlyIgnoreStatics>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, OnlyIgnoreInstance>();
    EXPECT_TRUE(hasFields);

    hasFields = Json::hasFields<Statics::Only, NoFields>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Only, InstanceField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Only, StaticField>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Only, BothFields>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Only, Functions>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Only, MoreInstance>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Only, MoreStatic>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Only, IgnoreInstanceField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Only, IgnoreStaticField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Only, IgnoreBothField>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Only, IgnoreMixed>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Only, OnlyIgnoreStatics>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Only, OnlyIgnoreInstance>();
    EXPECT_TRUE(hasFields);
    
    hasFields = Json::hasFields<Statics::Included, EmptySuper>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Included, FieldedSuper>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, NestedEmptySuper>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Included, NestedFieldedSuper>();
    EXPECT_TRUE(hasFields);
    hasFields = Json::hasFields<Statics::Included, DoublyNestedEmptySuper>();
    EXPECT_FALSE(hasFields);
    hasFields = Json::hasFields<Statics::Included, DoublyNestedFieldedSuper>();
}

TEST_HEADER(JsonSharedTest, firstIndex)
{
    size_t firstIndex = Json::firstIndex<Statics::Excluded, NoFields>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, InstanceField>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, StaticField>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, BothFields>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, Functions>();
    EXPECT_EQ(size_t(2), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, MoreInstance>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, MoreStatic>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, IgnoreInstanceField>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, IgnoreStaticField>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, IgnoreBothField>();
    EXPECT_EQ(size_t(2), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, IgnoreMixed>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, OnlyIgnoreStatics>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Excluded, OnlyIgnoreInstance>();
    EXPECT_EQ(size_t(2), firstIndex);

    firstIndex = Json::firstIndex<Statics::Included, NoFields>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, InstanceField>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, StaticField>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, BothFields>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, Functions>();
    EXPECT_EQ(size_t(2), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, MoreInstance>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, MoreStatic>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, IgnoreInstanceField>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, IgnoreStaticField>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, IgnoreBothField>();
    EXPECT_EQ(size_t(2), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, IgnoreMixed>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, OnlyIgnoreStatics>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Included, OnlyIgnoreInstance>();
    EXPECT_EQ(size_t(1), firstIndex);

    firstIndex = Json::firstIndex<Statics::Only, NoFields>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, InstanceField>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, StaticField>();
    EXPECT_EQ(size_t(0), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, BothFields>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, Functions>();
    EXPECT_EQ(size_t(2), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, MoreInstance>();
    EXPECT_EQ(size_t(2), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, MoreStatic>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, IgnoreInstanceField>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, IgnoreStaticField>();
    EXPECT_EQ(size_t(1), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, IgnoreBothField>();
    EXPECT_EQ(size_t(2), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, IgnoreMixed>();
    EXPECT_EQ(size_t(3), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, OnlyIgnoreStatics>();
    EXPECT_EQ(size_t(2), firstIndex);
    firstIndex = Json::firstIndex<Statics::Only, OnlyIgnoreInstance>();
    EXPECT_EQ(size_t(1), firstIndex);
}

TEST_HEADER(JsonSharedTest, FirstSuperIndex)
{
    size_t firstSuperIndex = Json::firstSuperIndex<Json::Statics::Included, EmptySuper>();
    EXPECT_EQ(size_t(0), firstSuperIndex);
    firstSuperIndex = Json::firstSuperIndex<Json::Statics::Included, DoubleSuper>();
    EXPECT_EQ(size_t(1), firstSuperIndex);
    firstSuperIndex = Json::firstSuperIndex<Json::Statics::Included, TripleSuper>();
    EXPECT_EQ(size_t(2), firstSuperIndex);
}

TEST_HEADER(JsonSharedTest, Exception)
{
    const char test[] = "test";
    
    try {
        throw Json::Exception(test);
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
    EXPECT_EQ(size_t(0), ctor.index);
    EXPECT_EQ(Json::JsonField::Type::Regular, ctor.type);
    EXPECT_STREQ("", ctor.name.c_str());

    size_t memberIndex = 1337;
    Json::JsonField::Type type = Json::JsonField::Type::SuperClass;
    std::string fieldName = "asdf";

    Json::JsonField other = Json::JsonField(memberIndex, type, fieldName);
    EXPECT_EQ(memberIndex, other.index);
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

    auto ctorMake = std::make_shared<Json::Bool>();
    EXPECT_FALSE(ctorMake->boolean());

    auto valueMake = std::make_shared<Json::Bool>(true);
    EXPECT_TRUE(valueMake->boolean());

    auto copyMake = std::make_shared<Json::Bool>(valueConstruct);
    EXPECT_TRUE(copyMake->boolean());

    Json::Bool other(false);
    EXPECT_FALSE(other.boolean());
    other = Json::Bool(valueConstruct);
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

    auto ctorMake = std::make_shared<Json::Number>();
    EXPECT_STREQ("0", ctorMake->number().c_str());

    const char charArrayParam[] = "qwerty";
    auto charArrayMake = std::make_shared<Json::Number>(charArrayParam);
    EXPECT_STREQ("qwerty", charArrayMake->number().c_str());

    const char* charPtrParam = &charArrayParam[0];
    auto charPtrMake = std::make_shared<Json::Number>(charPtrParam);
    EXPECT_STREQ("qwerty", charPtrMake->number().c_str());

    const std::string strParam = "uiop";
    auto stringMake = std::make_shared<Json::Number>(strParam);
    EXPECT_STREQ("uiop", stringMake->number().c_str());

    auto constCharMake = std::make_shared<Json::Number>("66.7");
    EXPECT_STREQ("66.7", constCharMake->number().c_str());

    auto valueMake = std::make_shared<Json::Number>(numericValue);
    EXPECT_STREQ(numericValue.c_str(), valueMake->number().c_str());

    auto copyMake = std::make_shared<Json::Number>(valueConstruct);
    EXPECT_STREQ(numericValue.c_str(), copyMake->number().c_str());

    short number = 553;
    auto numberCastMake = std::make_shared<Json::Number>(number);
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

    auto ctorMake = std::make_shared<Json::String>();
    EXPECT_STREQ("", ctorMake->string().c_str());

    auto valueMake = std::make_shared<Json::String>(stringValue);
    EXPECT_STREQ(stringValue.c_str(), valueMake->string().c_str());

    auto copyMake = std::make_shared<Json::String>(valueConstruct);
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
    value.insert(std::pair("first", std::make_shared<Json::Bool>(false)));
    value.insert(std::pair("second", std::make_shared<Json::String>("asdf")));
    Json::Object objectValue(value);
    auto first = objectValue.object().find("first");
    auto second = objectValue.object().find("second");
    EXPECT_STREQ("first", first->first.c_str());
    EXPECT_STREQ("second", second->first.c_str());
    EXPECT_FALSE(first->second->boolean());
    EXPECT_STREQ("asdf", second->second->string().c_str());

    auto ctorMake = std::make_shared<Json::Object>();
    EXPECT_TRUE(ctorMake->object().empty());

    auto valueMake = std::make_shared<Json::Object>(value);
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
    EXPECT_EQ(size_t(0), nullArray.arraySize());
    EXPECT_EQ(size_t(0), nullArray.nullArray());

    nullArray.nullArray()++;
    EXPECT_EQ(size_t(1), nullArray.arraySize());
    EXPECT_EQ(size_t(1), nullArray.nullArray());
    nullArray.nullArray()++;
    EXPECT_EQ(size_t(2), nullArray.arraySize());
    EXPECT_EQ(size_t(2), nullArray.nullArray());

    Json::NullArray valueConstruct(5);
    EXPECT_EQ(size_t(5), valueConstruct.arraySize());
    EXPECT_EQ(size_t(5), valueConstruct.nullArray());

    Json::NullArray copyConstruct(nullArray);
    EXPECT_EQ(size_t(2), copyConstruct.arraySize());
    EXPECT_EQ(size_t(2), copyConstruct.nullArray());

    auto ctorMake = std::make_shared<Json::NullArray>();
    EXPECT_EQ(size_t(0), ctorMake->arraySize());
    EXPECT_EQ(size_t(0), ctorMake->nullArray());

    auto valueMake = std::make_shared<Json::NullArray>(size_t(5));
    EXPECT_EQ(size_t(5), valueMake->arraySize());
    EXPECT_EQ(size_t(5), valueMake->nullArray());

    auto copyMake = std::make_shared<Json::NullArray>(nullArray);
    EXPECT_EQ(size_t(2), copyMake->arraySize());
    EXPECT_EQ(size_t(2), copyMake->nullArray());

    nullArray = valueConstruct;
    EXPECT_EQ(size_t(5), nullArray.arraySize());
    EXPECT_EQ(size_t(5), nullArray.nullArray());

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
    EXPECT_EQ(size_t(0), ctor.arraySize());
    EXPECT_TRUE(ctor.boolArray().empty());

    std::vector<bool> values;
    values.push_back(true);
    values.push_back(true);
    values.push_back(false);
    Json::BoolArray valueConstruct(values);
    EXPECT_EQ(size_t(3), valueConstruct.arraySize());
    EXPECT_EQ(true, valueConstruct.boolArray()[0]);
    EXPECT_EQ(true, valueConstruct.boolArray()[1]);
    EXPECT_EQ(false, valueConstruct.boolArray()[2]);

    Json::BoolArray copyConstruct(values);
    EXPECT_EQ(size_t(3), copyConstruct.arraySize());
    EXPECT_EQ(true, copyConstruct.boolArray()[0]);
    EXPECT_EQ(true, copyConstruct.boolArray()[1]);
    EXPECT_EQ(false, copyConstruct.boolArray()[2]);

    auto ctorMake = std::make_shared<Json::BoolArray>();
    EXPECT_EQ(size_t(0), ctorMake->arraySize());
    EXPECT_TRUE(ctorMake->boolArray().empty());

    auto valueMake = std::make_shared<Json::BoolArray>(values);
    EXPECT_EQ(size_t(3), valueMake->arraySize());
    EXPECT_EQ(true, valueMake->boolArray()[0]);
    EXPECT_EQ(true, valueMake->boolArray()[1]);
    EXPECT_EQ(false, valueMake->boolArray()[2]);

    auto copyMake = std::make_shared<Json::BoolArray>(valueConstruct);
    EXPECT_EQ(size_t(3), copyMake->arraySize());
    EXPECT_EQ(true, copyMake->boolArray()[0]);
    EXPECT_EQ(true, copyMake->boolArray()[1]);
    EXPECT_EQ(false, copyMake->boolArray()[2]);

    Json::BoolArray other;
    other = valueConstruct;
    EXPECT_EQ(size_t(3), other.arraySize());
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
    EXPECT_EQ(size_t(0), ctor.arraySize());

    std::vector<std::string> values;
    values.push_back("0");
    values.push_back("1");
    values.push_back("45");
    Json::NumberArray valueConstruct(values);
    EXPECT_EQ(size_t(3), valueConstruct.arraySize());
    EXPECT_STREQ("0", valueConstruct.numberArray()[0].c_str());
    EXPECT_STREQ("1", valueConstruct.numberArray()[1].c_str());
    EXPECT_STREQ("45", valueConstruct.numberArray()[2].c_str());

    Json::NumberArray copyConstruct(valueConstruct);
    EXPECT_EQ(size_t(3), copyConstruct.arraySize());
    EXPECT_STREQ("0", copyConstruct.numberArray()[0].c_str());
    EXPECT_STREQ("1", copyConstruct.numberArray()[1].c_str());
    EXPECT_STREQ("45", copyConstruct.numberArray()[2].c_str());

    auto ctorMake = std::make_shared<Json::NumberArray>();
    EXPECT_TRUE(ctorMake->numberArray().empty());
    EXPECT_EQ(size_t(0), ctorMake->arraySize());

    auto valueMake = std::make_shared<Json::NumberArray>(values);
    EXPECT_EQ(size_t(3), valueMake->arraySize());
    EXPECT_STREQ("0", valueMake->numberArray()[0].c_str());
    EXPECT_STREQ("1", valueMake->numberArray()[1].c_str());
    EXPECT_STREQ("45", valueMake->numberArray()[2].c_str());

    auto copyMake = std::make_shared<Json::NumberArray>(values);
    EXPECT_EQ(size_t(3), copyMake->arraySize());
    EXPECT_STREQ("0", copyMake->numberArray()[0].c_str());
    EXPECT_STREQ("1", copyMake->numberArray()[1].c_str());
    EXPECT_STREQ("45", copyMake->numberArray()[2].c_str());

    Json::NumberArray other = values;
    EXPECT_EQ(size_t(3), other.arraySize());
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
    EXPECT_EQ(size_t(0), ctor.arraySize());
    EXPECT_TRUE(ctor.stringArray().empty());

    std::vector<std::string> values;
    values.push_back("asdf");
    values.push_back("some string");
    values.push_back("1234");
    Json::StringArray valueConstruct(values);
    EXPECT_EQ(size_t(3), valueConstruct.arraySize());
    EXPECT_STREQ("asdf", valueConstruct.stringArray()[0].c_str());
    EXPECT_STREQ("some string", valueConstruct.stringArray()[1].c_str());
    EXPECT_STREQ("1234", valueConstruct.stringArray()[2].c_str());

    Json::StringArray copyConstruct(valueConstruct);
    EXPECT_EQ(size_t(3), copyConstruct.arraySize());
    EXPECT_STREQ("asdf", copyConstruct.stringArray()[0].c_str());
    EXPECT_STREQ("some string", copyConstruct.stringArray()[1].c_str());
    EXPECT_STREQ("1234", copyConstruct.stringArray()[2].c_str());

    auto ctorMake = std::make_shared<Json::StringArray>();
    EXPECT_EQ(size_t(0), ctorMake->arraySize());
    EXPECT_TRUE(ctorMake->stringArray().empty());

    auto valueMake = std::make_shared<Json::StringArray>(values);
    EXPECT_EQ(size_t(3), valueMake->arraySize());
    EXPECT_STREQ("asdf", valueMake->stringArray()[0].c_str());
    EXPECT_STREQ("some string", valueMake->stringArray()[1].c_str());
    EXPECT_STREQ("1234", valueMake->stringArray()[2].c_str());

    auto copyMake = std::make_shared<Json::StringArray>(valueConstruct);
    EXPECT_EQ(size_t(3), copyMake->arraySize());
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
    EXPECT_EQ(size_t(0), ctor.arraySize());
    EXPECT_TRUE(ctor.objectArray().empty());

    std::vector<std::map<std::string, std::shared_ptr<Json::Value>>> objects;
    std::map<std::string, std::shared_ptr<Json::Value>> firstObj;
    std::map<std::string, std::shared_ptr<Json::Value>> secondObj;
    std::map<std::string, std::shared_ptr<Json::Value>> thirdObj;
    firstObj.insert(std::pair("firstOne", std::make_shared<Json::Bool>(false)));
    firstObj.insert(std::pair("firstTwo", std::make_shared<Json::Number>("1234")));
    firstObj.insert(std::pair("firstThree", std::make_shared<Json::String>("asdf")));
    thirdObj.insert(std::pair("thirdThree", std::make_shared<Json::Object>()));
    objects.push_back(firstObj);
    objects.push_back(secondObj);
    objects.push_back(thirdObj);
    
    Json::ObjectArray valueConstruct(objects);
    EXPECT_EQ(size_t(3), valueConstruct.arraySize());
    EXPECT_EQ(size_t(3), valueConstruct.objectArray().size());
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
    EXPECT_EQ(size_t(3), copyConstruct.arraySize());
    EXPECT_EQ(size_t(3), copyConstruct.objectArray().size());
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

    auto ctorMake = std::make_shared<Json::ObjectArray>();
    EXPECT_EQ(size_t(0), ctorMake->arraySize());
    EXPECT_TRUE(ctorMake->objectArray().empty());

    auto valueMake = std::make_shared<Json::ObjectArray>(objects);
    EXPECT_EQ(size_t(3), valueMake->arraySize());
    EXPECT_EQ(size_t(3), valueMake->objectArray().size());
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

    auto copyMake = std::make_shared<Json::ObjectArray>(valueConstruct);
    EXPECT_EQ(size_t(3), copyMake->arraySize());
    EXPECT_EQ(size_t(3), copyMake->objectArray().size());
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
    EXPECT_EQ(size_t(3), other.arraySize());
    EXPECT_EQ(size_t(3), other.objectArray().size());
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
    EXPECT_EQ(size_t(0), ctor.arraySize());
    EXPECT_TRUE(ctor.mixedArray().empty());

    std::vector<std::shared_ptr<Json::Value>> nestedMixedArrayValues = { nullptr, std::make_shared<Json::Number>("1234") };
    std::shared_ptr<Json::Value> nestedMixedArrayValue = std::make_shared<Json::MixedArray>(nestedMixedArrayValues);

    std::vector<bool> nestedBoolArrayValues = { true, false, true };
    std::shared_ptr<Json::Value> nestedBoolArrayValue = std::make_shared<Json::BoolArray>(nestedBoolArrayValues);

    std::shared_ptr<Json::Object> objValue = std::make_shared<Json::Object>();
    objValue->put("some", std::make_shared<Json::String>("field"));
    objValue->put("someother", std::make_shared<Json::String>("field"));
    
    std::vector<std::shared_ptr<Json::Value>> values = { nestedMixedArrayValue, nestedBoolArrayValue, objValue };
    Json::MixedArray valueConstruct(values);
    EXPECT_EQ(size_t(3), valueConstruct.arraySize());
    EXPECT_EQ(size_t(3), valueConstruct.mixedArray().size());
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
    EXPECT_EQ(size_t(3), copyConstruct.arraySize());
    EXPECT_EQ(size_t(3), copyConstruct.mixedArray().size());
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

    auto ctorMake = std::make_shared<Json::MixedArray>();
    EXPECT_EQ(size_t(0), ctorMake->arraySize());
    EXPECT_TRUE(ctorMake->mixedArray().empty());
    
    auto valueMake = std::make_shared<Json::MixedArray>(values);
    EXPECT_EQ(size_t(3), valueMake->arraySize());
    EXPECT_EQ(size_t(3), valueMake->mixedArray().size());
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

    auto copyMake = std::make_shared<Json::MixedArray>(valueConstruct);
    EXPECT_EQ(size_t(3), copyMake->arraySize());
    EXPECT_EQ(size_t(3), copyMake->mixedArray().size());
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
    EXPECT_EQ(size_t(3), other.arraySize());
    EXPECT_EQ(size_t(3), other.mixedArray().size());
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
    value.insert(std::pair("first", std::make_shared<Json::Bool>(false)));
    value.insert(std::pair("second", std::make_shared<Json::String>("asdf")));
    Json::FieldCluster fieldClusterValue(value);
    auto first = fieldClusterValue.object().find("first");
    auto second = fieldClusterValue.object().find("second");
    EXPECT_STREQ("first", first->first.c_str());
    EXPECT_STREQ("second", second->first.c_str());
    EXPECT_FALSE(first->second->boolean());
    EXPECT_STREQ("asdf", second->second->string().c_str());

    auto ctorMake = std::make_shared<Json::FieldCluster>();
    EXPECT_TRUE(ctorMake->object().empty());

    auto valueMake = std::make_shared<Json::FieldCluster>(value);
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

    Json::Value::Assigner falseBool(std::make_unique<Json::Bool>(false));
    EXPECT_FALSE(falseBool.get()->boolean());
    Json::Value::Assigner trueBool = Json::Value::Assigner(std::make_unique<Json::Bool>(true));
    EXPECT_TRUE(trueBool.get()->boolean());
    
    int genericValueInstance = 0;
    EXPECT_THROW(nullAssigner.into(genericValueInstance), Json::Exception);

    Json::Bool someValue = Json::Bool(true);
    EXPECT_THROW(nullAssigner.into(someValue), Json::NullUnassignable);

    Json::Value* genericValuePtr = (Json::Value*)&someValue;
    nullAssigner.into(genericValuePtr);
    EXPECT_TRUE(genericValuePtr == nullptr);

    std::unique_ptr<Json::Value> genericValueUniquePtr = std::make_unique<Json::Bool>(false);
    nullAssigner.into(genericValueUniquePtr);
    EXPECT_TRUE(genericValueUniquePtr == nullptr);

    std::shared_ptr<Json::Value> genericValueSharedPtr = std::make_shared<Json::Bool>(true);
    nullAssigner.into(genericValueSharedPtr);
    EXPECT_TRUE(genericValueSharedPtr == nullptr);

    // All cases where allocatedValue != nullptr and type matches
    Json::Value::Assigner boolInstanceAssigner(std::make_unique<Json::Bool>(true));
    Json::Bool boolInstanceReceivable;
    boolInstanceAssigner.into(boolInstanceReceivable);
    EXPECT_TRUE(boolInstanceReceivable.boolean());

    Json::Value::Assigner boolPtrAssigner(std::make_unique<Json::Bool>(true));
    Json::Bool* boolPtrUnreceivable = nullptr;
    EXPECT_THROW(boolPtrAssigner.into(boolPtrUnreceivable), Json::Exception);

    Json::Value::Assigner boolUniqueAssigner(std::make_unique<Json::Bool>(true));
    std::unique_ptr<Json::Bool> boolUniqueRecievable = nullptr;
    boolUniqueAssigner.into(boolUniqueRecievable);
    EXPECT_TRUE(boolUniqueRecievable->boolean());

    Json::Value::Assigner boolSharedAssigner(std::make_unique<Json::Bool>(true));
    std::shared_ptr<Json::Bool> boolSharedReceivable = nullptr;
    boolSharedAssigner.into(boolSharedReceivable);
    EXPECT_TRUE(boolSharedReceivable->boolean());

    // All cases where allocatedValue != nullptr and type mismatches
    Json::Value::Assigner numberNonAssigner(std::make_unique<Json::Number>("1"));
    Json::String stringUnassignable;
    EXPECT_THROW(numberNonAssigner.into(stringUnassignable), Json::Value::TypeMismatch);

    Json::Value::Assigner numberNullptrAssigner(std::make_unique<Json::Number>("2"));
    Json::String* stringNullptr = nullptr;
    EXPECT_THROW(numberNullptrAssigner.into(stringNullptr), Json::Exception);

    Json::Value::Assigner numberPtrAssigner(std::make_unique<Json::Number>("3"));
    Json::String* stringPtrUnassignable = &stringUnassignable;
    EXPECT_THROW(numberPtrAssigner.into(stringPtrUnassignable), Json::Value::TypeMismatch);

    Json::Value::Assigner numberUniqueNullAssigner(std::make_unique<Json::Number>("4"));
    std::unique_ptr<Json::String> stringUniqueNullptr = nullptr;
    EXPECT_THROW(numberUniqueNullAssigner.into(stringUniqueNullptr), Json::Exception);

    Json::Value::Assigner numberUniquePtrAssigner(std::make_unique<Json::Number>("5"));
    std::unique_ptr<Json::String> stringUniquePtr = std::make_unique<Json::String>("asdf");
    EXPECT_THROW(numberUniquePtrAssigner.into(stringUniquePtr), Json::Value::TypeMismatch);

    Json::Value::Assigner numberSharedNullAssigner(std::make_unique<Json::Number>("6"));
    std::shared_ptr<Json::String> stringSharedNullptr = nullptr;
    EXPECT_THROW(numberSharedNullAssigner.into(stringSharedNullptr), Json::Exception);

    Json::Value::Assigner numberSharedPtrAssigner(std::make_unique<Json::Number>("7"));
    std::shared_ptr<Json::String> stringSharedPtr = std::make_unique<Json::String>("asdf");
    EXPECT_THROW(numberSharedPtrAssigner.into(stringSharedPtr), Json::Value::TypeMismatch);
}

struct IsNonPrimitiveTestObj {
    int a;
    REFLECT(IsNonPrimitiveTestObj, a)
};

TEST_HEADER(JsonIdentifiersTest, IsNonPrimitive)
{
    EXPECT_FALSE(Json::is_non_primitive_v<char>);
    EXPECT_FALSE(Json::is_non_primitive_v<char*>);
    EXPECT_FALSE(Json::is_non_primitive_v<char**>);
    EXPECT_FALSE(Json::is_non_primitive_v<char&>);
    EXPECT_FALSE(Json::is_non_primitive_v<char&&>);
    EXPECT_FALSE(Json::is_non_primitive_v<std::optional<char>>);
    EXPECT_FALSE(Json::is_non_primitive_v<std::unique_ptr<char>>);
    EXPECT_FALSE(Json::is_non_primitive_v<std::shared_ptr<char>>);
    EXPECT_FALSE(Json::is_non_primitive_v<int>);
    EXPECT_FALSE(Json::is_non_primitive_v<int*>);
    EXPECT_FALSE(Json::is_non_primitive_v<int**>);
    EXPECT_FALSE(Json::is_non_primitive_v<int&>);
    EXPECT_FALSE(Json::is_non_primitive_v<int&&>);
    EXPECT_FALSE(Json::is_non_primitive_v<std::optional<int>>);
    EXPECT_FALSE(Json::is_non_primitive_v<std::unique_ptr<int>>);
    EXPECT_FALSE(Json::is_non_primitive_v<std::shared_ptr<int>>);
    EXPECT_FALSE(Json::is_non_primitive_v<std::string>);
    EXPECT_FALSE(Json::is_non_primitive_v<Json::Generic::Bool>);
    EXPECT_FALSE(Json::is_non_primitive_v<Json::Generic::Number>);
    EXPECT_FALSE(Json::is_non_primitive_v<Json::Generic::String>);

    using Pair = std::pair<int, int>;
    using Tuple = std::tuple<int, int, int>;
    using StlArray = std::array<int, 2>;
    using Map = std::map<int, int>;
    using MultiMap = std::multimap<int, int>;
    using UnorderedMap = std::unordered_map<int, int>;
    using UnorderedMultiMap = std::unordered_multimap<int, int>;
    EXPECT_TRUE(Json::is_non_primitive_v<IsNonPrimitiveTestObj>);
    EXPECT_TRUE(Json::is_non_primitive_v<Pair>);
    EXPECT_TRUE(Json::is_non_primitive_v<Tuple>);
    EXPECT_TRUE(Json::is_non_primitive_v<int[2]>);
    EXPECT_TRUE(Json::is_non_primitive_v<StlArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::vector<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::deque<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::list<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::forward_list<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::stack<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::queue<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::priority_queue<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::set<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::multiset<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::unordered_set<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::unordered_multiset<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<Map>);
    EXPECT_TRUE(Json::is_non_primitive_v<MultiMap>);
    EXPECT_TRUE(Json::is_non_primitive_v<UnorderedMap>);
    EXPECT_TRUE(Json::is_non_primitive_v<UnorderedMultiMap>);
    EXPECT_TRUE(Json::is_non_primitive_v<Json::Generic::Object>);
    EXPECT_TRUE(Json::is_non_primitive_v<Json::Generic::NullArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<Json::Generic::BoolArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<Json::Generic::NumberArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<Json::Generic::StringArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<Json::Generic::ObjectArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<Json::Generic::MixedArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<Json::Generic::FieldCluster>);
    EXPECT_TRUE(Json::is_non_primitive_v<std::vector<int>** &>);
    
    EXPECT_FALSE(Json::is_non_primitive_v<const char>);
    EXPECT_FALSE(Json::is_non_primitive_v<const char*>);
    EXPECT_FALSE(Json::is_non_primitive_v<const char**>);
    EXPECT_FALSE(Json::is_non_primitive_v<const char&>);
    EXPECT_FALSE(Json::is_non_primitive_v<const char&&>);
    EXPECT_FALSE(Json::is_non_primitive_v<const std::optional<char>>);
    EXPECT_FALSE(Json::is_non_primitive_v<const std::unique_ptr<char>>);
    EXPECT_FALSE(Json::is_non_primitive_v<const std::shared_ptr<char>>);
    EXPECT_FALSE(Json::is_non_primitive_v<const int>);
    EXPECT_FALSE(Json::is_non_primitive_v<const int*>);
    EXPECT_FALSE(Json::is_non_primitive_v<const int**>);
    EXPECT_FALSE(Json::is_non_primitive_v<const int&>);
    EXPECT_FALSE(Json::is_non_primitive_v<const int&&>);
    EXPECT_FALSE(Json::is_non_primitive_v<const std::optional<int>>);
    EXPECT_FALSE(Json::is_non_primitive_v<const std::unique_ptr<int>>);
    EXPECT_FALSE(Json::is_non_primitive_v<const std::shared_ptr<int>>);
    EXPECT_FALSE(Json::is_non_primitive_v<const std::string>);
    EXPECT_FALSE(Json::is_non_primitive_v<const Json::Generic::Bool>);
    EXPECT_FALSE(Json::is_non_primitive_v<const Json::Generic::Number>);
    EXPECT_FALSE(Json::is_non_primitive_v<const Json::Generic::String>);

    EXPECT_TRUE(Json::is_non_primitive_v<const IsNonPrimitiveTestObj>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Pair>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Tuple>);
    EXPECT_TRUE(Json::is_non_primitive_v<const int[2]>);
    EXPECT_TRUE(Json::is_non_primitive_v<const StlArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::vector<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::deque<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::list<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::forward_list<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::stack<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::queue<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::priority_queue<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::set<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::multiset<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::unordered_set<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::unordered_multiset<int>>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Map>);
    EXPECT_TRUE(Json::is_non_primitive_v<const MultiMap>);
    EXPECT_TRUE(Json::is_non_primitive_v<const UnorderedMap>);
    EXPECT_TRUE(Json::is_non_primitive_v<const UnorderedMultiMap>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Json::Generic::Object>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Json::Generic::NullArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Json::Generic::BoolArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Json::Generic::NumberArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Json::Generic::StringArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Json::Generic::ObjectArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Json::Generic::MixedArray>);
    EXPECT_TRUE(Json::is_non_primitive_v<const Json::Generic::FieldCluster>);
    EXPECT_TRUE(Json::is_non_primitive_v<const std::vector<int>** &>);
}

TEST_HEADER(JsonIdentifiersTest, IsNonPrimitiveTuple)
{
    using EmptyTuple = std::tuple<>;
    using IntTuple = std::tuple<int>;
    using IntIntTuple = std::tuple<int, int>;
    using IntIntIntTuple = std::tuple<int, int, int>;
    using ArrayTuple = std::tuple<int[2]>;
    using IntArrayTuple = std::tuple<int, int[2]>;
    using ArrayIntTuple = std::tuple<int[2], int>;
    using IntIntArraayTuple = std::tuple<int, int, int[2]>;
    
    EXPECT_FALSE(Json::is_non_primitive_tuple_v<EmptyTuple>);
    EXPECT_FALSE(Json::is_non_primitive_tuple_v<IntTuple>);
    EXPECT_FALSE(Json::is_non_primitive_tuple_v<IntIntTuple>);
    EXPECT_FALSE(Json::is_non_primitive_tuple_v<IntIntIntTuple>);
    
    EXPECT_TRUE(Json::is_non_primitive_tuple_v<ArrayTuple>);
    EXPECT_TRUE(Json::is_non_primitive_tuple_v<IntArrayTuple>);
    EXPECT_TRUE(Json::is_non_primitive_tuple_v<ArrayIntTuple>);
    EXPECT_TRUE(Json::is_non_primitive_tuple_v<IntIntArraayTuple>);
}

TEST_HEADER(JsonIdentifiersTest, IsTuplePair)
{
    using EmptyTuple = std::tuple<>;
    using IntTuple = std::tuple<int>;
    using IntIntTuple = std::tuple<int, int>;
    using IntIntIntTuple = std::tuple<int, int, int>;
    
    EXPECT_FALSE(Json::is_tuple_pair_v<EmptyTuple>);
    EXPECT_FALSE(Json::is_tuple_pair_v<IntTuple>);
    EXPECT_TRUE(Json::is_tuple_pair_v<IntIntTuple>);
    EXPECT_FALSE(Json::is_tuple_pair_v<IntIntIntTuple>);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeNoSpecialization)
{
    CustomizeNoSpecialization noSpecialization {};

    bool isSpecialized = Json::Output::Customize<CustomizeNoSpecialization, int, Reflect<CustomizeNoSpecialization>::IndexOf::integer,
        NoNote, Reflect<CustomizeNoSpecialization>::MemberType::integer, Json::Statics::Included, true, 0, Json::twoSpaces>
        ::as(os, Json::defaultContext, noSpecialization, noSpecialization.integer);
    EXPECT_FALSE(isSpecialized);

    isSpecialized = Json::Output::Customize<CustomizeNoSpecialization, char, Reflect<CustomizeNoSpecialization>::IndexOf::character,
        NoNote, Reflect<CustomizeNoSpecialization>::MemberType::character, Json::Statics::Included, true, 0, Json::twoSpaces>
        ::as(os, Json::defaultContext, noSpecialization, noSpecialization.character);
    EXPECT_FALSE(isSpecialized);

    isSpecialized = Json::Output::haveSpecialization<CustomizeNoSpecialization, int, Reflect<CustomizeNoSpecialization>::IndexOf::integer,
        NoNote, Reflect<CustomizeNoSpecialization>::MemberType::integer, Json::Statics::Included, true, 0, Json::twoSpaces>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeFullySpecialized)
{
    CustomizeFullySpecialized fullySpecialized {};

    bool isSpecialized = Json::Output::Customize<CustomizeFullySpecialized, int, Reflect<CustomizeFullySpecialized>::IndexOf::firstField,
        NoNote, Reflect<CustomizeFullySpecialized>::MemberType::firstField, Json::Statics::Included, true, 0, Json::twoSpaces>
        ::as(os, Json::defaultContext, fullySpecialized, fullySpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<CustomizeFullySpecialized, int, Reflect<CustomizeFullySpecialized>::IndexOf::firstField,
        NoNote, Reflect<CustomizeFullySpecialized>::MemberType::firstField, Json::Statics::Included, true, 0, Json::twoSpaces>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<CustomizeFullySpecialized, int, Reflect<CustomizeFullySpecialized>::IndexOf::secondField,
        NoNote, Reflect<CustomizeFullySpecialized>::MemberType::secondField, Json::Statics::Included, true, 0, Json::twoSpaces>
        ::as(os, Json::defaultContext, fullySpecialized, fullySpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<CustomizeFullySpecialized, int, Reflect<CustomizeFullySpecialized>::IndexOf::secondField,
        NoNote, Reflect<CustomizeFullySpecialized>::MemberType::secondField, Json::Statics::Included, true, 0, Json::twoSpaces>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<CustomizeFullySpecialized, char, Reflect<CustomizeFullySpecialized>::IndexOf::unspecialized,
        NoNote, Reflect<CustomizeFullySpecialized>::MemberType::unspecialized, Json::Statics::Included, true, 0, Json::twoSpaces>
        ::as(os, Json::defaultContext, fullySpecialized, fullySpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<CustomizeFullySpecialized, char, Reflect<CustomizeFullySpecialized>::IndexOf::unspecialized,
        NoNote, Reflect<CustomizeFullySpecialized>::MemberType::unspecialized, Json::Statics::Included, true, 0, Json::twoSpaces>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize5ArgSpecialized)
{
    Customize5ArgSpecialized fiveArgSpecialized {};

    bool isSpecialized = Json::Output::Customize<Customize5ArgSpecialized, int, Reflect<Customize5ArgSpecialized>::IndexOf::firstField,
        NoNote, Reflect<Customize5ArgSpecialized>::MemberType::firstField>::as(os, Json::defaultContext, fiveArgSpecialized, fiveArgSpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized, int, Reflect<Customize5ArgSpecialized>::IndexOf::firstField,
        NoNote, Reflect<Customize5ArgSpecialized>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized, int, Reflect<Customize5ArgSpecialized>::IndexOf::secondField,
        NoNote, Reflect<Customize5ArgSpecialized>::MemberType::secondField>::as(os, Json::defaultContext, fiveArgSpecialized, fiveArgSpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized, int, Reflect<Customize5ArgSpecialized>::IndexOf::secondField,
        NoNote, Reflect<Customize5ArgSpecialized>::MemberType::secondField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized, char, Reflect<Customize5ArgSpecialized>::IndexOf::unspecialized,
        NoNote, Reflect<Customize5ArgSpecialized>::MemberType::unspecialized>::as(os, Json::defaultContext, fiveArgSpecialized, fiveArgSpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized, char, Reflect<Customize5ArgSpecialized>::IndexOf::unspecialized,
        NoNote, Reflect<Customize5ArgSpecialized>::MemberType::unspecialized>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize4ArgSpecialized)
{
    Customize4ArgSpecialized fourArgSpecialized {};

    bool isSpecialized = Json::Output::Customize<Customize4ArgSpecialized, int, Reflect<Customize4ArgSpecialized>::IndexOf::firstField,
        NoNote>::as(os, Json::defaultContext, fourArgSpecialized, fourArgSpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize4ArgSpecialized, int, Reflect<Customize4ArgSpecialized>::IndexOf::firstField,
        NoNote>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize4ArgSpecialized, int, Reflect<Customize4ArgSpecialized>::IndexOf::secondField,
        NoNote>::as(os, Json::defaultContext, fourArgSpecialized, fourArgSpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize4ArgSpecialized, int, Reflect<Customize4ArgSpecialized>::IndexOf::secondField,
        NoNote>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize4ArgSpecialized, char, Reflect<Customize4ArgSpecialized>::IndexOf::unspecialized,
        NoNote>::as(os, Json::defaultContext, fourArgSpecialized, fourArgSpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize4ArgSpecialized, char, Reflect<Customize4ArgSpecialized>::IndexOf::unspecialized,
        NoNote>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize3ArgSpecialized)
{
    Customize3ArgSpecialized threeArgSpecialized {};

    bool isSpecialized = Json::Output::Customize<Customize3ArgSpecialized, int, Reflect<Customize3ArgSpecialized>::IndexOf::firstField>
        ::as(os, Json::defaultContext, threeArgSpecialized, threeArgSpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize3ArgSpecialized, int, Reflect<Customize3ArgSpecialized>::IndexOf::firstField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize3ArgSpecialized, int, Reflect<Customize3ArgSpecialized>::IndexOf::secondField>
        ::as(os, Json::defaultContext, threeArgSpecialized, threeArgSpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize3ArgSpecialized, int, Reflect<Customize3ArgSpecialized>::IndexOf::secondField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize3ArgSpecialized, char, Reflect<Customize3ArgSpecialized>::IndexOf::unspecialized>
        ::as(os, Json::defaultContext, threeArgSpecialized, threeArgSpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize3ArgSpecialized, char, Reflect<Customize3ArgSpecialized>::IndexOf::unspecialized>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize2ArgSpecialized)
{
    Customize2ArgSpecialized twoArgSpecialized {};

    bool isSpecialized = Json::Output::Customize<Customize2ArgSpecialized, int>
        ::as(os, Json::defaultContext, twoArgSpecialized, twoArgSpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize2ArgSpecialized, int>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize2ArgSpecialized, int>
        ::as(os, Json::defaultContext, twoArgSpecialized, twoArgSpecialized.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize2ArgSpecialized, int>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize2ArgSpecialized, char>
        ::as(os, Json::defaultContext, twoArgSpecialized, twoArgSpecialized.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize2ArgSpecialized, char>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize5ArgSpecialized_OpAnnotationsDefaulted)
{
    Customize5ArgSpecialized_OpAnnotationsDefaulted fiveArgSpecialized_OpAnnotationsDefaulted {};

    bool isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::IndexOf::firstField,
        NoNote, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::MemberType::firstField>
        ::as(os, Json::defaultContext, fiveArgSpecialized_OpAnnotationsDefaulted, fiveArgSpecialized_OpAnnotationsDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::IndexOf::firstField,
        NoNote, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::IndexOf::secondField,
        NoNote, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::MemberType::secondField>
        ::as(os, Json::defaultContext, fiveArgSpecialized_OpAnnotationsDefaulted, fiveArgSpecialized_OpAnnotationsDefaulted.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::IndexOf::secondField,
        NoNote, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::MemberType::secondField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_OpAnnotationsDefaulted, char, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::IndexOf::unspecialized,
        NoNote, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::MemberType::unspecialized>
        ::as(os, Json::defaultContext, fiveArgSpecialized_OpAnnotationsDefaulted, fiveArgSpecialized_OpAnnotationsDefaulted.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_OpAnnotationsDefaulted, char, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::IndexOf::unspecialized,
        NoNote, Reflect<Customize5ArgSpecialized_OpAnnotationsDefaulted>::MemberType::unspecialized>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize5ArgSpecialized_MemberIndexDefaulted)
{
    Customize5ArgSpecialized_MemberIndexDefaulted fiveArgSpecialized_MemberIndexDefaulted {};

    bool isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_MemberIndexDefaulted>::MemberType::firstField>
        ::as(os, Json::defaultContext, fiveArgSpecialized_MemberIndexDefaulted, fiveArgSpecialized_MemberIndexDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_MemberIndexDefaulted>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_MemberIndexDefaulted>::MemberType::secondField>
        ::as(os, Json::defaultContext, fiveArgSpecialized_MemberIndexDefaulted, fiveArgSpecialized_MemberIndexDefaulted.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_MemberIndexDefaulted>::MemberType::secondField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_MemberIndexDefaulted, char, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_MemberIndexDefaulted>::MemberType::unspecialized>
        ::as(os, Json::defaultContext, fiveArgSpecialized_MemberIndexDefaulted, fiveArgSpecialized_MemberIndexDefaulted.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_MemberIndexDefaulted, char, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_MemberIndexDefaulted>::MemberType::unspecialized>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize5ArgSpecialized_BothDefaulted)
{
    Customize5ArgSpecialized_BothDefaulted fiveArgSpecialized_BothDefaulted {};

    bool isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_BothDefaulted, int, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_BothDefaulted>::MemberType::firstField>
        ::as(os, Json::defaultContext, fiveArgSpecialized_BothDefaulted, fiveArgSpecialized_BothDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_BothDefaulted, int, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_BothDefaulted>::MemberType::firstField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_BothDefaulted, int, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_BothDefaulted>::MemberType::secondField>
        ::as(os, Json::defaultContext, fiveArgSpecialized_BothDefaulted, fiveArgSpecialized_BothDefaulted.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_BothDefaulted, int, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_BothDefaulted>::MemberType::secondField>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize5ArgSpecialized_BothDefaulted, char, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_BothDefaulted>::MemberType::unspecialized>
        ::as(os, Json::defaultContext, fiveArgSpecialized_BothDefaulted, fiveArgSpecialized_BothDefaulted.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize5ArgSpecialized_BothDefaulted, char, Json::noMemberIndex,
        NoNote, Reflect<Customize5ArgSpecialized_BothDefaulted>::MemberType::unspecialized>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, Customize4ArgSpecialized_MemberIndexDefaulted)
{
    Customize4ArgSpecialized_MemberIndexDefaulted fourArgSpecialized_MemberIndexDefaulted {};

    bool isSpecialized = Json::Output::Customize<Customize4ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex, NoNote>
        ::as(os, Json::defaultContext, fourArgSpecialized_MemberIndexDefaulted, fourArgSpecialized_MemberIndexDefaulted.firstField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize4ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex, NoNote>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize4ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex, NoNote>
        ::as(os, Json::defaultContext, fourArgSpecialized_MemberIndexDefaulted, fourArgSpecialized_MemberIndexDefaulted.secondField);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize4ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex, NoNote>;
    EXPECT_TRUE(isSpecialized);

    isSpecialized = Json::Output::Customize<Customize4ArgSpecialized_MemberIndexDefaulted, char, Json::noMemberIndex, NoNote>
        ::as(os, Json::defaultContext, fourArgSpecialized_MemberIndexDefaulted, fourArgSpecialized_MemberIndexDefaulted.unspecialized);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<Customize4ArgSpecialized_MemberIndexDefaulted, char, Json::noMemberIndex, NoNote>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeUnspecialized)
{
    ContainsUnspecialized containsUnspecialized {};

    bool isSpecialized = Json::Output::CustomizeType<UnspecializedType, NoNote, Reflect<ContainsUnspecialized>::MemberType::unspecializedType,
        Json::Statics::Included, true, 0, Json::twoSpaces>::as(os, Json::defaultContext, containsUnspecialized.unspecializedType);
    EXPECT_FALSE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<ContainsUnspecialized, UnspecializedType, Reflect<ContainsUnspecialized>::IndexOf::unspecializedType, NoNote, Reflect<ContainsUnspecialized>::MemberType::unspecializedType,
        Json::Statics::Included, true, 0, Json::twoSpaces>;
    EXPECT_FALSE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeFullySpecialized)
{
    ContainsFullySpecialized containsFullySpecialized {};

    bool isSpecialized = Json::Output::CustomizeType<FullySpecializedType, NoNote, Reflect<ContainsFullySpecialized>::MemberType::fullySpecializedType,
        Json::Statics::Included, true, 0, Json::twoSpaces>::as(os, Json::defaultContext, containsFullySpecialized.fullySpecializedType);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<ContainsFullySpecialized, FullySpecializedType, Reflect<ContainsFullySpecialized>::IndexOf::fullySpecializedType, NoNote, Reflect<ContainsFullySpecialized>::MemberType::fullySpecializedType,
        Json::Statics::Included, true, 0, Json::twoSpaces>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeThreeArgSpecialized)
{
    ContainsThreeArgSpecialized containsThreeArgSpecialized {};

    bool isSpecialized = Json::Output::CustomizeType<ThreeArgSpecializedType, NoNote, Reflect<ContainsThreeArgSpecialized>::MemberType::threeArgSpecializedType>
        ::as(os, Json::defaultContext, containsThreeArgSpecialized.threeArgSpecializedType);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<ContainsThreeArgSpecialized, ThreeArgSpecializedType, Reflect<ContainsThreeArgSpecialized>::IndexOf::threeArgSpecializedType, NoNote, Reflect<ContainsThreeArgSpecialized>::MemberType::threeArgSpecializedType>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeTwoArgSpecialized)
{
    ContainsTwoArgSpecialized containsTwoArgSpecialized {};

    bool isSpecialized = Json::Output::CustomizeType<TwoArgSpecializedType, NoNote>
        ::as(os, Json::defaultContext, containsTwoArgSpecialized.twoArgSpecializedType);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<ContainsTwoArgSpecialized, TwoArgSpecializedType, Reflect<ContainsTwoArgSpecialized>::IndexOf::twoArgSpecializedType, NoNote>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeOneArgSpecialized)
{
    ContainsOneArgSpecialized containsOneArgSpecialized {};

    bool isSpecialized = Json::Output::CustomizeType<OneArgSpecializedType>
        ::as(os, Json::defaultContext, containsOneArgSpecialized.oneArgSpecializedType);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<ContainsOneArgSpecialized, OneArgSpecializedType>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputCustomizersTest, CustomizeTypeThreeArgSpecialized_OpAnnotationsDefaulted)
{
    ContainsThreeArgSpecializedType_OpAnnotationsDefaulted containsThreeArgSpecializedType_OpAnnotationsDefaulted {};

    bool isSpecialized = Json::Output::CustomizeType<ThreeArgSpecializedType_OpAnnotationsDefaulted, NoNote,
        Reflect<ContainsThreeArgSpecializedType_OpAnnotationsDefaulted>::MemberType::threeArgSpecializedType_OpAnnotationsDefaulted>
        ::as(os, Json::defaultContext, containsThreeArgSpecializedType_OpAnnotationsDefaulted.threeArgSpecializedType_OpAnnotationsDefaulted);
    EXPECT_TRUE(isSpecialized);
    isSpecialized = Json::Output::haveSpecialization<ContainsThreeArgSpecializedType_OpAnnotationsDefaulted, ThreeArgSpecializedType_OpAnnotationsDefaulted,
        Reflect<ContainsThreeArgSpecializedType_OpAnnotationsDefaulted>::IndexOf::threeArgSpecializedType_OpAnnotationsDefaulted,
        NoNote, Reflect<ContainsThreeArgSpecializedType_OpAnnotationsDefaulted>::MemberType::threeArgSpecializedType_OpAnnotationsDefaulted>;
    EXPECT_TRUE(isSpecialized);
}

TEST_HEADER(JsonOutputStaticAffixTest, StaticIndent)
{
    TestStreamType zeroIndents;
    zeroIndents << Json::Output::indent<true, 0, Json::twoSpaces>;
    EXPECT_STREQ("", zeroIndents.str().c_str());

    TestStreamType oneIndent;
    oneIndent << Json::Output::indent<true, 1, Json::twoSpaces>;
    EXPECT_STREQ("  ", oneIndent.str().c_str());

    TestStreamType twoIndents;
    twoIndents << Json::Output::indent<true, 2, Json::twoSpaces>;
    EXPECT_STREQ("    ", twoIndents.str().c_str());

    TestStreamType threeIndents;
    threeIndents << Json::Output::indent<true, 3, Json::twoSpaces>;
    EXPECT_STREQ("      ", threeIndents.str().c_str());

    TestStreamType zeroIndentsUnpretty;
    zeroIndentsUnpretty << Json::Output::indent<false, 0, Json::twoSpaces>;
    EXPECT_STREQ("", zeroIndentsUnpretty.str().c_str());

    TestStreamType oneIndentUnpretty;
    oneIndentUnpretty << Json::Output::indent<false, 1, Json::twoSpaces>;
    EXPECT_STREQ("", oneIndentUnpretty.str().c_str());

    TestStreamType twoIndentsUnpretty;
    twoIndentsUnpretty << Json::Output::indent<false, 2, Json::twoSpaces>;
    EXPECT_STREQ("", twoIndentsUnpretty.str().c_str());

    TestStreamType threeIndentsUnpretty;
    threeIndentsUnpretty << Json::Output::indent<false, 3, Json::twoSpaces>;
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
    nonPrimitivePrettyCompare << "[" << osEndl << Json::indent<true, 4, Json::twoSpaces>;
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
    nonPrimitivePrettyCompare << osEndl << Json::Output::indent<true, 3, Json::twoSpaces> << "]";
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

NOTE(TrulyEmpty)
struct TrulyEmpty
{
    REFLECT_NOTED(TrulyEmpty)
};

struct NoSuperOrStatic
{
    int instanceField;

    REFLECT(NoSuperOrStatic, instanceField)
};

struct NoSuperOrInstance
{
    static int staticField;

    REFLECT(NoSuperOrInstance, staticField)
};
int NoSuperOrInstance::staticField = 0;

struct NoSuper
{
    int instanceField;
    static int staticField;

    REFLECT(NoSuper, instanceField, staticField)
};
int NoSuper::staticField = 0;

struct RegularSuper
{
    int instanceField;
    static int staticField;

    REFLECT(RegularSuper, instanceField, staticField)
};

NOTE(NoStaticOrInstance, Super<RegularSuper>)
struct NoStaticOrInstance : public RegularSuper
{
    REFLECT_NOTED(NoStaticOrInstance)
};

NOTE(NoStatic, Super<RegularSuper>)
struct NoStatic : public RegularSuper
{
    int instanceField;
    
    REFLECT_NOTED(NoStatic, instanceField)
};

NOTE(NoInstance, Super<RegularSuper>)
struct NoInstance : public RegularSuper
{
    static int staticField;

    REFLECT_NOTED(NoInstance, staticField)
};
int NoInstance::staticField = 0;

NOTE(Everything, Super<RegularSuper>)
struct Everything : public RegularSuper
{
    int instanceField;
    static int staticField;

    REFLECT_NOTED(Everything, instanceField, staticField)
};
int Everything::staticField = 0;

TEST_HEADER(JsonOutputStaticAffixTest, StaticObjectSuffix)
{
    TestStreamType voidPrettyCompare;
    voidPrettyCompare << osEndl << Json::Output::indent<true, 3, Json::twoSpaces> << "}";
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
    prettyNotEmptyCompareStream << osEndl << Json::indent<true, 3, Json::twoSpaces> << "}";
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
    prettyFirstCompare << osEndl << Json::Output::indent<true, 3, Json::twoSpaces>;
    TestStreamType prettyFirst;
    prettyFirst << Json::Output::FieldPrefix<true, true, 3, Json::twoSpaces>;
    EXPECT_STREQ(prettyFirstCompare.str().c_str(), prettyFirst.str().c_str());

    TestStreamType prettyNonFirstCompare;
    prettyNonFirstCompare << "," << osEndl << Json::Output::indent<true, 3, Json::twoSpaces>;
    TestStreamType prettyNonFirst;
    prettyNonFirst << Json::Output::FieldPrefix<false, true, 3, Json::twoSpaces>;
    EXPECT_STREQ(prettyNonFirstCompare.str().c_str(), prettyNonFirst.str().c_str());
}

TEST_HEADER(JsonOutputStaticAffixTest, StaticFieldNameValueSeparator)
{
    TestStreamType pretty;
    pretty << Json::Output::fieldNameValueSeparator<true>;
    EXPECT_STREQ(": ", pretty.str().c_str());

    TestStreamType nonPretty;
    nonPretty << Json::Output::fieldNameValueSeparator<false>;
    EXPECT_STREQ(":", nonPretty.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, Indent)
{
    TestStreamType prettyIndent;
    Json::Put::indent<true, Json::twoSpaces>(prettyIndent, 3);
    EXPECT_STREQ("      ", prettyIndent.str().c_str());

    TestStreamType nonPrettyIndent;
    Json::Put::indent<false, Json::twoSpaces>(nonPrettyIndent, 3);
    EXPECT_STREQ("", nonPrettyIndent.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, ArrayPrefix)
{
    TestStreamType nonPretty;
    Json::Put::arrayPrefix<false, false, Json::twoSpaces>(nonPretty, 3);
    EXPECT_STREQ("[", nonPretty.str().c_str());

    TestStreamType prettyPrimitives;
    Json::Put::arrayPrefix<true, true, Json::twoSpaces>(prettyPrimitives, 3);
    EXPECT_STREQ("[ ", prettyPrimitives.str().c_str());

    TestStreamType prettyNonPrimitivesCompare;
    prettyNonPrimitivesCompare << "[" << osEndl;
    Json::Put::indent<true, Json::twoSpaces>(prettyNonPrimitivesCompare, 4);

    TestStreamType prettyNonPrimitives;
    Json::Put::arrayPrefix<true, false, Json::twoSpaces>(prettyNonPrimitives, 3);
    EXPECT_STREQ(prettyNonPrimitivesCompare.str().c_str(), prettyNonPrimitives.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, ArraySuffix)
{
    TestStreamType nonPretty;
    Json::Put::arraySuffix<false, false, Json::twoSpaces>(nonPretty, 3);
    EXPECT_STREQ("]", nonPretty.str().c_str());

    TestStreamType prettyPrimitives;
    Json::Put::arraySuffix<true, true, Json::twoSpaces>(prettyPrimitives, 3);
    EXPECT_STREQ(" ]", prettyPrimitives.str().c_str());

    TestStreamType prettyNonPrimitivesCompare;
    prettyNonPrimitivesCompare << osEndl;
    Json::Put::indent<true, Json::twoSpaces>(prettyNonPrimitivesCompare, 3);
    prettyNonPrimitivesCompare << "]";

    TestStreamType prettyNonPrimitives;
    Json::Put::arraySuffix<true, false, Json::twoSpaces>(prettyNonPrimitives, 3);
    EXPECT_STREQ(prettyNonPrimitivesCompare.str().c_str(), prettyNonPrimitives.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, ObjectPrefix)
{
    TestStreamType objectPrefix;
    Json::Put::objectPrefix<true, Json::twoSpaces>(objectPrefix);
    EXPECT_STREQ("{", objectPrefix.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, ObjectSuffix)
{
    TestStreamType nonPretty;
    Json::Put::objectSuffix<false, Json::twoSpaces>(nonPretty, false, 3);
    EXPECT_STREQ("}", nonPretty.str().c_str());

    TestStreamType prettyEmpty;
    Json::Put::objectSuffix<true, Json::twoSpaces>(prettyEmpty, true, 3);
    EXPECT_STREQ("}", prettyEmpty.str().c_str());

    TestStreamType prettyNonEmptyCompare;
    prettyNonEmptyCompare << osEndl;
    Json::Put::indent<true, Json::twoSpaces>(prettyNonEmptyCompare, 3);
    prettyNonEmptyCompare << "}";

    TestStreamType prettyNonEmpty;
    Json::Put::objectSuffix<true, Json::twoSpaces>(prettyNonEmpty, false, 3);
    EXPECT_STREQ(prettyNonEmptyCompare.str().c_str(), prettyNonEmpty.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, FieldPrefix)
{
    TestStreamType prettyFirstCompare;
    prettyFirstCompare << osEndl;
    Json::Put::indent<true, Json::twoSpaces>(prettyFirstCompare, 3);
    TestStreamType prettyFirst;
    Json::Put::fieldPrefix<true, Json::twoSpaces>(prettyFirst, true, 3);
    EXPECT_STREQ(prettyFirstCompare.str().c_str(), prettyFirst.str().c_str());

    TestStreamType prettyNonFirstCompare;
    prettyNonFirstCompare << "," << osEndl;
    Json::Put::indent<true, Json::twoSpaces>(prettyNonFirstCompare, 3);
    TestStreamType prettyNonFirst;
    Json::Put::fieldPrefix<true, Json::twoSpaces>(prettyNonFirst, false, 3);
    EXPECT_STREQ(prettyNonFirstCompare.str().c_str(), prettyNonFirst.str().c_str());

    TestStreamType nonPrettyNonFirst;
    Json::Put::fieldPrefix<false, Json::twoSpaces>(nonPrettyNonFirst, false, 3);
    EXPECT_STREQ(",", nonPrettyNonFirst.str().c_str());

    TestStreamType nonPrettyFirst;
    Json::Put::fieldPrefix<false, Json::twoSpaces>(nonPrettyFirst, true, 3);
    EXPECT_STREQ("", nonPrettyFirst.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, PartiallyStaticNestedPrefix)
{
    TestStreamType emptyArrayCompare;
    emptyArrayCompare << Json::ArrayPrefix<false, false, 3, Json::twoSpaces>;
    TestStreamType emptyArray;
    Json::Put::nestedPrefix<true, true, false, 3, Json::twoSpaces>(emptyArray, true);
    EXPECT_STREQ(emptyArrayCompare.str().c_str(), emptyArray.str().c_str());
    
    TestStreamType nonEmptyArrayCompare;
    nonEmptyArrayCompare << Json::ArrayPrefix<true, false, 3, Json::twoSpaces>;
    TestStreamType nonEmptyArray;
    Json::Put::nestedPrefix<true, true, false, 3, Json::twoSpaces>(nonEmptyArray, false);
    EXPECT_STREQ(nonEmptyArrayCompare.str().c_str(), nonEmptyArray.str().c_str());

    TestStreamType emptyObjectCompare;
    emptyObjectCompare << Json::ObjectPrefix<false, 3, Json::twoSpaces>;
    TestStreamType emptyObject;
    Json::Put::nestedPrefix<true, false, false, 3, Json::twoSpaces>(emptyObject, true);
    EXPECT_STREQ(emptyObjectCompare.str().c_str(), emptyObject.str().c_str());

    TestStreamType nonEmptyObjectCompare;
    nonEmptyObjectCompare << Json::ObjectPrefix<true, 3, Json::twoSpaces>;
    TestStreamType nonEmptyObject;
    Json::Put::nestedPrefix<true, false, false, 3, Json::twoSpaces>(nonEmptyObject, false);
    EXPECT_STREQ(nonEmptyObjectCompare.str().c_str(), nonEmptyObject.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, NestedPrefix)
{
    TestStreamType emptyArrayPrimitivesCompare;
    Json::Put::arrayPrefix<false, true, Json::twoSpaces>(emptyArrayPrimitivesCompare, 3);
    TestStreamType emptyArrayPrimitives;
    Json::Put::nestedPrefix<true, Json::twoSpaces>(emptyArrayPrimitives, true, true, true, 3);
    EXPECT_STREQ(emptyArrayPrimitivesCompare.str().c_str(), emptyArrayPrimitives.str().c_str());

    TestStreamType emptyArrayNonPrimitivesCompare;
    Json::Put::arrayPrefix<false, false, Json::twoSpaces>(emptyArrayNonPrimitivesCompare, 3);
    TestStreamType emptyArrayNonPrimitives;
    Json::Put::nestedPrefix<true, Json::twoSpaces>(emptyArrayNonPrimitives, true, false, true, 3);
    EXPECT_STREQ(emptyArrayNonPrimitivesCompare.str().c_str(), emptyArrayNonPrimitives.str().c_str());
    
    TestStreamType nonEmptyArrayPrimitivesCompare;
    Json::Put::arrayPrefix<true, true, Json::twoSpaces>(nonEmptyArrayPrimitivesCompare, 3);
    TestStreamType nonEmptyArrayPrimitives;
    Json::Put::nestedPrefix<true, Json::twoSpaces>(nonEmptyArrayPrimitives, true, true, false, 3);
    EXPECT_STREQ(nonEmptyArrayPrimitivesCompare.str().c_str(), nonEmptyArrayPrimitives.str().c_str());

    TestStreamType nonEmptyArrayNonPrimitivesCompare;
    Json::Put::arrayPrefix<true, false, Json::twoSpaces>(nonEmptyArrayNonPrimitivesCompare, 3);
    TestStreamType nonEmptyArrayNonPrimitives;
    Json::Put::nestedPrefix<true, Json::twoSpaces>(nonEmptyArrayNonPrimitives, true, false, false, 3);
    EXPECT_STREQ(nonEmptyArrayNonPrimitivesCompare.str().c_str(), nonEmptyArrayNonPrimitives.str().c_str());
    
    TestStreamType emptyObjectCompare;
    Json::Put::objectPrefix<false, Json::twoSpaces>(emptyObjectCompare);
    TestStreamType emptyObject;
    Json::Put::nestedPrefix<true, Json::twoSpaces>(emptyObject, false, true, true, 3);
    EXPECT_STREQ(emptyObjectCompare.str().c_str(), emptyObject.str().c_str());

    TestStreamType nonEmptyObjectCompare;
    Json::Put::objectPrefix<true, Json::twoSpaces>(nonEmptyObjectCompare);
    TestStreamType nonEmptyObject;
    Json::Put::nestedPrefix<true, Json::twoSpaces>(nonEmptyObject, false, true, false, 3);
    EXPECT_STREQ(nonEmptyObjectCompare.str().c_str(), nonEmptyObject.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, PartiallyStaticNestedSuffix)
{
    TestStreamType emptyArrayCompare;
    emptyArrayCompare << Json::ArraySuffix<false, false, 3, Json::twoSpaces>;
    TestStreamType emptyArray;
    Json::Put::nestedSuffix<true, true, false, 3, Json::twoSpaces>(emptyArray, true);
    EXPECT_STREQ(emptyArrayCompare.str().c_str(), emptyArray.str().c_str());

    TestStreamType nonEmptyArrayCompare;
    nonEmptyArrayCompare << Json::ArraySuffix<true, false, 3, Json::twoSpaces>;
    TestStreamType nonEmptyArray;
    Json::Put::nestedSuffix<true, true, false, 3, Json::twoSpaces>(nonEmptyArray, false);
    EXPECT_STREQ(nonEmptyArrayCompare.str().c_str(), nonEmptyArray.str().c_str());

    TestStreamType emptyObjectCompare;
    emptyObjectCompare << Json::ObjectSuffix<false, 3, Json::twoSpaces>;
    TestStreamType emptyObject;
    Json::Put::nestedSuffix<true, false, false, 3, Json::twoSpaces>(emptyObject, true);
    EXPECT_STREQ(emptyObjectCompare.str().c_str(), emptyObject.str().c_str());

    TestStreamType nonEmptyObjectCompare;
    nonEmptyObjectCompare << Json::ObjectSuffix<true, 3, Json::twoSpaces>;
    TestStreamType nonEmptyObject;
    Json::Put::nestedSuffix<true, false, false, 3, Json::twoSpaces>(nonEmptyObject, false);
    EXPECT_STREQ(nonEmptyObjectCompare.str().c_str(), nonEmptyObject.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, NestedSuffix)
{
    TestStreamType emptyArrayPrimitivesCompare;
    Json::Put::arraySuffix<false, true, Json::twoSpaces>(emptyArrayPrimitivesCompare, 3);
    TestStreamType emptyArrayPrimitives;
    Json::Put::nestedSuffix<true, Json::twoSpaces>(emptyArrayPrimitives, true, true, true, 3);
    EXPECT_STREQ(emptyArrayPrimitivesCompare.str().c_str(), emptyArrayPrimitives.str().c_str());
    
    TestStreamType emptyArrayNonPrimitivesCompare;
    Json::Put::arraySuffix<false, false, Json::twoSpaces>(emptyArrayNonPrimitivesCompare, 3);
    TestStreamType emptyArrayNonPrimitives;
    Json::Put::nestedSuffix<true, Json::twoSpaces>(emptyArrayNonPrimitives, true, false, true, 3);
    EXPECT_STREQ(emptyArrayNonPrimitivesCompare.str().c_str(), emptyArrayNonPrimitives.str().c_str());
    
    TestStreamType nonEmptyArrayPrimitivesCompare;
    Json::Put::arraySuffix<true, true, Json::twoSpaces>(nonEmptyArrayPrimitivesCompare, 3);
    TestStreamType nonEmptyArrayPrimitives;
    Json::Put::nestedSuffix<true, Json::twoSpaces>(nonEmptyArrayPrimitives, true, true, false, 3);
    EXPECT_STREQ(nonEmptyArrayPrimitivesCompare.str().c_str(), nonEmptyArrayPrimitives.str().c_str());
    
    TestStreamType nonEmptyArrayNonPrimitivesCompare;
    Json::Put::arraySuffix<true, false, Json::twoSpaces>(nonEmptyArrayNonPrimitivesCompare, 3);
    TestStreamType nonEmptyArrayNonPrimitives;
    Json::Put::nestedSuffix<true, Json::twoSpaces>(nonEmptyArrayNonPrimitives, true, false, false, 3);
    EXPECT_STREQ(nonEmptyArrayNonPrimitivesCompare.str().c_str(), nonEmptyArrayNonPrimitives.str().c_str());
    
    TestStreamType emptyObjectCompare;
    Json::Put::objectSuffix<false, Json::twoSpaces>(emptyObjectCompare, true, 3);
    TestStreamType emptyObject;
    Json::Put::nestedSuffix<true, Json::twoSpaces>(emptyObject, false, false, true, 3);
    EXPECT_STREQ(emptyObjectCompare.str().c_str(), emptyObject.str().c_str());
    
    TestStreamType nonEmptyObjectCompare;
    Json::Put::objectSuffix<true, Json::twoSpaces>(nonEmptyObjectCompare, false, 3);
    TestStreamType nonEmptyObject;
    Json::Put::nestedSuffix<true, Json::twoSpaces>(nonEmptyObject, false, false, false, 3);
    EXPECT_STREQ(nonEmptyObjectCompare.str().c_str(), nonEmptyObject.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, PartiallyStaticSeparator)
{
    TestStreamType jsonFirstPrettyCompare;
    jsonFirstPrettyCompare << osEndl << Json::indent<true, 3, Json::twoSpaces>;
    TestStreamType jsonFirstPretty;
    Json::Put::separator<true, true, false, 3, Json::twoSpaces>(jsonFirstPretty, true);
    EXPECT_STREQ(jsonFirstPrettyCompare.str().c_str(), jsonFirstPretty.str().c_str());
    
    TestStreamType jsonNonFirstPrettyCompare;
    jsonNonFirstPrettyCompare << "," << osEndl << Json::indent<true, 3, Json::twoSpaces>;
    TestStreamType jsonNonFirstPretty;
    Json::Put::separator<true, true, false, 3, Json::twoSpaces>(jsonNonFirstPretty, false);
    EXPECT_STREQ(jsonNonFirstPrettyCompare.str().c_str(), jsonNonFirstPretty.str().c_str());

    TestStreamType jsonFirstNonPretty;
    Json::Put::separator<false, true, false, 3, Json::twoSpaces>(jsonFirstNonPretty, true);
    EXPECT_STREQ("", jsonFirstNonPretty.str().c_str());

    TestStreamType jsonNonFirstNonPretty;
    Json::Put::separator<false, true, false, 3, Json::twoSpaces>(jsonNonFirstNonPretty, false);
    EXPECT_STREQ(",", jsonNonFirstNonPretty.str().c_str());
    
    TestStreamType nonJsonFieldFirst;
    Json::Put::separator<true, false, false, 3, Json::twoSpaces>(nonJsonFieldFirst, true);
    EXPECT_STREQ("", nonJsonFieldFirst.str().c_str());

    TestStreamType nonJsonFieldNestedPrettyCompare;
    nonJsonFieldNestedPrettyCompare << "," << osEndl << Json::indent<true, 3, Json::twoSpaces>;
    TestStreamType nonJsonFieldNestedPretty;
    Json::Put::separator<true, false, true, 3, Json::twoSpaces>(nonJsonFieldNestedPretty, false);
    EXPECT_STREQ(nonJsonFieldNestedPrettyCompare.str().c_str(), nonJsonFieldNestedPretty.str().c_str());

    TestStreamType nonJsonFieldNonNestedPretty;
    Json::Put::separator<true, false, false, 3, Json::twoSpaces>(nonJsonFieldNonNestedPretty, false);
    EXPECT_STREQ(", ", nonJsonFieldNonNestedPretty.str().c_str());

    TestStreamType nonJsonFieldNestedNonPretty;
    Json::Put::separator<false, false, true, 3, Json::twoSpaces>(nonJsonFieldNestedNonPretty, false);
    EXPECT_STREQ(",", nonJsonFieldNestedNonPretty.str().c_str());

    TestStreamType nonJsonFieldNonNestedNonPretty;
    Json::Put::separator<false, false, false, 3, Json::twoSpaces>(nonJsonFieldNonNestedNonPretty, false);
    EXPECT_STREQ(",", nonJsonFieldNonNestedNonPretty.str().c_str());
}

TEST_HEADER(JsonOutputPutAffixTest, Separator)
{
    TestStreamType jsonFirstPrettyCompare;
    jsonFirstPrettyCompare << osEndl;
    Json::Put::indent<true, Json::twoSpaces>(jsonFirstPrettyCompare, 3);
    TestStreamType jsonFirstPretty;
    Json::Put::separator<true, true, false, Json::twoSpaces>(jsonFirstPretty, true, 3);
    EXPECT_STREQ(jsonFirstPrettyCompare.str().c_str(), jsonFirstPretty.str().c_str());

    TestStreamType jsonFirstNonPretty;
    Json::Put::separator<false, true, false, Json::twoSpaces>(jsonFirstNonPretty, true, 3);
    EXPECT_STREQ("", jsonFirstNonPretty.str().c_str());

    TestStreamType jsonNonFirstPrettyCompare;
    jsonNonFirstPrettyCompare << "," << osEndl;
    Json::Put::indent<true, Json::twoSpaces>(jsonNonFirstPrettyCompare, 3);
    TestStreamType jsonNonFirstPretty;
    Json::Put::separator<true, true, false, Json::twoSpaces>(jsonNonFirstPretty, false, 3);
    EXPECT_STREQ(jsonNonFirstPrettyCompare.str().c_str(), jsonNonFirstPretty.str().c_str());

    TestStreamType jsonNonFirstNonPretty;
    Json::Put::separator<false, true, false, Json::twoSpaces>(jsonNonFirstNonPretty, false, 3);
    EXPECT_STREQ(",", jsonNonFirstNonPretty.str().c_str());
    
    TestStreamType nonJsonNonFirstNestedPrettyCompare;
    nonJsonNonFirstNestedPrettyCompare << "," << osEndl;
    Json::Put::indent<true, Json::twoSpaces>(nonJsonNonFirstNestedPrettyCompare, 3);
    TestStreamType nonJsonNonFirstNestedPretty;
    Json::Put::separator<true, false, true, Json::twoSpaces>(nonJsonNonFirstNestedPretty, false, 3);
    EXPECT_STREQ(nonJsonNonFirstNestedPrettyCompare.str().c_str(), nonJsonNonFirstNestedPretty.str().c_str());

    TestStreamType nonJsonNonFirstNestedNonPretty;
    Json::Put::separator<false, false, true, Json::twoSpaces>(nonJsonNonFirstNestedNonPretty, false, 3);
    EXPECT_STREQ(",", nonJsonNonFirstNestedNonPretty.str().c_str());

    TestStreamType nonJsonNonFirstNonNestedPretty;
    Json::Put::separator<true, false, false, Json::twoSpaces>(nonJsonNonFirstNonNestedPretty, false, 3);
    EXPECT_STREQ(", ", nonJsonNonFirstNonNestedPretty.str().c_str());

    TestStreamType nonJsonNonFirstNonNestedNonPretty;
    Json::Put::separator<false, false, false, Json::twoSpaces>(nonJsonNonFirstNonNestedNonPretty, false, 3);
    EXPECT_STREQ(",", nonJsonNonFirstNonNestedNonPretty.str().c_str());
}

struct PlaceholderAnnotations {};

TEST_HEADER(JsonOutputPut, Customization)
{
    using Notate = std::tuple<PlaceholderAnnotations>;
    int objectPlaceholder = 0;
    using Field = Reflect<CustomizeNoSpecialization>::MemberType::integer;
    CustomizeNoSpecialization customizeNoSpecialization {};
    bool customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customizeNoSpecialization, objectPlaceholder);
    EXPECT_FALSE(customized);

    CustomizeFullySpecialized customizeFullySpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customizeFullySpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize5ArgSpecialized customize5ArgSpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customize5ArgSpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize4ArgSpecialized customize4ArgSpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customize4ArgSpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize3ArgSpecialized customize3ArgSpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customize3ArgSpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize2ArgSpecialized customize2ArgSpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customize2ArgSpecialized, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize5ArgSpecialized_OpAnnotationsDefaulted customize5ArgSpecialized_opAnnotationsDefaulted {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customize5ArgSpecialized_opAnnotationsDefaulted, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize5ArgSpecialized_MemberIndexDefaulted customize5ArgSpecialized_memberIndexDefaulted {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customize5ArgSpecialized_memberIndexDefaulted, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize5ArgSpecialized_BothDefaulted customize5ArgSpecialized_bothDefaulted {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customize5ArgSpecialized_bothDefaulted, objectPlaceholder);
    EXPECT_TRUE(customized);

    Customize4ArgSpecialized_MemberIndexDefaulted customize4ArgSpecialized_memberIndexDefaulted {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, customize4ArgSpecialized_memberIndexDefaulted, objectPlaceholder);
    EXPECT_TRUE(customized);

    UnspecializedType unspecializedType {};
    ContainsUnspecialized containsUnspecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, containsUnspecialized, unspecializedType);
    EXPECT_FALSE(customized);

    FullySpecializedType fullySpecializedType {};
    ContainsFullySpecialized containsFullySpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, containsFullySpecialized, fullySpecializedType);
    EXPECT_TRUE(customized);

    ThreeArgSpecializedType threeArgSpecializedType {};
    ContainsThreeArgSpecialized containsThreeArgSpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, containsThreeArgSpecialized, threeArgSpecializedType);
    EXPECT_TRUE(customized);

    TwoArgSpecializedType twoArgSpecializedType {};
    ContainsTwoArgSpecialized containsTwoArgSpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, containsTwoArgSpecialized, twoArgSpecializedType);
    EXPECT_TRUE(customized);

    OneArgSpecializedType oneArgSpecializedType {};
    ContainsOneArgSpecialized containsOneArgSpecialized {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, containsOneArgSpecialized, oneArgSpecializedType);
    EXPECT_TRUE(customized);

    ThreeArgSpecializedType_OpAnnotationsDefaulted threeArgSpecializedType_opAnnotationsDefaulted {};
    ContainsThreeArgSpecializedType_OpAnnotationsDefaulted containsThreeArgSpecializedType_opAnnotationDefaulted {};
    customized = Json::Put::customization<Notate, Field, Json::Statics::Included, true, 3, Json::twoSpaces>(
        os, Json::defaultContext, containsThreeArgSpecializedType_opAnnotationDefaulted, threeArgSpecializedType_opAnnotationsDefaulted);
    EXPECT_TRUE(customized);
}

TEST_HEADER(JsonOutputPut, String)
{
    TestStreamType one, two, three, four, five, six, seven, eight, nine, ten, eleven;

    Json::Output::Put::string(one, "asdf");
    EXPECT_STREQ("\"asdf\"", one.str().c_str());
    
    Json::Output::Put::string(two, "\"");
    EXPECT_STREQ("\"\\\"\"", two.str().c_str());
    Json::Output::Put::string(three, "\\");
    EXPECT_STREQ("\"\\\\\"", three.str().c_str());
    Json::Output::Put::string(four, "/");
    EXPECT_STREQ("\"\\/\"", four.str().c_str());
    Json::Output::Put::string(five, "\b");
    EXPECT_STREQ("\"\\b\"", five.str().c_str());
    Json::Output::Put::string(six, "\f");
    EXPECT_STREQ("\"\\f\"", six.str().c_str());
    Json::Output::Put::string(seven, "\n");
    EXPECT_STREQ("\"\\n\"", seven.str().c_str());
    Json::Output::Put::string(eight, "\r");
    EXPECT_STREQ("\"\\r\"", eight.str().c_str());
    Json::Output::Put::string(nine, "\t");
    EXPECT_STREQ("\"\\t\"", nine.str().c_str());

    Json::Output::Put::string(ten, "");
    EXPECT_STREQ("\"\"", ten.str().c_str());

    Json::Output::Put::string(eleven, "asdf\"\\/\b\f\n\r\tjkl;");
    EXPECT_STREQ("\"asdf\\\"\\\\\\/\\b\\f\\n\\r\\tjkl;\"", eleven.str().c_str());
}

struct OstreamOverloaded {};

Json::OutStreamType & operator<<(Json::OutStreamType & oss, const OstreamOverloaded &)
{
    oss << "OstreamOverloaded";
    return oss;
}

TEST_HEADER(JsonOutputPut, StringTemplate)
{
    TestStreamType one, two;

    int integer = 1337;
    OstreamOverloaded ostreamOverloaded;
    
    Json::Output::Put::string(one, integer);
    EXPECT_STREQ("\"1337\"", one.str().c_str());

    Json::Output::Put::string(two, ostreamOverloaded);
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
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(booleanStream, Json::defaultContext, 0, boolean);
    EXPECT_STREQ("true", booleanStream.str().c_str());

    Json::Number number("123.456");
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(numberStream, Json::defaultContext, 0, number);
    EXPECT_STREQ("123.456", numberStream.str().c_str());

    Json::String str("asd\nf");
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(strStream, Json::defaultContext, 0, str);
    EXPECT_STREQ("\"asd\\nf\"", strStream.str().c_str());

    Json::Object obj;
    obj.put("astr", std::make_shared<Json::String>("asdf"));
    obj.put("null", nullptr);
    obj.put("number", std::make_shared<Json::Number>("1234"));
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(objStream, Json::defaultContext, 0, obj);
    EXPECT_STREQ("{\"astr\":\"asdf\",\"null\":null,\"number\":1234}", objStream.str().c_str());

    Json::NullArray nullArray(3);
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(nullArrayStream, Json::defaultContext, 0, nullArray);
    EXPECT_STREQ("[null,null,null]", nullArrayStream.str().c_str());

    Json::BoolArray boolArray;
    boolArray.boolArray().push_back(true);
    boolArray.boolArray().push_back(true);
    boolArray.boolArray().push_back(false);
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(boolArrayStream, Json::defaultContext, 0, boolArray);
    EXPECT_STREQ("[true,true,false]", boolArrayStream.str().c_str());

    Json::NumberArray numberArray;
    numberArray.numberArray().push_back("2");
    numberArray.numberArray().push_back("1");
    numberArray.numberArray().push_back("0.5");
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(numberArrayStream, Json::defaultContext, 0, numberArray);
    EXPECT_STREQ("[2,1,0.5]", numberArrayStream.str().c_str());

    Json::StringArray strArray;
    strArray.stringArray().push_back("as");
    strArray.stringArray().push_back("df");
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(strArrayStream, Json::defaultContext, 0, strArray);
    EXPECT_STREQ("[\"as\",\"df\"]", strArrayStream.str().c_str());

    Json::ObjectArray objArray;
    Json::ObjectValue objElement;
    objElement.insert(std::pair("astr", std::make_shared<Json::String>("asdf")));
    objElement.insert(std::pair("null", nullptr));
    objElement.insert(std::pair("number", std::make_shared<Json::Number>("1234")));
    objArray.objectArray().push_back(objElement);
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(objArrayStream, Json::defaultContext, 0, objArray);
    EXPECT_STREQ("[{\"astr\":\"asdf\",\"null\":null,\"number\":1234}]", objArrayStream.str().c_str());

    Json::MixedArray mixedArray;
    mixedArray.mixedArray().push_back(std::make_shared<Json::Bool>(true));
    mixedArray.mixedArray().push_back(nullptr);
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(mixedArrayStream, Json::defaultContext, 0, mixedArray);
    EXPECT_STREQ("[true,null]", mixedArrayStream.str().c_str());

    Json::FieldCluster fieldCluster;
    fieldCluster.put("astr", std::make_shared<Json::String>("asdf"));
    fieldCluster.put("number", std::make_shared<Json::Number>("1234"));
    Json::Put::genericValue<NoNote, false, Json::twoSpaces, true>(fieldClusterStream, Json::defaultContext, 0, fieldCluster);
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
    obj.put("astr", std::make_shared<Json::String>("asdf"));
    obj.put("number", std::make_shared<Json::Number>("1234"));
    Json::Put::genericIterable<NoNote, false, Json::twoSpaces>(objStream, Json::defaultContext, 3, obj);
    EXPECT_STREQ("{\"astr\":\"asdf\",\"number\":1234}", objStream.str().c_str());

    Json::NullArray nullArray(3);
    Json::Put::genericIterable<NoNote, false, Json::twoSpaces>(nullArrayStream, Json::defaultContext, 3, nullArray);
    EXPECT_STREQ("[null,null,null]", nullArrayStream.str().c_str());

    Json::BoolArray boolArray;
    boolArray.boolArray().push_back(true);
    boolArray.boolArray().push_back(true);
    boolArray.boolArray().push_back(false);
    Json::Put::genericIterable<NoNote, false, Json::twoSpaces>(boolArrayStream, Json::defaultContext, 3, boolArray);
    EXPECT_STREQ("[true,true,false]", boolArrayStream.str().c_str());

    Json::NumberArray numberArray;
    numberArray.numberArray().push_back("2");
    numberArray.numberArray().push_back("1");
    numberArray.numberArray().push_back("0.5");
    Json::Put::genericIterable<NoNote, false, Json::twoSpaces>(numberArrayStream, Json::defaultContext, 3, numberArray);
    EXPECT_STREQ("[2,1,0.5]", numberArrayStream.str().c_str());

    Json::StringArray stringArray;
    stringArray.stringArray().push_back("as");
    stringArray.stringArray().push_back("df");
    Json::Put::genericIterable<NoNote, false, Json::twoSpaces>(strArrayStream, Json::defaultContext, 3, stringArray);
    EXPECT_STREQ("[\"as\",\"df\"]", strArrayStream.str().c_str());

    Json::ObjectArray objArray;
    Json::ObjectValue objElement;
    objElement.insert(std::pair("astr", std::make_shared<Json::String>("asdf")));
    objElement.insert(std::pair("number", std::make_shared<Json::Number>("1234")));
    objArray.objectArray().push_back(objElement);
    Json::Put::genericIterable<NoNote, false, Json::twoSpaces>(objArrayStream, Json::defaultContext, 3, objArray);
    EXPECT_STREQ("[{\"astr\":\"asdf\",\"number\":1234}]", objArrayStream.str().c_str());

    Json::MixedArray mixedArray;
    mixedArray.mixedArray().push_back(std::make_shared<Json::Bool>(true));
    mixedArray.mixedArray().push_back(nullptr);
    Json::Put::genericIterable<NoNote, false, Json::twoSpaces>(mixedArrayStream, Json::defaultContext, 3, mixedArray);
    EXPECT_STREQ("[true,null]", mixedArrayStream.str().c_str());
}

enum class AnEnum : uint16_t { first, second };

struct AnObject
{
    int first;
    std::string second;
    char third;
};

struct MappedTo
{
    int a = 0;

    REFLECT(MappedTo, a)
};

struct MappedFrom
{
    int a = 0;

    REFLECT(MappedFrom, a)

    NOTE(MappedFrom, RareMapper::createMapping<MappedFrom, MappedTo>().a->a().bidirectional())
};

struct Keyable
{
    int a;

    REFLECT(Keyable, a)
};

bool operator<(const Keyable & lhs, const Keyable & rhs)
{
    return lhs.a < rhs.a;
}

TEST_HEADER(JsonOutputPut, Value)
{
    TestStreamType customizedStream,
        mappedObjStream,
        nullPointerStream,
        intPointerStream,
        constIntPointerStream,
        intConstPointerStream,
        constIntConstPointerStream,
        genericNumberStream,
        intVectorStream,
        intMapStream,
        rootObjStream,
        reflectedObjStream,
        strStream,
        enumIntStream,
        booleanStream,
        shortIntStream;

    using AMember = Json::MockMember<int>;

    int placeholderObj = 0;
    CustomizeFullySpecialized customized;
    customized.firstField = 1337;
    Json::Put::value<NoNote, Reflect<CustomizeFullySpecialized>::MemberType::firstField,
        Json::Statics::Excluded, false, 0, Json::twoSpaces, CustomizeFullySpecialized, true>
        (customizedStream, Json::defaultContext, customized, customized.firstField);
    EXPECT_STREQ("\"Customized1337\"", customizedStream.str().c_str());

    MappedFrom mappedFrom { 1 };
    using OpNoteWithMapping = std::tuple<RareMapper::UseMapping<MappedFrom, MappedTo>>;
    Json::Put::value<OpNoteWithMapping, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(mappedObjStream, Json::defaultContext, placeholderObj, mappedFrom);
    EXPECT_STREQ("{\"a\":1}", mappedObjStream.str().c_str());

    int* nullPointer = nullptr;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(nullPointerStream, Json::defaultContext, placeholderObj, nullPointer);
    EXPECT_STREQ("null", nullPointerStream.str().c_str());

    int anInt = 555;
    int* intPointer = &anInt;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intPointerStream, Json::defaultContext, placeholderObj, intPointer);
    EXPECT_STREQ("555", intPointerStream.str().c_str());

    anInt = 556;
    const int* constIntPointer = &anInt;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(constIntPointerStream, Json::defaultContext, placeholderObj, constIntPointer);
    EXPECT_STREQ("556", constIntPointerStream.str().c_str());
    
    anInt = 557;
    int* const intConstPointer = &anInt;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intConstPointerStream, Json::defaultContext, placeholderObj, intConstPointer);
    EXPECT_STREQ("557", intConstPointerStream.str().c_str());
    
    anInt = 558;
    const int* const constIntConstPointer = &anInt;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(constIntConstPointerStream, Json::defaultContext, placeholderObj, constIntConstPointer);
    EXPECT_STREQ("558", constIntConstPointerStream.str().c_str());

    Json::Number number("123.456");
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(genericNumberStream, Json::defaultContext, placeholderObj, number);
    EXPECT_STREQ("123.456", genericNumberStream.str().c_str());

    std::vector<int> intVector = { 1, 2, 3 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intVectorStream, Json::defaultContext, placeholderObj, intVector);
    EXPECT_STREQ("[1,2,3]", intVectorStream.str().c_str());

    std::map<int, int> intMap = { {1, 2}, {3, 4}, {5, 6} };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intMapStream, Json::defaultContext, placeholderObj, intMap);
    EXPECT_STREQ("{\"1\":2,\"3\":4,\"5\":6}", intMapStream.str().c_str());

    InstanceField rootObj;
    rootObj.a = 5;
    Json::Put::value<NoNote, Json::MockMember<InstanceField>, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(rootObjStream, Json::defaultContext, placeholderObj, rootObj);
    EXPECT_STREQ("{\"a\":5}", rootObjStream.str().c_str());

    InstanceField obj;
    obj.a = 6;
    Json::Put::value<NoNote, Json::MockMember<InstanceField>, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(reflectedObjStream, Json::defaultContext, placeholderObj, obj);
    EXPECT_STREQ("{\"a\":6}", reflectedObjStream.str().c_str());

    std::string str = "asdf";
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(strStream, Json::defaultContext, placeholderObj, str);
    EXPECT_STREQ("\"asdf\"", strStream.str().c_str());

    AnEnum enumInt = AnEnum::first;
    Json::Put::value<NoNote, Json::MockMember<AnEnum>, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(enumIntStream, Json::defaultContext, placeholderObj, enumInt);
    EXPECT_STREQ("0", enumIntStream.str().c_str());

    bool boolean = false;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(booleanStream, Json::defaultContext, placeholderObj, boolean);
    EXPECT_STREQ("false", booleanStream.str().c_str());

    short shortInt = 22222;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(shortIntStream, Json::defaultContext, placeholderObj, shortInt);
    EXPECT_STREQ("22222", shortIntStream.str().c_str());

    
    TestStreamType emptyTupleStream,
        intTupleStream,
        intIntTupleStream,
        intIntIntTupleStream,
        intIntPairStream,
        intIntMapStream,
        intKeyableMapStream,
        keyableIntMapStream,
        keyableKeyableMapStream,
        intIntTupleIntTupleStream;

    Keyable keyable { 0 };
    Keyable otherKey { 1 };

    std::tuple<> shouldBeNull;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(emptyTupleStream, Json::defaultContext, placeholderObj, shouldBeNull);
    EXPECT_STREQ("null", emptyTupleStream.str().c_str());

    std::tuple<int> shouldBeInt { 1337 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intTupleStream, Json::defaultContext, placeholderObj, shouldBeInt);
    EXPECT_STREQ("1337", intTupleStream.str().c_str());

    std::tuple<int, int> shouldBe2Array { 1, 2 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntTupleStream, Json::defaultContext, placeholderObj, shouldBe2Array);
    EXPECT_STREQ("[1,2]", intIntTupleStream.str().c_str());

    std::tuple<int, int, int> shouldBe3Array { 1, 2, 3 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntIntTupleStream, Json::defaultContext, placeholderObj, shouldBe3Array);
    EXPECT_STREQ("[1,2,3]", intIntIntTupleStream.str().c_str());

    std::pair<int, int> pair { 9000, 9001 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntPairStream, Json::defaultContext, placeholderObj, pair);
    EXPECT_STREQ("[9000,9001]", intIntPairStream.str().c_str());

    std::map<int, int> basicMap;
    basicMap.insert(std::make_pair(1, 2));
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntMapStream, Json::defaultContext, placeholderObj, basicMap);
    EXPECT_STREQ("{\"1\":2}", intIntMapStream.str().c_str());

    std::map<int, Keyable> primitiveObjMap;
    primitiveObjMap.insert(std::make_pair(1, keyable));
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intKeyableMapStream, Json::defaultContext, placeholderObj, primitiveObjMap);
    EXPECT_STREQ("{\"1\":{\"a\":0}}", intKeyableMapStream.str().c_str());

    std::map<Keyable, int> compKey;
    compKey.insert(std::make_pair(keyable, 1));
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(keyableIntMapStream, Json::defaultContext, placeholderObj, compKey);
    EXPECT_STREQ("[{\"key\":{\"a\":0},\"value\":1}]", keyableIntMapStream.str().c_str());

    std::map<Keyable, Keyable> compMap;
    compMap.insert(std::make_pair(keyable, otherKey));
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(keyableKeyableMapStream, Json::defaultContext, placeholderObj, compMap);
    EXPECT_STREQ("[{\"key\":{\"a\":0},\"value\":{\"a\":1}}]", keyableKeyableMapStream.str().c_str());

    std::tuple<std::tuple<int, int>, int> tup { {1, 2}, 3 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntTupleIntTupleStream, Json::defaultContext, placeholderObj, tup);
    EXPECT_STREQ("[[1,2],3]", intIntTupleIntTupleStream.str().c_str());
}

TEST_HEADER(JsonOutputPut, Tuple)
{
    TestStreamType emptyTupleStream,
        intTupleStream,
        intIntTupleStream,
        intIntIntTupleStream,
        intIntTupleIntTupleStream,
        intVectorTupleStream,
        intObjectTupleStream;

    using AMember = Json::MockMember<int>;
    
    int placeholderObj = 0;

    std::tuple<> shouldBeNull;
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(emptyTupleStream, Json::defaultContext, placeholderObj, shouldBeNull);
    EXPECT_STREQ("null", emptyTupleStream.str().c_str());

    std::tuple<int> shouldBeInt { 1337 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intTupleStream, Json::defaultContext, placeholderObj, shouldBeInt);
    EXPECT_STREQ("1337", intTupleStream.str().c_str());

    std::tuple<int, int> shouldBe2Array { 1, 2 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntTupleStream, Json::defaultContext, placeholderObj, shouldBe2Array);
    EXPECT_STREQ("[1,2]", intIntTupleStream.str().c_str());

    std::tuple<int, int, int> shouldBe3Array { 1, 2, 3 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntIntTupleStream, Json::defaultContext, placeholderObj, shouldBe3Array);
    EXPECT_STREQ("[1,2,3]", intIntIntTupleStream.str().c_str());

    std::tuple<std::tuple<int, int>, int> tup { {1, 2}, 3 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntTupleIntTupleStream, Json::defaultContext, placeholderObj, tup);
    EXPECT_STREQ("[[1,2],3]", intIntTupleIntTupleStream.str().c_str());

    std::tuple<int, std::vector<int>> intVectorTuple { 1, {2, 3, 4} };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intVectorTupleStream, Json::defaultContext, placeholderObj, intVectorTuple);
    EXPECT_STREQ("[1,[2,3,4]]", intVectorTupleStream.str().c_str());

    std::tuple<int, Keyable> intObjectTuple { 1, {2} };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intObjectTupleStream, Json::defaultContext, placeholderObj, intObjectTuple);
    EXPECT_STREQ("[1,{\"a\":2}]", intObjectTupleStream.str().c_str());
}

TEST_HEADER(JsonOutputPut, Pair)
{
    TestStreamType intIntPairStream,
        intVectorPairStream,
        intObjectPairStream,
        intPairPairStream;

    using AMember = Json::MockMember<int>;
    
    int placeholderObj = 0;
    
    std::pair<int, int> intIntPair { 0, 1 };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntPairStream, Json::defaultContext, placeholderObj, intIntPair);
    EXPECT_STREQ("[0,1]", intIntPairStream.str().c_str());

    std::pair<int, std::vector<int>> intVectorPair { 0, {1, 2, 3} };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intVectorPairStream, Json::defaultContext, placeholderObj, intVectorPair);
    EXPECT_STREQ("[0,[1,2,3]]", intVectorPairStream.str().c_str());

    std::pair<int, Keyable> intObjectPair { 0, {1} };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intObjectPairStream, Json::defaultContext, placeholderObj, intObjectPair);
    EXPECT_STREQ("[0,{\"a\":1}]", intObjectPairStream.str().c_str());

    std::pair<int, std::pair<int, int>> intPairPair { 0, {1, 2} };
    Json::Put::value<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intPairPairStream, Json::defaultContext, placeholderObj, intPairPair);
    EXPECT_STREQ("[0,[1,2]]", intPairPairStream.str().c_str());
}

TEST_HEADER(JsonOutputPut, KeyValueObject)
{
    TestStreamType intIntPairStream,
        objObjPairStream;

    using AMember = Json::MockMember<int>;
    
    int placeholderObj = 0;

    std::pair<int, int> intIntPair { 0, 1 };
    Json::Put::keyValueObject<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntPairStream, Json::defaultContext, placeholderObj, intIntPair);
    EXPECT_STREQ("{\"key\":0,\"value\":1}", intIntPairStream.str().c_str());

    std::pair<Keyable, Keyable> objObjPair { {0}, {1} };
    Json::Put::keyValueObject<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(objObjPairStream, Json::defaultContext, placeholderObj, objObjPair);
    EXPECT_STREQ("{\"key\":{\"a\":0},\"value\":{\"a\":1}}", objObjPairStream.str().c_str());
}

TEST_HEADER(JsonOutputPut, FieldPair)
{
    TestStreamType intIntPairStream,
        objObjPairStream;

    using AMember = Json::MockMember<int>;
    
    int placeholderObj = 0;

    std::pair<int, int> intIntPair { 0, 1 };
    Json::Put::fieldPair<NoNote, AMember, Json::Statics::Excluded, false, 0, Json::twoSpaces, int, true>(intIntPairStream, Json::defaultContext, placeholderObj, intIntPair);
    EXPECT_STREQ("\"0\":1", intIntPairStream.str().c_str());
}

struct ContainsIterables
{
    std::vector<int> intVector;
    int intArray[3];
    std::array<int, 2> stlArray;
    std::queue<int> intQueue;

    REFLECT(ContainsIterables, intVector, intArray, stlArray, intQueue)
};

TEST_HEADER(JsonOutputPut, Iterable)
{
    TestStreamType intVectorStream,
        intQueueStream,
        intArrayStream,
        stlIntArrayStream;

    ContainsIterables containsIterables = {
        { 1, 2, 3 },
        { 3, 4, 5 },
        { 4, 5 },
        {}
    };
    containsIterables.intQueue.push(2);
    containsIterables.intQueue.push(3);
    containsIterables.intQueue.push(4);

    Json::Put::iterable<NoNote, Reflect<ContainsIterables>::MemberType::intVector, Json::Statics::Excluded, false, 0, Json::twoSpaces>(
        intVectorStream, Json::defaultContext, containsIterables, containsIterables.intVector);
    EXPECT_STREQ("[1,2,3]", intVectorStream.str().c_str());

    Json::Put::iterable<NoNote, Reflect<ContainsIterables>::MemberType::intQueue, Json::Statics::Excluded, false, 0, Json::twoSpaces>(
        intQueueStream, Json::defaultContext, containsIterables, containsIterables.intQueue);
    EXPECT_STREQ("[2,3,4]", intQueueStream.str().c_str());

    Json::Put::iterable<NoNote, Reflect<ContainsIterables>::MemberType::intArray, Json::Statics::Excluded, false, 0, Json::twoSpaces>(
        intArrayStream, Json::defaultContext, containsIterables, containsIterables.intArray);
    EXPECT_STREQ("[3,4,5]", intArrayStream.str().c_str());

    Json::Put::iterable<NoNote, Reflect<ContainsIterables>::MemberType::stlArray, Json::Statics::Excluded, false, 0, Json::twoSpaces>(
        stlIntArrayStream, Json::defaultContext, containsIterables, containsIterables.stlArray);
    EXPECT_STREQ("[4,5]", stlIntArrayStream.str().c_str());
}

struct RegularFields
{
    int regular;

    NOTE(regularRenamed, Json::Name{"regular2"})
    int regularRenamed;

    NOTE(regularIgnored, Json::Ignore)
    int regularIgnored;
    
    Json::FieldCluster regularFieldCluster;

    REFLECT(RegularFields, regular, regularRenamed, regularIgnored, regularFieldCluster)
};

struct FieldClusterOnly
{
    Json::FieldCluster onlyFieldCluster;

    REFLECT(FieldClusterOnly, onlyFieldCluster)
};

struct FieldClusterFirst
{
    Json::FieldCluster firstFieldCluster;
    int regular;

    REFLECT(FieldClusterFirst, firstFieldCluster, regular)
};

struct FieldClusterPointer
{
    int regular;
    std::unique_ptr<Json::FieldCluster> fieldClusterPointer;
    int otherRegular;

    REFLECT(FieldClusterPointer, regular, fieldClusterPointer, otherRegular)
};

TEST_HEADER(JsonOutputPut, Field)
{
    TestStreamType putRegular;
    RegularFields regularFields = {1, 2, 3, {}};
    Json::Put::field<NoNote, Reflect<RegularFields>::MemberType::regular, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegular, Json::defaultContext, regularFields, "regular", regularFields.regular);
    EXPECT_STREQ("\"regular\":1", putRegular.str().c_str());

    TestStreamType putRegularRenamed;
    Json::Put::field<NoNote, Reflect<RegularFields>::MemberType::regularRenamed, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularRenamed, Json::defaultContext, regularFields, "regular2", regularFields.regularRenamed);
    EXPECT_STREQ(",\"regular2\":2", putRegularRenamed.str().c_str());

    TestStreamType putRegularIgnored;
    Json::Put::field<NoNote, Reflect<RegularFields>::MemberType::regularIgnored, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularIgnored, Json::defaultContext, regularFields, "regularIgnored", regularFields.regularIgnored);
    EXPECT_STREQ(",\"regularIgnored\":3", putRegularIgnored.str().c_str());

    TestStreamType putRegularEmptyCluster;
    Json::Put::field<NoNote, Reflect<RegularFields>::MemberType::regularFieldCluster, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularEmptyCluster, Json::defaultContext, regularFields, "regularFieldCluster", regularFields.regularFieldCluster);
    EXPECT_STREQ("", putRegularEmptyCluster.str().c_str());

    TestStreamType putRegularCluster;
    regularFields.regularFieldCluster.put("aField", std::make_shared<Json::String>("aString"));
    Json::Put::field<NoNote, Reflect<RegularFields>::MemberType::regularFieldCluster, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularCluster, Json::defaultContext, regularFields, "regularFieldCluster", regularFields.regularFieldCluster);
    EXPECT_STREQ(",\"aField\":\"aString\"", putRegularCluster.str().c_str());

    TestStreamType putOnlyCluster;
    FieldClusterOnly fieldClusterOnly;
    bool visited = false;
    try {
        Json::Put::field<NoNote, Reflect<FieldClusterOnly>::MemberType::onlyFieldCluster, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterOnly>(
            putOnlyCluster, Json::defaultContext, fieldClusterOnly, "onlyFieldCluster", fieldClusterOnly.onlyFieldCluster);
    } catch ( Json::Exception & ) {
        visited = true;
    }
    EXPECT_TRUE(visited);

    TestStreamType putClusterFirst;
    FieldClusterFirst fieldClusterFirst = {};
    visited = false;
    try {
        Json::Put::field<NoNote, Reflect<FieldClusterFirst>::MemberType::firstFieldCluster, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterFirst>(
            putOnlyCluster, Json::defaultContext, fieldClusterFirst, "firstFieldCluster", fieldClusterFirst.firstFieldCluster);
    } catch ( Json::Exception & ) {
        visited = true;
    }
    EXPECT_TRUE(visited);

    TestStreamType putClusterNullPointer;
    FieldClusterPointer fieldClusterPointer = {};
    fieldClusterPointer.fieldClusterPointer = nullptr;
    Json::Put::field<NoNote, Reflect<FieldClusterPointer>::MemberType::fieldClusterPointer, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterPointer>(
        putClusterNullPointer, Json::defaultContext, fieldClusterPointer, "fieldClusterPointer", fieldClusterPointer.fieldClusterPointer);
    EXPECT_STREQ("", putClusterNullPointer.str().c_str());

    TestStreamType putClusterPointerEmpty;
    fieldClusterPointer.fieldClusterPointer = std::make_unique<Json::FieldCluster>();
    Json::Put::field<NoNote, Reflect<FieldClusterPointer>::MemberType::fieldClusterPointer, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterPointer>(
        putClusterPointerEmpty, Json::defaultContext, fieldClusterPointer, "fieldClusterPointer", fieldClusterPointer.fieldClusterPointer);
    EXPECT_STREQ("", putClusterPointerEmpty.str().c_str());

    TestStreamType putClusterPointerNonEmpty;
    fieldClusterPointer.fieldClusterPointer->put("one", nullptr);
    fieldClusterPointer.fieldClusterPointer->put("two", std::make_shared<Json::Bool>(true));
    Json::Put::field<NoNote, Reflect<FieldClusterPointer>::MemberType::fieldClusterPointer, Json::Statics::Excluded, false, 0, Json::twoSpaces, FieldClusterPointer>(
        putClusterPointerNonEmpty, Json::defaultContext, fieldClusterPointer, "fieldClusterPointer", fieldClusterPointer.fieldClusterPointer);
    EXPECT_STREQ(",\"one\":null,\"two\":true", putClusterPointerNonEmpty.str().c_str());
}

TEST_HEADER(JsonOutputPut, Fields)
{
    TestStreamType putRegularFields;
    RegularFields regularFields = { 1, 2, 3, {} };
    Json::Put::fields<NoNote, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putRegularFields, Json::defaultContext, regularFields);
    EXPECT_STREQ("\"regular\":1,\"regular2\":2", putRegularFields.str().c_str());

    TestStreamType putMultipleRegularFields;
    regularFields.regularFieldCluster.put("unknown", std::make_shared<Json::String>("field"));
    Json::Put::fields<NoNote, Json::Statics::Excluded, false, 0, Json::twoSpaces, RegularFields>(
        putMultipleRegularFields, Json::defaultContext, regularFields);
    EXPECT_STREQ("\"regular\":1,\"regular2\":2,\"unknown\":\"field\"", putMultipleRegularFields.str().c_str());
}

TEST_HEADER(JsonOutputPut, SuperTest)
{
    TestStreamType putNoSupers;
    NestedEmptySuper emptySuper;
    Json::Put::super<NoNote, 0, EmptySuper, Json::Statics::Excluded, false, 0, Json::twoSpaces, NestedEmptySuper>(
        putNoSupers, Json::defaultContext, emptySuper, "__EmptySuper");
    EXPECT_STREQ("", putNoSupers.str().c_str());

    TestStreamType putFieldedSuper;
    NestedFieldedSuper nestedFieldedSuper;
    nestedFieldedSuper.a = 555;
    Json::Put::super<NoNote, 0, FieldedSuper, Json::Statics::Excluded, false, 0, Json::twoSpaces, NestedFieldedSuper>(
        putFieldedSuper, Json::defaultContext, nestedFieldedSuper, "__FieldedSuper");
    EXPECT_TRUE(std::regex_match(putFieldedSuper.str().c_str(), std::regex("\"__.*FieldedSuper\":\\{\"a\":555\\}")));

    TestStreamType putDoublyNestedSuper;
    DoublyNestedFieldedSuper doublyNestedFieldedSuper;
    doublyNestedFieldedSuper.a = 444;
    Json::Put::super<NoNote, 0, NestedFieldedSuper, Json::Statics::Excluded, false, 0, Json::twoSpaces, DoublyNestedFieldedSuper>(
        putDoublyNestedSuper, Json::defaultContext, doublyNestedFieldedSuper, "__NestedFieldedSuper");
    EXPECT_TRUE(std::regex_match(putDoublyNestedSuper.str().c_str(), std::regex("\"__.*NestedFieldedSuper\":\\{\"__.*FieldedSuper\":\\{\"a\":444\\}\\}")));
}

TEST_HEADER(JsonOutputPut, SupersTest)
{
    TestStreamType putNoSupers;
    EmptySuper emptySuper;
    Json::Put::supers<NoNote, Json::Statics::Included, false, 0, Json::twoSpaces, EmptySuper>(putNoSupers, Json::defaultContext, emptySuper);
    EXPECT_STREQ("", putNoSupers.str().c_str());

    TestStreamType putEmptySupers;
    NestedEmptySuper nestedEmptySuper;
    Json::Put::supers<NoNote, Json::Statics::Included, false, 0, Json::twoSpaces, NestedEmptySuper>(putEmptySupers, Json::defaultContext, nestedEmptySuper);
    EXPECT_STREQ("", putEmptySupers.str().c_str());

    TestStreamType putDoubleSuper;
    DoubleSuper doubleSuper;
    doubleSuper.a = 111;
    Json::Put::supers<NoNote, Json::Statics::Included, false, 0, Json::twoSpaces, DoubleSuper>(putDoubleSuper, Json::defaultContext, doubleSuper);
    EXPECT_TRUE(std::regex_match(putDoubleSuper.str().c_str(), std::regex("\"Fielded\":\\{\"a\":111\\}")));

    TestStreamType putTripleSuper;
    TripleSuper tripleSuper;
    tripleSuper.a = 222;
    Json::Put::supers<NoNote, Json::Statics::Included, false, 0, Json::twoSpaces, TripleSuper>(putTripleSuper, Json::defaultContext, tripleSuper);
    EXPECT_TRUE(std::regex_match(putTripleSuper.str().c_str(), std::regex("\"__.*FieldedSuper\":\\{\"a\":222\\}")));
}

struct NestedObj
{
    NOTE(boolean, Json::Name{"bool"})
    bool boolean;

    int ray[3];

    REFLECT(NestedObj, boolean, ray)
};

class AnObjectTest
{
    int integer;
    std::string str;
    NestedObj nestedObj;

    REFLECT(AnObjectTest, integer, str, nestedObj)

public:
    AnObjectTest(int integer, std::string str, NestedObj nestedObj) : integer(integer), str(str), nestedObj(nestedObj) {}
};

TEST_HEADER(JsonOutputPut, Object)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    TestStreamType objStream;
    Json::Put::object<NoNote, Json::Statics::Included, false, 0, Json::twoSpaces, AnObjectTest>(objStream, Json::defaultContext, anObject);
    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"bool\":false,\"ray\":[1,2,3]}}", objStream.str().c_str());
}

TEST_HEADER(JsonOutputTest, JsonOutputReflectedObject)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };
    Json::Output::ReflectedObject<NoNote, Statics::Included, false, 0, Json::twoSpaces, AnObjectTest> reflectedObj(anObject, nullptr);
    TestStreamType objStream;
    reflectedObj.put(objStream);

    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"bool\":false,\"ray\":[1,2,3]}}", objStream.str().c_str());
}

struct OptObj
{
    int a;

    REFLECT(OptObj, a)
};

struct Optional
{
    std::optional<int> a = std::nullopt;
    std::optional<OptObj> b = std::nullopt;

    REFLECT(Optional, a, b)
};

TEST_HEADER(JsonOutputTest, JsonOutOpt)
{
    Optional nullOpt {};
    TestStreamType nullOptStream {};
    nullOptStream << Json::out(nullOpt);
    EXPECT_STREQ("{\"a\":null,\"b\":null}", nullOptStream.str().c_str());

    Optional valueOpt { 1, OptObj{2} };
    TestStreamType valueOptStream {};
    valueOptStream << Json::out(valueOpt);
    EXPECT_STREQ("{\"a\":1,\"b\":{\"a\":2}}", valueOptStream.str().c_str());
}

struct SomeArrays
{
    int a[3] {1, 2, 3};
    int b[2][3] {{4, 5, 6}, {7, 8, 9}};
    int c[2][3][1] {{{4}, {5}, {6}}, {{7}, {8}, {9}}};
    std::array<int, 2> d {4, 5};

    REFLECT(SomeArrays, a, b, c, d)
};

TEST_HEADER(JsonOutputTest, JsonOut)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };
    auto reflectedObj = Json::out(anObject);
    TestStreamType objStream;
    reflectedObj.put(objStream);
    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"bool\":false,\"ray\":[1,2,3]}}", objStream.str().c_str());

    TestStreamType finalObjStream;
    finalObjStream << Json::out(anObject);
    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"bool\":false,\"ray\":[1,2,3]}}", finalObjStream.str().c_str());

    TestStreamType arrayStream;
    arrayStream << Json::out(SomeArrays{});
    EXPECT_STREQ("{\"a\":[1,2,3],\"b\":[[4,5,6],[7,8,9]],\"c\":[[[4],[5],[6]],[[7],[8],[9]]],\"d\":[4,5]}", arrayStream.str().c_str());
}

TEST_HEADER(JsonOutputTest, OutProxyReflected)
{
    ProxyReflectedObject obj = { 5 };

    TestStreamType objStream;
    objStream << Json::out(obj);
    EXPECT_STREQ("{\"a\":5}", objStream.str().c_str());
}

struct JsonReferences
{
    JsonReferences() : primitive(11), primitiveReference(primitive) {}

    int primitive;
    int & primitiveReference;
    static int staticPrimitive;
    static constexpr int & staticPrimitiveReference = staticPrimitive;

    REFLECT(JsonReferences, primitive, primitiveReference, staticPrimitive, staticPrimitiveReference)
};

int JsonReferences::staticPrimitive = 33;

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
        << "    \"bool\": false," << osEndl
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

TEST_HEADER(JsonOutputTest, JsonWriteRef)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    std::string output {};
    Json::write(anObject, output);
    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"bool\":false,\"ray\":[1,2,3]}}", output.c_str());
}

TEST_HEADER(JsonOutputTest, JsonWriteReturn)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };

    std::string output = Json::write(anObject);
    EXPECT_STREQ("{\"integer\":4,\"str\":\"aString\",\"nestedObj\":{\"bool\":false,\"ray\":[1,2,3]}}", output.c_str());
}

TEST_HEADER(JsonOutputTest, JsonWritePrettyRef)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };
    TestStreamType objStreamCompare;
    objStreamCompare
        << "{" << osEndl
        << "  \"integer\": 4," << osEndl
        << "  \"str\": \"aString\"," << osEndl
        << "  \"nestedObj\": {" << osEndl
        << "    \"bool\": false," << osEndl
        << "    \"ray\": [ 1, 2, 3 ]" << osEndl
        << "  }" << osEndl
        << "}";

    std::string output {};
    Json::writePretty(anObject, output);
    EXPECT_STREQ(objStreamCompare.str().c_str(), output.c_str());
}

TEST_HEADER(JsonOutputTest, JsonWritePrettyReturn)
{
    NestedObj nestedObj = { false, { 1, 2, 3 } };
    AnObjectTest anObject = { 4, "aString", nestedObj };
    TestStreamType objStreamCompare;
    objStreamCompare
        << "{" << osEndl
        << "  \"integer\": 4," << osEndl
        << "  \"str\": \"aString\"," << osEndl
        << "  \"nestedObj\": {" << osEndl
        << "    \"bool\": false," << osEndl
        << "    \"ray\": [ 1, 2, 3 ]" << osEndl
        << "  }" << osEndl
        << "}";

    std::string output = Json::writePretty(anObject);
    EXPECT_STREQ(objStreamCompare.str().c_str(), output.c_str());
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