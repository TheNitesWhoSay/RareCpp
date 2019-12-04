#ifndef MAIN_H
#define MAIN_H
#include "../CppRandomAccessReflectionLib/Json.h"
#include <iostream>
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
    static const std::unordered_map<std::string, TestEnum> TestEnumCacheCustom;

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
    REFLECT((Parents) A, () testEnum, (Reflected) composed, () first, () second,
        () ptr, () boolean, (Json::String) str, (Json::String) map, () vecVec, () ray)
};

struct EnhancedContext : public Json::Context
{
    virtual ~EnhancedContext() {}

    EnhancedContext(int enhanced) : enhanced(enhanced) {}

    int enhanced;
};

// In a .cpp file you can only use explicit instantiation (as below in From, or just specifying explicit template arguments as below in To)
// In a .h or .hpp file you could use proper template arguments (e.g. have the method apply to any index, or to the same type in any object at any index)
/**/

template <size_t FieldIndex>
struct Json::Input::Customize<A, A::TestEnum, FieldIndex>
{
    static bool As(std::istream & is, Context & context, const A & object, A::TestEnum & value)
    {
        std::string input = Json::Read::String(is);
        auto found = A::TestEnumCacheCustom.find(input);
        if ( found != A::TestEnumCacheCustom.end() )
        {
            value = found->second;
            return true;
        }
        else
        {
            return false;
        }
    }
};

template <>
struct Json::Input::CustomizeType<A>
{
    static bool As(std::istream & is, Context & context, A & object)
    {
        /*char c = '\0';
        Json::Checked::get(is, c, '{', "object opening \"{\"");
        if ( !Json::Checked::tryGet(is, '}', "object closing \"}\" or field name opening \"") )
        {
            do
            {
                std::string fieldName = Read::FieldName(is, c);
                Read::Field(is, c, object, fieldName);
            }
            while ( Checked::get(is, ',', '}', "\",\" or object closing \"}\"") );
        }*/
        return false;
    }
};

template <>
struct Json::Output::Customize<A, A::TestEnum>
{
    static bool As(std::ostream & os, Context & context, const A & object, const A::TestEnum & value)
    {
        EnhancedContext & enhanced = dynamic_cast<EnhancedContext &>(context);
        switch ( value )
        {
            case A::TestEnum::first: Json::Put::String(os, context, "firstCustom" + std::to_string(enhanced.enhanced)); return true;
            case A::TestEnum::second: Json::Put::String(os, context, "secondCustom" + std::to_string(enhanced.enhanced)); return true;
        }
        return false;
    }
};

#endif