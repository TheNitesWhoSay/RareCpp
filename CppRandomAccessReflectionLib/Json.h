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
        if constexpr ( indentLevel > 0 )
        {
            for ( size_t i=0; i<indentLevel; i++ )
                os << indent;
        }

        return os;
    }

    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    class Output
    {
    public:
        Output(const T & obj) : obj(obj) {}

        const T & obj;

        static constexpr std::ostream & putString(std::ostream & os, const std::string & str)
        {
            os << "\"";
            for ( size_t i=0; i<str.size(); i++ )
            {
                switch ( str[i] )
                {
                case '\"': os << "\\\""; break;
                case '\\': os << "\\\\"; break;
                case '/': os << "\\/"; break;
                case '\b': os << "\\b"; break;
                case '\f': os << "\\f"; break;
                case '\n': os << "\\n"; break;
                case '\r': os << "\\r"; break;
                case '\t': os << "\\t"; break;
                default: os << str[i]; break;
                }
            }
            os << "\"";
            return os;
        }

        static constexpr std::ostream & put(std::ostream & os, const T & obj)
        {
            os << "{" << std::endl;
            
            using Index = const size_t &;
            using Class = typename T::Class;
            Class::ForEachField(obj, [&](auto & field, auto & value) {
                
                using Field = typename std::remove_reference<decltype(field)>::type;
                using SubType = typename Field::sub_type;

                os << Indent<indentLevel+1, indent>() << "\"" << field.name << "\": ";
                if ( field.IsNull(value) )
                    os << "null";
                else if ( !field.isIterable )
                {
                    field.ForPrimitive(value, [&](auto & primitive) { // Primitive
                        if constexpr ( Field::IsString ) { putString(os, primitive); }
                        else { os << primitive; }
                    });
                    field.ForObject(value, [&](auto & object) { Output<SubType, indentLevel+1>::put(os, object); }); // Object
                }
                else if ( !field.containsPairs && !field.isReflected ) // Primitive Array
                {
                    os << "[ ";
                    field.ForPrimitivesConst(value, [&](Index index, auto & element) { // Primitive Array
                        if ( index > 0 ) { os << ", "; }
                        if constexpr ( Field::IsString ) { putString(os, element); }
                        else { os << element; }
                    });
                    field.ForPrimitivePointersConst(value, [&](Index index, auto & element) { // Primitive Pointer Array
                        if ( index > 0 ) { os << ", "; }
                        if ( element == nullptr ) { os << "null"; }
                        else if constexpr ( Field::IsString ) { putString(os, *element); }
                        else { os << *element; };
                    });
                    os << " ]";
                }
                else if ( !field.containsPairs && field.isReflected ) // Object Array
                {
                    os << "[" << std::endl << Indent<indentLevel+2, indent>();
                    field.ForObjectsConst(value, [&](Index index, auto & element) { // Object Array
                        if ( index > 0 ) { os << ", "; }
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
                            if ( index > 0 ) { os << ", "; }
                            if constexpr ( Field::IsString )
                            {
                                os << std::endl << Indent<indentLevel+2, indent>();
                                putString(os, first);
                                os << ": ";
                                putString(os, second);
                            }
                            else
                                os << std::endl << Indent<indentLevel+2, indent>() << "\"" << first << "\": " << second;
                        });
                        field.ForPrimitivePointerPairs(value, [&](Index index, auto & first, auto & second) { // Primitive Pointer Map
                            if ( index > 0 ) { os << ", "; }
                            os << std::endl << Indent<indentLevel+2, indent>() << "\"" << first << "\": ";
                            if ( second == nullptr ) { os << "null"; }
                            else if constexpr ( Field::IsString ) { putString(os, *second); }
                            else { os << *second; }
                        });
                    }
                    else
                    {
                        field.ForObjectPairs(value, [&](Index index, auto & first, auto & second) { // Object Map
                            if ( index > 0 ) { os << ", "; }
                            os << std::endl << Indent<indentLevel+2, indent>() << "\"" << first << "\": ";
                            Output<SubType, indentLevel+2>::put(os, second);
                        });
                        field.ForObjectPointerPairs(value, [&](Index index, auto & first, auto & second) { // Object Pointer Map
                            if ( index > 0 ) { os << ", "; }
                            os << std::endl << Indent<indentLevel+2, indent>() << "\"" << first << "\": ";
                            if ( second == nullptr ) { os << "null"; }
                            else { Output<SubType, indentLevel+2>::put(os, *second); }
                        });
                    }
                    os << std::endl << Indent<indentLevel+1, indent>() << "}";
				}
                if ( field.index < Class::totalFields-1 ) { os << "," << std::endl; }
                else { os << std::endl; }
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
        UnexpectedLineEnding(const char* lineEnding) : Exception(lineEnding) {}
    };

    class UnexpectedInputEnd : public Exception
    {
    public:
        UnexpectedInputEnd(const std::string & expected) : Exception((std::string("Expected ") + expected + " but found end of input").c_str()) {}
    };

    class StreamReadFail : public Exception
    {
    public:
        StreamReadFail() : Exception("Attempt to read from stream failed (std::ios::fail() == true)") {}
    };

    class StreamUngetFail : public Exception
    {
    public:
        StreamUngetFail(char c) : Exception((std::string("Attempt to unread last character (") + c + ") resulted in failure").c_str()) {}
    };
    
    enum_t(FieldType, uint8_t, {
        None,
        String,
        Number,
        Object,
        Array,
        True,
        False,
        Null
    });

    template <typename Field>
    std::string getExpectation(Field field)
    {
        std::string expectation;
        if constexpr ( Field::IsPointer )
            return "\"null\""; // TODO: More handling for pointers
        else if constexpr ( Field::IsBool )
            return "true or false";
        else if constexpr ( Field::IsArray )
            return "JSON Array";
        else if constexpr ( Field::IsObject )
            return "JSON Object";
        else if ( Field::IsString )
            return "JSON String";
        else
            return "TODO: Implement more expectations";
    }

    template <typename Field>
    class UnexpectedFieldValue : public Exception
    {
    public:
        UnexpectedFieldValue(Field field)
            : Exception((
                std::string("Unexpected value for field: \"") + field.name + "\". Expected: "
                + getExpectation<Field>(field)
            ).c_str()) {}

        UnexpectedFieldValue(Field field, const char* fieldValue)
            : Exception((
                std::string("Unexpected value: \"") + fieldValue + "\" for field: \"" + field.name + "\". Expected: "
                + getExpectation<Field>(field)
            ).c_str()) {}
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
                    << "      \"type\": "
                    << (fieldNameToJsonField.second.type == JsonField::Type::Regular ? "\"Regular\"" : "\"SuperClass\"")
                    << "," << std::endl
                    << "      \"name\": \"" << fieldNameToJsonField.second.name << "\"" << std::endl;

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

        static constexpr std::string readString(std::istream & is)
        {
            std::string result;
            bool finished = false;
            int c = '\0';
            do
            {
                c = is.get();
                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd("close quote");
                    else
                        throw StreamReadFail();
                }
                switch ( c )
                {
                    case '\"': // Closing quote
                        is.unget();
                        if ( !is.good() )
                            throw StreamUngetFail('\"');
                        finished = true;
                        break;
                    case '\\': // Escape sequence
                        c = is.get();
                        if ( !is.good() )
                        {
                            if ( is.eof() )
                                throw UnexpectedInputEnd("completion of string escape sequence");
                            else
                                throw StreamReadFail();
                        }
                        switch ( c )
                        {
                            case '\"': result += '\"'; break;
                            case '\\': result += '\\'; break;
                            case '/': result += '/'; break;
                            case 'b': result += '\b'; break;
                            case 'f': result += '\f'; break;
                            case 'n': result += '\n'; break;
                            case 'r': result += '\r'; break;
                            case 't': result += '\t'; break;
                            case 'u':
                            {
                                char hexEscapeSequence[6] = { 'u', '\0', '\0', '\0', '\0', '\0' };
                                for ( size_t i=1; i<5; i++ )
                                {
                                    c = is.get();
                                    if ( !is.good() )
                                    {
                                        if ( is.eof() )
                                            throw InvalidEscapeSequence((std::string(hexEscapeSequence)).c_str(), unicodeEscapeSequence);
                                        else
                                            throw StreamReadFail();
                                    }
                                    hexEscapeSequence[i] = c;
                                    if ( !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) )
                                        throw InvalidEscapeSequence((std::string(hexEscapeSequence)).c_str(), unicodeEscapeSequence);
                                }
                                for ( size_t i=1; i<5; i++ )
                                {
                                    char & cRef = hexEscapeSequence[i];
                                    if ( cRef >= '0' && cRef <= '9' )
                                        hexEscapeSequence[i] -= '0';
                                    else if ( cRef >= 'A' && cRef <= 'F' )
                                        hexEscapeSequence[i] = cRef - 'A' + 10;
                                    else if ( cRef >= 'a' && cRef <= 'f' )
                                        hexEscapeSequence[i] = cRef - 'a' + 10;
                                }
                                uint8_t highCharacter = 0x10 * hexEscapeSequence[1] + hexEscapeSequence[2];
                                uint8_t lowCharacter = 0x10 * hexEscapeSequence[3] + hexEscapeSequence[4];
                                if ( highCharacter > 0 )
                                    result += (char)highCharacter;
                         
                                result += (char)lowCharacter;
                            }
                            break;
                        }
                        break;
                    case '\n': throw UnexpectedLineEnding("\\n");
                    case '\r': throw UnexpectedLineEnding("\\r");
                    default: result += (char)c; break;
                }
            } while ( !finished );
            return result;
        }

        static constexpr void ignoreString(std::istream & is)
        {
            bool finished = false;
            int c = '\0';
            do
            {
                c = is.get();
                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd("close quote");
                    else
                        throw StreamReadFail();
                }
                switch ( c )
                {
                    case '\"': // Closing quote
                        is.unget();
                        if ( !is.good() )
                            throw StreamUngetFail('\"');
                        finished = true;
                        break;
                    case '\\': // Escape sequence
                        c = is.get();
                        if ( !is.good() )
                        {
                            if ( is.eof() )
                                throw UnexpectedInputEnd("completion of string escape sequence");
                            else
                                throw StreamReadFail();
                        }
                        switch ( c )
                        {
                            case '\"': case '\\': case '/': case 'b': case 'f': case 'n': case 'r': case 't': break;
                            case 'u':
                            {
                                char hexEscapeSequence[6] = { 'u', '\0', '\0', '\0', '\0', '\0' };
                                for ( size_t i=1; i<5; i++ )
                                {
                                    c = is.get();
                                    if ( !is.good() )
                                    {
                                        if ( is.eof() )
                                            throw InvalidEscapeSequence((std::string(hexEscapeSequence)).c_str(), unicodeEscapeSequence);
                                        else
                                            throw StreamReadFail();
                                    }
                                    hexEscapeSequence[i] = c;
                                    if ( !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) )
                                        throw InvalidEscapeSequence((std::string(hexEscapeSequence)).c_str(), unicodeEscapeSequence);
                                }
                            }
                            break;
                        }
                        break;
                    case '\n': throw UnexpectedLineEnding("\\n");
                    case '\r': throw UnexpectedLineEnding("\\r");
                    default: break;
                }
            } while ( !finished );
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

                    std::string fieldName;
                    try {
                        fieldName = readString(is);
                    } catch ( UnexpectedLineEnding & e) {
                        throw Exception((std::string("Expected field name close quote, found line ending (\"") + e.what() + "\")").c_str());
                    }
                    if ( fieldName.size() == 0 )
                        throw Exception("Expected field name, found empty quotes");

                    is >> c;
                    if ( c != '\"' )
                        throw Exception("Expected field name close quote");

                    is >> c;
                    if ( c != ':' )
                        throw Exception("Expected \":\"");

                    JsonField* jsonField = nullptr;
                    if ( numFieldsUnparsed > 0 )
                    {
                        std::multimap<size_t, JsonField> & fieldNameToJsonField = getClassFieldCache(t);
                        size_t fieldNameHash = strHash(fieldName);
                        auto fieldHashMatches = fieldNameToJsonField.equal_range(fieldNameHash);
                        for ( auto it = fieldHashMatches.first; it != fieldHashMatches.second; ++it )
                        {
                            if ( it->second.name.compare(fieldName) == 0 )
                                jsonField = &it->second;
                        }
                    }

                    is >> std::ws;
                    c = is.peek();
                    if ( jsonField != nullptr ) // Known field
                    {
                        Class::FieldAt(t, jsonField->index, [&](auto & field, auto & value) {
                            
                            using Field = typename std::remove_reference<decltype(field)>::type;

                            if constexpr ( field.IsPointer )
                            {
                                std::cout << "(ptr)" << std::endl;
                                // TODO: Pointers???
                            }

                            using SubType = typename std::remove_reference<decltype(value)>::type;
                            if constexpr ( Field::IsString ) // Must be valid quoted string
                            {
                                if ( c != '\"' )
                                    throw Exception("Expected string value open quote");

                                is.ignore(); // Consume the open quote
                                value = readString(is);

                                is >> c;
                                if ( c != '\"' )
                                    throw Exception((std::string("Expected string value close quote: ") + value).c_str());
                            }
                            else // Unquoted
                            {
                                switch ( c )
                                {
                                    case '\"': throw Exception("Invalid open quote starting non-string value");
                                    case '{': // TODO: Object or STL map or STL container of pairs, else error
                                        break;
                                    case '[': // TODO: Array or iterable STL container, else error
                                        break;
                                    case 't': // "true": bool or object assignable to bool, else error
                                    {
                                        is.ignore(); // Consume the 't'
                                        bool validTrue = false;
                                        is >> c;
                                        if ( c == 'r' )
                                        {
                                            is >> c;
                                            if ( c == 'u' )
                                            {
                                                if ( c == 'e' )
                                                {
                                                    is >> std::ws;
                                                    c = is.peek();
                                                    validTrue = c == ',' || c == '}';
                                                }
                                            }
                                        }
                                        if ( validTrue )
                                        {
                                            if constexpr ( field.IsBool )
                                                value = true;
                                            else
                                                throw UnexpectedFieldValue<Field>(field);
                                        }
                                        else
                                            throw UnexpectedFieldValue<Field>(field);
                                    }
                                    break;
                                    case 'f': // "false" bool or object assignable to bool, else error
                                    {
                                        is.ignore(); // Consume the 'f'
                                        bool validFalse = false;
                                        is >> c;
                                        if ( c == 'a' )
                                        {
                                            is >> c;
                                            if ( c == 'l' )
                                            {
                                                is >> c;
                                                if ( c == 's' )
                                                {
                                                    is >> c;
                                                    if ( c == 'e' )
                                                    {
                                                        is >> std::ws;
                                                        c = is.peek();
                                                        validFalse = c == ',' || c == '}';
                                                    }
                                                }
                                            }
                                        }
                                        if ( validFalse )
                                        {
                                            if constexpr ( field.IsBool )
                                                value = false;
                                            else
                                                throw UnexpectedFieldValue<Field>(field);
                                        }
                                        else
                                            throw UnexpectedFieldValue<Field>(field);
                                    }
                                    break;
                                    case 'n': // "null" pointer or object assignable to nullpointer, else error
                                    {
                                        is.ignore(); // Consume the 'n'
                                        bool validNull = false;
                                        is >> c;
                                        if ( c == 'u' )
                                        {
                                            is >> c;
                                            if ( c == 'l' )
                                            {
                                                is >> c;
                                                if ( c == 'l' )
                                                {
                                                    is >> std::ws;
                                                    c = is.peek();
                                                    validNull = c == ',' || c == '}';
                                                }
                                            }
                                        }
                                        if ( validNull )
                                        {
                                            if constexpr ( field.IsPointer )
                                                value = nullptr;
                                            else
                                                throw UnexpectedFieldValue<Field>(field);
                                        }
                                        else
                                            throw UnexpectedFieldValue<Field>(field);
                                    }
                                    break;
                                    case '-': // Number: primitive or object assignable to number, else error
                                    case '0': case '1': case '2': case '3': case '4':
                                    case '5': case '6': case '7': case '8': case '9':
                                        // TODO: Validate that this whole thing is a number
                                        if constexpr ( field.IsPrimitive )
                                            is >> value;
                                        // TODO: Check for object that can be assigned a number
                                        break;
                                    default: throw Exception("Expected field value (string, number, object, array, true, false, or null)");
                                }
                            }
                        });
                        numFieldsUnparsed --;
                    }
                    else // UnknownField
                    {
                        c = (char)is.peek();
                        if ( c == '\"' )
                        {
                            is.ignore(); // Consume the open quote

                            ignoreString(is); // Consume the string
                            
                            is >> c;
                            if ( c != '\"' )
                                throw Exception("Expected string value close quote");
                        }
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