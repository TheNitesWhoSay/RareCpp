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

class UseFieldValueTest {
    public:
        int first;
        const float second;

        struct Class {
            using ClassType = UseFieldValueTest;
            static constexpr NoAnnotation NoNote {};
            struct first_ {
                using Field = Fields::Field<int, decltype(&UseFieldValueTest::first), 0>;
                static constexpr Field field = { "first", "int", &UseFieldValueTest::first, NoNote };
                CLANG_ONLY(first)
            };
            struct second_ {
                using Field = Fields::Field<const float, decltype(&UseFieldValueTest::second), 1>;
                static constexpr Field field = { "second", "float", &UseFieldValueTest::second, NoNote };
                CLANG_ONLY(second)
            };
            template <typename Function>
            static void ForEachField(UseFieldValueTest & object, Function function) {
                USE_FIELD_VALUE(first)
                USE_FIELD_VALUE(second)
            }
        };
};

TEST(ReflectTest, RfMacroUseFieldValue)
{
    UseFieldValueTest useFieldValueTest = { 1, 1.1f };
    size_t index = 0;
    UseFieldValueTest::Class::ForEachField(useFieldValueTest, [&](auto & field, auto & value) {

        using Field = typename std::remove_reference<decltype(field)>::type;

        EXPECT_EQ(index, Field::Index);
        if constexpr ( Field::Index == 0 )
        {
            EXPECT_EQ(1, value);
            EXPECT_EQ(useFieldValueTest.first, value);
            bool isEqual = std::is_same_v<int, std::remove_reference_t<decltype(value)>>;
            EXPECT_TRUE(isEqual);
            value = 2;

            EXPECT_STREQ("first", field.name);
            EXPECT_STREQ("int", field.typeStr);
            isEqual = std::is_same_v<int, typename Field::Type>;
            EXPECT_TRUE(isEqual);
            isEqual = std::is_same_v<decltype(&UseFieldValueTest::first), typename Field::Pointer>;
            EXPECT_TRUE(isEqual);
            isEqual = std::is_same_v<NoAnnotation, typename Field::Annotations>;
            EXPECT_TRUE(isEqual);
            EXPECT_EQ(field.p, &UseFieldValueTest::first);
        }
        else if constexpr ( Field::Index == 1 )
        {
            EXPECT_EQ(1.1f, value);
            EXPECT_EQ(useFieldValueTest.second, value);
            bool isEqual = std::is_same_v<const float, std::remove_reference_t<decltype(value)>>;
            EXPECT_TRUE(isEqual);

            EXPECT_STREQ("second", field.name);
            EXPECT_STREQ("float", field.typeStr);
            isEqual = std::is_same_v<const float, typename Field::Type>;
            EXPECT_TRUE(isEqual);
            isEqual = std::is_same_v<decltype(&UseFieldValueTest::second), typename Field::Pointer>;
            EXPECT_TRUE(isEqual);
            isEqual = std::is_same_v<NoAnnotation, typename Field::Annotations>;
            EXPECT_TRUE(isEqual);
            EXPECT_EQ(field.p, &UseFieldValueTest::second);
        }
        index ++;
    });
    EXPECT_EQ(2, index);
    EXPECT_EQ(2, useFieldValueTest.first);
}

class UseFieldValueAtTest {
    public:
        int first;
        const float second;

        struct Class {
            using ClassType = UseFieldValueAtTest;
            enum_t(IndexOf, size_t, { first, second });
            static constexpr NoAnnotation NoNote {};
            struct first_ {
                using Field = Fields::Field<int, decltype(&UseFieldValueAtTest::first), 0>;
                static constexpr Field field = { "first", "int", &UseFieldValueAtTest::first, NoNote };
                CLANG_ONLY(first)
            };
            struct second_ {
                using Field = Fields::Field<const float, decltype(&UseFieldValueAtTest::second), 1>;
                static constexpr Field field = { "second", "float", &UseFieldValueAtTest::second, NoNote };
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
    UseFieldValueAtTest useFieldValueAtTest = { 1, 1.1f };
    size_t index = 0;

    bool visited = false;
    UseFieldValueAtTest::Class::FieldAt(useFieldValueAtTest, 0, [&](auto & field, auto & value) {

        using Field = typename std::remove_reference<decltype(field)>::type;
        if constexpr ( Field::Index == 0 )
        {
            EXPECT_EQ(1, value);
            EXPECT_EQ(useFieldValueAtTest.first, value);
            bool isEqual = std::is_same_v<int, std::remove_reference_t<decltype(value)>>;
            EXPECT_TRUE(isEqual);
            value = 2;

            EXPECT_STREQ("first", field.name);
            EXPECT_STREQ("int", field.typeStr);
            isEqual = std::is_same_v<int, typename Field::Type>;
            EXPECT_TRUE(isEqual);
            isEqual = std::is_same_v<decltype(&UseFieldValueAtTest::first), typename Field::Pointer>;
            EXPECT_TRUE(isEqual);
            isEqual = std::is_same_v<NoAnnotation, typename Field::Annotations>;
            EXPECT_TRUE(isEqual);
            EXPECT_EQ(field.p, &UseFieldValueAtTest::first);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    UseFieldValueAtTest::Class::FieldAt(useFieldValueAtTest, 1, [&](auto & field, auto & value) {

        using Field = typename std::remove_reference<decltype(field)>::type;
        if constexpr ( Field::Index == 1 )
        {
            EXPECT_EQ(1.1f, value);
            EXPECT_EQ(useFieldValueAtTest.second, value);
            bool isEqual = std::is_same_v<const float, std::remove_reference_t<decltype(value)>>;
            EXPECT_TRUE(isEqual);

            EXPECT_STREQ("second", field.name);
            EXPECT_STREQ("float", field.typeStr);
            isEqual = std::is_same_v<const float, typename Field::Type>;
            EXPECT_TRUE(isEqual);
            isEqual = std::is_same_v<decltype(&UseFieldValueAtTest::second), typename Field::Pointer>;
            EXPECT_TRUE(isEqual);
            isEqual = std::is_same_v<NoAnnotation, typename Field::Annotations>;
            EXPECT_TRUE(isEqual);
            EXPECT_EQ(field.p, &UseFieldValueAtTest::second);
            visited = true;
        }
    });
    EXPECT_TRUE(visited);
    EXPECT_EQ(2, useFieldValueAtTest.first);
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
        using ClassType = DescribeFieldTest;
        enum_t(IndexOf, size_t, { memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod });
        static constexpr NoAnnotation NoNote {};
        FOR_EACH(DESCRIBE_FIELD, memberValue, staticValue, memberValueReference, staticValueReference, memberMethod, staticMethod)
    };
};

int DescribeFieldTest::staticValue = 0;
int & DescribeFieldTest::staticValueReference = DescribeFieldTest::staticValue;

TEST(ReflectTest, RfMacroDescribeField)
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
    using MemberValueReferencePointer = nullptr_t;
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

    EXPECT_STREQ("memberValue", &DescribeFieldTest::Class::memberValue_::nameStr.value[0]);
    EXPECT_STREQ("staticValue", &DescribeFieldTest::Class::staticValue_::nameStr.value[0]);
    EXPECT_STREQ("memberValueReference", &DescribeFieldTest::Class::memberValueReference_::nameStr.value[0]);
    EXPECT_STREQ("staticValueReference", &DescribeFieldTest::Class::staticValueReference_::nameStr.value[0]);
    EXPECT_STREQ("memberMethod", &DescribeFieldTest::Class::memberMethod_::nameStr.value[0]);
    EXPECT_STREQ("staticMethod", &DescribeFieldTest::Class::staticMethod_::nameStr.value[0]);
    
    std::string memberValueTypeStr(&DescribeFieldTest::Class::memberValue_::typeStr.value[0]);
    std::string staticValueTypeStr(&DescribeFieldTest::Class::staticValue_::typeStr.value[0]);
    std::string memberValueReferenceTypeStr(&DescribeFieldTest::Class::memberValueReference_::typeStr.value[0]);
    std::string staticValueReferenceTypeStr(&DescribeFieldTest::Class::staticValueReference_::typeStr.value[0]);
    std::string memberMethodTypeStr(&DescribeFieldTest::Class::memberMethod_::typeStr.value[0]);
    std::string staticMethodTypeStr(&DescribeFieldTest::Class::staticMethod_::typeStr.value[0]);
    
    EXPECT_TRUE(std::regex_match(memberValueTypeStr.c_str(), std::regex("int")));
    EXPECT_TRUE(std::regex_match(staticValueTypeStr.c_str(), std::regex("int")));
    EXPECT_TRUE(std::regex_match(memberValueReferenceTypeStr.c_str(), std::regex("int\\s*&")));
    EXPECT_TRUE(std::regex_match(staticValueReferenceTypeStr.c_str(), std::regex("int\\s*&")));
    EXPECT_TRUE(std::regex_match(memberMethodTypeStr.c_str(), std::regex(".*void.*")));
    EXPECT_TRUE(std::regex_match(staticMethodTypeStr.c_str(), std::regex(".*void.*")));
    
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
    
    isSame = std::is_same<MemberValueNote, DescribeFieldTest::Class::memberValue_::NoteType>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<StaticValueNote, DescribeFieldTest::Class::staticValue_::NoteType>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<MemberValueReferenceNote, DescribeFieldTest::Class::memberValueReference_::NoteType>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<StaticValueReferenceNote, DescribeFieldTest::Class::staticValueReference_::NoteType>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<MemberMethodNote, DescribeFieldTest::Class::memberMethod_::NoteType>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<StaticMethodNote, DescribeFieldTest::Class::staticMethod_::NoteType>::value;
    EXPECT_TRUE(isSame);
    
    using MemberValueField = DescribeFieldTest::Class::memberValue_::Field;
    using StaticValueField = DescribeFieldTest::Class::staticValue_::Field;
    using MemberValueReferenceField = DescribeFieldTest::Class::memberValueReference_::Field;
    using StaticValueReferenceField = DescribeFieldTest::Class::staticValueReference_::Field;
    using MemberMethodField = DescribeFieldTest::Class::memberMethod_::Field;
    using StaticMethodField = DescribeFieldTest::Class::staticMethod_::Field;
    
    using ExpectedMemberValueField = Fields::Field<decltype(DescribeFieldTest::memberValue), decltype(&DescribeFieldTest::memberValue), 0, decltype(DescribeFieldTest::Class::NoNote)>;
    using ExpectedStaticValueField = Fields::Field<decltype(DescribeFieldTest::staticValue), decltype(&DescribeFieldTest::staticValue), 1, decltype(DescribeFieldTest::Class::NoNote)>;
    using ExpectedMemberValueReferenceField = Fields::Field<decltype(DescribeFieldTest::memberValueReference), nullptr_t, 2, decltype(DescribeFieldTest::memberValueReference_note)>;
    using ExpectedStaticValueReferenceField = Fields::Field<decltype(DescribeFieldTest::staticValueReference), decltype(&DescribeFieldTest::staticValueReference), 3, decltype(DescribeFieldTest::staticValueReference_note)>;
    using ExpectedMemberMethodField = Fields::Field<decltype(&DescribeFieldTest::memberMethod), decltype(&DescribeFieldTest::memberMethod), 4, decltype(DescribeFieldTest::Class::NoNote)>;
    using ExpectedStaticMethodField = Fields::Field<decltype(DescribeFieldTest::staticMethod), decltype(&DescribeFieldTest::staticMethod), 5, decltype(DescribeFieldTest::Class::NoNote)>;
    
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
    isSame = std::is_same_v<nullptr_t, MemberValueReferenceField::Pointer>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<nullptr_t, StaticValueReferenceField::Pointer>;
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

class GetFieldTest {
public:
    int first;
    float second;

    struct Class {
        static constexpr NoAnnotation NoNote {};
        struct first_ { static constexpr Fields::Field<decltype(first), decltype(&GetFieldTest::first), 0, NoAnnotation> field =
            { "first", "int", &GetFieldTest::first, NoNote }; };
        struct second_ { static constexpr Fields::Field<decltype(second), decltype(&GetFieldTest::second), 1, NoAnnotation> field =
            { "second", "float", &GetFieldTest::second, NoNote }; };
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

    EXPECT_STREQ("second", GetFieldTest::Class::Fields[1].name);
    EXPECT_STREQ("float", GetFieldTest::Class::Fields[1].typeStr);
}

struct UseFieldTest {
    int first;
    float second;

    struct Class {
        enum_t(IndexOf, size_t, { first, second });
        static constexpr NoAnnotation NoNote {};
        struct first_ { static constexpr Fields::Field<int, decltype(&UseFieldTest::first), 0> field = { "first", "int", &UseFieldTest::first, NoNote }; };
        struct second_ { static constexpr Fields::Field<float, decltype(&UseFieldTest::second), 1> field = { "second", "float", &UseFieldTest::second, NoNote }; };
        template <typename Function>
        constexpr static void ForEachField(Function function) {
            USE_FIELD(first)
            USE_FIELD(second)
        }
    };
};

TEST(ReflectTest, RfMacroUseField)
{
    UseFieldTest useFieldTest = { 1, 1.1f };
    size_t index = 0;
    UseFieldTest::Class::ForEachField([&](auto & field) {
        using Field = typename std::remove_reference<decltype(field)>::type;
        EXPECT_EQ(index, Field::Index);
        switch ( index ) {
        case 0:
            EXPECT_STREQ("first", field.name);
            EXPECT_STREQ("int", field.typeStr);
            break;
        case 1:
            EXPECT_STREQ("second", field.name);
            EXPECT_STREQ("float", field.typeStr);
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
            struct first_ { static constexpr Fields::Field<int, decltype(&UseFieldAtTest::first), 0> field = { "first", "int", &UseFieldAtTest::first, NoNote }; };
            struct second_ { static constexpr Fields::Field<float, decltype(&UseFieldAtTest::second), 1> field = { "second", "float", &UseFieldAtTest::second, NoNote }; };
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
        visited = true;
    });
    EXPECT_TRUE(visited);
    
    visited = false;
    UseFieldAtTest::Class::FieldAt(1, [&](auto & field) {
        EXPECT_STREQ("second", field.name);
        EXPECT_STREQ("float", field.typeStr);
        visited = true;
    });
    EXPECT_TRUE(visited);
}

class CumulativeMacroTest {
public:
    int first;
    float second;

    struct Class {
        using ClassType = CumulativeMacroTest;
        enum_t(IndexOf, size_t, {
            FOR_EACH(GET_FIELD_NAME, first, second)
        });
        static constexpr NoAnnotation NoNote {};
        using Annotations = decltype(NoNote);
        static constexpr Annotations & annotations = NoNote;
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
        template <typename Function> static void FieldAt(size_t fieldIndex, Function function) {
            switch ( fieldIndex ) {
                FOR_EACH(USE_FIELD_AT, first, second)
            }
        }
        template <typename Function> static void FieldAt(CumulativeMacroTest & object, size_t fieldIndex, Function function) {
            switch ( fieldIndex ) {
                FOR_EACH(USE_FIELD_VALUE_AT, first, second)
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
    REFLECT_EMPTY(is_reflected_REFLECT_EMPTY)
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
    hasClass = decltype(typeHasReflection<is_reflected_REFLECT_EMPTY>(0))::value;
    EXPECT_TRUE(hasClass);
}

TEST(ExtendedTypeSupportTest, IsReflected)
{
    EXPECT_FALSE(is_reflected<does_not_have_reflected_class>::value);
    EXPECT_FALSE(is_reflected<does_not_quite_have_reflected_class>::value);
    EXPECT_TRUE(is_reflected<does_have_reflected_class>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT_NOTED>::value);
    EXPECT_TRUE(is_reflected<is_reflected_REFLECT_EMPTY>::value);
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
    void memberMethod() {}
    static void staticMethod() {}

    REFLECT_NOTED(ReflectObj, primitive, object, primitiveArray, map, objCollection, stack, staticPrimitive, primitiveReference, staticPrimitiveReference, memberMethod, staticMethod)
};

int ReflectObj::staticPrimitive = 0;
int & ReflectObj::staticPrimitiveReference = ReflectObj::staticPrimitive;

TEST(ReflectTest, RfMacroReflectNoted)
{
    EXPECT_TRUE(is_reflected<ReflectSuperObj>::value);
    EXPECT_TRUE(is_reflected<ReflectSubObj>::value);
    EXPECT_TRUE(is_reflected<ReflectObj>::value);

    EXPECT_EQ(0, ReflectObj::Class::IndexOf::primitive);
    EXPECT_EQ(1, ReflectObj::Class::IndexOf::object);
    EXPECT_EQ(2, ReflectObj::Class::IndexOf::primitiveArray);
    EXPECT_EQ(3, ReflectObj::Class::IndexOf::map);
    EXPECT_EQ(4, ReflectObj::Class::IndexOf::objCollection);
    EXPECT_EQ(5, ReflectObj::Class::IndexOf::stack);
    EXPECT_EQ(6, ReflectObj::Class::IndexOf::staticPrimitive);
    EXPECT_EQ(7, ReflectObj::Class::IndexOf::primitiveReference);
    EXPECT_EQ(8, ReflectObj::Class::IndexOf::staticPrimitiveReference);
    EXPECT_EQ(9, ReflectObj::Class::IndexOf::memberMethod);
    EXPECT_EQ(10, ReflectObj::Class::IndexOf::staticMethod);
    
    EXPECT_STREQ("primitive", ReflectObj::Class::primitive_::nameStr.value);
    EXPECT_STREQ("object", ReflectObj::Class::object_::nameStr.value);
    EXPECT_STREQ("primitiveArray", ReflectObj::Class::primitiveArray_::nameStr.value);
    EXPECT_STREQ("map", ReflectObj::Class::map_::nameStr.value);
    EXPECT_STREQ("objCollection", ReflectObj::Class::objCollection_::nameStr.value);
    EXPECT_STREQ("stack", ReflectObj::Class::stack_::nameStr.value);
    EXPECT_STREQ("staticPrimitive", ReflectObj::Class::staticPrimitive_::nameStr.value);
    EXPECT_STREQ("primitiveReference", ReflectObj::Class::primitiveReference_::nameStr.value);
    EXPECT_STREQ("staticPrimitiveReference", ReflectObj::Class::staticPrimitiveReference_::nameStr.value);
    EXPECT_STREQ("memberMethod", ReflectObj::Class::memberMethod_::nameStr.value);
    EXPECT_STREQ("staticMethod", ReflectObj::Class::staticMethod_::nameStr.value);

    EXPECT_EQ(11, ReflectObj::Class::TotalFields);
    EXPECT_EQ(3, ReflectObj::Class::TotalStaticFields);

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
    typeStr = ReflectObj::Class::memberMethod_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("void") != std::string::npos);
    typeStr = ReflectObj::Class::staticMethod_::typeStr.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("void") != std::string::npos);
    
    EXPECT_STREQ("primitive", ReflectObj::Class::primitive_::field.name);
    EXPECT_STREQ("object", ReflectObj::Class::object_::field.name);
    EXPECT_STREQ("primitiveArray", ReflectObj::Class::primitiveArray_::field.name);
    EXPECT_STREQ("map", ReflectObj::Class::map_::field.name);
    EXPECT_STREQ("objCollection", ReflectObj::Class::objCollection_::field.name);
    EXPECT_STREQ("stack", ReflectObj::Class::stack_::field.name);
    EXPECT_STREQ("staticPrimitive", ReflectObj::Class::staticPrimitive_::field.name);
    EXPECT_STREQ("primitiveReference", ReflectObj::Class::primitiveReference_::field.name);
    EXPECT_STREQ("staticPrimitiveReference", ReflectObj::Class::staticPrimitiveReference_::field.name);
    EXPECT_STREQ("memberMethod", ReflectObj::Class::memberMethod_::field.name);
    EXPECT_STREQ("staticMethod", ReflectObj::Class::staticMethod_::field.name);

    EXPECT_STREQ(ReflectObj::Class::primitive_::typeStr.value, ReflectObj::Class::primitive_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::object_::typeStr.value, ReflectObj::Class::object_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::typeStr.value, ReflectObj::Class::primitiveArray_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::map_::typeStr.value, ReflectObj::Class::map_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::typeStr.value, ReflectObj::Class::objCollection_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::stack_::typeStr.value, ReflectObj::Class::stack_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitive_::typeStr.value, ReflectObj::Class::staticPrimitive_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveReference_::typeStr.value, ReflectObj::Class::primitiveReference_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitiveReference_::typeStr.value, ReflectObj::Class::staticPrimitiveReference_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::memberMethod_::typeStr.value, ReflectObj::Class::memberMethod_::field.typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticMethod_::typeStr.value, ReflectObj::Class::staticMethod_::field.typeStr);
    
    EXPECT_STREQ(ReflectObj::Class::primitive_::field.name, ReflectObj::Class::Fields[0].name);
    EXPECT_STREQ(ReflectObj::Class::object_::field.name, ReflectObj::Class::Fields[1].name);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::field.name, ReflectObj::Class::Fields[2].name);
    EXPECT_STREQ(ReflectObj::Class::map_::field.name, ReflectObj::Class::Fields[3].name);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::field.name, ReflectObj::Class::Fields[4].name);
    EXPECT_STREQ(ReflectObj::Class::stack_::field.name, ReflectObj::Class::Fields[5].name);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitive_::field.name, ReflectObj::Class::Fields[6].name);
    EXPECT_STREQ(ReflectObj::Class::primitiveReference_::field.name, ReflectObj::Class::Fields[7].name);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitiveReference_::field.name, ReflectObj::Class::Fields[8].name);
    EXPECT_STREQ(ReflectObj::Class::memberMethod_::field.name, ReflectObj::Class::Fields[9].name);
    EXPECT_STREQ(ReflectObj::Class::staticMethod_::field.name, ReflectObj::Class::Fields[10].name);
    
    EXPECT_STREQ(ReflectObj::Class::primitive_::field.typeStr, ReflectObj::Class::Fields[0].typeStr);
    EXPECT_STREQ(ReflectObj::Class::object_::field.typeStr, ReflectObj::Class::Fields[1].typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveArray_::field.typeStr, ReflectObj::Class::Fields[2].typeStr);
    EXPECT_STREQ(ReflectObj::Class::map_::field.typeStr, ReflectObj::Class::Fields[3].typeStr);
    EXPECT_STREQ(ReflectObj::Class::objCollection_::field.typeStr, ReflectObj::Class::Fields[4].typeStr);
    EXPECT_STREQ(ReflectObj::Class::stack_::field.typeStr, ReflectObj::Class::Fields[5].typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitive_::field.typeStr, ReflectObj::Class::Fields[6].typeStr);
    EXPECT_STREQ(ReflectObj::Class::primitiveReference_::field.typeStr, ReflectObj::Class::Fields[7].typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticPrimitiveReference_::field.typeStr, ReflectObj::Class::Fields[8].typeStr);
    EXPECT_STREQ(ReflectObj::Class::memberMethod_::field.typeStr, ReflectObj::Class::Fields[9].typeStr);
    EXPECT_STREQ(ReflectObj::Class::staticMethod_::field.typeStr, ReflectObj::Class::Fields[10].typeStr);
    
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
    EXPECT_EQ(11, index);
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
