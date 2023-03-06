#ifndef JSONINPUTTEST_H
#define JSONINPUTTEST_H
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
using namespace RareTs;

struct CustomizeUnspecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(CustomizeUnspecialized, firstField, secondField, unspecialized)
};

struct CustomizeFullySpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(CustomizeFullySpecialized, firstField, secondField, unspecialized)
};

template <size_t MemberIndex, typename OpAnnotations, typename Member>
struct Json::Input::Customize<CustomizeFullySpecialized, int, MemberIndex, OpAnnotations, Member>
{
    static bool as(std::istream &, Context &, CustomizeFullySpecialized &, int &) { return true; }
};

struct Customize5Args_OpAnnotationsDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize5Args_OpAnnotationsDefaulted, firstField, secondField, unspecialized)
};

template <size_t MemberIndex, typename Member>
struct Json::Input::Customize<Customize5Args_OpAnnotationsDefaulted, int, MemberIndex, NoNote, Member>
{
    static bool as(std::istream &, Context &, Customize5Args_OpAnnotationsDefaulted &, int &) { return true; }
};

struct Customize5Args_MemberIndexDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize5Args_MemberIndexDefaulted, firstField, secondField, unspecialized)
};

template <typename OpAnnotations, typename Member>
struct Json::Input::Customize<Customize5Args_MemberIndexDefaulted, int, Json::noMemberIndex, OpAnnotations, Member>
{
    static bool as(std::istream &, Context &, Customize5Args_MemberIndexDefaulted &, int &) { return true; }
};

struct Customize5Args_BothDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize5Args_BothDefaulted, firstField, secondField, unspecialized)
};

template <typename Member>
struct Json::Input::Customize<Customize5Args_BothDefaulted, int, Json::noMemberIndex, NoNote, Member>
{
    static bool as(std::istream &, Context &, Customize5Args_BothDefaulted &, int &) { return true; }
};

struct Customize4Args
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize4Args, firstField, secondField, unspecialized)
};

template <size_t MemberIndex, typename OpAnnotations>
struct Json::Input::Customize<Customize4Args, int, MemberIndex, OpAnnotations>
{
    static bool as(std::istream &, Context &, Customize4Args &, int &) { return true; }
};

struct Customize4Args_MemberIndexDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize4Args_MemberIndexDefaulted, firstField, secondField, unspecialized)
};

template <typename OpAnnotations>
struct Json::Input::Customize<Customize4Args_MemberIndexDefaulted, int, Json::noMemberIndex, OpAnnotations>
{
    static bool as(std::istream &, Context &, Customize4Args_MemberIndexDefaulted &, int &) { return true; }
};

struct Customize3Args
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize3Args, firstField, secondField, unspecialized)
};

template <size_t MemberIndex>
struct Json::Input::Customize<Customize3Args, int, MemberIndex>
{
    static bool as(std::istream &, Context &, Customize3Args &, int &) { return true; }
};

struct Customize2Args
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(Customize2Args, firstField, secondField, unspecialized)
};

template <>
struct Json::Input::Customize<Customize2Args, int>
{
    static bool as(std::istream &, Context &, Customize2Args &, int &) { return true; }
};

struct CustomizeTypeFullySpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(CustomizeTypeFullySpecialized, firstField, secondField, unspecialized)
};

template <typename OpAnnotations, typename Member>
struct Json::Input::CustomizeType<CustomizeTypeFullySpecialized, OpAnnotations, Member>
{
    static bool as(std::istream & input, Context &, CustomizeTypeFullySpecialized & value)
    {
        input >> value.firstField;
        return true;
    };
};

struct CustomizeType3Args_OpAnnotationsDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(CustomizeType3Args_OpAnnotationsDefaulted, firstField, secondField, unspecialized)
};

template <typename Member>
struct Json::Input::CustomizeType<CustomizeType3Args_OpAnnotationsDefaulted, NoNote, Member>
{
    static bool as(std::istream &, Context &, CustomizeType3Args_OpAnnotationsDefaulted &) { return true; }
};

struct CustomizeType2Args
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(CustomizeType2Args, firstField, secondField, unspecialized)
};

template <typename OpAnnotations>
struct Json::Input::CustomizeType<CustomizeType2Args, OpAnnotations>
{
    static bool as(std::istream &, Context &, CustomizeType2Args &) { return true; }
};

struct CustomizeType1Arg
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(CustomizeType1Arg, firstField, secondField, unspecialized)
};

template <>
struct Json::Input::CustomizeType<CustomizeType1Arg>
{
    static bool as(std::istream &, Context &, CustomizeType1Arg &) { return true; }
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