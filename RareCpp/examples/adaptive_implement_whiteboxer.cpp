#include <rarecpp/reflect.h>
#include <iostream>
#include <limits>
#include <string>

inline namespace adaptive_implement_whiteboxer
{
    inline namespace AdaptiveWhiteboxer
    {
        template <typename T, size_t I> struct type_wrapper { using type = typename RareTs::Class::template member_type<T, I>; }; // Work-around gcc access bugs

        template <typename T, size_t I = std::numeric_limits<size_t>::max()>
        struct reference_member : RareTs::Class::adapt_member<reference_member<T>::template type, T, I> {};

        template <typename T>
        struct reference_member<T, std::numeric_limits<size_t>::max()> {
            template <size_t I> using type = std::add_lvalue_reference_t<typename type_wrapper<T, I>::type>;
        };

        template <typename T, size_t ... Is>
        struct reference_members_adapter : RareTs::Class::adapt_member<reference_member<T>::template type, T, Is>... // Adapter extends the Ith members type ref
        {
            constexpr reference_members_adapter(T & t) // Initializes the reference values
                : RareTs::Class::adapt_member<reference_member<T>::template type, T, Is> {{ RareTs::Class::memberValue<Is>(t) }}... {}
        };

        template <typename T, size_t ... Is> constexpr auto whitebox(T & t, std::index_sequence<Is...>) {
            return reference_members_adapter<T, Is...>(t);
        }

        template <typename T> constexpr auto whitebox(T & t) {
            return RareTs::template Members<T>::template pack<RareTs::Filter::IsData>([&](auto & ... member) {
                return whitebox(t, std::index_sequence<RareTs::remove_cvref_t<decltype(member)>::index...>{});
            });
        }
    }

    class MyObj
    {
        int a = 1;
        int b = 2;
        int c = 3;

    public:
        REFLECT(MyObj, a, b, c)
    
        int checksum() { return a+b+c; }
    };

    void adaptiveImplementWhiteboxer()
    {
        MyObj myObj{};
        auto ref = whitebox(myObj);
        std::cout << myObj.checksum() << std::endl;
        std::cout << "a: " << ref.a << std::endl;
        std::cout << "b: " << ref.b << std::endl;
        std::cout << "c: " << ref.c << std::endl;
        ref.a++;
        ref.b++;
        ref.c++;
        std::cout << myObj.checksum() << std::endl;
    }
}
