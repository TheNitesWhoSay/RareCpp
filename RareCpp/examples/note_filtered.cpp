#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>

inline namespace note_filtered
{
    using Alias = std::string_view;

    struct NotedMembers
    {
        NOTE(a, 0, "asdf", 1, 2, Alias{"ayy"}, "qwerty")
        int a = 1;

        int b = 2;

        int c = 3;

        REFLECT(NotedMembers, a, b, c)
    };

    void noteFiltered()
    {
        // The note interface can use predicate-based filtering on the type of the values given in the NOTE
        std::cout << "first const char*: " << RareTs::MemberType<NotedMembers>::a::getNote<const char*>() << std::endl;
        std::cout << "first int: " << RareTs::MemberType<NotedMembers>::a::getNote<int>() << std::endl;
        std::cout << "first alias: " << RareTs::MemberType<NotedMembers>::a::getNote<Alias>() << std::endl;
    
        std::cout << "all const char*'s: ";
        RareTs::MemberType<NotedMembers>::a::forEachNote<const char*>([&](auto & note) {
            std::cout << note << ", ";
        });
        std::cout << std::endl;

        std::cout << "all ints: ";
        RareTs::MemberType<NotedMembers>::a::forEachNote<int>([&](auto & note) {
            std::cout << note << ", ";
        });
        std::cout << std::endl;

        std::cout << "all aliases: ";
        RareTs::MemberType<NotedMembers>::a::forEachNote<Alias>([&](auto & note) {
            std::cout << note << ", ";
        });
        std::cout << std::endl;
    }
}
