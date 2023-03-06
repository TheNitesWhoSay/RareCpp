#include <rarecpp/reflect.h>
#include <iostream>
#include <tuple>

inline namespace misc_make_tuple
{
    struct MyObj
    {
        int a;
        int b;
        std::string c;

        REFLECT(MyObj, a, b, c)
    };

    template <typename T>
    auto toTuple(T & t)
    {
        return RareTs::Values<MyObj>::pack(t, [&](auto & ... value) {
            return std::tie(value...);
        });
    }

    void miscMakeTuple()
    {
        MyObj myObj {1, 2, "3"}; // I don't generally recommend forming tuples out of structures, generally it's unnecessary indirection/unhelpful
        auto tup = toTuple(myObj); // But if you get something out of doing it, go for it
    
        std::cout << RareTs::toStr<decltype(tup)>() << std::endl;
        std::cout << std::get<0>(tup) << std::endl;
        std::cout << std::get<1>(tup) << std::endl;
        std::cout << std::get<2>(tup) << std::endl;
    }
}
