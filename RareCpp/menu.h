#ifndef MAIN_MENU_H
#define MAIN_MENU_H
#include <cstddef>
#include <functional>
#include <limits>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <iostream>

class Menu
{
    struct LabeledFunction
    {
        std::string_view label;
        std::function<void()> function;
    };
    
    std::string label = "";
    std::string prompt = "";
    std::vector<std::variant<Menu, LabeledFunction>> items{};

    inline void indent(size_t indentLevel, std::ostream & os)
    {
        for ( size_t i=0; i<indentLevel; ++i )
            os << "  ";
    }

    template <bool TopLevel>
    inline size_t select(size_t indentLevel, std::istream & is, std::ostream & os)
    {
        size_t total = items.size();
        os << std::endl;
        indent(indentLevel, os);
        os << prompt << std::endl;
        
        indent(indentLevel+1, os);
        os << "[0" << (TopLevel ? "]: Exit" : "]: Back") << std::endl;
        for ( size_t i=0; i<items.size(); ++i ) {
            std::visit([&](auto & item) {
                indent(indentLevel+1, os);
                os << "[" << i+1 << "]: " << item.label << std::endl;
            }, items[i]);
        }
        
        size_t selection = 0;
        do {
            os << ": ";
            try {
                is >> selection;
            } catch ( ... ) {
                is.clear();
                is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                selection = total+1;
            }
            if ( !is.good() )
            {
                is.clear();
                is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                selection = total+1;
            }
        }
        while ( selection > total );
        return selection;
    }
    
public:
    Menu(std::string_view label, std::string_view prompt) : label(label), prompt(prompt) {}
    Menu(std::string_view prompt) : label(""), prompt(prompt) {}

    auto & item(std::string_view itemLabel, std::function<void()> function) {
        items.push_back(LabeledFunction{itemLabel, function});
        return *this;
    }

    auto & item(Menu menu) {
        items.push_back(menu);
        return *this;
    }

    template <bool TopLevel = true>
    inline void open(size_t indentLevel = 0, std::istream & is = std::cin, std::ostream & os = std::cout)
    {
        size_t selection = 0;
        do {
            selection = select<TopLevel>(indentLevel, is, os);
            if ( selection > 0 && selection <= items.size() )
            {
                std::visit([&](auto & item) {
                    if constexpr ( std::is_same_v<LabeledFunction, std::remove_reference_t<decltype(item)>> )
                    {
                        try {
                            item.function();
                        } catch ( ... ) {
                            is.clear();
                            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            os << "Unhandled exception in example!" << std::endl;
                        }
                        if ( !is.good() )
                        {
                            is.clear();
                            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                    }
                    else
                        item.template open<false>(indentLevel+1, is, os);
                }, items[selection-1]);
            }
        }
        while ( selection != 0 );
    }
};

#endif