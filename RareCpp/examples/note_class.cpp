#include <rarecpp/reflect.h>
#include <iostream>

inline namespace note_class
{
    struct Super {};

    struct RegularClass : Super
    {
        REFLECT(RegularClass)
    };

    NOTE(NotedClass, RareTs::Super<Super>) // A class-level annotation can attach info to the class type, e.g. declaring supers
    struct NotedClass : Super
    {
        REFLECT_NOTED(NotedClass) // Note, you need a different macro for *CLASS* level annotations to work, member-level annotations work with either macro
    };
    // Class-level annotations have essentially the same capabilities/limits/interface as member-level annotations

    void noteClass()
    {
        std::cout << "RegularClass has " << RareTs::Supers<RegularClass>::total << " known super-classes" << std::endl;
        std::cout << "NotedClass has " << RareTs::Supers<NotedClass>::total << " known super-classes" << std::endl;

        using RegularClassSuper = RareTs::Supers<RegularClass>::SuperType<0>;
        using NotedClassSuper = RareTs::Supers<NotedClass>::SuperType<0>;
    
        std::cout << "RegularClass super: " << RareTs::toStr<RegularClassSuper>() << std::endl;
        std::cout << "NotedClass super: " << RareTs::toStr<NotedClassSuper>() << std::endl;

        std::cout << "all NotedClass note types: ";
        RareTs::Notes<NotedClass>::forEachNote([&](auto & note) {
            using Note = RareTs::remove_cvref_t<decltype(note)>;
            std::cout << RareTs::toStr<Note>() << ", ";
        });
        std::cout << std::endl;
    }
}
