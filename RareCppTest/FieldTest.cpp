#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
using namespace Reflection::Fields;
using Reflection::NoAnnotation;

constexpr const char FieldName[] = "fieldName";

TEST(ReflectionFieldTest, FieldSimple)
{
    char fieldTypeStr[] = "int";

    Field<> field = { FieldName, fieldTypeStr };
    bool isEqual = std::is_same<decltype(field), Field<void, std::false_type, void, std::nullptr_t, 0, NoAnnotation>>::value;
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

struct IntFieldBase {
    static constexpr const char name[] = "testVal";
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<int>();
    static constexpr const char* typeStr = typeStr_;
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = Reflection::NoNote;
    static constexpr auto p = &TestStruct::testVal;
};

struct StaticIntFieldBase {
    static constexpr const char name[] = "testStaticVal";
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<int>();
    static constexpr const char* typeStr = typeStr_;
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = Reflection::NoNote;
    static constexpr auto p = &TestStruct::testStaticVal;
};

TEST(ReflectionFieldTest, FieldTemplated)
{
    constexpr size_t fieldIndex = 2;
    char fieldTypeStr[] = "int";
    NoAnnotation noAnnotation{};

    Field<int, IntFieldBase, testVal_, decltype(&TestStruct::testVal), fieldIndex, NoAnnotation> field {};
    using IntField = decltype(field);

    EXPECT_STREQ(IntFieldBase::name, field.name);
    EXPECT_STREQ(IntFieldBase::typeStr, field.typeStr);
    
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

    Field<int, StaticIntFieldBase, testStaticVal_, decltype(&TestStruct::testStaticVal), fieldIndex, NoAnnotation> staticField {};
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

struct IntFieldRefBase {
    static constexpr const char name[] = "testVal";
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<int &>();
    static constexpr const char* typeStr = typeStr_;
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = Reflection::NoNote;
    static constexpr auto p = nullptr;
};

struct StaticIntFieldRefBase {
    static constexpr const char name[] = "testStaticVal";
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<int &>();
    static constexpr const char* typeStr = typeStr_;
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = Reflection::NoNote;
    static constexpr auto p = nullptr;
};

TEST(ReflectionFieldTest, ReferencesFieldTemplated)
{
    constexpr size_t fieldIndex = 2;
    char fieldTypeStr[] = "int&";
    size_t fieldArraySize = 0;
    bool fieldIsFunction = false;
    NoAnnotation noAnnotation{};

    Field<decltype(ReferencesTestStruct::testVal), IntFieldRefBase, void, std::nullptr_t, fieldIndex, NoAnnotation> field {};
    using IntField = decltype(field);

    EXPECT_STREQ(IntFieldRefBase::name, field.name);
    EXPECT_STREQ(IntFieldRefBase::typeStr, field.typeStr);
    
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

    Field<decltype(ReferencesTestStruct::testStaticVal), StaticIntFieldRefBase, void, decltype(&ReferencesTestStruct::testStaticVal), fieldIndex, NoAnnotation> staticField {};
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

struct MemberFunctionBase {
    static constexpr const char name[] = "reflectedFunction";
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<decltype(&TestFunctions::reflectedFunction)>();
    static constexpr const char* typeStr = typeStr_;
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = Reflection::NoNote;
    static constexpr auto p = &TestFunctions::reflectedFunction;
};

struct StaticFunctionBase {
    static constexpr const char name[] = "staticReflectedFunction";
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<decltype(TestFunctions::staticReflectedFunction)>();
    static constexpr const char* typeStr = typeStr_;
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = Reflection::NoNote;
    static constexpr auto p = &TestFunctions::staticReflectedFunction;
};

TEST(ReflectionFieldTest, MethodsAndFunctions)
{
    size_t fieldIndex = 0;
    char fieldTypeStr[] = "std::string TestFunctions::reflectedFunction()";
    NoAnnotation noAnnotation{};

    Field<decltype(&TestFunctions::reflectedFunction), MemberFunctionBase, void, decltype(&TestFunctions::reflectedFunction), 0, NoAnnotation> field {};
    using FunctionField = decltype(field);
    
    EXPECT_STREQ(MemberFunctionBase::name, field.name);
    EXPECT_STREQ(MemberFunctionBase::typeStr, field.typeStr);
    
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

    Field<decltype(&TestFunctions::staticReflectedFunction), StaticFunctionBase, void, decltype(&TestFunctions::staticReflectedFunction), 1, NoAnnotation> staticField {};
    using StaticFunctionField = decltype(staticField);
    
    EXPECT_STREQ(StaticFunctionBase::name, staticField.name);
    EXPECT_STREQ(StaticFunctionBase::typeStr, staticField.typeStr);
    
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

struct ZeroAnnotationsFieldBase {
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = Reflection::NoNote;
    static constexpr auto p = &AnnotationTest::zeroAnnotations;
};

struct OneAnnotationsFieldBase {
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = AnnotationTest::oneAnnotation_note;
    static constexpr auto p = &AnnotationTest::oneAnnotation;
};

struct TwoAnnotationsFieldBase {
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = AnnotationTest::twoAnnotations_note;
    static constexpr auto p = &AnnotationTest::twoAnnotations;
};

struct ThreeAnnotationsFieldBase {
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = AnnotationTest::threeAnnotations_note;
    static constexpr auto p = &AnnotationTest::threeAnnotations;
};

TEST(ReflectionFieldTest, Annotations)
{
    constexpr size_t fieldIndex = 2;
    char fieldTypeStr[] = "int";
    NoAnnotation noAnnotation{};

    Field<int, ZeroAnnotationsFieldBase, void, decltype(&AnnotationTest::zeroAnnotations), fieldIndex, NoAnnotation> zeroNoteField {};
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


    Field<int, OneAnnotationsFieldBase, void, decltype(&AnnotationTest::oneAnnotation), fieldIndex, decltype(AnnotationTest::oneAnnotation_note)> oneNoteField {};
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


    Field<int, TwoAnnotationsFieldBase, void, decltype(&AnnotationTest::twoAnnotations), fieldIndex, decltype(AnnotationTest::twoAnnotations_note)> twoNoteField {};
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


    Field<int, ThreeAnnotationsFieldBase, void, decltype(&AnnotationTest::threeAnnotations), fieldIndex, decltype(AnnotationTest::threeAnnotations_note)> threeNoteField {};
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

template <typename T>
struct Specializable {};

struct SpecializedAnnotations
{
    NOTE(field, ObjectMapper::MappedBy<AnnotationTest>)
    int field;
};

struct AnnotatedIntFieldBase {
    static constexpr const char name[] = "testVal";
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<int>();
    static constexpr const char* typeStr = typeStr_;
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
    static constexpr auto & annotations = SpecializedAnnotations::field_note;
    static constexpr auto p = &TestStruct::testVal;
};

TEST(ReflectionFieldTest, SpecializedAnnotations)
{
    Field<int, AnnotatedIntFieldBase, void, decltype(&SpecializedAnnotations::field), 0, decltype(SpecializedAnnotations::field_note)> fieldWithSpecializedNote {};

    bool hasSpecializedAnnotations = fieldWithSpecializedNote.HasSpecializedAnnotation<ObjectMapper::MappedByType>;
    EXPECT_TRUE(hasSpecializedAnnotations);

    hasSpecializedAnnotations = fieldWithSpecializedNote.HasSpecializedAnnotation<Specializable>;
    EXPECT_FALSE(hasSpecializedAnnotations);

    constexpr auto & mappedByAnnotationsTest = fieldWithSpecializedNote.getAnnotation<ObjectMapper::MappedByType>();
    using MappedByAnnotationsTest = std::remove_const_t<std::remove_reference_t<decltype(mappedByAnnotationsTest)>>;

    bool isSame = std::is_same_v<MappedByAnnotationsTest, ObjectMapper::MappedByType<AnnotationTest>>;
    EXPECT_TRUE(isSame);

    using SpecializationType = typename MappedByAnnotationsTest::DefaultMapping;
    isSame = std::is_same_v<SpecializationType, AnnotationTest>;
    EXPECT_TRUE(isSame);

    int visitCount = 0;
    decltype(fieldWithSpecializedNote)::forEach<ObjectMapper::MappedByType>([&](auto & annotation){
        isSame = std::is_same_v<ObjectMapper::IsMappedBy<AnnotationTest>, std::remove_const_t<std::remove_reference_t<decltype(annotation)>>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
}
