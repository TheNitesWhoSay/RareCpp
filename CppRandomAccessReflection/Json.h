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
            for ( size_t fieldIndex=0; fieldIndex<Class::totalFields; fieldIndex++ )
            {
                os << Indent(indent, indentLevel) << "\"" << Class::fieldNames[fieldIndex] << "\": ";

                if ( Class::isPrimitive(fieldIndex) )
                    Class::GetPrimitiveField(obj, fieldIndex, [&](auto primitive) { os << primitive; });
                else if ( Class::isReflectedObject(fieldIndex) )
                    Class::GetObjectField(obj, fieldIndex, [&](auto reflectedObject) { os << Output<decltype(reflectedObject)>(reflectedObject, indent, indentLevel); });
                else if ( Class::isPrimitiveArray(fieldIndex) )
                {
                    Class::GetPrimitiveArrayField(obj, fieldIndex, [&](auto primitiveArray) {
                        os << "[ ";
                        for ( size_t i=0; i<Class::arraySize[fieldIndex]; i++ )
                            os << (i > 0 ? ", \"" : "\"") << primitiveArray[i] << "\"";
                        
                        os << " ]";
                    });
                }
                else if ( Class::isReflectedObjectArray(fieldIndex) )
                {
                    os << "[" << std::endl << Indent(indent, indentLevel+1);
                    Class::GetObjectArrayField(obj, fieldIndex, [&](auto reflectedObjectArray) {
                        for ( size_t i=0; i<Class::arraySize[fieldIndex]; i++ )
                        {
                            auto reflectedObject = reflectedObjectArray[i];
                            os << Output<decltype(reflectedObject)>(reflectedObject, indent, indentLevel+1);
                            if ( i < Class::arraySize[fieldIndex]-1 )
                                os << ",";
                        }
                        os << std::endl << Indent(indent, indentLevel) << "]";
                    });
                }

                if ( fieldIndex < Class::totalFields-1 )
                    os << ",";

                os << std::endl;
            }
    
            indentLevel--;
            os << Indent(indent, indentLevel) << "}";
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