#include <rarecpp/reflect.h>
#include <iostream>
#include <string_view>

inline namespace overload_non_noted_argument_sets
{
    struct MyObj
    {
        void foo(int i, float f) {
            std::cout << "  called foo(int, float) with: " << i << ", " << f << std::endl;
        }

        void foo(std::string_view s) {
            std::cout << "  called foo(std::string_view) with: " << s << std::endl;
        }

        REFLECT(MyObj, foo)
    };

    void overloadNonNotedArgumentSets()
    {
        // If the arguments weren't declared in the NOTEs you can still get overloads if you know the EXACT type of the argument (implicit conversion not allowed)
        using FooIntFloatOverload = RareTs::OverloadInfo<MyObj, RareTs::IndexOf<MyObj>::foo, int, float>;
        using FooStringViewOverload = RareTs::OverloadInfo<MyObj, RareTs::IndexOf<MyObj>::foo, std::string_view>;
    
        std::cout << RareTs::toStr<typename FooIntFloatOverload::pointer_type<>>() << std::endl;
        std::cout << RareTs::toStr<typename FooStringViewOverload::pointer_type<>>() << std::endl;

        MyObj myObj {};
        (myObj.*FooIntFloatOverload::pointer<>)(0, 1.1f);
        (myObj.*FooStringViewOverload::pointer<>)("asdf");
    }
}
