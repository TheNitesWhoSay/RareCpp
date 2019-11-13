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
    
    namespace Exceptions
    {
        class Exception : public std::exception
        {
        public:
            Exception(const char* what) : std::exception(what) {}
        };
    
        class InvalidNumericCharacter : public Exception
        {
        public:
            InvalidNumericCharacter(char invalidCharacter, const char* expected)
                : Exception((std::string("Invalid numeric character: \"") + invalidCharacter + "\" expected: " + expected).c_str()) {}
        };

        class InvalidSecondDecimal : public Exception
        {
        public:
            InvalidSecondDecimal()
                : Exception(std::string("Invalid second decimal, expected: [0-9], \",\", or \"}\"").c_str()) {}
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

        template <typename Field>
        class UnexpectedFieldValue : public Exception
        {
        public:
            template <typename Field>
            static std::string getExpectation(Field field)
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

        class InvalidUnknownFieldValue : public Exception
        {
        public:
            InvalidUnknownFieldValue() : Exception("Expected field value (string, number, object, array, true, false, or null)") {}
        };
    };
    using namespace Exceptions;

    namespace Generic
    {
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

        class Value;
        using ObjectPtr = std::shared_ptr<std::map<std::string, std::shared_ptr<Value>>>;
        using FieldsPtr = std::shared_ptr<std::map<std::string, std::shared_ptr<Value>>>;

        class Value {
        public:
            enum_t(Type, uint8_t, {
                None,
                Boolean,
                Number,
                String,
                Object,
                BoolArray,
                NumberArray,
                StringArray,
                ObjectArray,
                MixedArray
            });

            virtual ~Value() {};

            virtual Type type() = 0;
        
            virtual bool & boolean() = 0;
            virtual std::string & number() = 0;
            virtual std::string & string() = 0;
            virtual std::map<std::string, std::shared_ptr<Value>> & object() = 0;

            virtual size_t arraySize() = 0;
            virtual std::vector<bool> & boolArray() = 0;
            virtual std::vector<std::string> & numberArray() = 0;
            virtual std::vector<std::string> & stringArray() = 0;
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() = 0;
	        virtual std::vector<std::shared_ptr<Value>> & mixedArray() = 0;

            template <typename T>
            bool getNumber(T & num)
            {
                return (std::stringstream(number()) >> num).good();
            }

            template <typename T>
            void setNumber(const T & number)
            {
                number() = std::to_string(number);
            }
        };
	
        class Bool : public Value {
        public:
            Bool() : value(false) {}
            Bool(bool value) : value(value) {}
            Bool(const Bool & other) : value(other.value) {}
            virtual ~Bool() {}
        
            virtual Type type() { return Value::Type::Boolean; }
        
            virtual bool & boolean() { return value; }
            virtual std::string & number() { throw Exception("Cannot call number() on a Json::Bool type!"); }
            virtual std::string & string() { throw Exception("Cannot call string() on a Json::Bool type!"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw Exception("Cannot call object() on a Json::Bool type!"); }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::Bool type!"); }

            virtual std::vector<bool> & boolArray() { throw Exception("Cannot call boolArray() on a Json::Bool type!"); }
            virtual std::vector<std::string> & numberArray() { throw Exception("Cannot call numberArray() on a Json::Bool type!"); }
            virtual std::vector<std::string> & stringArray() { throw Exception("Cannot call stringArray() on a Json::Bool type!"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw Exception("Cannot call objectArray() on a Json::Bool type!");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw Exception("Cannot call mixedArray() on a Json::Bool type!"); }

        private:
	        bool value;
        };
        class Number : public Value {
        public:
            Number() : value() {}
            Number(const std::string & value) : value(value) {}
            Number(const Number & other) : value(other.value) {}
            virtual ~Number() {}
        
            virtual Type type() { return Value::Type::Number; }
        
            virtual bool & boolean() { throw Exception("Cannot call boolean() on a Json::Number type!"); }
            virtual std::string & number() { return value; }
            virtual std::string & string() { throw Exception("Cannot call string() on a Json::Number type!"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw Exception("Cannot call object() on a Json::Number type!"); }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::Number type!"); }

            virtual std::vector<bool> & boolArray() { throw Exception("Cannot call boolArray() on a Json::Number type!"); }
            virtual std::vector<std::string> & numberArray() { throw Exception("Cannot call numberArray() on a Json::Number type!"); }
            virtual std::vector<std::string> & stringArray() { throw Exception("Cannot call stringArray() on a Json::Number type!"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw Exception("Cannot call objectArray() on a Json::Number type!");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw Exception("Cannot call mixedArray() on a Json::Number type!"); }
        
        private:
	        std::string value;
        };
        class String : public Value {
        public:
            String() : value() {}
            String(const std::string & value) : value(value) {}
            String(const String & other) : value(other.value) {}
            virtual ~String() {}
        
            virtual Type type() { return Value::Type::String; }
        
            virtual bool & boolean() { throw Exception("Cannot call boolean() on a Json::String type!"); }
            virtual std::string & number() { throw Exception("Cannot call number() on a Json::String type!"); }
            virtual std::string & string() { return value; }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw Exception("Cannot call object() on a Json::String type!"); }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::String type!"); }

            virtual std::vector<bool> & boolArray() { throw Exception("Cannot call boolArray() on a Json::String type!"); }
            virtual std::vector<std::string> & numberArray() { throw Exception("Cannot call numberArray() on a Json::String type!"); }
            virtual std::vector<std::string> & stringArray() { throw Exception("Cannot call stringArray() on a Json::String type!"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw Exception("Cannot call objectArray() on a Json::String type!");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw Exception("Cannot call mixedArray() on a Json::String type!"); }
        
        private:
	        std::string value;
        };
        class Object : public Value {
        public:
            Object() : value() {}
            Object(const Object & other) : value(other.value) {}
            virtual ~Object() {}
        
            virtual Type type() { return Value::Type::Object; }
        
            virtual bool & boolean() { throw Exception("Cannot call boolean() on a Json::Object type!"); }
            virtual std::string & number() { throw Exception("Cannot call number() on a Json::Object type!"); }
            virtual std::string & string() { throw Exception("Cannot call string() on a Json::Object type!"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { return value; }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::Object type!"); }

            virtual std::vector<bool> & boolArray() { throw Exception("Cannot call boolArray() on a Json::Object type!"); }
            virtual std::vector<std::string> & numberArray() { throw Exception("Cannot call numberArray() on a Json::Object type!"); }
            virtual std::vector<std::string> & stringArray() { throw Exception("Cannot call stringArray() on a Json::Object type!"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw Exception("Cannot call objectArray() on a Json::Object type!");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw Exception("Cannot call mixedArray() on a Json::Object type!"); }
        
        private:
	        std::map<std::string, std::shared_ptr<Value>> value;
        };
	
        class BoolArray : public Value {
        public:
            BoolArray() : values() {}
            BoolArray(const BoolArray & other) : values(other.values) {}
            virtual ~BoolArray() {}
        
            virtual Type type() { return Value::Type::BoolArray; }
        
            virtual bool & boolean() { throw Exception("Cannot call boolean() on a Json::BoolArray type!"); }
            virtual std::string & number() { throw Exception("Cannot call number() on a Json::BoolArray type!"); }
            virtual std::string & string() { throw Exception("Cannot call string() on a Json::BoolArray type!"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw Exception("Cannot call object() on a Json::BoolArray type!"); }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::BoolArray type!"); }

            virtual std::vector<bool> & boolArray() { return values; }
            virtual std::vector<std::string> & numberArray() { throw Exception("Cannot call numberArray() on a Json::BoolArray type!"); }
            virtual std::vector<std::string> & stringArray() { throw Exception("Cannot call stringArray() on a Json::BoolArray type!"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw Exception("Cannot call objectArray() on a Json::BoolArray type!");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw Exception("Cannot call mixedArray() on a Json::BoolArray type!"); }
        
        private:
	        std::vector<bool> values;
        };
        class NumberArray : public Value {
        public:
            NumberArray() : values() {}
            NumberArray(const NumberArray & other) : values(other.values) {}
            virtual ~NumberArray() {}
        
            virtual Type type() { return Value::Type::NumberArray; }
        
            virtual bool & boolean() { throw Exception("Cannot call boolean() on a Json::NumberArray type!"); }
            virtual std::string & number() { throw Exception("Cannot call number() on a Json::NumberArray type!"); }
            virtual std::string & string() { throw Exception("Cannot call string() on a Json::NumberArray type!"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw Exception("Cannot call object() on a Json::NumberArray type!"); }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::NumberArray type!"); }

            virtual std::vector<bool> & boolArray() { throw Exception("Cannot call boolArray() on a Json::NumberArray type!"); }
            virtual std::vector<std::string> & numberArray() { return values; }
            virtual std::vector<std::string> & stringArray() { throw Exception("Cannot call stringArray() on a Json::NumberArray type!"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw Exception("Cannot call objectArray() on a Json::NumberArray type!");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw Exception("Cannot call mixedArray() on a Json::NumberArray type!"); }
        
        private:
	        std::vector<std::string> values;
        };
        class StringArray : public Value {
        public:
            StringArray() : values() {}
            StringArray(const StringArray & other) : values(other.values) {}
            virtual ~StringArray() {}
        
            virtual Type type() { return Value::Type::StringArray; }
        
            virtual bool & boolean() { throw Exception("Cannot call boolean() on a Json::StringArray type!"); }
            virtual std::string & number() { throw Exception("Cannot call number() on a Json::StringArray type!"); }
            virtual std::string & string() { throw Exception("Cannot call string() on a Json::StringArray type!"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw Exception("Cannot call object() on a Json::StringArray type!"); }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::StringArray type!"); }

            virtual std::vector<bool> & boolArray() { throw Exception("Cannot call boolArray() on a Json::StringArray type!"); }
            virtual std::vector<std::string> & numberArray() { throw Exception("Cannot call numberArray() on a Json::StringArray type!"); }
            virtual std::vector<std::string> & stringArray() { return values; }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw Exception("Cannot call objectArray() on a Json::StringArray type!");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw Exception("Cannot call mixedArray() on a Json::StringArray type!"); }
        
        private:
	        std::vector<std::string> values;
        };
        class ObjectArray : public Value {
        public:
            ObjectArray() : values() {}
            ObjectArray(const ObjectArray & other) : values(other.values) {}
            virtual ~ObjectArray() {}
        
            virtual Type type() { return Value::Type::ObjectArray; }
        
            virtual bool & boolean() { throw Exception("Cannot call boolean() on a Json::ObjectArray type!"); }
            virtual std::string & number() { throw Exception("Cannot call number() on a Json::ObjectArray type!"); }
            virtual std::string & string() { throw Exception("Cannot call string() on a Json::ObjectArray type!"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw Exception("Cannot call object() on a Json::ObjectArray type!"); }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::ObjectArray type!"); }

            virtual std::vector<bool> & boolArray() { throw Exception("Cannot call boolArray() on a Json::ObjectArray type!"); }
            virtual std::vector<std::string> & numberArray() { throw Exception("Cannot call numberArray() on a Json::ObjectArray type!"); }
            virtual std::vector<std::string> & stringArray() { throw Exception("Cannot call stringArray() on a Json::ObjectArray type!"); }
            virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() { return values; }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw Exception("Cannot call mixedArray() on a Json::ObjectArray type!"); }
        
        private:
	        std::vector<std::map<std::string, std::shared_ptr<Value>>> values;
        };
        class MixedArray : public Value {
        public:
            MixedArray() : values() {}
            MixedArray(const MixedArray & other) : values(other.values) {}
            virtual ~MixedArray() {}
        
            virtual Type type() { return Value::Type::MixedArray; }
        
            virtual bool & boolean() { throw Exception("Cannot call boolean() on a Json::MixedArray type!"); }
            virtual std::string & number() { throw Exception("Cannot call number() on a Json::MixedArray type!"); }
            virtual std::string & string() { throw Exception("Cannot call string() on a Json::MixedArray type!"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw Exception("Cannot call object() on a Json::MixedArray type!"); }

            virtual size_t arraySize() { throw Exception("Cannot call arraySize() on a Json::MixedArray type!"); }

            virtual std::vector<bool> & boolArray() { throw Exception("Cannot call boolArray() on a Json::MixedArray type!"); }
            virtual std::vector<std::string> & numberArray() { throw Exception("Cannot call numberArray() on a Json::MixedArray type!"); }
            virtual std::vector<std::string> & stringArray() { throw Exception("Cannot call stringArray() on a Json::MixedArray type!"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw Exception("Cannot call objectArray() on a Json::MixedArray type!");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { return values; }

        private:
            std::vector<std::shared_ptr<Value>> values;
        };
    };
    using namespace Generic;

    namespace Affixes
    {
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

        template <bool containsPrimitives, size_t indentLevel, const char* indent = twoSpaces>
        struct ArrayPrefix { };

        template <bool containsPrimitives, size_t indentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const ArrayPrefix<containsPrimitives, indentLevel, indent>)
        {
            if constexpr ( containsPrimitives )
                os << "[ ";
            else
                os << "[" << std::endl << Indent<indentLevel, indent>();

            return os;
        }

        template <bool containsPrimitives, size_t indentLevel, const char* indent = twoSpaces>
        struct ArraySuffix { };

        template <bool containsPrimitives, size_t indentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const ArraySuffix<containsPrimitives, indentLevel, indent>)
        {
            if constexpr ( containsPrimitives )
                os << " ]";
            else
                os << std::endl << Indent<indentLevel, indent>() << "]";

            return os;
        }

        template <size_t indentLevel, const char* indent = twoSpaces>
        struct ObjectPrefix { };

        template <size_t indentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const ObjectPrefix<indentLevel, indent>)
        {
            os << "{";
            return os;
        }
        
        template <size_t indentLevel, const char* indent = twoSpaces>
        struct ObjectSuffix { };

        template <size_t indentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const ObjectSuffix<indentLevel, indent>)
        {
            os << std::endl << Indent<indentLevel, indent>() << "}";
            return os;
        }
        
        template <bool isArray, bool containsPrimitives, size_t indentLevel, const char* indent = twoSpaces>
        struct NestedPrefix { };

        template <bool isArray, bool containsPrimitives, size_t indentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const NestedPrefix<isArray, containsPrimitives, indentLevel, indent>)
        {
            if constexpr ( isArray )
                os << ArrayPrefix<containsPrimitives, indentLevel, indent>();
            else
                os << ObjectPrefix<indentLevel, indent>();

            return os;
        }

        template <bool isArray, bool containsPrimitives, size_t indentLevel, const char* indent = twoSpaces>
        struct NestedSuffix { };

        template <bool isArray, bool containsPrimitives, size_t indentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const NestedSuffix<isArray, containsPrimitives, indentLevel, indent>)
        {
            if constexpr ( isArray )
                os << ArraySuffix<containsPrimitives, indentLevel, indent>();
            else
                os << ObjectSuffix<indentLevel, indent>();

            return os;
        }
        
        template <bool isFirst, size_t indentLevel, const char* indent = twoSpaces>
        struct FieldPrefix { };

        template <bool isFirst, size_t indentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const FieldPrefix<isFirst, indentLevel, indent>)
        {
            if constexpr ( isFirst )
                os << std::endl << Indent<indentLevel, indent>();
            else 
                os << "," << std::endl << Indent<indentLevel, indent>();

            return os;
        }

        struct FieldNameValueSeparator { };

        std::ostream & operator<<(std::ostream & os, const FieldNameValueSeparator)
        {
            os << ": ";
            return os;
        }
                
        template <bool isJsonField, bool nestedSeparator, size_t indentLevel, const char* indent = twoSpaces>
        struct Separator
        {
            bool isFirst;

            Separator() : isFirst(false) {}
            Separator(bool isFirst) : isFirst(isFirst) {}
        };

        template <bool isJsonField, bool nestedSeparator, size_t indentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const Separator<isJsonField, nestedSeparator, indentLevel, indent> separator)
        {
            if constexpr ( isJsonField )
            {
                if ( separator.isFirst )
                    os << std::endl << Indent<indentLevel, indent>();
                else 
                    os << "," << std::endl << Indent<indentLevel, indent>();
            }
            else if ( !separator.isFirst )
            {
                if constexpr ( nestedSeparator )
                    os << "," << std::endl << Indent<indentLevel, indent>();
                else
                    os << ", ";
            }

            return os;
        }
    };
    using namespace Affixes;

    namespace Cache
    {
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

                if constexpr ( Class::totalFields > 0 )
                {
                    for ( size_t fieldIndex = 0; fieldIndex < Class::totalFields; fieldIndex++ )
                    {
                        inserted.first->second.insert(std::pair<size_t, JsonField>(
                            strHash(Class::fields[fieldIndex].name), JsonField(fieldIndex, JsonField::Type::Regular, Class::fields[fieldIndex].name)));
                    }
                }

                if constexpr ( Supers::totalSupers > 0 )
                {
                    for ( size_t superIndex = 0; superIndex < Supers::totalSupers; superIndex++ )
                    {
                        Supers::At(t, superIndex, [&](auto & superObj) {
                            using Super = typename std::remove_reference<decltype(superObj)>::type;
                            std::string superTypeFieldName = superTypeToJsonFieldName<Super>();
                            inserted.first->second.insert(std::pair<size_t, JsonField>(
                                strHash(superTypeFieldName), JsonField(superIndex, JsonField::Type::SuperClass, superTypeFieldName)));
                        });
                    }
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

        #define ENABLE_JSON_INPUT \
        std::hash<std::string> Json::strHash; \
        std::map<std::type_index, std::multimap<size_t, Json::Generic::JsonField>> Json::classToNameHashToJsonField
    };
    using namespace Cache;

    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    class Output
    {
    public:
        using Index = const size_t &;

        Output(const T & obj) : obj(obj) {}

        const T & obj;

        static constexpr void putString(std::ostream & os, const std::string & str)
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
        }

        template <typename T>
        static constexpr void putString(std::ostream & os, const T & t)
        {
            std::stringstream ss;
            ss << t;
            putString(os, ss.str());
        }
        
        template <size_t totalParentIterables, typename Field, typename Element>
        static constexpr void putValue(std::ostream & os, const Element & element)
        {
            if constexpr ( is_pointable<Element>::value )
            {
                if ( element == nullptr )
                    os << "null";
                else if constexpr ( is_iterable<Element>::value )
                    putIterable<totalParentIterables, Field, Element>(os, *element);
                else if constexpr ( Field::IsReflected )
                    Output<Field::sub_type, indentLevel+totalParentIterables+1>::put(os, *element);
                else if constexpr ( Field::IsString )
                    putString(os, *element);
                else
                    os << *element;
            }
            else if constexpr ( is_iterable<Element>::value )
                putIterable<totalParentIterables, Field, Element>(os, element);
            else if constexpr ( Field::IsReflected )
                Output<Field::sub_type, indentLevel+totalParentIterables+1>::put(os, element);
            else if constexpr ( Field::IsString )
                putString(os, element);
            else
                os << element;
        }

        template <size_t totalParentIterables, typename Field, typename Iterable>
        static constexpr void putIterable(std::ostream & os, const Iterable & iterable)
        {
            using Element = typename element_type<Iterable>::type;
            constexpr bool containsPrimitives = !Field::IsReflected && !is_iterable<Element>::value;
            constexpr bool containsPairs = contains_pairs<Iterable>::value;
            
            size_t i=0;
            os << NestedPrefix<!contains_pairs<Iterable>::value, containsPrimitives, indentLevel+totalParentIterables+2, indent>();
            if constexpr ( is_stl_iterable<Iterable>::value )
            {
                for ( auto & element : iterable )
                {
                    os << Separator<containsPairs, is_iterable<Element>::value, indentLevel+totalParentIterables+2, indent>(0 == i++);
                    if constexpr ( containsPairs )
                    {
                        putString(os, element.first);
                        os << FieldNameValueSeparator();
                        putValue<totalParentIterables+1, Field, Element>(os, element.second);
                    }
                    else // Not pairs
                        putValue<totalParentIterables+1, Field, Element>(os, element);
                }
            }
            else if constexpr ( is_adaptor<Iterable>::value )
            {
                const auto & sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                {
                    os << Separator<containsPairs, is_iterable<Element>::value, indentLevel+totalParentIterables+2, indent>(0 == i++);
                    putValue<totalParentIterables+1, Field, Element>(os, *it);
                }
            }
            else if constexpr ( std::is_array<Iterable>::value && Field::ArraySizes[totalParentIterables] > 0 )
            {
                for ( ; i<Field::ArraySizes[totalParentIterables]; i++ )
                {
                    os << Separator<containsPairs, is_iterable<Element>::value, indentLevel+totalParentIterables+2, indent>(0 == i);
                    putValue<totalParentIterables+1, Field, Element>(os, iterable[i]);
                }
            }
            os << NestedSuffix<!contains_pairs<Iterable>::value, containsPrimitives, indentLevel+totalParentIterables+1, indent>();
        }
        
        static constexpr std::ostream & put(std::ostream & os, const T & obj)
        {
            os << ObjectPrefix<indentLevel, indent>();

            T::Class::ForEachField(obj, [&](auto & field, auto & value)
            {
                using Field = typename std::remove_reference<decltype(field)>::type;

                os << FieldPrefix<Field::Index == 0, indentLevel+1, indent>();
                putString(os, field.name);
                os << FieldNameValueSeparator();
                putValue<0, Field>(os, value);
            });

            os << ObjectSuffix<indentLevel, indent>();
            return os;
        }
    };
    
    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    constexpr Output<T> out(const T & t)
    {
        return Output<T, indentLevel>(t);
    }
    
    template <typename T, size_t indentLevel = 0, const char* indent = twoSpaces>
    std::ostream & operator<<(std::ostream & os, const Json::Output<T, indentLevel> object)
    {
        return object.put(os, object.obj);
    }
    
    template <typename T>
    class Input
    {
    public:
        Input(T & obj) : obj(obj) {}

        T & obj;

        static constexpr const char unicodeEscapeSequence[] = "\\uHHHH (where H's are hex characters)";
        static constexpr const char escapeSequences[] =
            "\\\\ or \\\" or \\/ or \\b or \\f or \\n or \\r or \\t or \\uHHHH (where H's are hex characters)";

        static constexpr inline void checkedPeek(std::istream & is, char & c, const char* expectedDescription)
        {
            int character = is.peek();
            if ( !is.good() )
            {
                if ( is.eof() )
                    throw UnexpectedInputEnd(expectedDescription);
                else
                    throw StreamReadFail();
            }
            c = (char)character;
        }

        static constexpr inline void checkedGet(std::istream & is, char & c, const char* expectedDescription)
        {
            is >> c;
            if ( !is.good() )
            {
                if ( is.eof() )
                    throw UnexpectedInputEnd(expectedDescription);
                else
                    throw StreamReadFail();
            }
        }
        
        static constexpr inline void checkedGet(std::istream & is, char & c, int expectedChar, const char* expectedDescription)
        {
            is >> c;
            if ( !is.good() )
            {
                if ( is.eof() )
                    throw UnexpectedInputEnd(expectedDescription);
                else
                    throw StreamReadFail();
            }
            else if ( c != expectedChar )
                throw Exception((std::string("Expected: ") + expectedDescription).c_str());
        }

        static constexpr inline bool checkedUnget(std::istream & is, char ungetting)
        {
            is.unget();
            if ( !is.good() )
                throw StreamUngetFail(ungetting);

            return true;
        }

        static constexpr inline void checkedEscapeSequenceGet(std::istream & is, char & c, const char* hexEscapeSequence)
        {
            is >> c;
            if ( !is.good() )
            {
                if ( is.eof() )
                    throw InvalidEscapeSequence((std::string(hexEscapeSequence)).c_str(), unicodeEscapeSequence);
                else
                    throw StreamReadFail();
            }
        }

        static constexpr inline void checkedWhitespaceGet(std::istream & is, const char* expectedDescription)
        {
            is >> std::ws;
            if ( !is.good() )
            {
                if ( is.eof() )
                    throw UnexpectedInputEnd(expectedDescription);
                else
                    throw StreamReadFail();
            }
        }

        static constexpr std::string readString(std::istream & is)
        {
            std::string result;
            bool finished = false;
            char c = '\0';
            do
            {
                checkedGet(is, c, "close quote");
                switch ( c )
                {
                    case '\\': // Escape sequence
                        checkedGet(is, c, "completion of string escape sequence");
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
                                    checkedEscapeSequenceGet(is, c, hexEscapeSequence);
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
                    case '\"': finished = checkedUnget(is, '\"'); break; // Closing quote
                    default: result += c; break;
                }
            } while ( !finished );
            return result;
        }

        static constexpr void ignoreString(std::istream & is)
        {
            bool finished = false;
            char c = '\0';
            do
            {
                checkedGet(is, c, "close quote");
                switch ( c )
                {
                    case '\\': // Escape sequence
                        checkedGet(is, c, "completion of string escape sequence");
                        switch ( c )
                        {
                            case '\"': case '\\': case '/': case 'b': case 'f': case 'n': case 'r': case 't': break;
                            case 'u':
                            {
                                char hexEscapeSequence[6] = { 'u', '\0', '\0', '\0', '\0', '\0' };
                                for ( size_t i=1; i<5; i++ )
                                {
                                    checkedEscapeSequenceGet(is, c, hexEscapeSequence);
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
                    case '\"': finished = checkedUnget(is, '\"'); break; // Closing quote
                    default: break;
                }
            } while ( !finished );
        }

        static constexpr void ignoreQuotedString(std::istream & is)
        {
            char c = '\0';
            checkedGet(is, c, '\"', "string value open quote");
            ignoreString(is); // Consume the string
            checkedGet(is, c, '\"', "string value close quote");
        }

        static constexpr void readNumber(std::istream & is, std::stringstream & ss, bool isArray = false)
        {
            bool decimal = false;
            bool finished = false;
            char c = '\0';
            checkedGet(is, c, "\"-\" or [0-9]");
            switch ( c )
            {
                case '-': case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    ss.put(c);
                    break;
                default: throw InvalidNumericCharacter(c, "\"-\" or [0-9]");
            }
            do
            {
                checkedGet(is, c, "\",\" or \"}\"");
                switch ( c )
                {
                    case '.':
                        if ( decimal )
                            throw InvalidSecondDecimal();
                        else
                        {
                            ss.put('.');
                            decimal = true;
                        }
                        break;
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        ss.put(c);
                        break;
                    case ',':
                        finished = checkedUnget(is, c);
                        break;
                    case '}':
                        if ( isArray )
                            throw Exception("Invalid object terminator for array");
                        else
                            finished = checkedUnget(is, c);
                        break;
                    case ']':
                        if ( isArray )
                            finished = checkedUnget(is, c);
                        else
                            throw Exception("Invalid array terminator \"]\" for non-array");
                        break;
                    case ' ': case '\t': case '\n': case '\r':
                        checkedWhitespaceGet(is, decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\"");
                        checkedGet(is, c, (decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\""));
                        if ( c != ',' && c != '}' )
                            throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\""));
                        else
                            finished = checkedUnget(is, c);
                        break;
                    default:
                        throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\""));
                        break;
                }
            }
            while ( !finished );
        }
        
        static constexpr void ignoreNumber(std::istream & is)
        {
            bool decimal = false;
            bool finished = false;
            char c = '\0';
            checkedGet(is, c, "\"-\" or [0-9]");
            switch ( c )
            {
                case '-': case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9': break;
                default: throw InvalidNumericCharacter(c, "\"-\" or [0-9]");
            }
            do
            {
                checkedGet(is, c, "\",\" or \"}\"");
                switch ( c )
                {
                    case '.':
                        if ( decimal )
                            throw InvalidSecondDecimal();
                        else
                            decimal = true;
                        break;
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        break;
                    case ',': case '}':
                        finished = checkedUnget(is, '\"');
                        break;
                    case ' ': case '\t': case '\n': case '\r':
                        checkedWhitespaceGet(is, decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\"");
                        checkedGet(is, c, (decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\""));
                        if ( c != ',' && c != '}' )
                            throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\""));
                        else
                            finished = checkedUnget(is, '\"');
                        break;
                    default:
                        throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\""));
                        break;
                }
            }
            while ( !finished );
        }

        static constexpr void ignoreObject(std::istream & is)
        {
            char c = '\0';
            checkedGet(is, c, '{', "object opening \"{\"");
            do
            {
                checkedGet(is, c, "quoted field name or object closing \"}\"");
                if ( c == '}' ) // Object end
                    return;
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

                checkedGet(is, c, '\"', "field name close quote");
                checkedGet(is, c, ':', "\":\"");
                checkedWhitespaceGet(is, "field value (string, number, object, array, true, false, or null)");
                checkedPeek(is, c, "field value (string, number, object, array, true, false, or null)");
                switch ( c )
                {
                    case '\"': ignoreQuotedString(is); break; // String or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9': ignoreNumber(is); break; // Number or error
                    case '{': ignoreObject(is); break; // JSON object or error
                    case '[': ignoreArray(is); break; // JSON array or error
                    case 't': readTrue(is); throw InvalidUnknownFieldValue(); break; // "true" or error
                    case 'f': readFalse(is); throw InvalidUnknownFieldValue(); break; // "false" or error
                    case 'n': readNull(is); throw InvalidUnknownFieldValue(); break; // "null" or error
                    default: throw InvalidUnknownFieldValue(); break;
                }
            
                checkedGet(is, c, "\",\" or object closing \"}\"");
                if ( c == '}' )
                    return;
                else if ( c != ',' )
                    throw Exception("Expected \",\" or object closing \"}\"");
            } while ( true );
        }

        static constexpr void ignoreArray(std::istream & is)
        {
            char c = '\0';
            checkedGet(is, c, '[', "array opening \"[\"");
            checkedWhitespaceGet(is, "array value (string, number, object, array, true, false, or null) or array closing \"]\"");
            checkedPeek(is, c, "array value (string, number, object, array, true, false, or null) or array closing \"]\"");
            do
            {
                switch ( c )
                {
                    case '\"': ignoreQuotedString(is); break; // String or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9': ignoreNumber(is); break; // Number or error
                    case '{': ignoreObject(is); break; // JSON object or error
                    case '[': ignoreArray(is); break; // JSON array or error
                    case 't': readTrue(is); throw InvalidUnknownFieldValue(); break; // "true" or error
                    case 'f': readFalse(is); throw InvalidUnknownFieldValue(); break; // "false" or error
                    case 'n': readNull(is); throw InvalidUnknownFieldValue(); break; // "null" or error
                    default: throw InvalidUnknownFieldValue(); break;
                }

                checkedGet(is, c, "\",\" or array closing \"]\"");
                if ( c == ']' )
                    return;
                else if ( c != ',' )
                    throw Exception("Expected \",\" or array closing \"]\"");
            } while ( true );
        }

        static constexpr void readTrue(std::istream & is, bool isArray = false)
        {
            char c = '\0';
            int expectation[] = { 't', 'r', 'u', 'e' };
            for ( size_t i=0; i<4; i++ )
                checkedGet(is, c, expectation[i], "completion of field value");

            checkedWhitespaceGet(is, "completion of field value");
            checkedPeek(is, c, "\",\" or \"}\"");
            if ( isArray && c != ',' && c != ']' )
                throw Exception("Expected: \",\" or \"]\"");
            else if ( !isArray && c != ',' && c != '}' )
                throw Exception("Expected: \",\" or \"}\"");
        }
        
        static constexpr void readFalse(std::istream & is, bool isArray = false)
        {
            char c = '\0';
            int expectation[] = { 'f', 'a', 'l', 's', 'e' };
            for ( size_t i=0; i<5; i++ )
                checkedGet(is, c, expectation[i], "completion of field value");

            checkedWhitespaceGet(is, "completion of field value");
            checkedPeek(is, c, "\",\" or \"}\"");
            if ( isArray && c != ',' && c != ']' )
                throw Exception("Expected: \",\" or \"]\"");
            else if ( !isArray && c != ',' && c != '}' )
                throw Exception("Expected: \",\" or \"}\"");
        }

        static constexpr void readNull(std::istream & is, bool isArray = false)
        {
            char c = '\0';
            int expectation[] = { 'n', 'u', 'l', 'l' };
            for ( size_t i=0; i<4; i++ )
                checkedGet(is, c, expectation[i], "completion of field value");

            checkedWhitespaceGet(is, "completion of field value");
            checkedPeek(is, c, "\",\" or \"}\"");
            if ( isArray && c != ',' && c != ']' )
                throw Exception("Expected: \",\" or \"]\"");
            else if ( !isArray && c != ',' && c != '}' )
                throw Exception("Expected: \",\" or \"}\"");
        }
        
        template <typename Field, typename Value>
        static constexpr void readString(std::istream & is, Field & field, Value & value)
        {
            char c = '\0';
            checkedGet(is, c, '\"', "string value open quote");
            std::string str = readString(is);
            std::stringstream ss(str);
            ss >> value;
            checkedGet(is, c, '\"', "string value close quote");
        }

        template <typename Field, typename Value>
        static constexpr void readNumber(std::istream & is, Field & field, Value & value)
        {
            std::stringstream ss;
            readNumber(is, ss, false);
            if constexpr ( field.IsPrimitive )
                ss >> value;
            else
                ; // TODO: Object overriding instream?
        }

        template <typename Field, typename Value>
        static constexpr void readArrayNumber(std::istream & is, Field & field, Value & value)
        {
            std::stringstream ss;
            readNumber(is, ss, true);
            if constexpr ( field.ContainsPrimitives )
                ss >> value;
            else
                ; // TODO: Object overriding instream?
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::pair<L, R> & dest)
        {
            char c = '\0';
            checkedGet(is, c, "quoted field name or object closing \"}\"");
            if ( c == '}' ) // Object end
                return;
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

            checkedGet(is, c, '\"', "field name close quote");

            if ( (std::stringstream(fieldName) >> dest.first).bad() )
                throw Exception("Failed to read JSON object field name into pair LHS");

            checkedGet(is, c, ':', "\":\"");

            if constexpr ( Field::ContainsPointers )
            {
                char c = '\0';
                checkedWhitespaceGet(is, "field value (string, number, object, array, true, false, or null)");
                checkedPeek(is, c, "field value (string, number, object, array, true, false, or null)");
                if ( c == 'n' )
                    readNull(is, field, dest.second, true);
                else if ( dest.second != nullptr )
                {
                    if constexpr ( Field::ContainsStrings ) // Quoted string
                        readString(is, field, *dest.second);
                    else if constexpr ( Field::ContainsBools ) // "true" or "false"
                        readBool(is, field, *dest.second, true);
                    else if constexpr ( Field::ContainsPrimitives ) // Number
                        readArrayNumber(is, field, dest.second);
                    else if constexpr ( Field::ContainsObjects ) // Object
                        readObject(is, field, dest.second);
                    else
                        throw Exception((std::string("Field \"") + field.name + "\" cannot be read from a JSON array").c_str());
                }
                else
                    throw Exception((std::string("Array field \"") + field.name + "\" element at index " + std::to_string(index)
                        + " cannot be read while pointing to nullptr!").c_str());
            }
            if constexpr ( Field::ContainsStrings ) // Quoted string
                readString(is, field, dest.second);
            else if constexpr ( Field::ContainsBools ) // "true" or "false"
                readBool(is, field, dest.second, true);
            else if constexpr ( Field::ContainsPrimitives ) // Number
                readArrayNumber(is, field, dest.second);
            else if constexpr ( Field::ContainsObjects ) // Object
                readObject(is, field, dest.second);
            else
                throw Exception((std::string("Field \"") + field.name + "\" cannot be read from a JSON array").c_str());
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::pair<L, R>* dest, size_t destSize)
        {
            if ( index > destSize )
                throw Exception(((std::string("Array max size of ") +
                    std::to_string(destSize) + " exceeded for field \"") + field.name + "\"").c_str());
            
            readPairToIterable(is, index, field, dest[index]);
        }

        template <typename Field, typename L, typename R, size_t N>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::array<std::pair<L, R>, N> & dest)
        {
            if ( index > N )
                throw Exception(((std::string("Array max size of ") + std::to_string(N)
                    + " exceeded for field \"") + field.name + "\"").c_str());
            
            readPairToIterable(is, index, field, dest[index]);
        }
        
        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::vector<std::pair<L, R>> & dest)
        {
            std::pair<L, R> t;
            readPairToIterable(is, index, field, t);
            dest.push_back(t);
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::deque<std::pair<L, R>> & dest)
        {
            std::pair<L, R> t;
            readPairToIterable(is, index, field, t);
            dest.push_back(T);
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::forward_list<std::pair<L, R>> & dest)
        {
            std::pair<L, R> t;
            readPairToIterable(is, index, field, t);
            dest.insert_after(--dest.end(), t);
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::list<std::pair<L, R>> & dest)
        {
            std::pair<L, R> t;
            readPairToIterable(is, index, field, t);
            dest.push_back(t);
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::map<L, R> & dest)
        {
            std::pair<L, R> t;
            readPairToIterable(is, index, field, t);
            dest.insert(t);
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::multimap<L, R> & dest)
        {
            std::pair<L, R> t;
            readPairToIterable(is, index, field, t);
            dest.insert(t);
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::unordered_map<L, R> & dest)
        {
            std::pair<L, R> t;
            readPairToIterable(is, index, field, t);
            dest.insert(t);
        }

        template <typename Field, typename L, typename R>
        static constexpr void readPairToIterable(std::istream & is, size_t index, Field field, std::unordered_multimap<L, R> & dest)
        {
            std::pair<L, R> t;
            readPairToIterable(is, index, field, t);
            dest.insert(t);
        }

        template <typename Field, typename DereferencedValue>
        static constexpr void readObjectToPairs(std::istream & is, Field & field, DereferencedValue & dest)
        {
            char c = '\0';
            checkedGet(is, c, '{', "object opening \"{\"");
            dest.clear();
            size_t index = 0;
            do
            {
                checkedWhitespaceGet(is, "quoted field name or object closing \"}\"");
                checkedPeek(is, c, "quoted field name or object closing \"}\"");
                if ( c == '}' ) // Object end
                    return;
                else if ( c != '\"' )
                    throw Exception("Expected quoted field name or object closing \"}\"");

                readPairToIterable(is, index, field, dest);
                index++;
            
                checkedGet(is, c, "\",\" or object closing \"}\"");
                if ( c == '}' )
                    return;
                else if ( c != ',' )
                    throw Exception("Expected \",\" or object closing \"}\"");
            } while ( true );
        }

        template <typename Field, typename Value>
        static constexpr void readObject(std::istream & is, Field & field, Value & value)
        {
            if constexpr ( Field::IsObject ) // Object
                is >> Input<Value>(value);
            else if constexpr ( Field::IsObjectPointer ) // Object pointer
            {
                if ( !field.IsNull(value) )
                    is >> Input<Value>::get(is, *value);
                else
                    throw Exception((std::string("Object field \"") + field.name + "\" cannot be read while pointing to nullptr!").c_str());
            }
            else if constexpr ( Field::ContainsPairs ) // STL map or pair container
            {
                if constexpr ( Field::IsPointer )
                {
                    if ( field.IsNull(value) )
                        throw Exception((std::string("Object field \"") + field.name + "\" cannot be read while pointing to nullptr!").c_str());
                    else
                        readObjectToPairs(is, field, *value);
                }
                else
                    readObjectToPairs(is, field, value);
            }
            else
                throw Exception((std::string("Unexpected JSON objected provided for field: \"") + field.name + "\"").c_str());
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToElement(std::istream & is, size_t index, Field field, T & value)
        {
            if constexpr ( Field::ContainsPointers )
            {
                char c = '\0';
                checkedWhitespaceGet(is, "field value (string, number, object, array, true, false, or null)");
                checkedPeek(is, c, "field value (string, number, object, array, true, false, or null)");
                if ( c == 'n' )
                    readNull(is, field, value, true);
                else if ( value != nullptr )
                {
                    if constexpr ( Field::ContainsStrings ) // Quoted string
                        readString(is, field, *value);
                    else if constexpr ( Field::ContainsBools ) // "true" or "false"
                        readBool(is, field, *value, true);
                    else if constexpr ( Field::ContainsPrimitives ) // Number
                        readArrayNumber(is, field, value);
                    else if constexpr ( Field::ContainsObjects ) // Object
                        readObject(is, field, value);
                    else
                        throw Exception((std::string("Field \"") + field.name + "\" cannot be read from a JSON array").c_str());
                }
                else
                    throw Exception((std::string("Array field \"") + field.name + "\" element at index " + std::to_string(index)
                        + " cannot be read while pointing to nullptr!").c_str());
            }
            if constexpr ( Field::ContainsStrings ) // Quoted string
                readString(is, field, value);
            else if constexpr ( Field::ContainsBools ) // "true" or "false"
                readBool(is, field, value, true);
            else if constexpr ( Field::ContainsPrimitives ) // Number
                readArrayNumber(is, field, value);
            else if constexpr ( Field::ContainsObjects ) // Object
                readObject(is, field, value);
            else
                throw Exception((std::string("Field \"") + field.name + "\" cannot be read from a JSON array").c_str());
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, T* dest, size_t destSize)
        {
            if ( index > destSize )
                throw Exception(((std::string("Array max size of ") +
                    std::to_string(destSize) + " exceeded for field \"") + field.name + "\"").c_str());

            readArrayValueToElement(is, index, field, dest[index]);
        }

        template <typename Field, typename T, size_t N>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::array<T, N> dest)
        {
            if ( index > N )
                throw Exception(((std::string("Array max size of ") + std::to_string(N)
                    + " exceeded for field \"") + field.name + "\"").c_str());
            
            readArrayValueToElement(is, index, field, dest[index]);
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::vector<T> dest)
        {
            T t;
            dest.push_back(t);
            readArrayValueToElement(is, index, field, dest.back());
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::deque<T> dest)
        {
            T t;
            dest.push_back(T);
            readArrayValueToElement(is, index, field, dest.back());
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::forward_list<T> dest)
        {
            T t;
            auto inserted = dest.insert_after(--dest.end(), t);
            readArrayValueToElement(is, index, field, inserted);
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::list<T> dest)
        {
            T t;
            dest.push_back(t);
            readArrayValueToElement(is, index, field, dest.back());
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::stack<T> dest)
        {
            T t;
            dest.push(t);
            readArrayValueToElement(is, index, field, dest.top());
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::queue<T> dest)
        {
            T t;
            dest.push(t);
            readArrayValueToElement(is, index, field, dest.back());
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::priority_queue<T> dest)
        {
            T t;
            readArrayValueToElement(is, index, field, t);
            dest.push(t);
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::set<T> dest)
        {
            T t;
            auto inserted = dest.insert(t);
            readArrayValueToElement(is, index, field, *inserted);
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::multiset<T> dest)
        {
            T t;
            auto inserted = dest.insert(t);
            readArrayValueToElement(is, index, field, *inserted);
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::unordered_set<T> dest)
        {
            T t;
            auto inserted = dest.insert(t);
            readArrayValueToElement(is, index, field, *inserted);
        }

        template <typename Field, typename T>
        static constexpr void readArrayValueToIterable(std::istream & is, size_t index, Field field, std::unordered_multiset<T> dest)
        {
            T t;
            auto inserted = dest.insert(t);
            readArrayValueToElement(is, index, field, *inserted);
        }

        template <typename Field, typename Value>
        void clearStlContainer(Field & field, Value & value)
        {
            if constexpr ( Field::IsStlIterable )
                value.clear();
            else if constexpr ( Field::IsStlAdaptor )
            {
                while ( !value.empty() )
                    value.pop();
            }
        }

        template <typename Field, typename Value>
        static constexpr void readArray(std::istream & is, Field & field, Value & value)
        {
            char c = '\0';
            checkedGet(is, c, '[', "array opening \"[\"");
            if constexpr ( Field::IsArray || Field::IsStlIterable || Field::IsStlAdaptor )
            {
                size_t i = 0;
                if constexpr ( Field::IsPointer )
                {
                    if ( field.IsNull(value) )
                        throw Exception((std::string("Object field \"") + field.name
                            + "\" cannot be read while pointing to nullptr!").c_str());
                    else if constexpr ( Field::IsStlIterable )
                        *value.clear();
                    else if constexpr ( Field::IsStlAdaptor )
                    {

                    }
                }
                else if constexpr ( Field::IsStlIterable )
                    value.clear();

                if constexpr ( Field::ContainsPairs )
                    throw Exception((std::string("Object field \"") + field.name + "\" expects pairs, not an array of values!").c_str());
                else
                {
                    for ( i=0; true; i++)
                    {
                        if constexpr ( Field::IsPointer )
                            readArrayValueToIterable(is, i, field, *value);
                        else if constexpr ( Field::IsArray )
                            readArrayValueToIterable(is, i, field, value, field.arraySize);
                        else
                            readArrayValueToIterable(is, i, field, value);

                        checkedGet(is, c, "\",\" or array closing \"]\"");
                        if ( c == ']' )
                            return;
                        else if ( c != ',' )
                            throw Exception("Expected \",\" or array closing \"]\"");
                    }
                }
            }
        }

        template <typename Field, typename Value>
        static constexpr void readTrue(std::istream & is, Field & field, Value & value, bool isArray = false)
        {
            readTrue(is, isArray);
            if constexpr ( field.IsBool )
                value = true;
            else
                ; // TODO: Object overriding instream?
        }

        template <typename Field, typename Value>
        static constexpr void readFalse(std::istream & is, Field & field, Value & value, bool isArray = false)
        {
            readFalse(is, isArray);
            if constexpr ( field.IsBool )
                value = false;
            else
                ; // TODO: Object overriding instream?
        }

        template <typename Field, typename Value>
        static constexpr void readBool(std::istream & is, Field & field, Value & value, bool isArray = false)
        {
            char c = '\0';
            checkedWhitespaceGet(is, "true or false");
            checkedPeek(is, c, "true or false");
            if ( c == 't' )
                readTrue(is, isArray);
            else if ( c == 'f' )
                readFalse(is, isArray);
            else
                throw UnexpectedFieldValue<Field>(field);
        }

        template <typename Field, typename Value>
        static constexpr void readNull(std::istream & is, Field & field, Value & value, bool isArray = false)
        {
            readNull(is, isArray);
            if constexpr ( field.IsPointer )
                value = nullptr;
            else
                ; // TODO: Object overriding instream?
        }

        static std::istream & get(std::istream & is, T & t)
        {
            char c = '\0';
            checkedGet(is, c, '{', "object opening \"{\"");
                
            using Index = const size_t &;
            using Class = typename T::Class;
            using Supers = typename T::Supers;
            size_t numFieldsUnparsed = Class::totalFields + Supers::totalSupers;
            do
            {
                checkedGet(is, c, "quoted field name or object closing \"}\"");
                if ( c == '}' ) // Object end
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

                checkedGet(is, c, '\"', "field name close quote");
                checkedGet(is, c, ':', "\":\"");

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
                
                checkedWhitespaceGet(is, "completion of field value");
                checkedPeek(is, c, "completion of field value");
                if ( jsonField != nullptr ) // Known field
                {
                    Class::FieldAt(t, jsonField->index, [&](auto & field, auto & value) {
                            
                        using Field = typename std::remove_reference<decltype(field)>::type;

                        if constexpr ( field.IsPointer )
                            ; // TODO: Pointers???

                        using SubType = typename std::remove_reference<decltype(value)>::type;
                        if constexpr ( Field::IsString && !Field::IsIterable ) // Must be valid quoted string
                            readString(is, field, value);
                        else // Unquoted
                        {
                            switch ( c )
                            {
                                case '\"': throw Exception("Invalid open quote starting non-string value");
                                case '-': case '0': case '1': case '2': // Number: primitive or object overriding instream, else error
                                case '3': case '4': case '5': case '6': case '7': case '8': case '9': readNumber(is, field, value); break;
                                case '{': readObject(is, field, value); break; // Json object or error
                                case '[': readArray(is, field, value); break; // Json array or error
                                case 't': readTrue(is, field, value); break; // "true": bool or object overriding instream, else error
                                case 'f': readFalse(is, field, value); break; // "false" bool or object overriding instream, else error
                                case 'n': readNull(is, field, value); break; // "null" pointer or object overriding instream, else error
                                default: throw Exception("Expected field value (string, number, object, array, true, false, or null)");
                            }
                        }
                    });
                    numFieldsUnparsed --;
                }
                else // UnknownField
                {
                    switch ( c )
                    {
                        case '\"': ignoreQuotedString(is); break; // String or error
                        case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                        case '6': case '7': case '8': case '9': ignoreNumber(is); break; // Number or error
                        case '{': ignoreObject(is); break; // JSON object or error
                        case '[': ignoreArray(is); break; // JSON array or error
                        case 't': readTrue(is); throw InvalidUnknownFieldValue(); break; // "true" or error
                        case 'f': readFalse(is); throw InvalidUnknownFieldValue(); break; // "false" or error
                        case 'n': readNull(is); throw InvalidUnknownFieldValue(); break; // "null" or error
                        default: throw InvalidUnknownFieldValue(); break;
                    }
                }

                checkedGet(is, c, "\",\" or object closing \"}\"");
                if ( c == '}' )
                    return is;
                else if ( c != ',' )
                    throw Exception((std::string("Expected \",\" or object closing \"}\"") + "found: " + c).c_str());

            } while ( true );
        }
    };

    template <typename T>
    constexpr Input<T> in(T & t)
    {
        return Input<T>(t);
    }
    
    template <typename T>
    std::istream & operator>>(std::istream & is, Json::Input<T> object)
    {
        return object.get(is, object.obj);
    }

};

#endif