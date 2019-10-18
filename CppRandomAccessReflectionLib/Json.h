#ifndef JSON_H
#define JSON_H
#include <ostream>
#include "Reflect.h"
using namespace Reflect;

namespace Json {
    
    constexpr const char twoSpaces[] = "  ";

    template <size_t indentLevel, const char* indent = twoSpaces>
    struct Indent { };

    template <size_t indentLevel, const char* indent = twoSpaces>
    std::ostream & operator<<(std::ostream & os, const Indent<indentLevel, indent>)
    {
        #pragma warning(suppress: 6294) // I'll-defined loop warning when indentLevel is 0
        for ( size_t i=0; i<indentLevel; i++ )
            os << indent;

        return os;
    }

    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    class Output
    {
    public:
        Output(const T & obj) : obj(obj) {}

        const T & obj;

        static constexpr std::ostream & put(std::ostream & os, const T & obj)
        {
            os << "{" << std::endl;
            
            using Index = const size_t &;
            using Class = typename T::Class;
            Class::ForEachField(obj, [&](auto & field, auto & value) {
                
                using SubType = typename std::remove_reference<decltype(field)>::type::sub_type;

                os << Indent<indentLevel+1, indent>() << "\"" << field.name << "\": ";
                if ( field.IfNull(value) )
                    os << "null";
                else if ( !field.isIterable )
                {
                    field.ForPrimitive(value, [&](auto & primitive) { os << "\"" << primitive << "\""; }); // Primitive
                    field.ForObject(value, [&](auto & object) { Output<SubType, indentLevel+1>::put(os, object); }); // Object
                }
                else if ( !field.containsPairs && !field.isReflected ) // Primitive Array
                {
                    os << "[ ";
                    field.ForPrimitivesConst(value, [&](Index index, auto & element) { // Primitive Array
                        os << (index > 0 ? ", \"" : "\"") << element << "\"";
                    });
                    field.ForPrimitivePointersConst(value, [&](Index index, auto & element) { // Primitive Pointer Array
                        if ( index > 0 ) { os << ", "; }
                        if ( element == nullptr ) { os << "null"; }
                        else { os << "\"" << *element << "\""; }
                    });
                    os << " ]";
                }
                else if ( !field.containsPairs && field.isReflected ) // Object Array
                {
                    os << "[" << std::endl << Indent<indentLevel+2, indent>();
                    field.ForObjectsConst(value, [&](Index index, auto & element) { // Object Array
                        os << (index > 0 ? ", " : "");
                        Output<SubType, indentLevel+2>::put(os, element);
                    });
                    field.ForObjectPointersConst(value, [&](Index index, auto & element) { // Object Pointer Array
                        if ( index > 0 ) { os << ", "; }
                        if ( element == nullptr ) { os << "null"; }
                        else { Output<SubType, indentLevel+2>::put(os, *element); }
                    });
                    os << std::endl << Indent<indentLevel+1, indent>() << "]";
                }
                else // if ( field.containsPairs) // Map
                {
                    os << "{";
                    if ( !field.isReflected )
                    {
                        field.ForPrimitivePairs(value, [&](Index index, auto & first, auto & second) { // Primitive Map
                            os << (index > 0 ? ", " : "") << std::endl
                                << Indent<indentLevel+2, indent>() << "\"" << first << "\": \"" << second << "\"";
                        });
                        field.ForPrimitivePointerPairs(value, [&](Index index, auto & first, auto & second) { // Primitive Pointer Map
                            os << (index > 0 ? ", " : "") << std::endl << Indent<indentLevel+2, indent>() << "\"" << first << "\": ";
                            if ( second == nullptr ) { os << "null"; }
                            else { os << "\"" << *second << "\""; }
                        });
                    }
                    else
                    {
                        field.ForObjectPairs(value, [&](Index index, auto & first, auto & second) { // Object Map
                            os << (index > 0 ? ", " : "") << std::endl << Indent<indentLevel+2, indent>() << "\"" << first << "\": ";
                            Output<SubType, indentLevel+2>::put(os, second);
                        });
                        field.ForObjectPointerPairs(value, [&](Index index, auto & first, auto & second) { // Object Pointer Map
                            os << (index > 0 ? ", " : "") << std::endl << Indent<indentLevel+2, indent>() << "\"" << first << "\": ";
                            if ( second == nullptr ) { os << "null"; }
                            else { Output<SubType, indentLevel+2>::put(os, *second); }
                        });
                    }
                    os << std::endl << Indent<indentLevel+1, indent>() << "}";
				}
                os << (field.index < Class::totalFields-1 ? "," : "") << std::endl;
            });

            os << Indent<indentLevel, indent>() << "}";
            return os;
        }
    };

    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    std::ostream & operator<<(std::ostream & os, const Json::Output<T, indentLevel> object)
    {
        return object.put(os, object.obj);
    }

    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    constexpr Output<T> out(const T & t)
    {
        return Output<T, indentLevel>(t);
    }
};

#endif