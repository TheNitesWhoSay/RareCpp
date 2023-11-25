#include <rarecpp/reflect.h>
#include <iostream>
#include <string_view>

inline namespace unowned_private_reflect
{
    class MyObj
    {
        const char a[5] = "asdf";
        float b = 39.1f;
        static constexpr int c = 42;
    };
}

// REFLECT_PRIVATE is for reflecting objects you don't own which have private members, this is a common situation for objects from included libraries
// The REFLECT_PRIVATE macro must be used in the global scope and while syntatically identical to the in-class REFLECT macro it has additional limitations
// Namely it must be possible to create a pointer to the member, ergo reference members and overloaded members cannot be reflected and you can't get offsets
// REFLECT_PRIVATE does not involve casting/UB, it uses the standard-legal private-member exfoliation trick
REFLECT_PRIVATE(unowned_private_reflect::MyObj, a, b, c)

inline namespace unowned_private_reflect
{
    void unownedPrivateReflect()
    {
        MyObj myObj {};
        
        // Can be quite useful for whitebox-testing
        static_assert(RareTs::whitebox(myObj).c == 42);
        RareTs::whitebox(myObj).b = 133.7f;
        
        // Or just regular serialization-like purposes
        RareTs::Members<MyObj>::forEach(myObj, [&](auto member, auto & value) {
            std::cout << RareTs::toStr<typename decltype(member)::type>() << " " << member.name << " = " << value << ";" << std::endl;
        });
    }
}
