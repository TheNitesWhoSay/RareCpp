#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
using namespace Reflect::Fields;
using Reflect::NoAnnotation;

constexpr const char FieldName[] = "fieldName";

TEST(ReflectionFieldTest, FieldSimple)
{
    char fieldTypeStr[] = "int";

    Field<> field = { FieldName, fieldTypeStr };
    bool isEqual = std::is_same<decltype(field), Field<void, void, std::nullptr_t, 0, NoAnnotation, nullptr>>::value;
    EXPECT_TRUE(isEqual);
    
    EXPECT_STREQ(FieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
}

struct TestStruct
{
    int testVal;
    static int testStaticVal;
};
struct testVal_
{
    template <bool HasOffset, typename T = void> struct Get { static constexpr size_t offset() { return offsetof(TestStruct, testVal); } };
    template <typename T> struct Get<false, T> { static constexpr size_t offset() { return size_t(0); } };
};
struct testStaticVal_
{
    template <bool HasOffset, typename T = void> struct Get { static constexpr size_t offset() { return size_t(0); } };
    template <typename T> struct Get<false, T> { static constexpr size_t offset() { return size_t(0); } };
};
int TestStruct::testStaticVal = 0;

TEST(ReflectionFieldTest, FieldTemplated)
{
    constexpr size_t fieldIndex = 2;
    char fieldTypeStr[] = "int";
    NoAnnotation noAnnotation{};

    Field<int, testVal_, decltype(&TestStruct::testVal), fieldIndex, NoAnnotation, FieldName> field =
    { FieldName, fieldTypeStr, &TestStruct::testVal, noAnnotation };
    using IntField = decltype(field);

    EXPECT_STREQ(FieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    
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
    EXPECT_TRUE(IntField::HasOffset);
    constexpr size_t intFieldOffset = offsetof(TestStruct, testVal);
    EXPECT_EQ(intFieldOffset, IntField::getOffset());

    Field<int, testStaticVal_, decltype(&TestStruct::testStaticVal), fieldIndex, NoAnnotation, FieldName> staticField =
    { FieldName, fieldTypeStr, &TestStruct::testStaticVal, noAnnotation };
    using StaticIntField = decltype(staticField);

    EXPECT_EQ(staticField.p, &TestStruct::testStaticVal);
    EXPECT_TRUE(StaticIntField::IsStatic);
    EXPECT_FALSE(StaticIntField::IsFunction);
    EXPECT_FALSE(StaticIntField::HasOffset);
    EXPECT_EQ(0, StaticIntField::getOffset());
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
    char fieldTypeStr[] = "int&";
    size_t fieldArraySize = 0;
    bool fieldIsFunction = false;
    NoAnnotation noAnnotation{};

    Field<decltype(ReferencesTestStruct::testVal), void, std::nullptr_t, fieldIndex, NoAnnotation, FieldName> field =
    { FieldName, fieldTypeStr, nullptr, noAnnotation };
    using IntField = decltype(field);

    EXPECT_STREQ(FieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    
    bool isEqual = std::is_same<int&, IntField::Type>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<std::nullptr_t, IntField::Pointer>::value;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same<NoAnnotation, IntField::Annotations>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(field.p == nullptr);

    EXPECT_EQ(fieldIndex, IntField::Index);
    EXPECT_FALSE(IntField::IsStatic);
    EXPECT_FALSE(IntField::IsFunction);
    EXPECT_FALSE(IntField::HasOffset);

    Field<decltype(ReferencesTestStruct::testStaticVal), void, decltype(&ReferencesTestStruct::testStaticVal), fieldIndex, NoAnnotation, FieldName> staticField =
    { FieldName, fieldTypeStr, nullptr, noAnnotation };
    using StaticIntField = decltype(staticField);

    isEqual = std::is_same<StaticIntField::Pointer, std::nullptr_t>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(staticField.p == nullptr);

    EXPECT_EQ(fieldIndex, StaticIntField::Index);
    EXPECT_TRUE(StaticIntField::IsStatic);
    EXPECT_FALSE(StaticIntField::HasOffset);
}

struct TestFunctions
{
    std::string reflectedFunction() { return "Hit Reflected Function"; }
    static std::string staticReflectedFunction() { return "Hit Static Reflected Function"; };
};

constexpr const char ReflectedFunctionFieldName[] = "reflectedFunction";
constexpr const char StaticReflectedFunctionFieldName[] = "staticReflectedFunction";

TEST(ReflectionFieldTest, MethodsAndFunctions)
{
    size_t fieldIndex = 0;
    char fieldTypeStr[] = "std::string TestFunctions::reflectedFunction()";
    NoAnnotation noAnnotation{};

    Field<decltype(&TestFunctions::reflectedFunction), void, decltype(&TestFunctions::reflectedFunction), 0, NoAnnotation, ReflectedFunctionFieldName> field =
        { ReflectedFunctionFieldName, fieldTypeStr, &TestFunctions::reflectedFunction, noAnnotation };
    using FunctionField = decltype(field);
    
    EXPECT_STREQ(ReflectedFunctionFieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    
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
    EXPECT_FALSE(FunctionField::HasOffset);


    size_t staticFieldIndex = 1;
    char staticFieldTypeStr[] = "std::string TestFunctions::staticReflectedFunction()";
    size_t staticFieldArraySize = 0;
    bool staticFieldIsFunction = true;

    Field<decltype(&TestFunctions::staticReflectedFunction), void, decltype(&TestFunctions::staticReflectedFunction), 1, NoAnnotation, StaticReflectedFunctionFieldName> staticField = {
        StaticReflectedFunctionFieldName, staticFieldTypeStr,
        &TestFunctions::staticReflectedFunction, noAnnotation
    };
    using StaticFunctionField = decltype(staticField);
    
    EXPECT_STREQ(StaticReflectedFunctionFieldName, staticField.name);
    EXPECT_STREQ(staticFieldTypeStr, staticField.typeStr);
    
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
    EXPECT_FALSE(StaticFunctionField::HasOffset);
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

constexpr const char IrrelevantFieldName[] = "irrelevantFieldName";

TEST(ReflectionFieldTest, Annotations)
{
    constexpr size_t fieldIndex = 2;
    char fieldTypeStr[] = "int";
    NoAnnotation noAnnotation{};

    Field<int, void, decltype(&AnnotationTest::zeroAnnotations), fieldIndex, NoAnnotation, IrrelevantFieldName> zeroNoteField =
    { IrrelevantFieldName, fieldTypeStr, &AnnotationTest::zeroAnnotations, noAnnotation };
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


    Field<int, void, decltype(&AnnotationTest::oneAnnotation), fieldIndex, decltype(AnnotationTest::oneAnnotation_note), IrrelevantFieldName> oneNoteField =
    { IrrelevantFieldName, fieldTypeStr, &AnnotationTest::oneAnnotation, AnnotationTest::oneAnnotation_note };
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


    Field<int, void, decltype(&AnnotationTest::twoAnnotations), fieldIndex, decltype(AnnotationTest::twoAnnotations_note), IrrelevantFieldName> twoNoteField =
    { IrrelevantFieldName, fieldTypeStr, &AnnotationTest::twoAnnotations, AnnotationTest::twoAnnotations_note };
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


    Field<int, void, decltype(&AnnotationTest::threeAnnotations), fieldIndex, decltype(AnnotationTest::threeAnnotations_note), IrrelevantFieldName> threeNoteField =
    { IrrelevantFieldName, fieldTypeStr, &AnnotationTest::threeAnnotations, AnnotationTest::threeAnnotations_note };
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


