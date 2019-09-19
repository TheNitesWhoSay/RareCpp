#ifndef JSON_H
#define JSON_H
#include <string>
#include <ostream>
#include "Reflect.h"

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

                os << Indent(indent, indentLevel) << "\"" << field.fieldName << "\": ";
                field.IfPrimitive([&](auto) { os << value; });
                field.IfObject([&](auto) { os << Output<decltype(field)::type>(value, indent, indentLevel); });
                field.IfPrimitiveArray([&](auto) {
                    os << "[ ";
                    for ( size_t i=0; i<field.arraySize; i++ )
                        os << (i > 0 ? ", \"" : "\"") << value[i] << "\"";

                    os << " ]";
                });
                field.IfObjectArray([&](auto) {
                    os << "[" << std::endl << Indent(indent, indentLevel+1);
                    for ( size_t i=0; i<field.arraySize; i++ )
                    {
                        auto reflectedObject = value[i];
                        os << Output<decltype(reflectedObject)>(reflectedObject, indent, indentLevel+1);
                        if ( i < field.arraySize-1 )
                            os << ",";
                    }
                    os << std::endl << Indent(indent, indentLevel) << "]";
                });
                os << (field.fieldIndex < Class::totalFields ? "," : "") << std::endl;
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