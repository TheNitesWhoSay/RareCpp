#ifndef JSON_H
#define JSON_H
#include <ostream>
#include <sstream>
#include <typeinfo>
#include <typeindex>
#include <functional>
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

    class Exception : public std::exception
    {
    public:
        Exception(const char* what) : std::exception(what) {}
    };

    class InvalidEscapeSequence : public Exception
    {
    public:
        InvalidEscapeSequence(const char* escapeSequence, const char* expected)
            : Exception((std::string("Invalid escape sequence: \"") + escapeSequence + "\", expected: " + expected).c_str()) {}
    };

    class UnexpectedLineEnding : public Exception
    {
    public:
        UnexpectedLineEnding(const char* lineEnding, bool hadContent) : Exception(lineEnding), hadContent(hadContent) {}

        bool hadContent;
    };

    class JsonField
    {
    public:
        enum_t(Type, size_t, {
            Regular = 0,
            SuperClass = 1
        });
        
        JsonField() : index(0), type(Type::Regular), name("") {}
        JsonField(size_t index, Type fieldType, const std::string & name) : index(index), type(fieldType), name(name) {}
        
        size_t index;
        Type type;
        std::string name;
    };

    extern std::hash<std::string> strHash;

    extern std::map<std::type_index, std::multimap<size_t, JsonField>> classToNameHashToJsonField;

    std::string simplifyTypeStr(const std::string & superTypeStr)
    {
        std::string rawSimpleTypeStr = superTypeStr;
        if ( rawSimpleTypeStr.find("struct ", 0) != std::string::npos )
            rawSimpleTypeStr.erase(0, strlen("struct "));
        if ( rawSimpleTypeStr.find("class ", 0) != std::string::npos )
            rawSimpleTypeStr.erase(0, strlen("class "));

        std::string simpleTypeStr;
        for ( size_t i=0; i<rawSimpleTypeStr.size(); i++ )
        {
            if ( rawSimpleTypeStr[i] != ' ' )
                simpleTypeStr += rawSimpleTypeStr[i];
            else if ( ++i < rawSimpleTypeStr.size() ) // Remove space and upper-case the letter following the space
                simpleTypeStr += std::toupper(rawSimpleTypeStr[i]);
        }

        return simpleTypeStr;
    }

    template <typename T>
    std::string superTypeToJsonFieldName()
    {
        return std::string("__") + simplifyTypeStr(TypeToStr<T>());
    }

    template <typename T>
    static std::multimap<size_t, JsonField> & getClassFieldCache(const T & t)
    {
        using Class = typename T::Class;
        using Supers = typename T::Supers;
        auto found = classToNameHashToJsonField.find(std::type_index(typeid(T)));
        if ( found == classToNameHashToJsonField.end() )
        {
            auto inserted = classToNameHashToJsonField.insert(std::pair<std::type_index, std::multimap<size_t, JsonField>>(
                std::type_index(typeid(T)), std::multimap<size_t, JsonField>()));

            if ( !inserted.second )
                throw Exception("Failed to create cache for class!");

            for ( size_t fieldIndex = 0; fieldIndex < Class::totalFields; fieldIndex++ )
            {
                inserted.first->second.insert(std::pair<size_t, JsonField>(
                    strHash(Class::fields[fieldIndex].name), JsonField(fieldIndex, JsonField::Type::Regular, Class::fields[fieldIndex].name)));
            }

            for ( size_t superIndex = 0; superIndex < Supers::totalSupers; superIndex++ )
            {
                Supers::At(t, superIndex, [&](auto & superObj) {
                    using Super = typename std::remove_reference<decltype(superObj)>::type;
                    std::string superTypeFieldName = superTypeToJsonFieldName<Super>();
                    inserted.first->second.insert(std::pair<size_t, JsonField>(
                        strHash(superTypeFieldName), JsonField(superIndex, JsonField::Type::SuperClass, superTypeFieldName)));
                });
            }
                            
            return inserted.first->second;
        }
        return found->second;
    }

    static void putClassFieldCache(std::ostream & os)
    {
        os << "{" << std::endl;
        bool hasPrevPair = false;
        for ( auto & pair : classToNameHashToJsonField )
        {
            if ( hasPrevPair )
                os << "  }," << std::endl;

            os << "  \"" << simplifyTypeStr(pair.first.name()) << "\": {" << std::endl;
            bool hasPrevFieldNameToJsonField = false;
            for ( auto & fieldNameToJsonField : pair.second )
            {
                if ( hasPrevFieldNameToJsonField )
                    os << "    }," << std::endl;

                os << "    \"" << fieldNameToJsonField.first << "\": {" << std::endl
                    << "      \"index\": " << fieldNameToJsonField.second.index << "," << std::endl
                    << "      \"type\": " << (fieldNameToJsonField.second.type == JsonField::Type::Regular ? "Regular" : "SuperClass")
                    << "," << std::endl
                    << "      \"name\": " << fieldNameToJsonField.second.name << std::endl;

                hasPrevFieldNameToJsonField = true;
            }
            if ( hasPrevFieldNameToJsonField )
                os << "    }" << std::endl;
            hasPrevPair = true;
        }
        if ( hasPrevPair )
            os << "  }" << std::endl;
        os << "}" << std::endl;
    }

    template <typename T>
    class Input
    {
    public:
        static constexpr const char unicodeEscapeSequence[] = "\\uHHHH (where H's are hex characters)";
        static constexpr const char escapeSequences[] =
            "\\\\ or \\\" or \\/ or \\b or \\f or \\n or \\r or \\t or \\uHHHH (where H's are hex characters)";

        Input(T & obj) : obj(obj) {}

        T & obj;

        static constexpr std::string getString(std::stringbuf & rawStringBuf)
        {
            std::string result;
            std::string rawString = rawStringBuf.str();
            for ( size_t i=0; i<rawString.size(); i++ )
            {
                char c = rawString[i];
                switch ( c )
                {
                case '\\': // Escape sequence
                    if ( ++i < rawString.size() )
                    {
                        c = rawString[i];
                        switch ( c )
                        {
                            case '\"': result += '\"';
                            case '\\': result += '\\';
                            case '/': result += '/';
                            case 'b': result += '\b';
                            case 'f': result += '\f';
                            case 'n': result += '\n';
                            case 'r': result += '\r';
                            case 't': result += '\t';
                            case 'u':
                                if ( i+4 < rawString.size() &&
                                    rawString[i+1] >= '0' && rawString[i+1] <= '9' &&
                                    rawString[i+2] >= '0' && rawString[i+2] <= '9' &&
                                    rawString[i+3] >= '0' && rawString[i+3] <= '9' &&
                                    rawString[i+4] >= '0' && rawString[i+4] <= '9' )
                                {
                                    uint8_t highCharacter = 0x10 * rawString[++i] + rawString[++i];
                                    uint8_t lowCharacter = 0x10 * rawString[++i] + rawString[++i];
                                    if ( highCharacter > 0 ) result += (char)highCharacter;
                                    result += lowCharacter;
                                }
                                else
                                    throw InvalidEscapeSequence((std::string("\\u") +
                                        (i+4 < rawString.size() ? rawString.substr(i, 5) :
                                            rawString.substr(i, rawString.size()-i))).c_str(), unicodeEscapeSequence);
                                break;
                        }
                    }
                    else
                        throw InvalidEscapeSequence("\\", escapeSequences);
                    break;
                case '\n': throw UnexpectedLineEnding("\\n", !result.empty());
                case '\r': throw UnexpectedLineEnding("\\r", !result.empty());
                default: result += c;
                }
            }
            return result;
        }

        static std::istream & get(std::istream & is, T & t)
        {
            if ( is.good() )
            {
                char c;
                is >> c;
                if ( c != '{' )
                    throw Exception("Expected object opening \"{\"");
                
                using Index = const size_t &;
                using Class = typename T::Class;
                using Supers = typename T::Supers;
                size_t numFieldsUnparsed = Class::totalFields + Supers::totalSupers;
                while ( is.good() )
                {
                    is >> c;
                    if ( c == '}' )
                        return is;
                    else if ( c != '\"' )
                        throw Exception("Expected quoted field name or object closing \"}\"");

                    std::stringbuf fieldNameBuffer;
                    is.get(fieldNameBuffer, '\"');
                    
                    std::string fieldName;
                    try {
                        fieldName = getString(fieldNameBuffer);
                    } catch ( UnexpectedLineEnding & e) {
                        if ( e.hadContent )
                            throw Exception((std::string("Expected field name close quote, found line ending (\"") + e.what() + "\")").c_str());
                        else
                            throw Exception((std::string("Expected field name, found line ending (\"") + e.what() + "\")").c_str());
                    }
                    if ( fieldName.size() == 0 )
                        throw Exception("Expected field name, found empty quotes");

                    is >> c;
                    if ( c != '\"' )
                        throw Exception("Expected field name close quote");

                    is >> c;
                    if ( c != ':' )
                        throw Exception("Expected \":\"");

                    if ( numFieldsUnparsed > 0 )
                    {
                        std::multimap<size_t, JsonField> & fieldNameToJsonField = getClassFieldCache(t);
                        size_t fieldNameHash = strHash(fieldName);
                        auto fieldHashMatches = fieldNameToJsonField.equal_range(fieldNameHash);
                        JsonField* jsonField = nullptr;
                        for ( auto it = fieldHashMatches.first; it != fieldHashMatches.second; ++it )
                        {
                            if ( it->second.name.compare(fieldName) == 0 )
                                jsonField = &it->second;
                        }

                        if ( jsonField != nullptr ) // Known field
                        {
                            Class::FieldAt(t, jsonField->index, [&](auto & field, auto & value) {
                                field.ForPrimitive(value, [&](auto & primitive) {
                                    is >> primitive;
                                });
                            });

                            numFieldsUnparsed --;
                        }
                        // else // Unknown field
                    }

                    is >> c;
                    if ( c == '}' )
                        return is;
                    else if ( c != ',' )
                        throw Exception("Expected \",\" or object closing \"}\"");
                }
                if ( is.eof() )
                    throw Exception("Expected object closing \"}\"");

            }
            return is;
        }

    };

    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    std::ostream & operator<<(std::ostream & os, const Json::Output<T, indentLevel> object)
    {
        return object.put(os, object.obj);
    }

    template <typename T>
    std::istream & operator>>(std::istream & is, Json::Input<T> object)
    {
        return object.get(is, object.obj);
    }

    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    constexpr Output<T> out(const T & t)
    {
        return Output<T, indentLevel>(t);
    }

    template <typename T>
    constexpr Input<T> in(T & t)
    {
        return Input<T>(t);
    }
};

#define ENABLE_JSON_INPUT \
std::hash<std::string> Json::strHash; \
std::map<std::type_index, std::multimap<size_t, Json::JsonField>> Json::classToNameHashToJsonField

#endif