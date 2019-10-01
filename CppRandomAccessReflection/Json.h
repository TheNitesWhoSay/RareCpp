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

                os << Indent(indent, indentLevel) << "\"" << field.name << "\": ";
                if ( field.IfNull(value, [&](auto) { os << "null"; }) );
                else if ( !field.isIterable )
                {
                    field.ForPrimitive(value, [&](auto primitive) { os << "\"" << primitive << "\""; }); // Primitive
                    field.ForObject(value, [&](auto object) { os << Output<std::remove_reference<decltype(object)>::type>(object, indent, indentLevel); }); // Object
                }
                else if ( !field.containsPairs && !field.isReflected ) // Primitive Array
                {
                    os << "[ ";
                    field.ForPrimitives(value, [&](auto index, auto element) { os << (index > 0 ? ", \"" : "\"") << element << "\""; }); // Primitive Array
                    field.ForPrimitivePointers(value, [&](auto index, auto element) { // Primitive Pointer Array
                        if ( index > 0 ) { os << ", "; }
                        if ( element == nullptr ) { os << "null"; }
                        else { os << "\"" << *element << "\""; }
                    });
                    os << " ]";
                }
                else if ( !field.containsPairs && field.isReflected ) // Object Array
                {
                    os << "[" << std::endl << Indent(indent, indentLevel + 1);
                    field.ForObjects(value, [&](auto index, auto element) { // Object Array
                        os << (index > 0 ? ", " : "") << Output<std::remove_reference<decltype(element)>::type>(element, indent, indentLevel + 1);
                    });
                    field.ForObjectPointers(value, [&](auto index, auto element) { // Object Pointer Array
                        if ( index > 0 ) { os << ", "; }
                        if ( element == nullptr ) { os << "null"; }
                        else { os << Output<std::remove_reference<decltype(*element)>::type>(*element, indent, indentLevel + 1); }
                    });
                    os << std::endl << Indent(indent, indentLevel) << "]";
                }
                else // if ( field.containsPairs) // Map
                {
                    os << "{";
                    field.ForPrimitivePairs(value, [&](auto index, auto first, auto second) { // Primitive Map
                        os << (index > 0 ? ", " : "") << std::endl << Indent(indent, indentLevel + 1) << "\"" << first << "\": \"" << second << "\"";
                    });
                    field.ForPrimitivePointerPairs(value, [&](auto index, auto first, auto second) { // Primitive Pointer Map
                        os << (index > 0 ? ", " : "") << std::endl << Indent(indent, indentLevel + 1) << "\"" << first << "\": ";
                        if ( second == nullptr ) { os << "null"; }
                        else { os << "\"" << *second << "\""; }
                    });
                    field.ForObjectPairs(value, [&](auto index, auto first, auto second) { // Object Map
                        os << (index > 0 ? ", " : "") << std::endl << Indent(indent, indentLevel + 1) << "\"" << first << "\": "
                            << Output<std::remove_reference<decltype(second)>::type>(second, indent, indentLevel + 1);
                    });
                    field.ForObjectPointerPairs(value, [&](auto index, auto first, auto second) { // Object Pointer Map
                        os << (index > 0 ? ", " : "") << std::endl << Indent(indent, indentLevel + 1) << "\"" << first << "\": ";
                        if ( second == nullptr ) { os << "null"; }
                        else { os << Output<std::remove_reference<decltype(*second)>::type>(*second, indent, indentLevel + 1); }
                    });
                    os << std::endl << Indent(indent, indentLevel) << "}";
				}
                os << (field.index < Class::totalFields-1 ? "," : "") << std::endl;
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