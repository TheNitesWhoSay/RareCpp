#include <rarecpp/reflect.h>
#include <iostream>

inline namespace member_reflect_outside_of_class
{
    struct UnownedObject
    {
        int x = 1;
        int y = 2;
    };

    class AnotherUnowned
    {
    protected:
        int a = 3;
        int b = 4;
    };
}

template <> struct RareTs::Proxy<UnownedObject> : UnownedObject // This must be done at global scope
{
    // Using the REFLECT macro in a proxy allows you to REFLECT information same as though you had placed it in the target classes definition
    // This is especially useful for objects you do not own, e.g. those from other libraries
    REFLECT(RareTs::Proxy<UnownedObject>, x, y)
};

template <> struct RareTs::Proxy<AnotherUnowned> : AnotherUnowned // You can reflect protected members since proxies inherit from the target
{
    NOTE(a, "You can also place NOTE on members while proxying")

    REFLECT(RareTs::Proxy<AnotherUnowned>, a, b)
};

inline namespace member_reflect_outside_of_class
{
    void memberReflectOutsideOfClass()
    {
        UnownedObject point {};
        std::cout << "UnownedObject: " << std::endl;
        RareTs::Members<UnownedObject>::forEach(point, [&](auto member, auto & value) {
            std::cout << "  " << member.name << " = " << value << std::endl;
        });
        
        AnotherUnowned another {};
        std::cout << "AnotherUnowned: " << std::endl;
        RareTs::Members<AnotherUnowned>::forEach(another, [&](auto member, auto & value) {
            std::cout << "  " << member.name << " = " << value;
            if constexpr ( member.template hasNote<const char*>() )
                std::cout << " // " << member.template getNote<const char*>();
            std::cout << std::endl;
        });
    }
}