#include <rarecpp/reflect.h>
#include <iostream>
#include <string_view>

inline namespace overload_access_by_constexpr_index
{
    struct MyObj
    {
        NOTE(foo, RareTs::Overload<int, float>, RareTs::Overload<std::string_view>)
        int foo(int i, float f) {
            std::cout << "  called foo(int, float) with: " << i << ", " << f << std::endl;
            return i + (int)f;
        }

        std::string_view foo(std::string_view s) {
            std::cout << "  called foo(std::string_view) with: " << s << std::endl;
            s.remove_prefix(1);
            return s;
        }

        REFLECT(MyObj, foo)
    };

    void overloadAccessByConstexprIndex()
    {
        using ZerothFooOverload = RareTs::MemberType<MyObj>::foo::Overloads::Overload<0>;
        using FirstFooOverload = RareTs::MemberType<MyObj>::foo::Overloads::Overload<1>;
    
        using ZerothArgSet = ZerothFooOverload::Arguments;
        using FirstArgSet = FirstFooOverload::Arguments;
        using ZerothReturnType = ZerothFooOverload::Return;
        using FirstReturnType = FirstFooOverload::Return;
    
        std::cout << RareTs::toStr<ZerothArgSet>() << std::endl;
        std::cout << "  returns " << RareTs::toStr<ZerothReturnType>() << std::endl;
        std::cout << RareTs::toStr<FirstArgSet>() << std::endl;
        std::cout << "  returns " << RareTs::toStr<FirstReturnType>() << std::endl;
    }
}
