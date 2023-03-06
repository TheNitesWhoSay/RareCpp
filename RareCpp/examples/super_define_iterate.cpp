#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>

inline namespace super_define_iterate
{
    struct ParentA { int a=0; REFLECT(ParentA, a) };
    struct ParentB { int b=1; REFLECT(ParentB, b) };

    NOTE(ChildA, RareTs::Super<ParentA>) // Supers are defined in a class-level annotations like so
    struct ChildA : ParentA
    {
        REFLECT_NOTED(ChildA) // Class-level annotations require the "REFLECT_NOTED" rather than the "REFLECT" macro (not required for other note categories)
    };

    NOTE(ChildB,
        RareTs::Super<ParentA>(Json::Name{"parA"}), // Super-class relationships can in turn have their own notes set in the optional parens after a super-declaration
        RareTs::Super<ParentB>(Json::Ignore))
    struct ChildB : ParentA, ParentB
    {
        REFLECT_NOTED(ChildB) // Class-level annotations require the "REFLECT_NOTED" rather than the "REFLECT" macro (not required for other note categories)
    };

    void superDefineIterate()
    {
        std::cout << "ChildA: " << std::endl;
        RareTs::Supers<ChildA>::forEach([&](auto superInfo) {
            using SuperInfo = decltype(superInfo);
            constexpr size_t index = SuperInfo::index;
            using type = typename SuperInfo::type;

            std::cout << "[" << index << "] " << RareTs::toStr<type>() << std::endl;
        });
    
        ChildB childB {};
        std::cout << std::endl << "ChildB: " << std::endl;
        RareTs::Supers<ChildB>::forEach([&](auto superInfo) {
            using SuperInfo = decltype(superInfo);
            constexpr size_t index = SuperInfo::index;
            using type = typename SuperInfo::type;
            constexpr bool ignored = SuperInfo::template hasNote<Json::IgnoreType>();

            std::cout << "[" << index << "] " << RareTs::toStr<type>() << (ignored ? " (Json Ignored)" : "") << std::endl;
        });
        std::cout << Json::pretty(childB) << std::endl;

        RareTs::Supers<ChildB>::forEach(childB, [&](auto superInfo, auto & super) { // Second param is childB casted to the current iterations super-type
            using SuperInfo = decltype(superInfo);
            using type = typename SuperInfo::type;

            std::cout << "(" << RareTs::toStr<type>() << ")childB: " << Json::pretty(super) << std::endl;
        });
    }
}
