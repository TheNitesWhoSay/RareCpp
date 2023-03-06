#include <rarecpp/reflect.h>
#include <iostream>
#include <tuple>

inline namespace misc_runtime_to_constexpr_index
{
    void miscRuntimeToConstexprIndex()
    {
        using TypeList = std::tuple<int, char, float>;
        constexpr size_t totalTypes = std::tuple_size_v<TypeList>;

        size_t index = 0;
        std::cout << "Enter a tuple index (between 0 and 2): ";
        std::cin >> index;

        // For some small maximum value (e.g. totalTypes; this can't be too large)
        // you can turn a runtime index ("index") into a std::integral_constant ("i")
        // from which you can take a constexpr index ("decltype(i)::value")
        // which can be used as a template parameter/tuple index and such
        RareTs::forIndex<totalTypes>(index, [&](auto i) {
            constexpr size_t constexprIndex = decltype(i)::value;
        
            using SelectedTupleElement = std::tuple_element_t<constexprIndex, TypeList>;
            std::cout << "Selected tuple element: " << RareTs::toStr<SelectedTupleElement>() << std::endl;
        });

        // You can also loop over a set of constexpr indexes for some given maximum in a similar fashion
        RareTs::forIndexes<totalTypes>([&](auto i) {
            constexpr size_t constexprIndex = decltype(i)::value;
        
            using SelectedTupleElement = std::tuple_element_t<constexprIndex, TypeList>;
            std::cout << constexprIndex << ":" << RareTs::toStr<SelectedTupleElement>() << "," << std::endl;
        });
    }
}
