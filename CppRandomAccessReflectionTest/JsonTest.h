#ifndef JSONTEST_H
#define JSONTEST_H
#include "../CppRandomAccessReflectionLib/Reflect.h"
#include "../CppRandomAccessReflectionLib/Json.h"
using namespace Reflect;

struct CustomizeNoSpecialization
{
    int integer;
    char character;

    REFLECT(() CustomizeNoSpecialization, () integer, () character)
};

struct CustomizeFullySpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() CustomizeFullySpecialized, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex, typename OpAnnotations, typename Field, Json::Statics statics,
    bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent>
struct Json::Output::Customize<CustomizeFullySpecialized, int, FieldIndex, OpAnnotations, Field, statics,
    PrettyPrint, TotalParentIterables, IndentLevel, indent>
{
    static bool As(std::ostream & output, Json::Context & context, const CustomizeFullySpecialized & object, const int & value)
    {
        return true;
    }
};

struct Customize5ArgSpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize5ArgSpecialized, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex, typename OpAnnotations, typename Field>
struct Json::Output::Customize<Customize5ArgSpecialized, int, FieldIndex, OpAnnotations, Field>
{
    static bool As(std::ostream & output, Json::Context & context, const Customize5ArgSpecialized & object, const int & value)
    {
        return true;
    }
};

struct Customize4ArgSpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize4ArgSpecialized, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex, typename OpAnnotations>
struct Json::Output::Customize<Customize4ArgSpecialized, int, FieldIndex, OpAnnotations>
{
    static bool As(std::ostream & output, Json::Context & context, const Customize4ArgSpecialized & object, const int & value)
    {
        return true;
    }
};

struct Customize3ArgSpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize3ArgSpecialized, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex>
struct Json::Output::Customize<Customize3ArgSpecialized, int, FieldIndex>
{
    static bool As(std::ostream & output, Json::Context & context, const Customize3ArgSpecialized & object, const int & value)
    {
        return true;
    }
};

struct Customize2ArgSpecialized
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize3ArgSpecialized, () firstField, () secondField, () unspecialized)
};

template <>
struct Json::Output::Customize<Customize2ArgSpecialized, int>
{
    static bool As(std::ostream & output, Json::Context & context, const Customize2ArgSpecialized & object, const int & value)
    {
        return true;
    }
};

struct Customize5ArgSpecialized_OpAnnotationsDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize5ArgSpecialized_OpAnnotationsDefaulted, () firstField, () secondField, () unspecialized)
};

template <size_t FieldIndex, typename Field>
struct Json::Output::Customize<Customize5ArgSpecialized_OpAnnotationsDefaulted, int, FieldIndex, Annotate<>, Field>
{
    static bool As(std::ostream & output, Json::Context & context, const Customize5ArgSpecialized_OpAnnotationsDefaulted & object, const int & value)
    {
        return true;
    }
};

struct Customize5ArgSpecialized_FieldIndexDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize5ArgSpecialized_FieldIndexDefaulted, () firstField, () secondField, () unspecialized)
};

template <typename OpAnnotations, typename Field>
struct Json::Output::Customize<Customize5ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex, OpAnnotations, Field>
{
    static bool As(std::ostream & output, Json::Context & context, const Customize5ArgSpecialized_FieldIndexDefaulted & object, const int & value)
    {
        return true;
    }
};

struct Customize5ArgSpecialized_BothDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize5ArgSpecialized_BothDefaulted, () firstField, () secondField, () unspecialized)
};

template <typename Field>
struct Json::Output::Customize<Customize5ArgSpecialized_BothDefaulted, int, Json::NoFieldIndex, Annotate<>, Field>
{
    static bool As(std::ostream & output, Json::Context & context, const Customize5ArgSpecialized_BothDefaulted & object, const int & value)
    {
        return true;
    }
};

struct Customize4ArgSpecialized_FieldIndexDefaulted
{
    int firstField;
    int secondField;
    char unspecialized;

    REFLECT(() Customize4ArgSpecialized_FieldIndexDefaulted, () firstField, () secondField, () unspecialized)
};

template <typename OpAnnotations>
struct Json::Output::Customize<Customize4ArgSpecialized_FieldIndexDefaulted, int, Json::NoFieldIndex, OpAnnotations>
{
    static bool As(std::ostream & output, Json::Context & context, const Customize4ArgSpecialized_FieldIndexDefaulted & object, const int & value)
    {
        return true;
    }
};

enum_t(UnspecializedType, uint8_t, {
   one,
   two
});

struct ContainsUnspecialized
{
    UnspecializedType unspecializedType;

    REFLECT(() ContainsUnspecialized, () unspecializedType)
};

enum_t(FullySpecializedType, uint8_t, {
    one,
    two
});

struct ContainsFullySpecialized
{
    FullySpecializedType fullySpecializedType;

    REFLECT(() ContainsFullySpecialized, () fullySpecializedType)
};

template <typename OpAnnotations, typename Field, Json::Statics statics,
    bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent>
struct Json::Output::CustomizeType<FullySpecializedType, OpAnnotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent>
{
    static bool As(std::ostream & output, Json::Context & context, const FullySpecializedType & value)
    {
        return true;
    }
};

enum_t(ThreeArgSpecializedType, uint8_t, {
    one,
    two
});

struct ContainsThreeArgSpecialized
{
    ThreeArgSpecializedType threeArgSpecializedType;

    REFLECT(() ContainsThreeArgSpecialized, () threeArgSpecializedType)
};

template <typename OpAnnotations, typename Field>
struct Json::Output::CustomizeType<ThreeArgSpecializedType, OpAnnotations, Field>
{
    static bool As(std::ostream & output, Json::Context & context, const ThreeArgSpecializedType & value)
    {
        return true;
    }
};

enum_t(TwoArgSpecializedType, uint8_t, {
   one,
   two
});

struct ContainsTwoArgSpecialized
{
    TwoArgSpecializedType twoArgSpecializedType;

    REFLECT(() ContainsTwoArgSpecialized, () twoArgSpecializedType)
};

template<typename OpAnnotations>
struct Json::Output::CustomizeType<TwoArgSpecializedType, OpAnnotations>
{
    static bool As(std::ostream & output, Json::Context & context, const TwoArgSpecializedType & value)
    {
        return true;
    }
};

enum_t(OneArgSpecializedType, uint8_t, {
    one,
    two
});

struct ContainsOneArgSpecialized
{
    OneArgSpecializedType oneArgSpecializedType;

    REFLECT(() ContainsOneArgSpecialized, () oneArgSpecializedType)
};

template <>
struct Json::Output::CustomizeType<OneArgSpecializedType>
{
    static bool As(std::ostream & output, Json::Context & context, const OneArgSpecializedType & value)
    {
        return true;
    }
};

enum_t(ThreeArgSpecializedType_OpAnnotationsDefaulted, uint8_t, {
    one,
    two
});

struct ContainsThreeArgSpecializedType_OpAnnotationsDefaulted
{
    ThreeArgSpecializedType_OpAnnotationsDefaulted threeArgSpecializedType_OpAnnotationsDefaulted;

    REFLECT(() ContainsThreeArgSpecializedType_OpAnnotationsDefaulted, () threeArgSpecializedType_OpAnnotationsDefaulted)
};

template <typename OpAnnotations, typename Field>
struct Json::Output::CustomizeType<ThreeArgSpecializedType_OpAnnotationsDefaulted, OpAnnotations, Field>
{
    static bool As(std::ostream & output, Json::Context & context, const ThreeArgSpecializedType_OpAnnotationsDefaulted & value)
    {
        return true;
    }
};

#endif