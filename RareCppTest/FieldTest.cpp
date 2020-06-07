#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
using namespace Reflect::Fields;
using Reflect::NoAnnotation;

TEST(ReflectionSupportTest, FieldSimple)
{
    char fieldName[] = "fieldName";
    char fieldTypeStr[] = "int";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsReflected = false;

    Field<> field = { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable };
    
    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
}

struct TestStruct
{
    int testVal;
    static int testStaticVal;
};
int TestStruct::testStaticVal = 0;

TEST(ReflectionSupportTest, FieldTemplated)
{
    constexpr size_t fieldIndex = 2;
    char fieldName[] = "fieldName";
    char fieldTypeStr[] = "int";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsFunction = false;
    NoAnnotation noAnnotation{};

    Field<int, decltype(&TestStruct::testVal), fieldIndex> field =
    { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsFunction, &TestStruct::testVal, noAnnotation };
    using IntField = decltype(field);

    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
    
    bool isEqual = std::is_same<int, IntField::Type>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<decltype(&TestStruct::testVal), IntField::Pointer>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_EQ(field.p, &TestStruct::testVal);

    EXPECT_EQ(fieldIndex, IntField::Index);
    EXPECT_FALSE(IntField::IsStatic);

    Field<int, decltype(&TestStruct::testStaticVal), fieldIndex> staticField =
    { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsFunction, &TestStruct::testStaticVal, noAnnotation };
    using StaticIntField = decltype(staticField);

    EXPECT_EQ(staticField.p, &TestStruct::testStaticVal);
    EXPECT_TRUE(StaticIntField::IsStatic);
}

struct ReferencesTestStruct
{
    int & testVal;
    static int & testStaticVal;
};
int & ReferencesTestStruct::testStaticVal = TestStruct::testStaticVal;

TEST(ReflectionSupportTest, ReferencesFieldTemplated)
{
    constexpr size_t fieldIndex = 2;
    char fieldName[] = "fieldName";
    char fieldTypeStr[] = "int&";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsFunction = false;
    NoAnnotation noAnnotation{};

    Field<decltype(ReferencesTestStruct::testVal), nullptr_t, fieldIndex> field =
    { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsFunction, nullptr, noAnnotation };
    using IntField = decltype(field);

    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
    
    bool isEqual = std::is_same<int&, IntField::Type>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<IntField::Pointer, nullptr_t>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(field.p == nullptr);

    EXPECT_EQ(fieldIndex, IntField::Index);
    EXPECT_FALSE(IntField::IsStatic);

    Field<decltype(ReferencesTestStruct::testStaticVal), decltype(&ReferencesTestStruct::testStaticVal), fieldIndex> staticField =
    { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsFunction, nullptr, noAnnotation };
    using StaticIntField = decltype(staticField);

    isEqual = std::is_same<StaticIntField::Pointer, nullptr_t>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(staticField.p == nullptr);

    EXPECT_EQ(fieldIndex, StaticIntField::Index);
    EXPECT_TRUE(StaticIntField::IsStatic);
}
