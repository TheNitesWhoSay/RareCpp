#include <rarecpp/reflect.h>
#include <iostream>

inline namespace super_access_by_runtime_index
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

    void superAccessByRuntimeIndex()
    {
        size_t superIndex = 0;
        std::cout << "Enter a super index: ";
        std::cin >> superIndex;
        RareTs::Supers<Child>::at(superIndex, [&](auto superInfo) {
            using SuperInfo = decltype(superInfo);
            std::cout << "[" << SuperInfo::index << "] " << RareTs::toStr<typename SuperInfo::type>() << std::endl;
        });
    }
}
