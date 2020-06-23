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

NOTE(TriviallyNoted)
struct TriviallyNoted
{
    NOTE(value)
    int value;
};

NOTE(SinglyNoted, 'a')
struct SinglyNoted
{
    NOTE(value, 'b')
    int value;
};

struct SerializedName
{
    std::string_view value;
};

NOTE(StructNoted, SerializedName { "noted" })
struct StructNoted
{
    NOTE(multiple, '1', SerializedName{"renamedValue"})
    int multiple;
};

NOTE(ComplexNoted, std::tuple{33, SerializedName{"test"}}, 'c', 'd', 'e')
struct ComplexNoted
{
    NOTE(first, 'a', 'b', std::tuple{44, SerializedName{"1st"}, 'c'})
    int first;

    NOTE(second, 'x', 'y', 'z', std::pair{55, SerializedName{"2nd"}})
    int second;
};

TEST(ReflectTest, NoteMacro)
{
    bool isSame = std::is_same<std::remove_const<decltype(TriviallyNoted_note)>::type, std::tuple<>>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<std::remove_const<decltype(TriviallyNoted::value_note)>::type, std::tuple<>>::value;
    EXPECT_TRUE(isSame);

    isSame = std::is_same<std::remove_const<decltype(SinglyNoted_note)>::type, std::tuple<char>>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', std::get<0>(SinglyNoted_note));
    isSame = std::is_same<std::remove_const<decltype(SinglyNoted::value_note)>::type, std::tuple<char>>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('b', std::get<0>(SinglyNoted::value_note));
    
    isSame = std::is_same<std::remove_const<decltype(StructNoted_note)>::type, std::tuple<SerializedName>>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ("noted", std::get<0>(StructNoted_note).value);
    isSame = std::is_same<std::remove_const<decltype(StructNoted::multiple_note)>::type, std::tuple<char, SerializedName>>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('1', std::get<0>(StructNoted::multiple_note));
    EXPECT_STREQ("renamedValue", std::string(std::get<1>(StructNoted::multiple_note).value).c_str());

    isSame = std::is_same<std::remove_const<decltype(ComplexNoted_note)>::type, std::tuple<std::tuple<int, SerializedName>, char, char, char>>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(33, std::get<0>(std::get<0>(ComplexNoted_note)));
    EXPECT_STREQ("test", std::string(std::get<1>(std::get<0>(ComplexNoted_note)).value).c_str());
    EXPECT_EQ('c', std::get<1>(ComplexNoted_note));
    EXPECT_EQ('d', std::get<2>(ComplexNoted_note));
    EXPECT_EQ('e', std::get<3>(ComplexNoted_note));

    isSame = std::is_same<std::remove_const<decltype(ComplexNoted::first_note)>::type, std::tuple<char, char, std::tuple<int, SerializedName, char>>>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', std::get<0>(ComplexNoted::first_note));
    EXPECT_EQ('b', std::get<1>(ComplexNoted::first_note));
    EXPECT_EQ(44, std::get<0>(std::get<2>(ComplexNoted::first_note)));
    EXPECT_STREQ("1st", std::string(std::get<1>(std::get<2>(ComplexNoted::first_note)).value).c_str());
    EXPECT_EQ('c', std::get<2>(std::get<2>(ComplexNoted::first_note)));

    isSame = std::is_same<std::remove_const<decltype(ComplexNoted::second_note)>::type, std::tuple<char, char, char, std::pair<int, SerializedName>>>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('x', std::get<0>(ComplexNoted::second_note));
    EXPECT_EQ('y', std::get<1>(ComplexNoted::second_note));
    EXPECT_EQ('z', std::get<2>(ComplexNoted::second_note));
    EXPECT_EQ(55, std::get<3>(ComplexNoted::second_note).first);
    EXPECT_STREQ("2nd", std::string(std::get<3>(ComplexNoted::second_note).second.value).c_str());
}

TEST(ReflectTest, NoAnnotationType)
{
    bool isSame = std::is_same<NoAnnotation, std::tuple<>>::value;
    EXPECT_TRUE(isSame);
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
    
    isEqual = std::is_same<int, DescribeFieldTest::Class::first_::Field::Type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<decltype(&DescribeFieldTest::first), DescribeFieldTest::Class::first_::Field::Pointer>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_EQ(&DescribeFieldTest::first, DescribeFieldTest::Class::first_::field.p);
    EXPECT_EQ(0, DescribeFieldTest::Class::first_::Field::Index);
    EXPECT_FALSE(DescribeFieldTest::Class::first_::Field::IsStatic);

    
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
    
    isEqual = std::is_same<float, DescribeFieldTest::Class::second_::Field::Type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<decltype(&DescribeFieldTest::second), DescribeFieldTest::Class::second_::Field::Pointer>::value;
    EXPECT_TRUE(isEqual);

    EXPECT_EQ(&DescribeFieldTest::second, DescribeFieldTest::Class::second_::field.p);
    EXPECT_EQ(1, DescribeFieldTest::Class::second_::Field::Index);
    EXPECT_FALSE(DescribeFieldTest::Class::second_::Field::IsStatic);
}

class GetFieldTest {
public:
    int first;
    float second;

    struct Class {
        static constexpr NoAnnotation NoNote {};
        struct first_ { static constexpr Fields::Field<decltype(first), decltype(&GetFieldTest::first), 0, NoAnnotation> field =
            { "first", "int", 0, false, false, &GetFieldTest::first, NoNote }; };
        struct second_ { static constexpr Fields::Field<decltype(second), decltype(&GetFieldTest::second), 1, NoAnnotation> field =
            { "second", "float", 0, false, false, &GetFieldTest::second, NoNote }; };
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

    EXPECT_STREQ("second", GetFieldTest::Class::Fields[1].name);
    EXPECT_STREQ("float", GetFieldTest::Class::Fields[1].typeStr);
    EXPECT_EQ(0, GetFieldTest::Class::Fields[1].arraySize);
    EXPECT_EQ(false, GetFieldTest::Class::Fields[1].isIterable);
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
                static constexpr Field field = { "first", "int", 0, false, false, &UseFieldTest::first, NoNote };
                CLANG_ONLY(first)
            };
            struct second_ {
                using Field = Fields::Field<float, decltype(&UseFieldTest::second), 1>;
                static constexpr Field field = { "second", "float", 0, false, false, &UseFieldTest::second, NoNote };
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
            EXPECT_EQ(useFieldTest.first, value);
            break;
        case 1:
            EXPECT_STREQ("second", field.name);
            EXPECT_STREQ("float", field.typeStr);
            EXPECT_EQ(0, field.arraySize);
            EXPECT_EQ(false, field.isIterable);
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
            struct first_ { static constexpr Fields::Field<int, decltype(&UseFieldAtTest::first), 0> field = { "first", "int", 0, false, false, &UseFieldAtTest::first, NoNote }; };
            struct second_ { static constexpr Fields::Field<float, decltype(&UseFieldAtTest::second), 1> field = { "second", "float", 0, false, false, &UseFieldAtTest::second, NoNote }; };
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
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    UseFieldAtTest::Class::FieldAt(1, [&](auto & field) {
        EXPECT_STREQ("second", field.name);
        EXPECT_STREQ("float", field.typeStr);
        EXPECT_EQ(0, field.arraySize);
        EXPECT_EQ(false, field.isIterable);
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
                static constexpr Field field = { "first", "int", 0, false, false, &UseFieldValueAtTest::first, NoNote };
                CLANG_ONLY(first)
            };
            struct second_ {
                using Field = Fields::Field<float, decltype(&UseFieldValueAtTest::second), 1>;
                static constexpr Field field = { "second", "float", 0, false, false, &UseFieldValueAtTest::second, NoNote };
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

struct does_not_have_reflected_class {};
struct does_not_quite_have_reflected_class {
    struct Class {};
};
struct does_have_reflected_class {
    struct Class {
        static constexpr size_t TotalFields = 0;
    };
};

TEST(ExtendedTypeSupportTest, BaseTypeHasClass)
{
    bool hasClass = decltype(typeHasReflection<does_not_have_reflected_class>(0))::value;
    EXPECT_FALSE(hasClass);
    hasClass = decltype(typeHasReflection<does_not_quite_have_reflected_class>(0))::value;
    EXPECT_FALSE(hasClass);
    hasClass = decltype(typeHasReflection<does_have_reflected_class>(0))::value;
    EXPECT_TRUE(hasClass);
}

TEST(ExtendedTypeSupportTest, IsReflected)
{
    EXPECT_FALSE(is_reflected<does_not_have_reflected_class>::value);
    EXPECT_FALSE(is_reflected<does_not_quite_have_reflected_class>::value);
    EXPECT_TRUE(is_reflected<does_have_reflected_class>::value);
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
    EXPECT_TRUE(is_reflected<ReflectSuperObj>::value);
    EXPECT_TRUE(is_reflected<ReflectSubObj>::value);
    EXPECT_TRUE(is_reflected<ReflectObj>::value);

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
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic ) {
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
                if constexpr ( !is_reflected<Value>::value && is_static_array<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( !is_reflected<Value>::value && is_iterable<Value>::value && is_pair<typename element_type<Value>::type>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( is_reflected<typename element_type<Value>::type>::value && is_iterable<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !is_reflected<Value>::value && is_adaptor<Value>::value && !Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.pop();
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !is_reflected<Value>::value && Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && Field::IsStatic )
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
