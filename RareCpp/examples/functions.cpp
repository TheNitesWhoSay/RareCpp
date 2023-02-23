#include <rarecpp/reflect.h>
#include <cstddef>
#include <iostream>
#include <string_view>
#include <utility>
using RareTs::Overload;

template <typename F>
std::ostream & operator<<(std::ostream & os, const RareTs::Function<F> & info) {
    using Function = RareTs::remove_cvref_t<decltype(info)>;
    os << "Function: " << RareTs::toStr<Function>() << std::endl
        << "\tMemberOf: " << (Function::IsMemberFunction ? RareTs::toStr<typename Function::MemberOf>() : "(none)") << std::endl
        << "\tReturnType: " << RareTs::toStr<typename Function::Return>() << std::endl;
    if constexpr ( Function::ArgumentCount > 0 )
    {
        os << "\tArguments:";
        RareTs::forIndexes<Function::ArgumentCount>([&](auto A) {
            constexpr size_t argIndex = decltype(A)::value;
            using ArgType = typename Function::template Argument<argIndex>;
            os << std::endl << "\t\t [" << argIndex << "]: " << RareTs::toStr<ArgType>();
        });
    }
    else
        os << "\tArguments: (none)";
        
    return os;
}

template <typename ClassType, size_t I, typename ... Args>
std::ostream & operator<<(std::ostream & os, const RareTs::OverloadInfo<ClassType, I, Args...> & overloadInfo) {
    using Ovl = RareTs::remove_cvref_t<decltype(overloadInfo)>;
    using argument_types = typename Ovl::argument_types;
    constexpr auto name = RareTs::Member<ClassType, I>::name;
    constexpr size_t numOverloads = Ovl::totalQualifications;
    constexpr size_t numArguments = std::tuple_size_v<argument_types>;

    if constexpr ( numOverloads > 0 )
    {
        std::cout << name << "(";
        RareTs::forIndexes<numArguments>([&](auto J) {
            using Arg = std::tuple_element_t<decltype(J)::value, argument_types>;
            if constexpr ( decltype(J)::value > 0 )
                std::cout << ", ";
            std::cout << RareTs::toStr<Arg>();
        });
        std::cout << ")" << std::endl;

        using overloads = decltype(Ovl::pointers);
        RareTs::forIndexes<numOverloads>([&](auto J) {
            using overload = std::tuple_element_t<decltype(J)::value, overloads>;
            std::cout << "\t" << RareTs::toStr<overload>() << std::endl;
        });
    }
    return os;
}

struct bar
{
    int foo(char) { return 1; }
    int foo(char) const noexcept { return 2; }
    int foo(char) volatile { return 3; }
    int foo(char) const volatile { return 4; }

    int foo(char, int) { return 5; }

    int refFoo(char) & { return 5; }
    int refFoo(char) const & { return 6; }
    int refFoo(char) volatile & { return 7; }
    bool refFoo(char) const volatile & { return false; }
    int refFoo(char) && { return 9; }
    int refFoo(char) const && { return 10; }
    int refFoo(char) volatile && { return 11; }
    int refFoo(char) const volatile && noexcept { return 12; }

    int refFoo(char, int) & { return 1337; }
    
    NOTE(staticFoo, Overload<char>, Overload<char, int>)
    static int staticFoo(char) { return 13; }
    static int staticFoo(char, int) { return 13; }

    int a;
    static int b;
    int & c;
    static int & d;
    int e() { return 1337; }
    static int f() { return 13; }

    REFLECT(bar, foo, refFoo, staticFoo, a, b, c, d, e, f)
};

void functionExperiment()
{
    RareTs::forIndexes<RareTs::Class::member_count<bar>>([&](auto I) {
        
        constexpr size_t MemberIndex = decltype(I)::value;

        std::cout << RareTs::Class::member_name<bar, MemberIndex>
            << " " << (RareTs::is_overloaded_v<bar, MemberIndex> ? "[ovl]" : "[non]")
            << " [" << RareTs::toStr<RareTs::Class::member_type<bar, MemberIndex>>() << "]"
            << " [" << RareTs::toStr<RareTs::Class::member_pointer_type<bar, MemberIndex>>() << "]"
            << std::endl;

        RareTs::Member<bar, MemberIndex>::Overloads::template forEach<bar>([&](auto overload) {
            std::cout << "    Known overload: " << overload << std::endl;
        });
        if constexpr ( RareTs::is_overloaded_v<bar, MemberIndex> )
        {
            std::cout << "    Unknown overload: " << RareTs::OverloadInfo<bar, MemberIndex, char>{} << std::endl;
            std::cout << "    Unknown overload: " << RareTs::OverloadInfo<bar, MemberIndex, char, int>{} << std::endl;
        }
        std::cout << std::endl;
    });
}
