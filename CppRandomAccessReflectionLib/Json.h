#ifndef JSON_H
#define JSON_H
#include <string>
#include <ostream>
#include "Reflect.h"
using namespace Reflect;

namespace Json {
    
    class Indent
    {
    public:
        Indent(std::string indent, size_t indentLevel) : indent(indent), indentLevel(indentLevel) {}
        std::string indent;
        size_t indentLevel;
    };

    std::ostream & operator<<(std::ostream & os, Indent indent)
    {
        for ( size_t i=0; i<indent.indentLevel; i++ )
            os << indent.indent;

        return os;
    }

    template <typename T>
    class Output
    {
    public:
        Output(T & obj, const std::string &indent = "  ", size_t indentLevel = 0) : obj(obj), indent(indent), indentLevel(indentLevel) {}

        T & obj;
        const std::string indent;
        size_t indentLevel;

        void put(std::ostream & os)
        {
            os << "{" << std::endl;
            indentLevel++;

            using Class = typename T::Class;
            Class::ForEachField(obj, [&](auto field, auto value) {
                const auto & constField = field;

                os << Indent(indent, indentLevel) << "\"" << constField.name << "\": ";
                if ( constField.IfNull(value) )
                    os << "null";
                else if ( !constField.isIterable )
                {
                    constField.ForPrimitive(value, [&](auto primitive) { os << "\"" << primitive << "\""; }); // Primitive
                    constField.ForObject(value, [&](auto object) { os << Output<std::remove_reference<decltype(object)>::type>(object, indent, indentLevel); }); // Object
                }
                else if ( !constField.containsPairs && !constField.isReflected ) // Primitive Array
                {
                    os << "[ ";
                    constField.ForPrimitives(value, [&](auto index, auto element) { os << (index > 0 ? ", \"" : "\"") << element << "\""; }); // Primitive Array
                    constField.ForPrimitivePointers(value, [&](auto index, auto element) { // Primitive Pointer Array
                        if ( index > 0 ) { os << ", "; }
                        if ( element == nullptr ) { os << "null"; }
                        else { os << "\"" << *element << "\""; }
                    });
                    os << " ]";
                }
                else if ( !constField.containsPairs && constField.isReflected ) // Object Array
                {
                    os << "[" << std::endl << Indent(indent, indentLevel + 1);
                    constField.ForObjects(value, [&](auto index, auto element) { // Object Array
                        os << (index > 0 ? ", " : "") << Output<std::remove_reference<decltype(element)>::type>(element, indent, indentLevel + 1);
                    });
                    constField.ForObjectPointers(value, [&](auto index, auto element) { // Object Pointer Array
                        if ( index > 0 ) { os << ", "; }
                        if ( element == nullptr ) { os << "null"; }
                        else { os << Output<std::remove_reference<decltype(*element)>::type>(*element, indent, indentLevel + 1); }
                    });
                    os << std::endl << Indent(indent, indentLevel) << "]";
                }
                else // if ( constField.containsPairs) // Map
                {
                    os << "{";
                    if ( !constField.isReflected )
                    {
                        constField.ForPrimitivePairs(value, [&](auto index, auto first, auto second) { // Primitive Map
                            os << (index > 0 ? ", " : "") << std::endl << Indent(indent, indentLevel + 1) << "\"" << first << "\": \"" << second << "\"";
                        });
                        constField.ForPrimitivePointerPairs(value, [&](auto index, auto first, auto second) { // Primitive Pointer Map
                            os << (index > 0 ? ", " : "") << std::endl << Indent(indent, indentLevel + 1) << "\"" << first << "\": ";
                            if ( second == nullptr ) { os << "null"; }
                            else { os << "\"" << *second << "\""; }
                        });
                    }
                    else
                    {
                        constField.ForObjectPairs(value, [&](auto index, auto first, auto second) { // Object Map
                            os << (index > 0 ? ", " : "") << std::endl << Indent(indent, indentLevel + 1) << "\"" << first << "\": "
                                << Output<std::remove_reference<decltype(second)>::type>(second, indent, indentLevel + 1);
                        });
                        constField.ForObjectPointerPairs(value, [&](auto index, auto first, auto second) { // Object Pointer Map
                            os << (index > 0 ? ", " : "") << std::endl << Indent(indent, indentLevel + 1) << "\"" << first << "\": ";
                            if ( second == nullptr ) { os << "null"; }
                            else { os << Output<std::remove_reference<decltype(*second)>::type>(*second, indent, indentLevel + 1); }
                        });
                    }
                    os << std::endl << Indent(indent, indentLevel) << "}";
				}
                os << (constField.index < Class::totalFields-1 ? "," : "") << std::endl;
            });

            os << Indent(indent, --indentLevel) << "}";
        }
    };

    template <typename T>
    std::ostream & operator<<(std::ostream & os, Json::Output<T> object)
    {
        object.put(os);
        return os;
    }

    template <typename T>
    static Output<T> out(T & t, const std::string & indent = "  ", size_t indentLevel = 0)
    {
        return Output<T>(t, indent, indentLevel);
    }
};

#endif