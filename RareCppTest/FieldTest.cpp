#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
using namespace Reflect::Fields;
using Reflect::NoAnnotation;

TEST(ReflectionFieldTest, FieldSimple)
{
    char fieldName[] = "fieldName";
    char fieldTypeStr[] = "int";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsFunction = false;

    Field<> field = { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsFunction };
    bool isEqual = std::is_same<decltype(field), Field<void, nullptr_t, 0, NoAnnotation>>::value;
    EXPECT_TRUE(isEqual);
    
    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
    EXPECT_EQ(fieldIsFunction, field.isFunction);
}

struct TestStruct
{
    int testVal;
    static int testStaticVal;
};
int TestStruct::testStaticVal = 0;

TEST(ReflectionFieldTest, FieldTemplated)
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
    EXPECT_EQ(fieldIsFunction, field.isFunction);
    
    bool isEqual = std::is_same<int, IntField::Type>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<decltype(&TestStruct::testVal), IntField::Pointer>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<NoAnnotation, IntField::Annotations>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_EQ(field.p, &TestStruct::testVal);

    EXPECT_EQ(fieldIndex, IntField::Index);
    EXPECT_FALSE(IntField::IsStatic);
    EXPECT_FALSE(IntField::IsFunction);

    Field<int, decltype(&TestStruct::testStaticVal), fieldIndex> staticField =
    { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsFunction, &TestStruct::testStaticVal, noAnnotation };
    using StaticIntField = decltype(staticField);

    EXPECT_EQ(staticField.p, &TestStruct::testStaticVal);
    EXPECT_TRUE(StaticIntField::IsStatic);
    EXPECT_FALSE(StaticIntField::IsFunction);
}

struct ReferencesTestStruct
{
    int & testVal;
    static int & testStaticVal;
};
int & ReferencesTestStruct::testStaticVal = TestStruct::testStaticVal;

TEST(ReflectionFieldTest, ReferencesFieldTemplated)
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
    EXPECT_EQ(fieldIsFunction, field.isFunction);
    
    bool isEqual = std::is_same<int&, IntField::Type>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<nullptr_t, IntField::Pointer>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<NoAnnotation, IntField::Annotations>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(field.p == nullptr);

    EXPECT_EQ(fieldIndex, IntField::Index);
    EXPECT_FALSE(IntField::IsStatic);
    EXPECT_FALSE(IntField::IsFunction);

    Field<decltype(ReferencesTestStruct::testStaticVal), decltype(&ReferencesTestStruct::testStaticVal), fieldIndex> staticField =
    { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsFunction, nullptr, noAnnotation };
    using StaticIntField = decltype(staticField);

    isEqual = std::is_same<StaticIntField::Pointer, nullptr_t>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(staticField.p == nullptr);

    EXPECT_EQ(fieldIndex, StaticIntField::Index);
    EXPECT_TRUE(StaticIntField::IsStatic);
}

struct TestFunctions
{
    std::string reflectedFunction() { return "Hit Reflected Function"; }
    static std::string staticReflectedFunction() { return "Hit Static Reflected Function"; };
};

TEST(ReflectionFieldTest, MethodsAndFunctions)
{
    size_t fieldIndex = 0;
    char fieldName[] = "reflectedFunction";
    char fieldTypeStr[] = "std::string TestFunctions::reflectedFunction()";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsFunction = true;
    NoAnnotation noAnnotation{};

    Field<decltype(&TestFunctions::reflectedFunction), decltype(&TestFunctions::reflectedFunction), 0, NoAnnotation> field =
        { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsFunction, &TestFunctions::reflectedFunction, noAnnotation };
    using FunctionField = decltype(field);
    
    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
    EXPECT_EQ(fieldIsFunction, field.isFunction);
    
    bool isEqual = std::is_same<decltype(&TestFunctions::reflectedFunction), FunctionField::Type>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<decltype(&TestFunctions::reflectedFunction), FunctionField::Pointer>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<NoAnnotation, FunctionField::Annotations>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(field.p == &TestFunctions::reflectedFunction);

    EXPECT_EQ(fieldIndex, FunctionField::Index);
    EXPECT_FALSE(FunctionField::IsStatic);
    EXPECT_TRUE(FunctionField::IsFunction);


    size_t staticFieldIndex = 1;
    char staticFieldName[] = "staticReflectedFunction";
    char staticFieldTypeStr[] = "std::string TestFunctions::staticReflectedFunction()";
    size_t staticFieldArraySize = 0;
    bool staticFieldIsIterable = false;
    bool staticFieldIsFunction = true;

    Field<decltype(&TestFunctions::staticReflectedFunction), decltype(&TestFunctions::staticReflectedFunction), 1, NoAnnotation> staticField = {
        staticFieldName, staticFieldTypeStr, staticFieldArraySize, staticFieldIsIterable, staticFieldIsFunction,
        &TestFunctions::staticReflectedFunction, noAnnotation
    };
    using StaticFunctionField = decltype(staticField);
    
    EXPECT_STREQ(staticFieldName, staticField.name);
    EXPECT_STREQ(staticFieldTypeStr, staticField.typeStr);
    EXPECT_EQ(staticFieldArraySize, staticField.arraySize);
    EXPECT_EQ(staticFieldIsIterable, staticField.isIterable);
    EXPECT_EQ(staticFieldIsFunction, staticField.isFunction);
    
    isEqual = std::is_same<decltype(&TestFunctions::staticReflectedFunction), StaticFunctionField::Type>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<decltype(&TestFunctions::staticReflectedFunction), StaticFunctionField::Pointer>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<NoAnnotation, StaticFunctionField::Annotations>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(staticField.p == &TestFunctions::staticReflectedFunction);

    EXPECT_EQ(staticFieldIndex, StaticFunctionField::Index);
    EXPECT_TRUE(StaticFunctionField::IsStatic);
    EXPECT_TRUE(StaticFunctionField::IsFunction);
}

struct AnnotationTest
{
    int zeroAnnotations;

    NOTE(oneAnnotation, 'a')
    int oneAnnotation;

    NOTE(twoAnnotations, 'b', 'c')
    int twoAnnotations;

    NOTE(threeAnnotations, 'd', 1337, 'e')
    int threeAnnotations;
};

TEST(ReflectionFieldTest, Annotations)
{
    constexpr size_t fieldIndex = 2;
    char fieldName[] = "irrelevantFieldName";
    char fieldTypeStr[] = "int";
    size_t fieldArraySize = 0;
    NoAnnotation noAnnotation{};

    Field<int, decltype(&AnnotationTest::zeroAnnotations), fieldIndex, NoAnnotation> zeroNoteField =
    { fieldName, fieldTypeStr, fieldArraySize, false, false, &AnnotationTest::zeroAnnotations, noAnnotation };
    using ZeroNoteField = decltype(zeroNoteField);

    bool hasAnnotation = ZeroNoteField::template HasAnnotation<int>;
    EXPECT_FALSE(hasAnnotation);

    size_t visitCount = 0;
    zeroNoteField.forEach<int>([&](auto annotation) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);

    zeroNoteField.forEachAnnotation([&](auto annotation) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);


    Field<int, decltype(&AnnotationTest::oneAnnotation), fieldIndex, decltype(AnnotationTest::oneAnnotation_note)> oneNoteField =
    { fieldName, fieldTypeStr, fieldArraySize, false, false, &AnnotationTest::oneAnnotation, AnnotationTest::oneAnnotation_note };
    using OneNoteField = decltype(oneNoteField);

    hasAnnotation = OneNoteField::template HasAnnotation<int>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = OneNoteField::template HasAnnotation<char>;
    EXPECT_TRUE(hasAnnotation);

    EXPECT_EQ('a', oneNoteField.getAnnotation<char>());

    visitCount = 0;
    oneNoteField.forEach<int>([&](auto annotation) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    oneNoteField.forEach<char>([&](auto annotation) {
        EXPECT_EQ('a', annotation);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    oneNoteField.forEachAnnotation([&](auto annotation) {
        EXPECT_EQ('a', annotation);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);


    Field<int, decltype(&AnnotationTest::twoAnnotations), fieldIndex, decltype(AnnotationTest::twoAnnotations_note)> twoNoteField =
    { fieldName, fieldTypeStr, fieldArraySize, false, false, &AnnotationTest::twoAnnotations, AnnotationTest::twoAnnotations_note };
    using TwoNoteField = decltype(twoNoteField);

    hasAnnotation = TwoNoteField::template HasAnnotation<int>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = TwoNoteField::template HasAnnotation<char>;
    EXPECT_TRUE(hasAnnotation);

    EXPECT_EQ('b', twoNoteField.getAnnotation<char>());

    visitCount = 0;
    twoNoteField.forEach<int>([&](auto annotation) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    twoNoteField.forEach<char>([&](auto annotation) {
        if ( visitCount == 0 )
            EXPECT_EQ('b', annotation);
        else if ( visitCount == 1 )
            EXPECT_EQ('c', annotation);
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);

    visitCount = 0;
    twoNoteField.forEachAnnotation([&](auto annotation) {
        if ( visitCount == 0 )
            EXPECT_EQ('b', annotation);
        else if ( visitCount == 1 )
            EXPECT_EQ('c', annotation);
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);


    Field<int, decltype(&AnnotationTest::threeAnnotations), fieldIndex, decltype(AnnotationTest::threeAnnotations_note)> threeNoteField =
    { fieldName, fieldTypeStr, fieldArraySize, false, false, &AnnotationTest::threeAnnotations, AnnotationTest::threeAnnotations_note };
    using ThreeNoteField = decltype(threeNoteField);

    hasAnnotation = ThreeNoteField::template HasAnnotation<int>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = ThreeNoteField::template HasAnnotation<char>;
    EXPECT_TRUE(hasAnnotation);
    
    EXPECT_EQ('d', threeNoteField.getAnnotation<char>());
    EXPECT_EQ(1337, threeNoteField.getAnnotation<int>());

    visitCount = 0;
    threeNoteField.forEach<int>([&](auto annotation) {
        EXPECT_EQ(1337, annotation);
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    threeNoteField.forEach<char>([&](auto annotation) {
        if ( visitCount == 0 )
            EXPECT_EQ('d', annotation);
        else if ( visitCount == 2 )
            EXPECT_EQ('e', annotation);
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);

    visitCount = 0;
    threeNoteField.forEachAnnotation([&](auto annotation) {
        if ( visitCount == 0 )
            EXPECT_EQ('d', annotation);
        else if ( visitCount == 1 )
            EXPECT_EQ(1337, annotation);
        else if ( visitCount == 3 )
            EXPECT_EQ('e', annotation);
        visitCount++;
    });
    EXPECT_EQ(3, visitCount);
}


