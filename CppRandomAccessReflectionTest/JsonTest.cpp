#include <gtest/gtest.h>
#include "../CppRandomAccessReflectionLib/Json.h"
#include <regex>
using namespace Reflect;
using Json::Statics;

struct IsSpecializedTest {};

struct IsUnspecializedTest : Json::Unspecialized {};

TEST(JsonAnnotationsTest, IsSpecialized)
{
    EXPECT_TRUE(Json::is_specialized<IsSpecializedTest>::value);
    EXPECT_FALSE(Json::is_specialized<IsUnspecializedTest>::value);
}

TEST(JsonSharedTest, MatchesStatics)
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

TEST(JsonSharedTest, IgnoredFieldCount)
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

TEST(JsonSharedTest, HasFields)
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
    
}

TEST(JsonSharedTest, FirstIndex)
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

TEST(JsonSharedTest, LastIndex)
{
    size_t firstIndex = Json::LastIndex<Statics::Excluded, NoFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, InstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, StaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, BothFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, MoreInstance>();
    EXPECT_EQ(1, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, MoreStatic>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, IgnoreInstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, IgnoreStaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, IgnoreBothField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, IgnoreMixed>();
    EXPECT_EQ(1, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, OnlyIgnoreStatics>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Excluded, OnlyIgnoreInstance>();
    EXPECT_EQ(0, firstIndex);

    firstIndex = Json::LastIndex<Statics::Included, NoFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, InstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, StaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, BothFields>();
    EXPECT_EQ(1, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, MoreInstance>();
    EXPECT_EQ(2, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, MoreStatic>();
    EXPECT_EQ(2, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, IgnoreInstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, IgnoreStaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, IgnoreBothField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, IgnoreMixed>();
    EXPECT_EQ(4, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, OnlyIgnoreStatics>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Included, OnlyIgnoreInstance>();
    EXPECT_EQ(1, firstIndex);
    
    firstIndex = Json::LastIndex<Statics::Only, NoFields>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, InstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, StaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, BothFields>();
    EXPECT_EQ(1, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, MoreInstance>();
    EXPECT_EQ(2, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, MoreStatic>();
    EXPECT_EQ(2, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, IgnoreInstanceField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, IgnoreStaticField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, IgnoreBothField>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, IgnoreMixed>();
    EXPECT_EQ(4, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, OnlyIgnoreStatics>();
    EXPECT_EQ(0, firstIndex);
    firstIndex = Json::LastIndex<Statics::Only, OnlyIgnoreInstance>();
    EXPECT_EQ(1, firstIndex);
}

TEST(JsonSharedTest, Exception)
{
    const char test[] = "test";
    
    try {
        throw Json::Exception(test);
        EXPECT_TRUE(false);
    } catch ( Json::Exception & e ) {
        EXPECT_STREQ(test, e.what());
    }
}

TEST(JsonSharedTest, NullUnassignable)
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

TEST(JsonSharedTest, SimplifyTypeStr)
{
    EXPECT_STREQ("MyStruct", Json::simplifyTypeStr("struct My struct").c_str());
    EXPECT_STREQ("MyClass", Json::simplifyTypeStr("class My class").c_str());
}

struct MyStruct {};
class MyClass {};

TEST(JsonSharedTest, SuperTypeToJsonFieldName)
{
    EXPECT_STREQ("__MyStruct", Json::superTypeToJsonFieldName<MyStruct>().c_str());
    EXPECT_STREQ("__MyClass", Json::superTypeToJsonFieldName<MyClass>().c_str());
}

TEST(JsonSharedTest, FieldClusterToJsonFieldName)
{
    EXPECT_STREQ("____fieldCluster", Json::fieldClusterToJsonFieldName().c_str());
}

TEST(JsonGenericTest, JsonField)
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

TEST(JsonGenericTest, TypeMismatch)
{
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::None).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::Boolean).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::Number).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::String).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::Object).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::Array).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::BoolArray).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::NumberArray).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::StringArray).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::ObjectArray).empty());
    EXPECT_FALSE(Json::TypeMismatch::getTypeStr(Json::Value::Type::MixedArray).empty());

    Json::TypeMismatch functionTypeMismatch(Json::Value::Type::NullArray, Json::Value::Type::Number, "nullArray");
    EXPECT_EQ(Json::Value::Type::NullArray, functionTypeMismatch.valueType);
    EXPECT_EQ(Json::Value::Type::None, functionTypeMismatch.rValueType);
    EXPECT_EQ(Json::Value::Type::Number, functionTypeMismatch.functionType);

    Json::TypeMismatch assignTypeMismatch(Json::Value::Type::BoolArray, Json::Value::Type::Boolean);
    EXPECT_EQ(Json::Value::Type::BoolArray, assignTypeMismatch.valueType);
    EXPECT_EQ(Json::Value::Type::Boolean, assignTypeMismatch.rValueType);
    EXPECT_EQ(Json::Value::Type::None, assignTypeMismatch.functionType);
}

TEST(JsonGenericTest, Bool)
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
    EXPECT_THROW(ctor.number(), Json::TypeMismatch);
    EXPECT_THROW(ctor.string(), Json::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::TypeMismatch);

    const Json::Bool & constCtor = ctor;

    EXPECT_EQ(false, constCtor.boolean());
    EXPECT_THROW(constCtor.number(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.string(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.object(), Json::TypeMismatch);

    EXPECT_THROW(ctor.arraySize(), Json::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::TypeMismatch);

    EXPECT_THROW(constCtor.nullArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.boolArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.numberArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.stringArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.objectArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.mixedArray(), Json::TypeMismatch);
}

TEST(JsonGenericTest, Number)
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
    
    EXPECT_THROW(ctor.boolean(), Json::TypeMismatch);
    EXPECT_STREQ("0", ctor.number().c_str());
    EXPECT_THROW(ctor.string(), Json::TypeMismatch);
    EXPECT_THROW(ctor.object(), Json::TypeMismatch);

    const Json::Number & constCtor = ctor;
    
    EXPECT_THROW(constCtor.boolean(), Json::TypeMismatch);
    EXPECT_STREQ("0", constCtor.number().c_str());
    EXPECT_THROW(constCtor.string(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.object(), Json::TypeMismatch);

    EXPECT_THROW(ctor.arraySize(), Json::TypeMismatch);

    EXPECT_THROW(ctor.nullArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.boolArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.numberArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.stringArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.objectArray(), Json::TypeMismatch);
    EXPECT_THROW(ctor.mixedArray(), Json::TypeMismatch);

    EXPECT_THROW(constCtor.nullArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.boolArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.numberArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.stringArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.objectArray(), Json::TypeMismatch);
    EXPECT_THROW(constCtor.mixedArray(), Json::TypeMismatch);
}

TEST(JsonGenericTest, String)
{
    // TODO
}

TEST(JsonGenericTest, Object)
{
    // TODO
}

TEST(JsonGenericTest, NullArray)
{
    // TODO
}

TEST(JsonGenericTest, BoolArray)
{
    // TODO
}

TEST(JsonGenericTest, NumberArray)
{
    // TODO
}

TEST(JsonGenericTest, StringArray)
{
    // TODO
}

TEST(JsonGenericTest, ObjectArray)
{
    // TODO
}

TEST(JsonGenericTest, MixedArray)
{
    // TODO
}

TEST(JsonGenericTest, FieldCluster)
{
    // TODO
}

TEST(JsonGenericTest, GenericValueAssigner)
{
    // Note: As this relies on the specific Json::Generics for proper testing this test should come after
    Json::Value::Assigner nullAssigner(nullptr);
    EXPECT_EQ(nullptr, nullAssigner.get());

    Json::Value::Assigner falseBool(new Json::Bool(false));
    EXPECT_FALSE(falseBool.get()->boolean());
    Json::Value::Assigner trueBool = Json::Value::Assigner(new Json::Bool(true));
    EXPECT_TRUE(trueBool.get()->boolean());
    
    Json::Value* genericValue = nullptr;
    nullAssigner.into(genericValue);

    // TODO: Finish assigner tests
}
