#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <map>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>
using namespace RareTs;

namespace ReflectPrivateTest { struct EmptyObj {}; }
REFLECT_PRIVATE_EMPTY(ReflectPrivateTest::EmptyObj)

namespace ReflectPrivateTest {

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

}

REFLECT_PRIVATE(ReflectPrivateTest::UnownedObj1, a, b)
REFLECT_PRIVATE(ReflectPrivateTest::UnownedObj2, a, b, c)

namespace ReflectPrivateTest
{

class ReflectSuperObj {
public:
    int superVal;
};

class ReflectSubObj {
public:
    int val;
};

class ReflectObj : public ReflectSuperObj {
public:
    ReflectObj() : primitive(0), object({}) { primitiveArray[0] = 0; primitiveArray[1] = 1; }

    int primitive;
    ReflectSubObj object;
    int primitiveArray[2];
    std::map<int,float> map;
    std::vector<ReflectSubObj> objCollection;
    std::stack<int> stack;
    static int staticPrimitive;
    int memberMethod() { return 1; }
    static int staticMethod() { return 2; }
};

class PrivateIndexOf
{
    PrivateIndexOf() : primitive(0), object({}) { primitiveArray[0] = 0; primitiveArray[1] = 1; }

    int primitive;
    ReflectSubObj object;
    int primitiveArray[2];
    std::map<int,float> map;
    std::vector<ReflectSubObj> objCollection;
    std::stack<int> stack;
    static int staticPrimitive;
    int memberMethod() { return 1; }
    static int staticMethod() { return 2; }
    
    inline auto getPrimitive() { return primitive; }
    inline auto getObject() { return object; }
    inline auto getPrimitiveArray() { return primitiveArray; }
    inline auto getMap() { return map; }
    inline auto getObjCollection() { return objCollection; }
    inline auto getStack() { return stack; }
    inline auto getStaticPrimitive() { return staticPrimitive; }
    inline auto getMemberMethod() { return &PrivateIndexOf::memberMethod; }
    inline auto getStaticMethod() { return &PrivateIndexOf::staticMethod; }
};

int ReflectObj::staticPrimitive = 33;

}

REFLECT_PRIVATE(ReflectPrivateTest::ReflectSuperObj, superVal)
REFLECT_PRIVATE(ReflectPrivateTest::ReflectSubObj, val)
REFLECT_PRIVATE_NOTED((ReflectPrivateTest::ReflectObj) (Super<ReflectPrivateTest::ReflectSuperObj>),
    (primitive) (),
    (object) (),
    (primitiveArray) (),
    (map) (),
    (objCollection) (),
    (stack) (),
    (staticPrimitive) (),
    (memberMethod) (),
    (staticMethod) ()
)
REFLECT_PRIVATE(ReflectPrivateTest::PrivateIndexOf, primitive, object, primitiveArray, map, objCollection, stack, staticPrimitive, memberMethod, staticMethod)

namespace ReflectPrivateTest
{

TEST(PrivateReflectTest, IndexOf)
{
    EXPECT_EQ(0, RareTs::IndexOf<ReflectObj>::primitive);
    EXPECT_EQ(1, RareTs::IndexOf<ReflectObj>::object);
    EXPECT_EQ(2, RareTs::IndexOf<ReflectObj>::primitiveArray);
    EXPECT_EQ(3, RareTs::IndexOf<ReflectObj>::map);
    EXPECT_EQ(4, RareTs::IndexOf<ReflectObj>::objCollection);
    EXPECT_EQ(5, RareTs::IndexOf<ReflectObj>::stack);
    EXPECT_EQ(6, RareTs::IndexOf<ReflectObj>::staticPrimitive);
    EXPECT_EQ(7, RareTs::IndexOf<ReflectObj>::memberMethod);
    EXPECT_EQ(8, RareTs::IndexOf<ReflectObj>::staticMethod);
    
    EXPECT_EQ(0, RareTs::IndexOf<PrivateIndexOf>::primitive);
    EXPECT_EQ(1, RareTs::IndexOf<PrivateIndexOf>::object);
    EXPECT_EQ(2, RareTs::IndexOf<PrivateIndexOf>::primitiveArray);
    EXPECT_EQ(3, RareTs::IndexOf<PrivateIndexOf>::map);
    EXPECT_EQ(4, RareTs::IndexOf<PrivateIndexOf>::objCollection);
    EXPECT_EQ(5, RareTs::IndexOf<PrivateIndexOf>::stack);
    EXPECT_EQ(6, RareTs::IndexOf<PrivateIndexOf>::staticPrimitive);
    EXPECT_EQ(7, RareTs::IndexOf<PrivateIndexOf>::memberMethod);
    EXPECT_EQ(8, RareTs::IndexOf<PrivateIndexOf>::staticMethod);
}

TEST(PrivateReflectTest, MemberType)
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
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 7>, RareTs::MemberType<ReflectObj>::memberMethod>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Member<ReflectObj, 8>, RareTs::MemberType<ReflectObj>::staticMethod>;
    EXPECT_TRUE(isSame);
}

TEST(PrivateReflectTest, Typedefs)
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

TEST(PrivateReflectTestMembers, HeaderDefinitions)
{
    auto totalMembers = Reflect<ReflectSuperObj>::Members::total;
    EXPECT_EQ(size_t(1), totalMembers);
    totalMembers = Reflect<ReflectSubObj>::Members::total;
    EXPECT_EQ(size_t(1), totalMembers);
    totalMembers = Reflect<ReflectObj>::Members::total;
    EXPECT_EQ(size_t(9), totalMembers);

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
    int memberMethod() { return 3; }
    static int staticMethod() { return 4; }
};

int ReflectPackTest::staticValue = 2;

}

REFLECT_PRIVATE(ReflectPrivateTest::ReflectPackTest, memberValue, staticValue, memberMethod, staticMethod)

namespace ReflectPrivateTest
{

namespace TestFilters {
    template <typename Member, typename = enable_if_member_t<Member>> struct Integers : std::bool_constant<
        std::is_same_v<int, RareTs::remove_cvref_t<typename Member::type>>
    > {};
    template <typename Member, typename = enable_if_member_t<Member>> struct ReflectSubObjs : std::bool_constant<
        std::is_same_v<ReflectSubObj, RareTs::remove_cvref_t<typename Member::type>>
    > {};
}

TEST(PrivateReflectTestMembers, Pack)
{
    bool visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto memberValue, auto staticValue, auto memberMethod, auto staticMethod) {
        using MemberField = decltype(memberValue);
        using StaticField = decltype(staticValue);
        using MemberMethod = decltype(memberMethod);
        using StaticMethod = decltype(staticMethod);

        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto & memberValue, auto & staticValue, auto & memberMethod, auto & staticMethod) {
        using MemberField = RareTs::remove_cvref_t<decltype(memberValue)>;
        using StaticField = RareTs::remove_cvref_t<decltype(staticValue)>;
        using MemberMethod = RareTs::remove_cvref_t<decltype(memberMethod)>;
        using StaticMethod = RareTs::remove_cvref_t<decltype(staticMethod)>;
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto && memberValue, auto && staticValue, auto && memberMethod, auto && staticMethod) {
        using MemberField = RareTs::remove_cvref_t<decltype(memberValue)>;
        using StaticField = RareTs::remove_cvref_t<decltype(staticValue)>;
        using MemberMethod = RareTs::remove_cvref_t<decltype(memberMethod)>;
        using StaticMethod = RareTs::remove_cvref_t<decltype(staticMethod)>;

        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](const auto memberValue, const auto staticValue, const auto memberMethod, const auto staticMethod) {
        using MemberField = RareTs::remove_cvref_t<decltype(memberValue)>;
        using StaticField = RareTs::remove_cvref_t<decltype(staticValue)>;
        using MemberMethod = RareTs::remove_cvref_t<decltype(memberMethod)>;
        using StaticMethod = RareTs::remove_cvref_t<decltype(staticMethod)>;
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](const auto & memberValue, const auto & staticValue, const auto & memberMethod, const auto & staticMethod) {
        using MemberField = RareTs::remove_cvref_t<decltype(memberValue)>;
        using StaticField = RareTs::remove_cvref_t<decltype(staticValue)>;
        using MemberMethod = RareTs::remove_cvref_t<decltype(memberMethod)>;
        using StaticMethod = RareTs::remove_cvref_t<decltype(staticMethod)>;
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = std::tuple_element_t<0, Ts>;
        using StaticField = std::tuple_element_t<1, Ts>;
        using MemberMethod = std::tuple_element_t<2, Ts>;
        using StaticMethod = std::tuple_element_t<3, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberMethod.index);
        EXPECT_EQ(size_t(3), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = RareTs::remove_cvref_t<std::tuple_element_t<0, Ts>>;
        using StaticField = RareTs::remove_cvref_t<std::tuple_element_t<1, Ts>>;
        using MemberMethod = RareTs::remove_cvref_t<std::tuple_element_t<2, Ts>>;
        using StaticMethod = RareTs::remove_cvref_t<std::tuple_element_t<3, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberMethod.index);
        EXPECT_EQ(size_t(3), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](auto && ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = RareTs::remove_cvref_t<std::tuple_element_t<0, Ts>>;
        using StaticField = RareTs::remove_cvref_t<std::tuple_element_t<1, Ts>>;
        using MemberMethod = RareTs::remove_cvref_t<std::tuple_element_t<2, Ts>>;
        using StaticMethod = RareTs::remove_cvref_t<std::tuple_element_t<3, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberMethod.index);
        EXPECT_EQ(size_t(3), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](const auto ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = std::remove_const_t<std::tuple_element_t<0, Ts>>;
        using StaticField = std::remove_const_t<std::tuple_element_t<1, Ts>>;
        using MemberMethod = std::remove_const_t<std::tuple_element_t<2, Ts>>;
        using StaticMethod = std::remove_const_t<std::tuple_element_t<3, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberMethod.index);
        EXPECT_EQ(size_t(3), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Members::pack([&](const auto & ... ts) {
        constexpr size_t size = sizeof...(ts);
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberField = RareTs::remove_cvref_t<std::tuple_element_t<0, Ts>>;
        using StaticField = RareTs::remove_cvref_t<std::tuple_element_t<1, Ts>>;
        using MemberMethod = RareTs::remove_cvref_t<std::tuple_element_t<2, Ts>>;
        using StaticMethod = RareTs::remove_cvref_t<std::tuple_element_t<3, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);
        
        bool isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 0>, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 1>, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 2>, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<Reflection::Member<ReflectPackTest, 3>, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_EQ(size_t(0), memberValue.index);
        EXPECT_EQ(size_t(1), staticValue.index);
        EXPECT_EQ(size_t(2), memberMethod.index);
        EXPECT_EQ(size_t(3), staticMethod.index);

        visited = true;
    });
    EXPECT_TRUE(visited);

    // Note: members are always lvalues and don't have a need to be passed as rvalue references
}

TEST(PrivateReflectTestMembers, FilteredCount)
{
    size_t filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::None>();
    EXPECT_EQ(size_t(9), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsData>();
    EXPECT_EQ(size_t(7), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsFunction>();
    EXPECT_EQ(size_t(2), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsInstanceMember>();
    EXPECT_EQ(size_t(7), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsStaticMember>();
    EXPECT_EQ(size_t(2), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsInstanceData>();
    EXPECT_EQ(size_t(6), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsInstanceFunction>();
    EXPECT_EQ(size_t(1), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsStaticData>();
    EXPECT_EQ(size_t(1), filteredCount);
    filteredCount = Reflect<ReflectObj>::Members::filteredCount<Filter::IsStaticFunction>();
    EXPECT_EQ(size_t(1), filteredCount);
}

TEST(PrivateReflectTestMembers, IndexOfStr)
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
    EXPECT_EQ(size_t(7), Reflect<ReflectObj>::Members::indexOf("memberMethod"));
    EXPECT_EQ(size_t(8), Reflect<ReflectObj>::Members::indexOf("staticMethod"));

    EXPECT_EQ(npos, Reflect<PrivateIndexOf>::Members::indexOf(""));
    EXPECT_EQ(npos, Reflect<PrivateIndexOf>::Members::indexOf("asdf"));
    EXPECT_EQ(size_t(0), Reflect<PrivateIndexOf>::Members::indexOf("primitive"));
    EXPECT_EQ(size_t(1), Reflect<PrivateIndexOf>::Members::indexOf("object"));
    EXPECT_EQ(size_t(2), Reflect<PrivateIndexOf>::Members::indexOf("primitiveArray"));
    EXPECT_EQ(size_t(3), Reflect<PrivateIndexOf>::Members::indexOf("map"));
    EXPECT_EQ(size_t(4), Reflect<PrivateIndexOf>::Members::indexOf("objCollection"));
    EXPECT_EQ(size_t(5), Reflect<PrivateIndexOf>::Members::indexOf("stack"));
    EXPECT_EQ(size_t(6), Reflect<PrivateIndexOf>::Members::indexOf("staticPrimitive"));
    EXPECT_EQ(size_t(7), Reflect<PrivateIndexOf>::Members::indexOf("memberMethod"));
    EXPECT_EQ(size_t(8), Reflect<PrivateIndexOf>::Members::indexOf("staticMethod"));
}

TEST(PrivateReflectTestMembers, ForEachMember)
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
        else if constexpr ( Member::index == size_t(7) )
        {
            int result = (reflectObj.*member.pointer)();
            EXPECT_EQ(1, result);
            visitCount++;
        }
        else if constexpr ( Member::index == size_t(8) )
        {
            int result = member.pointer();
            EXPECT_EQ(2, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(3, visitCount);
    EXPECT_EQ(size_t(9), index);
}

TEST(PrivateReflectTestMembers, ForEachMemberFiltered)
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

        if constexpr ( Member::index == 7 )
        {
            int result = (reflectObj.*member.pointer)();
            EXPECT_EQ(1, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(1, visitCount);
    EXPECT_EQ(size_t(7), index);

    index = 0;
    visitCount = 0;
    Reflect<ReflectObj>::Members::forEach<Filter::IsStaticMember>([&](auto & member) {
        
        using Member = std::remove_reference_t<decltype(member)>;

        if constexpr ( Member::isStatic && Member::index == 6 )
        {
            EXPECT_EQ(ReflectObj::staticPrimitive, *member.pointer);
            visitCount++;
        }
        else if constexpr ( Member::index == 8 )
        {
            int result = member.pointer();
            EXPECT_EQ(2, result);
            visitCount++;
        }
        index ++;
    });
    EXPECT_EQ(2, visitCount);
    EXPECT_EQ(size_t(2), index);
}

TEST(PrivateReflectTestMembers, ForEachStatic)
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

TEST(PrivateReflectTestMembers, ForEachStaticFiltered)
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
            default:
                EXPECT_TRUE(false);
                break;
        }
        index ++;
    });
    EXPECT_EQ(size_t(1), index);

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

TEST(PrivateReflectTestMembers, ForEachInstanced)
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
                if constexpr ( !Member::isStatic && Member::isFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
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
    EXPECT_EQ(size_t(9), index);
}

TEST(PrivateReflectTestMembers, ForEachInstancedFiltered)
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
                if constexpr ( !Member::isStatic && Member::isFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
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
    EXPECT_EQ(size_t(9), index);

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
            default:
                EXPECT_TRUE(false);
                break;
        }
        index ++;
    });
    EXPECT_EQ(size_t(7), index);

    index = 0;
    Reflect<ReflectObj>::Members::forEach<Filter::IsFunction>(reflectObj, [&](auto & member, auto & value) {
        
        using Member = std::remove_reference_t<decltype(member)>;

        bool visited = false;
        switch ( index ) {
            case 0:
                EXPECT_EQ(size_t(7), Member::index);
                if constexpr ( !Member::isStatic && Member::isFunction ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 1:
                EXPECT_EQ(size_t(8), Member::index);
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

TEST(PrivateReflectTestMembers, MemberAt)
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
    EXPECT_EQ(size_t(9), visitCount);
}

TEST(PrivateReflectTestMembers, MemberAtFiltered)
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

TEST(PrivateReflectTestMembers, AtStatic)
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
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(PrivateReflectTestMembers, AtStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Members::at<TestFilters::Integers>(6, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(6), member.index);
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>>  ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at<TestFilters::ReflectSubObjs>(6, [&](auto &, auto &) {
        EXPECT_TRUE(false); // Should not be visited since member at index 6 is an int, not ReflectSubObj
    });
    EXPECT_EQ(size_t(1), visitCount);
}

TEST(PrivateReflectTestMembers, AtInstanced)
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
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::at(8, reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(9), visitCount);
}

TEST(PrivateReflectTestMembers, AtInstancedFiltered)
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

TEST(PrivateReflectTestMembers, NamedMember)
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
    Reflect<ReflectObj>::Members::named("memberMethod", [&](auto & member) {
        EXPECT_EQ(size_t(7), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named("staticMethod", [&](auto & member) {
        EXPECT_EQ(size_t(8), member.index);
        visitCount++;
    });
    EXPECT_EQ(size_t(9), visitCount);
}

TEST(PrivateReflectTestMembers, NamedMemberFiltered)
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
    Reflect<ReflectObj>::Members::named<Filter::IsInstanceFunction>("memberMethod", [&](auto & member) {
        EXPECT_EQ(size_t(7), member.index);
        visitCount++;
    });
    Reflect<ReflectObj>::Members::named<Filter::IsStaticFunction>("staticMethod", [&](auto & member) {
        EXPECT_EQ(size_t(8), member.index);
        visitCount++;
    });
    EXPECT_EQ(size_t(9), visitCount);
}

TEST(PrivateReflectTestMembers, NamedStatic)
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
    Reflect<ReflectObj>::Members::named("memberMethod", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named("staticMethod", [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(PrivateReflectTestMembers, NamedStaticFiltered)
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
    Reflect<ReflectObj>::Members::named<Filter::None>("memberMethod", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::IsInstanceFunction>("staticMethod", [&](auto &, auto &) { EXPECT_TRUE(false); }); // Non-instance/should not be visited
    Reflect<ReflectObj>::Members::named<Filter::IsStaticFunction>("staticMethod", [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(PrivateReflectTestMembers, NamedInstanced)
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
    Reflect<ReflectObj>::Members::named("memberMethod", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(7), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Members::named("staticMethod", reflectObj, [&](auto & member, auto & value) {
        EXPECT_EQ(size_t(8), member.index);
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(9), visitCount);
}

TEST(PrivateReflectTestMembers, NamedInstancedFiltered)
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

TEST(PrivateReflectTestValues, Pack)
{
    ReflectPackTest obj {};

    bool visited = false;
    Reflect<ReflectPackTest>::Values::pack([&](auto memberValue, auto staticValue, auto memberMethod, auto staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<decltype(&ReflectPackTest::memberValue), decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::pack([&](auto & memberValue, auto & staticValue, auto & memberMethod, auto & staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::pack([&](auto && memberValue, auto && staticValue, auto && memberMethod, auto && staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::pack([&](const auto memberValue, const auto staticValue, const auto memberMethod, const auto staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::pack([&](const auto & memberValue, const auto & staticValue, const auto & memberMethod, const auto & staticMethod) {
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::memberValue)>>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(staticValue)>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberMethodType = std::tuple_element_t<2, Ts>;
        using StaticMethodType = std::tuple_element_t<3, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);
        
        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<decltype(&ReflectPackTest::memberValue), MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberMethodType = std::tuple_element_t<2, Ts>;
        using StaticMethodType = std::tuple_element_t<3, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberValue)>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, StaticType>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberMethodType = std::tuple_element_t<2, Ts>;
        using StaticMethodType = std::tuple_element_t<3, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberValue)>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, StaticType>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberMethodType = std::tuple_element_t<2, Ts>;
        using StaticMethodType = std::tuple_element_t<3, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberValue)>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, StaticType>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberMethodType = std::tuple_element_t<2, Ts>;
        using StaticMethodType = std::tuple_element_t<3, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(&ReflectPackTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&ReflectPackTest::memberValue)>>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, StaticType>;
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

TEST(PrivateReflectTestValues, PackInstanced)
{
    ReflectPackTest obj {};

    bool visited = false;
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto memberValue, auto staticValue, auto memberMethod, auto staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<int, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&ReflectPackTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto & memberValue, auto & staticValue, auto & memberMethod, auto & staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<int &, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::pack(obj, [&](auto && memberValue, auto && staticValue, auto && memberMethod, auto && staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<int &, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    Reflect<ReflectPackTest>::Values::pack(obj, [&](const auto memberValue, const auto staticValue, const auto memberMethod, const auto staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<const int, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&ReflectPackTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    Reflect<ReflectPackTest>::Values::pack(obj, [&](const auto & memberValue, const auto & staticValue, const auto & memberMethod, const auto & staticMethod) {
        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<const int &, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(staticValue)>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::tuple_element_t<0, Ts>;
        using StaticType = std::tuple_element_t<1, Ts>;
        using MemberMethodType = std::tuple_element_t<2, Ts>;
        using StaticMethodType = std::tuple_element_t<3, Ts>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::remove_reference_t<std::tuple_element_t<0, Ts>>;
        using StaticType = std::remove_reference_t<std::tuple_element_t<1, Ts>>;
        using MemberMethodType = std::remove_reference_t<std::tuple_element_t<2, Ts>>;
        using StaticMethodType = std::remove_reference_t<std::tuple_element_t<3, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::remove_reference_t<std::tuple_element_t<0, Ts>>;
        using StaticType = std::remove_reference_t<std::tuple_element_t<1, Ts>>;
        using MemberMethodType = std::remove_reference_t<std::tuple_element_t<2, Ts>>;
        using StaticMethodType = std::remove_reference_t<std::tuple_element_t<3, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = std::remove_const_t<std::tuple_element_t<0, Ts>>;
        using StaticType = std::remove_const_t<std::tuple_element_t<1, Ts>>;
        using MemberMethodType = std::remove_const_t<std::tuple_element_t<2, Ts>>;
        using StaticMethodType = std::remove_const_t<std::tuple_element_t<3, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
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
        EXPECT_EQ(size_t(4), size);
        using Ts = std::tuple<decltype(ts)...>;
        auto values = std::forward_as_tuple(ts...);

        using MemberType = RareTs::remove_cvref_t<std::tuple_element_t<0, Ts>>;
        using StaticType = RareTs::remove_cvref_t<std::tuple_element_t<1, Ts>>;
        using MemberMethodType = RareTs::remove_cvref_t<std::tuple_element_t<2, Ts>>;
        using StaticMethodType = RareTs::remove_cvref_t<std::tuple_element_t<3, Ts>>;
        
        auto memberValue = std::get<0>(values);
        auto staticValue = std::get<1>(values);
        auto memberMethod = std::get<2>(values);
        auto staticMethod = std::get<3>(values);

        EXPECT_EQ(1, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_EQ(3, (obj.*memberMethod)());
        EXPECT_EQ(4, staticMethod());
        
        bool isSame = std::is_same_v<int, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, StaticType>;
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

TEST(PrivateReflectTestValues, ForEachStatic)
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

TEST(PrivateReflectTestValues, ForEachStaticFiltered)
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
            default:
                EXPECT_TRUE(false);
                break;
        }
        index ++;
    });
    EXPECT_EQ(size_t(1), index);

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

TEST(PrivateReflectTestValues, ForEachInstanced)
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
                if constexpr ( std::is_member_pointer_v<Value> ) {
                    auto returned = (reflectObj.*value)();
                    EXPECT_EQ(1, returned);
                    visited = true;
                }
                EXPECT_TRUE(visited);
                break;
            case 8:
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
    EXPECT_EQ(size_t(9), index);
}

TEST(PrivateReflectTestValues, ForEachInstancedFiltered)
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
            default:
                EXPECT_TRUE(false);
                break;
        }
        index ++;
    });
    EXPECT_EQ(size_t(7), index);

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

TEST(PrivateReflectTestValues, AtStatic)
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
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(PrivateReflectTestValues, AtStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::at<std::is_same, int>(6, [&](auto & value) {
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>>  ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at<TestFilters::ReflectSubObjs>(10, [&](auto &) {
        // Should not be visited as "staticMethod" is a function pointer, not a ReflectedSubObj
    });
    EXPECT_EQ(size_t(1), visitCount);
}

TEST(PrivateReflectTestValues, AtInstanced)
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
        if constexpr ( std::is_member_pointer_v<Value> ) {
            auto returned = (reflectObj.*value)();
            EXPECT_EQ(1, returned);
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::at(8, reflectObj, [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(9), visitCount);
}

TEST(PrivateReflectTestValues, AtInstancedFiltered)
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

TEST(PrivateReflectTestValues, NamedStatic)
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
    Reflect<ReflectObj>::Values::named("memberMethod", [&](auto &) { EXPECT_TRUE(false); }); // Non-static/should not be visited
    Reflect<ReflectObj>::Values::named("staticMethod", [&](auto & value) {
        using Value = std::remove_reference_t<decltype(value)>;
        if constexpr ( std::is_pointer_v<Value> ) {
            auto returned = value();
            EXPECT_EQ(2, returned);
            visitCount++;
        }
    });
    EXPECT_EQ(size_t(2), visitCount);
}

TEST(PrivateReflectTestValues, NamedStaticFiltered)
{
    size_t visitCount = 0;
    Reflect<ReflectObj>::Values::named<std::is_same, int>("staticPrimitive", [&](auto & value) {
        if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(value)>> ) {
            visitCount++;
        }
    });
    Reflect<ReflectObj>::Values::named<TestFilters::ReflectSubObjs>("staticMethod", [&](auto &) {
        // Should not be visited as "staticMethod" is a function pointer, not a ReflectedSubObj
    });
    EXPECT_EQ(size_t(1), visitCount);
}

TEST(PrivateReflectTestValues, NamedInstanced)
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
    EXPECT_EQ(size_t(9), visitCount);
}

TEST(PrivateReflectTestValues, NamedInstancedFiltered)
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
};

class EmptyNotes
{
    int a;
    inline auto getA() { return a; }
};

class SinglyNoted
{
    int a;
    inline auto getA() { return a; }
};

struct SerializedName
{
    std::string_view value;
};

class StructNoted
{
    int a;
    inline auto getA() { return a; }
};

class ComplexNoted
{
    int a;
    inline auto getA() { return a; }
};

template <typename T>
struct Specializable { using type = T; };

class SpecializedNoted
{
    int a;
    inline auto getA() { return a; }
};

}

REFLECT_PRIVATE(ReflectPrivateTest::Unnoted, a)
REFLECT_PRIVATE_NOTED((ReflectPrivateTest::EmptyNotes) (),
    (a) ())
REFLECT_PRIVATE_NOTED((ReflectPrivateTest::SinglyNoted) ('a'),
    (a) ())
REFLECT_PRIVATE_NOTED((ReflectPrivateTest::StructNoted) (ReflectPrivateTest::SerializedName { "noted" }),
    (a) ())
REFLECT_PRIVATE_NOTED((ReflectPrivateTest::ComplexNoted) (std::tuple{33, ReflectPrivateTest::SerializedName{"test"}}, 'c', 'd', 'e'),
    (a) ())
REFLECT_PRIVATE_NOTED((ReflectPrivateTest::SpecializedNoted) ('a', ReflectPrivateTest::Specializable<int>{}),
    (a) ())

namespace ReflectPrivateTest
{
    using complex_note_type = std::tuple<std::tuple<int, SerializedName>, char, char, char>;

TEST(PrivateReflectTestNotes, ForEach)
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
        if constexpr ( std::is_same_v<Annotation, std::remove_const_t<std::tuple_element_t<0, complex_note_type>>> )
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

TEST(PrivateReflectTestNotes, ForEachFiltered)
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
    Reflect<ComplexNoted>::Notes::forEachNote<std::remove_const_t<std::tuple_element_t<0, complex_note_type>>>([&](auto &) {
        EXPECT_EQ(0, visitCount);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
}

TEST(PrivateReflectTestNotes, Has)
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

TEST(PrivateReflectTestNotes, HasSpecialization)
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

TEST(PrivateReflectTestNotes, Get)
{
    auto singlyNotedChar = Reflect<SinglyNoted>::Notes::getNote<char>();
    bool isSame = std::is_same_v<char, decltype(singlyNotedChar)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', singlyNotedChar);

    constexpr auto & structNotedSerializedName = Reflect<StructNoted>::Notes::getNote<SerializedName>();
    isSame = std::is_same_v<SerializedName, RareTs::remove_cvref_t<decltype(structNotedSerializedName)>>;
    EXPECT_TRUE(isSame);
    EXPECT_STREQ("noted", std::string(structNotedSerializedName.value).c_str());

    using NestedTuple = std::remove_const_t<std::tuple_element_t<0, complex_note_type>>;
    constexpr auto & complexNotedTuple = Reflect<ComplexNoted>::Notes::getNote<NestedTuple>();
    isSame = std::is_same_v<NestedTuple, RareTs::remove_cvref_t<decltype(complexNotedTuple)>>;
    EXPECT_TRUE(isSame);
}

TEST(PrivateReflectTestNotes, GetSpecialization)
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
    Reflect<ComplexNoted>::Notes::forEachNote<std::remove_const_t<std::tuple_element_t<0, complex_note_type>>>([&](auto &) {
        EXPECT_EQ(0, visitCount);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
}

}
