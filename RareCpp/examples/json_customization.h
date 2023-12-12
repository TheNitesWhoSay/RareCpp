#ifndef JSONCUSTOMIZATION_H
#define JSONCUSTOMIZATION_H
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>
using RareTs::Super;

class SuperA {
public:
    SuperA() : superVal(0) {}

    int superVal;

    REFLECT(SuperA, superVal)
};

NOTE(OtherSuperA)
class OtherSuperA {
public:
    REFLECT_NOTED(OtherSuperA)
};

class Composed {
public:
    Composed() : composedVal(0) {}

    int composedVal;

    REFLECT(Composed, composedVal)
};

NOTE(A, Super<SuperA>(Json::Name{"SupA"}), Super<OtherSuperA>)
class A : public SuperA, public OtherSuperA {
public:
    enum class TestEnum { first, second };
    static const std::unordered_map<std::string, TestEnum> TestEnumCache;
    static const std::unordered_map<std::string, TestEnum> TestEnumCacheCustom;

    A() : testEnum(TestEnum::first), first(0), firstReference(first), ptr(nullptr), composed(), boolean(false), putCache(false), customInt(0), str("") { ray[0] = 0; ray[1] = 0; }

    TestEnum testEnum;
    int first;
    int & firstReference;
    static int second;
    static constexpr int & secondReference = second;
    int* ptr;
    Composed composed;
    NOTE(boolean, Json::Ignore)
    bool boolean;
    bool putCache;
    uint16_t customInt;
    NOTE(str, Json::Name{"string"})
    std::string str;
    std::map<std::string, std::string> map;
    std::vector<std::vector<int>> vecVec;
    int ray[2];
    std::shared_ptr<Json::Value> runtime;
    std::vector<std::shared_ptr<int>> autoAllocate;
    std::shared_ptr<Json::Generic::FieldCluster> unknownFields;

    REFLECT_NOTED(A, testEnum, composed, first, firstReference, second, secondReference,
        ptr, boolean, putCache, customInt, str, map, vecVec, ray, runtime, autoAllocate, unknownFields)
};

std::istream & operator>>(std::istream & is, A::TestEnum & testEnum);

template <>
struct Json::Output::Customize<A, uint16_t, RareTs::Reflect<A>::IndexOf::customInt>
{
    static bool as(OutStreamType & os, Context &, const A &, const uint16_t & value)
    {
        if ( value > 9000 )
            Json::Put::string(os, "Over 9000!");

        return value > 9000;
    }
};

struct EnhancedContext : public Json::Context
{
    virtual ~EnhancedContext() {}

    EnhancedContext(int enhanced) : enhanced(enhanced) {}

    int enhanced;
};

// In a .cpp file you can usually only use explicit instantiation for templates (under normal circumstances)
// In a .h or .hpp file you could use proper template arguments (e.g. have the method apply to any index, or to the same type in any object at any index)
template <size_t MemberIndex>
struct Json::Input::Customize<A, A::TestEnum, MemberIndex>
{
    static bool as(std::istream & is, Context &, const A &, A::TestEnum & value)
    {
        std::string input = Json::Read::string<>(is);
        auto found = A::TestEnumCacheCustom.find(input);
        if ( found != A::TestEnumCacheCustom.end() )
        {
            value = found->second;
            return true;
        }
        else
            return false;
    }
};

template <>
struct Json::Input::CustomizeType<A>
{
    static bool as(std::istream & is, Context & context, A & object)
    {
        char c = '\0';
        Json::Read::objectPrefix(is, c);
        if ( !Json::Read::tryObjectSuffix(is) )
        {
            do
            {
                std::string fieldName = Json::Read::fieldName(is, c);
                Json::Read::field(is, context, c, object, fieldName);
            }
            while ( Json::Read::fieldSeparator(is) );
        }
        return true;
    }
};

template <>
struct Json::Output::Customize<A, A::TestEnum>
{
    static bool as(Json::OutStreamType & os, Context & context, const A &, const A::TestEnum & value)
    {
        try {
            EnhancedContext & enhanced = dynamic_cast<EnhancedContext &>(context);
            switch ( value )
            {
                case A::TestEnum::first: Json::Put::string(os, "firstCustom" + std::to_string(enhanced.enhanced)); return true;
                case A::TestEnum::second: Json::Put::string(os, "secondCustom" + std::to_string(enhanced.enhanced)); return true;
            }
            return true;
        } catch ( std::bad_cast & ) {
            return false;
        }
    }
};

#endif