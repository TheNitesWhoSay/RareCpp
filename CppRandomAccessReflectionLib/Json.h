#ifndef JSON_H
#define JSON_H
#include <ostream>
#include <sstream>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include "Reflect.h"

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
                else if constexpr ( Field::template HasAnnotation<Json::String> )
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

        template <size_t IndentLevel, const char* indent = twoSpaces>
        struct Indent { };

        template <size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const Indent<IndentLevel, indent>)
        {
            if constexpr ( IndentLevel > 0 )
            {
                for ( size_t i=0; i<IndentLevel; i++ )
                    os << indent;
            }

            return os;
        }

        template <bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
        struct ArrayPrefix { };

        template <bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const ArrayPrefix<ContainsPrimitives, IndentLevel, indent>)
        {
            if constexpr ( ContainsPrimitives )
                os << "[ ";
            else
                os << "[" << std::endl << Indent<IndentLevel, indent>();

            return os;
        }

        template <bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
        struct ArraySuffix { };

        template <bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const ArraySuffix<ContainsPrimitives, IndentLevel, indent>)
        {
            if constexpr ( ContainsPrimitives )
                os << " ]";
            else
                os << std::endl << Indent<IndentLevel, indent>() << "]";

            return os;
        }

        template <size_t IndentLevel, const char* indent = twoSpaces>
        struct ObjectPrefix { };

        template <size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const ObjectPrefix<IndentLevel, indent>)
        {
            os << "{";
            return os;
        }
        
        template <size_t IndentLevel, const char* indent = twoSpaces>
        struct ObjectSuffix { };

        template <size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const ObjectSuffix<IndentLevel, indent>)
        {
            os << std::endl << Indent<IndentLevel, indent>() << "}";
            return os;
        }
        
        template <bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
        struct NestedPrefix { };

        template <bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const NestedPrefix<IsArray, ContainsPrimitives, IndentLevel, indent>)
        {
            if constexpr ( IsArray )
                os << ArrayPrefix<ContainsPrimitives, IndentLevel, indent>();
            else
                os << ObjectPrefix<IndentLevel, indent>();

            return os;
        }

        template <bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
        struct NestedSuffix { };

        template <bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const NestedSuffix<IsArray, ContainsPrimitives, IndentLevel, indent>)
        {
            if constexpr ( IsArray )
                os << ArraySuffix<ContainsPrimitives, IndentLevel, indent>();
            else
                os << ObjectSuffix<IndentLevel, indent>();

            return os;
        }
        
        template <bool IsFirst, size_t IndentLevel, const char* indent = twoSpaces>
        struct FieldPrefix { };

        template <bool IsFirst, size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const FieldPrefix<IsFirst, IndentLevel, indent>)
        {
            if constexpr ( IsFirst )
                os << std::endl << Indent<IndentLevel, indent>();
            else 
                os << "," << std::endl << Indent<IndentLevel, indent>();

            return os;
        }

        struct FieldNameValueSeparator { };

        std::ostream & operator<<(std::ostream & os, const FieldNameValueSeparator)
        {
            os << ": ";
            return os;
        }
                
        template <bool IsJsonField, bool NestedSeparator, size_t IndentLevel, const char* indent = twoSpaces>
        struct Separator
        {
            bool isFirst;

            Separator() : isFirst(false) {}
            Separator(bool isFirst) : isFirst(isFirst) {}
        };

        template <bool IsJsonField, bool NestedSeparator, size_t IndentLevel, const char* indent = twoSpaces>
        std::ostream & operator<<(std::ostream & os, const Separator<IsJsonField, NestedSeparator, IndentLevel, indent> separator)
        {
            if constexpr ( IsJsonField )
            {
                if ( separator.isFirst )
                    os << std::endl << Indent<IndentLevel, indent>();
                else 
                    os << "," << std::endl << Indent<IndentLevel, indent>();
            }
            else if ( !separator.isFirst )
            {
                if constexpr ( NestedSeparator )
                    os << "," << std::endl << Indent<IndentLevel, indent>();
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
            return std::string("__") + simplifyTypeStr(ExtendedTypeSupport::TypeToStr<T>());
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

                if constexpr ( Class::TotalFields > 0 )
                {
                    for ( size_t fieldIndex = 0; fieldIndex < Class::TotalFields; fieldIndex++ )
                    {
                        inserted.first->second.insert(std::pair<size_t, JsonField>(
                            strHash(Class::Fields[fieldIndex].name), JsonField(fieldIndex, JsonField::Type::Regular, Class::Fields[fieldIndex].name)));
                    }
                }

                if constexpr ( Supers::TotalSupers > 0 )
                {
                    for ( size_t superIndex = 0; superIndex < Supers::TotalSupers; superIndex++ )
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

        template <typename T>
        static JsonField* getJsonField(T & t, std::string & fieldName)
        {
            std::multimap<size_t, JsonField> & fieldNameToJsonField = getClassFieldCache(t);
            size_t fieldNameHash = strHash(fieldName);
            auto fieldHashMatches = fieldNameToJsonField.equal_range(fieldNameHash);
            for ( auto it = fieldHashMatches.first; it != fieldHashMatches.second; ++it )
            {
                if ( it->second.name.compare(fieldName) == 0 )
                    return &it->second;
            }
            return nullptr;
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

    template <typename T, size_t IndentLevel = 0, const char* indent = twoSpaces>
    class Output
    {
    public:
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
        
        template <size_t TotalParentIterables, typename Field, typename Element>
        static constexpr void putValue(std::ostream & os, const Element & element)
        {
            if constexpr ( ExtendedTypeSupport::is_pointable<Element>::value )
            {
                if ( element == nullptr )
                    os << "null";
                else if constexpr ( ExtendedTypeSupport::is_iterable<Element>::value )
                    putIterable<TotalParentIterables, Field, Element>(os, *element);
                else if constexpr ( Field::template HasAnnotation<Reflect::Reflected> )
                    Output<typename ExtendedTypeSupport::remove_pointer<Element>::type, IndentLevel+TotalParentIterables+1>::put(os, *element);
                else if constexpr ( Field::template HasAnnotation<Json::String> )
                    putString(os, *element);
                else
                    os << *element;
            }
            else if constexpr ( ExtendedTypeSupport::is_iterable<Element>::value )
                putIterable<TotalParentIterables, Field, Element>(os, element);
            else if constexpr ( Field::template HasAnnotation<Reflect::Reflected> )
                Output<Element, IndentLevel+TotalParentIterables+1>::put(os, element);
            else if constexpr ( Field::template HasAnnotation<Json::String> )
                putString(os, element);
            else
                os << element;
        }

        template <size_t TotalParentIterables, typename Field, typename Element, typename Key>
        static constexpr void putValue(std::ostream & os, const std::pair<Key, Element> & pair)
        {
            putString(os, pair.first);
            os << FieldNameValueSeparator();
            putValue<TotalParentIterables, Field>(os, pair.second);
        }
        
        template <size_t TotalParentIterables, typename Field, typename Iterable>
        static constexpr void putIterable(std::ostream & os, const Iterable & iterable)
        {
            using Element = typename ExtendedTypeSupport::element_type<Iterable>::type;
            constexpr bool ContainsIterables = ExtendedTypeSupport::is_iterable<typename ExtendedTypeSupport::pair_rhs<Element>::type>::value;
            constexpr bool ContainsPrimitives = !Field::template HasAnnotation<Reflect::Reflected> && !ContainsIterables;
            constexpr bool ContainsPairs = ExtendedTypeSupport::is_pair<Element>::value;
            
            size_t i=0;
            os << NestedPrefix<!ContainsPairs, ContainsPrimitives, IndentLevel+TotalParentIterables+2, indent>();
            if constexpr ( ExtendedTypeSupport::is_stl_iterable<Iterable>::value )
            {
                for ( auto & element : iterable )
                {
                    os << Separator<ContainsPairs, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(0 == i++);
                    putValue<TotalParentIterables+1, Field>(os, element);
                }
            }
            else if constexpr ( ExtendedTypeSupport::is_adaptor<Iterable>::value )
            {
                const auto & sequenceContainer = ExtendedTypeSupport::get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                {
                    os << Separator<ContainsPairs, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(0 == i++);
                    putValue<TotalParentIterables+1, Field>(os, *it);
                }
            }
            else if constexpr ( std::is_array<Iterable>::value && std::extent<Field::Type>::value > 0 )
            {
                for ( ; i<std::extent<Field::Type>::value; i++ )
                {
                    os << Separator<ContainsPairs, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(0 == i);
                    putValue<TotalParentIterables+1, Field>(os, iterable[i]);
                }
            }
            os << NestedSuffix<!ContainsPairs, ContainsPrimitives, IndentLevel+TotalParentIterables+1, indent>();
        }
        
        static constexpr std::ostream & put(std::ostream & os, const T & obj)
        {
            os << ObjectPrefix<IndentLevel, indent>();

            T::Class::ForEachField(obj, [&](auto & field, auto & value)
            {
                using Field = typename std::remove_reference<decltype(field)>::type;

                os << FieldPrefix<Field::Index == 0, IndentLevel+1, indent>();
                putString(os, field.name);
                os << FieldNameValueSeparator();
                putValue<0, Field>(os, value);
            });

            os << ObjectSuffix<IndentLevel, indent>();
            return os;
        }
    };
    
    template <typename T>
    class Input
    {
    public:
        Input(T & obj) : obj(obj) {}

        T & obj;

        static constexpr const char unicodeEscapeSequence[] = "\\uHHHH (where H's are hex characters)";

        class Checked
        {
        public:
            static constexpr inline void peek(std::istream & is, char & c, const char* expectedDescription)
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
            
            static constexpr inline bool tryGet(std::istream & is, int character, const char* expectedDescription)
            {
                is >> std::ws;
                if ( is.good() )
                {
                    int c = is.peek();
                    if ( is.good() )
                    {
                        if ( c == character )
                        {
                            is.ignore();
                            if ( is.good() )
                                return true;
                        }
                        else
                            return false;
                    }
                }
                if ( is.eof() )
                    throw UnexpectedInputEnd(expectedDescription);
                else
                    throw StreamReadFail();
            }
            
            template <bool usePrimary>
            static constexpr inline bool tryGet(std::istream & is, int character, int secondaryCharacter,
                const char* expectedDescription, const char* secondaryDescription)
            {
                if ( usePrimary )
                    return tryGet(is, character, expectedDescription);
                else
                    return tryGet(is, secondaryCharacter, secondaryDescription);
            }

            static constexpr inline bool get(std::istream & is, char trueChar, char falseChar, const char* expectedDescription)
            {
                char c = '\0';
                is >> c;
                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd(expectedDescription);
                    else
                        throw StreamReadFail();
                }
                else if ( c == trueChar )
                    return true;
                else if ( c == falseChar )
                    return false;
                else
                    throw Exception((std::string("Expected: ") + expectedDescription).c_str());
            }

            static constexpr inline void get(std::istream & is, char & c, const char* expectedDescription)
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
            
            template <bool usePrimary>
            static constexpr inline char get(std::istream & is, char trueChar, char falseChar, char secondaryFalseChar,
                const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    return get(is, trueChar, falseChar, expectedDescription);
                else
                    return get(is, trueChar, secondaryFalseChar, secondaryDescription);
            }

            template <bool usePrimary>
            static constexpr inline void get(std::istream & is, char & c, const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    get(is, c, expectedDescription);
                else
                    get(is, c, secondaryDescription);
            }

            template <bool usePrimary>
            static constexpr inline void get(std::istream & is, char & c, int expectedChar, int secondaryChar,
                const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    get(is, c, expectedChar, expectedDescription);
                else
                    get(is, c, secondaryChar, secondaryDescription);
            }

            static constexpr inline bool unget(std::istream & is, char ungetting)
            {
                is.unget();
                if ( !is.good() )
                    throw StreamUngetFail(ungetting);

                return true;
            }

            static constexpr inline void escapeSequenceGet(std::istream & is, char & c, const char* hexEscapeSequence)
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

            static constexpr inline void consumeWhitespace(std::istream & is, const char* expectedDescription)
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

            template <bool usePrimary>
            static constexpr inline void consumeWhitespace(std::istream & is, const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    consumeWhitespace(is, expectedDescription);
                else
                    consumeWhitespace(is, secondaryDescription);
            }
        };
        
        template <typename Iterable>
        static constexpr void clear(Iterable & iterable)
        {
            if constexpr ( ExtendedTypeSupport::has_clear<Iterable>::value )
                iterable.clear();
            else if constexpr ( ExtendedTypeSupport::is_adaptor<Iterable>::value )
            {
                while ( !iterable.empty() )
                    iterable.pop();
            }
        }

        template <typename Iterable, typename Element>
        static constexpr void append(Iterable & iterable, Element & element)
        {
            if constexpr ( ExtendedTypeSupport::has_push_back<Iterable>::value )
                iterable.push_back(element);
            else if constexpr ( ExtendedTypeSupport::is_forward_list<Iterable>::value )
                iterable.insert_after(--iterable.end(), element);
            else if constexpr ( ExtendedTypeSupport::has_push<Iterable>::value )
                iterable.push(element);
            else if constexpr ( ExtendedTypeSupport::has_insert<Iterable>::value )
                iterable.insert(element);
        }

        template <bool InArray>
        static constexpr void readTrue(std::istream & is, char & c)
        {
            Checked::consumeWhitespace(is, "completion of field value");
            int expectation[] = { 't', 'r', 'u', 'e' };
            for ( size_t i=0; i<4; i++ )
                Checked::get(is, c, expectation[i], "completion of field value");

            Checked::consumeWhitespace(is, "\",\" or \"}\"");
            Checked::peek(is, c, "\",\" or \"}\"");
            if ( InArray && c != ',' && c != ']' )
                throw Exception("Expected: \",\" or \"]\"");
            else if ( !InArray && c != ',' && c != '}' )
                throw Exception("Expected: \",\" or \"}\"");
        }

        template <bool InArray>
        static constexpr void readFalse(std::istream & is, char & c)
        {
            Checked::consumeWhitespace(is, "completion of field value");
            int expectation[] = { 'f', 'a', 'l', 's', 'e' };
            for ( size_t i=0; i<5; i++ )
                Checked::get(is, c, expectation[i], "completion of field value");

            Checked::consumeWhitespace(is, "\",\" or \"}\"");
            Checked::peek(is, c, "\",\" or \"}\"");
            if ( InArray && c != ',' && c != ']' )
                throw Exception("Expected: \",\" or \"]\"");
            else if ( !InArray && c != ',' && c != '}' )
                throw Exception("Expected: \",\" or \"}\"");
        }

        template <bool InArray, typename Value>
        static constexpr void readBool(std::istream & is, char & c, Value & value)
        {
            Checked::consumeWhitespace(is, "true or false");
            Checked::peek(is, c, "true or false");
            if ( c == 't' )
            {
                readTrue<InArray>(is, c);
                value = true;
            }
            else if ( c == 'f' )
            {
                readFalse<InArray>(is, c);
                value = false;
            }
            else
                throw Exception("Expected: \"true\" or \"false\"");
        }

        template <bool InArray>
        static constexpr void readNull(std::istream & is, char & c)
        {
            Checked::consumeWhitespace(is, "completion of field value");
            int expectation[] = { 'n', 'u', 'l', 'l' };
            for ( size_t i=0; i<4; i++ )
                Checked::get(is, c, expectation[i], "completion of field value");

            Checked::consumeWhitespace(is, "\",\" or \"}\"");
            Checked::peek(is, c, "\",\" or \"}\"");
            if ( InArray && c != ',' && c != ']' )
                throw Exception("Expected: \",\" or \"]\"");
            else if ( !InArray && c != ',' && c != '}' )
                throw Exception("Expected: \",\" or \"}\"");
        }

        template <bool InArray>
        static constexpr bool tryReadNull(std::istream & is, char & c)
        {
            Checked::consumeWhitespace(is, "null or field value");
            Checked::peek(is, c, "null or field value");
            if ( c == 'n' )
            {
                int expectation[] = { 'n', 'u', 'l', 'l' };
                for ( size_t i=0; i<4; i++ )
                    Checked::get(is, c, expectation[i], "completion of \"null\"");

                Checked::consumeWhitespace(is, "\",\" or \"}\"");
                Checked::peek(is, c, "\",\" or \"}\"");
                if ( InArray && c != ',' && c != ']' )
                    throw Exception("Expected: \",\" or \"]\"");
                else if ( !InArray && c != ',' && c != '}' )
                    throw Exception("Expected: \",\" or \"}\"");
                
                return true;
            }
            return false;
        }

        template <bool InArray>
        static constexpr void ignoreNumber(std::istream & is, char & c)
        {
            bool decimal = false;
            bool finished = false;
            Checked::get(is, c, "\"-\" or [0-9]");
            switch ( c )
            {
                case '-': case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9': break;
                default: throw InvalidNumericCharacter(c, "\"-\" or [0-9]");
            }
            constexpr char terminator = InArray ? ']' : '}';
            do
            {
                Checked::get<InArray>(is, c, "\",\" or \"]\"", "\",\" or \"}\"");

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
                    case ',': Checked::unget(is, ','); break;
                    case terminator: Checked::unget(is, terminator); break;
                    case ' ': case '\t': case '\n': case '\r':
                        Checked::consumeWhitespace<InArray>(is, decimal ? "[0-9], \",\", or \"]\"" : "\".\", [0-9], \",\", or \"]\"",
                            decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\"");
                        Checked::get<InArray>(is, c, decimal ? "[0-9], \",\", or \"]\"" : "\".\", [0-9], \",\", or \"]\"",
                            decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\"");

                        if ( InArray && c != ',' && c != ']' )
                            throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"]\"" : "\".\", [0-9], \",\", or \"]\""));
                        else if ( c != ',' && c != '}' )
                            throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\""));
                        else
                            Checked::unget(is, '\"');
                        break;
                    default:
                        throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\""));
                        break;
                }
            }
            while ( c != ',' && c != terminator );
        }

        static constexpr void ignoreString(std::istream & is, char & c)
        {
            Checked::get(is, c, '\"', "string value open quote");
            do
            {
                Checked::get(is, c, "string value close quote");
                switch ( c )
                {
                    case '\\': // Escape sequence
                        Checked::get(is, c, "completion of string escape sequence");
                        switch ( c )
                        {
                            case '\"': case '\\': case '/': case 'b': case 'f': case 'n': case 'r': case 't': break;
                            case 'u':
                            {
                                char hexEscapeSequence[6] = { 'u', '\0', '\0', '\0', '\0', '\0' };
                                for ( size_t i=1; i<5; i++ )
                                {
                                    Checked::escapeSequenceGet(is, c, hexEscapeSequence);
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
                    case '\"': break; // Closing quote
                    default: break;
                }
            } while ( c != '\"' );
        }

        static constexpr void getString(std::istream & is, char & c, std::stringstream & ss)
        {
            Checked::get(is, c, '\"', "string value open quote");
            do
            {
                Checked::get(is, c, "string value close quote");
                switch ( c )
                {
                    case '\\': // Escape sequence
                        Checked::get(is, c, "completion of string escape sequence");
                        switch ( c )
                        {
                            case '\"': ss.put('\"'); break;
                            case '\\': ss.put('\\'); break;
                            case '/': ss.put('/'); break;
                            case 'b': ss.put('\b'); break;
                            case 'f': ss.put('\f'); break;
                            case 'n': ss.put('\n'); break;
                            case 'r': ss.put('\r'); break;
                            case 't': ss.put('\t'); break;
                            case 'u':
                            {
                                char hexEscapeSequence[6] = { 'u', '\0', '\0', '\0', '\0', '\0' };
                                for ( size_t i=1; i<5; i++ )
                                {
                                    Checked::escapeSequenceGet(is, c, hexEscapeSequence);
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
                                    ss.put(highCharacter);

                                ss.put(lowCharacter);
                            }
                            break;
                        }
                        break;
                    case '\n': throw UnexpectedLineEnding("\\n");
                    case '\r': throw UnexpectedLineEnding("\\r");
                    case '\"': break; // Closing quote
                    default: ss.put(c); break;
                }
            } while ( c != '\"' );
        }

        static constexpr void getString(std::istream & is, char & c, std::string & str)
        {
            std::stringstream ss;
            getString(is, c, ss);
            str = ss.str();
        }

        template <typename T>
        static constexpr void getString(std::istream & is, char & c, T & t)
        {
            std::stringstream ss;
            getString(is, c, ss);
            ss >> t;
        }
        
        template <bool InArray>
        static constexpr void ignoreValue(std::istream & is, char & c)
        {
            Checked::consumeWhitespace(is, "completion of field value");
            Checked::peek(is, c, "completion of field value");
            switch ( c )
            {
                case '\"': ignoreString(is, c); break; // String or error
                case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                case '6': case '7': case '8': case '9': ignoreNumber<InArray>(is, c); break; // Number or error
                case '{': ignoreIterable<false>(is, c); break; // JSON object or error
                case '[': ignoreIterable<true>(is, c); break; // JSON array or error
                case 't': readTrue<InArray>(is, c); break; // "true" or error
                case 'f': readFalse<InArray>(is, c); break; // "false" or error
                case 'n': readNull<InArray>(is, c); break; // "null" or error
                default: throw InvalidUnknownFieldValue(); break;
            }
        }

        template <bool InArray, typename Field, typename Element>
        static constexpr void getValue(std::istream & is, char & c, Element & element)
        {
            if constexpr ( ExtendedTypeSupport::is_pointable<Element>::value )
            {
                if ( element == nullptr ) // If element pointer is nullptr the only valid value is "null"
                    readNull<InArray>(is, c);
                else if ( tryReadNull<InArray>(is, c) ) // If element pointer is not nullptr, "null" is a possible value
                    element = nullptr;
                else if constexpr ( ExtendedTypeSupport::is_iterable<Element>::value )
                    getIterable<Field, Element>(is, c, *element);
                else if constexpr ( Field::template HasAnnotation<Reflect::Reflected> )
                    Input<typename remove_pointer<Element>::type>::get(is, c, *element);
                else if constexpr ( Field::template HasAnnotation<Json::String> )
                    getString(is, *element);
                else if constexpr ( ExtendedTypeSupport::is_bool<typename ExtendedTypeSupport::remove_pointer<Element>::type>::value )
                    readBool<InArray>(is, c, *element);
                else
                    is >> *element;
            }
            else if constexpr ( ExtendedTypeSupport::is_iterable<Element>::value )
                getIterable<Field, Element>(is, c, element);
            else if constexpr ( Field::template HasAnnotation<Reflect::Reflected> )
                Input<Element>::get(is, c, element);
            else if constexpr ( Field::template HasAnnotation<Json::String> )
                getString(is, c, element);
            else if constexpr ( ExtendedTypeSupport::is_bool<Element>::value )
                readBool<InArray>(is, c, element);
            else
                is >> element;
        }

        template <bool InArray, typename Field, typename Key, typename Element>
        static constexpr void getValue(std::istream & is, char & c, std::pair<Key, Element> & pair)
        {
            getString(is, c, pair.first);
            Checked::get(is, c, ':', "field name-value separator \":\"");
            getValue<InArray, Field, Element>(is, c, pair.second);
        }
        
        template <bool IsArray>
        static constexpr void ignoreIterable(std::istream & is, char & c)
        {
            Checked::get<IsArray>(is, c, '[', '{', "array opening \"[\"", "object opening \"{\"");
            if ( !Checked::tryGet<IsArray>(is, ']', '}', "array closing \"]\" or array element", "object closing \"}\" or field name opening \"") )
            {
                do
                {
                    if ( !IsArray )
                    {
                        ignoreString(is, c);
                        Checked::get(is, c, ':', "field name-value separator \":\"");
                    }
                    ignoreValue<IsArray>(is, c);
                }
                while ( Checked::get<IsArray>(is, ',', ']', '}', "\",\" or array closing \"]\"", "\",\" or object closing \"}\"") );
            }
        }

        template <typename Field, typename Iterable>
        static constexpr void getIterable(std::istream & is, char & c, Iterable & iterable)
        {
            using Element = typename ExtendedTypeSupport::element_type<Iterable>::type;
            constexpr bool ContainsPairs = ExtendedTypeSupport::is_pair<Element>::value;
            
            Checked::get<ContainsPairs>(is, c, '{', '[', "object opening \"{\"", "array opening \"[\"");
            if ( !Checked::tryGet<ContainsPairs>(is, '}', ']', "object closing \"}\" or field name opening \"", "array closing \"]\" or array element") )
            {
                clear(iterable);
                size_t i=0;
                do
                {
                    if constexpr ( ExtendedTypeSupport::is_static_array<Iterable>::value )
                    {
                        if ( i >= ExtendedTypeSupport::static_array_size<Iterable>::value )
                            throw Exception("Array size exceeded!");
                        else
                            getValue<!ContainsPairs, Field>(is, c, iterable[i++]);
                    }
                    else // Appendable STL container
                    {
                        typename ExtendedTypeSupport::element_type<Iterable>::type value;
                        getValue<!ContainsPairs, Field>(is, c, value);
                        append<Iterable, typename ExtendedTypeSupport::element_type<Iterable>::type>(iterable, value);
                    }
                }
                while ( Checked::get<ContainsPairs>(is, ',', '}', ']', "\",\" or object closing \"}\"", "\",\" or array closing \"]\"") );
            }
        }

        static std::istream & get(std::istream & is, char & c, T & t)
        {
            using Class = typename T::Class;

            Checked::get(is, c, '{', "object opening \"{\"");
            if ( !Checked::tryGet(is, '}', "object closing \"}\" or field name opening \"") )
            {
                do
                {
                    std::string fieldName;
                    try {
                        getString(is, c, fieldName);
                    } catch ( UnexpectedLineEnding & e) {
                        throw Exception((std::string("Expected field name close quote, found line ending (\"") + e.what() + "\")").c_str());
                    }
                    Checked::get(is, c, ':', "field name-value separator \":\"");

                    JsonField* jsonField = getJsonField(t, fieldName);
                    if ( jsonField != nullptr ) // Known field
                    {
                        Class::FieldAt(t, jsonField->index, [&](auto & field, auto & value) {

                            using Field = typename std::remove_reference<decltype(field)>::type;

                            getValue<false, Field>(is, c, value);
                        });
                    }
                    else // Unknown field
                        ignoreValue<false>(is, c);
                }
                while ( Checked::get(is, ',', '}', "\",\" or object closing \"}\"") );
            }
            return is;
        }
    };

    template <typename T, size_t IndentLevel = 0, const char* indent = twoSpaces>
    constexpr Output<T> out(const T & t)
    {
        return Output<T, IndentLevel, indent>(t);
    }
    
    template <typename T, size_t IndentLevel = 0, const char* indent = twoSpaces>
    std::ostream & operator<<(std::ostream & os, const Json::Output<T, IndentLevel> object)
    {
        return object.put(os, object.obj);
    }

    template <typename T>
    constexpr Input<T> in(T & t)
    {
        return Input<T>(t);
    }
    
    template <typename T>
    std::istream & operator>>(std::istream & is, Json::Input<T> object)
    {
        char c = '\0';
        return object.get(is, c, object.obj);
    }

};

#endif