#include <rarecpp/reflect.h>
#include <iostream>
#include <string_view>

inline namespace overload_pack_expand
{
    struct MyObj
    {
        NOTE(foo, RareTs::Overload<int, float>, RareTs::Overload<std::string_view>)
        void foo(int i, float f) {
            std::cout << "  called foo(int, float) with: " << i << ", " << f << std::endl;
        }

        void foo(std::string_view s) {
            std::cout << "  called foo(std::string_view) with: " << s << std::endl;
        }

        REFLECT(MyObj, foo)
    };

    template <typename T>
    void useOverload(T & overload)
    {
        std::cout << RareTs::toStr<typename RareTs::remove_cvref_t<decltype(overload)>::Arguments>() << std::endl;
    }

    void overloadPackExpand()
    {
        RareTs::MemberType<MyObj>::foo::Overloads::pack(MyObj{}, [&](auto ... overloadInfo) {
            (useOverload(overloadInfo), ...);
        });
    }
}
