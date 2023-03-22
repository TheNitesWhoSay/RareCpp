#include <rarecpp/reflect.h>
#include <iostream>
#include <string_view>

inline namespace overload_access_by_runtime_index
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

    void overloadAccessByRuntimeIndex()
    {
        std::size_t i = 0;
        std::cout << "Enter the index of an overload: ";
        std::cin >> i;

        MyObj myObj{};
        RareTs::MemberType<MyObj>::foo::Overloads::at(myObj, i, [&](auto overloadInfo) {
            using OverloadInfo = decltype(overloadInfo);
            using ArgumentTypes = typename OverloadInfo::Arguments;
        
            if constexpr ( std::is_same_v<std::tuple<int, float>, ArgumentTypes> )
                (myObj.*(overloadInfo.pointer))(0, 1.1f);
            else
                (myObj.*(overloadInfo.pointer))("qwerty");
        });
    }
}
