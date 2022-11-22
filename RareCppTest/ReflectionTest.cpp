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
    bool isSame = std::is_same_v<Reflection::NoAnnotation, std::tuple<>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::remove_const_t<decltype(Reflection::NoNote)>, std::tuple<>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same<std::remove_const<decltype(TriviallyNoted_note)>::type, std::tuple<>>::value;
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

TEST(ReflectionTest, NoAnnotationType)
{
    bool isSame = std::is_same<NoAnnotation, std::tuple<>>::value;
    EXPECT_TRUE(isSame);
}

struct UnreflectedObj {};

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

TEST(ReflectionTest, IsProxied)
{
    EXPECT_FALSE(is_proxied<UnreflectedObj>::value);
    EXPECT_TRUE(is_proxied<UnownedObj1>::value);
    EXPECT_TRUE(is_proxied<UnownedObj2>::value);
    EXPECT_TRUE(is_proxied<OwnedObj>::value);
}

TEST(ReflectionTest, Unproxy)
{
    bool isSame = std::is_same_v<Reflection::unproxy<UnreflectedObj>::T, UnreflectedObj>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::unproxy<UnownedObj1>::T, UnownedObj1>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::unproxy<UnownedObj2>::T, UnownedObj2>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::unproxy<OwnedObj>::T, OwnedObj>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::unproxy<Reflection::Proxy<UnownedObj1>>::T, UnownedObj1>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::unproxy<Reflection::Proxy<UnownedObj2>>::T, UnownedObj2>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::unproxy<Reflection::Proxy<OwnedObj>>::T, OwnedObj>;
    EXPECT_TRUE(isSame);
}

struct does_not_have_reflected_class {};
struct does_not_quite_have_reflected_class {
    struct Class {};
};
struct does_have_reflected_class {
    struct Class {
        static constexpr size_t Total = 0;
    };
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

TEST(ExtendedTypeSupportTest, BaseTypeHasClass)
{
    bool hasClass = decltype(typeHasReflection<does_not_have_reflected_class>(0))::value;
    EXPECT_FALSE(hasClass);
    hasClass = decltype(typeHasReflection<does_not_quite_have_reflected_class>(0))::value;
    EXPECT_FALSE(hasClass);
    hasClass = decltype(typeHasReflection<does_have_reflected_class>(0))::value;
    EXPECT_TRUE(hasClass);
    hasClass = decltype(typeHasReflection<is_reflected_REFLECT>(0))::value;
    EXPECT_TRUE(hasClass);
    hasClass = decltype(typeHasReflection<is_reflected_REFLECT_NOTED>(0))::value;
    EXPECT_TRUE(hasClass);
}

TEST(ExtendedTypeSupportTest, IsReflected)
{
    EXPECT_FALSE(is_reflected<does_not_have_reflected_class>::value);
    EXPECT_FALSE(is_reflected<does_not_quite_have_reflected_class>::value);
    EXPECT_TRUE(is_reflected<does_have_reflected_class>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT_NOTED>::value);
    EXPECT_FALSE(is_reflected<UnreflectedObj>::value);
    EXPECT_TRUE(is_reflected<UnownedObj1>::value);
    EXPECT_TRUE(is_reflected<UnownedObj2>::value);
    EXPECT_TRUE(is_reflected<OwnedObj>::value);

    EXPECT_FALSE(is_reflected_v<does_not_have_reflected_class>);
    EXPECT_FALSE(is_reflected_v<does_not_quite_have_reflected_class>);
    EXPECT_TRUE(is_reflected_v<does_have_reflected_class>);
    EXPECT_TRUE(is_reflected_v<is_reflected_REFLECT>);
    EXPECT_TRUE(is_reflected_v<is_reflected_REFLECT_NOTED>);
    EXPECT_FALSE(is_reflected_v<UnreflectedObj>);
    EXPECT_TRUE(is_reflected_v<UnownedObj1>);
    EXPECT_TRUE(is_reflected_v<UnownedObj2>);
    EXPECT_TRUE(is_reflected_v<OwnedObj>);
}

TEST(ReflectionTest, ReflectedType)
{
    bool isSame = std::is_same_v<Reflection::reflected_type<UnreflectedObj>::type, UnreflectedObj>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::reflected_type<UnownedObj1>::type, Reflection::Proxy<UnownedObj1>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::reflected_type<UnownedObj2>::type, Reflection::Proxy<UnownedObj2>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::reflected_type<OwnedObj>::type, Reflection::Proxy<OwnedObj>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<Reflection::reflected_type_t<UnreflectedObj>, UnreflectedObj>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::reflected_type_t<UnownedObj1>, Reflection::Proxy<UnownedObj1>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::reflected_type_t<UnownedObj2>, Reflection::Proxy<UnownedObj2>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::reflected_type_t<OwnedObj>, Reflection::Proxy<OwnedObj>>;
    EXPECT_TRUE(isSame);
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
    bool isSame = std::is_same_v<Reflection::clazz<UnownedObj1>::type, Reflection::Proxy<UnownedObj1>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::clazz<UnownedObj2>::type, Reflection::Proxy<UnownedObj2>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::clazz<OwnedObj>::type, Reflection::Proxy<OwnedObj>::Class>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<Reflection::class_t<UnownedObj1>, Reflection::Proxy<UnownedObj1>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::class_t<UnownedObj2>, Reflection::Proxy<UnownedObj2>::Class>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::class_t<OwnedObj>, Reflection::Proxy<OwnedObj>::Class>;
    EXPECT_TRUE(isSame);
}

TEST(ReflectionTest, ClassNotesT)
{
    bool isSame = std::is_same_v<ReflectionObj::Class::Annotations, class_notes<ReflectionObj>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::NoAnnotation, std::remove_const_t<class_notes<ReflectSuperObj>::type>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::NoAnnotation, std::remove_const_t<class_notes<is_reflected_REFLECT_EMPTY>::type>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::NoAnnotation, std::remove_const_t<class_notes<does_not_have_reflected_class>::type>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<ReflectionObj::Class::Annotations, class_notes_t<ReflectionObj>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::NoAnnotation, std::remove_const_t<class_notes_t<ReflectSuperObj>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::NoAnnotation, std::remove_const_t<class_notes_t<is_reflected_REFLECT_EMPTY>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::NoAnnotation, std::remove_const_t<class_notes_t<does_not_have_reflected_class>>>;
    EXPECT_TRUE(isSame);
}

struct ClangOnlyTest {
    static constexpr int first = 1;
    static int second;
    int third;
    
    
    template <typename T_, bool IsReference> struct GetPointer { static constexpr std::nullptr_t value = nullptr; }; \
    template <typename T_> struct GetPointer<T_, true> { static constexpr std::nullptr_t value = nullptr; }; \

    struct FirstField { static constexpr auto p = &ClangOnlyTest::first; };
    struct SecondField { static constexpr auto p = &ClangOnlyTest::second; };
    struct ThirdField { static constexpr auto p = &ClangOnlyTest::third; };

    struct first_ { STATIC_VALUE_HELPER(first) };
    struct second_ { STATIC_VALUE_HELPER(second) };
    struct third_ { STATIC_VALUE_HELPER(third) };
};

int ClangOnlyTest::second = 2;

TEST(ReflectionTest, RfMacroClangOnly)
{
#ifdef __clang__
    auto firstValue = ClangOnlyTest::first_::getStatic<true, ClangOnlyTest::FirstField, ClangOnlyTest::GetPointer<nullptr_t, true>, ClangOnlyTest>(0);
    auto secondValue = ClangOnlyTest::second_::getStatic<true, ClangOnlyTest::SecondField, ClangOnlyTest::GetPointer<nullptr_t, true>, ClangOnlyTest>(0);
    auto thirdValue = ClangOnlyTest::third_::getStatic<false, ClangOnlyTest::ThirdField, ClangOnlyTest::GetPointer<nullptr_t, true>, ClangOnlyTest>(0);
    EXPECT_EQ(1, firstValue);
    EXPECT_EQ(2, secondValue);
    std::cout << thirdValue << std::endl;
    EXPECT_EQ(&ClangOnlyTest::third, thirdValue);
#endif
}

class DescribeFieldTest {
public:
    int memberValue;

    NOTE(staticValue)
    static int staticValue;

    NOTE(memberValueReference, 'a')
    int & memberValueReference;

    NOTE(staticValueReference, '1', 1)
    static int & staticValueReference;

    void memberMethod() {}

    static void staticMethod() {}

    struct Class {
        using ProxyType = DescribeFieldTest;
        using ClassType = DescribeFieldTest;
        enum_t(IndexOf, size_t, { memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod });
        static constexpr NoAnnotation NoNote {};
        template <size_t FieldIndex, typename U_ = void> struct FieldDescr;
        FOR_EACH(DESCRIBE_FIELD, memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod)
    };
};

int DescribeFieldTest::staticValue = 0;
int & DescribeFieldTest::staticValueReference = DescribeFieldTest::staticValue;

TEST(ReflectionTest, RfMacroDescribeField)
{
    using MemberValueId = decltype(DescribeFieldTest::Class::memberValue_::identify<DescribeFieldTest>(0));
    using StaticValueId = decltype(DescribeFieldTest::Class::staticValue_::identify<DescribeFieldTest>(0));
    using MemberValueReferenceId = decltype(DescribeFieldTest::Class::memberValueReference_::identify<DescribeFieldTest>(0));
    using StaticValueReferenceId = decltype(DescribeFieldTest::Class::staticValueReference_::identify<DescribeFieldTest>(0));
    using MemberMethodId = decltype(DescribeFieldTest::Class::memberMethod_::identify<DescribeFieldTest>(0));
    using StaticMethodId = decltype(DescribeFieldTest::Class::staticMethod_::identify<DescribeFieldTest>(0));

    using MemberValueType = decltype(DescribeFieldTest::memberValue);
    using MemberValuePointer = decltype(&DescribeFieldTest::memberValue);
    using ExpectedMemberValueId = TypePair<MemberValueType, MemberValuePointer>;

    using StaticValueType = decltype(DescribeFieldTest::staticValue);
    using StaticValuePointer = decltype(&DescribeFieldTest::staticValue);
    using ExpectedStaticValueId = TypePair<StaticValueType, StaticValuePointer>;

    using MemberValueReferenceType = decltype(DescribeFieldTest::memberValueReference);
    using MemberValueReferencePointer = std::nullptr_t;
    using ExpectedMemberValueReferenceId = TypePair<MemberValueReferenceType, MemberValueReferencePointer>;

    using StaticValueReferenceType = decltype(DescribeFieldTest::staticValueReference);
    using StaticValueReferencePointer = decltype(&DescribeFieldTest::staticValueReference);
    using ExpectedStaticValueReferenceId = TypePair<StaticValueReferenceType, StaticValueReferencePointer>;

    using MemberMethodType = decltype(&DescribeFieldTest::memberMethod);
    using MemberMethodPointer = decltype(&DescribeFieldTest::memberMethod);
    using ExpectedMemberMethodId = TypePair<MemberMethodType, MemberMethodPointer>;

    using StaticMethodType = decltype(DescribeFieldTest::staticMethod);
    using StaticMethodPointer = decltype(&DescribeFieldTest::staticMethod);
    using ExpectedStaticMethodId = TypePair<StaticMethodType, StaticMethodPointer>;
    
    bool isSame = std::is_same_v<MemberValueType, DescribeFieldTest::Class::memberValue_::Type>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<MemberValuePointer, DescribeFieldTest::Class::memberValue_::Pointer>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedMemberValueId, MemberValueId>; EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<StaticValueType, DescribeFieldTest::Class::staticValue_::Type>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<StaticValuePointer, DescribeFieldTest::Class::staticValue_::Pointer>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedStaticValueId, StaticValueId>; EXPECT_TRUE(isSame);

    isSame = std::is_same_v<MemberValueReferenceType, DescribeFieldTest::Class::memberValueReference_::Type>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<MemberValueReferencePointer, DescribeFieldTest::Class::memberValueReference_::Pointer>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedMemberValueReferenceId, MemberValueReferenceId>; EXPECT_TRUE(isSame);

    isSame = std::is_same_v<StaticValueReferenceType, DescribeFieldTest::Class::staticValueReference_::Type>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<StaticValueReferencePointer, DescribeFieldTest::Class::staticValueReference_::Pointer>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedStaticValueReferenceId, StaticValueReferenceId>; EXPECT_TRUE(isSame);

    isSame = std::is_same_v<MemberMethodType, DescribeFieldTest::Class::memberMethod_::Type>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<MemberMethodPointer, DescribeFieldTest::Class::memberMethod_::Pointer>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedMemberMethodId, MemberMethodId>; EXPECT_TRUE(isSame);

    isSame = std::is_same_v<StaticMethodType, DescribeFieldTest::Class::staticMethod_::Type>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<StaticMethodPointer, DescribeFieldTest::Class::staticMethod_::Pointer>; EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedStaticMethodId, StaticMethodId>; EXPECT_TRUE(isSame);
    
    MemberValuePointer memberValuePointer = DescribeFieldTest::Class::memberValue_::GetPointer<DescribeFieldTest, false>::value;
    StaticValuePointer staticValuePointer = DescribeFieldTest::Class::staticValue_::GetPointer<DescribeFieldTest, false>::value;
    MemberValueReferencePointer memberValueReferencePointer = DescribeFieldTest::Class::memberValueReference_::GetPointer<DescribeFieldTest, true>::value;
    StaticValueReferencePointer staticValueReferencePointer = DescribeFieldTest::Class::staticValueReference_::GetPointer<DescribeFieldTest, true>::value;
    MemberMethodPointer memberMethodPointer = DescribeFieldTest::Class::memberMethod_::GetPointer<DescribeFieldTest, false>::value;
    StaticMethodPointer staticMethodPointer = DescribeFieldTest::Class::staticMethod_::GetPointer<DescribeFieldTest, false>::value;
    
    EXPECT_EQ(&DescribeFieldTest::memberValue, memberValuePointer);
    EXPECT_EQ(&DescribeFieldTest::staticValue, staticValuePointer);
    EXPECT_TRUE(nullptr == memberValueReferencePointer);
    EXPECT_TRUE(nullptr == staticValueReferencePointer);
    EXPECT_EQ(&DescribeFieldTest::memberMethod, memberMethodPointer);
    EXPECT_EQ(&DescribeFieldTest::staticMethod, staticMethodPointer);
    
    using MemberValueNote = decltype(DescribeFieldTest::Class::memberValue_::idNote<DescribeFieldTest>(0));
    using StaticValueNote = decltype(DescribeFieldTest::Class::staticValue_::idNote<DescribeFieldTest>(0));
    using MemberValueReferenceNote = decltype(DescribeFieldTest::Class::memberValueReference_::idNote<DescribeFieldTest>(0));
    using StaticValueReferenceNote = decltype(DescribeFieldTest::Class::staticValueReference_::idNote<DescribeFieldTest>(0));
    using MemberMethodNote = decltype(DescribeFieldTest::Class::memberMethod_::idNote<DescribeFieldTest>(0));
    using StaticMethodNote = decltype(DescribeFieldTest::Class::staticMethod_::idNote<DescribeFieldTest>(0));
    
    isSame = std::is_same<decltype(DescribeFieldTest::Class::NoNote), MemberValueNote>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::staticValue_note), StaticValueNote>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::memberValueReference_note), MemberValueReferenceNote>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::staticValueReference_note), StaticValueReferenceNote>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::Class::NoNote), MemberMethodNote>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::Class::NoNote), StaticMethodNote>::value;
    EXPECT_TRUE(isSame);
    
    MemberValueNote memberValueNote = DescribeFieldTest::Class::memberValue_::GetNote<DescribeFieldTest, true>::value;
    StaticValueNote staticValueNote = DescribeFieldTest::Class::staticValue_::GetNote<DescribeFieldTest, false>::value;
    MemberValueReferenceNote memberValueReferenceNote = DescribeFieldTest::Class::memberValueReference_::GetNote<DescribeFieldTest, false>::value;
    StaticValueReferenceNote staticValueReferenceNoteNote = DescribeFieldTest::Class::staticValueReference_::GetNote<DescribeFieldTest, false>::value;
    MemberMethodNote memberMethodNote = DescribeFieldTest::Class::memberMethod_::GetNote<DescribeFieldTest, true>::value;
    StaticMethodNote staticMethodNote = DescribeFieldTest::Class::staticMethod_::GetNote<DescribeFieldTest, true>::value;
    
    EXPECT_EQ(DescribeFieldTest::Class::NoNote, memberValueNote);
    EXPECT_EQ(DescribeFieldTest::Class::NoNote, staticValueNote);
    EXPECT_EQ(DescribeFieldTest::memberValueReference_note, memberValueReferenceNote);
    EXPECT_EQ(DescribeFieldTest::staticValueReference_note, staticValueReferenceNoteNote);
    EXPECT_EQ(DescribeFieldTest::Class::NoNote, memberMethodNote);
    EXPECT_EQ(DescribeFieldTest::Class::NoNote, staticMethodNote);
    
    using MemberValueField = DescribeFieldTest::Class::memberValue_::Field;
    using StaticValueField = DescribeFieldTest::Class::staticValue_::Field;
    using MemberValueReferenceField = DescribeFieldTest::Class::memberValueReference_::Field;
    using StaticValueReferenceField = DescribeFieldTest::Class::staticValueReference_::Field;
    using MemberMethodField = DescribeFieldTest::Class::memberMethod_::Field;
    using StaticMethodField = DescribeFieldTest::Class::staticMethod_::Field;

    using ExpectedMemberValueField = Fields::Field<decltype(DescribeFieldTest::memberValue), DescribeFieldTest::Class::memberValue_::FieldBase, DescribeFieldTest::Class::memberValue_, decltype(&DescribeFieldTest::memberValue), 0, decltype(DescribeFieldTest::Class::NoNote)>;
    using ExpectedStaticValueField = Fields::Field<decltype(DescribeFieldTest::staticValue), DescribeFieldTest::Class::staticValue_::FieldBase, DescribeFieldTest::Class::staticValue_, decltype(&DescribeFieldTest::staticValue), 1, decltype(DescribeFieldTest::Class::NoNote)>;
    using ExpectedMemberValueReferenceField = Fields::Field<decltype(DescribeFieldTest::memberValueReference), DescribeFieldTest::Class::memberValueReference_::FieldBase, DescribeFieldTest::Class::memberValueReference_, std::nullptr_t, 2, decltype(DescribeFieldTest::memberValueReference_note)>;
    using ExpectedStaticValueReferenceField = Fields::Field<decltype(DescribeFieldTest::staticValueReference), DescribeFieldTest::Class::staticValueReference_::FieldBase, DescribeFieldTest::Class::staticValueReference_, decltype(&DescribeFieldTest::staticValueReference), 3, decltype(DescribeFieldTest::staticValueReference_note)>;
    using ExpectedMemberMethodField = Fields::Field<decltype(&DescribeFieldTest::memberMethod), DescribeFieldTest::Class::memberMethod_::FieldBase, DescribeFieldTest::Class::memberMethod_, decltype(&DescribeFieldTest::memberMethod), 4, decltype(DescribeFieldTest::Class::NoNote)>;
    using ExpectedStaticMethodField = Fields::Field<decltype(DescribeFieldTest::staticMethod), DescribeFieldTest::Class::staticMethod_::FieldBase, DescribeFieldTest::Class::staticMethod_, decltype(&DescribeFieldTest::staticMethod), 5, decltype(DescribeFieldTest::Class::NoNote)>;
    
    isSame = std::is_same_v<ExpectedMemberValueField, MemberValueField>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedStaticValueField, StaticValueField>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedMemberValueReferenceField, MemberValueReferenceField>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedStaticValueReferenceField, StaticValueReferenceField>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedMemberMethodField, MemberMethodField>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ExpectedStaticMethodField, StaticMethodField>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<decltype(DescribeFieldTest::memberValue), MemberValueField::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<decltype(DescribeFieldTest::staticValue), StaticValueField::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<decltype(DescribeFieldTest::memberValueReference), MemberValueReferenceField::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<decltype(DescribeFieldTest::staticValueReference), StaticValueReferenceField::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<decltype(&DescribeFieldTest::memberMethod), MemberMethodField::Type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<decltype(DescribeFieldTest::staticMethod), StaticMethodField::Type>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<decltype(&DescribeFieldTest::memberValue), MemberValueField::Pointer>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<decltype(&DescribeFieldTest::staticValue), StaticValueField::Pointer>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::nullptr_t, MemberValueReferenceField::Pointer>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::nullptr_t, StaticValueReferenceField::Pointer>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<decltype(&DescribeFieldTest::memberMethod), MemberMethodField::Pointer>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<decltype(&DescribeFieldTest::staticMethod), StaticMethodField::Pointer>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same<decltype(DescribeFieldTest::Class::NoNote), MemberValueField::Annotations>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::staticValue_note), StaticValueField::Annotations>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::memberValueReference_note), MemberValueReferenceField::Annotations>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::staticValueReference_note), StaticValueReferenceField::Annotations>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::Class::NoNote), MemberMethodField::Annotations>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<decltype(DescribeFieldTest::Class::NoNote), StaticMethodField::Annotations>::value;
    EXPECT_TRUE(isSame);
    
    EXPECT_EQ(0, MemberValueField::Index);
    EXPECT_EQ(1, StaticValueField::Index);
    EXPECT_EQ(2, MemberValueReferenceField::Index);
    EXPECT_EQ(3, StaticValueReferenceField::Index);
    EXPECT_EQ(4, MemberMethodField::Index);
    EXPECT_EQ(5, StaticMethodField::Index);
    
    EXPECT_FALSE(MemberValueField::IsStatic);
    EXPECT_TRUE(StaticValueField::IsStatic);
    EXPECT_FALSE(MemberValueReferenceField::IsStatic);
    EXPECT_TRUE(StaticValueReferenceField::IsStatic);
    EXPECT_FALSE(MemberMethodField::IsStatic);
    EXPECT_TRUE(StaticMethodField::IsStatic);
    
    EXPECT_FALSE(MemberValueField::IsFunction);
    EXPECT_FALSE(StaticValueField::IsFunction);
    EXPECT_FALSE(MemberValueReferenceField::IsFunction);
    EXPECT_FALSE(StaticValueReferenceField::IsFunction);
    EXPECT_TRUE(MemberMethodField::IsFunction);
    EXPECT_TRUE(StaticMethodField::IsFunction);
    
    EXPECT_TRUE(MemberValueField::HasOffset);
    EXPECT_FALSE(StaticValueField::HasOffset);
    EXPECT_FALSE(MemberValueReferenceField::HasOffset);
    EXPECT_FALSE(StaticValueReferenceField::HasOffset);
    EXPECT_FALSE(MemberMethodField::HasOffset);
    EXPECT_FALSE(StaticMethodField::HasOffset);

    MemberValueField memberValueField = DescribeFieldTest::Class::memberValue_::field;
    StaticValueField staticValueField = DescribeFieldTest::Class::staticValue_::field;
    MemberValueReferenceField memberValueReferenceField = DescribeFieldTest::Class::memberValueReference_::field;
    StaticValueReferenceField staticValueReferenceField = DescribeFieldTest::Class::staticValueReference_::field;
    MemberMethodField memberMethodField = DescribeFieldTest::Class::memberMethod_::field;
    StaticMethodField staticMethodField = DescribeFieldTest::Class::staticMethod_::field;
    
    EXPECT_STREQ("memberValue", memberValueField.name);
    EXPECT_STREQ("staticValue", staticValueField.name);
    EXPECT_STREQ("memberValueReference", memberValueReferenceField.name);
    EXPECT_STREQ("staticValueReference", staticValueReferenceField.name);
    EXPECT_STREQ("memberMethod", memberMethodField.name);
    EXPECT_STREQ("staticMethod", staticMethodField.name);
    
    EXPECT_TRUE(std::regex_match(memberValueField.typeStr, std::regex("int")));
    EXPECT_TRUE(std::regex_match(staticValueField.typeStr, std::regex("int")));
    EXPECT_TRUE(std::regex_match(memberValueReferenceField.typeStr, std::regex("int\\s*&")));
    EXPECT_TRUE(std::regex_match(staticValueReferenceField.typeStr, std::regex("int\\s*&")));
    EXPECT_TRUE(std::regex_match(memberMethodField.typeStr, std::regex(".*void.*")));
    EXPECT_TRUE(std::regex_match(staticMethodField.typeStr, std::regex(".*void.*")));
    
    EXPECT_EQ(DescribeFieldTest::Class::NoNote, memberValueField.annotations);
    EXPECT_EQ(DescribeFieldTest::Class::NoNote, staticValueField.annotations);
    EXPECT_EQ(DescribeFieldTest::memberValueReference_note, memberValueReferenceField.annotations);
    EXPECT_EQ(DescribeFieldTest::staticValueReference_note, staticValueReferenceField.annotations);
    EXPECT_EQ(DescribeFieldTest::Class::NoNote, memberMethodField.annotations);
    EXPECT_EQ(DescribeFieldTest::Class::NoNote, staticMethodField.annotations);
}

TEST(ReflectionTest, RfMacroDescribeFieldCallbacks)
{
    int visitCount = 0;
    int referrable = 1337;
    DescribeFieldTest describeFieldTest{9001, referrable};
    DescribeFieldTest::Class::memberValue_::Field::callback<false>(describeFieldTest, [&](auto & value) {
        EXPECT_EQ(describeFieldTest.memberValue, value);
        value = 11;
        EXPECT_EQ(describeFieldTest.memberValue, value);
        visitCount++;
    });
    EXPECT_EQ(11, describeFieldTest.memberValue);
    DescribeFieldTest::Class::staticValue_::Field::callback<false>(describeFieldTest, [&](auto & value) {
        EXPECT_EQ(DescribeFieldTest::staticValue, value);
        value = 22;
        EXPECT_EQ(DescribeFieldTest::staticValue, value);
        visitCount++;
    });
    EXPECT_EQ(22, DescribeFieldTest::staticValue);
    DescribeFieldTest::Class::memberValueReference_::Field::callback<false>(describeFieldTest, [&](auto & value) {
        EXPECT_EQ(referrable, value);
        value = 33;
        EXPECT_EQ(referrable, value);
        visitCount++;
    });
    EXPECT_EQ(33, describeFieldTest.memberValueReference);
    DescribeFieldTest::Class::staticValueReference_::Field::callback<false>(describeFieldTest, [&](auto & value) {
        EXPECT_EQ(DescribeFieldTest::staticValueReference, value);
        value = 44;
        EXPECT_EQ(DescribeFieldTest::staticValueReference, value);
        visitCount++;
    });
    EXPECT_EQ(44, DescribeFieldTest::staticValueReference);
    DescribeFieldTest::Class::memberMethod_::Field::callback<false>(describeFieldTest, [&](auto & value) {
        (describeFieldTest.*value)();
        EXPECT_EQ(value, &DescribeFieldTest::memberMethod);
        visitCount++;
    });
    DescribeFieldTest::Class::staticMethod_::Field::callback<false>(describeFieldTest, [&](auto & value) {
        value();
        EXPECT_EQ(value, &DescribeFieldTest::staticMethod);
        visitCount++;
    });
    EXPECT_EQ(6, visitCount);
    
    visitCount = 0;
    DescribeFieldTest::Class::memberValue_::Field::callback<true>(describeFieldTest, [&](auto & field, auto & value) {
        EXPECT_EQ(describeFieldTest.memberValue, value);
        value = 111;
        EXPECT_EQ(describeFieldTest.memberValue, value);
        visitCount++;
    });
    EXPECT_EQ(111, describeFieldTest.memberValue);
    DescribeFieldTest::Class::staticValue_::Field::callback<true>(describeFieldTest, [&](auto & field, auto & value) {
        EXPECT_EQ(DescribeFieldTest::staticValue, value);
        value = 222;
        EXPECT_EQ(DescribeFieldTest::staticValue, value);
        visitCount++;
    });
    EXPECT_EQ(222, DescribeFieldTest::staticValue);
    DescribeFieldTest::Class::memberValueReference_::Field::callback<true>(describeFieldTest, [&](auto & field, auto & value) {
        EXPECT_EQ(referrable, value);
        value = 333;
        EXPECT_EQ(referrable, value);
        visitCount++;
    });
    EXPECT_EQ(333, describeFieldTest.memberValueReference);
    DescribeFieldTest::Class::staticValueReference_::Field::callback<true>(describeFieldTest, [&](auto & field, auto & value) {
        EXPECT_EQ(DescribeFieldTest::staticValueReference, value);
        value = 444;
        EXPECT_EQ(DescribeFieldTest::staticValueReference, value);
        visitCount++;
    });
    EXPECT_EQ(444, DescribeFieldTest::staticValueReference);
    DescribeFieldTest::Class::memberMethod_::Field::callback<true>(describeFieldTest, [&](auto & field, auto & value) {
        (describeFieldTest.*value)();
        EXPECT_EQ(&DescribeFieldTest::memberMethod, value);
        visitCount++;
    });
    DescribeFieldTest::Class::staticMethod_::Field::callback<true>(describeFieldTest, [&](auto & field, auto & value) {
        value();
        EXPECT_EQ(&DescribeFieldTest::staticMethod, value);
        visitCount++;
    });
    EXPECT_EQ(6, visitCount);

    visitCount = 0;
    DescribeFieldTest::Class::memberValue_::Field::callback<false>([&](auto & value) {
        EXPECT_EQ(value, &DescribeFieldTest::memberValue);
        describeFieldTest.*value = 1111;
        EXPECT_EQ(value, &DescribeFieldTest::memberValue);
        visitCount++;
    });
    EXPECT_EQ(1111, describeFieldTest.memberValue);
    DescribeFieldTest::Class::staticValue_::Field::callback<false>([&](auto & value) {
        EXPECT_EQ(value, DescribeFieldTest::staticValue);
        value = 2222;
        EXPECT_EQ(value, DescribeFieldTest::staticValue);
        visitCount++;
    });
    EXPECT_EQ(2222, DescribeFieldTest::staticValue);
    DescribeFieldTest::Class::memberValueReference_::Field::callback<false>([&](auto & value) {
        EXPECT_TRUE(value == nullptr);
        visitCount++;
    });
    DescribeFieldTest::Class::staticValueReference_::Field::callback<false>([&](auto & value) {
        EXPECT_EQ(value, DescribeFieldTest::staticValueReference);
        value = 4444;
        EXPECT_EQ(value, DescribeFieldTest::staticValueReference);
        visitCount++;
    });
    EXPECT_EQ(4444, DescribeFieldTest::staticValueReference);
    DescribeFieldTest::Class::memberMethod_::Field::callback<false>([&](auto & value) {
        (describeFieldTest.*value)();
        EXPECT_EQ(value, &DescribeFieldTest::memberMethod);
        visitCount++;
    });
    DescribeFieldTest::Class::staticMethod_::Field::callback<false>([&](auto & value) {
        value();
        EXPECT_EQ(value, &DescribeFieldTest::staticMethod);
        visitCount++;
    });
    EXPECT_EQ(6, visitCount);
    
    visitCount = 0;
    DescribeFieldTest::Class::memberValue_::Field::callback<true>([&](auto & field, auto & value) {
        EXPECT_EQ(value, &DescribeFieldTest::memberValue);
        describeFieldTest.*value = 11111;
        EXPECT_EQ(value, &DescribeFieldTest::memberValue);
        visitCount++;
    });
    EXPECT_EQ(11111, describeFieldTest.memberValue);
    DescribeFieldTest::Class::staticValue_::Field::callback<true>([&](auto & field, auto & value) {
        EXPECT_EQ(value, DescribeFieldTest::staticValue);
        value = 22222;
        EXPECT_EQ(value, DescribeFieldTest::staticValue);
        visitCount++;
    });
    EXPECT_EQ(22222, DescribeFieldTest::staticValue);
    DescribeFieldTest::Class::memberValueReference_::Field::callback<true>([&](auto & field, auto & value) {
        EXPECT_TRUE(value == nullptr);
        visitCount++;
    });
    DescribeFieldTest::Class::staticValueReference_::Field::callback<true>([&](auto & field, auto & value) {
        EXPECT_EQ(value, DescribeFieldTest::staticValueReference);
        value = 44444;
        EXPECT_EQ(value, DescribeFieldTest::staticValueReference);
        visitCount++;
    });
    EXPECT_EQ(44444, DescribeFieldTest::staticValueReference);
    DescribeFieldTest::Class::memberMethod_::Field::callback<true>([&](auto & field, auto & value) {
        (describeFieldTest.*value)();
        EXPECT_EQ(value, &DescribeFieldTest::memberMethod);
        visitCount++;
    });
    DescribeFieldTest::Class::staticMethod_::Field::callback<true>([&](auto & field, auto & value) {
        value();
        EXPECT_EQ(value, &DescribeFieldTest::staticMethod);
        visitCount++;
    });
    EXPECT_EQ(6, visitCount);
}

struct NoOpCallbacks {
    static constexpr const char name[] = "first";
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<int>();
    static constexpr const char* typeStr = typeStr_;
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(Function && function) {}
    template <bool IncludeField, typename Function, typename T_ = void> static constexpr void callback(T_ && t, Function && function) {}
};

struct GetFieldTest {
    int first;
    float second;

    struct Class {
        static constexpr NoAnnotation NoNote {};
        struct first_ {
            static constexpr const char nameStr[] = "first";
            static constexpr Fields::Field<decltype(first), NoOpCallbacks, void, decltype(&GetFieldTest::first), 0, NoAnnotation> field {};
        };
        struct second_ {
            static constexpr const char nameStr[] = "second";
            static constexpr Fields::Field<decltype(second), NoOpCallbacks, void, decltype(&GetFieldTest::second), 1, NoAnnotation> field {};
        };
        static constexpr Fields::Field<> Fields[2] {
            GET_FIELD(first)
            GET_FIELD(second)
        };
    };
};

TEST(ReflectionTest, RfMacroGetField)
{
    EXPECT_STREQ(NoOpCallbacks::name, GetFieldTest::Class::Fields[0].name);
    EXPECT_STREQ(NoOpCallbacks::typeStr, GetFieldTest::Class::Fields[0].typeStr);

    EXPECT_STREQ(NoOpCallbacks::name, GetFieldTest::Class::Fields[1].name);
    EXPECT_STREQ(NoOpCallbacks::typeStr, GetFieldTest::Class::Fields[1].typeStr);
}

class PassMacroTest {
public:
    int memberValue = 1;
    static int staticValue; // = 2
    int & memberValueReference;
    static int & staticValueReference;
    int memberMethod() { return 5; }
    static int staticMethod() { return 6; }

    PassMacroTest() : memberValueReference(memberValue) {}

    struct Class {
        using ProxyType = PassMacroTest;
        using ClassType = PassMacroTest;
        enum_t(IndexOf, size_t, { memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod });
        template <size_t FieldIndex, typename U_ = void> struct FieldDescr;
        FOR_EACH(DESCRIBE_FIELD, memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod)
        template <typename Function> static constexpr auto field_pack(Function && function) {
            return function(FOR_HEAD_TAIL(PASS_FIELD, PASS_FIELD_, memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod)); }
        template <typename Function, typename U> static constexpr auto value_pack(U && t, Function && function) {
            return function(FOR_HEAD_TAIL(PASS_VALUE, PASS_VALUE_, memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod)); }
        template <typename Function> static constexpr auto value_pack(Function && function) {
            return function(FOR_HEAD_TAIL(PASS_STATIC_VALUE, PASS_STATIC_VALUE_, memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod)); }
    };
};

int PassMacroTest::staticValue = 2;
int & PassMacroTest::staticValueReference = PassMacroTest::staticValue;

TEST(ReflectionTest, RfMacroPassFieldTest)
{
    bool visited = false;
    PassMacroTest::Class::field_pack([&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
        using MemberField = decltype(memberValue);
        using StaticField = decltype(staticValue);
        using MemberValueReference = decltype(memberValueReference);
        using StaticValueReference = decltype(staticValueReference);
        using MemberMethod = decltype(memberMethod);
        using StaticMethod = decltype(staticMethod);

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
        using MemberField = std::remove_const_t<std::remove_reference_t<decltype(memberValue)>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<decltype(staticValue)>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<decltype(memberValueReference)>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<decltype(staticValueReference)>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<decltype(memberMethod)>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<decltype(staticMethod)>>;

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
        using MemberField = std::remove_const_t<std::remove_reference_t<decltype(memberValue)>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<decltype(staticValue)>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<decltype(memberValueReference)>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<decltype(staticValueReference)>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<decltype(memberMethod)>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<decltype(staticMethod)>>;

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
        using MemberField = std::remove_const_t<std::remove_reference_t<decltype(memberValue)>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<decltype(staticValue)>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<decltype(memberValueReference)>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<decltype(staticValueReference)>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<decltype(memberMethod)>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<decltype(staticMethod)>>;

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
        using MemberField = std::remove_const_t<std::remove_reference_t<decltype(memberValue)>>;
        using StaticField = std::remove_const_t<std::remove_reference_t<decltype(staticValue)>>;
        using MemberValueReference = std::remove_const_t<std::remove_reference_t<decltype(memberValueReference)>>;
        using StaticValueReference = std::remove_const_t<std::remove_reference_t<decltype(staticValueReference)>>;
        using MemberMethod = std::remove_const_t<std::remove_reference_t<decltype(memberMethod)>>;
        using StaticMethod = std::remove_const_t<std::remove_reference_t<decltype(staticMethod)>>;

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](auto ... ts) {
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

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](auto & ... ts) {
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

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](auto && ... ts) {
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

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](const auto ... ts) {
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

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::field_pack([&](const auto & ... ts) {
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

        bool isSame = std::is_same_v<PassMacroTest::Class::memberValue_::Field, MemberField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValue_::Field, StaticField>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberValueReference_::Field, MemberValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticValueReference_::Field, StaticValueReference>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::memberMethod_::Field, MemberMethod>; EXPECT_TRUE(isSame);
        isSame = std::is_same_v<PassMacroTest::Class::staticMethod_::Field, StaticMethod>; EXPECT_TRUE(isSame);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.name, memberValue.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.name, staticValue.name);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.name, memberValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.name, staticValueReference.name);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.name, memberMethod.name);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.name, staticMethod.name);
        
        EXPECT_STREQ(PassMacroTest::Class::memberValue_::field.typeStr, memberValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValue_::field.typeStr, staticValue.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberValueReference_::field.typeStr, memberValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticValueReference_::field.typeStr, staticValueReference.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::memberMethod_::field.typeStr, memberMethod.typeStr);
        EXPECT_STREQ(PassMacroTest::Class::staticMethod_::field.typeStr, staticMethod.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);

    // Note: fields are always lvalues and don't have a need to be passed as rvalue references
}

TEST(ReflectionTest, RfMacroPassValueTest) {
    PassMacroTest obj {};

    bool visited = false;
    PassMacroTest::Class::value_pack(obj, [&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
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
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
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
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
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
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
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
        isSame = std::is_same_v<std::add_const_t<decltype(&PassMacroTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&PassMacroTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
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
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&PassMacroTest::memberMethod)>>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&PassMacroTest::staticMethod)>>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](auto ... ts) {
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
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](auto & ... ts) {
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
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](auto && ... ts) {
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
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](const auto ... ts) {
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
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack(obj, [&](const auto & ... ts) {
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
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    // Note: class members are always lvalues and don't have a need to be passed as rvalue references
}

TEST(ReflectionTest, RfMacroPassStaticValueTest) {
    PassMacroTest obj {};

    bool visited = false;
    PassMacroTest::Class::value_pack([&](auto memberValue, auto staticValue, auto memberValueReference, auto staticValueReference, auto memberMethod, auto staticMethod) {
        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<decltype(&PassMacroTest::memberValue), decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    PassMacroTest::Class::value_pack([&](auto & memberValue, auto & staticValue, auto & memberValueReference, auto & staticValueReference, auto & memberMethod, auto & staticMethod) {
        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    PassMacroTest::Class::value_pack([&](auto && memberValue, auto && staticValue, auto && memberValueReference, auto && staticValueReference, auto && memberMethod, auto && staticMethod) {
        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack([&](const auto memberValue, const auto staticValue, const auto memberValueReference, const auto staticValueReference, const auto memberMethod, const auto staticMethod) {
        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_const_t<decltype(&PassMacroTest::memberValue)>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const nullptr_t, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&PassMacroTest::memberMethod)>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&PassMacroTest::staticMethod)>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    PassMacroTest::Class::value_pack([&](const auto & memberValue, const auto & staticValue, const auto & memberValueReference, const auto & staticValueReference, const auto & memberMethod, const auto & staticMethod) {
        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&PassMacroTest::memberValue)>>, decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const nullptr_t &, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&PassMacroTest::memberMethod)>>, decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&PassMacroTest::staticMethod)>>, decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack([&](auto ... ts) {
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
        
        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<decltype(&PassMacroTest::memberValue), decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack([&](auto & ... ts) {
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

        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::memberValue)>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t &, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int &, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::memberMethod)>, MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<decltype(&PassMacroTest::staticMethod)>, StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack([&](auto && ... ts) {
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

        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<decltype(&PassMacroTest::memberValue), decltype(memberValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValue)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<nullptr_t, decltype(memberValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<int, decltype(staticValueReference)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::memberMethod), decltype(memberMethod)>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<decltype(&PassMacroTest::staticMethod), decltype(staticMethod)>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack([&](const auto ... ts) {
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

        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_const_t<decltype(&PassMacroTest::memberValue)>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const nullptr_t, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&PassMacroTest::memberMethod)>, MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_const_t<decltype(&PassMacroTest::staticMethod)>, StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    PassMacroTest::Class::value_pack([&](const auto & ... ts) {
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

        EXPECT_EQ(&PassMacroTest::memberValue, memberValue);
        EXPECT_EQ(2, staticValue);
        EXPECT_TRUE(memberValueReference == nullptr);
        EXPECT_EQ(2, staticValueReference);
        EXPECT_EQ(5, (obj.*memberMethod)());
        EXPECT_EQ(6, staticMethod());
        
        bool isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&PassMacroTest::memberValue)>>, MemberType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, StaticType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const nullptr_t &, MemberValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<const int &, StaticValueReferenceType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&PassMacroTest::memberMethod)>>, MemberMethodType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::add_lvalue_reference_t<std::add_const_t<decltype(&PassMacroTest::staticMethod)>>, StaticMethodType>;
        EXPECT_TRUE(isSame);
        visited = true;
    });
    EXPECT_TRUE(visited);

    // Note: class members are always lvalues and don't have a need to be passed as rvalue references
}

class CumulativeMacroTest {
public:
    int first;
    float second;

    struct Class {
        using ProxyType = CumulativeMacroTest;
        using ClassType = Reflection::unproxy_t<ProxyType>;
        static constexpr size_t Total = COUNT_ARGUMENTS(first, second);
        enum_t(IndexOf, size_t, { first, second });
        using Annotations = Reflection::NoAnnotation;
        static constexpr const Reflection::NoAnnotation & annotations = Reflection::NoNote;
        template <size_t FieldIndex, typename U_ = void> struct FieldDescr; FOR_EACH(DESCRIBE_FIELD, first, second)
        static constexpr Reflection::Fields::Field<> Fields[Total+1] { FOR_EACH(GET_FIELD, first, second) };
        template <typename Function> static constexpr auto field_pack(Function && function) {
            return function(FOR_HEAD_TAIL(PASS_FIELD, PASS_FIELD_, first, second)); }
        template <typename Function, typename U> static constexpr auto value_pack(U && t, Function && function) {
            return function(FOR_HEAD_TAIL(PASS_VALUE, PASS_VALUE_, first, second)); }
        template <typename Function> static constexpr auto value_pack(Function && function) {
            return function(FOR_HEAD_TAIL(PASS_STATIC_VALUE, PASS_STATIC_VALUE_, first, second)); }
    };
};

TEST(ReflectionTest, RfMacroCumulative)
{
    // This is just the entire REFLECT macro, it may be useful for testing changes, but there's no need to unit test it
    EXPECT_TRUE(true);
}

TEST(ReflectionTest, RfMacroReflectNoted)
{
    EXPECT_TRUE(is_reflected<ReflectSuperObj>::value);
    EXPECT_TRUE(is_reflected<ReflectSubObj>::value);
    EXPECT_TRUE(is_reflected<ReflectionObj>::value);

    bool isSame = std::is_same_v<ReflectSuperObj, ReflectSuperObj::Class::ProxyType>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectSuperObj, ReflectSuperObj::Class::ClassType>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectSubObj, ReflectSubObj::Class::ProxyType>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectSubObj, ReflectSubObj::Class::ClassType>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectionObj, ReflectionObj::Class::ProxyType>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<ReflectionObj, ReflectionObj::Class::ClassType>;
    EXPECT_TRUE(isSame);
    
    EXPECT_EQ(1, ReflectSuperObj::Class::Total);
    EXPECT_EQ(1, ReflectSubObj::Class::Total);
    EXPECT_EQ(11, ReflectionObj::Class::Total);

    EXPECT_EQ(0, ReflectSuperObj::Class::IndexOf::superVal);
    EXPECT_EQ(0, ReflectSubObj::Class::IndexOf::val);
    EXPECT_EQ(0, ReflectionObj::Class::IndexOf::primitive);
    EXPECT_EQ(1, ReflectionObj::Class::IndexOf::object);
    EXPECT_EQ(2, ReflectionObj::Class::IndexOf::primitiveArray);
    EXPECT_EQ(3, ReflectionObj::Class::IndexOf::map);
    EXPECT_EQ(4, ReflectionObj::Class::IndexOf::objCollection);
    EXPECT_EQ(5, ReflectionObj::Class::IndexOf::stack);
    EXPECT_EQ(6, ReflectionObj::Class::IndexOf::staticPrimitive);
    EXPECT_EQ(7, ReflectionObj::Class::IndexOf::primitiveReference);
    EXPECT_EQ(8, ReflectionObj::Class::IndexOf::staticPrimitiveReference);
    EXPECT_EQ(9, ReflectionObj::Class::IndexOf::memberMethod);
    EXPECT_EQ(10, ReflectionObj::Class::IndexOf::staticMethod);
    
    isSame = std::is_same_v<Reflection::NoAnnotation, ReflectSuperObj::Class::Annotations>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Reflection::NoAnnotation, ReflectSubObj::Class::Annotations>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<Reflection::Inheritance::SuperClass<ReflectSuperObj>>, std::remove_const_t<ReflectionObj::Class::Annotations>>;
    EXPECT_TRUE(isSame);

    std::string typeStr = ReflectionObj::Class::primitive_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int") != std::string::npos);
    typeStr = ReflectionObj::Class::object_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("ReflectSubObj") != std::string::npos);
    typeStr = ReflectionObj::Class::primitiveArray_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int") != std::string::npos);
    typeStr = ReflectionObj::Class::map_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("map<int,float") != std::string::npos);
    typeStr = ReflectionObj::Class::objCollection_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("vector<") != std::string::npos && typeStr.find("ReflectSubObj") != std::string::npos);
    typeStr = ReflectionObj::Class::stack_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("stack<int") != std::string::npos);
    typeStr = ReflectionObj::Class::staticPrimitive_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int") != std::string::npos);
    typeStr = ReflectionObj::Class::primitiveReference_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int&") != std::string::npos);
    typeStr = ReflectionObj::Class::staticPrimitiveReference_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int&") != std::string::npos);
    typeStr = ReflectionObj::Class::memberMethod_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int") != std::string::npos);
    typeStr = ReflectionObj::Class::staticMethod_::Field::typeStr;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("int") != std::string::npos);

    EXPECT_STREQ(ReflectionObj::Class::primitive_::Field::typeStr, ReflectionObj::Class::primitive_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::object_::Field::typeStr, ReflectionObj::Class::object_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::primitiveArray_::Field::typeStr, ReflectionObj::Class::primitiveArray_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::map_::Field::typeStr, ReflectionObj::Class::map_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::objCollection_::Field::typeStr, ReflectionObj::Class::objCollection_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::stack_::Field::typeStr, ReflectionObj::Class::stack_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::staticPrimitive_::Field::typeStr, ReflectionObj::Class::staticPrimitive_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::primitiveReference_::Field::typeStr, ReflectionObj::Class::primitiveReference_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::staticPrimitiveReference_::Field::typeStr, ReflectionObj::Class::staticPrimitiveReference_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::memberMethod_::Field::typeStr, ReflectionObj::Class::memberMethod_::field.typeStr);
    EXPECT_STREQ(ReflectionObj::Class::staticMethod_::Field::typeStr, ReflectionObj::Class::staticMethod_::field.typeStr);
    
    EXPECT_STREQ("primitive", ReflectionObj::Class::primitive_::Field::name);
    EXPECT_STREQ("object", ReflectionObj::Class::object_::Field::name);
    EXPECT_STREQ("primitiveArray", ReflectionObj::Class::primitiveArray_::Field::name);
    EXPECT_STREQ("map", ReflectionObj::Class::map_::Field::name);
    EXPECT_STREQ("objCollection", ReflectionObj::Class::objCollection_::Field::name);
    EXPECT_STREQ("stack", ReflectionObj::Class::stack_::Field::name);
    EXPECT_STREQ("staticPrimitive", ReflectionObj::Class::staticPrimitive_::Field::name);
    EXPECT_STREQ("primitiveReference", ReflectionObj::Class::primitiveReference_::Field::name);
    EXPECT_STREQ("staticPrimitiveReference", ReflectionObj::Class::staticPrimitiveReference_::Field::name);
    EXPECT_STREQ("memberMethod", ReflectionObj::Class::memberMethod_::Field::name);
    EXPECT_STREQ("staticMethod", ReflectionObj::Class::staticMethod_::Field::name);
    
    EXPECT_STREQ("primitive", ReflectionObj::Class::primitive_::field.name);
    EXPECT_STREQ("object", ReflectionObj::Class::object_::field.name);
    EXPECT_STREQ("primitiveArray", ReflectionObj::Class::primitiveArray_::field.name);
    EXPECT_STREQ("map", ReflectionObj::Class::map_::field.name);
    EXPECT_STREQ("objCollection", ReflectionObj::Class::objCollection_::field.name);
    EXPECT_STREQ("stack", ReflectionObj::Class::stack_::field.name);
    EXPECT_STREQ("staticPrimitive", ReflectionObj::Class::staticPrimitive_::field.name);
    EXPECT_STREQ("primitiveReference", ReflectionObj::Class::primitiveReference_::field.name);
    EXPECT_STREQ("staticPrimitiveReference", ReflectionObj::Class::staticPrimitiveReference_::field.name);
    EXPECT_STREQ("memberMethod", ReflectionObj::Class::memberMethod_::field.name);
    EXPECT_STREQ("staticMethod", ReflectionObj::Class::staticMethod_::field.name);
    
    EXPECT_STREQ(ReflectionObj::Class::primitive_::field.name, ReflectionObj::Class::Fields[0].name);
    EXPECT_STREQ(ReflectionObj::Class::object_::field.name, ReflectionObj::Class::Fields[1].name);
    EXPECT_STREQ(ReflectionObj::Class::primitiveArray_::field.name, ReflectionObj::Class::Fields[2].name);
    EXPECT_STREQ(ReflectionObj::Class::map_::field.name, ReflectionObj::Class::Fields[3].name);
    EXPECT_STREQ(ReflectionObj::Class::objCollection_::field.name, ReflectionObj::Class::Fields[4].name);
    EXPECT_STREQ(ReflectionObj::Class::stack_::field.name, ReflectionObj::Class::Fields[5].name);
    EXPECT_STREQ(ReflectionObj::Class::staticPrimitive_::field.name, ReflectionObj::Class::Fields[6].name);
    EXPECT_STREQ(ReflectionObj::Class::primitiveReference_::field.name, ReflectionObj::Class::Fields[7].name);
    EXPECT_STREQ(ReflectionObj::Class::staticPrimitiveReference_::field.name, ReflectionObj::Class::Fields[8].name);
    EXPECT_STREQ(ReflectionObj::Class::memberMethod_::field.name, ReflectionObj::Class::Fields[9].name);
    EXPECT_STREQ(ReflectionObj::Class::staticMethod_::field.name, ReflectionObj::Class::Fields[10].name);
    
    EXPECT_STREQ(ReflectionObj::Class::primitive_::field.typeStr, ReflectionObj::Class::Fields[0].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::object_::field.typeStr, ReflectionObj::Class::Fields[1].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::primitiveArray_::field.typeStr, ReflectionObj::Class::Fields[2].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::map_::field.typeStr, ReflectionObj::Class::Fields[3].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::objCollection_::field.typeStr, ReflectionObj::Class::Fields[4].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::stack_::field.typeStr, ReflectionObj::Class::Fields[5].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::staticPrimitive_::field.typeStr, ReflectionObj::Class::Fields[6].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::primitiveReference_::field.typeStr, ReflectionObj::Class::Fields[7].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::staticPrimitiveReference_::field.typeStr, ReflectionObj::Class::Fields[8].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::memberMethod_::field.typeStr, ReflectionObj::Class::Fields[9].typeStr);
    EXPECT_STREQ(ReflectionObj::Class::staticMethod_::field.typeStr, ReflectionObj::Class::Fields[10].typeStr);
}
