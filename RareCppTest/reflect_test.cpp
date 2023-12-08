#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <map>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>
using namespace RareTs;

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
};

struct OwnedObj
{
    int a;
    int b;

    REFLECT(OwnedObj, a, b)
};

template <> struct RareTs::Proxy<UnownedObj1> : public UnownedObj1
{
    REFLECT(RareTs::Proxy<UnownedObj1>, a, b)
};

template <> struct RareTs::Proxy<UnownedObj2> : public UnownedObj2
{
    REFLECT(RareTs::Proxy<UnownedObj2>, a, b, c)
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
    static constexpr int & staticPrimitiveReference = staticPrimitive;
    int memberMethod() { return 1; }
    static int staticMethod() { return 2; }

    REFLECT_NOTED(ReflectObj, primitive, object, primitiveArray, map, objCollection, stack, staticPrimitive, primitiveReference, staticPrimitiveReference, memberMethod, staticMethod)
};

class PrivateIndexOf
{
    PrivateIndexOf() : primitive(0), object({}), primitiveReference(primitive) { primitiveArray[0] = 0; primitiveArray[1] = 1; }

    int primitive;
    ReflectSubObj object;
    int primitiveArray[2];
    std::map<int,float> map;
    std::vector<ReflectSubObj> objCollection;
    std::stack<int> stack;
    static int staticPrimitive;
    int & primitiveReference;
    static constexpr int & staticPrimitiveReference = staticPrimitive;
    int memberMethod() { return 1; }
    static int staticMethod() { return 2; }
    
    inline auto getPrimitive() { return primitive; }
    inline auto getObject() { return object; }
    inline auto getPrimitiveArray() { return primitiveArray; }
    inline auto getMap() { return map; }
    inline auto getObjCollection() { return objCollection; }
    inline auto getStack() { return stack; }
    inline auto getStaticPrimitive() { return staticPrimitive; }
    inline auto getPrimitiveReference() { return primitiveReference; }
    inline auto getStaticPrimitiveReference() { return staticPrimitiveReference; }
    inline auto getMemberMethod() { return &PrivateIndexOf::memberMethod; }
    inline auto getStaticMethod() { return &PrivateIndexOf::staticMethod; }

    REFLECT(PrivateIndexOf, primitive, object, primitiveArray, map, objCollection, stack, staticPrimitive, primitiveReference, staticPrimitiveReference, memberMethod, staticMethod)
};

int ReflectObj::staticPrimitive = 33;

TEST(ReflectTest, IndexOf)
{
    EXPECT_EQ(0, RareTs::IndexOf<ReflectObj>::primitive);
    EXPECT_EQ(1, RareTs::IndexOf<ReflectObj>::object);
    EXPECT_EQ(2, RareTs::IndexOf<ReflectObj>::primitiveArray);
    EXPECT_EQ(3, RareTs::IndexOf<ReflectObj>::map);
    EXPECT_EQ(4, RareTs::IndexOf<ReflectObj>::objCollection);
    EXPECT_EQ(5, RareTs::IndexOf<ReflectObj>::stack);
    EXPECT_EQ(6, RareTs::IndexOf<ReflectObj>::staticPrimitive);
    EXPECT_EQ(7, RareTs::IndexOf<ReflectObj>::primitiveReference);
    EXPECT_EQ(8, RareTs::IndexOf<ReflectObj>::staticPrimitiveReference);
    EXPECT_EQ(9, RareTs::IndexOf<ReflectObj>::memberMethod);
    EXPECT_EQ(10, RareTs::IndexOf<ReflectObj>::staticMethod);
    
    EXPECT_EQ(0, RareTs::IndexOf<PrivateIndexOf>::primitive);
    EXPECT_EQ(1, RareTs::IndexOf<PrivateIndexOf>::object);
    EXPECT_EQ(2, RareTs::IndexOf<PrivateIndexOf>::primitiveArray);
    EXPECT_EQ(3, RareTs::IndexOf<PrivateIndexOf>::map);
    EXPECT_EQ(4, RareTs::IndexOf<PrivateIndexOf>::objCollection);
    EXPECT_EQ(5, RareTs::IndexOf<PrivateIndexOf>::stack);
    EXPECT_EQ(6, RareTs::IndexOf<PrivateIndexOf>::staticPrimitive);
    EXPECT_EQ(7, RareTs::IndexOf<PrivateIndexOf>::primitiveReference);
    EXPECT_EQ(8, RareTs::IndexOf<PrivateIndexOf>::staticPrimitiveReference);
    EXPECT_EQ(9, RareTs::IndexOf<PrivateIndexOf>::memberMethod);
    EXPECT_EQ(10, RareTs::IndexOf<PrivateIndexOf>::staticMethod);
}

TEST(ReflectTest, MemberType)
{
    bool isSame = std::is_same_v<RareTs::Member<ReflectObj, 0>, RareTs::MemberType<ReflectObj>::primitive>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 1>, RareTs::MemberType<ReflectObj>::object>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 2>, RareTs::MemberType<ReflectObj>::primitiveArray>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 3>, RareTs::MemberType<ReflectObj>::map>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 4>, RareTs::MemberType<ReflectObj>::objCollection>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 5>, RareTs::MemberType<ReflectObj>::stack>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 6>, RareTs::MemberType<ReflectObj>::staticPrimitive>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 7>, RareTs::MemberType<ReflectObj>::primitiveReference>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 8>, RareTs::MemberType<ReflectObj>::staticPrimitiveReference>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 9>, RareTs::MemberType<ReflectObj>::memberMethod>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 10>, RareTs::MemberType<ReflectObj>::staticMethod>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectTest, Typedefs)
{
    bool isSame = std::is_same_v<ReflectSuperObj, Reflect<ReflectSuperObj>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectSubObj, Reflect<ReflectSubObj>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectObj, Reflect<ReflectObj>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<OwnedObj, Reflect<OwnedObj>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<UnownedObj1, Reflect<UnownedObj1>::type>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<Reflection::Inherit<ReflectSuperObj>, Reflect<ReflectSuperObj>::Supers>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Inherit<ReflectSubObj>, Reflect<ReflectSubObj>::Supers>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Inherit<ReflectObj, 0>, Reflect<ReflectObj>::Supers>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Inherit<OwnedObj>, Reflect<OwnedObj>::Supers>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::Inherit<UnownedObj1>, Reflect<UnownedObj1>::Supers>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectTestMembers, HeaderDefinitions)
{
    auto totalMembers = Reflect<ReflectSuperObj>::Members::total;
    EXPECT_EQ(size_t(1), totalMembers);
    totalMembers = Reflect<ReflectSubObj>::Members::total;
    EXPECT_EQ(size_t(1), totalMembers);
    totalMembers = Reflect<ReflectObj>::Members::total;
    EXPECT_EQ(size_t(11), totalMembers);

    bool isSame = std::is_same_v<Member<ReflectSuperObj, 0>, Reflect<ReflectSuperObj>::Members::Member<0>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectSubObj, 0>, Reflect<ReflectSubObj>::Members::Member<0>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 0>, Reflect<ReflectObj>::Members::Member<0>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 1>, Reflect<ReflectObj>::Members::Member<1>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 2>, Reflect<ReflectObj>::Members::Member<2>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 3>, Reflect<ReflectObj>::Members::Member<3>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 4>, Reflect<ReflectObj>::Members::Member<4>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 5>, Reflect<ReflectObj>::Members::Member<5>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 6>, Reflect<ReflectObj>::Members::Member<6>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 7>, Reflect<ReflectObj>::Members::Member<7>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 8>, Reflect<ReflectObj>::Members::Member<8>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 9>, Reflect<ReflectObj>::Members::Member<9>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Member<ReflectObj, 10>, Reflect<ReflectObj>::Members::Member<10>>;
    EXPECT_TRUE(isSame);
    
    EXPECT_EQ(size_t(0), Reflect<ReflectSuperObj>::Members::template member<0>.index);
    EXPECT_EQ(size_t(0), Reflect<ReflectSubObj>::Members::template member<0>.index);
    EXPECT_EQ(size_t(0), Reflect<ReflectObj>::Members::template member<0>.index);
    EXPECT_EQ(size_t(1), Reflect<ReflectObj>::Members::template member<1>.index);
    EXPECT_EQ(size_t(2), Reflect<ReflectObj>::Members::template member<2>.index);
    EXPECT_EQ(size_t(3), Reflect<ReflectObj>::Members::template member<3>.index);
    EXPECT_EQ(size_t(4), Reflect<ReflectObj>::Members::template member<4>.index);
    EXPECT_EQ(size_t(5), Reflect<ReflectObj>::Members::template member<5>.index);
    EXPECT_EQ(size_t(6), Reflect<ReflectObj>::Members::template member<6>.index);
    EXPECT_EQ(size_t(7), Reflect<ReflectObj>::Members::template member<7>.index);
    EXPECT_EQ(size_t(8), Reflect<ReflectObj>::Members::template member<8>.index);
    EXPECT_EQ(size_t(9), Reflect<ReflectObj>::Members::template member<9>.index);
    EXPECT_EQ(size_t(10), Reflect<ReflectObj>::Members::template member<10>.index);

    constexpr std::string_view primitiveMemberName = "primitive";
    constexpr std::string_view objectMemberName = "object";
    constexpr size_t primitiveIndex = Reflect<ReflectObj>::Members::indexOf(primitiveMemberName);
    constexpr size_t objectIndex = Reflect<ReflectObj>::Members::indexOf(objectMemberName);
    EXPECT_EQ(primitiveIndex, Reflect<ReflectObj>::Members::Member<primitiveIndex>::index);
    EXPECT_EQ(objectIndex, Reflect<ReflectObj>::Members::Member<objectIndex>::index);
}

class ReflectPackTest {
public:
    int memberValue = 1;
    static int staticValue; // = 2
    int & memberValueReference;
    static constexpr int & staticValueReference = staticValue;
    int memberMethod() { return 5; }
    static int staticMethod() { return 6; }

    ReflectPackTest() : memberValueReference(memberValue) {}

    REFLECT(ReflectPackTest, memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod)
};

int ReflectPackTest::staticValue = 2;

namespace TestFilters {
    template <typename Member, typename = enable_if_member_t<Member>> struct Integers : std::bool_constant<
        std::is_same_v<int, RareTs::remove_cvref_t<typename Member::type>>
    > {};
    template <typename Member, typename = enable_if_member_t<Member>> struct ReflectSubObjs : std::bool_constant<
        std::is_same_v<ReflectSubObj, RareTs::remove_cvref_t<typename Member::type>>
    > {};
}

TEST(ReflectTestMembers, Pack)
{
    bool visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
        using MemberField = decltype(memberValue);
        using StaticField = decltype(staticValue);
        using MemberValueReference = decltype(memberValueReference);
        using StaticValueReference = decltype(staticValueReference);
        using MemberMethod = decltype(memberMethod);
        using StaticMethod = decltype(staticMethod);

        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
        using MemberField = RareTs::remove_cvref_t<decltype(memberValue)>;
        using StaticField = RareTs::remove_cvref_t<decltype(staticValue)>;
        using MemberValueReference = RareTs::remove_cvref_t<decltype(memberValueReference)>;
        using StaticValueReference = RareTs::remove_cvref_t<decltype(staticValueReference)>;
        using MemberMethod = RareTs::remove_cvref_t<decltype(memberMethod)>;
        using StaticMethod = RareTs::remove_cvref_t<decltype(staticMethod)>;
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
        using MemberField = RareTs::remove_cvref_t<decltype(memberValue)>;
        using StaticField = RareTs::remove_cvref_t<decltype(staticValue)>;
        using MemberValueReference = RareTs::remove_cvref_t<decltype(memberValueReference)>;
        using StaticValueReference = RareTs::remove_cvref_t<decltype(staticValueReference)>;
        using MemberMethod = RareTs::remove_cvref_t<decltype(memberMethod)>;
        using StaticMethod = RareTs::remove_cvref_t<decltype(staticMethod)>;

        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
        using MemberField = RareTs::remove_cvref_t<decltype(memberValue)>;
        using StaticField = RareTs::remove_cvref_t<decltype(staticValue)>;
        using MemberValueReference = RareTs::remove_cvref_t<decltype(memberValueReference)>;
        using StaticValueReference = RareTs::remove_cvref_t<decltype(staticValueReference)>;
        using MemberMethod = RareTs::remove_cvref_t<decltype(memberMethod)>;
        using StaticMethod = RareTs::remove_cvref_t<decltype(staticMethod)>;
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
        using MemberField = RareTs::remove_cvref_t<decltype(memberValue)>;
        using StaticField = RareTs::remove_cvref_t<decltype(staticValue)>;
        using MemberValueReference = RareTs::remove_cvref_t<decltype(memberValueReference)>;
        using StaticValueReference = RareTs::remove_cvref_t<decltype(staticValueReference)>;
        using MemberMethod = RareTs::remove_cvref_t<decltype(memberMethod)>;
        using StaticMethod = RareTs::remove_cvref_t<decltype(staticMethod)>;
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberValueReference.index);
        EXPECT_EQ(size_t(3), staticValueReference.index);
        EXPECT_EQ(size_t(4), memberMethod.index);
        EXPECT_EQ(size_t(5), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = RareTs::remove_cvref_t<std::tuple_element_t<0, Ts>>;
        using StaticField = RareTs::remove_cvref_t<std::tuple_element_t<1, Ts>>;
        using MemberValueReference = RareTs::remove_cvref_t<std::tuple_element_t<2, Ts>>;
        using StaticValueReference = RareTs::remove_cvref_t<std::tuple_element_t<3, Ts>>;
        using MemberMethod = RareTs::remove_cvref_t<std::tuple_element_t<4, Ts>>;
        using StaticMethod = RareTs::remove_cvref_t<std::tuple_element_t<5, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberValueReference.index);
        EXPECT_EQ(size_t(3), staticValueReference.index);
        EXPECT_EQ(size_t(4), memberMethod.index);
        EXPECT_EQ(size_t(5), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto && ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = RareTs::remove_cvref_t<std::tuple_element_t<0, Ts>>;
        using StaticField = RareTs::remove_cvref_t<std::tuple_element_t<1, Ts>>;
        using MemberValueReference = RareTs::remove_cvref_t<std::tuple_element_t<2, Ts>>;
        using StaticValueReference = RareTs::remove_cvref_t<std::tuple_element_t<3, Ts>>;
        using MemberMethod = RareTs::remove_cvref_t<std::tuple_element_t<4, Ts>>;
        using StaticMethod = RareTs::remove_cvref_t<std::tuple_element_t<5, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberValueReference.index);
        EXPECT_EQ(size_t(3), staticValueReference.index);
        EXPECT_EQ(size_t(4), memberMethod.index);
        EXPECT_EQ(size_t(5), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](const auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberValueReference.index);
        EXPECT_EQ(size_t(3), staticValueReference.index);
        EXPECT_EQ(size_t(4), memberMethod.index);
        EXPECT_EQ(size_t(5), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](const auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = RareTs::remove_cvref_t<std::tuple_element_t<0, Ts>>;
        using StaticField = RareTs::remove_cvref_t<std::tuple_element_t<1, Ts>>;
        using MemberValueReference = RareTs::remove_cvref_t<std::tuple_element_t<2, Ts>>;
        using StaticValueReference = RareTs::remove_cvref_t<std::tuple_element_t<3, Ts>>;
        using MemberMethod = RareTs::remove_cvref_t<std::tuple_element_t<4, Ts>>;
        using StaticMethod = RareTs::remove_cvref_t<std::tuple_element_t<5, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberValueReference = std::get<2>(values);
        auto staticValueReference = std::get<3>(values);
        auto memberMethod = std::get<4>(values);
        auto staticMethod = std::get<5>(values);
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 4>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 5>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberValueReference.index);
        EXPECT_EQ(size_t(3), staticValueReference.index);
        EXPECT_EQ(size_t(4), memberMethod.index);
        EXPECT_EQ(size_t(5), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    // Note: members are always lvalues and don't have a need to be passed as rvalue references
}

TEST(ReflectTestMembers, FilteredCount)
{
    size_t filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::None>();
    EXPECT_EQ(size_t(11), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsData>();
    EXPECT_EQ(size_t(9), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsFunction>();
    EXPECT_EQ(size_t(2), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsInstanceMember>();
    EXPECT_EQ(size_t(8), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsStaticMember>();
    EXPECT_EQ(size_t(3), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsInstanceData>();
    EXPECT_EQ(size_t(7), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsInstanceFunction>();
    EXPECT_EQ(size_t(1), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsStaticData>();
    EXPECT_EQ(size_t(2), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsStaticFunction>();
    EXPECT_EQ(size_t(1), filteredCount);
}

TEST(ReflectTestMembers, IndexOfStr)
{
    constexpr size_t npos = std::numeric_limits<size_t>::max();
    EXPECT_EQ(npos, Reflect<EmptyObj>::Members::indexOf(""));
    EXPECT_EQ(npos, Reflect<EmptyObj>::Members::indexOf("asdf"));
    
    EXPECT_EQ(npos, Reflect<ReflectObj>::Members::indexOf(""));
    EXPECT_EQ(npos, Reflect<ReflectObj>::Members::indexOf("unknownField"));
    EXPECT_EQ(size_t(0), Reflect<ReflectObj>::Members::indexOf("primitive"));
    EXPECT_EQ(size_t(1), Reflect<ReflectObj>::Members::indexOf("object"));
    EXPECT_EQ(size_t(2), Reflect<ReflectObj>::Members::indexOf("primitiveArray"));
    EXPECT_EQ(size_t(3), Reflect<ReflectObj>::Members::indexOf("map"));
    EXPECT_EQ(size_t(4), Reflect<ReflectObj>::Members::indexOf("objCollection"));
    EXPECT_EQ(size_t(5), Reflect<ReflectObj>::Members::indexOf("stack"));
    EXPECT_EQ(size_t(6), Reflect<ReflectObj>::Members::indexOf("staticPrimitive"));
    EXPECT_EQ(size_t(7), Reflect<ReflectObj>::Members::indexOf("primitiveReference"));
    EXPECT_EQ(size_t(8), Reflect<ReflectObj>::Members::indexOf("staticPrimitiveReference"));
    EXPECT_EQ(size_t(9), Reflect<ReflectObj>::Members::indexOf("memberMethod"));
    EXPECT_EQ(size_t(10), Reflect<ReflectObj>::Members::indexOf("staticMethod"));

    EXPECT_EQ(npos, Reflect<PrivateIndexOf>::Members::indexOf(""));
    EXPECT_EQ(npos, Reflect<PrivateIndexOf>::Members::indexOf("asdf"));
    EXPECT_EQ(size_t(0), Reflect<PrivateIndexOf>::Members::indexOf("primitive"));
    EXPECT_EQ(size_t(1), Reflect<PrivateIndexOf>::Members::indexOf("object"));
    EXPECT_EQ(size_t(2), Reflect<PrivateIndexOf>::Members::indexOf("primitiveArray"));
    EXPECT_EQ(size_t(3), Reflect<PrivateIndexOf>::Members::indexOf("map"));
    EXPECT_EQ(size_t(4), Reflect<PrivateIndexOf>::Members::indexOf("objCollection"));
    EXPECT_EQ(size_t(5), Reflect<PrivateIndexOf>::Members::indexOf("stack"));
    EXPECT_EQ(size_t(6), Reflect<PrivateIndexOf>::Members::indexOf("staticPrimitive"));
    EXPECT_EQ(size_t(7), Reflect<PrivateIndexOf>::Members::indexOf("primitiveReference"));
    EXPECT_EQ(size_t(8), Reflect<PrivateIndexOf>::Members::indexOf("staticPrimitiveReference"));
    EXPECT_EQ(size_t(9), Reflect<PrivateIndexOf>::Members::indexOf("memberMethod"));
    EXPECT_EQ(size_t(10), Reflect<PrivateIndexOf>::Members::indexOf("staticMethod"));
}

TEST(ReflectTestMembers, ForEachMember)
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
    Reflect<ReflectObj>::Members::forEach([&](auto & member) {
        
        using Member = std::remove_reference_t<decltype(member)>;

        EXPECT_EQ(index, Member::index);
        if constexpr ( Member::isStatic && Member::index == size_t(6) )
        {
            EXPECT_EQ(ReflectObj::staticPrimitive, *member.pointer);
            visitCount++;
        }
        else if constexpr ( Member::index == size_t(9) )
        {
            int result = (reflectObj.*member.pointer)();
            EXPECT_EQ(1, result);
            visitCount++;
        }
        else if constexpr ( Member::index == size_t(10) )
        {
            int result = member.pointer();
            EXPECT_EQ(2, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(3, visitCount);
    EXPECT_EQ(size_t(11), index);
}

TEST(ReflectTestMembers, ForEachMemberFiltered)
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
    Reflect<ReflectObj>::Members::forEach<Filter::IsInstanceMember>([&](auto & member) {
        
        using Member = std::remove_reference_t<decltype(member)>;

        if constexpr ( Member::index == 9 )
        {
            int result = (reflectObj.*member.pointer)();
            EXPECT_EQ(1, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(1, visitCount);
    EXPECT_EQ(size_t(8), index);

    index = 0;
    visitCount = 0;
    Reflect<ReflectObj>::Members::forEach<Filter::IsStaticMember>([&](auto & member) {
        
        using Member = std::remove_reference_t<decltype(member)>;

        if constexpr ( Member::isStatic && Member::index == 6 )
        {
            EXPECT_EQ(ReflectObj::staticPrimitive, *member.pointer);
            visitCount++;
        }
        else if constexpr ( Member::index == 10 )
        {
            int result = member.pointer();
            EXPECT_EQ(2, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(2, visitCount);
    EXPECT_EQ(size_t(3), index);
}

TEST(ReflectTestMembers, ForEachStatic)
{
    size_t index = 0;
    Reflect<ReflectObj>::Members::forEach([&](auto & member, auto & value) {
        
        using Member = std::remove_reference_t<decltype(member)>;
        using Value = std::remove_reference_t<decltype(value)>;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected_v<Value> && Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( Member::isStatic && Member::isFunction ) {
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
    EXPECT_EQ(size_t(3), index);
}

TEST(ReflectTestMembers, ForEachStaticFiltered)
{
    size_t index = 0;
    Reflect<ReflectObj>::Members::forEach<Filter::IsStaticData>([&](auto & member, auto & value) {
        
        using Member = std::remove_reference_t<decltype(member)>;
        using Value = std::remove_reference_t<decltype(value)>;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected_v<Value> && Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(ReflectObj::staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && Member::isStatic && !Member::isFunction )
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
    EXPECT_EQ(size_t(2), index);

    index = 0;
    Reflect<ReflectObj>::Members::forEach<Filter::IsStaticFunction>([&](auto & member, auto & value) {
        
        using Member = std::remove_reference_t<decltype(member)>;

        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( Member::isStatic && Member::isFunction ) {
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
    EXPECT_EQ(size_t(1), index);
}

TEST(ReflectTestMembers, ForEachInstanced)
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
    Reflect<ReflectObj>::Members::forEach(reflectObj, [&](auto & member, auto & value) {
        
        using Member = std::remove_reference_t<decltype(member)>;
        using Value = std::remove_reference_t<decltype(value)>;

        EXPECT_EQ(index, Member::index);
        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic && !Member::isFunction ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic ) {
                    Reflect<Value>::Values::at(0, value, [&](auto & value) {
                        EXPECT_EQ(reflectObj.object.val, value);
                        visited = true;
                    });
                    EXPECT_TRUE(visited);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( !is_reflected_v<Value> && is_static_array_v<Value> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( !is_reflected_v<Value> && is_iterable_v<Value> && is_pair_v<element_type_t<Value>> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> && !Member::isStatic )
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
                if constexpr ( !is_reflected_v<Value> && Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 9:
                if constexpr ( !Member::isStatic && Member::isFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 10:
                if constexpr ( Member::isStatic && Member::isFunction ) {
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
    EXPECT_EQ(size_t(11), index);
}

TEST(ReflectTestMembers, ForEachInstancedFiltered)
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
    Reflect<ReflectObj>::Members::forEach<Filter::None>(reflectObj, [&](auto & member, auto & value) {
        
        using Member = std::remove_reference_t<decltype(member)>;
        using Value = std::remove_reference_t<decltype(value)>;

        EXPECT_EQ(index, Member::index);
        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic && !Member::isFunction ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic ) {
                    Reflect<Value>::Values::at(0, value, [&](auto & value) {
                        EXPECT_EQ(reflectObj.object.val, value);
                        visited = true;
                    });
                    EXPECT_TRUE(visited);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( !is_reflected_v<Value> && is_static_array_v<Value> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( !is_reflected_v<Value> && is_iterable_v<Value> && is_pair_v<element_type_t<Value>> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> && !Member::isStatic )
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
                if constexpr ( !is_reflected_v<Value> && Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 9:
                if constexpr ( !Member::isStatic && Member::isFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 10:
                if constexpr ( Member::isStatic && Member::isFunction ) {
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
    EXPECT_EQ(size_t(11), index);

    index = 0;
    Reflect<ReflectObj>::Members::forEach<Filter::IsData>(reflectObj, [&](auto & member, auto & value) {
        
        using Member = std::remove_reference_t<decltype(member)>;
        using Value = std::remove_reference_t<decltype(value)>;

        EXPECT_EQ(index, Member::index);
        bool visited = false;
        switch ( index ) {
            case 0:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic && !Member::isFunction ) {
                    EXPECT_EQ(reflectObj.primitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                if constexpr ( is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic ) {
                    Reflect<Value>::Values::at(0, value, [&](auto & value) {
                        EXPECT_EQ(reflectObj.object.val, value);
                        visited = true;
                    });
                    EXPECT_TRUE(visited);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( !is_reflected_v<Value> && is_static_array_v<Value> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( !is_reflected_v<Value> && is_iterable_v<Value> && is_pair_v<element_type_t<Value>> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> && !Member::isStatic ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> && !Member::isStatic )
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
                if constexpr ( !is_reflected_v<Value> && Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && !Member::isStatic && !Member::isFunction )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && Member::isStatic && !Member::isFunction )
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
    EXPECT_EQ(size_t(9), index);

    index = 0;
    Reflect<ReflectObj>::Members::forEach<Filter::IsFunction>(reflectObj, [&](auto & member, auto & value) {
        
        using Member = std::remove_reference_t<decltype(member)>;

        bool visited = false;
        switch ( index ) {
            case 0:
                EXPECT_EQ(size_t(9), Member::index);
                if constexpr ( !Member::isStatic && Member::isFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                EXPECT_EQ(size_t(10), Member::index);
                if constexpr ( Member::isStatic && Member::isFunction ) {
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
    EXPECT_EQ(size_t(2), index);
}

TEST(ReflectTestMembers, MemberAt)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::at(0, [&](auto & member) {
        EXPECT_EQ(size_t(0), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(1, [&](auto & member) {
        EXPECT_EQ(size_t(1), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(2, [&](auto & member) {
        EXPECT_EQ(size_t(2), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(3, [&](auto & member) {
        EXPECT_EQ(size_t(3), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(4, [&](auto & member) {
        EXPECT_EQ(size_t(4), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(5, [&](auto & member) {
        EXPECT_EQ(size_t(5), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(6, [&](auto & member) {
        EXPECT_EQ(size_t(6), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(7, [&](auto & member) {
        EXPECT_EQ(size_t(7), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(8, [&](auto & member) {
        EXPECT_EQ(size_t(8), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(9, [&](auto & member) {
        EXPECT_EQ(size_t(9), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at(10, [&](auto & member) {
        EXPECT_EQ(size_t(10), member.index);
        visitCount++;
    });
    EXPECT_EQ(size_t(11), visitCount);
}

TEST(ReflectTestMembers, MemberAtFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::at<std::is_same, int>(0, [&](auto & member) {
        EXPECT_EQ(size_t(0), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at<TestFilters::ReflectSubObjs>(1, [&](auto & member) {
        EXPECT_EQ(size_t(1), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at<TestFilters::ReflectSubObjs>(2, [&](auto &) {
        EXPECT_TRUE(false); // Should not be visited since member at index 2 is an int array, not ReflectSubObj
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(ReflectTestMembers, AtStatic)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::at(0, [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::at(1, [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::at(2, [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::at(3, [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::at(4, [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::at(5, [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::at(6, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(6), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(7, [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::at(8, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(9, [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::at(10, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(10), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(3), visitCount);
}

TEST(ReflectTestMembers, AtStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::at<TestFilters::Integers>(6, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(6), member.index);
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>>  ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at<TestFilters::Integers>(8, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>> ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at<TestFilters::ReflectSubObjs>(6, [&](auto &, auto &) {
        EXPECT_TRUE(false); // Should not be visited since member at index 6 is an int, not ReflectSubObj
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(ReflectTestMembers, AtInstanced)
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
    Reflect<ReflectObj>::Members::at(0, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(0), member.index);
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(1, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(1), member.index);
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(2, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(2), member.index);
        if constexpr ( is_static_array_v<std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
            EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(3, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(3), member.index);
        if constexpr ( is_pair_v<element_type_t<std::remove_reference_t<decltype(value)>>> ) {
            EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
            EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(4, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(4), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> ) {
            EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
            EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(5, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(5), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> ) {
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.pop();
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.push(3);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(6, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(6), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(reflectObj.staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(7, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(7), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.primitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(8, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(9, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(9), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(10, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(10), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(11), visitCount);
}

TEST(ReflectTestMembers, AtInstancedFiltered)
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
    Reflect<ReflectObj>::Members::at<TestFilters::Integers>(0, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(0), member.index);
        EXPECT_EQ(reflectObj.primitive, value);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at<TestFilters::ReflectSubObjs>(1, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(1), member.index);
        EXPECT_EQ(reflectObj.object.val, value.val);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::at<TestFilters::ReflectSubObjs>(2, reflectObj, [&](auto &, auto &) {
        EXPECT_TRUE(false); // Should not be visited since member at index 2 is an int array, not ReflectSubObj
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(ReflectTestMembers, NamedMember)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::named("", [&](auto &) { EXPECT_TRUE(false); }); // Not a member name/should not be visited
    Reflect<ReflectObj>::Members::named("unknown", [&](auto &) { EXPECT_TRUE(false); }); // Not a member name/should not be visited
    Reflect<ReflectObj>::Members::named("PrimitivE", [&](auto &) { EXPECT_TRUE(false); }); // Not a member name/should not be visited
    Reflect<ReflectObj>::Members::named("primitive", [&](auto & member) {
        EXPECT_EQ(size_t(0), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("object", [&](auto & member) {
        EXPECT_EQ(size_t(1), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("primitiveArray", [&](auto & member) {
        EXPECT_EQ(size_t(2), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("map", [&](auto & member) {
        EXPECT_EQ(size_t(3), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("objCollection", [&](auto & member) {
        EXPECT_EQ(size_t(4), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("stack", [&](auto & member) {
        EXPECT_EQ(size_t(5), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("staticPrimitive", [&](auto & member) {
        EXPECT_EQ(size_t(6), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("primitiveReference", [&](auto & member) {
        EXPECT_EQ(size_t(7), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("staticPrimitiveReference", [&](auto & member) {
        EXPECT_EQ(size_t(8), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("memberMethod", [&](auto & member) {
        EXPECT_EQ(size_t(9), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("staticMethod", [&](auto & member) {
        EXPECT_EQ(size_t(10), member.index);
        visitCount++;
    });
    EXPECT_EQ(size_t(11), visitCount);
}

TEST(ReflectTestMembers, NamedMemberFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::named<Filter::IsData>("", [&](auto &) { EXPECT_TRUE(false); }); // Not a member name/should not be visited
    Reflect<ReflectObj>::Members::named<std::is_same, float>("primitive", [&](auto &) { EXPECT_TRUE(false); }); // Not a float/should not be visited
    Reflect<ReflectObj>::Members::named<std::is_same, int>("primitive", [&](auto & member) {
        EXPECT_EQ(size_t(0), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<TestFilters::ReflectSubObjs>("object", [&](auto & member) {
        EXPECT_EQ(size_t(1), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<Filter::IsData>("primitiveArray", [&](auto & member) {
        EXPECT_EQ(size_t(2), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<Filter::Is<std::map>::Specialization>("map", [&](auto & member) {
        EXPECT_EQ(size_t(3), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<Filter::Is<std::vector>::Specialization>("objCollection", [&](auto & member) {
        EXPECT_EQ(size_t(4), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<Filter::IsInstanceData>("stack", [&](auto & member) {
        EXPECT_EQ(size_t(5), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<TestFilters::Integers>("staticPrimitive", [&](auto & member) {
        EXPECT_EQ(size_t(6), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<std::is_same, int &>("primitiveReference", [&](auto & member) {
        EXPECT_EQ(size_t(7), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<Filter::IsStaticMember>("staticPrimitiveReference", [&](auto & member) {
        EXPECT_EQ(size_t(8), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<Filter::IsInstanceFunction>("memberMethod", [&](auto & member) {
        EXPECT_EQ(size_t(9), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<Filter::IsStaticFunction>("staticMethod", [&](auto & member) {
        EXPECT_EQ(size_t(10), member.index);
        visitCount++;
    });
    EXPECT_EQ(size_t(11), visitCount);
}

TEST(ReflectTestMembers, NamedStatic)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::named("primitive", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("object", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("primitiveArray", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("map", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("objCollection", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("stack", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("staticPrimitive", [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(6), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("primitiveReference", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("staticPrimitiveReference", [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("memberMethod", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("staticMethod", [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(10), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(3), visitCount);
}

TEST(ReflectTestMembers, NamedStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::named<TestFilters::Integers>("primitive", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::None>("object", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::None>("primitiveArray", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::None>("map", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::None>("objCollection", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::None>("stack", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<std::is_same, float>("staticPrimitive", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-float/should not be visited
    Reflect<ReflectObj>::Members::named<std::is_same, int>("staticPrimitive", [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(6), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named<Filter::None>("primitiveReference", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<std::is_same, float &>("staticPrimitiveReference", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-float/should not be visited
    Reflect<ReflectObj>::Members::named<std::is_same, int &>("staticPrimitiveReference", [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named<Filter::None>("memberMethod", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::IsInstanceFunction>("staticMethod", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-instance/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::IsStaticFunction>("staticMethod", [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(10), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(3), visitCount);
}

TEST(ReflectTestMembers, NamedInstanced)
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
    Reflect<ReflectObj>::Members::named("primitive", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(0), member.index);
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("object", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(1), member.index);
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("primitiveArray", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(2), member.index);
        if constexpr ( is_static_array_v<std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
            EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("map", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(3), member.index);
        if constexpr ( is_pair_v<element_type_t<std::remove_reference_t<decltype(value)>>> ) {
            EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
            EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("objCollection", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(4), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> ) {
            EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
            EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("stack", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(5), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> ) {
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.pop();
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.push(3);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("staticPrimitive", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(6), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(reflectObj.staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("primitiveReference", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(7), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.primitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("staticPrimitiveReference", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("memberMethod", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(9), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("staticMethod", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(10), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(11), visitCount);
}

TEST(ReflectTestMembers, NamedInstancedFiltered)
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
    Reflect<ReflectObj>::Members::named<TestFilters::Integers>("primitive", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(0), member.index);
        EXPECT_EQ(reflectObj.primitive, value);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<TestFilters::ReflectSubObjs>("object", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(1), member.index);
        EXPECT_EQ(reflectObj.object.val, value.val);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<TestFilters::ReflectSubObjs>("primitiveArray", reflectObj, [&](auto &, auto &) {
        EXPECT_TRUE(false); // Should as this is an array, not ReflectSubObj
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(ReflectTestValues, Pack)
{
    ReflectPackTest obj {};

    bool visited = false;
    Reflect<ReflectPackTest>::Values::pack([&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
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
        isSame = std::is_same_v<std::nullptr_t, decltype(memberValueReference)>;
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
    Reflect<ReflectPackTest>::Values::pack([&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
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
        isSame = std::is_same_v<std::nullptr_t &, decltype(memberValueReference)>;
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
    Reflect<ReflectPackTest>::Values::pack([&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
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
        isSame = std::is_same_v<std::nullptr_t &, decltype(memberValueReference)>;
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
    Reflect<ReflectPackTest>::Values::pack([&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
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
        isSame = std::is_same_v<const std::nullptr_t, decltype(memberValueReference)>;
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
    Reflect<ReflectPackTest>::Values::pack([&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
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
        isSame = std::is_same_v<const std::nullptr_t &, decltype(memberValueReference)>;
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
    Reflect<ReflectPackTest>::Values::pack([&](auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
        
        bool isSame = std::is_same_v<decltype(&ReflectPackTest::memberValue), MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::nullptr_t, MemberValueReferenceType>;
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
    Reflect<ReflectPackTest>::Values::pack([&](auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
        isSame = std::is_same_v<std::nullptr_t &, MemberValueReferenceType>;
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
    Reflect<ReflectPackTest>::Values::pack([&](auto && ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
        isSame = std::is_same_v<std::nullptr_t &, MemberValueReferenceType>;
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
    Reflect<ReflectPackTest>::Values::pack([&](const auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
        isSame = std::is_same_v<const std::nullptr_t, MemberValueReferenceType>;
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
    Reflect<ReflectPackTest>::Values::pack([&](const auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
        isSame = std::is_same_v<const std::nullptr_t &, MemberValueReferenceType>;
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto && ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](const auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
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
    Reflect<ReflectPackTest>::Values::pack(obj, [&](const auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(6), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = RareTs::remove_cvref_t<std::tuple_element_t<0, Ts>>;
        using StaticType = RareTs::remove_cvref_t<std::tuple_element_t<1, Ts>>;
        using MemberValueReferenceType = RareTs::remove_cvref_t<std::tuple_element_t<2, Ts>>;
        using StaticValueReferenceType = RareTs::remove_cvref_t<std::tuple_element_t<3, Ts>>;
        using MemberMethodType = RareTs::remove_cvref_t<std::tuple_element_t<4, Ts>>;
        using StaticMethodType = RareTs::remove_cvref_t<std::tuple_element_t<5, Ts>>;
        
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
    Reflect<ReflectObj>::Values::forEach([&](auto & value) {
        
        using Value = std::remove_reference_t<decltype(value)>;

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
    EXPECT_EQ(size_t(3), index);
}

TEST(ReflectTestValues, ForEachStaticFiltered)
{
    size_t index = 0;
    Reflect<ReflectObj>::Values::forEach<Filter::IsStaticData>([&](auto & value) {
        
        using Value = std::remove_reference_t<decltype(value)>;

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
    EXPECT_EQ(size_t(2), index);

    index = 0;
    Reflect<ReflectObj>::Values::forEach<Filter::IsStaticFunction>([&](auto & value) {
        
        using Value = std::remove_reference_t<decltype(value)>;

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
    EXPECT_EQ(size_t(1), index);
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
    Reflect<ReflectObj>::Values::forEach(reflectObj, [&](auto & value) {
        
        using Value = std::remove_reference_t<decltype(value)>;

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
                    Reflect<Value>::Values::at(0, value, [&](auto & value) {
                        EXPECT_EQ(reflectObj.object.val, value);
                        visited = true;
                    });
                    EXPECT_TRUE(visited);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( is_static_array_v<Value> ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( is_pair_v<element_type_t<Value>> ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> ) {
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.pop();
                    EXPECT_EQ(reflectObj.stack.top(), value.top());
                    reflectObj.stack.push(3);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 6:
                if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
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
    EXPECT_EQ(size_t(11), index);
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
    Reflect<ReflectObj>::Values::forEach<Filter::IsData>(reflectObj, [&](auto & value) {
        
        using Value = std::remove_reference_t<decltype(value)>;

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
                    Reflect<Value>::Values::at(0, value, [&](auto & value) {
                        EXPECT_EQ(reflectObj.object.val, value);
                        visited = true;
                    });
                    EXPECT_TRUE(visited);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 2:
                if constexpr ( is_static_array_v<Value> ) {
                    EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
                    EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 3:
                if constexpr ( is_pair_v<element_type_t<Value>> ) {
                    EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
                    EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 4:
                if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> ) {
                    EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
                    EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 5:
                if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> )
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
                if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> )
                {
                    EXPECT_EQ(reflectObj.staticPrimitive, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 7:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> )
                {
                    EXPECT_EQ(reflectObj.primitiveReference, value);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
                if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> )
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
    EXPECT_EQ(size_t(9), index);

    index = 0;
    Reflect<ReflectObj>::Values::forEach<Filter::IsFunction>(reflectObj, [&](auto & value) {
        
        using Value = std::remove_reference_t<decltype(value)>;

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
    EXPECT_EQ(size_t(2), index);
}

TEST(ReflectTestValues, AtStatic)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::at(0, [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::at(1, [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::at(2, [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::at(3, [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::at(4, [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::at(5, [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::at(6, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(7, [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::at(8, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(9, [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::at(10, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(3), visitCount);
}

TEST(ReflectTestValues, AtStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::at<std::is_same, int>(6, [&](auto & value) {
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>>  ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at<std::is_same, int &>(8, [&](auto & value) {
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>> ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at<TestFilters::ReflectSubObjs>(10, [&](auto &) {
        // Should not be visited as "staticMethod" is a function pointer, not a ReflectedSubObj
    });
    EXPECT_EQ(size_t(2), visitCount);
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
    Reflect<ReflectObj>::Values::at(0, reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(1, reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(2, reflectObj, [&](auto & value) {
        if constexpr ( is_static_array_v<std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
            EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(3, reflectObj, [&](auto & value) {
        if constexpr ( is_pair_v<element_type_t<std::remove_reference_t<decltype(value)>>> ) {
            EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
            EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(4, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> ) {
            EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
            EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(5, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> ) {
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.pop();
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.push(3);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(6, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(reflectObj.staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(7, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.primitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(8, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(9, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(10, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(11), visitCount);
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
    Reflect<ReflectObj>::Values::at<std::is_same, int>(0, reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at<TestFilters::ReflectSubObjs>(1, reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at<TestFilters::ReflectSubObjs>(2, reflectObj, [&](auto &) {
        EXPECT_TRUE(false); // Should not be visited since member at index 2 is an int array, not ReflectSubObj
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(ReflectTestValues, NamedStatic)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::named("primitive", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("object", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("primitiveArray", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("map", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("objCollection", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("stack", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("staticPrimitive", [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(ReflectObj::staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("primitiveReference", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("staticPrimitiveReference", [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(ReflectObj::staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("memberMethod", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("staticMethod", [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(3), visitCount);
}

TEST(ReflectTestValues, NamedStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::named<std::is_same, int>("staticPrimitive", [&](auto & value) {
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>> ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named<std::is_same, int &>("staticPrimitiveReference", [&](auto & value) {
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>> ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named<TestFilters::ReflectSubObjs>("staticMethod", [&](auto &) {
        // Should not be visited as "staticMethod" is a function pointer, not a ReflectedSubObj
    });
    EXPECT_EQ(size_t(2), visitCount);
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
    Reflect<ReflectObj>::Values::named("primitive", reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("object", reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("primitiveArray", reflectObj, [&](auto & value) {
        if constexpr ( is_static_array_v<std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitiveArray[0], value[0]);
            EXPECT_EQ(reflectObj.primitiveArray[1], value[1]);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("map", reflectObj, [&](auto & value) {
        if constexpr ( is_pair_v<element_type_t<std::remove_reference_t<decltype(value)>>> ) {
            EXPECT_EQ(reflectObj.map.begin()->first, value.begin()->first);
            EXPECT_EQ((++reflectObj.map.begin())->first, (++value.begin())->first);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("objCollection", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( is_reflected_v<element_type_t<Value>> && is_iterable_v<Value> ) {
            EXPECT_EQ(reflectObj.objCollection[0].val, value[0].val);
            EXPECT_EQ(reflectObj.objCollection[1].val, value[1].val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("stack", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && is_adaptor_v<Value> ) {
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.pop();
            EXPECT_EQ(reflectObj.stack.top(), value.top());
            reflectObj.stack.push(3);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("staticPrimitive", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && std::is_same_v<int, Value> ) {
            EXPECT_EQ(reflectObj.staticPrimitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("primitiveReference", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.primitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("staticPrimitiveReference", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( !is_reflected_v<Value> && !is_iterable_v<Value> && std::is_same_v<int, std::remove_reference_t<Value>> ) {
            EXPECT_EQ(reflectObj.staticPrimitiveReference, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("memberMethod", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named("staticMethod", reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(11), visitCount);
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
    Reflect<ReflectObj>::Values::named<std::is_same, int>("primitive", reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<int, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.primitive, value);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named<TestFilters::ReflectSubObjs>("object", reflectObj, [&](auto & value) {
        if constexpr ( std::is_same_v<ReflectSubObj, std::remove_reference_t<decltype(value)>> ) {
            EXPECT_EQ(reflectObj.object.val, value.val);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named<TestFilters::ReflectSubObjs>("primitiveArray", reflectObj, [&](auto &) {
        EXPECT_TRUE(false); // Should not be visited since member at index 2 is an int array, not ReflectSubObj
    });
    EXPECT_EQ(size_t(2), visitCount);
}

class Unnoted
{
    int a;
    inline auto getA() { return a; }

    REFLECT(Unnoted, a)
};

NOTE(EmptyNotes)
class EmptyNotes
{
    int a;
    inline auto getA() { return a; }

    REFLECT_NOTED(EmptyNotes, a)
};

NOTE(SinglyNoted, 'a')
class SinglyNoted
{
    int a;
    inline auto getA() { return a; }

    REFLECT_NOTED(SinglyNoted, a)
};

struct SerializedName
{
    std::string_view value;
};

NOTE(StructNoted, SerializedName { "noted" })
class StructNoted
{
    int a;
    inline auto getA() { return a; }

    REFLECT_NOTED(StructNoted, a)
};

NOTE(ComplexNoted, std::tuple{33, SerializedName{"test"}}, 'c', 'd', 'e')
class ComplexNoted
{
    int a;
    inline auto getA() { return a; }

    REFLECT_NOTED(ComplexNoted, a)
};

template <typename T>
struct Specializable { using type = T; };

NOTE(SpecializedNoted, 'a', Specializable<int>{})
class SpecializedNoted
{
    int a;
    inline auto getA() { return a; }

    REFLECT_NOTED(SpecializedNoted, a)
};

TEST(ReflectTestNotes, ForEach)
{
    int visitCount = 0;;
    Reflect<Unnoted>::Notes::forEachNote([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<EmptyNotes>::Notes::forEachNote([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<SinglyNoted>::Notes::forEachNote([&](auto & annotation) {
        using Annotation = RareTs::remove_cvref_t<decltype(annotation)>;
        bool isSame = std::is_same_v<char, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('a', annotation);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<StructNoted>::Notes::forEachNote([&](auto & annotation) {
        using Annotation = RareTs::remove_cvref_t<decltype(annotation)>;
        bool isSame = std::is_same_v<SerializedName, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_STREQ("noted", std::string(annotation.value).c_str());
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<ComplexNoted>::Notes::forEachNote([&](auto & annotation) {
        using Annotation = RareTs::remove_cvref_t<decltype(annotation)>;
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
    Reflect<Unnoted>::Notes::forEachNote<int>([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<EmptyNotes>::Notes::forEachNote<int>([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<SinglyNoted>::Notes::forEachNote<char>([&](auto & annotation) {
        using Annotation = RareTs::remove_cvref_t<decltype(annotation)>;
        bool isSame = std::is_same_v<char, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('a', annotation);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<StructNoted>::Notes::forEachNote<SerializedName>([&](auto & annotation) {
        using Annotation = RareTs::remove_cvref_t<decltype(annotation)>;
        bool isSame = std::is_same_v<SerializedName, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_STREQ("noted", std::string(annotation.value).c_str());
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<ComplexNoted>::Notes::forEachNote<char>([&](auto & annotation) {
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
    Reflect<ComplexNoted>::Notes::forEachNote<std::remove_const_t<std::tuple_element_t<0, decltype(ComplexNoted_note)>>>([&](auto &) {
        EXPECT_EQ(0, visitCount);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
}

TEST(ReflectTestNotes, Has)
{
    bool hasNote = Reflect<Unnoted>::Notes::template hasNote<int>();
    EXPECT_FALSE(hasNote);

    hasNote = Reflect<EmptyNotes>::Notes::template hasNote<int>();
    EXPECT_FALSE(hasNote);
    
    hasNote = Reflect<SinglyNoted>::Notes::template hasNote<int>();
    EXPECT_FALSE(hasNote);
    hasNote = Reflect<SinglyNoted>::Notes::template hasNote<char>();
    EXPECT_TRUE(hasNote);
    
    hasNote = Reflect<StructNoted>::Notes::template hasNote<int>();
    EXPECT_FALSE(hasNote);
    hasNote = Reflect<StructNoted>::Notes::template hasNote<SerializedName>();
    EXPECT_TRUE(hasNote);
    
    hasNote = Reflect<ComplexNoted>::Notes::template hasNote<int>();
    EXPECT_FALSE(hasNote);
    hasNote = Reflect<ComplexNoted>::Notes::template hasNote<char>();
    EXPECT_TRUE(hasNote);
}

TEST(ReflectTestNotes, HasSpecialization)
{
    bool hasSpecialization = Reflect<Unnoted>::Notes::template hasNote<Specializable>();
    EXPECT_FALSE(hasSpecialization);

    hasSpecialization = Reflect<EmptyNotes>::Notes::template hasNote<Specializable>();
    EXPECT_FALSE(hasSpecialization);
    
    hasSpecialization = Reflect<SinglyNoted>::Notes::template hasNote<Specializable>();
    EXPECT_FALSE(hasSpecialization);
    
    hasSpecialization = Reflect<StructNoted>::Notes::template hasNote<Specializable>();
    EXPECT_FALSE(hasSpecialization);
    
    hasSpecialization = Reflect<ComplexNoted>::Notes::template hasNote<Specializable>();
    EXPECT_FALSE(hasSpecialization);

    hasSpecialization = Reflect<SpecializedNoted>::Notes::template hasNote<Specializable>();
    EXPECT_TRUE(hasSpecialization);
}

TEST(ReflectTestNotes, Get)
{
    auto singlyNotedChar = Reflect<SinglyNoted>::Notes::getNote<char>();
    bool isSame = std::is_same_v<char, decltype(singlyNotedChar)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', singlyNotedChar);

    constexpr auto & structNotedSerializedName = Reflect<StructNoted>::Notes::getNote<SerializedName>();
    isSame = std::is_same_v<SerializedName, RareTs::remove_cvref_t<decltype(structNotedSerializedName)>>;
    EXPECT_TRUE(isSame);
    EXPECT_STREQ("noted", std::string(structNotedSerializedName.value).c_str());

    using NestedTuple = std::remove_const_t<std::tuple_element_t<0, decltype(ComplexNoted_note)>>;
    constexpr auto & complexNotedTuple = Reflect<ComplexNoted>::Notes::getNote<NestedTuple>();
    isSame = std::is_same_v<NestedTuple, RareTs::remove_cvref_t<decltype(complexNotedTuple)>>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectTestNotes, GetSpecialization)
{
    int visitCount = 0;;
    Reflect<Unnoted>::Notes::forEachNote<int>([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<EmptyNotes>::Notes::forEachNote<int>([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    Reflect<SinglyNoted>::Notes::forEachNote<char>([&](auto & annotation) {
        using Annotation = RareTs::remove_cvref_t<decltype(annotation)>;
        bool isSame = std::is_same_v<char, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('a', annotation);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<StructNoted>::Notes::forEachNote<SerializedName>([&](auto & annotation) {
        using Annotation = RareTs::remove_cvref_t<decltype(annotation)>;
        bool isSame = std::is_same_v<SerializedName, Annotation>;
        EXPECT_TRUE(isSame);
        EXPECT_STREQ("noted", std::string(annotation.value).c_str());
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Reflect<ComplexNoted>::Notes::forEachNote<char>([&](auto & annotation) {
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
    Reflect<ComplexNoted>::Notes::forEachNote<std::remove_const_t<std::tuple_element_t<0, decltype(ComplexNoted_note)>>>([&](auto &) {
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
    static constexpr int & staticPrimitiveReference = staticPrimitive;

    REFLECT(ReflectReferences, primitive, primitiveReference, staticPrimitive, staticPrimitiveReference)
};

int ReflectReferences::staticPrimitive = 33;

TEST(ReflectTest, RfMacroReflectReferences)
{    
    ReflectReferences reflectReferences;
    EXPECT_EQ(11, reflectReferences.primitive);
    EXPECT_EQ(11, reflectReferences.primitiveReference);
    EXPECT_EQ(33, ReflectReferences::staticPrimitive);
    EXPECT_EQ(33, ReflectReferences::staticPrimitiveReference);

    bool visited = false;
    Reflect<ReflectReferences>::Values::at(1, reflectReferences, [&](auto & value) {
        EXPECT_EQ(11, value);
        bool isEqual = std::is_same_v<decltype(value), decltype(reflectReferences.primitiveReference)>;
        EXPECT_TRUE(isEqual);
        value = 22;
        EXPECT_EQ(22, reflectReferences.primitive);
        EXPECT_EQ(22, reflectReferences.primitiveReference);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectReferences>::Values::at(3, reflectReferences, [&](auto & value) {
        EXPECT_EQ(33, value);
        bool isEqual = std::is_same_v<decltype(value), decltype(ReflectReferences::staticPrimitiveReference)>;
        EXPECT_TRUE(isEqual);
        value = 44;
        EXPECT_EQ(44, reflectReferences.staticPrimitive);
        EXPECT_EQ(44, reflectReferences.staticPrimitiveReference);
        visited = true;
    });
    EXPECT_TRUE(visited);
}

class ReflectOffsets
{
    uint16_t a = 1;
    int b = 2;
    float c = 3.0f;
    double d = 4.0;
    char e = '5';
    size_t f = 6;

    REFLECT(ReflectOffsets, a, b, c, d, e, f)

    friend struct TestOffsetsFriend;
};

struct TestOffsetsFriend
{
    inline static size_t aOffset = offsetof(ReflectOffsets, a);
    inline static size_t bOffset = offsetof(ReflectOffsets, b);
    inline static size_t cOffset = offsetof(ReflectOffsets, c);
    inline static size_t dOffset = offsetof(ReflectOffsets, d);
    inline static size_t eOffset = offsetof(ReflectOffsets, e);
    inline static size_t fOffset = offsetof(ReflectOffsets, f);
};

TEST(ReflectionTest, RfMacroReflectOffsets)
{
    constexpr bool isStandardLayout = std::is_standard_layout_v<ReflectOffsets>;
    EXPECT_TRUE(isStandardLayout);

    ReflectOffsets reflectOffsets {};

    Reflect<ReflectOffsets>::Members::forEach(reflectOffsets, [&](auto & member) {
        using Member = std::remove_reference_t<decltype(member)>;
        constexpr bool fieldHasOffset = Member::hasOffset;
        EXPECT_TRUE(fieldHasOffset);

        size_t fieldOffset = std::numeric_limits<size_t>::max();
        switch ( member.index )
        {
            case 0: fieldOffset = TestOffsetsFriend::aOffset; break;
            case 1: fieldOffset = TestOffsetsFriend::bOffset; break;
            case 2: fieldOffset = TestOffsetsFriend::cOffset; break;
            case 3: fieldOffset = TestOffsetsFriend::dOffset; break;
            case 4: fieldOffset = TestOffsetsFriend::eOffset; break;
            case 5: fieldOffset = TestOffsetsFriend::fOffset; break;
        }
        EXPECT_EQ(fieldOffset, member.getOffset());
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
    EXPECT_EQ(0, Class::class_t<Instantiation>::I_::N_);

    char a = std::get<0>(Class::class_t<Instantiation>::notes);
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
    EXPECT_EQ(2, Class::class_t<Instantiation>::I_::N_);

    int total = 0;
    Instantiation instance {};
    Reflect<Instantiation>::Values::forEach(instance, [&](auto & value) {
        total += value;
    });
    EXPECT_EQ(3, total);

    int a = 0;
    Reflect<Instantiation>::Values::at(0, instance, [&](auto & value) {
        a = value;
    });
    EXPECT_EQ(1, a);
    int b = 0;
    Reflect<Instantiation>::Values::at(1, instance, [&](auto & value) {
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
    EXPECT_EQ(2, Class::class_t<Instantiation>::I_::N_);

    char b = std::get<0>(Class::class_t<Instantiation>::notes);
    EXPECT_EQ(b, 'b');

    int total = 0;
    Instantiation instance {};
    Reflect<Instantiation>::Values::forEach(instance, [&](auto & value) {
        total += value;
    });
    EXPECT_EQ(3, total);

    int a = 0;
    Reflect<Instantiation>::Values::at(0, instance, [&](auto & value) {
        a = value;
    });
    EXPECT_EQ(1, a);
    int bVal = 0;
    Reflect<Instantiation>::Values::at(1, instance, [&](auto & value) {
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
    EXPECT_EQ(1, Class::class_t<Instantiation>::I_::N_);

    int total = 0;
    Instantiation instance {};
    Reflect<Instantiation>::Values::forEach(instance, [&](auto & value) {
        total += value;
    });
    EXPECT_EQ(1, total);

    int a = 0;
    Reflect<Instantiation>::Values::at(0, instance, [&](auto & value) {
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
    EXPECT_EQ(1, Class::class_t<Instantiation>::I_::N_);

    int total = 0;
    Instantiation instance { {1, "one"} };
    Reflect<Instantiation>::Values::forEach(instance, [&](auto & value) {
        total += value.first;
    });
    EXPECT_EQ(1, total);

    int l = 0;
    std::string r {};
    Reflect<Instantiation>::Values::at(0, instance, [&](auto & value) {
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
private:
    int b;
    int c;
public:
    auto getSum() { return b+c; }
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

    EXPECT_EQ(1, Class::class_t<PrimaryTemplateType>::I_::N_);
    EXPECT_EQ(2, Class::class_t<PartialSpecializationType>::I_::N_);

    PrimaryTemplateType ptt { 1 };
    PartialSpecializationType pst { 1, 2 };

    int total = 0;
    Reflect<PrimaryTemplateType>::Values::forEach(ptt, [&](auto & value) {
        total += value;
    });
    EXPECT_EQ(1, total);

    int a = 0;
    Reflect<PrimaryTemplateType>::Values::at(0, ptt, [&](auto & value) {
        a = value;
    });
    EXPECT_EQ(1, a);

    total = 0;
    Reflect<PartialSpecializationType>::Values::forEach(pst, [&](auto & value) {
        total += value;
    });
    EXPECT_EQ(3, total);
    
    int b = 0;
    int c = 0;
    Reflect<PartialSpecializationType>::Values::at(0, pst, [&](auto & value) {
        b = value;
    });
    Reflect<PartialSpecializationType>::Values::at(1, pst, [&](auto & value) {
        c = value;
    });
    EXPECT_EQ(1, b);
    EXPECT_EQ(2, c);
}

template <typename T>
struct ReflectSpecializationOnly
{
private:
    int a;
public:
    auto getA() { return a; }
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

    EXPECT_EQ(2, Class::class_t<PartialSpecializationType>::I_::N_);

    PartialSpecializationType pst { 1, 2 };

    int total = 0;
    Reflect<PartialSpecializationType>::Values::forEach(pst, [&](auto & value) {
        total += value;
    });
    EXPECT_EQ(3, total);
    
    int b = 0;
    int c = 0;
    Reflect<PartialSpecializationType>::Values::at(0, pst, [&](auto & value) {
        b = value;
    });
    Reflect<PartialSpecializationType>::Values::at(1, pst, [&](auto & value) {
        c = value;
    });
    EXPECT_EQ(1, b);
    EXPECT_EQ(2, c);
}

struct TestOverloads
{
    NOTE(foo)
    int foo(char) { return 1; }
    int foo(char) const noexcept { return 2; }
    int foo(char) volatile { return 3; }
    int foo(char) const volatile { return 4; }

    int foo(char, int) { return 5; }

    NOTE(refFoo)
    int refFoo(char) & { return 5; }
    int refFoo(char) const & { return 6; }
    int refFoo(char) volatile & { return 7; }
    bool refFoo(char) const volatile & { return false; }
    int refFoo(char) && { return 9; }
    int refFoo(char) const && { return 10; }
    int refFoo(char) volatile && { return 11; }
    int refFoo(char) const volatile && noexcept { return 12; }

    int refFoo(char, int) & { return 1337; }
    
    NOTE(staticFoo, Overload<char>, Overload<char, int>)
    static int staticFoo(char) { return 13; }
    static int staticFoo(char, int) { return 14; }
    
    NOTE(constableFoo, Overload<int>)
    int constableFoo(int) { return 15; }
    int constableFoo(int) const { return 16; }

    int a;
    static int b;
    int & c;
    static int & d;
    int e() { return 1337; }
    static int f() { return 13; }

    TestOverloads() : a(0), c(a) {};

    REFLECT(TestOverloads, foo, refFoo, staticFoo, constableFoo, a, b, c, d, e, f)
};
int TestOverloads::b = 0;
int & TestOverloads::d = TestOverloads::b;

TEST(ReflectionTest, ReflectOverloadMember)
{
    TestOverloads testOverloads{};

    using Foo = RareTs::MemberType<TestOverloads>::foo;
    using RefFoo = RareTs::MemberType<TestOverloads>::refFoo;
    using StaticFoo = RareTs::MemberType<TestOverloads>::staticFoo;
    using ConstableFoo = RareTs::MemberType<TestOverloads>::constableFoo;

    bool isSame = std::is_same_v<RareTs::Member<TestOverloads, 0>, Foo>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<TestOverloads, 1>, RefFoo>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<TestOverloads, 2>, StaticFoo>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<TestOverloads, 3>, ConstableFoo>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<std::nullptr_t, typename Foo::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::nullptr_t, typename RefFoo::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::nullptr_t, typename StaticFoo::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::nullptr_t, typename ConstableFoo::type>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<std::nullptr_t, typename Foo::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::nullptr_t, typename RefFoo::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::nullptr_t, typename StaticFoo::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::nullptr_t, typename ConstableFoo::pointer_type>;
    EXPECT_TRUE(isSame);
    
    EXPECT_STREQ("void", Foo::typeStr);
    EXPECT_STREQ("void", RefFoo::typeStr);
    EXPECT_STREQ("void", StaticFoo::typeStr);
    EXPECT_STREQ("void", ConstableFoo::typeStr);
    
    EXPECT_STREQ("foo", Foo::name);
    EXPECT_STREQ("refFoo", RefFoo::name);
    EXPECT_STREQ("staticFoo", StaticFoo::name);
    EXPECT_STREQ("constableFoo", ConstableFoo::name);
    
    EXPECT_EQ(0, Foo::index);
    EXPECT_EQ(1, RefFoo::index);
    EXPECT_EQ(2, StaticFoo::index);
    EXPECT_EQ(3, ConstableFoo::index);
    
    EXPECT_FALSE(Foo::isStatic);
    EXPECT_FALSE(RefFoo::isStatic);
    EXPECT_FALSE(StaticFoo::isStatic);
    EXPECT_FALSE(ConstableFoo::isStatic);
    
    EXPECT_FALSE(Foo::isFunction);
    EXPECT_FALSE(RefFoo::isFunction);
    EXPECT_FALSE(StaticFoo::isFunction);
    EXPECT_FALSE(ConstableFoo::isFunction);
    
    EXPECT_TRUE(Foo::isOverloaded);
    EXPECT_TRUE(RefFoo::isOverloaded);
    EXPECT_TRUE(StaticFoo::isOverloaded);
    EXPECT_TRUE(ConstableFoo::isOverloaded);
    
    EXPECT_FALSE(Foo::isData);
    EXPECT_FALSE(RefFoo::isData);
    EXPECT_FALSE(StaticFoo::isData);
    EXPECT_FALSE(ConstableFoo::isData);
    
    EXPECT_FALSE(Foo::hasOffset);
    EXPECT_FALSE(RefFoo::hasOffset);
    EXPECT_FALSE(StaticFoo::hasOffset);
    EXPECT_FALSE(ConstableFoo::hasOffset);
    
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Foo::getOffset());
    EXPECT_EQ(std::numeric_limits<size_t>::max(), RefFoo::getOffset());
    EXPECT_EQ(std::numeric_limits<size_t>::max(), StaticFoo::getOffset());
    EXPECT_EQ(std::numeric_limits<size_t>::max(), ConstableFoo::getOffset());
    
    EXPECT_EQ(nullptr, Foo::pointer);
    EXPECT_EQ(nullptr, RefFoo::pointer);
    EXPECT_EQ(nullptr, StaticFoo::pointer);
    EXPECT_EQ(nullptr, ConstableFoo::pointer);
    
    EXPECT_EQ(nullptr, Foo::value());
    EXPECT_EQ(nullptr, RefFoo::value());
    EXPECT_EQ(nullptr, StaticFoo::value());
    EXPECT_EQ(nullptr, ConstableFoo::value());

    EXPECT_EQ(nullptr, Foo::value(testOverloads));
    EXPECT_EQ(nullptr, RefFoo::value(testOverloads));
    EXPECT_EQ(nullptr, StaticFoo::value(testOverloads));
    EXPECT_EQ(nullptr, ConstableFoo::value(testOverloads));
}

TEST(ReflectionTest, ReflectOverloadMemberKnownOverloadsType)
{
    TestOverloads testOverloads{};

    using Foo = RareTs::MemberType<TestOverloads>::foo;
    using RefFoo = RareTs::MemberType<TestOverloads>::refFoo;
    using StaticFoo = RareTs::MemberType<TestOverloads>::staticFoo;
    using ConstableFoo = RareTs::MemberType<TestOverloads>::constableFoo;
    
    using FooOverloads = Foo::Overloads;
    using RefFooOverloads = RefFoo::Overloads;
    using StaticFooOverloads = StaticFoo::Overloads;
    using ConstableFooOverloads = ConstableFoo::Overloads;

    bool isSame = std::is_same_v<RareTs::KnownOverloads<TestOverloads, 0, RareTs::Class::template member_note_wrapper<TestOverloads, 0>>, FooOverloads>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::KnownOverloads<TestOverloads, 1, RareTs::Class::template member_note_wrapper<TestOverloads, 1>>, RefFooOverloads>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::KnownOverloads<TestOverloads, 2, RareTs::Class::template member_note_wrapper<TestOverloads, 2>, 0, 1>, StaticFooOverloads>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::KnownOverloads<TestOverloads, 3, RareTs::Class::template member_note_wrapper<TestOverloads, 3>, 0>, ConstableFooOverloads>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectionTest, ReflectKnownOverloads)
{
    TestOverloads testOverloads{};
    const TestOverloads constTestOverloads{};

    using Foo = RareTs::MemberType<TestOverloads>::foo;
    using RefFoo = RareTs::MemberType<TestOverloads>::refFoo;
    using StaticFoo = RareTs::MemberType<TestOverloads>::staticFoo;
    using ConstableFoo = RareTs::MemberType<TestOverloads>::constableFoo;
    
    using FooOverloads = typename Foo::Overloads;
    using RefFooOverloads = typename RefFoo::Overloads;
    using StaticFooOverloads = typename StaticFoo::Overloads;
    using ConstableFooOverloads = typename ConstableFoo::Overloads;

    bool isSame = std::is_same_v<TestOverloads, typename FooOverloads::class_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<TestOverloads, typename RefFooOverloads::class_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<TestOverloads, typename StaticFooOverloads::class_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<TestOverloads, typename ConstableFooOverloads::class_type>;
    EXPECT_TRUE(isSame);
    
    EXPECT_EQ(0, FooOverloads::memberIndex);
    EXPECT_EQ(1, RefFooOverloads::memberIndex);
    EXPECT_EQ(2, StaticFooOverloads::memberIndex);
    EXPECT_EQ(3, ConstableFooOverloads::memberIndex);
    
    EXPECT_EQ(0, FooOverloads::totalArgumentSets);
    EXPECT_EQ(0, RefFooOverloads::totalArgumentSets);
    EXPECT_EQ(2, StaticFooOverloads::totalArgumentSets);
    EXPECT_EQ(1, ConstableFooOverloads::totalArgumentSets);

    size_t visitCount = 0;
    FooOverloads::forEach([&](auto) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    FooOverloads::forEach<const TestOverloads>([&](auto) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);
    FooOverloads::forEach(testOverloads, [&](auto) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    visitCount = 0;
    RefFooOverloads::forEach([&](auto) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    RefFooOverloads::forEach<const TestOverloads>([&](auto) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);
    RefFooOverloads::forEach(testOverloads, [&](auto) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    using StaticFooCharOverload = typename StaticFooOverloads::template Overload<0>;
    using StaticFooCharIntOverload = typename StaticFooOverloads::template Overload<1>;

    isSame = std::is_same_v<int(*)(char), typename StaticFooCharOverload::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(*)(char, int), typename StaticFooCharIntOverload::pointer_type>;
    EXPECT_TRUE(isSame);

    auto staticFooCharOverloadPtr = static_cast<int(*)(char)>(&TestOverloads::staticFoo);
    auto staticFooCharIntOverloadPtr = static_cast<int(*)(char, int)>(&TestOverloads::staticFoo);
    EXPECT_EQ(staticFooCharOverloadPtr, StaticFooCharOverload{}.pointer);
    EXPECT_EQ(staticFooCharIntOverloadPtr, StaticFooCharIntOverload{}.pointer);
    
    using UnqualStaticFooCharOverload = typename StaticFooOverloads::template ResolveOverload<0, TestOverloads>;
    using UnqualStaticFooCharIntOverload = typename StaticFooOverloads::template ResolveOverload<1, TestOverloads>;
    isSame = std::is_same_v<int(*)(char), typename UnqualStaticFooCharOverload::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(*)(char, int), typename UnqualStaticFooCharIntOverload::pointer_type>;
    EXPECT_TRUE(isSame);

    EXPECT_EQ(staticFooCharOverloadPtr, UnqualStaticFooCharOverload{}.pointer);
    EXPECT_EQ(staticFooCharIntOverloadPtr, UnqualStaticFooCharIntOverload{}.pointer);

    visitCount = 0;
    StaticFooOverloads::forEach([&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(Overload::index, visitCount);
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(*)(char), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(staticFooCharOverloadPtr, overload.pointer);
            ++visitCount;
        }
        else if constexpr ( Overload::index == 1 )
        {
            isSame = std::is_same_v<int(*)(char, int), typename Overload::pointer_type>;
            EXPECT_EQ(staticFooCharIntOverloadPtr, overload.pointer);
            EXPECT_TRUE(isSame);
            ++visitCount;
        }
    });
    EXPECT_EQ(2, visitCount);

    visitCount = 0;
    StaticFooOverloads::forEach<TestOverloads>([&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(Overload::index, visitCount);
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(*)(char), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(staticFooCharOverloadPtr, overload.pointer);
            ++visitCount;
        }
        else if constexpr ( Overload::index == 1 )
        {
            isSame = std::is_same_v<int(*)(char, int), typename Overload::pointer_type>;
            EXPECT_EQ(staticFooCharIntOverloadPtr, overload.pointer);
            EXPECT_TRUE(isSame);
            ++visitCount;
        }
    });
    EXPECT_EQ(2, visitCount);

    visitCount = 0;
    StaticFooOverloads::forEach(testOverloads, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(Overload::index, visitCount);
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(*)(char), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(staticFooCharOverloadPtr, overload.pointer);
            ++visitCount;
        }
        else if constexpr ( Overload::index == 1 )
        {
            isSame = std::is_same_v<int(*)(char, int), typename Overload::pointer_type>;
            EXPECT_EQ(staticFooCharIntOverloadPtr, overload.pointer);
            EXPECT_TRUE(isSame);
            ++visitCount;
        }
    });
    EXPECT_EQ(2, visitCount);

    bool visited = false;
    StaticFooOverloads::at(0, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(*)(char), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(staticFooCharOverloadPtr, overload.pointer);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    visited = false;
    StaticFooOverloads::at(1, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 1 )
        {
            isSame = std::is_same_v<int(*)(char, int), typename Overload::pointer_type>;
            EXPECT_EQ(staticFooCharIntOverloadPtr, overload.pointer);
            EXPECT_TRUE(isSame);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);

    visited = false;
    StaticFooOverloads::at<TestOverloads>(0, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(*)(char), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(staticFooCharOverloadPtr, overload.pointer);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    visited = false;
    StaticFooOverloads::at<TestOverloads>(1, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 1 )
        {
            isSame = std::is_same_v<int(*)(char, int), typename Overload::pointer_type>;
            EXPECT_EQ(staticFooCharIntOverloadPtr, overload.pointer);
            EXPECT_TRUE(isSame);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);

    visited = false;
    StaticFooOverloads::at(testOverloads, 0, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(*)(char), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(staticFooCharOverloadPtr, overload.pointer);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    visited = false;
    StaticFooOverloads::at(testOverloads, 1, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 1 )
        {
            isSame = std::is_same_v<int(*)(char, int), typename Overload::pointer_type>;
            EXPECT_EQ(staticFooCharIntOverloadPtr, overload.pointer);
            EXPECT_TRUE(isSame);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    StaticFooOverloads::pack([&](auto charOverload, auto charIntOverload) {
        using CharOverload = RareTs::remove_cvref_t<decltype(charOverload)>;
        using CharIntOverload = RareTs::remove_cvref_t<decltype(charIntOverload)>;
        EXPECT_EQ(0, CharOverload::index);
        EXPECT_EQ(1, CharIntOverload::index);
        isSame = std::is_same_v<int(*)(char), typename CharOverload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(staticFooCharOverloadPtr, charOverload.pointer);
        isSame = std::is_same_v<int(*)(char, int), typename CharIntOverload::pointer_type>;
        EXPECT_EQ(staticFooCharIntOverloadPtr, charIntOverload.pointer);
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    StaticFooOverloads::pack<TestOverloads>([&](auto charOverload, auto charIntOverload) {
        using CharOverload = RareTs::remove_cvref_t<decltype(charOverload)>;
        using CharIntOverload = RareTs::remove_cvref_t<decltype(charIntOverload)>;
        EXPECT_EQ(0, CharOverload::index);
        EXPECT_EQ(1, CharIntOverload::index);
        isSame = std::is_same_v<int(*)(char), typename CharOverload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(staticFooCharOverloadPtr, charOverload.pointer);
        isSame = std::is_same_v<int(*)(char, int), typename CharIntOverload::pointer_type>;
        EXPECT_EQ(staticFooCharIntOverloadPtr, charIntOverload.pointer);
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    StaticFooOverloads::pack(testOverloads, [&](auto charOverload, auto charIntOverload) {
        using CharOverload = RareTs::remove_cvref_t<decltype(charOverload)>;
        using CharIntOverload = RareTs::remove_cvref_t<decltype(charIntOverload)>;
        EXPECT_EQ(0, CharOverload::index);
        EXPECT_EQ(1, CharIntOverload::index);
        isSame = std::is_same_v<int(*)(char), typename CharOverload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(staticFooCharOverloadPtr, charOverload.pointer);
        isSame = std::is_same_v<int(*)(char, int), typename CharIntOverload::pointer_type>;
        EXPECT_EQ(staticFooCharIntOverloadPtr, charIntOverload.pointer);
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    using ConstableFooOverload = typename ConstableFooOverloads::template Overload<0>;
    using NonConstConstableFooOverload = typename ConstableFooOverloads::template Overload<0, TestOverloads>;
    using ConstConstableFooOverload = typename ConstableFooOverloads::template Overload<0, const TestOverloads>;
    
    isSame = std::is_same_v<int(TestOverloads::*)(int), typename ConstableFooOverload::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestOverloads::*)(int), typename NonConstConstableFooOverload::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestOverloads::*)(int) const, typename ConstConstableFooOverload::pointer_type>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<int(TestOverloads::*)(int), typename ConstableFooOverload::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestOverloads::*)(int), typename NonConstConstableFooOverload::pointer_type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestOverloads::*)(int) const, typename ConstConstableFooOverload::pointer_type>;
    EXPECT_TRUE(isSame);
    
    auto nonConstConstableFooOverloadPtr = static_cast<int(TestOverloads::*)(int)>(&TestOverloads::constableFoo);
    auto constConstableFooOverloadPtr = static_cast<int(TestOverloads::*)(int) const>(&TestOverloads::constableFoo);
    EXPECT_EQ(nonConstConstableFooOverloadPtr, ConstableFooOverload{}.pointer);
    EXPECT_EQ(nonConstConstableFooOverloadPtr, NonConstConstableFooOverload{}.pointer);
    EXPECT_EQ(constConstableFooOverloadPtr, ConstConstableFooOverload{}.pointer);
    
    visited = false;
    ConstableFooOverloads::forEach([&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    ConstableFooOverloads::forEach<TestOverloads>([&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ConstableFooOverloads::forEach<const TestOverloads>([&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int) const, typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(constConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    ConstableFooOverloads::forEach(testOverloads, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ConstableFooOverloads::forEach(constTestOverloads, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int) const, typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(constConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    ConstableFooOverloads::at(0, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    ConstableFooOverloads::at<TestOverloads>(0, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    visited = false;
    ConstableFooOverloads::at<const TestOverloads>(0, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(TestOverloads::*)(int) const, typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(constConstableFooOverloadPtr, overload.pointer);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    ConstableFooOverloads::at(testOverloads, 0, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    visited = false;
    ConstableFooOverloads::at(constTestOverloads, 0, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        if constexpr ( Overload::index == 0 )
        {
            isSame = std::is_same_v<int(TestOverloads::*)(int) const, typename Overload::pointer_type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(constConstableFooOverloadPtr, overload.pointer);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    ConstableFooOverloads::pack([&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    ConstableFooOverloads::pack<TestOverloads>([&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ConstableFooOverloads::pack<const TestOverloads>([&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int) const, typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(constConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    ConstableFooOverloads::pack(testOverloads, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int), typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(nonConstConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
    visited = false;
    ConstableFooOverloads::pack(constTestOverloads, [&](auto overload) {
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;
        EXPECT_EQ(0, Overload::index);
        isSame = std::is_same_v<int(TestOverloads::*)(int) const, typename Overload::pointer_type>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ(constConstableFooOverloadPtr, overload.pointer);
        visited = true;
    });
    EXPECT_TRUE(visited);
}

struct TestUnknowns
{
private:
    NOTE(foo)
    int foo(char) { return 1; }
    int foo(char) const noexcept { return 2; }
    int foo(char) volatile { return 3; }
    int foo(char) const volatile { return 4; }

    int foo(char, int) { return 5; }

    NOTE(refFoo)
    int refFoo(char) & { return 5; }
    int refFoo(char) const & { return 6; }
    int refFoo(char) volatile & { return 7; }
    bool refFoo(char) const volatile & { return false; }
    int refFoo(char) && { return 9; }
    int refFoo(char) const && { return 10; }
    int refFoo(char) volatile && { return 11; }
    int refFoo(char) const volatile && noexcept { return 12; }

    int refFoo(char, int) & { return 1337; }
    
    NOTE(staticFoo)
    static int staticFoo(char) { return 13; }
    static int staticFoo(char, int) { return 13; }

    int a = 0;
    inline static int b = 0;
    int & c = a;
    inline static int & d = b;
    int e() { return 1337; }
    static int f() { return 13; }
    
    inline auto getA() { return a; }
    inline auto getB() { return b; }
    inline auto getC() { return c; }
    inline auto getD() { return d; }
    inline auto getE() { return &TestUnknowns::e; }
    inline auto getF() { return &TestUnknowns::f; }

    REFLECT(TestUnknowns, foo, refFoo, staticFoo, a, b, c, d, e, f)

public:
    static constexpr auto fooCharInfoPtr = static_cast<int(TestUnknowns::*)(char)>(&TestUnknowns::foo);
    static constexpr auto fooCharInfoPtrConst = static_cast<int(TestUnknowns::*)(char) const>(&TestUnknowns::foo);
    static constexpr auto fooCharInfoPtrVolatile = static_cast<int(TestUnknowns::*)(char) volatile>(&TestUnknowns::foo);
    static constexpr auto fooCharInfoPtrConstVolatile = static_cast<int(TestUnknowns::*)(char) const volatile>(&TestUnknowns::foo);
    
    static constexpr auto fooCharIntInfoPtr = static_cast<int(TestUnknowns::*)(char, int)>(&TestUnknowns::foo);
    
    static constexpr auto refFooCharInfoPtr = static_cast<int(TestUnknowns::*)(char) &>(&TestUnknowns::refFoo);
    static constexpr auto refFooCharInfoPtrConst = static_cast<int(TestUnknowns::*)(char) const &>(&TestUnknowns::refFoo);
    static constexpr auto refFooCharInfoPtrVolatile = static_cast<int(TestUnknowns::*)(char) volatile &>(&TestUnknowns::refFoo);
    static constexpr auto refFooCharInfoPtrConstVolatile = static_cast<bool(TestUnknowns::*)(char) const volatile &>(&TestUnknowns::refFoo);
    static constexpr auto rrefFooCharInfoPtr = static_cast<int(TestUnknowns::*)(char) &&>(&TestUnknowns::refFoo);
    static constexpr auto rrefFooCharInfoPtrConst = static_cast<int(TestUnknowns::*)(char) const &&>(&TestUnknowns::refFoo);
    static constexpr auto rrefFooCharInfoPtrVolatile = static_cast<int(TestUnknowns::*)(char) volatile &&>(&TestUnknowns::refFoo);
    static constexpr auto rrefFooCharInfoPtrConstVolatile = static_cast<int(TestUnknowns::*)(char) const volatile &&>(&TestUnknowns::refFoo);
    
    static constexpr auto rrefFooCharIntInfoPtr = static_cast<int(TestUnknowns::*)(char, int) &>(&TestUnknowns::refFoo);
    
    static constexpr auto staticFooCharInfoPtr = static_cast<int(*)(char)>(&TestUnknowns::staticFoo);
    static constexpr auto staticFooCharIntInfoPtr = static_cast<int(*)(char, int)>(&TestUnknowns::staticFoo);
};

TEST(ReflectionTest, ReflectUnknownOverloads)
{
    using FooCharInfo = RareTs::OverloadInfo<TestUnknowns, 0, char>;
    using FooCharIntInfo = RareTs::OverloadInfo<TestUnknowns, 0, char, int>;
    using RefFooCharInfo = RareTs::OverloadInfo<TestUnknowns, 1, char>;
    using RefFooCharIntInfo = RareTs::OverloadInfo<TestUnknowns, 1, char, int>;
    using StaticFooCharInfo = RareTs::OverloadInfo<TestUnknowns, 2, char>;
    using StaticFooCharIntInfo = RareTs::OverloadInfo<TestUnknowns, 2, char, int>;

    bool isSame = std::is_same_v<std::tuple<char>, typename FooCharInfo::argument_types>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char, int>, typename FooCharIntInfo::argument_types>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char>, typename RefFooCharInfo::argument_types>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char, int>, typename RefFooCharIntInfo::argument_types>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char>, typename StaticFooCharInfo::argument_types>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char, int>, typename StaticFooCharIntInfo::argument_types>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<int(TestUnknowns::*)(char), typename FooCharInfo::pointer_type<TestUnknowns>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) const noexcept, typename FooCharInfo::pointer_type<const TestUnknowns>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) volatile, typename FooCharInfo::pointer_type<volatile TestUnknowns>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) const volatile, typename FooCharInfo::pointer_type<const volatile TestUnknowns>>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<int(TestUnknowns::*)(char, int), typename FooCharIntInfo::pointer_type<TestUnknowns>>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<int(TestUnknowns::*)(char) &, typename RefFooCharInfo::pointer_type<TestUnknowns &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) const &, typename RefFooCharInfo::pointer_type<const TestUnknowns &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) volatile &, typename RefFooCharInfo::pointer_type<volatile TestUnknowns &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<bool(TestUnknowns::*)(char) const volatile &, typename RefFooCharInfo::pointer_type<const volatile TestUnknowns &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) &&, typename RefFooCharInfo::pointer_type<TestUnknowns &&>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) const &&, typename RefFooCharInfo::pointer_type<const TestUnknowns &&>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) volatile &&, typename RefFooCharInfo::pointer_type<volatile TestUnknowns &&>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int(TestUnknowns::*)(char) const volatile && noexcept, typename RefFooCharInfo::pointer_type<const volatile TestUnknowns &&>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<int(TestUnknowns::*)(char, int) &, typename RefFooCharIntInfo::pointer_type<TestUnknowns &>>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<int(*)(char), typename StaticFooCharInfo::pointer_type<TestUnknowns>>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<int(*)(char, int), typename StaticFooCharIntInfo::pointer_type<TestUnknowns>>;
    EXPECT_TRUE(isSame);
    
    auto fooCharInfoPtr = TestUnknowns::fooCharInfoPtr;
    auto fooCharInfoPtrConst = TestUnknowns::fooCharInfoPtrConst;
    auto fooCharInfoPtrVolatile = TestUnknowns::fooCharInfoPtrVolatile;
    auto fooCharInfoPtrConstVolatile = TestUnknowns::fooCharInfoPtrConstVolatile;
    
    auto fooCharIntInfoPtr = TestUnknowns::fooCharIntInfoPtr;
    
    auto refFooCharInfoPtr = TestUnknowns::refFooCharInfoPtr;
    auto refFooCharInfoPtrConst = TestUnknowns::refFooCharInfoPtrConst;
    auto refFooCharInfoPtrVolatile = TestUnknowns::refFooCharInfoPtrVolatile;
    auto refFooCharInfoPtrConstVolatile = TestUnknowns::refFooCharInfoPtrConstVolatile;
    auto rrefFooCharInfoPtr = TestUnknowns::rrefFooCharInfoPtr;
    auto rrefFooCharInfoPtrConst = TestUnknowns::rrefFooCharInfoPtrConst;
    auto rrefFooCharInfoPtrVolatile = TestUnknowns::rrefFooCharInfoPtrVolatile;
    auto rrefFooCharInfoPtrConstVolatile = TestUnknowns::rrefFooCharInfoPtrConstVolatile;
    
    auto rrefFooCharIntInfoPtr = TestUnknowns::rrefFooCharIntInfoPtr;
    
    auto staticFooCharInfoPtr = TestUnknowns::staticFooCharInfoPtr;
    auto staticFooCharIntInfoPtr = TestUnknowns::staticFooCharIntInfoPtr;

    EXPECT_EQ(fooCharInfoPtr, FooCharInfo::pointer<TestUnknowns>);
    EXPECT_EQ(fooCharInfoPtrConst, FooCharInfo::pointer<const TestUnknowns>);
    EXPECT_EQ(fooCharInfoPtrVolatile, FooCharInfo::pointer<volatile TestUnknowns>);
    EXPECT_EQ(fooCharInfoPtrConstVolatile, FooCharInfo::pointer<const volatile TestUnknowns>);
    
    EXPECT_EQ(fooCharIntInfoPtr, FooCharIntInfo::pointer<TestUnknowns>);
    
    EXPECT_EQ(refFooCharInfoPtr, RefFooCharInfo::pointer<TestUnknowns &>);
    EXPECT_EQ(refFooCharInfoPtrConst, RefFooCharInfo::pointer<const TestUnknowns &>);
    EXPECT_EQ(refFooCharInfoPtrVolatile, RefFooCharInfo::pointer<volatile TestUnknowns &>);
    EXPECT_EQ(refFooCharInfoPtrConstVolatile, RefFooCharInfo::pointer<const volatile TestUnknowns &>);
    EXPECT_EQ(rrefFooCharInfoPtr, RefFooCharInfo::pointer<TestUnknowns &&>);
    EXPECT_EQ(rrefFooCharInfoPtrConst, RefFooCharInfo::pointer<const TestUnknowns &&>);
    EXPECT_EQ(rrefFooCharInfoPtrVolatile, RefFooCharInfo::pointer<volatile TestUnknowns &&>);
    EXPECT_EQ(rrefFooCharInfoPtrConstVolatile, RefFooCharInfo::pointer<const volatile TestUnknowns &&>);
    
    EXPECT_EQ(rrefFooCharIntInfoPtr, RefFooCharIntInfo::pointer<TestUnknowns &>);
    
    EXPECT_EQ(staticFooCharInfoPtr, StaticFooCharInfo::pointer<TestUnknowns>);
    EXPECT_EQ(staticFooCharIntInfoPtr, StaticFooCharIntInfo::pointer<TestUnknowns>);
    
    EXPECT_FALSE(FooCharInfo::isStatic);
    EXPECT_FALSE(FooCharIntInfo::isStatic);
    EXPECT_FALSE(RefFooCharInfo::isStatic);
    EXPECT_FALSE(RefFooCharIntInfo::isStatic);
    EXPECT_TRUE(StaticFooCharInfo::isStatic);
    EXPECT_TRUE(StaticFooCharIntInfo::isStatic);

    EXPECT_EQ(4, FooCharInfo::totalQualifications);
    EXPECT_EQ(1, FooCharIntInfo::totalQualifications);
    EXPECT_EQ(8, RefFooCharInfo::totalQualifications);
    EXPECT_EQ(1, RefFooCharIntInfo::totalQualifications);
    EXPECT_EQ(1, StaticFooCharInfo::totalQualifications);
    EXPECT_EQ(1, StaticFooCharIntInfo::totalQualifications);
    
    EXPECT_EQ(fooCharInfoPtr, std::get<0>(FooCharInfo::pointers));
    EXPECT_EQ(fooCharInfoPtrConst, std::get<1>(FooCharInfo::pointers));
    EXPECT_EQ(fooCharInfoPtrVolatile, std::get<2>(FooCharInfo::pointers));
    EXPECT_EQ(fooCharInfoPtrConstVolatile, std::get<3>(FooCharInfo::pointers));
    
    EXPECT_EQ(fooCharIntInfoPtr, std::get<0>(FooCharIntInfo::pointers));
    
    EXPECT_EQ(refFooCharInfoPtr, std::get<0>(RefFooCharInfo::pointers));
    EXPECT_EQ(refFooCharInfoPtrConst, std::get<1>(RefFooCharInfo::pointers));
    EXPECT_EQ(refFooCharInfoPtrVolatile, std::get<2>(RefFooCharInfo::pointers));
    EXPECT_EQ(refFooCharInfoPtrConstVolatile, std::get<3>(RefFooCharInfo::pointers));
    EXPECT_EQ(rrefFooCharInfoPtr, std::get<4>(RefFooCharInfo::pointers));
    EXPECT_EQ(rrefFooCharInfoPtrConst, std::get<5>(RefFooCharInfo::pointers));
    EXPECT_EQ(rrefFooCharInfoPtrVolatile, std::get<6>(RefFooCharInfo::pointers));
    EXPECT_EQ(rrefFooCharInfoPtrConstVolatile, std::get<7>(RefFooCharInfo::pointers));
    
    EXPECT_EQ(rrefFooCharIntInfoPtr, std::get<0>(RefFooCharIntInfo::pointers));
    
    EXPECT_EQ(staticFooCharInfoPtr, std::get<0>(StaticFooCharInfo::pointers));
    
    EXPECT_EQ(staticFooCharIntInfoPtr, std::get<0>(StaticFooCharIntInfo::pointers));

    isSame = std::is_same_v<typename FooCharInfo::pointer_types, std::tuple<
        int(TestUnknowns::*)(char),
        int(TestUnknowns::*)(char) const noexcept,
        int(TestUnknowns::*)(char) volatile,
        int(TestUnknowns::*)(char) const volatile>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<typename FooCharIntInfo::pointer_types, std::tuple<int(TestUnknowns::*)(char, int)>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<typename RefFooCharInfo::pointer_types, std::tuple<
        int(TestUnknowns::*)(char) &,
        int(TestUnknowns::*)(char) const &,
        int(TestUnknowns::*)(char) volatile &,
        bool(TestUnknowns::*)(char) const volatile &,
        int(TestUnknowns::*)(char) &&,
        int(TestUnknowns::*)(char) const &&,
        int(TestUnknowns::*)(char) volatile &&,
        int(TestUnknowns::*)(char) const volatile && noexcept>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<typename RefFooCharIntInfo::pointer_types, std::tuple<int(TestUnknowns::*)(char, int) &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<typename StaticFooCharInfo::pointer_types, std::tuple<int(*)(char)>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<typename StaticFooCharIntInfo::pointer_types, std::tuple<int(*)(char, int)>>;
    EXPECT_TRUE(isSame);
}
