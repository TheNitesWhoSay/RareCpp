#ifndef JSON_INPUT_TEST
#define JSON_INPUT_TEST
#include "../CppRandomAccessReflectionLib/Reflect.h"
#include "../CppRandomAccessReflectionLib/Json.h"
using namespace Reflect;

struct CustomizeFullySpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() CustomizeFullySpecialized, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex, typename OpAnnotations, typename Field>
struct Json::Input::Customize<CustomizeFullySpecialized, int, FieldIndex, OpAnnotations, Field>
{
    static bool As(std::istream & input, Context & context, CustomizeFullySpecialized & object, int & value) { return true; }
};

struct Customize5Args_OpAnnotationsDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize5Args_OpAnnotationsDefaulted, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex, typename Field>
struct Json::Input::Customize<Customize5Args_OpAnnotationsDefaulted, int, FieldIndex, Annotate<>, Field>
{
    static bool As(std::istream & input, Context & context, Customize5Args_OpAnnotationsDefaulted & object, int & value) { return true; }
};

struct Customize5Args_FieldIndexDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize5Args_FieldIndexDefaulted, () firstField, () secondField, () unspecialized)
};

template <typename OpAnnotations, typename Field>
struct Json::Input::Customize<Customize5Args_FieldIndexDefaulted, int, Json::NoFieldIndex, OpAnnotations, Field>
{
    static bool As(std::istream & input, Context & context, Customize5Args_FieldIndexDefaulted & object, int & value) { return true; }
};

struct Customize5Args_BothDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize5Args_BothDefaulted, () firstField, () secondField, () unspecialized)
};

template <typename Field>
struct Json::Input::Customize<Customize5Args_BothDefaulted, int, Json::NoFieldIndex, Annotate<>, Field>
{
    static bool As(std::istream & input, Context & context, Customize5Args_BothDefaulted & object, int & value) { return true; }
};

struct Customize4Args
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize4Args, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex, typename OpAnnotations>
struct Json::Input::Customize<Customize4Args, int, FieldIndex, OpAnnotations>
{
    static bool As(std::istream & input, Context & context, Customize4Args & object, int & value) { return true; }
};

struct Customize4Args_FieldIndexDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize4Args_FieldIndexDefaulted, () firstField, () secondField, () unspecialized)
};

template <typename OpAnnotations>
struct Json::Input::Customize<Customize4Args_FieldIndexDefaulted, int, Json::NoFieldIndex, OpAnnotations>
{
    static bool As(std::istream & input, Context & context, Customize4Args_FieldIndexDefaulted & object, int & value) { return true; }
};

struct Customize3Args
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize3Args, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex>
struct Json::Input::Customize<Customize3Args, int, FieldIndex>
{
    static bool As(std::istream & input, Context & context, Customize3Args & object, int & value) { return true; }
};

struct Customize2Args
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize2Args, () firstField, () secondField, () unspecialized)
};

template <>
struct Json::Input::Customize<Customize2Args, int>
{
    static bool As(std::istream & input, Context & context, Customize2Args & object, int & value) { return true; }
};

struct CustomizeTypeFullySpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() CustomizeTypeFullySpecialized, () firstField, () secondField, () unspecialized)
};

template <typename OpAnnotations, typename Field>
struct Json::Input::CustomizeType<CustomizeTypeFullySpecialized, OpAnnotations, Field>
{
    static bool As(std::istream & input, Context & context, const CustomizeTypeFullySpecialized & value) { return true; };
};

struct Customize3Args_OpAnnotationsDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize3Args_OpAnnotationsDefaulted, () firstField, () secondField, () unspecialized)
};

template <typename Field>
struct Json::Input::CustomizeType<Customize3Args_OpAnnotationsDefaulted, Annotate<>, Field>
{
    static bool As(std::istream & input, Context & context, const Customize3Args_OpAnnotationsDefaulted & value) { return true; }
};

struct CustomizeType2Args
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() CustomizeType2Args, () firstField, () secondField, () unspecialized)
};

template <typename OpAnnotations>
struct Json::Input::CustomizeType<CustomizeType2Args, OpAnnotations>
{
    static bool As(std::istream & input, Context & context, const CustomizeType2Args & value) { return true; }
};

struct CustomizeType1Arg
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() CustomizeType1Arg, () firstField, () secondField, () unspecialized)
};

template <>
struct Json::Input::CustomizeType<CustomizeType1Arg>
{
    static bool As(std::istream & input, Context & context, const CustomizeType1Arg & value) { return true; }
};

#endif