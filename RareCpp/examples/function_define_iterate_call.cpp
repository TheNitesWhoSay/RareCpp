#include <rarecpp/reflect.h>
#include <iostream>
#include <tuple>

inline namespace function_define_iterate_call
{
    struct MyController
    {
        void foo() {
            std::cout << "foo" << std::endl;
        }

        int bar(int value) {
            std::cout << "bar(" << value << ") = ";
            return value+1;
        }

        static void staticFoo() {
            std::cout << "staticFoo" << std::endl;
        }

        REFLECT(MyController, foo, bar, staticFoo)
    };

    void functionDefineIterateCall()
    {
        MyController myObj {};
        RareTs::Members<MyController>::forEach(myObj, [&](auto member, auto & value) {
            using PointerType = typename decltype(member)::pointer_type;
            constexpr size_t totalArgs = RareTs::argument_count_v<PointerType>;
            using ReturnType = RareTs::return_type_t<PointerType>; // The return type of the function
            using MemberOf = RareTs::member_of_t<PointerType>; // If this is a non-static member function, this gets the type it's a non-static member function in

            std::cout << "[" << member.index << "] " << RareTs::toStr<PointerType>() << std::endl; // Print function pointer (~the unnamed function signature)

            // Print ~the same signature except using the granular types we got from reflection/type introspection
            std::cout << "[" << member.index << "] " << RareTs::toStr<ReturnType>() << " ";
            if constexpr ( !std::is_void_v<MemberOf> )
                std::cout << RareTs::toStr<MemberOf>() << "::";
            std::cout << member.name << "(";
            RareTs::forIndexes<totalArgs>([&](auto i) {
                constexpr size_t tupleIndex = decltype(i)::value;
                using ArgumentType = RareTs::argument_t<tupleIndex, PointerType>;
                std::cout << RareTs::toStr<ArgumentType>();
            });
            std::cout << ")" << std::endl;

            using ArgumentTypes = RareTs::arguments_t<PointerType>; // Get arguments as tuple
        
            // Call the function, it's a little different for member functions vs static functions, and functions may differ by their arguments
            std::cout << "  ";
            if constexpr ( std::is_same_v<std::tuple<int>, ArgumentTypes> ) // is a function taking an int argument
                std::cout << (myObj.*value)(1) << std::endl;
            else if constexpr ( std::is_same_v<std::tuple<>, ArgumentTypes> && !member.isStatic ) // is a no-argument function
                (myObj.*value)();
            else if constexpr ( member.isStatic )
                value();
        });
    }
}
