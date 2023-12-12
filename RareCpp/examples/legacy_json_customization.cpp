#include "json_customization.h"
#include <rarecpp/json.h>
#include <iostream>
#include <limits>
using Json::Statics;

int A::second = 0;

const std::unordered_map<std::string, A::TestEnum> A::TestEnumCache {
    { "first", A::TestEnum::first },
    { "second", A::TestEnum::second }
};

const std::unordered_map<std::string, A::TestEnum> A::TestEnumCacheCustom {
    { "firstCustom", A::TestEnum::first },
    { "secondCustom", A::TestEnum::second }
};

Json::OutStreamType & operator<<(Json::OutStreamType & os, const A::TestEnum & testEnum)
{
    switch ( testEnum )
    {
        case A::TestEnum::first: Json::Put::string(os, "first"); break;
        case A::TestEnum::second: Json::Put::string(os, "second"); break;
    }
    return os;
}

std::istream & operator>>(std::istream & is, A::TestEnum & testEnum)
{
    std::string input = Json::Read::string<>(is);
    auto found = A::TestEnumCache.find(input);
    if ( found != A::TestEnumCache.end() )
        testEnum = found->second;

    return is;
}

A a;

void jsonCustomizationExample()
{
    try {
        std::cout << "Enter Class A:" << std::endl;
        std::cin >> Json::in(a);
    } catch ( Json::Exception & e ) {
        std::cout << std::endl << "Exception: " << e.what() << std::endl;
    }

    if ( a.putCache )
    {
        Json::putClassFieldCache(std::cout);
        std::cout << "..." << std::endl;
    }
    std::cout << "Read in: " << Json::pretty<Statics::Included>(a, std::make_shared<EnhancedContext>(1337)) << std::endl;
    std::cout << "..." << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
