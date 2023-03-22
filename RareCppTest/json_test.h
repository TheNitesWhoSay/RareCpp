#ifndef JSONTEST_H
#define JSONTEST_H
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <rarecpp/string_buffer.h>
using namespace RareTs;
using namespace RareBufferedStream;
using Json::Statics;

struct CustomizeNoSpecialization
{
    int integer;
    char character;

    REFLECT(CustomizeNoSpecialization, integer, character)
};

struct CustomizeFullySpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(CustomizeFullySpecialized, firstField, secondField, unspecialized)
};

template <size_t MemberIndex, typename OpAnnotations, typename Member, Json::Statics statics,
    bool PrettyPrint, size_t IndentLevel, const char* Indent>
struct Json::Output::Customize<CustomizeFullySpecialized, int, MemberIndex, OpAnnotations, Member, statics,
    PrettyPrint, IndentLevel, Indent>
{
    static bool as(Json::OutStreamType & output, Json::Context &, const CustomizeFullySpecialized &, const int & value)
    {
        Json::Put::string(output, "Customized" + std::to_string(value));
        return true;
    }
};

struct Customize5ArgSpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize5ArgSpecialized, firstField, secondField, unspecialized)
};

template <size_t MemberIndex, typename OpAnnotations, typename Member>
struct Json::Output::Customize<Customize5ArgSpecialized, int, MemberIndex, OpAnnotations, Member>
{
    static bool as(Json::OutStreamType &, Json::Context &, const Customize5ArgSpecialized &, const int &)
    {
        return true;
    }
};

struct Customize4ArgSpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize4ArgSpecialized, firstField, secondField, unspecialized)
};

template <size_t MemberIndex, typename OpAnnotations>
struct Json::Output::Customize<Customize4ArgSpecialized, int, MemberIndex, OpAnnotations>
{
    static bool as(Json::OutStreamType &, Json::Context &, const Customize4ArgSpecialized &, const int &)
    {
        return true;
    }
};

struct Customize3ArgSpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize3ArgSpecialized, firstField, secondField, unspecialized)
};

template <size_t MemberIndex>
struct Json::Output::Customize<Customize3ArgSpecialized, int, MemberIndex>
{
    static bool as(Json::OutStreamType &, Json::Context &, const Customize3ArgSpecialized &, const int &)
    {
        return true;
    }
};

struct Customize2ArgSpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize2ArgSpecialized, firstField, secondField, unspecialized)
};

template <>
struct Json::Output::Customize<Customize2ArgSpecialized, int>
{
    static bool as(Json::OutStreamType &, Json::Context &, const Customize2ArgSpecialized &, const int &)
    {
        return true;
    }
};

struct Customize5ArgSpecialized_OpAnnotationsDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize5ArgSpecialized_OpAnnotationsDefaulted, firstField, secondField, unspecialized)
};

template <size_t MemberIndex, typename Member>
struct Json::Output::Customize<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, MemberIndex, NoNote, Member>
{
    static bool as(Json::OutStreamType &, Json::Context &, const Customize5ArgSpecialized_OpAnnotationsDefaulted &, const int &)
    {
        return true;
    }
};

struct Customize5ArgSpecialized_MemberIndexDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize5ArgSpecialized_MemberIndexDefaulted, firstField, secondField, unspecialized)
};

template <typename OpAnnotations, typename Member>
struct Json::Output::Customize<Customize5ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex, OpAnnotations, Member>
{
    static bool as(Json::OutStreamType &, Json::Context &, const Customize5ArgSpecialized_MemberIndexDefaulted &, const int &)
    {
        return true;
    }
};

struct Customize5ArgSpecialized_BothDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize5ArgSpecialized_BothDefaulted, firstField, secondField, unspecialized)
};

template <typename Member>
struct Json::Output::Customize<Customize5ArgSpecialized_BothDefaulted, int, Json::noMemberIndex, NoNote, Member>
{
    static bool as(Json::OutStreamType &, Json::Context &, const Customize5ArgSpecialized_BothDefaulted &, const int &)
    {
        return true;
    }
};

struct Customize4ArgSpecialized_MemberIndexDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize4ArgSpecialized_MemberIndexDefaulted, firstField, secondField, unspecialized)
};

template <typename OpAnnotations>
struct Json::Output::Customize<Customize4ArgSpecialized_MemberIndexDefaulted, int, Json::noMemberIndex, OpAnnotations>
{
    static bool as(Json::OutStreamType &, Json::Context &, const Customize4ArgSpecialized_MemberIndexDefaulted &, const int &)
    {
        return true;
    }
};

enum class UnspecializedType : uint8_t { one, two };

struct ContainsUnspecialized
{
    UnspecializedType unspecializedType;

    REFLECT(ContainsUnspecialized, unspecializedType)
};

enum class FullySpecializedType : uint8_t { one, two };

struct ContainsFullySpecialized
{
    FullySpecializedType fullySpecializedType;

    REFLECT(ContainsFullySpecialized, fullySpecializedType)
};

template <typename OpAnnotations, typename Member, Json::Statics statics,
    bool PrettyPrint, size_t IndentLevel, const char* Indent>
struct Json::Output::CustomizeType<FullySpecializedType, OpAnnotations, Member, statics, PrettyPrint, IndentLevel, Indent>
{
    static bool as(Json::OutStreamType &, Json::Context &, const FullySpecializedType &)
    {
        return true;
    }
};

enum class ThreeArgSpecializedType : uint8_t { one, two };

struct ContainsThreeArgSpecialized
{
    ThreeArgSpecializedType threeArgSpecializedType;

    REFLECT(ContainsThreeArgSpecialized, threeArgSpecializedType)
};

template <typename OpAnnotations, typename Member>
struct Json::Output::CustomizeType<ThreeArgSpecializedType, OpAnnotations, Member>
{
    static bool as(Json::OutStreamType &, Json::Context &, const ThreeArgSpecializedType &)
    {
        return true;
    }
};

enum class TwoArgSpecializedType : uint8_t { one, two };

struct ContainsTwoArgSpecialized
{
    TwoArgSpecializedType twoArgSpecializedType;

    REFLECT(ContainsTwoArgSpecialized, twoArgSpecializedType)
};

template<typename OpAnnotations>
struct Json::Output::CustomizeType<TwoArgSpecializedType, OpAnnotations>
{
    static bool as(Json::OutStreamType &, Json::Context &, const TwoArgSpecializedType &)
    {
        return true;
    }
};

enum class OneArgSpecializedType : uint8_t { one, two };

struct ContainsOneArgSpecialized
{
    OneArgSpecializedType oneArgSpecializedType;

    REFLECT(ContainsOneArgSpecialized, oneArgSpecializedType)
};

template <>
struct Json::Output::CustomizeType<OneArgSpecializedType>
{
    static bool as(Json::OutStreamType &, Json::Context &, const OneArgSpecializedType &)
    {
        return true;
    }
};

enum class ThreeArgSpecializedType_OpAnnotationsDefaulted : uint8_t { one, two };

struct ContainsThreeArgSpecializedType_OpAnnotationsDefaulted
{
    ThreeArgSpecializedType_OpAnnotationsDefaulted threeArgSpecializedType_OpAnnotationsDefaulted;

    REFLECT(ContainsThreeArgSpecializedType_OpAnnotationsDefaulted, threeArgSpecializedType_OpAnnotationsDefaulted)
};

template <typename OpAnnotations, typename Member>
struct Json::Output::CustomizeType<ThreeArgSpecializedType_OpAnnotationsDefaulted, OpAnnotations, Member>
{
    static bool as(Json::OutStreamType &, Json::Context &, const ThreeArgSpecializedType_OpAnnotationsDefaulted &)
    {
        return true;
    }
};

struct ProxyReflectedObject
{
    int a;
};

template <> struct RareTs::Proxy<ProxyReflectedObject> : public ProxyReflectedObject
{
    REFLECT(RareTs::Proxy<ProxyReflectedObject>, a)
};

#endif