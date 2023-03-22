#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <string>

inline namespace builder_simple
{
    struct MyObj
    {
        int myInt;
        std::string myString;

        REFLECT(MyObj, myInt, myString)
    };

    void builderSimple()
    {
        auto myObj = RareBuilder<MyObj>().myInt(1234).myString("asdf").build();
        std::cout << Json::pretty(myObj) << std::endl;
    }
}
