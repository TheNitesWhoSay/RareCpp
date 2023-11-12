#include <rarecpp/reflect.h>
#include <iostream>
#include <tuple>

inline namespace misc_tuplify
{
    struct MyObj
    {
        int a;
        int b;
        std::string c;

        REFLECT(MyObj, a, b, c)
    };

    void miscTuplify()
    {
        MyObj myObj {1, 2, "3"}; // This can be turned into something that's tuple-like without the type-lossy nature of the std::tie approach
        auto tup = RareTs::tuplify(myObj); // "tup" wraps a reference to myObj, can be used with tuple-like interfaces & lets you use structured bindings

        std::cout << "tuple size: " << std::tuple_size_v<decltype(tup)> << std::endl;
        std::cout << RareTs::toStr<std::tuple_element_t<0, decltype(tup)>>() << ": " << std::get<0>(tup) << std::endl;
        std::cout << RareTs::toStr<std::tuple_element_t<1, decltype(tup)>>() << ": " << std::get<1>(tup) << std::endl;
        std::cout << RareTs::toStr<std::tuple_element_t<2, decltype(tup)>>() << ": " << std::get<2>(tup) << std::endl;

        auto & [a, b, c] = tup; // structured binding
        std::cout << a << ", " << b << ", " << c << std::endl;

        using Tup = RareTs::member_type_tuple_t<MyObj>; // Can also get a std::tuple type representative of the members if only metaprogramming is needed
        std::cout << RareTs::toStr<Tup>() << std::endl;
    }
}
