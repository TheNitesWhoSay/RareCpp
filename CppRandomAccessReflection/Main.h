#ifndef MAIN_H
#define MAIN_H
#include "../CppRandomAccessReflectionLib/Json.h"
using namespace Reflect;
using u8 = uint8_t;

class SuperA {
public:
    SuperA() : superVal(0) {}

    int superVal;

    REFLECT(() SuperA, () superVal)
};

class OtherSuperA {
public:
    REFLECT_EMPTY(() OtherSuperA)
};

class Composed {
public:
    Composed() : composedVal(0) {}

    int composedVal;

    REFLECT(() Composed, () composedVal)
};

class A : public SuperA, public OtherSuperA {
public:
    enum_t(TestEnum, u8, {
        first,
        second
    });
    static const std::unordered_map<std::string, TestEnum> TestEnumCache;

    A() : testEnum(TestEnum::first), first(0), ptr(nullptr), composed(), boolean(false), str("") { ray[0] = 0; ray[1] = 0; }

    TestEnum testEnum;
    int first;
    static int second;
    int* ptr;
    Composed composed;
    bool boolean;
    std::string str;
    std::map<std::string, std::string> map;
    std::vector<std::vector<int>> vecVec;
    int ray[2];

    class NestedClass
    {
    public:
        int nestedVal;

        REFLECT(() NestedClass, () nestedVal)
    };

    using Parents = Inherit<SuperA, OtherSuperA>;
    REFLECT((Parents) A, (Json::Custom) testEnum, (Reflected) composed, () first, () second,
        () ptr, () boolean, (Json::String) str, (Json::String) map, () vecVec, () ray)
};

// In a .cpp file you can only use explicit instantiation (as below in From, or just specifying explicit template arguments as below in To)
// In a .h or .hpp file you could use proper template arguments (e.g. have the method apply to any index, or to the same type in any object at any index)
/**/

template <size_t FieldIndex>
struct Json::CustomizeInput<A, A::TestEnum, FieldIndex>
{
    static bool As(std::istream & is, const A & object, A::TestEnum & value)
    {
        std::string input = Json::Input<A>::getString(is);
        auto found = A::TestEnumCache.find(input);
        if ( found != A::TestEnumCache.end() )
        {
            value = found->second;
            return true;
        }
        else
            return false;
    }
};

template <size_t FieldIndex>
struct Json::CustomizeOutput<A, A::TestEnum, FieldIndex>
{
    static bool As(std::ostream & os, const A & object, const A::TestEnum & value)
    {
        switch ( value )
        {
            case A::TestEnum::first: os << "firstCustom"; return true;
            case A::TestEnum::second: os << "secondCustom"; return true;
        }
        return false;
    }
};

#endif