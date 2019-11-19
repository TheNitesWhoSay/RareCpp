#include <gtest/gtest.h>
#include "../CppRandomAccessReflectionLib/Reflect.h"
using namespace Reflect::Fields;

TEST(ReflectionSupportTest, FieldSimple)
{
    char fieldName[] = "fieldName";
    char fieldTypeStr[] = "int";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsReflected = false;

    Field<> field = { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsReflected };
    
    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
    EXPECT_EQ(fieldIsReflected, field.isReflected);
}

TEST(ReflectionSupportTest, FieldTemplated)
{
    constexpr size_t fieldIndex = 2;
    char fieldName[] = "fieldName";
    char fieldTypeStr[] = "int";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsReflected = false;

    Field<int, fieldIndex> field = { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsReflected };
    using IntField = decltype(field);

    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
    EXPECT_EQ(fieldIsReflected, field.isReflected);
    
    bool isEqual = std::is_same<int, IntField::Type>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(fieldIndex, IntField::Index);
}
