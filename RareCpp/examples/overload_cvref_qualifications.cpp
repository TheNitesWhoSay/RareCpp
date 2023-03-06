#include <rarecpp/reflect.h>
#include <iostream>
#include <string_view>

inline namespace overload_cvref_qualifications
{
    struct MyObj
    {
        NOTE(foo, RareTs::Overload<int, float>, RareTs::Overload<std::string_view>)
        void foo(int i, float f) {
            std::cout << "  called foo(int, float) <<NON CONST>> with: " << i << ", " << f << std::endl;
        }
        void foo(int i, float f) const {
            std::cout << "  called foo(int, float) <<CONST>> with: " << i << ", " << f << std::endl;
        }
        void foo(std::string_view s) {
            std::cout << "  called foo(std::string_view) with: " << s << std::endl;
        }

        REFLECT(MyObj, foo)
    };

    void overloadCvrefQualifications()
    {
        const MyObj myObj{};

        std::cout << "Least qualified: " << std::endl;
        RareTs::MemberType<MyObj>::foo::Overloads::forEach([&](auto overloadInfo) { // Here you haven't supplied anything indicating the cvref qualification
            using OverloadInfo = decltype(overloadInfo); // Consequently you're only get the "least" qualified version of overloads
            using Pointer = typename OverloadInfo::pointer_type;
            std::cout << "  " << RareTs::toStr<Pointer>() << std::endl;
        });
    
        std::cout << "const qualified: " << std::endl;
        RareTs::MemberType<MyObj>::foo::Overloads::forEach<const MyObj>([&](auto overloadInfo) { // Here you've specified a qualification using the template param
            using OverloadInfo = decltype(overloadInfo); // Consequently you're only get the const version of overloads
            using Pointer = typename OverloadInfo::pointer_type;
            std::cout << "  " << RareTs::toStr<Pointer>() << std::endl;

            (myObj.*overloadInfo.pointer)(0, 2.2f);
        });
    
        std::cout << "unqualified: " << std::endl;
        RareTs::MemberType<MyObj>::foo::Overloads::forEach<MyObj>([&](auto overloadInfo) { // Here you've specified a qualification using the template param
            using OverloadInfo = decltype(overloadInfo); // Consequently you're only get the given qualification (const) versions of overloads
            using Pointer = typename OverloadInfo::pointer_type;
            using ArgumentTypes = typename OverloadInfo::Arguments;
            std::cout << "  " << RareTs::toStr<Pointer>() << std::endl;
            std::cout << "  " << RareTs::toStr<ArgumentTypes>() << std::endl;
        });
    
        std::cout << "resolved on const instance: " << std::endl;
        RareTs::MemberType<MyObj>::foo::Overloads::forEach(myObj, [&](auto overloadInfo) { // Here you supply an instance of MyObj
            using OverloadInfo = decltype(overloadInfo); // Consequently you're getting the best-match overload for how your instance of MyObj was qualified
            using Pointer = typename OverloadInfo::pointer_type;
            std::cout << "  " << RareTs::toStr<Pointer>() << std::endl;

            (myObj.*overloadInfo.pointer)(0, 2.2f);
        });
    }
}
