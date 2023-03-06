#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>

inline namespace note_class_members
{
    struct RegularMembers
    {
        int a = 1;
        int b = 2;
        int c = 3;

        REFLECT(RegularMembers, a, b, c)
    };

    // You can use notes (aka annotations) to tag fields with additional compile-time info, such as renaming or ignoring a field in serializers
    // NOTE has the form NOTE(memberName, noteValues), where noteValues can include any constexpr-friendly value/object
    // By convention NOTE comes just before the thing it's NOTE'ing, but it's only required to come before the REFLECT macro in which the thing is reflected
    struct NotedMembers
    {
        NOTE(a, Json::Name{"int"}, Json::Stringify, 0, "asdf")
        int a = 1;

        int b = 2;

        NOTE(c, Json::Ignore)
        int c = 3;

        REFLECT(NotedMembers, a, b, c)
    };

    void noteClassMembers()
    {
        RegularMembers regularMembers{};
        NotedMembers notedMembers{};

        std::cout << "regularMembers: " << Json::pretty(regularMembers) << std::endl;
        std::cout << "notedMembers: " << Json::pretty(notedMembers) << std::endl;
    
        std::cout << "a has int note? : " << (RareTs::MemberType<NotedMembers>::a::hasNote<int>() ? "true" : "false") << std::endl;
        std::cout << "a has float note? : " << (RareTs::MemberType<NotedMembers>::a::hasNote<float>() ? "true" : "false") << std::endl;
        std::cout << "a has a const char* note with value: " << RareTs::MemberType<NotedMembers>::a::getNote<const char*>() << std::endl;
        std::cout << "all a note types: ";
        RareTs::MemberType<NotedMembers>::a::forEachNote([&](auto & note) {
            std::cout << RareTs::toStr<RareTs::remove_cvref_t<decltype(note)>>() << ", ";
        });
        std::cout << std::endl;
    }
}
