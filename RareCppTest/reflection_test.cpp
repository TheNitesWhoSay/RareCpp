#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <map>
#include <stack>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
using namespace RareTs;

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

TEST(ReflectionTest, NoteMacro)
{
    bool isSame = std::is_same_v<RareTs::NoNote, std::tuple<>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::remove_const_t<decltype(RareTs::noNote)>, std::tuple<>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<std::remove_const_t<decltype(TriviallyNoted_note)>, std::tuple<>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::remove_const_t<decltype(TriviallyNoted::value_note)>, std::tuple<>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<std::remove_const_t<decltype(SinglyNoted_note)>, std::tuple<char>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', std::get<0>(SinglyNoted_note));
    isSame = std::is_same_v<std::remove_const_t<decltype(SinglyNoted::value_note)>, std::tuple<char>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('b', std::get<0>(SinglyNoted::value_note));
    
    isSame = std::is_same_v<std::remove_const_t<decltype(StructNoted_note)>, std::tuple<SerializedName>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ("noted", std::get<0>(StructNoted_note).value);
    isSame = std::is_same_v<std::remove_const_t<decltype(StructNoted::multiple_note)>, std::tuple<char, SerializedName>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('1', std::get<0>(StructNoted::multiple_note));
    EXPECT_STREQ("renamedValue", std::string(std::get<1>(StructNoted::multiple_note).value).c_str());

    isSame = std::is_same_v<std::remove_const_t<decltype(ComplexNoted_note)>, std::tuple<std::tuple<int, SerializedName>, char, char, char>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(33, std::get<0>(std::get<0>(ComplexNoted_note)));
    EXPECT_STREQ("test", std::string(std::get<1>(std::get<0>(ComplexNoted_note)).value).c_str());
    EXPECT_EQ('c', std::get<1>(ComplexNoted_note));
    EXPECT_EQ('d', std::get<2>(ComplexNoted_note));
    EXPECT_EQ('e', std::get<3>(ComplexNoted_note));

    isSame = std::is_same_v<std::remove_const_t<decltype(ComplexNoted::first_note)>, std::tuple<char, char, std::tuple<int, SerializedName, char>>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', std::get<0>(ComplexNoted::first_note));
    EXPECT_EQ('b', std::get<1>(ComplexNoted::first_note));
    EXPECT_EQ(44, std::get<0>(std::get<2>(ComplexNoted::first_note)));
    EXPECT_STREQ("1st", std::string(std::get<1>(std::get<2>(ComplexNoted::first_note)).value).c_str());
    EXPECT_EQ('c', std::get<2>(std::get<2>(ComplexNoted::first_note)));

    isSame = std::is_same_v<std::remove_const_t<decltype(ComplexNoted::second_note)>, std::tuple<char, char, char, std::pair<int, SerializedName>>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('x', std::get<0>(ComplexNoted::second_note));
    EXPECT_EQ('y', std::get<1>(ComplexNoted::second_note));
    EXPECT_EQ('z', std::get<2>(ComplexNoted::second_note));
    EXPECT_EQ(55, std::get<3>(ComplexNoted::second_note).first);
    EXPECT_STREQ("2nd", std::string(std::get<3>(ComplexNoted::second_note).second.value).c_str());
}

class UnreflectedObj {
    int a;
public:
    auto getA() { return a; }
};

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

TEST(ReflectionTest, IsProxied)
{
    EXPECT_FALSE(is_proxied<UnreflectedObj>::value);
    EXPECT_TRUE(is_proxied<UnownedObj1>::value);
    EXPECT_TRUE(is_proxied<UnownedObj2>::value);
    EXPECT_FALSE(is_proxied<OwnedObj>::value);

    EXPECT_FALSE(is_proxied_v<UnreflectedObj>);
    EXPECT_TRUE(is_proxied_v<UnownedObj1>);
    EXPECT_TRUE(is_proxied_v<UnownedObj2>);
    EXPECT_FALSE(is_proxied_v<OwnedObj>);
}

TEST(ReflectionTest, Unproxy)
{
    bool isSame = std::is_same_v<RareTs::Class::unproxy_t<UnreflectedObj>, UnreflectedObj>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Class::unproxy_t<UnownedObj1>, UnownedObj1>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Class::unproxy_t<UnownedObj2>, UnownedObj2>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Class::unproxy_t<OwnedObj>, OwnedObj>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Class::unproxy_t<RareTs::Proxy<UnownedObj1>>, UnownedObj1>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Class::unproxy_t<RareTs::Proxy<UnownedObj2>>, UnownedObj2>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Class::unproxy_t<RareTs::Proxy<OwnedObj>>, OwnedObj>;
    EXPECT_TRUE(isSame);
}

class does_not_have_reflected_class {
    int a;
public:
    auto getA() { return a; }
};
struct does_have_reflected_class {
    struct Class {
        struct I_ { enum { N_ }; };
    };
    friend constexpr Class classType(RareTs::type_tag<RareTs::Proxy<does_have_reflected_class>>);
};
struct is_reflected_REFLECT {
    int value;

    REFLECT(is_reflected_REFLECT, value)
};
NOTE(is_reflected_REFLECT_NOTED)
struct is_reflected_REFLECT_NOTED {
    int value;

    REFLECT_NOTED(is_reflected_REFLECT_NOTED, value)
};
NOTE(is_reflected_REFLECT_EMPTY)
struct is_reflected_REFLECT_EMPTY {
    REFLECT_NOTED(is_reflected_REFLECT_EMPTY)
};
class is_reflected_REFLECT_PRIVATE_EMPTY {};
REFLECT_PRIVATE_EMPTY(is_reflected_REFLECT_PRIVATE_EMPTY, "note")
class is_reflected_REFLECT_PRIVATE { int a; };
REFLECT_PRIVATE(is_reflected_REFLECT_PRIVATE, a)
struct is_reflected_REFLECT_PRIVATE_NOTED { int a; };
REFLECT_PRIVATE_NOTED(
    (is_reflected_REFLECT_PRIVATE_NOTED)(),
    (a)()
)
struct is_reflected_empty_aggregate {};
struct is_reflected_aggregate_int { int a; };
struct is_reflected_aggregate_int_int { int a; int b; };
struct is_reflected_aggregate_intray { int a[12]; };
struct is_reflected_aggregate_intray_intray { int a[12]; int b[12]; };

TEST(ReflectionTest, IsInClassReflected)
{
    EXPECT_FALSE(is_in_class_reflected<does_not_have_reflected_class>::value);
    EXPECT_TRUE(is_in_class_reflected<does_have_reflected_class>::value);
    EXPECT_TRUE(is_in_class_reflected<is_reflected_REFLECT>::value);
    EXPECT_TRUE(is_in_class_reflected<is_reflected_REFLECT_NOTED>::value);
    EXPECT_FALSE(is_in_class_reflected<is_reflected_REFLECT_PRIVATE_EMPTY>::value);
    EXPECT_FALSE(is_in_class_reflected<is_reflected_REFLECT_PRIVATE>::value);
    EXPECT_FALSE(is_in_class_reflected<is_reflected_REFLECT_PRIVATE_NOTED>::value);
    EXPECT_FALSE(is_in_class_reflected<is_reflected_empty_aggregate>::value);
    EXPECT_FALSE(is_in_class_reflected<is_reflected_aggregate_int>::value);
    EXPECT_FALSE(is_in_class_reflected<is_reflected_aggregate_int_int>::value);
    EXPECT_FALSE(is_in_class_reflected<is_reflected_aggregate_intray>::value);
    EXPECT_FALSE(is_in_class_reflected<is_reflected_aggregate_intray_intray>::value);
    EXPECT_FALSE(is_in_class_reflected<UnreflectedObj>::value);
    EXPECT_FALSE(is_in_class_reflected<UnownedObj1>::value);
    EXPECT_FALSE(is_in_class_reflected<UnownedObj2>::value);
    EXPECT_TRUE(is_in_class_reflected<OwnedObj>::value);

    EXPECT_FALSE(is_in_class_reflected_v<does_not_have_reflected_class>);
    EXPECT_TRUE(is_in_class_reflected_v<does_have_reflected_class>);
    EXPECT_TRUE(is_in_class_reflected_v<is_reflected_REFLECT>);
    EXPECT_TRUE(is_in_class_reflected_v<is_reflected_REFLECT_NOTED>);
    EXPECT_FALSE(is_in_class_reflected_v<is_reflected_REFLECT_PRIVATE_EMPTY>);
    EXPECT_FALSE(is_in_class_reflected_v<is_reflected_REFLECT_PRIVATE>);
    EXPECT_FALSE(is_in_class_reflected_v<is_reflected_REFLECT_PRIVATE_NOTED>);
    EXPECT_FALSE(is_in_class_reflected_v<is_reflected_empty_aggregate>);
    EXPECT_FALSE(is_in_class_reflected_v<is_reflected_aggregate_int>);
    EXPECT_FALSE(is_in_class_reflected_v<is_reflected_aggregate_int_int>);
    EXPECT_FALSE(is_in_class_reflected_v<is_reflected_aggregate_intray>);
    EXPECT_FALSE(is_in_class_reflected_v<is_reflected_aggregate_intray_intray>);
    EXPECT_FALSE(is_in_class_reflected_v<UnreflectedObj>);
    EXPECT_FALSE(is_in_class_reflected_v<UnownedObj1>);
    EXPECT_FALSE(is_in_class_reflected_v<UnownedObj2>);
    EXPECT_TRUE(is_in_class_reflected_v<OwnedObj>);
}

TEST(ReflectionTest, IsPrivateReflected)
{
    EXPECT_FALSE(is_private_reflected<does_not_have_reflected_class>::value);
    EXPECT_FALSE(is_private_reflected<does_have_reflected_class>::value);
    EXPECT_FALSE(is_private_reflected<is_reflected_REFLECT>::value);
    EXPECT_FALSE(is_private_reflected<is_reflected_REFLECT_NOTED>::value);
    EXPECT_TRUE(is_private_reflected<is_reflected_REFLECT_PRIVATE_EMPTY>::value);
    EXPECT_TRUE(is_private_reflected<is_reflected_REFLECT_PRIVATE>::value);
    EXPECT_TRUE(is_private_reflected<is_reflected_REFLECT_PRIVATE_NOTED>::value);
    EXPECT_FALSE(is_private_reflected<is_reflected_empty_aggregate>::value);
    EXPECT_FALSE(is_private_reflected<is_reflected_aggregate_int>::value);
    EXPECT_FALSE(is_private_reflected<is_reflected_aggregate_int_int>::value);
    EXPECT_FALSE(is_private_reflected<is_reflected_aggregate_intray>::value);
    EXPECT_FALSE(is_private_reflected<is_reflected_aggregate_intray_intray>::value);
    EXPECT_FALSE(is_private_reflected<UnreflectedObj>::value);
    EXPECT_FALSE(is_private_reflected<UnownedObj1>::value);
    EXPECT_FALSE(is_private_reflected<UnownedObj2>::value);
    EXPECT_FALSE(is_private_reflected<OwnedObj>::value);

    EXPECT_FALSE(is_private_reflected_v<does_not_have_reflected_class>);
    EXPECT_FALSE(is_private_reflected_v<does_have_reflected_class>);
    EXPECT_FALSE(is_private_reflected_v<is_reflected_REFLECT>);
    EXPECT_FALSE(is_private_reflected_v<is_reflected_REFLECT_NOTED>);
    EXPECT_TRUE(is_private_reflected_v<is_reflected_REFLECT_PRIVATE_EMPTY>);
    EXPECT_TRUE(is_private_reflected_v<is_reflected_REFLECT_PRIVATE>);
    EXPECT_TRUE(is_private_reflected_v<is_reflected_REFLECT_PRIVATE_NOTED>);
    EXPECT_FALSE(is_private_reflected_v<is_reflected_empty_aggregate>);
    EXPECT_FALSE(is_private_reflected_v<is_reflected_aggregate_int>);
    EXPECT_FALSE(is_private_reflected_v<is_reflected_aggregate_int_int>);
    EXPECT_FALSE(is_private_reflected_v<is_reflected_aggregate_intray>);
    EXPECT_FALSE(is_private_reflected_v<is_reflected_aggregate_intray_intray>);
    EXPECT_FALSE(is_private_reflected_v<UnreflectedObj>);
    EXPECT_FALSE(is_private_reflected_v<UnownedObj1>);
    EXPECT_FALSE(is_private_reflected_v<UnownedObj2>);
    EXPECT_FALSE(is_private_reflected_v<OwnedObj>);
}

#ifndef RARE_NO_CPP_20
TEST(ReflectionTest, IsAggregateReflected)
{
    EXPECT_FALSE(is_aggregate_reflected<does_not_have_reflected_class>::value);
    EXPECT_FALSE(is_aggregate_reflected<does_have_reflected_class>::value);
    EXPECT_FALSE(is_aggregate_reflected<is_reflected_REFLECT>::value);
    EXPECT_FALSE(is_aggregate_reflected<is_reflected_REFLECT_NOTED>::value);
    EXPECT_FALSE(is_aggregate_reflected<is_reflected_REFLECT_PRIVATE_EMPTY>::value);
    EXPECT_FALSE(is_aggregate_reflected<is_reflected_REFLECT_PRIVATE>::value);
    EXPECT_FALSE(is_aggregate_reflected<is_reflected_REFLECT_PRIVATE_NOTED>::value);
    EXPECT_TRUE(is_aggregate_reflected<is_reflected_empty_aggregate>::value);
    EXPECT_TRUE(is_aggregate_reflected<is_reflected_aggregate_int>::value);
    EXPECT_TRUE(is_aggregate_reflected<is_reflected_aggregate_int_int>::value);
    EXPECT_TRUE(is_aggregate_reflected<is_reflected_aggregate_intray>::value);
    EXPECT_TRUE(is_aggregate_reflected<is_reflected_aggregate_intray_intray>::value);
    EXPECT_FALSE(is_aggregate_reflected<UnreflectedObj>::value);
    EXPECT_FALSE(is_aggregate_reflected<UnownedObj1>::value);
    EXPECT_FALSE(is_aggregate_reflected<UnownedObj2>::value);
    EXPECT_FALSE(is_aggregate_reflected<OwnedObj>::value);
    bool isStdArrayAggregate = is_aggregate_reflected<std::array<int, 2>>::value;
    EXPECT_FALSE(isStdArrayAggregate);

    EXPECT_FALSE(is_aggregate_reflected_v<does_not_have_reflected_class>);
    EXPECT_FALSE(is_aggregate_reflected_v<does_have_reflected_class>);
    EXPECT_FALSE(is_aggregate_reflected_v<is_reflected_REFLECT>);
    EXPECT_FALSE(is_aggregate_reflected_v<is_reflected_REFLECT_NOTED>);
    EXPECT_FALSE(is_aggregate_reflected_v<is_reflected_REFLECT_PRIVATE_EMPTY>);
    EXPECT_FALSE(is_aggregate_reflected_v<is_reflected_REFLECT_PRIVATE>);
    EXPECT_FALSE(is_aggregate_reflected_v<is_reflected_REFLECT_PRIVATE_NOTED>);
    EXPECT_TRUE(is_aggregate_reflected_v<is_reflected_empty_aggregate>);
    EXPECT_TRUE(is_aggregate_reflected_v<is_reflected_aggregate_int>);
    EXPECT_TRUE(is_aggregate_reflected_v<is_reflected_aggregate_int_int>);
    EXPECT_TRUE(is_aggregate_reflected_v<is_reflected_aggregate_intray>);
    EXPECT_TRUE(is_aggregate_reflected_v<is_reflected_aggregate_intray_intray>);
    EXPECT_FALSE(is_aggregate_reflected_v<UnreflectedObj>);
    EXPECT_FALSE(is_aggregate_reflected_v<UnownedObj1>);
    EXPECT_FALSE(is_aggregate_reflected_v<UnownedObj2>);
    EXPECT_FALSE(is_aggregate_reflected_v<OwnedObj>);
    isStdArrayAggregate = is_aggregate_reflected_v<std::array<int, 2>>;
    EXPECT_FALSE(isStdArrayAggregate);
}
#endif

TEST(ReflectionTest, IsReflected)
{
    EXPECT_FALSE(is_reflected<does_not_have_reflected_class>::value);
    EXPECT_TRUE(is_reflected<does_have_reflected_class>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT_NOTED>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT_PRIVATE_EMPTY>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT_PRIVATE>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT_PRIVATE_NOTED>::value);
#ifdef RARE_NO_CPP_20
    EXPECT_FALSE(is_reflected<is_reflected_empty_aggregate>::value);
    EXPECT_FALSE(is_reflected<is_reflected_aggregate_int>::value);
    EXPECT_FALSE(is_reflected<is_reflected_aggregate_int_int>::value);
    EXPECT_FALSE(is_reflected<is_reflected_aggregate_intray>::value);
    EXPECT_FALSE(is_reflected<is_reflected_aggregate_intray_intray>::value);
#else
    EXPECT_TRUE(is_reflected<is_reflected_empty_aggregate>::value);
    EXPECT_TRUE(is_reflected<is_reflected_aggregate_int>::value);
    EXPECT_TRUE(is_reflected<is_reflected_aggregate_int_int>::value);
    EXPECT_TRUE(is_reflected<is_reflected_aggregate_intray>::value);
    EXPECT_TRUE(is_reflected<is_reflected_aggregate_intray_intray>::value);
#endif
    EXPECT_FALSE(is_reflected<UnreflectedObj>::value);
    EXPECT_TRUE(is_reflected<UnownedObj1>::value);
    EXPECT_TRUE(is_reflected<UnownedObj2>::value);
    EXPECT_TRUE(is_reflected<OwnedObj>::value);

    EXPECT_FALSE(is_reflected_v<does_not_have_reflected_class>);
    EXPECT_TRUE(is_reflected_v<does_have_reflected_class>);
    EXPECT_TRUE(is_reflected_v<is_reflected_REFLECT>);
    EXPECT_TRUE(is_reflected_v<is_reflected_REFLECT_NOTED>);
    EXPECT_TRUE(is_reflected_v<is_reflected_REFLECT_PRIVATE_EMPTY>);
    EXPECT_TRUE(is_reflected_v<is_reflected_REFLECT_PRIVATE>);
    EXPECT_TRUE(is_reflected_v<is_reflected_REFLECT_PRIVATE_NOTED>);
#ifdef RARE_NO_CPP_20
    EXPECT_FALSE(is_reflected_v<is_reflected_empty_aggregate>);
    EXPECT_FALSE(is_reflected_v<is_reflected_aggregate_int>);
    EXPECT_FALSE(is_reflected_v<is_reflected_aggregate_int_int>);
    EXPECT_FALSE(is_reflected_v<is_reflected_aggregate_intray>);
    EXPECT_FALSE(is_reflected_v<is_reflected_aggregate_intray_intray>);
#else
    EXPECT_TRUE(is_reflected_v<is_reflected_empty_aggregate>);
    EXPECT_TRUE(is_reflected_v<is_reflected_aggregate_int>);
    EXPECT_TRUE(is_reflected_v<is_reflected_aggregate_int_int>);
    EXPECT_TRUE(is_reflected_v<is_reflected_aggregate_intray>);
    EXPECT_TRUE(is_reflected_v<is_reflected_aggregate_intray_intray>);
#endif
    EXPECT_FALSE(is_reflected_v<UnreflectedObj>);
    EXPECT_TRUE(is_reflected_v<UnownedObj1>);
    EXPECT_TRUE(is_reflected_v<UnownedObj2>);
    EXPECT_TRUE(is_reflected_v<OwnedObj>);
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

NOTE(ReflectionObj, Super<ReflectSuperObj>)
class ReflectionObj : public ReflectSuperObj {
public:
    ReflectionObj() : primitive(0), object({}), primitiveReference(primitive) { primitiveArray[0] = 0; primitiveArray[1] = 1; }

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

    REFLECT_NOTED(ReflectionObj, primitive, object, primitiveArray, map, objCollection, stack, staticPrimitive, primitiveReference, staticPrimitiveReference, memberMethod, staticMethod)
};

int ReflectionObj::staticPrimitive = 0;
int & ReflectionObj::staticPrimitiveReference = ReflectionObj::staticPrimitive;

TEST(ReflectionTest, ClassT)
{
    bool isSame = std::is_same_v<RareTs::Class::class_t<UnownedObj1>, typename RareTs::Proxy<UnownedObj1>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Class::class_t<UnownedObj2>, typename RareTs::Proxy<UnownedObj2>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::Class::class_t<OwnedObj>, typename OwnedObj::Class>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectionTest, ClassNotesT)
{
    bool isSame = std::is_same_v<RareTs::remove_cvref_t<decltype(ReflectionObj::Class::notes)>, Class::class_notes_t<ReflectionObj>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::NoNote, std::remove_const_t<Class::class_notes_t<ReflectSuperObj>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::NoNote, std::remove_const_t<Class::class_notes_t<is_reflected_REFLECT_EMPTY>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<RareTs::NoNote, std::remove_const_t<Class::class_notes_t<does_not_have_reflected_class>>>;
    EXPECT_TRUE(isSame);
}

struct TechnicallyAMember { using type = void; using Notes = void; static constexpr size_t index = 0;};
struct NotAMember{};
struct MissingIndex { using type = void; using Notes = void; };
struct MissingNotes { using type = void; static constexpr size_t index = 0; };
struct MissingType { using Notes = void; static constexpr size_t index = 0; };

TEST(ReflectionTest, IsMember)
{
    using SuperValMember = RareTs::Member<ReflectSuperObj, 0>;
    EXPECT_TRUE(RareTs::is_member_v<SuperValMember>);
    EXPECT_TRUE(RareTs::is_member_v<TechnicallyAMember>);
    EXPECT_FALSE(RareTs::is_member_v<NotAMember>);
    EXPECT_FALSE(RareTs::is_member_v<MissingIndex>);
    EXPECT_FALSE(RareTs::is_member_v<MissingNotes>);
    EXPECT_FALSE(RareTs::is_member_v<MissingType>);
}

struct MemberFuncs
{
    void memberFunc() {}
    void memberFuncNoExcept() noexcept {}
    void memberFuncConst() const {}
    void overload() {}
    void overload() const {}
    void overload(int) {}
    friend void friendFunc();
    static void staticFunc();
    static void staticFuncNoExcept() noexcept;
};

void freeFunc() {}
void friendFunc() {}
void MemberFuncs::staticFunc() {}
void MemberFuncs::staticFuncNoExcept() noexcept {}

TEST(ReflectionTest, IsInstanceMethod)
{
    EXPECT_TRUE(RareTs::is_instance_method_v<decltype(&MemberFuncs::memberFunc)>);
    EXPECT_TRUE(RareTs::is_instance_method_v<decltype(&MemberFuncs::memberFuncNoExcept)>);
    EXPECT_TRUE(RareTs::is_instance_method_v<decltype(&MemberFuncs::memberFuncConst)>);
    EXPECT_TRUE(RareTs::is_instance_method_v<decltype(static_cast<void(MemberFuncs::*)()>(&MemberFuncs::overload))>);
    EXPECT_TRUE(RareTs::is_instance_method_v<decltype(static_cast<void(MemberFuncs::*)() const>(&MemberFuncs::overload))>);
    EXPECT_TRUE(RareTs::is_instance_method_v<decltype(static_cast<void(MemberFuncs::*)(int)>(&MemberFuncs::overload))>);
    EXPECT_FALSE(RareTs::is_instance_method_v<decltype(&freeFunc)>);
    EXPECT_FALSE(RareTs::is_instance_method_v<decltype(&friendFunc)>);
    EXPECT_FALSE(RareTs::is_instance_method_v<decltype(&MemberFuncs::staticFunc)>);
    EXPECT_FALSE(RareTs::is_instance_method_v<decltype(&MemberFuncs::staticFuncNoExcept)>);
}

struct SubNotesSuper {};
struct SubNotesOtherSuper {};

NOTE(SubNotesSubClass, 0, RareTs::Super<SubNotesSuper>)
struct SubNotesSubClass : SubNotesSuper {

    NOTE(ovl,
        RareTs::Overload<>,
        RareTs::Overload<int>)
    void ovl() {}
    float ovl(int) { return 1.1f; }

    REFLECT_NOTED(SubNotesSubClass, ovl)
};

NOTE(ValuedSubNotesSubClass, 5, 6,
    RareTs::Super<SubNotesSuper>(0, 'a', 'b'), RareTs::Super<SubNotesOtherSuper>('a', 1, 'b'))
struct ValuedSubNotesSubClass : SubNotesSuper, SubNotesOtherSuper {

    NOTE(ovl,
        RareTs::Overload<>('c', 1, 'd'),
        7,
        RareTs::Overload<int>('e', 'f', 2))
    void ovl() {}
    float ovl(int) { return 1.1f; }

    REFLECT_NOTED(ValuedSubNotesSubClass, ovl)
};

TEST(ReflectionTest, SubNotes)
{
    using Supers = RareTs::Reflection::Inheritance::detail::SuperNotes<SubNotesSubClass>;
    constexpr auto & emptySuper = RareTs::sub_notes<Supers, std::integral_constant<size_t, 0>>::value;
    bool isSame = std::is_same_v<std::tuple<>, RareTs::remove_cvref_t<decltype(emptySuper)>>;
    EXPECT_TRUE(isSame);

    using ValuedSupers = RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>;
    constexpr auto & valuedSuper = RareTs::sub_notes<ValuedSupers, std::integral_constant<size_t, 0>>::value;
    constexpr auto & otherValuedSuper = RareTs::sub_notes<ValuedSupers, std::integral_constant<size_t, 1>>::value;
    isSame = std::is_same_v<std::tuple<int, char, char>, RareTs::remove_cvref_t<decltype(valuedSuper)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<0>(valuedSuper));
    EXPECT_EQ('a', std::get<1>(valuedSuper));
    EXPECT_EQ('b', std::get<2>(valuedSuper));
    isSame = std::is_same_v<std::tuple<char, int, char>, RareTs::remove_cvref_t<decltype(otherValuedSuper)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', std::get<0>(otherValuedSuper));
    EXPECT_EQ(1, std::get<1>(otherValuedSuper));
    EXPECT_EQ('b', std::get<2>(otherValuedSuper));
    
    using NoArgOvlNoteWrapper = RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>;
    using IntOvlNoteWrapper = RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>;
    constexpr auto & emptyNoArgOvl = RareTs::sub_notes<NoArgOvlNoteWrapper, std::integral_constant<size_t, 0>>::value;
    constexpr auto & emptyIntArgOvl = RareTs::sub_notes<IntOvlNoteWrapper, std::integral_constant<size_t, 1>>::value;
    isSame = std::is_same_v<std::tuple<>, RareTs::remove_cvref_t<decltype(emptyNoArgOvl)>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<>, RareTs::remove_cvref_t<decltype(emptyIntArgOvl)>>;
    EXPECT_TRUE(isSame);
    
    using ValuedNoArgOvlNoteWrapper = RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>;
    using ValuedIntArgOvlNoteWrapper = RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>;
    constexpr auto & valuedNoArgOvl = RareTs::sub_notes<ValuedNoArgOvlNoteWrapper, std::integral_constant<size_t, 0>>::value;
    constexpr auto & valuedIntArgOvl = RareTs::sub_notes<ValuedIntArgOvlNoteWrapper, std::integral_constant<size_t, 2>>::value;
    isSame = std::is_same_v<std::tuple<char, int, char>, RareTs::remove_cvref_t<decltype(valuedNoArgOvl)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('c', std::get<0>(valuedNoArgOvl));
    EXPECT_EQ(1, std::get<1>(valuedNoArgOvl));
    EXPECT_EQ('d', std::get<2>(valuedNoArgOvl));
    isSame = std::is_same_v<std::tuple<char, char, int>, RareTs::remove_cvref_t<decltype(valuedIntArgOvl)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('e', std::get<0>(valuedIntArgOvl));
    EXPECT_EQ('f', std::get<1>(valuedIntArgOvl));
    EXPECT_EQ(2, std::get<2>(valuedIntArgOvl));
}

TEST(ReflectionTest, AnnotationsTypeNotes)
{
    using ClassNotes = RareTs::AnnotationsType<RareTs::Class::class_t<ValuedSubNotesSubClass>, void, 0, 1, 2, 3>;
    constexpr auto & classNotes = ClassNotes::notes;
    bool isSame = std::is_same_v<
        const std::tuple<int, int, RareTs::NotedSuper<SubNotesSuper, int, char, char>, RareTs::NotedSuper<SubNotesOtherSuper, char, int, char>> &,
        decltype(classNotes)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(5, std::get<0>(classNotes));
    EXPECT_EQ(6, std::get<1>(classNotes));

    using FieldNotes = RareTs::AnnotationsType<typename RareTs::Class::template member_note_wrapper<ValuedSubNotesSubClass, 0>, void, 0, 1, 2>;
    constexpr auto & fieldNotes = FieldNotes::notes;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<
        const std::tuple<RareTs::NotedOverload<std::tuple<>, char, int, char>, int, RareTs::NotedOverload<std::tuple<int>, char, char, int>> &,
        decltype(fieldNotes)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(7, std::get<1>(fieldNotes));

    using Supers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<SubNotesSubClass>, std::integral_constant<size_t, 0>>;
    constexpr auto & emptySuper = Supers::notes;
    isSame = std::is_same_v<std::tuple<>, RareTs::remove_cvref_t<decltype(emptySuper)>>;
    EXPECT_TRUE(isSame);

    using ValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using OtherValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 1>, 0, 1, 2>;
    constexpr auto & valuedSuper = ValuedSupers::notes;
    constexpr auto & otherValuedSuper = OtherValuedSupers::notes;
    isSame = std::is_same_v<std::tuple<int, char, char>, RareTs::remove_cvref_t<decltype(valuedSuper)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<0>(valuedSuper));
    EXPECT_EQ('a', std::get<1>(valuedSuper));
    EXPECT_EQ('b', std::get<2>(valuedSuper));
    isSame = std::is_same_v<std::tuple<char, int, char>, RareTs::remove_cvref_t<decltype(otherValuedSuper)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('a', std::get<0>(otherValuedSuper));
    EXPECT_EQ(1, std::get<1>(otherValuedSuper));
    EXPECT_EQ('b', std::get<2>(otherValuedSuper));

    using NoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>, std::integral_constant<size_t, 0>>;
    using IntOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>, std::integral_constant<size_t, 1>>;
    constexpr auto & emptyNoArgOvl = NoArgOvlNoteWrapper::notes;
    constexpr auto & emptyIntArgOvl = IntOvlNoteWrapper::notes;
    isSame = std::is_same_v<std::tuple<>, RareTs::remove_cvref_t<decltype(emptyNoArgOvl)>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<>, RareTs::remove_cvref_t<decltype(emptyIntArgOvl)>>;
    EXPECT_TRUE(isSame);

    using ValuedNoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using ValuedIntArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 2>, 0, 1, 2>;
    constexpr auto & valuedNoArgOvl = ValuedNoArgOvlNoteWrapper::notes;
    constexpr auto & valuedIntArgOvl = ValuedIntArgOvlNoteWrapper::notes;
    isSame = std::is_same_v<std::tuple<char, int, char>, RareTs::remove_cvref_t<decltype(valuedNoArgOvl)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('c', std::get<0>(valuedNoArgOvl));
    EXPECT_EQ(1, std::get<1>(valuedNoArgOvl));
    EXPECT_EQ('d', std::get<2>(valuedNoArgOvl));
    isSame = std::is_same_v<std::tuple<char, char, int>, RareTs::remove_cvref_t<decltype(valuedIntArgOvl)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ('e', std::get<0>(valuedIntArgOvl));
    EXPECT_EQ('f', std::get<1>(valuedIntArgOvl));
    EXPECT_EQ(2, std::get<2>(valuedIntArgOvl));
}

TEST(ReflectionTest, AnnotationsTypeNotesType)
{
    using ClassNotes = RareTs::AnnotationsType<RareTs::Class::class_t<ValuedSubNotesSubClass>, void, 0, 1, 2, 3>;
    bool isSame = std::is_same_v<
        std::tuple<int, int, RareTs::NotedSuper<SubNotesSuper, int, char, char>, RareTs::NotedSuper<SubNotesOtherSuper, char, int, char>>,
        typename ClassNotes::Notes>;
    EXPECT_TRUE(isSame);

    using FieldNotes = RareTs::AnnotationsType<typename RareTs::Class::template member_note_wrapper<ValuedSubNotesSubClass, 0>, void, 0, 1, 2>;
    isSame = std::is_same_v<
        std::tuple<RareTs::NotedOverload<std::tuple<>, char, int, char>, int, RareTs::NotedOverload<std::tuple<int>, char, char, int>>,
        typename FieldNotes::Notes>;
    EXPECT_TRUE(isSame);

    using Supers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<SubNotesSubClass>, std::integral_constant<size_t, 0>>;
    isSame = std::is_same_v<std::tuple<>, typename Supers::Notes>;
    EXPECT_TRUE(isSame);

    using ValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using OtherValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 1>, 0, 1, 2>;
    isSame = std::is_same_v<std::tuple<int, char, char>, typename ValuedSupers::Notes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char, int, char>, typename OtherValuedSupers::Notes>;
    EXPECT_TRUE(isSame);
    
    using NoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>, std::integral_constant<size_t, 0>>;
    using IntOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>, std::integral_constant<size_t, 1>>;
    isSame = std::is_same_v<std::tuple<>, typename NoArgOvlNoteWrapper::Notes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<>, typename IntOvlNoteWrapper::Notes>;
    EXPECT_TRUE(isSame);
    
    using ValuedNoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using ValuedIntArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 2>, 0, 1, 2>;
    isSame = std::is_same_v<std::tuple<char, int, char>, typename ValuedNoArgOvlNoteWrapper::Notes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char, char, int>, typename ValuedIntArgOvlNoteWrapper::Notes>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectionTest, AnnotationsTypeHasNotes)
{
    using ClassNotes = RareTs::AnnotationsType<RareTs::Class::class_t<ValuedSubNotesSubClass>, void, 0, 1, 2, 3>;
    EXPECT_TRUE(ClassNotes::hasNote<int>());
    EXPECT_FALSE(ClassNotes::hasNote<char>());
    EXPECT_TRUE(ClassNotes::hasNote<RareTs::NotedSuper>());
    bool hasNote = ClassNotes::hasNote<RareTs::NotedSuper<SubNotesSuper, int, char, char>>();
    EXPECT_TRUE(hasNote);
    hasNote = ClassNotes::hasNote<RareTs::NotedSuper<SubNotesOtherSuper, char, int, char>>();
    EXPECT_TRUE(hasNote);
    hasNote = ClassNotes::hasNote<RareTs::NotedSuper<SubNotesSuper, char, char, int>>();
    EXPECT_FALSE(hasNote);

    using FieldNotes = RareTs::AnnotationsType<typename RareTs::Class::template member_note_wrapper<ValuedSubNotesSubClass, 0>, void, 0, 1, 2>;
    EXPECT_TRUE(FieldNotes::hasNote<int>());
    EXPECT_FALSE(FieldNotes::hasNote<char>());
    EXPECT_TRUE(FieldNotes::hasNote<RareTs::NotedOverload>());
    hasNote = FieldNotes::hasNote<RareTs::NotedOverload<std::tuple<>, char, int, char>>();
    EXPECT_TRUE(hasNote);
    hasNote = FieldNotes::hasNote<RareTs::NotedOverload<std::tuple<int>, char, char, int>>();
    EXPECT_TRUE(hasNote);
    hasNote = FieldNotes::hasNote<RareTs::NotedOverload<SubNotesSuper, int, char, char>>();
    EXPECT_FALSE(hasNote);

    using Supers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<SubNotesSubClass>, std::integral_constant<size_t, 0>>;
    EXPECT_FALSE(Supers::hasNote<int>());
    EXPECT_FALSE(Supers::hasNote<char>());
    EXPECT_FALSE(Supers::hasNote<RareTs::SuperClass>());
    EXPECT_FALSE(Supers::hasNote<RareTs::NotedSuper>());

    using ValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using OtherValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 1>, 0, 1, 2>;
    EXPECT_TRUE(ValuedSupers::hasNote<int>());
    EXPECT_TRUE(ValuedSupers::hasNote<char>());
    EXPECT_FALSE(ValuedSupers::hasNote<long>());
    EXPECT_TRUE(OtherValuedSupers::hasNote<int>());
    EXPECT_TRUE(OtherValuedSupers::hasNote<char>());
    EXPECT_FALSE(OtherValuedSupers::hasNote<long>());
    
    using NoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>, std::integral_constant<size_t, 0>>;
    using IntOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>, std::integral_constant<size_t, 1>>;
    EXPECT_FALSE(NoArgOvlNoteWrapper::hasNote<int>());
    EXPECT_FALSE(NoArgOvlNoteWrapper::hasNote<char>());
    EXPECT_FALSE(NoArgOvlNoteWrapper::hasNote<long>());
    EXPECT_FALSE(IntOvlNoteWrapper::hasNote<int>());
    EXPECT_FALSE(IntOvlNoteWrapper::hasNote<char>());
    EXPECT_FALSE(IntOvlNoteWrapper::hasNote<long>());
    
    using ValuedNoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using ValuedIntArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 2>, 0, 1, 2>;
    EXPECT_TRUE(ValuedNoArgOvlNoteWrapper::hasNote<int>());
    EXPECT_TRUE(ValuedNoArgOvlNoteWrapper::hasNote<char>());
    EXPECT_FALSE(ValuedNoArgOvlNoteWrapper::hasNote<long>());
    EXPECT_TRUE(ValuedIntArgOvlNoteWrapper::hasNote<int>());
    EXPECT_TRUE(ValuedIntArgOvlNoteWrapper::hasNote<char>());
    EXPECT_FALSE(ValuedIntArgOvlNoteWrapper::hasNote<long>());
}

TEST(ReflectionTest, AnnotationsTypeGetNote)
{
    using ClassNotes = RareTs::AnnotationsType<RareTs::Class::class_t<ValuedSubNotesSubClass>, void, 0, 1, 2, 3>;
    EXPECT_EQ(5, ClassNotes::getNote<int>());

    using FieldNotes = RareTs::AnnotationsType<typename RareTs::Class::template member_note_wrapper<ValuedSubNotesSubClass, 0>, void, 0, 1, 2>;
    EXPECT_EQ(7, FieldNotes::getNote<int>());

    using ValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using OtherValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 1>, 0, 1, 2>;
    EXPECT_EQ(0, ValuedSupers::getNote<int>());
    EXPECT_EQ('a', ValuedSupers::getNote<char>());
    EXPECT_EQ(1, OtherValuedSupers::getNote<int>());
    EXPECT_EQ('a', OtherValuedSupers::getNote<char>());
    
    using ValuedNoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using ValuedIntArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 2>, 0, 1, 2>;
    EXPECT_EQ(1, ValuedNoArgOvlNoteWrapper::getNote<int>());
    EXPECT_EQ('c', ValuedNoArgOvlNoteWrapper::getNote<char>());
    EXPECT_EQ(2, ValuedIntArgOvlNoteWrapper::getNote<int>());
    EXPECT_EQ('e', ValuedIntArgOvlNoteWrapper::getNote<char>());
}

TEST(ReflectionTest, AnnotationsTypeForEachNote)
{
    using ClassNotes = RareTs::AnnotationsType<RareTs::Class::class_t<ValuedSubNotesSubClass>, void, 0, 1, 2, 3>;
    size_t visitCount = 0;
    ClassNotes::forEachNote([&](auto & note) {
        bool visited = false;
        switch ( visitCount ) {
        case 0:
            if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(note)>> )
            {
                EXPECT_EQ(5, note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(note)>> )
            {
                EXPECT_EQ(6, note);
                visited = true;
            }
            break;
        case 2:
            if constexpr ( std::is_same_v<RareTs::NotedSuper<SubNotesSuper, int, char, char>, RareTs::remove_cvref_t<decltype(note)>> )
                visited = true;
            break;
        case 3:
            if constexpr ( std::is_same_v<RareTs::NotedSuper<SubNotesOtherSuper, char, int, char>, RareTs::remove_cvref_t<decltype(note)>> )
                visited = true;
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(4, visitCount);
    visitCount = 0;
    ClassNotes::forEachNote<int>([&](auto & note) {
        bool visited = false;
        switch ( visitCount ) {
        case 0:
            if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(note)>> )
            {
                EXPECT_EQ(5, note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<int, RareTs::remove_cvref_t<decltype(note)>> )
            {
                EXPECT_EQ(6, note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    ClassNotes::forEachNote<RareTs::NotedSuper>([&](auto & note) {
        bool visited = false;
        switch ( visitCount ) {
        case 0:
            if constexpr ( std::is_same_v<RareTs::NotedSuper<SubNotesSuper, int, char, char>, RareTs::remove_cvref_t<decltype(note)>> )
                visited = true;
            break;
        case 1:
            if constexpr ( std::is_same_v<RareTs::NotedSuper<SubNotesOtherSuper, char, int, char>, RareTs::remove_cvref_t<decltype(note)>> )
                visited = true;
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(2, visitCount);

    using FieldNotes = RareTs::AnnotationsType<typename RareTs::Class::template member_note_wrapper<ValuedSubNotesSubClass, 0>, void, 0, 1, 2>;
    visitCount = 0;
    FieldNotes::forEachNote([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<RareTs::NotedOverload<std::tuple<>, char, int, char>, Note> )
            {
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(7, note);
                visited = true;
            }
            break;
        case 2:
            if constexpr ( std::is_same_v<RareTs::NotedOverload<std::tuple<int>, char, char, int>, Note> )
            {
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    FieldNotes::forEachNote<int>([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(7, note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    FieldNotes::forEachNote<RareTs::NotedOverload>([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<RareTs::NotedOverload<std::tuple<>, char, int, char>, Note> )
            {
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<RareTs::NotedOverload<std::tuple<int>, char, char, int>, Note> )
            {
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(2, visitCount);

    using Supers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<SubNotesSubClass>, std::integral_constant<size_t, 0>>;
    visitCount = 0;
    Supers::forEachNote([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);

    using ValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using OtherValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 1>, 0, 1, 2>;
    visitCount = 0;
    ValuedSupers::forEachNote([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(0, note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('a', note);
                visited = true;
            }
            break;
        case 2:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('b', note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    ValuedSupers::forEachNote<int>([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(0, note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    ValuedSupers::forEachNote<char>([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('a', note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('b', note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    ValuedSupers::forEachNote<RareTs::NotedOverload>([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    OtherValuedSupers::forEachNote([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('a', note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(1, note);
                visited = true;
            }
            break;
        case 2:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('b', note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(3, visitCount);
    
    using NoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>, std::integral_constant<size_t, 0>>;
    using IntOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>, std::integral_constant<size_t, 1>>;
    visitCount = 0;
    NoArgOvlNoteWrapper::forEachNote([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    IntOvlNoteWrapper::forEachNote([&](auto &) {
        ++visitCount;
    });
    EXPECT_EQ(0, visitCount);
    
    using ValuedNoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using ValuedIntArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>, std::integral_constant<size_t, 2>, 0, 1, 2>;
    visitCount = 0;
    ValuedNoArgOvlNoteWrapper::forEachNote([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('c', note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(1, note);
                visited = true;
            }
            break;
        case 2:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('d', note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    ValuedNoArgOvlNoteWrapper::forEachNote<int>([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(1, note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    ValuedNoArgOvlNoteWrapper::forEachNote<char>([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('c', note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('d', note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    ValuedIntArgOvlNoteWrapper::forEachNote([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('e', note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('f', note);
                visited = true;
            }
            break;
        case 2:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(2, note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    ValuedIntArgOvlNoteWrapper::forEachNote<int>([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<int, Note> )
            {
                EXPECT_EQ(2, note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    ValuedIntArgOvlNoteWrapper::forEachNote<char>([&](auto & note) {
        using Note = RareTs::remove_cvref_t<decltype(note)>;
        bool visited = false;
        switch ( visitCount )
        {
        case 0:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('e', note);
                visited = true;
            }
            break;
        case 1:
            if constexpr ( std::is_same_v<char, Note> )
            {
                EXPECT_EQ('f', note);
                visited = true;
            }
            break;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(2, visitCount);
}

TEST(ReflectionTest, AnnotationsTypeDeductionHelpers)
{
    using ClassNotes = RareTs::AnnotationsType<RareTs::Class::class_t<ValuedSubNotesSubClass>, void, 0, 1, 2, 3>;
    using ClassNotesDeduction = decltype(RareTs::Reflection::Annotations::detail::annotationsType<Class::class_t<ValuedSubNotesSubClass>>(std::make_index_sequence<4>()));
    bool isSame = std::is_same_v<ClassNotes, ClassNotesDeduction>;
    EXPECT_TRUE(isSame);

    using FieldNotes = RareTs::AnnotationsType<typename RareTs::Class::template member_note_wrapper<ValuedSubNotesSubClass, 0>, void, 0, 1, 2>;
    using FieldNotesDeduction = RareTs::Class::MemberAnnotationsType<ValuedSubNotesSubClass, 0>;
    isSame = std::is_same_v<FieldNotes, FieldNotesDeduction>;
    EXPECT_TRUE(isSame);

    using Supers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<SubNotesSubClass>, std::integral_constant<size_t, 0>>;
    using SupersNotez = RareTs::Reflection::Inheritance::detail::SuperNotes<SubNotesSubClass>;
    using SupersNotes = RareTs::remove_cvref_t<decltype(SupersNotez::notes)>;
    using SupersDeduction = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<SupersNotez, 0>(
                        std::make_index_sequence<std::tuple_size_v<typename std::remove_reference_t<std::tuple_element_t<0, SupersNotes>>::Notes>>()));
    isSame = std::is_same_v<Supers, SupersDeduction>;
    EXPECT_TRUE(isSame);

    using ValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using ValuedSupersNotez = RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>;
    using ValuedSupersNotes = RareTs::remove_cvref_t<decltype(ValuedSupersNotez::notes)>;
    using ValuedSupersDeduction = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<ValuedSupersNotez, 0>(
                        std::make_index_sequence<std::tuple_size_v<typename std::remove_reference_t<std::tuple_element_t<0, ValuedSupersNotes>>::Notes>>()));
    isSame = std::is_same_v<ValuedSupers, ValuedSupersDeduction>;
    EXPECT_TRUE(isSame);
    using OtherValuedSupers = RareTs::AnnotationsType<RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>, std::integral_constant<size_t, 1>, 0, 1, 2>;
    using OtherValuedSupersNotez = RareTs::Reflection::Inheritance::detail::SuperNotes<ValuedSubNotesSubClass>;
    using OtherValuedSupersNotes = RareTs::remove_cvref_t<decltype(OtherValuedSupersNotez::notes)>;
    using OtherValuedSupersDeduction = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<OtherValuedSupersNotez, 1>(
                        std::make_index_sequence<std::tuple_size_v<typename std::remove_reference_t<std::tuple_element_t<1, OtherValuedSupersNotes>>::Notes>>()));
    isSame = std::is_same_v<OtherValuedSupers, OtherValuedSupersDeduction>;
    EXPECT_TRUE(isSame);
    
    using NoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Functions::detail::OverloadNotes<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>>, std::integral_constant<size_t, 0>>;
    using NoArgOvlNoteWrapperNotez = RareTs::Functions::detail::OverloadNotes<typename RareTs::Class::template member_note_wrapper<SubNotesSubClass, 0>>;
    using NoArgOvlNoteWrapperNotes = RareTs::remove_cvref_t<decltype(NoArgOvlNoteWrapperNotez::notes)>;
    using NoArgOvlNoteWrapperNote = std::remove_reference_t<std::tuple_element_t<0, NoArgOvlNoteWrapperNotes>>;
    using NoArgOvlNoteWrapperDeduction = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<NoArgOvlNoteWrapperNotez, 0>(std::make_index_sequence<std::tuple_size_v<typename NoArgOvlNoteWrapperNote::Notes>>()));
    isSame = std::is_same_v<NoArgOvlNoteWrapper, NoArgOvlNoteWrapperDeduction>;
    EXPECT_TRUE(isSame);
    using IntOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Functions::detail::OverloadNotes<RareTs::Class::member_note_wrapper<SubNotesSubClass, 0>>, std::integral_constant<size_t, 1>>;
    using IntOvlNoteWrapperNotez = RareTs::Functions::detail::OverloadNotes<typename RareTs::Class::template member_note_wrapper<SubNotesSubClass, 0>>;
    using IntOvlNoteWrapperNotes = RareTs::remove_cvref_t<decltype(IntOvlNoteWrapperNotez::notes)>;
    using IntOvlNoteWrapperNote = std::remove_reference_t<std::tuple_element_t<1, IntOvlNoteWrapperNotes>>;
    using IntOvlNoteWrapperDeduction = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<IntOvlNoteWrapperNotez, 1>(std::make_index_sequence<std::tuple_size_v<typename IntOvlNoteWrapperNote::Notes>>()));
    isSame = std::is_same_v<IntOvlNoteWrapper, IntOvlNoteWrapperDeduction>;
    EXPECT_TRUE(isSame);
    
    using ValuedNoArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Functions::detail::OverloadNotes<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>>, std::integral_constant<size_t, 0>, 0, 1, 2>;
    using ValuedNoArgOvlNoteWrapperNotez = RareTs::Functions::detail::OverloadNotes<typename RareTs::Class::template member_note_wrapper<ValuedSubNotesSubClass, 0>>;
    using ValuedNoArgOvlNoteWrapperNotes = RareTs::remove_cvref_t<decltype(ValuedNoArgOvlNoteWrapperNotez::notes)>;
    using ValuedNoArgOvlNoteWrapperNote = std::remove_reference_t<std::tuple_element_t<0, ValuedNoArgOvlNoteWrapperNotes>>;
    using ValuedNoArgOvlNoteWrapperDeduction = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<ValuedNoArgOvlNoteWrapperNotez, 0>(std::make_index_sequence<std::tuple_size_v<typename ValuedNoArgOvlNoteWrapperNote::Notes>>()));
    isSame = std::is_same_v<ValuedNoArgOvlNoteWrapper, ValuedNoArgOvlNoteWrapperDeduction>;
    EXPECT_TRUE(isSame);
    using ValuedIntArgOvlNoteWrapper = RareTs::AnnotationsType<RareTs::Functions::detail::OverloadNotes<RareTs::Class::member_note_wrapper<ValuedSubNotesSubClass, 0>>, std::integral_constant<size_t, 2>, 0, 1, 2>;
    using ValuedIntArgOvlNoteWrapperNotez = RareTs::Functions::detail::OverloadNotes<typename RareTs::Class::template member_note_wrapper<ValuedSubNotesSubClass, 0>>;
    using ValuedIntArgOvlNoteWrapperNotes = RareTs::remove_cvref_t<decltype(ValuedIntArgOvlNoteWrapperNotez::notes)>;
    using ValuedIntArgOvlNoteWrapperNote = std::remove_reference_t<std::tuple_element_t<1, ValuedIntArgOvlNoteWrapperNotes>>;
    using ValuedIntArgOvlNoteWrapperDeduction = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<ValuedIntArgOvlNoteWrapperNotez, 2>(std::make_index_sequence<std::tuple_size_v<typename ValuedIntArgOvlNoteWrapperNote::Notes>>()));
    isSame = std::is_same_v<ValuedIntArgOvlNoteWrapper, ValuedIntArgOvlNoteWrapperDeduction>;
    EXPECT_TRUE(isSame);
}

struct TestIsOverloaded
{
    int a;
    int & b;
    static int c;
    static int & d;
    void e() {}
    void f() {}
    NOTE(g)
    void g() {}
    void g(int) {}
    NOTE(h)
    void h() {}
    void h() const {}

    REFLECT(TestIsOverloaded, a, b, c, d, e, f, g, h)
};

int TestIsOverloaded::c = 0;
int & TestIsOverloaded::d = c;

TEST(ReflectionTest, IsOverloaded)
{
    bool memberOverloaded = RareTs::is_overloaded_v<TestIsOverloaded, 0>;
    EXPECT_FALSE(memberOverloaded);
    memberOverloaded = RareTs::is_overloaded_v<TestIsOverloaded, 1>;
    EXPECT_FALSE(memberOverloaded);
    memberOverloaded = RareTs::is_overloaded_v<TestIsOverloaded, 2>;
    EXPECT_FALSE(memberOverloaded);
    memberOverloaded = RareTs::is_overloaded_v<TestIsOverloaded, 3>;
    EXPECT_FALSE(memberOverloaded);
    memberOverloaded = RareTs::is_overloaded_v<TestIsOverloaded, 4>;
    EXPECT_FALSE(memberOverloaded);
    memberOverloaded = RareTs::is_overloaded_v<TestIsOverloaded, 5>;
    EXPECT_FALSE(memberOverloaded);
    memberOverloaded = RareTs::is_overloaded_v<TestIsOverloaded, 6>;
    EXPECT_TRUE(memberOverloaded);
    memberOverloaded = RareTs::is_overloaded_v<TestIsOverloaded, 7>;
    EXPECT_TRUE(memberOverloaded);
}

class MyObjAccess {
    public:
        int aData = 0;
        int & aRef = aData;
        static int aStaticData;
        static int & aStaticRef;
        void aFunc();
        static void aStaticFunc();
        NOTE(aOvl)
        void aOvl(int);
        void aOvl(float);
    protected:
        int bData = 0;
        int & bRef = aData;
        static int bStaticData;
        static int & bStaticRef;
        void bFunc();
        static void bStaticFunc();
        NOTE(bOvl)
        void bOvl(int);
        void bOvl(float);
    private:
        int cData = 0;
        int & cRef = cData;
        static int cStaticData;
        static int & cStaticRef;
        void cFunc();
        static void cStaticFunc();
        NOTE(cOvl)
        void cOvl(int);
        void cOvl(float);

    REFLECT(MyObjAccess, aData, aRef, aStaticData, aStaticRef, aFunc, aStaticFunc, aOvl,
        bData, bRef, bStaticData, bStaticRef, bFunc, bStaticFunc, bOvl,
        cData, cRef, cStaticData, cStaticRef, cFunc, cStaticFunc, cOvl)
};

TEST(ReflectionTest, AccessModifier)
{

#ifdef __clang__
#if __clang_major__ >= 19
#define CLANG19ANDUP
#endif
#endif

    constexpr RareTs::AccessMod publicReference = // Not supported if _MSC_VER < 1932 and defaults to private
#ifdef RARE_DISABLE_REF_ACCESS
        RareTs::AccessMod::Private;
#else
        RareTs::AccessMod::Public;
#endif

#ifndef CLANG19ANDUP
    constexpr RareTs::AccessMod protectedReference = // If _MSC_VER < 1932 then not supported/defaults to private
#ifdef RARE_DISABLE_REF_ACCESS
        RareTs::AccessMod::Private;
#else
        RareTs::AccessMod::Protected;
#endif
#endif

    RareTs::AccessMod accessModifier = RareTs::access_modifier_v<MyObjAccess, 0>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 1>; // If _MSC_VER < 1932 then not supported/defaults to private
    EXPECT_EQ(publicReference, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 2>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 3>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 4>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 5>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 6>; // Overloads are not currently supported and default to private
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);

#ifdef CLANG19ANDUP
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 7>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 8>; // If _MSC_VER < 1932 then not supported/defaults to private
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 9>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 10>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 11>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 12>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 13>; // Overloads are not currently supported and default to private
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);

    accessModifier = RareTs::access_modifier_v<MyObjAccess, 14>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 15>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 16>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 17>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 18>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 19>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 20>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
#else
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 7>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 8>; // If _MSC_VER < 1932 then not supported/defaults to private
    EXPECT_EQ(protectedReference, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 9>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 10>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 11>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 12>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 13>; // Overloads are not currently supported and default to private
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);

    accessModifier = RareTs::access_modifier_v<MyObjAccess, 14>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 15>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 16>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 17>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 18>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 19>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccess, 20>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
#endif
}


class MyObjAccessPrivRefl {
    public:
        int aData = 0;
        static int aStaticData;
        void aFunc();
        static void aStaticFunc();
    protected:
        int bData = 0;
        static int bStaticData;
        void bFunc();
        static void bStaticFunc();
    private:
        int cData = 0;
        static int cStaticData;
        void cFunc();
        static void cStaticFunc();
};

REFLECT_PRIVATE(MyObjAccessPrivRefl,
    aData, aStaticData, aFunc, aStaticFunc,
    bData, bStaticData, bFunc, bStaticFunc,
    cData, cStaticData, cFunc, cStaticFunc)

TEST(ReflectionTest, PrivateReflectedAccessModifier)
{
    RareTs::AccessMod accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 0>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 1>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 2>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 3>;
    EXPECT_EQ(RareTs::AccessMod::Public, accessModifier);

    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 4>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 5>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 6>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 7>;
    EXPECT_EQ(RareTs::AccessMod::Protected, accessModifier);

    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 8>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 9>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 10>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
    accessModifier = RareTs::access_modifier_v<MyObjAccessPrivRefl, 11>;
    EXPECT_EQ(RareTs::AccessMod::Private, accessModifier);
}
