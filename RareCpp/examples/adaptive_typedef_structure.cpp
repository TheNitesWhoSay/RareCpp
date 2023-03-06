#include <rarecpp/reflect.h>
#include <iostream>
#include <limits>
#include <string>

inline namespace adaptive_typedef_structure
{
    inline namespace AdaptiveTypedefs
    {
        template <typename T, size_t I = std::numeric_limits<size_t>::max()>
        struct partition_member_type : RareTs::Class::adapt_member_type<partition_member_type<T>::template type, T, I> {};

        template <typename T>
        struct partition_member_type<T, std::numeric_limits<size_t>::max()> {
            template <size_t I> using type = RareTs::Class::member_type<T, I>;
        };

        template <typename T, size_t ... Is>
        struct partition_member_types : RareTs::Class::adapt_member_type<partition_member_type<T>::template type, T, Is> ... {};

        template <typename T, size_t ... Is>
        constexpr partition_member_types<T, Is...> memberTypedefs(std::index_sequence<Is...>);

        template <typename T> using member_typedefs = decltype(memberTypedefs<T>(std::make_index_sequence<RareTs::Class::member_count<T>>()));
    }

    struct MyObj
    {
        int a;
        std::string b;
        float c;

        REFLECT(MyObj, a, b, c)
    };

    void adaptiveTypedefStructure()
    {
        using MemberTypedefs = member_typedefs<MyObj>;
        std::cout << "a: " << RareTs::toStr<MemberTypedefs::a>() << std::endl;
        std::cout << "b: " << RareTs::toStr<MemberTypedefs::b>() << std::endl;
        std::cout << "c: " << RareTs::toStr<MemberTypedefs::c>() << std::endl << std::endl;

        using PartitionTypedefs = decltype(memberTypedefs<MyObj>(std::index_sequence<1, 2>{}));
        //std::cout << RareTs::toStr<PartitionTypedefs::a>() << std::endl; // Member index 0 was not supplied for this partition, so field 'a' is not present
        std::cout << "b: " << RareTs::toStr<PartitionTypedefs::b>() << std::endl;
        std::cout << "c: " << RareTs::toStr<PartitionTypedefs::c>() << std::endl;
    }
}
