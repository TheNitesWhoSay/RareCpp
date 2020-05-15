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

TEST(ReflectTest, SuperIndex)
{
    constexpr size_t superIndex = 5;
    EXPECT_EQ(superIndex, SuperIndex<superIndex>::Index);
}

TEST(ReflectTest, InheritedType)
{
    using TwoArg = Inherit<Inherit<int, float>>;
    using ThreeArg = Inherit<Inherit<int, float, char>>;
    using FourArg = Inherit<Inherit<int, float, short, char>>;

    EXPECT_EQ(0, Inherit<>::TotalSupers);
    EXPECT_EQ(0, Inherit<Inherit<>>::TotalSupers);
    EXPECT_EQ(1, Inherit<int>::TotalSupers);
    EXPECT_EQ(1, Inherit<Inherit<int>>::TotalSupers);
    EXPECT_EQ(2, TwoArg::TotalSupers);
    EXPECT_EQ(3, ThreeArg::TotalSupers);
    EXPECT_EQ(4, FourArg::TotalSupers);

    int val = 0;
    bool visited = false;
    Inherit<>::ForEach(val, [&](auto index, auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    Inherit<>::ForEach([&](auto index, auto superType) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    Inherit<>::At(val, 0, [&](auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    Inherit<>::At(0, [&](auto superType) { visited = true; });
    EXPECT_FALSE(visited);

    visited = false;
    Inherit<Inherit<>>::ForEach(val, [&](auto index, auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    Inherit<Inherit<>>::ForEach([&](auto index, auto superType) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    Inherit<Inherit<>>::At(val, 0, [&](auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    Inherit<Inherit<>>::At(0, [&](auto superType) { visited = true; });
    EXPECT_FALSE(visited);
    
    size_t visitCount = 0;
    Inherit<int>::ForEach(val, [&](auto index, auto superObj) {
        EXPECT_EQ(0, decltype(index)::Index);
        bool isSame = std::is_same<int, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visitCount ++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    Inherit<int>::ForEach([&](auto index, auto superType) {
        using Super = typename decltype(superType)::type;
        EXPECT_EQ(0, decltype(index)::Index);
        bool isSame = std::is_same<int, Super>::value;
        EXPECT_TRUE(isSame);
        visitCount ++;
    });
    EXPECT_EQ(1, visitCount);
    visited = false;
    Inherit<int>::At(val, 0, [&](auto superObj) {
        bool isSame = std::is_same<int, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    Inherit<int>::At(0, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<int, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    Inherit<int>::At(val, 1, [&](auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    Inherit<int>::At(1, [&](auto superType) { visited = true; });
    EXPECT_FALSE(visited);
    
    visitCount = 0;
    Inherit<Inherit<int>>::ForEach(val, [&](auto index, auto superObj) {
        EXPECT_EQ(0, decltype(index)::Index);
        bool isSame = std::is_same<int, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visitCount ++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    Inherit<Inherit<int>>::ForEach([&](auto index, auto superType) {
        using Super = typename decltype(superType)::type;
        EXPECT_EQ(0, decltype(index)::Index);
        bool isSame = std::is_same<int, Super>::value;
        EXPECT_TRUE(isSame);
        visitCount ++;
    });
    EXPECT_EQ(1, visitCount);
    visited = false;
    Inherit<Inherit<int>>::At(val, 0, [&](auto superObj) {
        bool isSame = std::is_same<int, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    Inherit<Inherit<int>>::At(0, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<int, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    Inherit<Inherit<int>>::At(val, 1, [&](auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    Inherit<Inherit<int>>::At(1, [&](auto superType) { visited = true; });
    EXPECT_FALSE(visited);
    
    visitCount = 0;
    TwoArg::ForEach(val, [&](auto index, auto superObj) {
        EXPECT_EQ(visitCount, decltype(index)::Index);
        bool isSame = false;
        switch ( decltype(index)::Index ) {
            case 0: isSame = std::is_same<int, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    TwoArg::ForEach([&](auto index, auto superType) {
        using Super = typename decltype(superType)::type;
        EXPECT_EQ(visitCount, decltype(index)::Index);
        bool isSame = false;
        switch ( decltype(index)::Index ) {
            case 0: isSame = std::is_same<int, Super>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, Super>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(2, visitCount);
    visited = false;
    TwoArg::At(val, 0, [&](auto superObj) {
        bool isSame = std::is_same<int, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    TwoArg::At(0, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<int, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    TwoArg::At(val, 1, [&](auto superObj) {
        bool isSame = std::is_same<float, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    TwoArg::At(1, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<float, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    TwoArg::At(val, 2, [&](auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    TwoArg::At(2, [&](auto superType) { visited = true; });
    EXPECT_FALSE(visited);
    
    visitCount = 0;
    ThreeArg::ForEach(val, [&](auto index, auto superObj) {
        EXPECT_EQ(visitCount, decltype(index)::Index);
        bool isSame = false;
        switch ( decltype(index)::Index ) {
            case 0: isSame = std::is_same<int, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            case 2: isSame = std::is_same<char, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    ThreeArg::ForEach([&](auto index, auto superType) {
        using Super = typename decltype(superType)::type;
        EXPECT_EQ(visitCount, decltype(index)::Index);
        bool isSame = false;
        switch ( decltype(index)::Index ) {
            case 0: isSame = std::is_same<int, Super>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, Super>::value; EXPECT_TRUE(isSame); break;
            case 2: isSame = std::is_same<char, Super>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(3, visitCount);
    visited = false;
    ThreeArg::At(val, 0, [&](auto superObj) {
        bool isSame = std::is_same<int, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(0, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<int, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(val, 1, [&](auto superObj) {
        bool isSame = std::is_same<float, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(1, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<float, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(val, 2, [&](auto superObj) {
        bool isSame = std::is_same<char, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(2, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<char, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ThreeArg::At(val, 3, [&](auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    ThreeArg::At(3, [&](auto superType) { visited = true; });
    EXPECT_FALSE(visited);
    
    visitCount = 0;
    FourArg::ForEach(val, [&](auto index, auto superObj) {
        EXPECT_EQ(visitCount, decltype(index)::Index);
        bool isSame = false;
        switch ( decltype(index)::Index ) {
            case 0: isSame = std::is_same<int, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            case 2: isSame = std::is_same<short, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            case 3: isSame = std::is_same<char, decltype(superObj)>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(4, visitCount);
    visitCount = 0;
    FourArg::ForEach([&](auto index, auto superType) {
        using Super = typename decltype(superType)::type;
        EXPECT_EQ(visitCount, decltype(index)::Index);
        bool isSame = false;
        switch ( decltype(index)::Index ) {
            case 0: isSame = std::is_same<int, Super>::value; EXPECT_TRUE(isSame); break;
            case 1: isSame = std::is_same<float, Super>::value; EXPECT_TRUE(isSame); break;
            case 2: isSame = std::is_same<short, Super>::value; EXPECT_TRUE(isSame); break;
            case 3: isSame = std::is_same<char, Super>::value; EXPECT_TRUE(isSame); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount ++;
    });
    EXPECT_EQ(4, visitCount);
    visited = false;
    FourArg::At(val, 0, [&](auto superObj) {
        bool isSame = std::is_same<int, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(0, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<int, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(val, 1, [&](auto superObj) {
        bool isSame = std::is_same<float, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(1, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<float, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(val, 2, [&](auto superObj) {
        bool isSame = std::is_same<short, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(2, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<short, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(val, 3, [&](auto superObj) {
        bool isSame = std::is_same<char, decltype(superObj)>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(3, [&](auto superType) {
        using Super = typename decltype(superType)::type;
        bool isSame = std::is_same<char, Super>::value;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    FourArg::At(val, 4, [&](auto superObj) { visited = true; });
    EXPECT_FALSE(visited);
    visited = false;
    FourArg::At(4, [&](auto superType) { visited = true; });
    EXPECT_FALSE(visited);
}

TEST(ReflectTest, Annotation)
{
    EXPECT_FALSE(Annotate<>::Has<bool>);
    EXPECT_FALSE(Annotate<bool>::Has<int>);
    EXPECT_TRUE(Annotate<bool>::Has<bool>);
    bool has = Annotate<bool, int>::Has<bool>;
    EXPECT_TRUE(has);
    has = Annotate<bool, int>::Has<int>;
    EXPECT_TRUE(has);
    has = Annotate<bool, int>::Has<float>;
    EXPECT_FALSE(has);
    
    EXPECT_FALSE(Annotate<Annotate<>>::Has<bool>);
    EXPECT_FALSE(Annotate<Annotate<bool>>::Has<int>);
    EXPECT_TRUE(Annotate<Annotate<bool>>::Has<bool>);
    has = Annotate<Annotate<bool, int>>::Has<bool>;
    EXPECT_TRUE(has);
    has = Annotate<Annotate<bool, int>>::Has<int>;
    EXPECT_TRUE(has);
    has = Annotate<Annotate<bool, int>>::Has<float>;
    EXPECT_FALSE(has);
}

TEST(ReflectTest, ReflectedAnnotation)
{
    bool hasReflectionAnnotation = Fields::Field<void, void*, 0>::HasAnnotation<Reflected>;
    EXPECT_FALSE(hasReflectionAnnotation);
    hasReflectionAnnotation = Fields::Field<void, void*, 0, Annotate<>>::HasAnnotation<Reflected>;
    EXPECT_FALSE(hasReflectionAnnotation);
    hasReflectionAnnotation = Fields::Field<void, void*, 0, Reflected>::HasAnnotation<Reflected>;
    EXPECT_TRUE(hasReflectionAnnotation);
}

TEST(ReflectTest, RfMacroAliasType)
{
    class Obj {
    public:
        int myField;

        struct Class {
            ALIAS_TYPE(() myField);
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

        struct Class {
            enum_t(IndexOf, size_t, {
                FOR_EACH(GET_FIELD_NAME, () first, () second)
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
        FOR_EACH(DESCRIBE_FIELD, () first, () second)
    };
};

TEST(ReflectTest, RfMacroDescribeField)
{
    bool isEqual = std::is_same<decltype(&DescribeFieldTest::first), DescribeFieldTest::Class::first_::Pointer>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<Fields::Field<int, DescribeFieldTest::Class::first_::Pointer, 0, Annotate<>>,
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
    EXPECT_FALSE(DescribeFieldTest::Class::first_::Field::HasAnnotation<Reflected>);

    
    isEqual = std::is_same<decltype(&DescribeFieldTest::second), DescribeFieldTest::Class::second_::Pointer>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<Fields::Field<float, DescribeFieldTest::Class::second_::Pointer, 0, Annotate<>>,
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
    EXPECT_FALSE(DescribeFieldTest::Class::second_::Field::HasAnnotation<Reflected>);
}

class GetFieldTest {
public:
    int first;
    float second;

    struct Class {
        struct first_ { static constexpr Fields::Field<int> field = { "first", "int", 0, false, false }; };
        struct second_ { static constexpr Fields::Field<float> field = { "second", "float", 0, false, false }; };
        static constexpr Fields::Field<> Fields[2] = {
            GET_FIELD(() first)
            GET_FIELD(() second)
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
            struct first_ { static constexpr Fields::Field<int, decltype(&UseFieldTest::first), 0> field = { "first", "int", 0, false, false, &UseFieldTest::first }; };
            struct second_ { static constexpr Fields::Field<float, decltype(&UseFieldTest::second), 1> field = { "second", "float", 0, false, false, &UseFieldTest::second }; };
            template <typename Function>
            static void ForEachField(UseFieldTest & object, Function function) {
                USE_FIELD_VALUE(() first)
                USE_FIELD_VALUE(() second)
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
            struct first_ { static constexpr Fields::Field<int, decltype(&UseFieldAtTest::first), 0> field = { "first", "int", 0, false, false, &UseFieldAtTest::first }; };
            struct second_ { static constexpr Fields::Field<float, decltype(&UseFieldAtTest::second), 1> field = { "second", "float", 0, false, false, &UseFieldAtTest::second }; };
            template <typename Function>
            static void FieldAt(size_t fieldIndex, Function function) {
                switch ( fieldIndex ) {
                    USE_FIELD_AT(() first)
                    USE_FIELD_AT(() second)
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
            enum_t(IndexOf, size_t, { first, second });
            struct first_ { static constexpr Fields::Field<int, decltype(&UseFieldValueAtTest::first), 0> field = { "first", "int", 0, false, false, &UseFieldValueAtTest::first }; };
            struct second_ { static constexpr Fields::Field<float, decltype(&UseFieldValueAtTest::second), 1> field = { "second", "float", 0, false, false, &UseFieldValueAtTest::second }; };
            template <typename Function>
            static void FieldAt(UseFieldValueAtTest & object, size_t fieldIndex, Function function) {
                switch ( fieldIndex ) {
                    USE_FIELD_VALUE_AT(() first)
                    USE_FIELD_VALUE_AT(() second)
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
            ADD_IF_STATIC(() testVal)
            ADD_IF_STATIC(() testStaticVal)
            ADD_IF_STATIC(() otherTestStaticVal);
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
            FOR_EACH(GET_FIELD_NAME, () first, () second)
        });
        FOR_EACH(ALIAS_TYPE, () first, () second)
        FOR_EACH(DESCRIBE_FIELD, () first, () second)
        static constexpr size_t TotalFields = COUNT_ARGUMENTS(() first, () second);
        static constexpr size_t TotalStaticFields = 0 FOR_EACH(ADD_IF_STATIC, () first, () second);
        static constexpr Fields::Field<> Fields[TotalFields] = {
            FOR_EACH(GET_FIELD, () first, () second)
        };
        template <typename Function> static void ForEachField(Function function) {
            FOR_EACH(USE_FIELD, () first, () second)
        }
        template <typename Function> static void ForEachField(CumulativeMacroTest & object, Function function) {
            FOR_EACH(USE_FIELD_VALUE, () first, () second)
        }
        template <typename Function> static void ForEachField(const CumulativeMacroTest & object, Function function) {
            FOR_EACH(USE_FIELD_VALUE, () first, () second)
        }
        template <typename Function> static void FieldAt(CumulativeMacroTest & object, size_t fieldIndex, Function function) {
            switch ( fieldIndex ) {
                FOR_EACH(USE_FIELD_VALUE_AT, () first, () second)
            }
        }
        template <typename Function> static void FieldAt(size_t fieldIndex, Function function) {
            switch ( fieldIndex ) {
                FOR_EACH(USE_FIELD_AT, () first, () second)
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

    REFLECT(() ReflectSuperObj, () superVal)
};

class ReflectSubObj {
public:
    int val;

    REFLECT(() ReflectSubObj, () val)
};

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

    REFLECT((ReflectSuperObj) ReflectObj, () primitive, (Reflected) object, () primitiveArray, () map, (Reflected) objCollection, () stack, () staticPrimitive, () primitiveReference, () staticPrimitiveReference)
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
    isSame = std::is_same<int, ReflectObj::Class::staticPrimitive>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<int&, ReflectObj::Class::primitiveReference>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<int&, ReflectObj::Class::staticPrimitiveReference>::value;
    EXPECT_TRUE(isSame);
    
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
                if constexpr ( !Field::template HasAnnotation<Reflect::Reflected> && !is_iterable<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( Field::template HasAnnotation<Reflect::Reflected> && !is_iterable<Value>::value && !Field::IsStatic ) {
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
                if constexpr ( !Field::template HasAnnotation<Reflect::Reflected> && is_static_array<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( !Field::template HasAnnotation<Reflect::Reflected> && is_iterable<Value>::value && is_pair<typename element_type<Value>::type>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( Field::template HasAnnotation<Reflect::Reflected> && is_iterable<Value>::value && !Field::IsStatic ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !Field::template HasAnnotation<Reflect::Reflected> && is_adaptor<Value>::value && !Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.pop();
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !Field::template HasAnnotation<Reflect::Reflected> && Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !Field::template HasAnnotation<Reflect::Reflected> && !is_iterable<Value>::value && !Field::IsStatic )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !Field::template HasAnnotation<Reflect::Reflected> && !is_iterable<Value>::value && Field::IsStatic )
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

    REFLECT(() ReflectReferences, () primitive, () primitiveReference, () staticPrimitive, () staticPrimitiveReference)
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
