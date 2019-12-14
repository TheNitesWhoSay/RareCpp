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

TEST(JsonGenericTest, String)
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

TEST(JsonGenericTest, Object)
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

TEST(JsonGenericTest, NullArray)
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

TEST(JsonGenericTest, BoolArray)
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

TEST(JsonGenericTest, NumberArray)
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

TEST(JsonGenericTest, StringArray)
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

TEST(JsonGenericTest, ObjectArray)
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

TEST(JsonGenericTest, MixedArray)
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

TEST(JsonGenericTest, FieldCluster)
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

TEST(JsonGenericTest, GenericValueAssigner)
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
