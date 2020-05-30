#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <type_traits>
using namespace Reflect;
using namespace ExtendedTypeSupport;

struct TestSuper{};
struct TestOtherSuper{};
struct TestAnnotation { int value; };
struct TestOtherAnnotation { int otherValue; };
struct ThirdAnnotation {};

TEST(ReflectTest, SuperIndex)
{
    constexpr size_t superIndex = 5;
    constexpr size_t otherSuperIndex = 6;

    bool isEqual = std::is_same<TestSuper, SuperInfo<TestSuper, superIndex, NoAnnotation>::Type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<TestOtherSuper, SuperInfo<TestOtherSuper, otherSuperIndex, NoAnnotation>::Type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<NoAnnotation, SuperInfo<TestSuper, superIndex, NoAnnotation>::Annotations>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<NoAnnotation, SuperInfo<TestOtherSuper, otherSuperIndex, NoAnnotation>::Annotations>::value;
    EXPECT_TRUE(isEqual);
    isEqual = superIndex == SuperInfo<TestSuper, superIndex, NoAnnotation>::Index;
    EXPECT_TRUE(isEqual);
    isEqual = otherSuperIndex == SuperInfo<TestOtherSuper, otherSuperIndex, NoAnnotation>::Index;
    EXPECT_TRUE(isEqual);
    
    bool hasAnnotation = SuperInfo<TestSuper, superIndex, NoAnnotation>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<>>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<TestAnnotation>>::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<TestOtherAnnotation>>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<TestAnnotation, TestOtherAnnotation>>::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<TestAnnotation, TestOtherAnnotation>>::template HasAnnotation<TestOtherAnnotation>;
    EXPECT_TRUE(hasAnnotation);

    NoAnnotation noNote{};
    SuperInfo<TestSuper, superIndex, NoAnnotation> testSuperNoAnnotations{noNote};

    TestAnnotation testAnnotation { 33 };
    SuperInfo<TestSuper, superIndex, std::tuple<TestAnnotation>> annotated{testAnnotation};
    using Annotated = decltype(annotated);
    hasAnnotation = Annotated::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);

    EXPECT_EQ(33, annotated.getAnnotation<TestAnnotation>().value);

    int timesVisited = 0;
    annotated.forEach<TestAnnotation>([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    annotated.forEachAnnotation([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    TestOtherAnnotation testOtherAnnotation { 44 };
    TestOtherAnnotation testOtherAnnotation55 { 55 };
    SuperInfo<TestOtherSuper, otherSuperIndex, std::tuple<TestAnnotation, TestOtherAnnotation, TestOtherAnnotation>> otherAnnotated
        {std::tuple {testAnnotation, testOtherAnnotation, testOtherAnnotation55} };
    using OtherAnnotated = decltype(otherAnnotated);
    hasAnnotation = OtherAnnotated::template HasAnnotation<ThirdAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = OtherAnnotated::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = OtherAnnotated::template HasAnnotation<TestOtherAnnotation>;
    EXPECT_TRUE(hasAnnotation);

    auto otherAnnotatedTestAnnotation = otherAnnotated.getAnnotation<TestAnnotation>();
    EXPECT_EQ(33, otherAnnotatedTestAnnotation.value);
    auto otherAnnotatedTestOtherAnnotation = otherAnnotated.getAnnotation<TestOtherAnnotation>();
    EXPECT_EQ(44, otherAnnotatedTestOtherAnnotation.otherValue);

    timesVisited = 0;
    otherAnnotated.forEach<TestAnnotation>([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEach<TestOtherAnnotation>([&](auto & annotation) {
        if ( timesVisited == 0 )
            EXPECT_EQ(44, annotation.otherValue);
        else if ( timesVisited == 1 )
            EXPECT_EQ(55, annotation.otherValue);

        timesVisited++;
    });
    EXPECT_EQ(2, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEach<ThirdAnnotation>([&](auto & annotation) {
        timesVisited ++;
    });
    EXPECT_EQ(0, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEachAnnotation([&](auto & annotation) {
        using AnnotationType = std::remove_const<std::remove_reference<decltype(annotation)>::type>::type;
        constexpr bool isTestAnnotation = std::is_same_v<TestAnnotation, AnnotationType>;
        constexpr bool isTestOtherAnnotation = std::is_same_v<TestOtherAnnotation, AnnotationType>;
        EXPECT_TRUE(isTestAnnotation || isTestOtherAnnotation);
        if constexpr ( isTestAnnotation )
        {
            EXPECT_EQ(33, annotation.value);
        }
        else if constexpr ( isTestOtherAnnotation )
        {
            if ( timesVisited == 1 )
                EXPECT_EQ(44, annotation.otherValue);
            else if ( timesVisited == 2 )
                EXPECT_EQ(55, annotation.otherValue);
        }
        timesVisited ++;
    });
    EXPECT_EQ(3, timesVisited);
}

TEST(ReflectTest, RfMacroGetFieldName)
{
    class Obj {
    public:
        int first;
        float second;

        struct Class {
            enum_t(IndexOf, size_t, {
                FOR_EACH(GET_FIELD_NAME, first, second)
            });
        };
    };

    EXPECT_EQ(0, Obj::Class::IndexOf::first);
    EXPECT_EQ(1, Obj::Class::IndexOf::second);
}

class DescribeFieldTest {
public:
    int first;
    float second;

    struct Class {
        using ClassType = DescribeFieldTest;
        enum_t(IndexOf, size_t, { first, second });
        using first = int;
        using second = float;
        static constexpr NoAnnotation NoNote {};
        FOR_EACH(DESCRIBE_FIELD, first, second)
    };
};

TEST(ReflectTest, RfMacroDescribeField)
{
    bool isEqual = std::is_same<decltype(&DescribeFieldTest::first), DescribeFieldTest::Class::first_::Pointer>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<Fields::Field<int, DescribeFieldTest::Class::first_::Pointer, 0, NoAnnotation>,
        DescribeFieldTest::Class::first_::Field>::value;

    EXPECT_STREQ("first", DescribeFieldTest::Class::first_::nameStr.value);
    std::string firstTypeStr = DescribeFieldTest::Class::first_::typeStr.value;
    firstTypeStr.erase(std::remove(firstTypeStr.begin(), firstTypeStr.end(), ' '), firstTypeStr.end());
    EXPECT_TRUE(firstTypeStr.find("int") != std::string::npos);

    EXPECT_STREQ("first", DescribeFieldTest::Class::first_::field.name);
    std::string firstTypeFieldStr = DescribeFieldTest::Class::first_::field.typeStr;
    firstTypeFieldStr.erase(std::remove(firstTypeFieldStr.begin(), firstTypeFieldStr.end(), ' '), firstTypeFieldStr.end());
    EXPECT_TRUE(firstTypeFieldStr.find("int") != std::string::npos);
    EXPECT_EQ(0, DescribeFieldTest::Class::first_::field.arraySize);
    EXPECT_EQ(false, DescribeFieldTest::Class::first_::field.isIterable);
    EXPECT_EQ(false, DescribeFieldTest::Class::first_::field.isReflected);
    
    isEqual = std::is_same<int, DescribeFieldTest::Class::first_::Field::Type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<decltype(&DescribeFieldTest::first), DescribeFieldTest::Class::first_::Field::Pointer>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_EQ(&DescribeFieldTest::first, DescribeFieldTest::Class::first_::field.p);
    EXPECT_EQ(0, DescribeFieldTest::Class::first_::Field::Index);
    EXPECT_FALSE(DescribeFieldTest::Class::first_::Field::IsStatic);
    EXPECT_FALSE(DescribeFieldTest::Class::first_::Field::IsReflected);

    
    isEqual = std::is_same<decltype(&DescribeFieldTest::second), DescribeFieldTest::Class::second_::Pointer>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<Fields::Field<float, DescribeFieldTest::Class::second_::Pointer, 0, NoAnnotation>,
        DescribeFieldTest::Class::second_::Field>::value;

    EXPECT_STREQ("second", DescribeFieldTest::Class::second_::nameStr.value);
    std::string secondTypeStr = DescribeFieldTest::Class::second_::typeStr.value;
    secondTypeStr.erase(std::remove(secondTypeStr.begin(), secondTypeStr.end(), ' '), secondTypeStr.end());
    EXPECT_TRUE(secondTypeStr.find("float") != std::string::npos);

    EXPECT_STREQ("second", DescribeFieldTest::Class::second_::field.name);
    std::string secondTypeFieldStr = DescribeFieldTest::Class::second_::field.typeStr;
    secondTypeFieldStr.erase(std::remove(secondTypeFieldStr.begin(), secondTypeFieldStr.end(), ' '), secondTypeFieldStr.end());
    EXPECT_TRUE(secondTypeFieldStr.find("float") != std::string::npos);
    EXPECT_EQ(0, DescribeFieldTest::Class::second_::field.arraySize);
    EXPECT_EQ(false, DescribeFieldTest::Class::second_::field.isIterable);
    EXPECT_EQ(false, DescribeFieldTest::Class::second_::field.isReflected);
    
    isEqual = std::is_same<float, DescribeFieldTest::Class::second_::Field::Type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<decltype(&DescribeFieldTest::second), DescribeFieldTest::Class::second_::Field::Pointer>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_EQ(&DescribeFieldTest::second, DescribeFieldTest::Class::second_::field.p);
    EXPECT_EQ(1, DescribeFieldTest::Class::second_::Field::Index);
    EXPECT_FALSE(DescribeFieldTest::Class::second_::Field::IsStatic);
    EXPECT_FALSE(DescribeFieldTest::Class::second_::Field::IsReflected);
}

class GetFieldTest {
public:
    int first;
    float second;

    struct Class {
        static constexpr NoAnnotation NoNote {};
        struct first_ { static constexpr Fields::Field<decltype(first), decltype(&GetFieldTest::first), 0, NoAnnotation> field =
            { "first", "int", 0, false, false, false, &GetFieldTest::first, NoNote }; };
        struct second_ { static constexpr Fields::Field<decltype(second), decltype(&GetFieldTest::second), 1, NoAnnotation> field =
            { "second", "float", 0, false, false, false, &GetFieldTest::second, NoNote }; };
        static constexpr Fields::Field<> Fields[2] = {
            GET_FIELD(first)
            GET_FIELD(second)
        };
    };
};

TEST(ReflectTest, RfMacroGetField)
{
    EXPECT_STREQ("first", GetFieldTest::Class::Fields[0].name);
    EXPECT_STREQ("int", GetFieldTest::Class::Fields[0].typeStr);
    EXPECT_EQ(0, GetFieldTest::Class::Fields[0].arraySize);
    EXPECT_EQ(false, GetFieldTest::Class::Fields[0].isIterable);
    EXPECT_EQ(false, GetFieldTest::Class::Fields[0].isReflected);

    EXPECT_STREQ("second", GetFieldTest::Class::Fields[1].name);
    EXPECT_STREQ("float", GetFieldTest::Class::Fields[1].typeStr);
    EXPECT_EQ(0, GetFieldTest::Class::Fields[1].arraySize);
    EXPECT_EQ(false, GetFieldTest::Class::Fields[1].isIterable);
    EXPECT_EQ(false, GetFieldTest::Class::Fields[1].isReflected);
}

class UseFieldTest {
    public:
        int first;
        float second;

        struct Class {
            using ClassType = UseFieldTest;
            static constexpr NoAnnotation NoNote {};
            struct first_ {
                using Field = Fields::Field<int, decltype(&UseFieldTest::first), 0>;
                static constexpr Field field = { "first", "int", 0, false, false, false, &UseFieldTest::first, NoNote };
                CLANG_ONLY(first)
            };
            struct second_ {
                using Field = Fields::Field<float, decltype(&UseFieldTest::second), 1>;
                static constexpr Field field = { "second", "float", 0, false, false, false, &UseFieldTest::second, NoNote };
                CLANG_ONLY(second)
            };
            template <typename Function>
            static void ForEachField(UseFieldTest & object, Function function) {
                USE_FIELD_VALUE(first)
                USE_FIELD_VALUE(second)
            }
        };
};

TEST(ReflectTest, RfMacroUseField)
{
    UseFieldTest useFieldTest = { 1, 1.1f };
    size_t index = 0;
    UseFieldTest::Class::ForEachField(useFieldTest, [&](auto & field, auto & value) {

        using Field = typename std::remove_reference<decltype(field)>::type;

        EXPECT_EQ(index, Field::Index);
        switch ( index ) {
        case 0:
            EXPECT_STREQ("first", field.name);
            EXPECT_STREQ("int", field.typeStr);
            EXPECT_EQ(0, field.arraySize);
            EXPECT_EQ(false, field.isIterable);
            EXPECT_EQ(false, field.isReflected);
            EXPECT_EQ(useFieldTest.first, value);
            break;
        case 1:
            EXPECT_STREQ("second", field.name);
            EXPECT_STREQ("float", field.typeStr);
            EXPECT_EQ(0, field.arraySize);
            EXPECT_EQ(false, field.isIterable);
            EXPECT_EQ(false, field.isReflected);
            EXPECT_EQ(useFieldTest.second, value);
            break;
        default: EXPECT_TRUE(false); break;
        }
        index ++;
    });
    EXPECT_EQ(2, index);
}

class UseFieldAtTest {
    public:
        int first;
        float second;

        struct Class {
            enum_t(IndexOf, size_t, { first, second });
            static constexpr NoAnnotation NoNote {};
            struct first_ { static constexpr Fields::Field<int, decltype(&UseFieldAtTest::first), 0> field = { "first", "int", 0, false, false, false, &UseFieldAtTest::first, NoNote }; };
            struct second_ { static constexpr Fields::Field<float, decltype(&UseFieldAtTest::second), 1> field = { "second", "float", 0, false, false, false, &UseFieldAtTest::second, NoNote }; };
            template <typename Function>
            static void FieldAt(size_t fieldIndex, Function function) {
                switch ( fieldIndex ) {
                    USE_FIELD_AT(first)
                    USE_FIELD_AT(second)
                }
            }
        };
};

TEST(ReflectTest, RfMacroUseFieldAt)
{
    UseFieldAtTest useFieldTest = { 1, 1.1f };
    size_t index = 0;

    bool visited = false;
    UseFieldAtTest::Class::FieldAt(0, [&](auto & field) {
        EXPECT_STREQ("first", field.name);
        EXPECT_STREQ("int", field.typeStr);
        EXPECT_EQ(0, field.arraySize);
        EXPECT_EQ(false, field.isIterable);
        EXPECT_EQ(false, field.isReflected);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    UseFieldAtTest::Class::FieldAt(1, [&](auto & field) {
        EXPECT_STREQ("second", field.name);
        EXPECT_STREQ("float", field.typeStr);
        EXPECT_EQ(0, field.arraySize);
        EXPECT_EQ(false, field.isIterable);
        EXPECT_EQ(false, field.isReflected);
        visited = true;
    });
    EXPECT_TRUE(visited);
}

class UseFieldValueAtTest {
    public:
        int first;
        float second;

        struct Class {
            using ClassType = UseFieldValueAtTest;
            enum_t(IndexOf, size_t, { first, second });
            static constexpr NoAnnotation NoNote {};
            struct first_ {
                using Field = Fields::Field<int, decltype(&UseFieldValueAtTest::first), 0>;
                static constexpr Field field = { "first", "int", 0, false, false, false, &UseFieldValueAtTest::first, NoNote };
                CLANG_ONLY(first)
            };
            struct second_ {
                using Field = Fields::Field<float, decltype(&UseFieldValueAtTest::second), 1>;
                static constexpr Field field = { "second", "float", 0, false, false, false, &UseFieldValueAtTest::second, NoNote };
                CLANG_ONLY(second)
            };
            template <typename Function>
            static void FieldAt(UseFieldValueAtTest & object, size_t fieldIndex, Function function) {
                switch ( fieldIndex ) {
                    USE_FIELD_VALUE_AT(first)
                    USE_FIELD_VALUE_AT(second)
                }
            }
        };
};

TEST(ReflectTest, RfMacroUseFieldValueAt)
{
    UseFieldValueAtTest useFieldTest = { 1, 1.1f };
    size_t index = 0;

    bool visited = false;
    UseFieldValueAtTest::Class::FieldAt(useFieldTest, 0, [&](auto & field, auto & value) {
        EXPECT_STREQ("first", field.name);
        EXPECT_STREQ("int", field.typeStr);
        EXPECT_EQ(0, field.arraySize);
        EXPECT_EQ(false, field.isIterable);
        EXPECT_EQ(false, field.isReflected);
        EXPECT_EQ(useFieldTest.first, value);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    UseFieldValueAtTest::Class::FieldAt(useFieldTest, 1, [&](auto & field, auto & value) {
        EXPECT_STREQ("second", field.name);
        EXPECT_STREQ("float", field.typeStr);
        EXPECT_EQ(0, field.arraySize);
        EXPECT_EQ(false, field.isIterable);
        EXPECT_EQ(false, field.isReflected);
        EXPECT_EQ(useFieldTest.second, value);
        visited = true;
    });
    EXPECT_TRUE(visited);
}

struct AddIfStaticTest
{
    int testVal;
    static int testStaticVal;
    static int otherTestStaticVal;

    struct Class
    {
        struct testVal_ {
            using Field = Fields::Field<int, decltype(&AddIfStaticTest::testVal), 0>;
        };
        struct testStaticVal_ {
            using Field = Fields::Field<int, decltype(&AddIfStaticTest::testStaticVal), 1>;
        };
        struct otherTestStaticVal_ {
            using Field = Fields::Field<int, decltype(&AddIfStaticTest::otherTestStaticVal), 2>;
        };

        static constexpr size_t TotalStatics = 0
            ADD_IF_STATIC(testVal)
            ADD_IF_STATIC(testStaticVal)
            ADD_IF_STATIC(otherTestStaticVal);
    };
};

TEST(ReflectTest, RfMacroAddIfStatic)
{
    EXPECT_EQ(2, AddIfStaticTest::Class::TotalStatics);
}

class CumulativeMacroTest {
public:
    int first;
    float second;

    struct Class {
        using ClassType = DescribeFieldTest;
        enum_t(IndexOf, size_t, {
            FOR_EACH(GET_FIELD_NAME, first, second)
        });
        static constexpr NoAnnotation NoNote {};
        FOR_EACH(DESCRIBE_FIELD, first, second)
        static constexpr size_t TotalFields = COUNT_ARGUMENTS(first, second);
        static constexpr size_t TotalStaticFields = 0 FOR_EACH(ADD_IF_STATIC, first, second);
        static constexpr Fields::Field<> Fields[TotalFields] = {
            FOR_EACH(GET_FIELD, first, second)
        };
        template <typename Function> static void ForEachField(Function function) {
            FOR_EACH(USE_FIELD, first, second)
        }
        template <typename Function> static void ForEachField(CumulativeMacroTest & object, Function function) {
            FOR_EACH(USE_FIELD_VALUE, first, second)
        }
        template <typename Function> static void ForEachField(const CumulativeMacroTest & object, Function function) {
            FOR_EACH(USE_FIELD_VALUE, first, second)
        }
        template <typename Function> static void FieldAt(CumulativeMacroTest & object, size_t fieldIndex, Function function) {
            switch ( fieldIndex ) {
                FOR_EACH(USE_FIELD_VALUE_AT, first, second)
            }
        }
        template <typename Function> static void FieldAt(size_t fieldIndex, Function function) {
            switch ( fieldIndex ) {
                FOR_EACH(USE_FIELD_AT, first, second)
            }
        }
    };

};

TEST(ReflectTest, RfMacroCumulative)
{
    // This is just the entire REFLECT macro, it may be useful for testing changes, but there's no need to unit test it
    EXPECT_TRUE(true);
}

class ReflectSuperObj {
public:
    int superVal;

    REFLECT(ReflectSuperObj, superVal)
};

class ReflectSubObj {
public:
    int val;

    REFLECT(ReflectSubObj, val)
};

NOTE(ReflectObj, Super<ReflectSuperObj>)
class ReflectObj : public ReflectSuperObj {
public:
    ReflectObj() : primitive(0), object({}), primitiveReference(primitive) { primitiveArray[0] = 0; primitiveArray[1] = 1; }

    int primitive;
    ReflectSubObj object;
    int primitiveArray[2];
    std::map<int,float> map;
    std::vector<ReflectSubObj> objCollection;
    std::stack<int> stack;
    static int staticPrimitive;
    int & primitiveReference;
    static int & staticPrimitiveReference;

    REFLECT_NOTED(ReflectObj, primitive, object, primitiveArray, map, objCollection, stack, staticPrimitive, primitiveReference, staticPrimitiveReference)
};

int ReflectObj::staticPrimitive = 0;
int & ReflectObj::staticPrimitiveReference = ReflectObj::staticPrimitive;

TEST(ReflectTest, RfMacroReflect)
{
    EXPECT_EQ(9, ReflectObj::Class::TotalFields);
    EXPECT_EQ(2, ReflectObj::Class::TotalStaticFields);

    EXPECT_EQ(0, ReflectObj::Class::IndexOf::primitive);
    EXPECT_EQ(1, ReflectObj::Class::IndexOf::object);
    EXPECT_EQ(2, ReflectObj::Class::IndexOf::primitiveArray);
    EXPECT_EQ(3, ReflectObj::Class::IndexOf::map);
    EXPECT_EQ(4, ReflectObj::Class::IndexOf::objCollection);
    EXPECT_EQ(5, ReflectObj::Class::IndexOf::stack);
    EXPECT_EQ(6, ReflectObj::Class::IndexOf::staticPrimitive);
    EXPECT_EQ(7, ReflectObj::Class::IndexOf::primitiveReference);
    EXPECT_EQ(8, ReflectObj::Class::IndexOf::staticPrimitiveReference);
    
    EXPECT_STREQ("primitive", ReflectObj::Class::primitive_::nameStr.value);
    EXPECT_STREQ("object", ReflectObj::Class::object_::nameStr.value);
    EXPECT_STREQ("primitiveArray", ReflectObj::Class::primitiveArray_::nameStr.value);
    EXPECT_STREQ("map", ReflectObj::Class::map_::nameStr.value);
    EXPECT_STREQ("objCollection", ReflectObj::Class::objCollection_::nameStr.value);
    EXPECT_STREQ("stack", ReflectObj::Class::stack_::nameStr.value);
    EXPECT_STREQ("staticPrimitive", ReflectObj::Class::staticPrimitive_::nameStr.value);
    EXPECT_STREQ("primitiveReference", ReflectObj::Class::primitiveReference_::nameStr.value);
    EXPECT_STREQ("staticPrimitiveReference", ReflectObj::Class::staticPrimitiveReference_::nameStr.value);

    std::string typeStr = ReflectObj::Class::primitive_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int") != std::string::npos);
    typeStr = ReflectObj::Class::object_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("ReflectSubObj") != std::string::npos);
    typeStr = ReflectObj::Class::primitiveArray_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int") != std::string::npos);
    typeStr = ReflectObj::Class::map_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("map<int,float") != std::string::npos);
    typeStr = ReflectObj::Class::objCollection_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("vector<") != std::string::npos && typeStr.find("ReflectSubObj") != std::string::npos);
    typeStr = ReflectObj::Class::stack_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("stack<int") != std::string::npos);
    typeStr = ReflectObj::Class::staticPrimitive_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int") != std::string::npos);
    typeStr = ReflectObj::Class::primitiveReference_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int&") != std::string::npos);
    typeStr = ReflectObj::Class::staticPrimitiveReference_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int&") != std::string::npos);
    
    EXPECT_STREQ("primitive", ReflectObj::Class::primitive_::field.name);
    EXPECT_STREQ("object", ReflectObj::Class::object_::field.name);
    EXPECT_STREQ("primitiveArray", ReflectObj::Class::primitiveArray_::field.name);
    EXPECT_STREQ("map", ReflectObj::Class::map_::field.name);
    EXPECT_STREQ("objCollection", ReflectObj::Class::objCollection_::field.name);
    EXPECT_STREQ("stack", ReflectObj::Class::stack_::field.name);
    EXPECT_STREQ("staticPrimitive", ReflectObj::Class::staticPrimitive_::field.name);
    EXPECT_STREQ("primitiveReference", ReflectObj::Class::primitiveReference_::field.name);
    EXPECT_STREQ("staticPrimitiveReference", ReflectObj::Class::staticPrimitiveReference_::field.name);

    EXPECT_STREQ(ReflectObj::Class::primitive_::typeStr.value, ReflectObj::Class::primitive_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::object_::typeStr.value, ReflectObj::Class::object_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::typeStr.value, ReflectObj::Class::primitiveArray_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::map_::typeStr.value, ReflectObj::Class::map_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::typeStr.value, ReflectObj::Class::objCollection_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::stack_::typeStr.value, ReflectObj::Class::stack_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitive_::typeStr.value, ReflectObj::Class::staticPrimitive_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveReference_::typeStr.value, ReflectObj::Class::primitiveReference_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitiveReference_::typeStr.value, ReflectObj::Class::staticPrimitiveReference_::field.typeStr);
    
    EXPECT_EQ(0, ReflectObj::Class::primitive_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::object_::field.arraySize);
    EXPECT_EQ(2, ReflectObj::Class::primitiveArray_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::map_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::objCollection_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::stack_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::staticPrimitive_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::primitiveReference_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::staticPrimitiveReference_::field.arraySize);
    
    EXPECT_EQ(false, ReflectObj::Class::primitive_::field.isIterable);
    EXPECT_EQ(false, ReflectObj::Class::object_::field.isIterable);
    EXPECT_EQ(true, ReflectObj::Class::primitiveArray_::field.isIterable);
    EXPECT_EQ(true, ReflectObj::Class::map_::field.isIterable);
    EXPECT_EQ(true, ReflectObj::Class::objCollection_::field.isIterable);
    EXPECT_EQ(true, ReflectObj::Class::stack_::field.isIterable);
    EXPECT_EQ(false, ReflectObj::Class::staticPrimitive_::field.isIterable);
    EXPECT_EQ(false, ReflectObj::Class::primitiveReference_::field.isIterable);
    EXPECT_EQ(false, ReflectObj::Class::staticPrimitiveReference_::field.isIterable);
    
    EXPECT_EQ(false, ReflectObj::Class::primitive_::field.isReflected);
    EXPECT_EQ(true, ReflectObj::Class::object_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::primitiveArray_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::map_::field.isReflected);
    EXPECT_EQ(true, ReflectObj::Class::objCollection_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::stack_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::staticPrimitive_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::primitiveReference_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::staticPrimitiveReference_::field.isReflected);
    
    EXPECT_STREQ(ReflectObj::Class::primitive_::field.name, ReflectObj::Class::Fields[0].name);
    EXPECT_STREQ(ReflectObj::Class::object_::field.name, ReflectObj::Class::Fields[1].name);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::field.name, ReflectObj::Class::Fields[2].name);
    EXPECT_STREQ(ReflectObj::Class::map_::field.name, ReflectObj::Class::Fields[3].name);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::field.name, ReflectObj::Class::Fields[4].name);
    EXPECT_STREQ(ReflectObj::Class::stack_::field.name, ReflectObj::Class::Fields[5].name);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitive_::field.name, ReflectObj::Class::Fields[6].name);
    EXPECT_STREQ(ReflectObj::Class::primitiveReference_::field.name, ReflectObj::Class::Fields[7].name);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitiveReference_::field.name, ReflectObj::Class::Fields[8].name);
    
    EXPECT_STREQ(ReflectObj::Class::primitive_::field.typeStr, ReflectObj::Class::Fields[0].typeStr);
    EXPECT_STREQ(ReflectObj::Class::object_::field.typeStr, ReflectObj::Class::Fields[1].typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::field.typeStr, ReflectObj::Class::Fields[2].typeStr);
    EXPECT_STREQ(ReflectObj::Class::map_::field.typeStr, ReflectObj::Class::Fields[3].typeStr);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::field.typeStr, ReflectObj::Class::Fields[4].typeStr);
    EXPECT_STREQ(ReflectObj::Class::stack_::field.typeStr, ReflectObj::Class::Fields[5].typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitive_::field.typeStr, ReflectObj::Class::Fields[6].typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveReference_::field.typeStr, ReflectObj::Class::Fields[7].typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitiveReference_::field.typeStr, ReflectObj::Class::Fields[8].typeStr);
    
    EXPECT_EQ(ReflectObj::Class::primitive_::field.arraySize, ReflectObj::Class::Fields[0].arraySize);
    EXPECT_EQ(ReflectObj::Class::object_::field.arraySize, ReflectObj::Class::Fields[1].arraySize);
    EXPECT_EQ(ReflectObj::Class::primitiveArray_::field.arraySize, ReflectObj::Class::Fields[2].arraySize);
    EXPECT_EQ(ReflectObj::Class::map_::field.arraySize, ReflectObj::Class::Fields[3].arraySize);
    EXPECT_EQ(ReflectObj::Class::objCollection_::field.arraySize, ReflectObj::Class::Fields[4].arraySize);
    EXPECT_EQ(ReflectObj::Class::stack_::field.arraySize, ReflectObj::Class::Fields[5].arraySize);
    EXPECT_EQ(ReflectObj::Class::staticPrimitive_::field.arraySize, ReflectObj::Class::Fields[6].arraySize);
    EXPECT_EQ(ReflectObj::Class::primitiveReference_::field.arraySize, ReflectObj::Class::Fields[7].arraySize);
    EXPECT_EQ(ReflectObj::Class::staticPrimitiveReference_::field.arraySize, ReflectObj::Class::Fields[8].arraySize);
    
    EXPECT_EQ(ReflectObj::Class::primitive_::field.isIterable, ReflectObj::Class::Fields[0].isIterable);
    EXPECT_EQ(ReflectObj::Class::object_::field.isIterable, ReflectObj::Class::Fields[1].isIterable);
    EXPECT_EQ(ReflectObj::Class::primitiveArray_::field.isIterable, ReflectObj::Class::Fields[2].isIterable);
    EXPECT_EQ(ReflectObj::Class::map_::field.isIterable, ReflectObj::Class::Fields[3].isIterable);
    EXPECT_EQ(ReflectObj::Class::objCollection_::field.isIterable, ReflectObj::Class::Fields[4].isIterable);
    EXPECT_EQ(ReflectObj::Class::stack_::field.isIterable, ReflectObj::Class::Fields[5].isIterable);
    EXPECT_EQ(ReflectObj::Class::staticPrimitive_::field.isIterable, ReflectObj::Class::Fields[6].isIterable);
    EXPECT_EQ(ReflectObj::Class::primitiveReference_::field.isIterable, ReflectObj::Class::Fields[7].isIterable);
    EXPECT_EQ(ReflectObj::Class::staticPrimitiveReference_::field.isIterable, ReflectObj::Class::Fields[8].isIterable);
    
    EXPECT_EQ(ReflectObj::Class::primitive_::field.isReflected, ReflectObj::Class::Fields[0].isReflected);
    EXPECT_EQ(ReflectObj::Class::object_::field.isReflected, ReflectObj::Class::Fields[1].isReflected);
    EXPECT_EQ(ReflectObj::Class::primitiveArray_::field.isReflected, ReflectObj::Class::Fields[2].isReflected);
    EXPECT_EQ(ReflectObj::Class::map_::field.isReflected, ReflectObj::Class::Fields[3].isReflected);
    EXPECT_EQ(ReflectObj::Class::objCollection_::field.isReflected, ReflectObj::Class::Fields[4].isReflected);
    EXPECT_EQ(ReflectObj::Class::stack_::field.isReflected, ReflectObj::Class::Fields[5].isReflected);
    EXPECT_EQ(ReflectObj::Class::staticPrimitive_::field.isReflected, ReflectObj::Class::Fields[6].isReflected);
    EXPECT_EQ(ReflectObj::Class::primitiveReference_::field.isReflected, ReflectObj::Class::Fields[7].isReflected);
    EXPECT_EQ(ReflectObj::Class::staticPrimitiveReference_::field.isReflected, ReflectObj::Class::Fields[8].isReflected);
    
    ReflectSubObj reflectSubObj = { 20 };
    ReflectSubObj reflectSubObjZero = { 90 };
    ReflectSubObj reflectSubObjOne = { 99 };
    ReflectObj reflectObj = {};
    reflectObj.superVal = 101;
    reflectObj.primitive = 10;
    reflectObj.object = reflectSubObj;
    reflectObj.primitiveArray[0] = 30;
    reflectObj.primitiveArray[1] = 40;
    reflectObj.map.insert(std::pair<int, float>(50, 60.0f));
    reflectObj.map.insert(std::pair<int, float>(70, 80.0f));
    reflectObj.objCollection.push_back(reflectSubObjZero);
    reflectObj.objCollection.push_back(reflectSubObjOne);
    reflectObj.stack.push(2);
    reflectObj.stack.push(3);

    size_t index = 0;
    ReflectObj::Class::ForEachField(reflectObj, [&](auto & field, auto & value) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;
        using Value = typename std::remove_reference<decltype(value)>::type;

        EXPECT_EQ(index, Field::Index);
        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !Field::IsReflected && !is_iterable<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( Field::IsReflected && !is_iterable<Value>::value && !Field::IsStatic ) {
                    using ObjClass = typename Value::Class;
                    ObjClass::FieldAt(value, 0, [&](auto & field, auto & value) {
                        EXPECT_EQ(reflectObj.object.val, value);
                        visited = true;
                    });
                    EXPECT_TRUE(visited);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( !Field::IsReflected && is_static_array<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( !Field::IsReflected && is_iterable<Value>::value && is_pair<typename element_type<Value>::type>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( Field::IsReflected && is_iterable<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !Field::IsReflected && is_adaptor<Value>::value && !Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.pop();
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !Field::IsReflected && Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !Field::IsReflected && !is_iterable<Value>::value && !Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !Field::IsReflected && !is_iterable<Value>::value && Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            default:
                EXPECT_TRUE(false);
                break;
        }
        index ++;
    });
    EXPECT_EQ(9, index);

    index = 0;
    bool visited = true;
    ReflectObj::Class::ForEachField([&](auto & field) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;

        EXPECT_EQ(index, Field::Index);
        if constexpr ( Field::IsStatic && Field::Index == 6 )
        {
            EXPECT_EQ(ReflectObj::staticPrimitive, *field.p);
            visited = true;
        }
        index ++;
    });
    EXPECT_TRUE(visited);
    EXPECT_EQ(9, index);
}

struct ReflectReferences
{
    ReflectReferences() : primitive(11), primitiveReference(primitive) {}

    int primitive;
    int & primitiveReference;
    static int staticPrimitive;
    static int & staticPrimitiveReference;

    REFLECT(ReflectReferences, primitive, primitiveReference, staticPrimitive, staticPrimitiveReference)
};

int ReflectReferences::staticPrimitive = 33;
int & ReflectReferences::staticPrimitiveReference = ReflectReferences::staticPrimitive;

TEST(ReflectTest, RfMacroReflectReferences)
{
    ReflectReferences reflectReferences;
    EXPECT_EQ(11, reflectReferences.primitive);
    EXPECT_EQ(11, reflectReferences.primitiveReference);
    EXPECT_EQ(33, ReflectReferences::staticPrimitive);
    EXPECT_EQ(33, ReflectReferences::staticPrimitiveReference);

    bool visited = false;
    ReflectReferences::Class::FieldAt(reflectReferences, 1, [&](auto & field, auto & value) {
        EXPECT_EQ(11, value);
        bool isEqual = std::is_same<decltype(value), decltype(reflectReferences.primitiveReference)>::value;
        EXPECT_TRUE(isEqual);
        value = 22;
        EXPECT_EQ(22, reflectReferences.primitive);
        EXPECT_EQ(22, reflectReferences.primitiveReference);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    ReflectReferences::Class::FieldAt(reflectReferences, 3, [&](auto & field, auto & value) {
        EXPECT_EQ(33, value);
        bool isEqual = std::is_same<decltype(value), decltype(ReflectReferences::staticPrimitiveReference)>::value;
        EXPECT_TRUE(isEqual);
        value = 44;
        EXPECT_EQ(44, reflectReferences.staticPrimitive);
        EXPECT_EQ(44, reflectReferences.staticPrimitiveReference);
        visited = true;
    });
    EXPECT_TRUE(visited);
}
