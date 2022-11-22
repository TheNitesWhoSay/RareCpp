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
#include <regex>
#include <variant>
using namespace Reflection;
using namespace ExtendedTypeSupport;

struct EmptyObj { REFLECT(EmptyObj) };

struct UnownedObj1
{
    int a;
    int b;
};

class UnownedObj2
{
public:
    int a;
    int b;

protected:
    int c;

private:
    int d;
};

struct OwnedObj
{
    int a;
    int b;

    REFLECT(OwnedObj, a, b)
};

template <> struct Reflection::Proxy<UnownedObj1> : public UnownedObj1
{
    REFLECT(Reflection::Proxy<UnownedObj1>, a, b)
};

template <> struct Reflection::Proxy<UnownedObj2> : public UnownedObj2
{
    REFLECT(Reflection::Proxy<UnownedObj2>, a, b, c)
};

template <> struct Reflection::Proxy<OwnedObj> : public OwnedObj
{
    REFLECT(Reflection::Proxy<OwnedObj>, a, b)
};

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
    int memberMethod() { return 1; }
    static int staticMethod() { return 2; }

    REFLECT_NOTED(ReflectObj, primitive, object, primitiveArray, map, objCollection, stack, staticPrimitive, primitiveReference, staticPrimitiveReference, memberMethod, staticMethod)
};

int ReflectObj::staticPrimitive = 33;
int & ReflectObj::staticPrimitiveReference = ReflectObj::staticPrimitive;

TEST(ReflectTest, Typedefs)
{
    bool isSame = std::is_same_v<ReflectSuperObj, Reflect<ReflectSuperObj>::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectSubObj, Reflect<ReflectSubObj>::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj, Reflect<ReflectObj>::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<OwnedObj, Reflect<OwnedObj>::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<UnownedObj1, Reflect<UnownedObj1>::Type>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<ReflectSuperObj::Class, Reflect<ReflectSuperObj>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectSubObj::Class, Reflect<ReflectSubObj>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class, Reflect<ReflectObj>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Proxy<OwnedObj>::Class, Reflect<OwnedObj>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Proxy<UnownedObj1>::Class, Reflect<UnownedObj1>::Class>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<Reflection::Inherit<ReflectSuperObj, ReflectSuperObj::Class::Annotations>, Reflect<ReflectSuperObj>::Supers>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Inherit<ReflectSubObj, ReflectSubObj::Class::Annotations>, Reflect<ReflectSubObj>::Supers>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Inherit<ReflectObj, ReflectObj::Class::Annotations>, Reflect<ReflectObj>::Supers>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Inherit<OwnedObj, OwnedObj::Class::Annotations>, Reflect<OwnedObj>::Supers>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Inherit<UnownedObj1, Reflection::Proxy<UnownedObj1>::Class::Annotations>, Reflect<UnownedObj1>::Supers>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectTestFields, HeaderDefinitions)
{
    auto totalFields = Reflect<ReflectSuperObj>::Fields::Total;
    EXPECT_EQ(1, totalFields);
    totalFields = Reflect<ReflectSubObj>::Fields::Total;
    EXPECT_EQ(1, totalFields);
    totalFields = Reflect<ReflectObj>::Fields::Total;
    EXPECT_EQ(11, totalFields);

    bool isSame = std::is_same_v<ReflectSuperObj::Class::superVal_::Field, Reflect<ReflectSuperObj>::Fields::Field<0>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectSubObj::Class::val_::Field, Reflect<ReflectSubObj>::Fields::Field<0>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::primitive_::Field, Reflect<ReflectObj>::Fields::Field<0>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::object_::Field, Reflect<ReflectObj>::Fields::Field<1>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::primitiveArray_::Field, Reflect<ReflectObj>::Fields::Field<2>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::map_::Field, Reflect<ReflectObj>::Fields::Field<3>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::objCollection_::Field, Reflect<ReflectObj>::Fields::Field<4>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::stack_::Field, Reflect<ReflectObj>::Fields::Field<5>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::staticPrimitive_::Field, Reflect<ReflectObj>::Fields::Field<6>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::primitiveReference_::Field, Reflect<ReflectObj>::Fields::Field<7>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::staticPrimitiveReference_::Field, Reflect<ReflectObj>::Fields::Field<8>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::memberMethod_::Field, Reflect<ReflectObj>::Fields::Field<9>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj::Class::staticMethod_::Field, Reflect<ReflectObj>::Fields::Field<10>>;
    EXPECT_TRUE(isSame);
    
    EXPECT_EQ(0, Reflect<ReflectSuperObj>::Fields::template field<0>.Index);
    EXPECT_EQ(0, Reflect<ReflectSubObj>::Fields::template field<0>.Index);
    EXPECT_EQ(0, Reflect<ReflectObj>::Fields::template field<0>.Index);
    EXPECT_EQ(1, Reflect<ReflectObj>::Fields::template field<1>.Index);
    EXPECT_EQ(2, Reflect<ReflectObj>::Fields::template field<2>.Index);
    EXPECT_EQ(3, Reflect<ReflectObj>::Fields::template field<3>.Index);
    EXPECT_EQ(4, Reflect<ReflectObj>::Fields::template field<4>.Index);
    EXPECT_EQ(5, Reflect<ReflectObj>::Fields::template field<5>.Index);
    EXPECT_EQ(6, Reflect<ReflectObj>::Fields::template field<6>.Index);
    EXPECT_EQ(7, Reflect<ReflectObj>::Fields::template field<7>.Index);
    EXPECT_EQ(8, Reflect<ReflectObj>::Fields::template field<8>.Index);
    EXPECT_EQ(9, Reflect<ReflectObj>::Fields::template field<9>.Index);
    EXPECT_EQ(10, Reflect<ReflectObj>::Fields::template field<10>.Index);

    constexpr std::string_view primitiveFieldName = "primitive";
    constexpr std::string_view objectFieldName = "object";
    constexpr size_t primitiveIndex = Reflect<ReflectObj>::Fields::IndexOf(primitiveFieldName);
    constexpr size_t objectIndex = Reflect<ReflectObj>::Fields::IndexOf(objectFieldName);
    EXPECT_EQ(primitiveIndex, Reflect<ReflectObj>::Fields::Field<primitiveIndex>::Index);
    EXPECT_EQ(objectIndex, Reflect<ReflectObj>::Fields::Field<objectIndex>::Index);
}

class ReflectPackTest {
public:
    int memberValue = 1;
    static int staticValue; // = 2
    int & memberValueReference;
    static int & staticValueReference;
    int memberMethod() { return 5; }
    static int staticMethod() { return 6; }

    ReflectPackTest() : memberValueReference(memberValue) {}

    REFLECT(ReflectPackTest, memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod)
};

int ReflectPackTest::staticValue = 2;
int & ReflectPackTest::staticValueReference = ReflectPackTest::staticValue;

namespace TestFilters {
    template <typename Field, typename = enable_if_field_t<Field>> struct Integers : std::bool_constant<
        std::is_same_v<int, std::remove_const_t<std::remove_reference_t<typename Field::Type>>>
    > {};
    template <typename Field, typename = enable_if_field_t<Field>> struct ReflectSubObjs : std::bool_constant<
        std::is_same_v<ReflectSubObj, std::remove_const_t<std::remove_reference_t<typename Field::Type>>>
    > {};
};

TEST(ReflectTestFields, Pack)
{
    bool visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
        using MemberField = decltype(memberValue);
        using StaticField = decltype(staticValue);
        using MemberValueReference = decltype(memberValueReference);
        using StaticValueReference = decltype(staticValueReference);
        using MemberMethod = decltype(memberMethod);
        using StaticMethod = decltype(staticMethod);

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
        using MemberField = std::remove_const_t<std::remove_reference_t<decltype(memberValue)>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<decltype(staticValue)>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<decltype(memberValueReference)>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<decltype(staticValueReference)>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<decltype(memberMethod)>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<decltype(staticMethod)>>;

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
        using MemberField = std::remove_const_t<std::remove_reference_t<decltype(memberValue)>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<decltype(staticValue)>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<decltype(memberValueReference)>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<decltype(staticValueReference)>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<decltype(memberMethod)>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<decltype(staticMethod)>>;

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
        using MemberField = std::remove_const_t<std::remove_reference_t<decltype(memberValue)>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<decltype(staticValue)>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<decltype(memberValueReference)>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<decltype(staticValueReference)>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<decltype(memberMethod)>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<decltype(staticMethod)>>;

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
        using MemberField = std::remove_const_t<std::remove_reference_t<decltype(memberValue)>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<decltype(staticValue)>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<decltype(memberValueReference)>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<decltype(staticValueReference)>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<decltype(memberMethod)>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<decltype(staticMethod)>>;

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = std::tuple_element_t<0, Ts>;
        using StaticField = std::tuple_element_t<1, Ts>;
        using MemberValueReference = std::tuple_element_t<2, Ts>;
        using StaticValueReference = std::tuple_element_t<3, Ts>;
        using MemberMethod = std::tuple_element_t<4, Ts>;
        using StaticMethod = std::tuple_element_t<5, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<0, Ts>>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<1, Ts>>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<2, Ts>>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<3, Ts>>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<4, Ts>>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<5, Ts>>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](auto && ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<0, Ts>>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<1, Ts>>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<2, Ts>>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<3, Ts>>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<4, Ts>>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<5, Ts>>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](const auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = std::remove_const_t<std::tuple_element_t<0, Ts>>;
        using StaticField = std::remove_const_t<std::tuple_element_t<1, Ts>>;
        using MemberValueReference = std::remove_const_t<std::tuple_element_t<2, Ts>>;
        using StaticValueReference = std::remove_const_t<std::tuple_element_t<3, Ts>>;
        using MemberMethod = std::remove_const_t<std::tuple_element_t<4, Ts>>;
        using StaticMethod = std::remove_const_t<std::tuple_element_t<5, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Fields::Pack([&](const auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<0, Ts>>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<1, Ts>>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<2, Ts>>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<3, Ts>>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<4, Ts>>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<5, Ts>>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        bool isSame = std::is_same_v<ReflectPackTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<ReflectPackTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(ReflectPackTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(ReflectPackTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    // Note: fields are always lvalues and don't have a need to be passed as rvalue references
}

TEST(ReflectTestFields, FilteredCount)
{
    size_t filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::None>();
    EXPECT_EQ(11, filteredCount);
    filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::IsData>();
    EXPECT_EQ(9, filteredCount);
    filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::IsFunction>();
    EXPECT_EQ(2, filteredCount);
    filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::IsInstanceField>();
    EXPECT_EQ(8, filteredCount);
    filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::IsStaticField>();
    EXPECT_EQ(3, filteredCount);
    filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::IsInstanceData>();
    EXPECT_EQ(7, filteredCount);
    filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::IsInstanceFunction>();
    EXPECT_EQ(1, filteredCount);
    filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::IsStaticData>();
    EXPECT_EQ(2, filteredCount);
    filteredCount = Reflect<ReflectObj>::Fields::FilteredCount<Filter::IsStaticFunction>();
    EXPECT_EQ(1, filteredCount);
}

TEST(ReflectTestFields, IndexOf)
{
    constexpr size_t npos = std::numeric_limits<size_t>::max();
    EXPECT_EQ(npos, Reflect<EmptyObj>::Fields::IndexOf(""));
    EXPECT_EQ(npos, Reflect<EmptyObj>::Fields::IndexOf("asdf"));
    
    EXPECT_EQ(npos, Reflect<ReflectObj>::Fields::IndexOf(""));
    EXPECT_EQ(npos, Reflect<ReflectObj>::Fields::IndexOf("unknownField"));
    EXPECT_EQ(0, Reflect<ReflectObj>::Fields::IndexOf("primitive"));
    EXPECT_EQ(1, Reflect<ReflectObj>::Fields::IndexOf("object"));
    EXPECT_EQ(2, Reflect<ReflectObj>::Fields::IndexOf("primitiveArray"));
    EXPECT_EQ(3, Reflect<ReflectObj>::Fields::IndexOf("map"));
    EXPECT_EQ(4, Reflect<ReflectObj>::Fields::IndexOf("objCollection"));
    EXPECT_EQ(5, Reflect<ReflectObj>::Fields::IndexOf("stack"));
    EXPECT_EQ(6, Reflect<ReflectObj>::Fields::IndexOf("staticPrimitive"));
    EXPECT_EQ(7, Reflect<ReflectObj>::Fields::IndexOf("primitiveReference"));
    EXPECT_EQ(8, Reflect<ReflectObj>::Fields::IndexOf("staticPrimitiveReference"));
    EXPECT_EQ(9, Reflect<ReflectObj>::Fields::IndexOf("memberMethod"));
    EXPECT_EQ(10, Reflect<ReflectObj>::Fields::IndexOf("staticMethod"));
}

TEST(ReflectTestFields, ForEachField)
{
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
    int visitCount = 0;
    Reflect<ReflectObj>::Fields::ForEach([&](auto & field) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;

        EXPECT_EQ(index, Field::Index);
        if constexpr ( Field::IsStatic && Field::Index == 6 )
        {
            EXPECT_EQ(ReflectObj::staticPrimitive, *field.p);
            visitCount++;
        }
        else if constexpr ( Field::Index == 9 )
        {
            int result = (reflectObj.*field.p)();
            EXPECT_EQ(1, result);
            visitCount++;
        }
        else if constexpr ( Field::Index == 10 )
        {
            int result = field.p();
            EXPECT_EQ(2, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(3, visitCount);
    EXPECT_EQ(11, index);
}

TEST(ReflectTestFields, ForEachFieldFiltered)
{
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
    int visitCount = 0;
    Reflect<ReflectObj>::Fields::ForEach<Filter::IsInstanceField>([&](auto & field) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;

        if constexpr ( Field::Index == 9 )
        {
            int result = (reflectObj.*field.p)();
            EXPECT_EQ(1, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(1, visitCount);
    EXPECT_EQ(8, index);

    index = 0;
    visitCount = 0;
    Reflect<ReflectObj>::Fields::ForEach<Filter::IsStaticField>([&](auto & field) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;

        if constexpr ( Field::IsStatic && Field::Index == 6 )
        {
            EXPECT_EQ(ReflectObj::staticPrimitive, *field.p);
            visitCount++;
        }
        else if constexpr ( Field::Index == 10 )
        {
            int result = field.p();
            EXPECT_EQ(2, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(2, visitCount);
    EXPECT_EQ(3, index);
}

TEST(ReflectTestFields, ForEachStatic)
{
    size_t index = 0;
    Reflect<ReflectObj>::Fields::ForEach([&](auto & field, auto & value) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( Field::IsStatic && Field::IsFunction ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(3, index);
}

TEST(ReflectTestFields, ForEachStaticFiltered)
{
    size_t index = 0;
    Reflect<ReflectObj>::Fields::ForEach<Filter::IsStaticData>([&](auto & field, auto & value) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
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
    EXPECT_EQ(2, index);

    index = 0;
    Reflect<ReflectObj>::Fields::ForEach<Filter::IsStaticFunction>([&](auto & field, auto & value) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( Field::IsStatic && Field::IsFunction ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(1, index);
}

TEST(ReflectTestFields, ForEachInstanced)
{
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
    Reflect<ReflectObj>::Fields::ForEach(reflectObj, [&](auto & field, auto & value) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;
        using Value = typename std::remove_reference<decltype(value)>::type;

        EXPECT_EQ(index, Field::Index);
        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic && !Field::IsFunction ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic ) {
                    using ObjClass = typename Value::Class;
                    Reflect<Value>::Fields::At(0, value, [&](auto & field, auto & value) {
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
                    reflectObj.stack.push(3);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !is_reflected<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 9:
                if constexpr ( !Field::IsStatic && Field::IsFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 10:
                if constexpr ( Field::IsStatic && Field::IsFunction ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(11, index);
}

TEST(ReflectTestFields, ForEachInstancedFiltered)
{
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
    Reflect<ReflectObj>::Fields::ForEach<Filter::None>(reflectObj, [&](auto & field, auto & value) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;
        using Value = typename std::remove_reference<decltype(value)>::type;

        EXPECT_EQ(index, Field::Index);
        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic && !Field::IsFunction ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic ) {
                    using ObjClass = typename Value::Class;
                    Reflect<Value>::Fields::At(0, value, [&](auto & field, auto & value) {
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
                    reflectObj.stack.push(3);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !is_reflected<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 9:
                if constexpr ( !Field::IsStatic && Field::IsFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 10:
                if constexpr ( Field::IsStatic && Field::IsFunction ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(11, index);

    index = 0;
    Reflect<ReflectObj>::Fields::ForEach<Filter::IsData>(reflectObj, [&](auto & field, auto & value) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;
        using Value = typename std::remove_reference<decltype(value)>::type;

        EXPECT_EQ(index, Field::Index);
        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic && !Field::IsFunction ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic ) {
                    using ObjClass = typename Value::Class;
                    Reflect<Value>::Fields::At(0, value, [&](auto & field, auto & value) {
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
                    reflectObj.stack.push(3);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !is_reflected<Value>::value && Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && !Field::IsStatic && !Field::IsFunction )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && Field::IsStatic && !Field::IsFunction )
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
    Reflect<ReflectObj>::Fields::ForEach<Filter::IsFunction>(reflectObj, [&](auto & field, auto & value) {
        
        using Field = typename std::remove_reference<decltype(field)>::type;
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                EXPECT_EQ(9, Field::Index);
                if constexpr ( !Field::IsStatic && Field::IsFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                EXPECT_EQ(10, Field::Index);
                if constexpr ( Field::IsStatic && Field::IsFunction ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(2, index);
}

TEST(ReflectTestFields, FieldAt)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::At(0, [&](auto & field) {
        EXPECT_EQ(0, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(1, [&](auto & field) {
        EXPECT_EQ(1, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(2, [&](auto & field) {
        EXPECT_EQ(2, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(3, [&](auto & field) {
        EXPECT_EQ(3, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(4, [&](auto & field) {
        EXPECT_EQ(4, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(5, [&](auto & field) {
        EXPECT_EQ(5, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(6, [&](auto & field) {
        EXPECT_EQ(6, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(7, [&](auto & field) {
        EXPECT_EQ(7, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(8, [&](auto & field) {
        EXPECT_EQ(8, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(9, [&](auto & field) {
        EXPECT_EQ(9, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At(10, [&](auto & field) {
        EXPECT_EQ(10, field.Index);
        visitCount++;
    });
    EXPECT_EQ(11, visitCount);
}

TEST(ReflectTestFields, FieldAtFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::At<std::is_same, int>(0, [&](auto & field) {
        EXPECT_EQ(0, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At<TestFilters::ReflectSubObjs>(1, [&](auto & field) {
        EXPECT_EQ(1, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At<TestFilters::ReflectSubObjs>(2, [&](auto & field) {
        EXPECT_TRUE(false); // Should not be visited since field at index 2 is an int array, not ReflectSubObj
    });
    EXPECT_EQ(2, visitCount);
}

TEST(ReflectTestFields, AtStatic)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::At(0, [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::At(1, [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::At(2, [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::At(3, [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::At(4, [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::At(5, [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::At(6, [&](auto & field, auto & value) {
        EXPECT_EQ(6, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(7, [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::At(8, [&](auto & field, auto & value) {
        EXPECT_EQ(8, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(9, [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::At(10, [&](auto & field, auto & value) {
        EXPECT_EQ(10, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectTestFields, AtStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::At<TestFilters::Integers>(6, [&](auto & field, auto & value) {
        EXPECT_EQ(6, field.Index);
        if constexpr ( std::is_same_v<int, std::remove_const_t<std::remove_reference_t<decltype(value)>>>  ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At<TestFilters::Integers>(8, [&](auto & field, auto & value) {
        EXPECT_EQ(8, field.Index);
        if constexpr ( std::is_same_v<int, std::remove_const_t<std::remove_reference_t<decltype(value)>>> ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At<TestFilters::ReflectSubObjs>(6, [&](auto & field, auto & value) {
        EXPECT_TRUE(false); // Should not be visited since field at index 6 is an int, not ReflectSubObj
    });
    EXPECT_EQ(2, visitCount);
}

TEST(ReflectTestFields, AtInstanced)
{
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

    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::At(0, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(0, field.Index);
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(1, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(1, field.Index);
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(2, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(2, field.Index);
        if constexpr ( is_static_array<std::remove_reference_t<decltype(value)>>::value ) {
            EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
            EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(3, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(3, field.Index);
        if constexpr ( is_pair<typename element_type<std::remove_reference_t<decltype(value)>>::type>::value ) {
            EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
            EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(4, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(4, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( is_reflected<typename element_type<Value>::type>::value && is_iterable<Value>::value ) {
            EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
            EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(5, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(5, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && is_adaptor<Value>::value ) {
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.pop();
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.push(3);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(6, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(6, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(reflectObj.staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(7, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(7, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.primitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(8, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(8, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(9, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(9, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::At(10, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(10, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(11, visitCount);
}

TEST(ReflectTestFields, AtInstancedFiltered)
{
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

    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::At<TestFilters::Integers>(0, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(0, field.Index);
        EXPECT_EQ(reflectObj.primitive, value);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At<TestFilters::ReflectSubObjs>(1, reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(1, field.Index);
        EXPECT_EQ(reflectObj.object.val, value.val);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::At<TestFilters::ReflectSubObjs>(2, reflectObj, [&](auto & field, auto & value) {
        EXPECT_TRUE(false); // Should not be visited since field at index 2 is an int array, not ReflectSubObj
    });
    EXPECT_EQ(2, visitCount);
}

TEST(ReflectTestFields, NamedField)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::Named("", [&](auto & field) { EXPECT_TRUE(false); }); // Not a field name/should not be visited
    Reflect<ReflectObj>::Fields::Named("unknown", [&](auto & field) { EXPECT_TRUE(false); }); // Not a field name/should not be visited
    Reflect<ReflectObj>::Fields::Named("PrimitivE", [&](auto & field) { EXPECT_TRUE(false); }); // Not a field name/should not be visited
    Reflect<ReflectObj>::Fields::Named("primitive", [&](auto & field) {
        EXPECT_EQ(0, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("object", [&](auto & field) {
        EXPECT_EQ(1, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("primitiveArray", [&](auto & field) {
        EXPECT_EQ(2, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("map", [&](auto & field) {
        EXPECT_EQ(3, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("objCollection", [&](auto & field) {
        EXPECT_EQ(4, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("stack", [&](auto & field) {
        EXPECT_EQ(5, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("staticPrimitive", [&](auto & field) {
        EXPECT_EQ(6, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("primitiveReference", [&](auto & field) {
        EXPECT_EQ(7, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("staticPrimitiveReference", [&](auto & field) {
        EXPECT_EQ(8, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("memberMethod", [&](auto & field) {
        EXPECT_EQ(9, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named("staticMethod", [&](auto & field) {
        EXPECT_EQ(10, field.Index);
        visitCount++;
    });
    EXPECT_EQ(11, visitCount);
}

TEST(ReflectTestFields, NamedFieldFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::Named<Filter::IsData>("", [&](auto & field) { EXPECT_TRUE(false); }); // Not a field name/should not be visited
    Reflect<ReflectObj>::Fields::Named<std::is_same, float>("primitive", [&](auto & field) { EXPECT_TRUE(false); }); // Not a float/should not be visited
    Reflect<ReflectObj>::Fields::Named<std::is_same, int>("primitive", [&](auto & field) {
        EXPECT_EQ(0, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<TestFilters::ReflectSubObjs>("object", [&](auto & field) {
        EXPECT_EQ(1, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<Filter::IsData>("primitiveArray", [&](auto & field) {
        EXPECT_EQ(2, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<Filter::Is<std::map>::Specialization>("map", [&](auto & field) {
        EXPECT_EQ(3, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<Filter::Is<std::vector>::Specialization>("objCollection", [&](auto & field) {
        EXPECT_EQ(4, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<Filter::IsInstanceData>("stack", [&](auto & field) {
        EXPECT_EQ(5, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<TestFilters::Integers>("staticPrimitive", [&](auto & field) {
        EXPECT_EQ(6, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<std::is_same, int &>("primitiveReference", [&](auto & field) {
        EXPECT_EQ(7, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<Filter::IsStaticField>("staticPrimitiveReference", [&](auto & field) {
        EXPECT_EQ(8, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<Filter::IsInstanceFunction>("memberMethod", [&](auto & field) {
        EXPECT_EQ(9, field.Index);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<Filter::IsStaticFunction>("staticMethod", [&](auto & field) {
        EXPECT_EQ(10, field.Index);
        visitCount++;
    });
    EXPECT_EQ(11, visitCount);
}

TEST(ReflectTestFields, NamedStatic)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::Named("primitive", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named("object", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named("primitiveArray", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named("map", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named("objCollection", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named("stack", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named("staticPrimitive", [&](auto & field, auto & value) {
        EXPECT_EQ(6, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("primitiveReference", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named("staticPrimitiveReference", [&](auto & field, auto & value) {
        EXPECT_EQ(8, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("memberMethod", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named("staticMethod", [&](auto & field, auto & value) {
        EXPECT_EQ(10, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectTestFields, NamedStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::Named<TestFilters::Integers>("primitive", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named<Filter::None>("object", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named<Filter::None>("primitiveArray", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named<Filter::None>("map", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named<Filter::None>("objCollection", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named<Filter::None>("stack", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named<std::is_same, float>("staticPrimitive", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-float/should not be visited
    Reflect<ReflectObj>::Fields::Named<std::is_same, int>("staticPrimitive", [&](auto & field, auto & value) {
        EXPECT_EQ(6, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named<Filter::None>("primitiveReference", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named<std::is_same, float &>("staticPrimitiveReference", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-float/should not be visited
    Reflect<ReflectObj>::Fields::Named<std::is_same, int &>("staticPrimitiveReference", [&](auto & field, auto & value) {
        EXPECT_EQ(8, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named<Filter::None>("memberMethod", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Fields::Named<Filter::IsInstanceFunction>("staticMethod", [&](auto & field, auto & value) { EXPECT_TRUE(false); }); // Non-instance/should not be visited
    Reflect<ReflectObj>::Fields::Named<Filter::IsStaticFunction>("staticMethod", [&](auto & field, auto & value) {
        EXPECT_EQ(10, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectTestFields, NamedInstanced)
{
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

    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::Named("primitive", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(0, field.Index);
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("object", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(1, field.Index);
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("primitiveArray", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(2, field.Index);
        if constexpr ( is_static_array<std::remove_reference_t<decltype(value)>>::value ) {
            EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
            EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("map", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(3, field.Index);
        if constexpr ( is_pair<typename element_type<std::remove_reference_t<decltype(value)>>::type>::value ) {
            EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
            EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("objCollection", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(4, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( is_reflected<typename element_type<Value>::type>::value && is_iterable<Value>::value ) {
            EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
            EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("stack", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(5, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && is_adaptor<Value>::value ) {
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.pop();
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.push(3);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("staticPrimitive", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(6, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(reflectObj.staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("primitiveReference", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(7, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.primitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("staticPrimitiveReference", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(8, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("memberMethod", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(9, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Fields::Named("staticMethod", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(10, field.Index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(11, visitCount);
}

TEST(ReflectTestFields, NamedInstancedFiltered)
{
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

    size_t visitCount = 0;
    Reflect<ReflectObj>::Fields::Named<TestFilters::Integers>("primitive", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(0, field.Index);
        EXPECT_EQ(reflectObj.primitive, value);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<TestFilters::ReflectSubObjs>("object", reflectObj, [&](auto & field, auto & value) {
        EXPECT_EQ(1, field.Index);
        EXPECT_EQ(reflectObj.object.val, value.val);
        visitCount++;
    });
    Reflect<ReflectObj>::Fields::Named<TestFilters::ReflectSubObjs>("primitiveArray", reflectObj, [&](auto & field, auto & value) {
        EXPECT_TRUE(false); // Should as this is an array, not ReflectSubObj
    });
    EXPECT_EQ(2, visitCount);
}

TEST(ReflectTestValues, Pack)
{
    ReflectPackTest obj {};

    bool visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<decltype(&ReflectPackTest::memberValue), decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const nullptr_t, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::memberValue)>>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const nullptr_t &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::memberMethod)>>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::staticMethod)>>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberValueReferenceType = std::tuple_element_t<2, Ts>;
        using StaticValueReferenceType = std::tuple_element_t<3, Ts>;
        using MemberMethodType = std::tuple_element_t<4, Ts>;
        using StaticMethodType = std::tuple_element_t<5, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);
        
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<decltype(&ReflectPackTest::memberValue), decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberValueReferenceType = std::tuple_element_t<2, Ts>;
        using StaticValueReferenceType = std::tuple_element_t<3, Ts>;
        using MemberMethodType = std::tuple_element_t<4, Ts>;
        using StaticMethodType = std::tuple_element_t<5, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberValue)>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t &, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](auto && ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberValueReferenceType = std::tuple_element_t<2, Ts>;
        using StaticValueReferenceType = std::tuple_element_t<3, Ts>;
        using MemberMethodType = std::tuple_element_t<4, Ts>;
        using StaticMethodType = std::tuple_element_t<5, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<decltype(&ReflectPackTest::memberValue), decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](const auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberValueReferenceType = std::tuple_element_t<2, Ts>;
        using StaticValueReferenceType = std::tuple_element_t<3, Ts>;
        using MemberMethodType = std::tuple_element_t<4, Ts>;
        using StaticMethodType = std::tuple_element_t<5, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberValue)>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const nullptr_t, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberMethod)>, MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::staticMethod)>, StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack([&](const auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberValueReferenceType = std::tuple_element_t<2, Ts>;
        using StaticValueReferenceType = std::tuple_element_t<3, Ts>;
        using MemberMethodType = std::tuple_element_t<4, Ts>;
        using StaticMethodType = std::tuple_element_t<5, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::memberValue)>>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const nullptr_t &, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::memberMethod)>>, MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::staticMethod)>>, StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    // Note: class members are always lvalues and don't have a need to be passed as rvalue references
}

TEST(ReflectTestValues, PackInstanced)
{
    ReflectPackTest obj {};

    bool visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<int, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<int &, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<int &, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<const int, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<const int &, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::memberMethod)>>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::staticMethod)>>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberValueReferenceType = std::tuple_element_t<2, Ts>;
        using StaticValueReferenceType = std::tuple_element_t<3, Ts>;
        using MemberMethodType = std::tuple_element_t<4, Ts>;
        using StaticMethodType = std::tuple_element_t<5, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::remove_reference_t<std::tuple_element_t<0, Ts>>;
        using StaticType = std::remove_reference_t<std::tuple_element_t<1, Ts>>;
        using MemberValueReferenceType = std::remove_reference_t<std::tuple_element_t<2, Ts>>;
        using StaticValueReferenceType = std::remove_reference_t<std::tuple_element_t<3, Ts>>;
        using MemberMethodType = std::remove_reference_t<std::tuple_element_t<4, Ts>>;
        using StaticMethodType = std::remove_reference_t<std::tuple_element_t<5, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](auto && ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::remove_reference_t<std::tuple_element_t<0, Ts>>;
        using StaticType = std::remove_reference_t<std::tuple_element_t<1, Ts>>;
        using MemberValueReferenceType = std::remove_reference_t<std::tuple_element_t<2, Ts>>;
        using StaticValueReferenceType = std::remove_reference_t<std::tuple_element_t<3, Ts>>;
        using MemberMethodType = std::remove_reference_t<std::tuple_element_t<4, Ts>>;
        using StaticMethodType = std::remove_reference_t<std::tuple_element_t<5, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](const auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::remove_const_t<std::tuple_element_t<0, Ts>>;
        using StaticType = std::remove_const_t<std::tuple_element_t<1, Ts>>;
        using MemberValueReferenceType = std::remove_const_t<std::tuple_element_t<2, Ts>>;
        using StaticValueReferenceType = std::remove_const_t<std::tuple_element_t<3, Ts>>;
        using MemberMethodType = std::remove_const_t<std::tuple_element_t<4, Ts>>;
        using StaticMethodType = std::remove_const_t<std::tuple_element_t<5, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::Pack(obj, [&](const auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(6, size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<0, Ts>>>;
        using StaticType = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<1, Ts>>>;
        using MemberValueReferenceType = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<2, Ts>>>;
        using StaticValueReferenceType = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<3, Ts>>>;
        using MemberMethodType = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<4, Ts>>>;
        using StaticMethodType = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<5, Ts>>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(1, memberValueReference);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    // Note: class members are always lvalues and don't have a need to be passed as rvalue references
}

TEST(ReflectTestValues, ForEachStatic)
{
    size_t index = 0;
    Reflect<ReflectObj>::Values::ForEach([&](auto & value) {
        
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( std::is_same_v<int, Value> )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( std::is_same_v<int, Value> )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( std::is_pointer_v<Value> ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(3, index);
}

TEST(ReflectTestValues, ForEachStaticFiltered)
{
    size_t index = 0;
    Reflect<ReflectObj>::Values::ForEach<Filter::IsStaticData>([&](auto & value) {
        
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( std::is_same_v<int, Value> )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( std::is_same_v<int, Value> )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
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
    EXPECT_EQ(2, index);

    index = 0;
    Reflect<ReflectObj>::Values::ForEach<Filter::IsStaticFunction>([&](auto & value) {
        
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( std::is_pointer_v<Value> ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(1, index);
}

TEST(ReflectTestValues, ForEachInstanced)
{
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
    Reflect<ReflectObj>::Values::ForEach(reflectObj, [&](auto & value) {
        
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
                    using ObjClass = typename Value::Class;
                    Reflect<Value>::Fields::At(0, value, [&](auto & field, auto & value) {
                        EXPECT_EQ(reflectObj.object.val, value);
                        visited = true;
                    });
                    EXPECT_TRUE(visited);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( is_static_array<Value>::value ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( is_pair<typename element_type<Value>::type>::value ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( is_reflected<typename element_type<Value>::type>::value && is_iterable<Value>::value ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !is_reflected<Value>::value && is_adaptor<Value>::value ) {
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.pop();
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.push(3);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
                    EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 9:
                if constexpr ( std::is_member_pointer_v<Value> ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 10:
                if constexpr ( std::is_pointer_v<Value> ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(11, index);
}

TEST(ReflectTestValues, ForEachInstancedFiltered)
{
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
    Reflect<ReflectObj>::Values::ForEach<Filter::IsData>(reflectObj, [&](auto & value) {
        
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
                    using ObjClass = typename Value::Class;
                    Reflect<Value>::Fields::At(0, value, [&](auto & field, auto & value) {
                        EXPECT_EQ(reflectObj.object.val, value);
                        visited = true;
                    });
                    EXPECT_TRUE(visited);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( is_static_array<Value>::value ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( is_pair<typename element_type<Value>::type>::value ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( is_reflected<typename element_type<Value>::type>::value && is_iterable<Value>::value ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !is_reflected<Value>::value && is_adaptor<Value>::value )
                {
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.pop();
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.push(3);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> )
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
    Reflect<ReflectObj>::Values::ForEach<Filter::IsFunction>(reflectObj, [&](auto & value) {
        
        using Value = typename std::remove_reference<decltype(value)>::type;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( std::is_member_pointer_v<Value> ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( std::is_pointer_v<Value> ) {
                    auto returned = value();
                    EXPECT_EQ(2, returned);
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
    EXPECT_EQ(2, index);
}

TEST(ReflectTestValues, AtStatic)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::At(0, [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::At(1, [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::At(2, [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::At(3, [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::At(4, [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::At(5, [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::At(6, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(7, [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::At(8, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(9, [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::At(10, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectTestValues, AtStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::At<std::is_same, int>(6, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_const_t<std::remove_reference_t<decltype(value)>>>  ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At<std::is_same, int &>(8, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_const_t<std::remove_reference_t<decltype(value)>>> ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At<TestFilters::ReflectSubObjs>(10, [&](auto & value) {
        // Should not be visited as "staticMethod" is a function pointer, not a ReflectedSubObj
    });
    EXPECT_EQ(2, visitCount);
}

TEST(ReflectTestValues, AtInstanced)
{
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

    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::At(0, reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(1, reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(2, reflectObj, [&](auto & value) {
        if constexpr ( is_static_array<std::remove_reference_t<decltype(value)>>::value ) {
            EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
            EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(3, reflectObj, [&](auto & value) {
        if constexpr ( is_pair<typename element_type<std::remove_reference_t<decltype(value)>>::type>::value ) {
            EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
            EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(4, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( is_reflected<typename element_type<Value>::type>::value && is_iterable<Value>::value ) {
            EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
            EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(5, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && is_adaptor<Value>::value ) {
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.pop();
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.push(3);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(6, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(reflectObj.staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(7, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.primitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(8, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(9, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At(10, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(11, visitCount);
}

TEST(ReflectTestValues, AtInstancedFiltered)
{
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

    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::At<std::is_same, int>(0, reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At<TestFilters::ReflectSubObjs>(1, reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::At<TestFilters::ReflectSubObjs>(2, reflectObj, [&](auto & value) {
        EXPECT_TRUE(false); // Should not be visited since field at index 2 is an int array, not ReflectSubObj
    });
    EXPECT_EQ(2, visitCount);
}

TEST(ReflectTestValues, NamedStatic)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::Named("primitive", [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::Named("object", [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::Named("primitiveArray", [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::Named("map", [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::Named("objCollection", [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::Named("stack", [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::Named("staticPrimitive", [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("primitiveReference", [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::Named("staticPrimitiveReference", [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("memberMethod", [&](auto & value) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::Named("staticMethod", [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectTestValues, NamedStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::Named<std::is_same, int>("staticPrimitive", [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_const_t<std::remove_reference_t<decltype(value)>>>  ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named<std::is_same, int &>("staticPrimitiveReference", [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_const_t<std::remove_reference_t<decltype(value)>>> ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named<TestFilters::ReflectSubObjs>("staticMethod", [&](auto & value) {
        // Should not be visited as "staticMethod" is a function pointer, not a ReflectedSubObj
    });
    EXPECT_EQ(2, visitCount);
}

TEST(ReflectTestValues, NamedInstanced)
{
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

    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::Named("primitive", reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("object", reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("primitiveArray", reflectObj, [&](auto & value) {
        if constexpr ( is_static_array<std::remove_reference_t<decltype(value)>>::value ) {
            EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
            EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("map", reflectObj, [&](auto & value) {
        if constexpr ( is_pair<typename element_type<std::remove_reference_t<decltype(value)>>::type>::value ) {
            EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
            EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("objCollection", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( is_reflected<typename element_type<Value>::type>::value && is_iterable<Value>::value ) {
            EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
            EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("stack", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && is_adaptor<Value>::value ) {
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.pop();
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.push(3);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("staticPrimitive", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && std::is_same_v<int, Value> ) {
            EXPECT_EQ(reflectObj.staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("primitiveReference", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.primitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("staticPrimitiveReference", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected<Value>::value && !is_iterable<Value>::value && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("memberMethod", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named("staticMethod", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(11, visitCount);
}

TEST(ReflectTestValues, NamedInstancedFiltered)
{
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

    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::Named<std::is_same, int>("primitive", reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named<TestFilters::ReflectSubObjs>("object", reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::Named<TestFilters::ReflectSubObjs>("primitiveArray", reflectObj, [&](auto & value) {
        EXPECT_TRUE(false); // Should not be visited since field at index 2 is an int array, not ReflectSubObj
    });
    EXPECT_EQ(2, visitCount);
}

// TODO: Decide whether we're keeping (and unit testing) or getting rid of members

struct NonNoted
{
    int a;

    REFLECT(NonNoted, a)
};

NOTE(EmptyNotes)
struct EmptyNotes
{
    int a;

    REFLECT_NOTED(EmptyNotes, a)
};

NOTE(SinglyNoted, 'a')
struct SinglyNoted
{
    int a;

    REFLECT_NOTED(SinglyNoted, a)
};

struct SerializedName
{
    std::string_view value;
};

NOTE(StructNoted, SerializedName { "noted" })
struct StructNoted
{
    int a;

    REFLECT_NOTED(StructNoted, a)
};

NOTE(ComplexNoted, std::tuple{33, SerializedName{"test"}}, 'c', 'd', 'e')
struct ComplexNoted
{
    int a;

    REFLECT_NOTED(ComplexNoted, a)
};

template <typename T>
struct Specializable { using type = T; };

NOTE(SpecializedNoted, 'a', Specializable<int>{})
struct SpecializedNoted
{
    int a;

    REFLECT_NOTED(SpecializedNoted, a)
};

TEST(ReflectTestNotes, ForEach)
{
    int visitCount = 0;;
    Reflect<NonNoted>::Notes::ForEach([&](auto & annotation) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<EmptyNotes>::Notes::ForEach([&](auto & annotation) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<SinglyNoted>::Notes::ForEach([&](auto & annotation) {
        using Annotation = std::remove_const_t<std::remove_reference_t<decltype(annotation)>>;
        bool isSame = std::is_same_v<char, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('a', annotation);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<StructNoted>::Notes::ForEach([&](auto & annotation) {
        using Annotation = std::remove_const_t<std::remove_reference_t<decltype(annotation)>>;
        bool isSame = std::is_same_v<SerializedName, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_STREQ("noted", std::string(annotation.value).c_str());
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<ComplexNoted>::Notes::ForEach([&](auto & annotation) {
        using Annotation = std::remove_const_t<std::remove_reference_t<decltype(annotation)>>;
        if constexpr ( std::is_same_v<Annotation, std::remove_const_t<std::tuple_element_t<0, decltype(ComplexNoted_note)>>> )
        {
            EXPECT_EQ(0, visitCount);
            ++visitCount;
        }
        else if constexpr ( std::is_same_v<Annotation, char> )
        {
            switch ( visitCount ) {
                case 1: EXPECT_EQ('c', annotation); break;
                case 2: EXPECT_EQ('d', annotation); break;
                case 3: EXPECT_EQ('e', annotation); break;
                default: EXPECT_TRUE(false); break;
            }
            ++visitCount;
        }
        else
        {
            EXPECT_TRUE(false); // Should not reach this branch
        }
    });
    EXPECT_EQ(4, visitCount);
}

TEST(ReflectTestNotes, ForEachFiltered)
{
    int visitCount = 0;;
    Reflect<NonNoted>::Notes::ForEach<int>([&](auto & annotation) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<EmptyNotes>::Notes::ForEach<int>([&](auto & annotation) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<SinglyNoted>::Notes::ForEach<char>([&](auto & annotation) {
        using Annotation = std::remove_const_t<std::remove_reference_t<decltype(annotation)>>;
        bool isSame = std::is_same_v<char, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('a', annotation);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<StructNoted>::Notes::ForEach<SerializedName>([&](auto & annotation) {
        using Annotation = std::remove_const_t<std::remove_reference_t<decltype(annotation)>>;
        bool isSame = std::is_same_v<SerializedName, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_STREQ("noted", std::string(annotation.value).c_str());
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<ComplexNoted>::Notes::ForEach<char>([&](auto & annotation) {
        switch ( visitCount ) {
            case 0: EXPECT_EQ('c', annotation); break;
            case 1: EXPECT_EQ('d', annotation); break;
            case 2: EXPECT_EQ('e', annotation); break;
            default: EXPECT_TRUE(false); break;
        }
        ++visitCount;
    });
    EXPECT_EQ(3, visitCount);

    visitCount = 0;
    Reflect<ComplexNoted>::Notes::ForEach<std::remove_const_t<std::tuple_element_t<0, decltype(ComplexNoted_note)>>>([&](auto & annotation) {
        EXPECT_EQ(0, visitCount);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
}

TEST(ReflectTestNotes, Has)
{
    bool hasNote = Reflect<NonNoted>::Notes::template Has<int>;
    EXPECT_FALSE(hasNote);

    hasNote = Reflect<EmptyNotes>::Notes::template Has<int>;
    EXPECT_FALSE(hasNote);
    
    hasNote = Reflect<SinglyNoted>::Notes::template Has<int>;
    EXPECT_FALSE(hasNote);
    hasNote = Reflect<SinglyNoted>::Notes::template Has<char>;
    EXPECT_TRUE(hasNote);
    
    hasNote = Reflect<StructNoted>::Notes::template Has<int>;
    EXPECT_FALSE(hasNote);
    hasNote = Reflect<StructNoted>::Notes::template Has<SerializedName>;
    EXPECT_TRUE(hasNote);
    
    hasNote = Reflect<ComplexNoted>::Notes::template Has<int>;
    EXPECT_FALSE(hasNote);
    hasNote = Reflect<ComplexNoted>::Notes::template Has<char>;
    EXPECT_TRUE(hasNote);
}

TEST(ReflectTestNotes, HasSpecialization)
{
    bool hasSpecialization = Reflect<NonNoted>::Notes::template HasSpecialization<Specializable>;
    EXPECT_FALSE(hasSpecialization);

    hasSpecialization = Reflect<EmptyNotes>::Notes::template HasSpecialization<Specializable>;
    EXPECT_FALSE(hasSpecialization);
    
    hasSpecialization = Reflect<SinglyNoted>::Notes::template HasSpecialization<Specializable>;
    EXPECT_FALSE(hasSpecialization);
    
    hasSpecialization = Reflect<StructNoted>::Notes::template HasSpecialization<Specializable>;
    EXPECT_FALSE(hasSpecialization);
    
    hasSpecialization = Reflect<ComplexNoted>::Notes::template HasSpecialization<Specializable>;
    EXPECT_FALSE(hasSpecialization);

    hasSpecialization = Reflect<SpecializedNoted>::Notes::template HasSpecialization<Specializable>;
    EXPECT_TRUE(hasSpecialization);
}

TEST(ReflectTestNotes, Get)
{
    auto singlyNotedChar = Reflect<SinglyNoted>::Notes::Get<char>();
    bool isSame = std::is_same_v<char, decltype(singlyNotedChar)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', singlyNotedChar);

    constexpr auto & structNotedSerializedName = Reflect<StructNoted>::Notes::Get<SerializedName>();
    isSame = std::is_same_v<SerializedName, std::remove_const_t<std::remove_reference_t<decltype(structNotedSerializedName)>>>;
    EXPECT_TRUE(isSame);
    EXPECT_STREQ("noted", std::string(structNotedSerializedName.value).c_str());

    using NestedTuple = std::remove_const_t<std::tuple_element_t<0, decltype(ComplexNoted_note)>>;
    constexpr auto & complexNotedTuple = Reflect<ComplexNoted>::Notes::Get<NestedTuple>();
    isSame = std::is_same_v<NestedTuple, std::remove_const_t<std::remove_reference_t<decltype(complexNotedTuple)>>>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectTestNotes, GetSpecialization)
{
    int visitCount = 0;;
    Reflect<NonNoted>::Notes::ForEach<int>([&](auto & annotation) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<EmptyNotes>::Notes::ForEach<int>([&](auto & annotation) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<SinglyNoted>::Notes::ForEach<char>([&](auto & annotation) {
        using Annotation = std::remove_const_t<std::remove_reference_t<decltype(annotation)>>;
        bool isSame = std::is_same_v<char, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('a', annotation);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<StructNoted>::Notes::ForEach<SerializedName>([&](auto & annotation) {
        using Annotation = std::remove_const_t<std::remove_reference_t<decltype(annotation)>>;
        bool isSame = std::is_same_v<SerializedName, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_STREQ("noted", std::string(annotation.value).c_str());
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<ComplexNoted>::Notes::ForEach<char>([&](auto & annotation) {
        switch ( visitCount ) {
            case 0: EXPECT_EQ('c', annotation); break;
            case 1: EXPECT_EQ('d', annotation); break;
            case 2: EXPECT_EQ('e', annotation); break;
            default: EXPECT_TRUE(false); break;
        }
        ++visitCount;
    });
    EXPECT_EQ(3, visitCount);

    visitCount = 0;
    Reflect<ComplexNoted>::Notes::ForEach<std::remove_const_t<std::tuple_element_t<0, decltype(ComplexNoted_note)>>>([&](auto & annotation) {
        EXPECT_EQ(0, visitCount);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
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
    Reflect<ReflectReferences>::Fields::At(1, reflectReferences, [&](auto & field, auto & value) {
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
    Reflect<ReflectReferences>::Fields::At(3, reflectReferences, [&](auto & field, auto & value) {
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

struct ReflectOffsets
{
    uint16_t a;
    int b;
    float c;
    double d;
    char e;
    size_t f;

    REFLECT(ReflectOffsets, a, b, c, d, e, f)
};

TEST(ReflectionTest, RfMacroReflectOffsets)
{
    constexpr bool isStandardLayout = std::is_standard_layout_v<ReflectOffsets>;
    EXPECT_TRUE(isStandardLayout);

    ReflectOffsets reflectOffsets { uint16_t(1), 2, 3.0f, 4.0, '5', size_t(6) };

    Reflect<ReflectOffsets>::Fields::ForEach(reflectOffsets, [&](auto & field, auto & value) {
        using Field = std::remove_reference_t<decltype(field)>;
        constexpr bool fieldHasOffset = Field::HasOffset;
        EXPECT_TRUE(fieldHasOffset);

        size_t fieldOffset = std::numeric_limits<size_t>::max();
        switch ( field.Index )
        {
            case 0: fieldOffset = offsetof(ReflectOffsets, a); break;
            case 1: fieldOffset = offsetof(ReflectOffsets, b); break;
            case 2: fieldOffset = offsetof(ReflectOffsets, c); break;
            case 3: fieldOffset = offsetof(ReflectOffsets, d); break;
            case 4: fieldOffset = offsetof(ReflectOffsets, e); break;
            case 5: fieldOffset = offsetof(ReflectOffsets, f); break;
        }
        EXPECT_EQ(fieldOffset, field.getOffset());
    });
}

NOTE(ReflectEmptyTemplate, 'a')
template <typename T>
struct ReflectEmptyTemplate
{
    REFLECT_NOTED(ReflectEmptyTemplate)
};

TEST(ReflectionTest, ReflectEmptyTemplate)
{
    using Instantiation = ReflectEmptyTemplate<int>;

    bool isReflected = Reflection::is_reflected_v<Instantiation>;
    EXPECT_TRUE(isReflected);
    EXPECT_EQ(0, Instantiation::Class::Total);

    char a = std::get<0>(Instantiation::Class::annotations);
    EXPECT_EQ(a, 'a');
}

template <typename T>
struct ReflectTemplate
{
    int a = 1;
    int b = 2;

    REFLECT(ReflectTemplate, a, b)
};

TEST(ReflectionTest, ReflectTemplate)
{
    using Instantiation = ReflectTemplate<int>;

    bool isReflected = Reflection::is_reflected_v<Instantiation>;
    EXPECT_TRUE(isReflected);
    EXPECT_EQ(2, Instantiation::Class::Total);

    int total = 0;
    Instantiation instance {};
    Reflect<Instantiation>::Fields::ForEach(instance, [&](auto & field, auto & value) {
        total += value;
    });
    EXPECT_EQ(3, total);

    int a = 0;
    Reflect<Instantiation>::Fields::At(0, instance, [&](auto & field, auto & value) {
        a = value;
    });
    EXPECT_EQ(1, a);
    int b = 0;
    Reflect<Instantiation>::Fields::At(1, instance, [&](auto & field, auto & value) {
        b = value;
    });
    EXPECT_EQ(2, b);
}

NOTE(ReflectNotedTemplate, 'b')
template <typename T>
struct ReflectNotedTemplate
{
    int a = 1;
    int b = 2;

    REFLECT_NOTED(ReflectNotedTemplate, a, b)
};

TEST(ReflectionTest, ReflectNotedTemplate)
{
    using Instantiation = ReflectNotedTemplate<int>;

    bool isReflected = Reflection::is_reflected_v<Instantiation>;
    EXPECT_TRUE(isReflected);
    EXPECT_EQ(2, Instantiation::Class::Total);

    char b = std::get<0>(Instantiation::Class::annotations);
    EXPECT_EQ(b, 'b');

    int total = 0;
    Instantiation instance {};
    Reflect<Instantiation>::Fields::ForEach(instance, [&](auto & field, auto & value) {
        total += value;
    });
    EXPECT_EQ(3, total);

    int a = 0;
    Reflect<Instantiation>::Fields::At(0, instance, [&](auto & field, auto & value) {
        a = value;
    });
    EXPECT_EQ(1, a);
    int bVal = 0;
    Reflect<Instantiation>::Fields::At(1, instance, [&](auto & field, auto & value) {
        bVal = value;
    });
    EXPECT_EQ(2, bVal);
}

template <typename T>
struct ReflectTemplateToField
{
    T a = 1;

    REFLECT(ReflectTemplateToField, a)
};

TEST(ReflectionTest, ReflectTemplateToField)
{
    using Instantiation = ReflectTemplateToField<int>;

    bool isReflected = Reflection::is_reflected_v<Instantiation>;
    EXPECT_TRUE(isReflected);
    EXPECT_EQ(1, Instantiation::Class::Total);

    int total = 0;
    Instantiation instance {};
    Reflect<Instantiation>::Fields::ForEach(instance, [&](auto & field, auto & value) {
        total += value;
    });
    EXPECT_EQ(1, total);

    int a = 0;
    Reflect<Instantiation>::Fields::At(0, instance, [&](auto & field, auto & value) {
        a = value;
    });
    EXPECT_EQ(1, a);
}

template <typename T, typename U>
struct ReflectMultiTemplateArg
{
    std::pair<T, U> p;

    REFLECT(ReflectMultiTemplateArg, p)
};

TEST(ReflectionTest, ReflectMultiTemplateArg)
{
    using Instantiation = ReflectMultiTemplateArg<int, std::string>;

    bool isReflected = Reflection::is_reflected_v<Instantiation>;
    EXPECT_TRUE(isReflected);
    EXPECT_EQ(1, Instantiation::Class::Total);

    int total = 0;
    Instantiation instance { {1, "one"} };
    Reflect<Instantiation>::Fields::ForEach(instance, [&](auto & field, auto & value) {
        total += value.first;
    });
    EXPECT_EQ(1, total);

    int l = 0;
    std::string r {};
    Reflect<Instantiation>::Fields::At(0, instance, [&](auto & field, auto & value) {
        l = value.first;
        r = value.second;
    });
    EXPECT_EQ(1, l);
    EXPECT_STREQ("one", r.c_str());
}

template <typename T>
struct ReflectPartialSpecialization
{
    int a;

    REFLECT(ReflectPartialSpecialization, a)
};

template <>
struct ReflectPartialSpecialization<uint16_t>
{
    int b;
    int c;

    REFLECT(ReflectPartialSpecialization, b, c)
};

template <>
struct ReflectPartialSpecialization<int16_t>
{
    int b;
    int c;
};

TEST(ReflectionTest, ReflectPartialSpecialization)
{
    using PrimaryTemplateType = ReflectPartialSpecialization<int>;
    using PartialSpecializationType = ReflectPartialSpecialization<uint16_t>;
    using UnreflectedSpecializationType = ReflectPartialSpecialization<int16_t>;

    bool primaryTemplateTypeIsReflected = Reflection::is_reflected_v<PrimaryTemplateType>;
    bool partialSpecializationTypeIsReflected = Reflection::is_reflected_v<PartialSpecializationType>;
    bool unreflectedSpecializationTypeIsReflected = Reflection::is_reflected_v<UnreflectedSpecializationType>;
    EXPECT_TRUE(primaryTemplateTypeIsReflected);
    EXPECT_TRUE(partialSpecializationTypeIsReflected);
    EXPECT_FALSE(unreflectedSpecializationTypeIsReflected);

    EXPECT_EQ(1, PrimaryTemplateType::Class::Total);
    EXPECT_EQ(2, PartialSpecializationType::Class::Total);

    PrimaryTemplateType ptt { 1 };
    PartialSpecializationType pst { 1, 2 };

    int total = 0;
    Reflect<PrimaryTemplateType>::Fields::ForEach(ptt, [&](auto & field, auto & value) {
        total += value;
    });
    EXPECT_EQ(1, total);

    int a = 0;
    Reflect<PrimaryTemplateType>::Fields::At(0, ptt, [&](auto & field, auto & value) {
        a = value;
    });
    EXPECT_EQ(1, a);

    total = 0;
    Reflect<PartialSpecializationType>::Fields::ForEach(pst, [&](auto & field, auto & value) {
        total += value;
    });
    EXPECT_EQ(3, total);
    
    int b = 0;
    int c = 0;
    Reflect<PartialSpecializationType>::Fields::At(0, pst, [&](auto & field, auto & value) {
        b = value;
    });
    Reflect<PartialSpecializationType>::Fields::At(1, pst, [&](auto & field, auto & value) {
        c = value;
    });
    EXPECT_EQ(1, b);
    EXPECT_EQ(2, c);
}

template <typename T>
struct ReflectSpecializationOnly
{
    int a;
};

template <>
struct ReflectSpecializationOnly<uint16_t>
{
    int b;
    int c;

    REFLECT(ReflectSpecializationOnly, b, c)
};

TEST(ReflectionTest, ReflectSpecializationOnly)
{
    using PrimaryTemplateType = ReflectSpecializationOnly<int>;
    using PartialSpecializationType = ReflectSpecializationOnly<uint16_t>;

    bool primaryTemplateTypeIsReflected = Reflection::is_reflected_v<PrimaryTemplateType>;
    bool partialSpecializationTypeIsReflected = Reflection::is_reflected_v<PartialSpecializationType>;
    EXPECT_FALSE(primaryTemplateTypeIsReflected);
    EXPECT_TRUE(partialSpecializationTypeIsReflected);

    EXPECT_EQ(2, PartialSpecializationType::Class::Total);

    PartialSpecializationType pst { 1, 2 };

    int total = 0;
    Reflect<PartialSpecializationType>::Fields::ForEach(pst, [&](auto & field, auto & value) {
        total += value;
    });
    EXPECT_EQ(3, total);
    
    int b = 0;
    int c = 0;
    Reflect<PartialSpecializationType>::Fields::At(0, pst, [&](auto & field, auto & value) {
        b = value;
    });
    Reflect<PartialSpecializationType>::Fields::At(1, pst, [&](auto & field, auto & value) {
        c = value;
    });
    EXPECT_EQ(1, b);
    EXPECT_EQ(2, c);
}
