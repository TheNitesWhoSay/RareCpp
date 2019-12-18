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

#endif