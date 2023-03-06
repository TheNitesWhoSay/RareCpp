#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <string>
using Json::Statics;

struct StructWithStatics
{
    static int StaticValue;
};
int StructWithStatics::StaticValue = 2;

int globalValue = 1;
const std::string globalStr = "asdf";
const bool globalBool = true;
const int* globalPtr = nullptr;

struct MyGlobals
{
    static constexpr auto & staticVal = StructWithStatics::StaticValue;
    static constexpr auto & globalValue = ::globalValue;
    static constexpr auto & globalStr = ::globalStr;
    static constexpr auto & globalBool = ::globalBool;
    static constexpr auto & globalPtr = ::globalPtr;

    REFLECT(MyGlobals, staticVal, globalValue, globalStr, globalBool, globalPtr)
};

void globalReflectionExample()
{
    MyGlobals myGlobals;
    std::cout << std::endl << Json::pretty<Statics::Included>(myGlobals) << std::endl << std::endl;
}
