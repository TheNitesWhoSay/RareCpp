#include <rarecpp/reflect.h>
#include <iostream>
#include <tuple>

inline namespace misc_direct_tuplification
{
    struct MyObj
    {
        int a;
        int b;
        std::string c;

        REFLECT(MyObj, a, b, c)
    };
}

// Adding tuple protocol directly to your types involves adding some specializations/overloads to std, the in-object REFLECT macro simply can't do this
// Another macro in the global scope might be able to do this for you some of the time, but not covering all cases (namely templated classes)
// Consequently there's some manual work to enable direct tuplification, though RareCpp does save you from needing per-member specializations/overloads
namespace std
{
    template <> struct tuple_size<MyObj> { static constexpr size_t value = RareTs::Members<MyObj>::total; }; // Need a std::tuple_size specialization
    template <size_t I> struct tuple_element<I, MyObj> { using type = typename RareTs::Member<MyObj, I>::type; }; // Need a std::tuple_element specialization
    template <size_t I> constexpr auto & get(MyObj & obj) { return RareTs::Member<MyObj, I>::value(obj); }; // Need a std::get overload(s)
    template <size_t I> constexpr auto & get(const MyObj & obj) { return RareTs::Member<MyObj, I>::value(obj); }; // also sometimes MyObj && and const MyObj &&
}

inline namespace misc_direct_tuplification
{
    void miscDirectTuplification()
    {
        MyObj myObj {1, 2, "3"};

        std::cout << "tuple size: " << std::tuple_size_v<MyObj> << std::endl;
        std::cout << RareTs::toStr<std::tuple_element_t<0, MyObj>>() << ": " << std::get<0>(myObj) << std::endl;
        std::cout << RareTs::toStr<std::tuple_element_t<1, MyObj>>() << ": " << std::get<1>(myObj) << std::endl;
        std::cout << RareTs::toStr<std::tuple_element_t<2, MyObj>>() << ": " << std::get<2>(myObj) << std::endl;

        auto & [a, b, c] = myObj; // structured binding
        std::cout << a << ", " << b << ", " << c << std::endl;
    }
}
