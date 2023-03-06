#include <rarecpp/reflect.h>
#include <iostream>

inline namespace super_access_by_constexpr_index
{
    struct ParentA { int a=0; REFLECT(ParentA, a) };
    struct ParentB { int b=1; REFLECT(ParentB, b) };

    NOTE(Child,
        RareTs::Super<ParentA>,
        RareTs::Super<ParentB>)
    struct Child : ParentA, ParentB
    {
        REFLECT_NOTED(Child)
    };

    void superAccessByConstexprIndex()
    {
        using zeroth = RareTs::Supers<Child>::SuperInfo<0>::type;
        using first = RareTs::Supers<Child>::SuperInfo<1>::type;
        std::cout << "[0] " << RareTs::toStr<zeroth>() << std::endl;
        std::cout << "[1] " << RareTs::toStr<first>() << std::endl;
    }
}
