#include <rarecpp/reflect.h>
#include <iostream>
#include <limits>
#include <string>

inline namespace adaptive_clone_structure
{
    inline namespace AdaptiveCloning
    {
        template <typename T, size_t I = std::numeric_limits<size_t>::max()>
        struct partition_member : RareTs::Class::adapt_member<partition_member<T>::template type, T, I> {};

        template <typename T>
        struct partition_member<T, std::numeric_limits<size_t>::max()> {
            template <size_t I> using type = RareTs::Class::member_type<T, I>;
        };

        template <typename T, size_t ... Is>
        struct partition_members : RareTs::Class::adapt_member<partition_member<T>::template type, T, Is> ... {};

        template <typename T, size_t ... Is>
        constexpr partition_members<T, Is...> cloneMembers(std::index_sequence<Is...>);

        template <typename T> using clone_members = decltype(cloneMembers<T>(std::make_index_sequence<RareTs::Class::member_count<T>>()));
    }

    struct MyObj
    {
        int a;
        std::string b;
        float c;

        REFLECT(MyObj, a, b, c)
    };

    void adaptiveCloneStructure()
    {
        using OpaqueObj = clone_members<MyObj>;
        OpaqueObj opaqueObj{};
    
        static_assert(!std::is_same_v<MyObj, OpaqueObj>, "These are different types, but they have the same/cloned members");
        opaqueObj.a = 1;
        opaqueObj.b = 2;
        opaqueObj.c = 3;
        std::cout << "Made opaque clone {a, b, c}" << std::endl;

        using PartitionObj = decltype(cloneMembers<MyObj>(std::index_sequence<0, 2>{}));
        PartitionObj partitionObj{};
        partitionObj.a = 1;
        //partitionObj.b = 2; // the field "b" at index 1 was not cloned, only fields 0 and 2 were cloned
        partitionObj.c = 3;
        std::cout << "Made partition {a:" << partitionObj.a << ", c:" << partitionObj.c << "}" << std::endl;
    }
}
