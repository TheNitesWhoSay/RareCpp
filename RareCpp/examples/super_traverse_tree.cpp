#include <rarecpp/reflect.h>
#include <iostream>

inline namespace super_traverse_tree
{
    struct GrandparentA {};
    struct GrandparentB {};

    NOTE(ParentA, RareTs::Super<GrandparentA>)
    struct ParentA : GrandparentA { REFLECT_NOTED(ParentA) };

    NOTE(ParentB, RareTs::Super<GrandparentB>)
    struct ParentB : GrandparentB { REFLECT_NOTED(ParentB) };

    NOTE(Child, RareTs::Super<ParentA>, RareTs::Super<ParentB>)
    struct Child : ParentA, ParentB { REFLECT_NOTED(Child) };

    template <typename T, size_t Level = 0>
    void printTree()
    {
        if constexpr ( Level > 0 )
        {
            for ( size_t i=0; i<Level; ++i )
                std::cout << "  ";
        }

        std::cout << RareTs::toStr<T>() << std::endl;
        if constexpr ( RareTs::is_reflected_v<T> )
        {
            RareTs::Supers<T>::forEach([](auto superInfo) { // Note that this only loops over the (zero, one, or multiple) inherited classes immediately on type T
                printTree<typename decltype(superInfo)::type, Level+1>(); // Generally need to grab the type of a super and recurse on it to get to ancestors
            });
        }
    }

    void superTraverseTree()
    {
        printTree<Child>();
    }
}
