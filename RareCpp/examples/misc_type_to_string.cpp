#include <rarecpp/reflect.h>
#include <iostream>

inline namespace misc_type_to_string
{
    struct structure {};

    template <typename T> struct templated {};

    void miscTypeToString()
    {
        std::cout << "int: " << RareTs::toStr<int>() << std::endl;
        std::cout << "structure: " << RareTs::toStr<structure>() << std::endl;
        std::cout << "templated<float>: " << RareTs::toStr<templated<float>>() << std::endl;
    }
}
