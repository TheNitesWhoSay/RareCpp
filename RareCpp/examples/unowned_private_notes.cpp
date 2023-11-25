#include <rarecpp/reflect.h>
#include <rarecpp/json.h>
#include <iostream>
#include <string_view>

inline namespace unowned_private_noted
{
    struct MyObj
    {
        const char a[5] = "asdf";
        float b = 39.1f;
        static constexpr int c = 42;
    };
}

// Very similar to REFLECT_PRIVATE except with a slightly different syntax allowing notes to be added to the reflected type or members
// Like REFLECT_PRIVATE, this must be done in the global scope and is purposed for types with private members that you can't place the REFLECT macro in
REFLECT_PRIVATE_NOTED(
    (unowned_private_noted::MyObj) (RareTs::Buildable),
    (a) (Json::Ignore),
    (b) (Json::Name{"testName"}, Json::Stringify),
    (c) ()
)

inline namespace unowned_private_noted
{
    void unownedPrivateReflectNoted()
    {
        MyObj myObj {};
        RareTs::Members<MyObj>::forEach(myObj, [&](auto member, auto & value) {
            std::cout << RareTs::toStr<typename decltype(member)::type>() << " " << member.name << " = " << value << ";" << std::endl;
        });
        std::cout << Json::pretty(myObj);
    }
}
