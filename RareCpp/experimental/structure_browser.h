#ifndef STRUCTURE_BROWSER
#define STRUCTURE_BROWSER
#include <rarecpp/reflect.h>
#include <iostream>

// Experimental code to browse a structure from an auto-generated console menu
namespace RareStructureBrowser
{
    using Header = std::string_view;

    template <typename T>
    constexpr size_t lastMenuItem()
    {
        return RareTs::Members<T>::total;
    }

    template <typename T, bool TopLevel = true>
    inline size_t & selectFromMenu(size_t & selection, std::istream & is = std::cin, std::ostream & os = std::cout)
    {
        constexpr bool isMainMenu = RareTs::Notes<T>::template hasNote<MainMenuType>();
        constexpr size_t total = RareTs::Members<T>::total;
        if constexpr ( RareTs::Notes<T>::template hasNote<RareMenu::Header>() )
            os << std::endl << RareTs::Notes<T>::template getNote<RareMenu::Header>() << std::endl;
        else
            os << std::endl << "Make a selection: " << std::endl;

        RareTs::Members<T>::forEach([&](auto member) {
            using Member = decltype(member);
            if constexpr ( Member::template hasNote<const char*>() )
                os << "  [" << Member::index << "]: " << Member::template getNote<const char*>() << std::endl;
            else
                os << "  [" << Member::index << "]: " << Member::name << std::endl;
        });
        os << "  [" << total << (isMainMenu ? "]: Exit" : "]: Back") << std::endl;
        do {
            os << ": ";
            is >> selection;
        }
        while ( selection > total );
        return selection;
    }

    template <typename T, bool TopLevel = true>
    inline void openMenu(std::istream & is = std::cin, std::ostream & os = std::cout)
    {
        size_t selection = 0;
        do {
            RareTs::Members<T>::at(selectFromMenu<T, TopLevel>(selection, is, os), [&](auto member) {
                using Member = decltype(member);
                using MemberType = typename Member::type;
                if constexpr ( member.isStatic && member.isFunction )
                    member.value()();
                else if constexpr ( member.isData )
                {
                    if constexpr ( RareTs::is_reflected_v<MemberType> )
                        openMenu<MemberType, false>(is, os);
                    else if constexpr ( std::is_invocable<std::remove_pointer_t<MemberType>>::value ) // No-arg invokable function pointer
                        (*member.value())();
                }
            });
        }
        while ( selection != RareTs::Members<T>::total );
    }
}

#endif