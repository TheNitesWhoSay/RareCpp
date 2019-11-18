#include <gtest/gtest.h>
#include "../CppRandomAccessReflectionLib/Reflect.h"
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

TEST(ReflectTest, TypeToStr)
{
    EXPECT_STREQ("int", TypeToStr<int>().c_str());

    // Expecting some junk like how it's a struct or class, but it should still contain the pair text
    std::string typeStr = TypeToStr<std::pair<int,int>>();
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("pair<int,int") != std::string::npos);

    // Expecting some junk like how it's a struct or class and what allocator it's using, but should contain the map text
    std::string mapStr = TypeToStr<std::map<int,int>>();
    mapStr.erase(std::remove(mapStr.begin(), mapStr.end(), ' '), mapStr.end());
    EXPECT_TRUE(mapStr.find("map<int,int") != std::string::npos);
}

TEST(ReflectTest, BasicType)
{
    EXPECT_FALSE(Reflect::B::Reflected);
}

TEST(ReflectTest, ReflectedType)
{
    EXPECT_TRUE(Reflect::R::Reflected);
}

TEST(ReflectTest, InheritedType)
{
    using TwoArg = I<I<int, float>>;
    using ThreeArg = I<I<int, float, char>>;
    using FourArg = I<I<int, float, short, char>>;

    EXPECT_EQ(0, I<>::TotalSupers);
    EXPECT_EQ(0, I<I<>>::TotalSupers);
    EXPECT_EQ(1, I<int>::TotalSupers);
    EXPECT_EQ(1, I<I<int>>::TotalSupers);
    EXPECT_EQ(2, TwoArg::TotalSupers);
    EXPECT_EQ(3, ThreeArg::TotalSupers);
    EXPECT_EQ(4, FourArg::TotalSupers);

    int val = 0;
    bool visited = false;
    I<>::ForEach(val, [&](auto index, auto superClass) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    I<>::At(val, 0, [&](auto superClass) { visited = true; });
    EXPECT_FALSE(visited);

    visited = false;
    I<I<>>::ForEach(val, [&](auto index, auto superClass) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    I<I<>>::At(val, 0, [&](auto superClass) { visited = true; });
    EXPECT_FALSE(visited);

    size_t visitCount = 0;
    I<int>::ForEach(val, [&](auto index, auto superClass) {
        EXPECT_EQ(0, index);
        bool isSame = std::is_same<int, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visitCount ++;
    });
    EXPECT_EQ(1, visitCount);
    visited = false;
    I<int>::At(val, 0, [&](auto superClass) {
        bool isSame = std::is_same<int, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    I<int>::At(val, 1, [&](auto superClass) { visited = true; });
    EXPECT_FALSE(visited);

    visitCount = 0;
    I<I<int>>::ForEach(val, [&](auto index, auto superClass) {
        EXPECT_EQ(0, index);
        bool isSame = std::is_same<int, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visitCount ++;
    });
    EXPECT_EQ(1, visitCount);
    visited = false;
    I<I<int>>::At(val, 0, [&](auto superClass) {
        bool isSame = std::is_same<int, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    I<I<int>>::At(val, 1, [&](auto superClass) { visited = true; });
    EXPECT_FALSE(visited);

    visitCount = 0;
    TwoArg::ForEach(val, [&](auto index, auto superClass) {
        EXPECT_EQ(visitCount, index);
        bool isSame = false;
        switch ( index ) {
            case 0: isSame = std::is_same<int, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(2, visitCount);
    visited = false;
    TwoArg::At(val, 0, [&](auto superClass) {
        bool isSame = std::is_same<int, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    TwoArg::At(val, 1, [&](auto superClass) {
        bool isSame = std::is_same<float, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    TwoArg::At(val, 2, [&](auto superClass) { visited = true; });
    EXPECT_FALSE(visited);

    visitCount = 0;
    ThreeArg::ForEach(val, [&](auto index, auto superClass) {
        EXPECT_EQ(visitCount, index);
        bool isSame = false;
        switch ( index ) {
            case 0: isSame = std::is_same<int, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            case 2: isSame = std::is_same<char, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(3, visitCount);
    visited = false;
    ThreeArg::At(val, 0, [&](auto superClass) {
        bool isSame = std::is_same<int, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(val, 1, [&](auto superClass) {
        bool isSame = std::is_same<float, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(val, 2, [&](auto superClass) {
        bool isSame = std::is_same<char, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(val, 3, [&](auto superClass) { visited = true; });
    EXPECT_FALSE(visited);

    visitCount = 0;
    FourArg::ForEach(val, [&](auto index, auto superClass) {
        EXPECT_EQ(visitCount, index);
        bool isSame = false;
        switch ( index ) {
            case 0: isSame = std::is_same<int, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            case 2: isSame = std::is_same<short, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            case 3: isSame = std::is_same<char, decltype(superClass)>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(4, visitCount);
    visited = false;
    FourArg::At(val, 0, [&](auto superClass) {
        bool isSame = std::is_same<int, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(val, 1, [&](auto superClass) {
        bool isSame = std::is_same<float, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(val, 2, [&](auto superClass) {
        bool isSame = std::is_same<short, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(val, 3, [&](auto superClass) {
        bool isSame = std::is_same<char, decltype(superClass)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(val, 4, [&](auto superClass) { visited = true; });
    EXPECT_FALSE(visited);
}

TEST(ReflectTest, RfMacroAliasType)
{
    class Obj {
    public:
        int myField;

        class Class {
        public:
            ALIAS_TYPE((B) myField);
        };
    };

    bool isSame = std::is_same<int, Obj::Class::myField>::value;
    EXPECT_TRUE(isSame);
}

TEST(ReflectTest, RfMacroGetFieldName)
{
    class Obj {
    public:
        int first;
        float second;

        class Class {
        public:
            enum_t(IndexOf, size_t, {
                FOR_EACH(GET_FIELD_NAME, (B) first, (B) second)
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

    class Class {
    public:
        enum_t(IndexOf, size_t, { first, second });
        using first = int;
        using second = float;
        FOR_EACH(DESCRIBE_FIELD, (B) first, (B) second)
    };
};

TEST(ReflectTest, RfMacroDescribeField)
{
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
    EXPECT_EQ(false, DescribeFieldTest::Class::first_::field.isString);


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
    EXPECT_EQ(false, DescribeFieldTest::Class::second_::field.isString);
}

class GetFieldTest {
public:
    int first;
    float second;

    class Class {
    public:
        struct first_ { static constexpr Field<int, false, false> field = { "first", "int", 0, false, false, false }; };
        struct second_ { static constexpr Field<float, false, false> field = { "second", "float", 0, false, false, false }; };
        static constexpr Field<> Fields[2] = {
            GET_FIELD((B) first)
            GET_FIELD((B) second)
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
    EXPECT_EQ(false, GetFieldTest::Class::Fields[0].isString);

    EXPECT_STREQ("second", GetFieldTest::Class::Fields[1].name);
    EXPECT_STREQ("float", GetFieldTest::Class::Fields[1].typeStr);
    EXPECT_EQ(0, GetFieldTest::Class::Fields[1].arraySize);
    EXPECT_EQ(false, GetFieldTest::Class::Fields[1].isIterable);
    EXPECT_EQ(false, GetFieldTest::Class::Fields[1].isReflected);
    EXPECT_EQ(false, GetFieldTest::Class::Fields[1].isString);
}

class UseFieldTest {
    public:
        int first;
        float second;

        class Class {
        public:
            struct first_ { static constexpr Field<int, false, false, 0> field = { "first", "int", 0, false, false, false }; };
            struct second_ { static constexpr Field<float, false, false, 1> field = { "second", "float", 0, false, false, false }; };
            template <typename Function>
            static void ForEachField(UseFieldTest & object, Function function) {
                USE_FIELD((B) first)
                USE_FIELD((B) second)
            }
        };
};

TEST(ReflectTest, RfMacroUseField)
{
    UseFieldTest useFieldTest = { 1, 1.1f };
    size_t index = 0;
    UseFieldTest::Class::ForEachField(useFieldTest, [&](auto & field, auto & value) {

        using Field = std::remove_reference<decltype(field)>::type;

        EXPECT_EQ(index, Field::Index);
        switch ( index ) {
        case 0:
            EXPECT_STREQ("first", field.name);
            EXPECT_STREQ("int", field.typeStr);
            EXPECT_EQ(0, field.arraySize);
            EXPECT_EQ(false, field.isIterable);
            EXPECT_EQ(false, field.isReflected);
            EXPECT_EQ(false, field.isString);
            EXPECT_EQ(useFieldTest.first, value);
            break;
        case 1:
            EXPECT_STREQ("second", field.name);
            EXPECT_STREQ("float", field.typeStr);
            EXPECT_EQ(0, field.arraySize);
            EXPECT_EQ(false, field.isIterable);
            EXPECT_EQ(false, field.isReflected);
            EXPECT_EQ(false, field.isString);
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

        class Class {
        public:
            enum_t(IndexOf, size_t, { first, second });
            struct first_ { static constexpr Field<int, false, false> field = { "first", "int", 0, false, false, false }; };
            struct second_ { static constexpr Field<float, false, false> field = { "second", "float", 0, false, false, false }; };
            template <typename Function>
            static void FieldAt(UseFieldAtTest & object, size_t fieldIndex, Function function) {
                switch ( fieldIndex ) {
                    USE_FIELD_AT((B) first)
                    USE_FIELD_AT((B) second)
                }
            }
        };
};

TEST(ReflectTest, RfMacroUseFieldAt)
{
    UseFieldAtTest useFieldTest = { 1, 1.1f };
    size_t index = 0;

    bool visited = false;
    UseFieldAtTest::Class::FieldAt(useFieldTest, 0, [&](auto & field, auto & value) {
        EXPECT_STREQ("first", field.name);
        EXPECT_STREQ("int", field.typeStr);
        EXPECT_EQ(0, field.arraySize);
        EXPECT_EQ(false, field.isIterable);
        EXPECT_EQ(false, field.isReflected);
        EXPECT_EQ(false, field.isString);
        EXPECT_EQ(useFieldTest.first, value);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    UseFieldAtTest::Class::FieldAt(useFieldTest, 1, [&](auto & field, auto & value) {
        EXPECT_STREQ("second", field.name);
        EXPECT_STREQ("float", field.typeStr);
        EXPECT_EQ(0, field.arraySize);
        EXPECT_EQ(false, field.isIterable);
        EXPECT_EQ(false, field.isReflected);
        EXPECT_EQ(false, field.isString);
        EXPECT_EQ(useFieldTest.second, value);
        visited = true;
    });
    EXPECT_TRUE(visited);
}

class CumulativeMacroTest {
public:
    int first;
    float second;

    class Class {
    public:
        static constexpr size_t totalFields = COUNT_ARGUMENTS((B) first, (B) second);
        enum_t(IndexOf, size_t, {
            FOR_EACH(GET_FIELD_NAME, (B) first, (B) second)
        });
        FOR_EACH(ALIAS_TYPE, (B) first, (B) second)
        FOR_EACH(DESCRIBE_FIELD, (B) first, (B) second)
        static constexpr Field<> Fields[totalFields] = {
            FOR_EACH(GET_FIELD, (B) first, (B) second)
        };
        template <typename Function> static void ForEachField(CumulativeMacroTest & object, Function function) {
            FOR_EACH(USE_FIELD, (B) first, (B) second)
        }
        template <typename Function> static void ForEachField(const CumulativeMacroTest & object, Function function) {
            FOR_EACH(USE_FIELD, (B) first, (B) second)
        }
        template <typename Function> static void FieldAt(CumulativeMacroTest & object, size_t fieldIndex, Function function) {
            switch ( fieldIndex ) {
                FOR_EACH(USE_FIELD_AT, (B) first, (B) second)
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

    REFLECT(() ReflectSuperObj, (B) superVal)
};

class ReflectSubObj {
public:
    int val;

    REFLECT(() ReflectSubObj, (B) val)
};

class ReflectObj : public ReflectSuperObj {
public:
    int primitive;
    ReflectSubObj object;
    int primitiveArray[2];
    std::map<int,float> map;
    std::vector<ReflectSubObj> objCollection;
    std::stack<int> stack;

    REFLECT((ReflectSuperObj) ReflectObj, (B) primitive, (R) object, (B) primitiveArray, (B) map, (R) objCollection, (B) stack)
};

TEST(ReflectTest, RfMacroReflect)
{
    EXPECT_EQ(6, ReflectObj::Class::TotalFields);

    EXPECT_EQ(0, ReflectObj::Class::IndexOf::primitive);
    EXPECT_EQ(1, ReflectObj::Class::IndexOf::object);
    EXPECT_EQ(2, ReflectObj::Class::IndexOf::primitiveArray);
    EXPECT_EQ(3, ReflectObj::Class::IndexOf::map);
    EXPECT_EQ(4, ReflectObj::Class::IndexOf::objCollection);
    EXPECT_EQ(5, ReflectObj::Class::IndexOf::stack);
    
    bool isSame = std::is_same<int, ReflectObj::Class::primitive>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<ReflectSubObj, ReflectObj::Class::object>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<int[2], ReflectObj::Class::primitiveArray>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<std::map<int,float>, ReflectObj::Class::map>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<std::vector<ReflectSubObj>, ReflectObj::Class::objCollection>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<std::stack<int>, ReflectObj::Class::stack>::value;
    EXPECT_TRUE(isSame);
    
    EXPECT_STREQ("primitive", ReflectObj::Class::primitive_::nameStr.value);
    EXPECT_STREQ("object", ReflectObj::Class::object_::nameStr.value);
    EXPECT_STREQ("primitiveArray", ReflectObj::Class::primitiveArray_::nameStr.value);
    EXPECT_STREQ("map", ReflectObj::Class::map_::nameStr.value);
    EXPECT_STREQ("objCollection", ReflectObj::Class::objCollection_::nameStr.value);
    EXPECT_STREQ("stack", ReflectObj::Class::stack_::nameStr.value);

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
    
    EXPECT_STREQ("primitive", ReflectObj::Class::primitive_::field.name);
    EXPECT_STREQ("object", ReflectObj::Class::object_::field.name);
    EXPECT_STREQ("primitiveArray", ReflectObj::Class::primitiveArray_::field.name);
    EXPECT_STREQ("map", ReflectObj::Class::map_::field.name);
    EXPECT_STREQ("objCollection", ReflectObj::Class::objCollection_::field.name);
    EXPECT_STREQ("stack", ReflectObj::Class::stack_::field.name);

    EXPECT_STREQ(ReflectObj::Class::primitive_::typeStr.value, ReflectObj::Class::primitive_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::object_::typeStr.value, ReflectObj::Class::object_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::typeStr.value, ReflectObj::Class::primitiveArray_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::map_::typeStr.value, ReflectObj::Class::map_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::typeStr.value, ReflectObj::Class::objCollection_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::stack_::typeStr.value, ReflectObj::Class::stack_::field.typeStr);
    
    EXPECT_EQ(0, ReflectObj::Class::primitive_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::object_::field.arraySize);
    EXPECT_EQ(2, ReflectObj::Class::primitiveArray_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::map_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::objCollection_::field.arraySize);
    EXPECT_EQ(0, ReflectObj::Class::stack_::field.arraySize);
    
    EXPECT_EQ(false, ReflectObj::Class::primitive_::field.isIterable);
    EXPECT_EQ(false, ReflectObj::Class::object_::field.isIterable);
    EXPECT_EQ(true, ReflectObj::Class::primitiveArray_::field.isIterable);
    EXPECT_EQ(true, ReflectObj::Class::map_::field.isIterable);
    EXPECT_EQ(true, ReflectObj::Class::objCollection_::field.isIterable);
    EXPECT_EQ(true, ReflectObj::Class::stack_::field.isIterable);
    
    EXPECT_EQ(false, ReflectObj::Class::primitive_::field.isReflected);
    EXPECT_EQ(true, ReflectObj::Class::object_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::primitiveArray_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::map_::field.isReflected);
    EXPECT_EQ(true, ReflectObj::Class::objCollection_::field.isReflected);
    EXPECT_EQ(false, ReflectObj::Class::stack_::field.isReflected);
    
    EXPECT_EQ(false, ReflectObj::Class::primitive_::field.isString);
    EXPECT_EQ(false, ReflectObj::Class::object_::field.isString);
    EXPECT_EQ(false, ReflectObj::Class::primitiveArray_::field.isString);
    EXPECT_EQ(false, ReflectObj::Class::map_::field.isString);
    EXPECT_EQ(false, ReflectObj::Class::objCollection_::field.isString);
    EXPECT_EQ(false, ReflectObj::Class::stack_::field.isString);
    
    EXPECT_STREQ(ReflectObj::Class::primitive_::field.name, ReflectObj::Class::Fields[0].name);
    EXPECT_STREQ(ReflectObj::Class::object_::field.name, ReflectObj::Class::Fields[1].name);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::field.name, ReflectObj::Class::Fields[2].name);
    EXPECT_STREQ(ReflectObj::Class::map_::field.name, ReflectObj::Class::Fields[3].name);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::field.name, ReflectObj::Class::Fields[4].name);
    EXPECT_STREQ(ReflectObj::Class::stack_::field.name, ReflectObj::Class::Fields[5].name);
    
    EXPECT_STREQ(ReflectObj::Class::primitive_::field.typeStr, ReflectObj::Class::Fields[0].typeStr);
    EXPECT_STREQ(ReflectObj::Class::object_::field.typeStr, ReflectObj::Class::Fields[1].typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::field.typeStr, ReflectObj::Class::Fields[2].typeStr);
    EXPECT_STREQ(ReflectObj::Class::map_::field.typeStr, ReflectObj::Class::Fields[3].typeStr);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::field.typeStr, ReflectObj::Class::Fields[4].typeStr);
    EXPECT_STREQ(ReflectObj::Class::stack_::field.typeStr, ReflectObj::Class::Fields[5].typeStr);
    
    EXPECT_EQ(ReflectObj::Class::primitive_::field.arraySize, ReflectObj::Class::Fields[0].arraySize);
    EXPECT_EQ(ReflectObj::Class::object_::field.arraySize, ReflectObj::Class::Fields[1].arraySize);
    EXPECT_EQ(ReflectObj::Class::primitiveArray_::field.arraySize, ReflectObj::Class::Fields[2].arraySize);
    EXPECT_EQ(ReflectObj::Class::map_::field.arraySize, ReflectObj::Class::Fields[3].arraySize);
    EXPECT_EQ(ReflectObj::Class::objCollection_::field.arraySize, ReflectObj::Class::Fields[4].arraySize);
    EXPECT_EQ(ReflectObj::Class::stack_::field.arraySize, ReflectObj::Class::Fields[5].arraySize);
    
    EXPECT_EQ(ReflectObj::Class::primitive_::field.isIterable, ReflectObj::Class::Fields[0].isIterable);
    EXPECT_EQ(ReflectObj::Class::object_::field.isIterable, ReflectObj::Class::Fields[1].isIterable);
    EXPECT_EQ(ReflectObj::Class::primitiveArray_::field.isIterable, ReflectObj::Class::Fields[2].isIterable);
    EXPECT_EQ(ReflectObj::Class::map_::field.isIterable, ReflectObj::Class::Fields[3].isIterable);
    EXPECT_EQ(ReflectObj::Class::objCollection_::field.isIterable, ReflectObj::Class::Fields[4].isIterable);
    EXPECT_EQ(ReflectObj::Class::stack_::field.isIterable, ReflectObj::Class::Fields[5].isIterable);
    
    EXPECT_EQ(ReflectObj::Class::primitive_::field.isReflected, ReflectObj::Class::Fields[0].isReflected);
    EXPECT_EQ(ReflectObj::Class::object_::field.isReflected, ReflectObj::Class::Fields[1].isReflected);
    EXPECT_EQ(ReflectObj::Class::primitiveArray_::field.isReflected, ReflectObj::Class::Fields[2].isReflected);
    EXPECT_EQ(ReflectObj::Class::map_::field.isReflected, ReflectObj::Class::Fields[3].isReflected);
    EXPECT_EQ(ReflectObj::Class::objCollection_::field.isReflected, ReflectObj::Class::Fields[4].isReflected);
    EXPECT_EQ(ReflectObj::Class::stack_::field.isReflected, ReflectObj::Class::Fields[5].isReflected);
    
    EXPECT_EQ(ReflectObj::Class::primitive_::field.isString, ReflectObj::Class::Fields[0].isString);
    EXPECT_EQ(ReflectObj::Class::object_::field.isString, ReflectObj::Class::Fields[1].isString);
    EXPECT_EQ(ReflectObj::Class::primitiveArray_::field.isString, ReflectObj::Class::Fields[2].isString);
    EXPECT_EQ(ReflectObj::Class::map_::field.isString, ReflectObj::Class::Fields[3].isString);
    EXPECT_EQ(ReflectObj::Class::objCollection_::field.isString, ReflectObj::Class::Fields[4].isString);
    EXPECT_EQ(ReflectObj::Class::stack_::field.isString, ReflectObj::Class::Fields[5].isString);
    
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
    ReflectObj::Class::ForEachField(reflectObj, [&](auto field, auto & value) {
        
        using Field = std::remove_reference<decltype(field)>::type;
        using Value = std::remove_reference<decltype(value)>::type;

        EXPECT_EQ(index, Field::Index);
        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !Field::IsReflected && !is_iterable<Value>::value ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( Field::IsReflected && !is_iterable<Value>::value ) {
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
                if constexpr ( !Field::IsReflected && is_static_array<Value>::value ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( !Field::IsReflected && is_iterable<Value>::value && is_pair<element_type<Value>::type>::value ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( Field::IsReflected && is_iterable<Value>::value ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !Field::IsReflected && is_adaptor<Value>::value )
                    visited = true;

                EXPECT_TRUE(visited);
                break;
            default:
                EXPECT_TRUE(false);
                break;
        }
        index ++;
    });
    EXPECT_EQ(6, index);
}
