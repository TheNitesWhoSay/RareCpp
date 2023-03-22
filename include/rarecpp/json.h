// MIT License, Copyright (c) 2019-2023 Justin F https://github.com/TheNitesWhoSay/RareCpp/blob/master/LICENSE
#ifndef JSON_H
#define JSON_H
#ifndef REFLECT_H // This check, while normally redundant to have here, helps the file work on godbolt
#include "reflect.h"
#endif
#ifdef USE_BUFFERED_STREAMS
#include "string_buffer.h"
#endif
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <istream>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

namespace Json
{
#ifdef USE_BUFFERED_STREAMS
    using namespace RareBufferedStream;
    using OutStreamType = StringBuffer;
    inline constexpr RareBufferedStream::EndL endl = {};
#else
    using OutStreamType = std::ostream;
    struct endl_type {};
    inline constexpr endl_type endl;
    inline std::ostream & operator<<(std::ostream & os, const endl_type) { return os << std::endl; }
#endif

    using RareTs::Reflect;
    
    inline namespace Annotations
    {
        // Member annotation telling JSON to explicitly use the numeric value of an enum
        struct EnumIntType {};
        inline constexpr EnumIntType EnumInt{};

        // Member annotation telling JSON to skip a field during input and output
        struct IgnoreType {};
        inline constexpr IgnoreType Ignore{};

        // Member annotation telling JSON to treat a std::string field as though it wasn't a string
        struct UnstringType {};
        inline constexpr UnstringType Unstring{};

        struct StringifyType {};
        inline constexpr StringifyType Stringify{};

        // Member or Super annotation telling JSON to use a different name for the field
        struct Name
        {
             std::string_view value;
        };

        template <typename ...Ts> using OpNotes = std::tuple<Ts...>;
    }
    
    inline namespace Shared
    {
        enum class Statics
        {
            Excluded = 0,
            Included = 1,
            Only = 2
        };

        template <Statics statics> struct StaticType : std::integral_constant<Statics, statics> {};

        template <bool IsStatic, typename Statics>
        struct matches_statics
        {
            static constexpr bool value =
                Statics::value == Shared::Statics::Included ||
                (Statics::value == Shared::Statics::Excluded && !IsStatic) ||
                (Statics::value == Shared::Statics::Only && IsStatic);
        };

        template <typename Member, class Statics, typename = RareTs::enable_if_member_t<Member>>
        struct IsUnignoredDataMatchingStatics : std::bool_constant<Member::isData && !Member::template hasNote<Json::IgnoreType>() &&
            matches_statics<Member::isStatic, Statics>::value> {};

        template <Statics statics, typename Object, bool IncludeSupers = true>
        constexpr bool hasFields()
        {
            constexpr bool hasFieldsMatchingStatics =
                Reflect<Object>::Members::template filteredCount<IsUnignoredDataMatchingStatics, StaticType<statics>>() > 0;

            if constexpr ( hasFieldsMatchingStatics )
                return true;
            else if constexpr ( Reflect<Object>::Supers::total == 0 || !IncludeSupers )
                return false;
            else
            {
                bool hasMembers = false;
                Reflect<Object>::Supers::forEach([&](auto superInfo) {
                    using Super = typename decltype(superInfo)::type;
                    if constexpr ( hasFields<statics, Super>() )
                        hasMembers = true;
                });
                return hasMembers;
            }
        }

        template <Statics statics, typename Object>
        constexpr size_t firstIndex()
        {
            constexpr size_t totalMembers = Reflect<Object>::Members::total;
            size_t firstIndex = totalMembers;
            if constexpr ( totalMembers > 0 ) {
            Reflect<Object>::Members::template forEach<IsUnignoredDataMatchingStatics, StaticType<statics>>([&](auto & field) {
                if ( firstIndex == totalMembers )
                    firstIndex = std::remove_reference_t<decltype(field)>::index;
            });
            }
            return firstIndex;
        }

        template <Statics statics, typename Object, size_t Index = 0>
        constexpr size_t firstSuperIndex()
        {
            if constexpr ( Index < Reflect<Object>::Supers::total )
            {
                using SuperInfo = typename Reflect<Object>::Supers::template SuperInfo<Index>;
                if constexpr ( hasFields<statics, typename SuperInfo::type>() )
                    return Index;
                else
                    return firstSuperIndex<statics, Object, Index+1>();
            }
            else
                return Reflect<Object>::Supers::total;
        }

        inline constexpr size_t noMemberIndex = std::numeric_limits<size_t>::max();

        template <typename T = void> struct MockMember {
            using type = T;
            static constexpr size_t index = 0;
            using Notes = RareTs::NoNote;
            template <typename Annotation> static constexpr bool hasNote() { return false; }
        };
        using NoMember = MockMember<>;

        struct Context
        {
            virtual ~Context() = default;
        };
        inline Context defaultContext;

        class Exception : public std::exception
        {
        public:
            Exception(const char* what) : message(what) {}

            virtual const char* what() const noexcept { return message.c_str(); }

        private:
            std::string message;
        };

        class NullUnassignable : public Exception
        {
        public:
            NullUnassignable() : Exception("Null cannot be assigned to a non-pointable value!") {}
        };

        inline namespace TypeNames
        {
            inline std::string simplifyTypeStr(const std::string & superTypeStr)
            {
                std::string rawSimpleTypeStr = superTypeStr;
                if ( rawSimpleTypeStr.find("struct ", 0) != std::string::npos )
                    rawSimpleTypeStr.erase(0, strlen("struct "));
                if ( rawSimpleTypeStr.find("class ", 0) != std::string::npos )
                    rawSimpleTypeStr.erase(0, strlen("class "));

                std::string simpleTypeStr;
                for ( size_t i=0; i<rawSimpleTypeStr.size(); i++ ) {
                    if ( rawSimpleTypeStr[i] != ' ' )
                        simpleTypeStr += rawSimpleTypeStr[i];
                    else if ( ++i < rawSimpleTypeStr.size() ) /* Remove space and upper-case the letter following the space */
                        simpleTypeStr += static_cast<char>(std::toupper(rawSimpleTypeStr[i]));
                }
                return simpleTypeStr;
            }

            template <typename T>
            inline std::string superTypeToJsonFieldName()
            {
                return std::string("__") + simplifyTypeStr(std::string(RareTs::toStr<T>()));
            }

            inline std::string fieldClusterToJsonFieldName()
            {
                return std::string("____fieldCluster");
            }
        }
    }

    inline namespace Generic
    {
        struct JsonField
        {
            enum class Type
            {
                Regular,
                SuperClass,
                FieldCluster
            };
        
            JsonField() : index(0), type(Type::Regular), name("") {}
            JsonField(size_t index, Type fieldType, const std::string & name) : index(index), type(fieldType), name(name) {}
        
            size_t index;
            Type type;
            std::string name;
        };
        
        class Value {
        public:

            enum class Type
            {
                None,
                Null,
                Boolean,
                Number,
                String,
                Object,
                Array,
                NullArray,
                BoolArray,
                NumberArray,
                StringArray,
                ObjectArray,
                MixedArray,
                FieldCluster
            };
            
            class TypeMismatch : public Exception
            {
            public:
                static std::string getTypeStr(Value::Type type)
                {
                    switch ( type )
                    {
                        case Value::Type::None: return "None";
                        case Value::Type::Null: return "Null";
                        case Value::Type::Boolean: return "Boolean";
                        case Value::Type::Number: return "Number";
                        case Value::Type::String: return "String";
                        case Value::Type::Object: return "Object";
                        case Value::Type::Array: return "Array";
                        case Value::Type::NullArray: return "NullArray";
                        case Value::Type::BoolArray: return "BoolArray";
                        case Value::Type::NumberArray: return "NumberArray";
                        case Value::Type::StringArray: return "StringArray";
                        case Value::Type::ObjectArray: return "ObjectArray";
                        case Value::Type::MixedArray: return "MixedArray";
                        case Value::Type::FieldCluster: return "FieldCluster";
                        default: return "Unknown";
                    }
                }

                TypeMismatch(Type valueType, Type functionType, const std::string & functionName)
                    : Exception((std::string("Cannot call ") + functionName + "() on a Json::" + getTypeStr(valueType) + " type!").c_str()),
                    valueType(valueType), rValueType(Type::None), functionType(functionType) {}

                TypeMismatch(Value::Type lhsType, Value::Type rhsType)
                    : Exception((std::string("Cannot assign a Json::") + getTypeStr(rhsType) + " value to a Json::" + getTypeStr(lhsType) + " type!").c_str()),
                    valueType(lhsType), rValueType(rhsType), functionType(Value::Type::None) {}

                Value::Type valueType;
                Value::Type rValueType;
                Value::Type functionType;
            };

            class Assigner
            {
            public:
                Assigner(std::unique_ptr<Value> && allocatedValue) : allocatedValue(std::move(allocatedValue)) {}

                template <typename Value, typename... Args>
                static std::shared_ptr<Assigner> make(Args &&... args) { return std::make_shared<Assigner>(std::move(std::make_unique<Value>(args...))); }
                static std::shared_ptr<Assigner> make(std::nullptr_t) { return std::make_shared<Assigner>(nullptr); }
                
                Value* get() { return allocatedValue.get(); }

                // Assigns the stored Json::Generic::Value to the value passed to the method, then discards the stored Json::Generic::Value
                template <typename T>
                void into(T & value)
                {
                    if constexpr ( RareTs::is_pointable_v<T> )
                    {
                        if ( allocatedValue != nullptr )
                        {
                            using Dereferenced = RareTs::remove_pointer_t<T>;
                            if ( value == nullptr )
                            {
                                Dereferenced* casted = dynamic_cast<Dereferenced*>(allocatedValue.get());
                                if ( casted == nullptr )
                                    throw TypeMismatch(Value::Type::None, allocatedValue->type());
                                else if constexpr ( std::is_same_v<std::shared_ptr<Dereferenced>, T> )
                                    value = std::shared_ptr<Dereferenced>(casted);
                                else if constexpr ( std::is_same_v<std::unique_ptr<Dereferenced>, T> )
                                    value = std::unique_ptr<Dereferenced>(casted);
                                else
                                    throw Exception("Cannot assign a non-null value to a null pointer unless the type is std::shared_ptr or std::unique_ptr");
                            }
                            else if ( value->type() != allocatedValue->type() ) // value != nullptr
                            {
                                Dereferenced* casted = dynamic_cast<Dereferenced*>(allocatedValue.get());
                                if constexpr ( std::is_same_v<std::shared_ptr<Dereferenced>, T> && std::is_same_v<Dereferenced, Json::Value> )
                                    value = std::shared_ptr<Dereferenced>(casted); // Type mismatch, but reassignable
                                else if constexpr ( std::is_same_v<std::unique_ptr<Dereferenced>, T> && std::is_same_v<Dereferenced, Json::Value> )
                                    value = std::unique_ptr<Dereferenced>(casted); // Type mismatch, but reassignable
                                else
                                    throw TypeMismatch(value->type(), allocatedValue->type());
                            }
                            else if constexpr ( std::is_same_v<std::shared_ptr<Dereferenced>, T> ) // && value != nullptr && types match
                                value = std::shared_ptr<Dereferenced>(dynamic_cast<Dereferenced*>(allocatedValue.get()));
                            else if constexpr ( std::is_same_v<std::unique_ptr<Dereferenced>, T> ) // && value != nullptr && types match
                                value = std::unique_ptr<Dereferenced>(dynamic_cast<Dereferenced*>(allocatedValue.get()));
                            else // value != nullptr && value->type() == allocatedValue->type()
                            {
                                *value = *allocatedValue; // Non-null but not a smart pointer you can trust to perform cleanup, use value assignment
                                allocatedValue = nullptr;
                            }

                            allocatedValue.release();
                        }
                        else if ( value != nullptr ) // allocatedValue == nullptr
                            value = nullptr;
                    }
                    else if constexpr ( std::is_base_of_v<Value, T> )
                    {
                        if ( allocatedValue == nullptr ) // !is_pointable<T>::value
                            throw NullUnassignable();
                        else if ( value.type() != allocatedValue->type() ) // !is_pointable<T>::value && allocatedValue != nullptr
                            throw TypeMismatch(value.type(), allocatedValue->type());
                        else // !is_pointable_v<T> && allocatedValue != nullptr && value.type() == allocatedValue->type()
                        {
                            value = *allocatedValue;
                            allocatedValue = nullptr;
                        }
                    }
                    else
                        throw Exception("Cannot assign generic value to any type except Json::Value");
                }

                std::shared_ptr<Value> out() { return std::shared_ptr(std::move(allocatedValue)); }

            private:
                std::unique_ptr<Value> allocatedValue;
            };

            virtual ~Value() {};

            virtual Type type() const = 0;
            
            virtual bool & boolean() = 0;
            virtual std::string & number() = 0;
            virtual std::string & string() = 0;
            virtual std::map<std::string, std::shared_ptr<Value>> & object() = 0;

            virtual const bool & boolean() const = 0;
            virtual const std::string & number() const = 0;
            virtual const std::string & string() const = 0;
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const = 0;

            virtual size_t arraySize() const = 0;
            
            virtual size_t & nullArray() = 0;
            virtual std::vector<bool> & boolArray() = 0;
            virtual std::vector<std::string> & numberArray() = 0;
            virtual std::vector<std::string> & stringArray() = 0;
            virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() = 0;
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() = 0;
            
            virtual const size_t & nullArray() const = 0;
            virtual const std::vector<bool> & boolArray() const = 0;
            virtual const std::vector<std::string> & numberArray() const = 0;
            virtual const std::vector<std::string> & stringArray() const = 0;
            virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const = 0;
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const = 0;

            template <typename T>
            bool getNumber(T & num) const
            {
                return (std::stringstream(number()) >> num).good();
            }

            template <typename T>
            void setNumber(const T & number)
            {
                number() = std::to_string(number);
            }
        };
        
        using ObjectValue = std::map<std::string, std::shared_ptr<Value>>;

        class Bool final : public Value {
        public:
            Bool() : value(false) {}
            Bool(bool value) : value(value) {}
            Bool(const Bool & other) : value(other.value) {}
            
            Bool & operator=(const Value & other) { value = other.boolean(); return *this; }
            Bool & operator=(const Bool & other) { value = other.boolean(); return *this; }

            Type type() const final { return Value::Type::Boolean; }
            
            bool & boolean() final { return value; }
            std::string & number() final { throw TypeMismatch(Value::Type::Boolean, Value::Type::Number, "number"); }
            std::string & string() final { throw TypeMismatch(Value::Type::Boolean, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::Boolean, Value::Type::Object, "object"); }

            const bool & boolean() const final { return value; }
            const std::string & number() const final { throw TypeMismatch(Value::Type::Boolean, Value::Type::Number, "number"); }
            const std::string & string() const final { throw TypeMismatch(Value::Type::Boolean, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::Boolean, Value::Type::Object, "object");
            }

            size_t arraySize() const final { throw TypeMismatch(Value::Type::Boolean, Value::Type::Array, "arraySize"); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::Boolean, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::Boolean, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::Boolean, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::Boolean, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::Boolean, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::Boolean, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::Boolean, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::Boolean, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final { throw TypeMismatch(Value::Type::Boolean, Value::Type::NumberArray, "numberArray"); }
            const std::vector<std::string> & stringArray() const final { throw TypeMismatch(Value::Type::Boolean, Value::Type::StringArray, "stringArray"); }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::Boolean, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::Boolean, Value::Type::MixedArray, "mixedArray");
            }

        private:
            bool value;
        };
        class Number final : public Value {
        public:
            Number() : value("0") {}
            Number(const std::string & value) : value(value) {}
            Number(const Number & other) : value(other.value) {}
            Number(const char* value) : value(value) {}
            template <size_t N> Number(const char(&value)[N]) : value(value, N) {}
            template <typename T> Number(const T & value) : value(std::to_string(value)) {}
            
            Number & operator=(const Value & other) { value = other.number(); return *this; }
            Number & operator=(const Number & other) { value = other.number(); return *this; }
            
            Type type() const final { return Value::Type::Number; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::Number, Value::Type::Boolean, "bool"); }
            std::string & number() final { return value; }
            std::string & string() final { throw TypeMismatch(Value::Type::Number, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::Number, Value::Type::Object, "object"); }

            const bool & boolean() const final { throw TypeMismatch(Value::Type::Number, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { return value; }
            const std::string & string() const final { throw TypeMismatch(Value::Type::Number, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::Number, Value::Type::Object, "object");
            }

            size_t arraySize() const final { throw TypeMismatch(Value::Type::Number, Value::Type::Array, "arraySize"); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::Number, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::Number, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::Number, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::Number, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::Number, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::Number, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::Number, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::Number, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final { throw TypeMismatch(Value::Type::Number, Value::Type::NumberArray, "numberArray"); }
            const std::vector<std::string> & stringArray() const final { throw TypeMismatch(Value::Type::Number, Value::Type::StringArray, "stringArray"); }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::Number, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::Number, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
            std::string value;
        };
        class String final : public Value {
        public:
            String() : value() {}
            String(const std::string & value) : value(value) {}
            String(const String & other) : value(other.value) {}
            
            String & operator=(const Value & other) { value = other.string(); return *this; }
            String & operator=(const String & other) { value = other.string(); return *this; }
            
            Type type() const final { return Value::Type::String; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::String, Value::Type::Boolean, "bool"); }
            std::string & number() final { throw TypeMismatch(Value::Type::String, Value::Type::Number, "number"); }
            std::string & string() final { return value; }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::String, Value::Type::Object, "object"); }

            const bool & boolean() const final { throw TypeMismatch(Value::Type::String, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { throw TypeMismatch(Value::Type::String, Value::Type::Number, "number"); }
            const std::string & string() const final { return value; }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::String, Value::Type::Object, "object");
            }

            size_t arraySize() const final { throw TypeMismatch(Value::Type::String, Value::Type::Array, "arraySize"); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::String, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::String, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::String, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::String, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::String, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::String, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::String, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::String, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final { throw TypeMismatch(Value::Type::String, Value::Type::NumberArray, "numberArray"); }
            const std::vector<std::string> & stringArray() const final { throw TypeMismatch(Value::Type::String, Value::Type::StringArray, "stringArray"); }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::String, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::String, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
            std::string value;
        };
        class Object : public Value {
        public:
            Object() : value() {}
            Object(const std::map<std::string, std::shared_ptr<Value>> & value) : value(value) {}
            Object(const Object & other) : value(other.value) {}
            ~Object() override {}
            
            Object & operator=(const Value & other) { value = other.object(); return *this; }
            Object & operator=(const Object & other) { value = other.object(); return *this; }
            
            Type type() const override { return Value::Type::Object; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::Object, Value::Type::Boolean, "bool"); }
            std::string & number() final { throw TypeMismatch(Value::Type::Object, Value::Type::Number, "number"); }
            std::string & string() final { throw TypeMismatch(Value::Type::Object, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { return value; }
            
            const bool & boolean() const final { throw TypeMismatch(Value::Type::Object, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { throw TypeMismatch(Value::Type::Object, Value::Type::Number, "number"); }
            const std::string & string() const final { throw TypeMismatch(Value::Type::Object, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final { return value; }

            size_t arraySize() const final { throw TypeMismatch(Value::Type::Object, Value::Type::Array, "arraySize"); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::Object, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::Object, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::Object, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::Object, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::Object, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::Object, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::Object, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::Object, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final { throw TypeMismatch(Value::Type::Object, Value::Type::NumberArray, "numberArray"); }
            const std::vector<std::string> & stringArray() const final { throw TypeMismatch(Value::Type::Object, Value::Type::StringArray, "stringArray"); }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::Object, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::Object, Value::Type::MixedArray, "mixedArray");
            }
            
            void put(std::string fieldName, std::shared_ptr<Value> val)
            {
                this->value.insert_or_assign(fieldName, val);
            }
            
        private:
            std::map<std::string, std::shared_ptr<Value>> value;
        };
        
        class NullArray final : public Value {
        public:
            NullArray() : nullCount(0) {}
            NullArray(size_t nullCount) : nullCount(nullCount) {}
            NullArray(const NullArray & other) : nullCount(other.nullCount) {}
            
            NullArray & operator=(const Value & other) { nullCount = other.nullArray(); return *this; }
            NullArray & operator=(const NullArray & other) { nullCount = other.nullArray(); return *this; }
            
            Type type() const final { return Value::Type::NullArray; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::NullArray, Value::Type::Boolean, "bool"); }
            std::string & number() final { throw TypeMismatch(Value::Type::NullArray, Value::Type::Number, "number"); }
            std::string & string() final { throw TypeMismatch(Value::Type::NullArray, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::NullArray, Value::Type::Object, "object"); }

            const bool & boolean() const final { throw TypeMismatch(Value::Type::NullArray, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { throw TypeMismatch(Value::Type::NullArray, Value::Type::Number, "number"); }
            const std::string & string() const final { throw TypeMismatch(Value::Type::NullArray, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::Object, "object");
            }

            size_t arraySize() const final { return nullCount; }
            
            size_t & nullArray() final { return nullCount; }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::NullArray, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::NullArray, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::NullArray, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::NullArray, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { return nullCount; }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::NullArray, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::NumberArray, "numberArray");
            }
            const std::vector<std::string> & stringArray() const final {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::StringArray, "stringArray");
            }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::MixedArray, "mixedArray");
            }

        private:
            size_t nullCount;
        };
        class BoolArray final : public Value {
        public:
            BoolArray() : values() {}
            BoolArray(const std::vector<bool> & values) : values(values) {}
            BoolArray(const BoolArray & other) : values(other.values) {}
            
            BoolArray & operator=(const Value & other) { values = other.boolArray(); return *this; }
            BoolArray & operator=(const BoolArray & other) { values = other.boolArray(); return *this; }

            Type type() const final { return Value::Type::BoolArray; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Boolean, "bool"); }
            std::string & number() final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Number, "number"); }
            std::string & string() final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Object, "object"); }
            
            const bool & boolean() const final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Number, "number"); }
            const std::string & string() const final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::Object, "object");
            }

            size_t arraySize() const final { return values.size(); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { return values; }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::BoolArray, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { return values; }
            const std::vector<std::string> & numberArray() const final {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::NumberArray, "numberArray");
            }
            const std::vector<std::string> & stringArray() const final {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::StringArray, "stringArray");
            }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
            std::vector<bool> values;
        };
        class NumberArray final : public Value {
        public:
            NumberArray() : values() {}
            NumberArray(const std::vector<std::string> & values) : values(values) {}
            NumberArray(const NumberArray & other) : values(other.values) {}
            
            NumberArray & operator=(const Value & other) { values = other.numberArray(); return *this; }
            NumberArray & operator=(const NumberArray & other) { values = other.numberArray(); return *this; }
            
            Type type() const final { return Value::Type::NumberArray; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Boolean, "bool"); }
            std::string & number() final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Number, "number"); }
            std::string & string() final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Object, "object"); }
            
            const bool & boolean() const final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Number, "number"); }
            const std::string & string() const final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::Object, "object");
            }

            size_t arraySize() const final { return values.size(); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { return values; }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::NumberArray, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final { return values; }
            const std::vector<std::string> & stringArray() const final {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::StringArray, "stringArray");
            }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
            std::vector<std::string> values;
        };
        class StringArray final : public Value {
        public:
            StringArray() : values() {}
            StringArray(const std::vector<std::string> & other) : values(other) {}
            StringArray(const StringArray & other) : values(other.values) {}
            
            StringArray & operator=(const Value & other) { values = other.stringArray(); return *this; }
            StringArray & operator=(const StringArray & other) { values = other.stringArray(); return *this; }
            
            Type type() const final { return Value::Type::StringArray; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::StringArray, Value::Type::Boolean, "bool"); }
            std::string & number() final { throw TypeMismatch(Value::Type::StringArray, Value::Type::Number, "number"); }
            std::string & string() final { throw TypeMismatch(Value::Type::StringArray, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::StringArray, Value::Type::Object, "object"); }
            
            const bool & boolean() const final { throw TypeMismatch(Value::Type::StringArray, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { throw TypeMismatch(Value::Type::StringArray, Value::Type::Number, "number"); }
            const std::string & string() const final { throw TypeMismatch(Value::Type::StringArray, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::Object, "object");
            }

            size_t arraySize() const final { return values.size(); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::StringArray, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::StringArray, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::StringArray, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { return values; }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::StringArray, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::StringArray, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::StringArray, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::NumberArray, "numberArray");
            }
            const std::vector<std::string> & stringArray() const final { return values; }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::MixedArray, "mixedArray");
            }

        private:
            std::vector<std::string> values;
        };
        class ObjectArray final : public Value {
        public:
            ObjectArray() : values() {}
            ObjectArray(const std::vector<std::map<std::string, std::shared_ptr<Value>>> & values) : values(values) {}
            ObjectArray(const ObjectArray & other) : values(other.values) {}
            
            ObjectArray & operator=(const Value & other) { values = other.objectArray(); return *this; }
            ObjectArray & operator=(const ObjectArray & other) { values = other.objectArray(); return *this; }
            
            Type type() const final { return Value::Type::ObjectArray; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Boolean, "bool"); }
            std::string & number() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Number, "number"); }
            std::string & string() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Object, "object"); }
            
            const bool & boolean() const final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Number, "number"); }
            const std::string & string() const final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Object, "object");
            }

            size_t arraySize() const final { return values.size(); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final { return values; }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::MixedArray, "mixedArray"); }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final {
                throw TypeMismatch(Value::Type::ObjectArray, Value::Type::NumberArray, "numberArray");
            }
            const std::vector<std::string> & stringArray() const final {
                throw TypeMismatch(Value::Type::ObjectArray, Value::Type::StringArray, "stringArray");
            }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final { return values; }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final {
                throw TypeMismatch(Value::Type::ObjectArray, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
            std::vector<std::map<std::string, std::shared_ptr<Value>>> values;
        };
        class MixedArray final : public Value {
        public:
            MixedArray() : values() {}
            MixedArray(const std::vector<std::shared_ptr<Value>> & values) : values(values) {}
            MixedArray(const MixedArray & other) : values(other.values) {}
            
            MixedArray & operator=(const Value & other) { values = other.mixedArray(); return *this; }
            MixedArray & operator=(const MixedArray & other) { values = other.mixedArray(); return *this; }
            
            Type type() const final { return Value::Type::MixedArray; }
            
            bool & boolean() final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Boolean, "bool"); }
            std::string & number() final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Number, "number"); }
            std::string & string() final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::String, "string"); }
            std::map<std::string, std::shared_ptr<Value>> & object() final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Object, "object"); }
            
            const bool & boolean() const final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Boolean, "bool"); }
            const std::string & number() const final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Number, "number"); }
            const std::string & string() const final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::String, "string"); }
            const std::map<std::string, std::shared_ptr<Value>> & object() const final {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::Object, "object");
            }

            size_t arraySize() const final { return values.size(); }
            
            size_t & nullArray() final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::NullArray, "nullArray"); }
            std::vector<bool> & boolArray() final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::BoolArray, "boolArray"); }
            std::vector<std::string> & numberArray() final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::NumberArray, "numberArray"); }
            std::vector<std::string> & stringArray() final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::StringArray, "stringArray"); }
            std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() final {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::ObjectArray, "objectArray");
            }
            std::vector<std::shared_ptr<Value>> & mixedArray() final { return values; }
            
            const size_t & nullArray() const final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::NullArray, "nullArray"); }
            const std::vector<bool> & boolArray() const final { throw TypeMismatch(Value::Type::MixedArray, Value::Type::BoolArray, "boolArray"); }
            const std::vector<std::string> & numberArray() const final {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::NumberArray, "numberArray");
            }
            const std::vector<std::string> & stringArray() const final {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::StringArray, "stringArray");
            }
            const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const final {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::ObjectArray, "objectArray");
            }
            const std::vector<std::shared_ptr<Value>> & mixedArray() const final { return values; }

        private:
            std::vector<std::shared_ptr<Value>> values;
        };

        class FieldCluster final : public Object
        {
        public:
            FieldCluster() : Object() {}
            FieldCluster(const std::map<std::string, std::shared_ptr<Value>> & value) : Object(value) {}
            FieldCluster(const FieldCluster & other) : Object(other) {}
            
            FieldCluster & operator=(const Value & other) { object() = other.object(); return *this; }
            FieldCluster & operator=(const FieldCluster & other) { object() = other.object(); return *this; }
            
            Type type() const final { return Value::Type::FieldCluster; }
        };
    }
    
    inline namespace Identifiers
    {
        template <typename T> struct is_non_primitive : std::bool_constant<
            RareTs::is_tuple_v<T> || RareTs::is_pair_v<T> || RareTs::is_iterable_v<T> || RareTs::is_reflected_v<T>> {};
        template <typename T> struct is_non_primitive<const T> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<T*> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<T &> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<T &&> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<std::unique_ptr<T>> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<std::shared_ptr<T>> { static constexpr bool value = is_non_primitive<T>::value; };
        template <> struct is_non_primitive<std::string> { static constexpr bool value = false; };
        template <> struct is_non_primitive<Json::Generic::Object> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::NullArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::BoolArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::NumberArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::StringArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::ObjectArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::MixedArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::FieldCluster> { static constexpr bool value = true; };
        template <typename T> inline constexpr bool is_non_primitive_v = is_non_primitive<T>::value;

        template <size_t Index, typename ...Ts> struct is_non_primitive_tuple_element;
        template <typename ...Ts> struct is_non_primitive_tuple_element<0, std::tuple<Ts...>> { static constexpr bool value = false; };
        template <size_t Index, typename ...Ts> struct is_non_primitive_tuple_element<Index, std::tuple<Ts...>> {
            using T = std::tuple_element_t<Index-1, std::tuple<Ts...>>;
            static constexpr bool value = is_non_primitive_v<T> || is_non_primitive_tuple_element<Index-1, std::tuple<Ts...>>::value;
        };

        template <typename T> struct is_non_primitive_tuple { static constexpr bool value = false; };
        template <typename ...Ts> struct is_non_primitive_tuple<std::tuple<Ts...>> {
            static constexpr bool value = is_non_primitive_tuple_element<sizeof...(Ts), std::tuple<Ts...>>::value;
        };
        template <typename T> inline constexpr bool is_non_primitive_tuple_v = is_non_primitive_tuple<T>::value;

        template <typename T> struct is_tuple_pair { static constexpr bool value = false; };
        template <typename T1, typename T2> struct is_tuple_pair<std::tuple<T1, T2>> { static constexpr bool value = true; };
        template <typename T> inline constexpr bool is_tuple_pair_v = is_tuple_pair<T>::value;
    }

    inline namespace Output
    {
        inline constexpr const char twoSpaces[] = "  ";
        
        inline namespace Customizers
        {
            template <typename Object, typename Value,
                size_t MemberIndex = noMemberIndex, typename OpNotes = RareTs::NoNote, typename Member = NoMember, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t IndentLevel = 0, const char* Indent = twoSpaces>
            struct Customize : public RareTs::Unspecialized
            {
                // Should return true if you put any output, else you should leave output unchanged
                static bool as(OutStreamType & /*output*/, Context & /*context*/, const Object & /*object*/, const Value & /*value*/) { return false; }
            };

            template <typename Value, typename OpNotes = RareTs::NoNote, typename Member = NoMember, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t IndentLevel = 0, const char* Indent = twoSpaces>
            struct CustomizeType : public RareTs::Unspecialized
            {
                // Should return true if you put any output, else you should leave output unchanged
                static bool as(OutStreamType & /*output*/, Context & /*context*/, const Value & /*value*/) { return false; }
            };

            template <typename Object, typename Value,
                size_t MemberIndex = noMemberIndex, typename OpNotes = RareTs::NoNote, typename Member = NoMember, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t IndentLevel = 0, const char* Indent = twoSpaces>
            inline constexpr bool haveSpecialization =
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, OpNotes, Member, statics,
                    PrettyPrint, IndentLevel, Indent>> || // Fully specialized
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, OpNotes, Member>> || // Customize<5args> specialized
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, OpNotes>> || // Customize<4args> specialized
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex>> || // Customize<3args> specialized
                RareTs::is_specialized_v<Customize<Object, Value>> || // Customize<2args> specialized
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, RareTs::NoNote, Member>> || // Customize<5args>, OpNotes defaulted
                RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, OpNotes, Member>> || // Customize<5args>, MemberIndex defaulted
                RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, RareTs::NoNote, Member>> || // Customize<5args>, both defaulted
                RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, OpNotes>> || // Customize<4args>, MemberIndex defaulted
                RareTs::is_specialized_v<CustomizeType<Value, OpNotes, Member, statics,
                    PrettyPrint, IndentLevel, Indent>> || // Fully specialized
                RareTs::is_specialized_v<CustomizeType<Value, OpNotes, Member>> || // CustomizeType<3args> specialized
                RareTs::is_specialized_v<CustomizeType<Value, OpNotes>> || // CustomizeType<2args> specialized
                RareTs::is_specialized_v<CustomizeType<Value>> || // CustomizeType<1arg> specialized
                RareTs::is_specialized_v<CustomizeType<Value, RareTs::NoNote, Member>>; // CustomizeType<3arg>, OpNotes defaulted
        }

        inline namespace StaticAffix
        {
            template <bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces>
            struct IndentType {};
            
            template <bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces>
            inline constexpr IndentType<PrettyPrint, IndentLevel, Indent> indent{};

            template <bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces>
            inline OutStreamType & operator<<(OutStreamType & os, const IndentType<PrettyPrint, IndentLevel, Indent> &)
            {
                if constexpr ( IndentLevel > 0 && PrettyPrint )
                {
                    for ( size_t i=0; i<IndentLevel; i++ )
                        os << Indent;
                }

                return os;
            }
            
            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* Indent = twoSpaces>
            struct ArrayPrefixType {};

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* Indent = twoSpaces>
            inline constexpr ArrayPrefixType<PrettyPrint, ContainsPrimitives, IndentLevel, Indent> ArrayPrefix{};

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* Indent = twoSpaces>
            inline OutStreamType & operator<<(OutStreamType & os, const ArrayPrefixType<PrettyPrint, ContainsPrimitives, IndentLevel, Indent> &)
            {
                if constexpr ( !PrettyPrint )
                    os << "[";
                else if constexpr ( ContainsPrimitives )
                    os << "[ ";
                else
                    os << "[" << Json::endl << indent<PrettyPrint, IndentLevel+1, Indent>;

                return os;
            }

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* Indent = twoSpaces>
            struct ArraySuffixType {};

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* Indent = twoSpaces>
            inline constexpr ArraySuffixType<PrettyPrint, ContainsPrimitives, IndentLevel, Indent> ArraySuffix{};

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* Indent = twoSpaces>
            inline OutStreamType & operator<<(OutStreamType & os, const ArraySuffixType<PrettyPrint, ContainsPrimitives, IndentLevel, Indent> &)
            {
                if constexpr ( !PrettyPrint )
                    os << "]";
                else if constexpr ( ContainsPrimitives )
                    os << " ]";
                else
                    os << Json::endl << indent<PrettyPrint, IndentLevel, Indent> << "]";

                return os;
            }

            template <bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            struct ObjectPrefixType {};
                
            template <bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            inline constexpr ObjectPrefixType<PrettyPrint, IndentLevel, Indent> ObjectPrefix{};
                
            template <bool PrettyPrint, size_t IndentLevel, const char* Indent, Statics statics, typename Object>
            inline OutStreamType & operator<<(OutStreamType & os, const ObjectPrefixType<PrettyPrint, IndentLevel, Indent, statics, Object> &)
            {
                os << "{";
                return os;
            }

            template <bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            struct ObjectSuffixType {};
                
            template <bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            inline constexpr ObjectSuffixType<PrettyPrint, IndentLevel, Indent, statics, Object> ObjectSuffix{};
                
            template <bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            inline OutStreamType & operator<<(OutStreamType & os, const ObjectSuffixType<PrettyPrint, IndentLevel, Indent, statics, Object> &)
            {
                if constexpr ( std::is_same_v<void, Object> )
                {
                    if constexpr ( PrettyPrint )
                        os << Json::endl << indent<PrettyPrint, IndentLevel, Indent> << "}";
                    else
                        os << "}";
                }
                else
                {
                    constexpr bool NotEmpty = hasFields<statics, Object>();
                    if constexpr ( PrettyPrint && NotEmpty )
                        os << Json::endl << indent<PrettyPrint, IndentLevel, Indent> << "}";
                    else
                        os << "}";
                }
                return os;
            }
                
            template <bool IsFirst, bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces>
            struct FieldPrefixType {};
                
            template <bool IsFirst, bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces>
            inline constexpr FieldPrefixType<IsFirst, PrettyPrint, IndentLevel, Indent> FieldPrefix {};
                
            template <bool IsFirst, bool PrettyPrint, size_t IndentLevel, const char* Indent = twoSpaces>
            inline OutStreamType & operator<<(OutStreamType & os, const FieldPrefixType<IsFirst, PrettyPrint, IndentLevel, Indent> &)
            {
                if constexpr ( PrettyPrint )
                {
                    if constexpr ( IsFirst )
                        os << Json::endl << indent<PrettyPrint, IndentLevel, Indent>;
                    else 
                        os << "," << Json::endl << indent<PrettyPrint, IndentLevel, Indent>;
                }
                else if constexpr ( !IsFirst )
                    os << ",";

                return os;
            }
                
            template <bool PrettyPrint>
            struct FieldNameValueSeparatorType {};

            template <bool PrettyPrint>
            inline constexpr FieldNameValueSeparatorType<PrettyPrint> fieldNameValueSeparator{};
                
            template <bool PrettyPrint>
            inline OutStreamType & operator<<(OutStreamType & os, const FieldNameValueSeparatorType<PrettyPrint> &)
            {
                if constexpr ( PrettyPrint )
                    os << ": ";
                else
                    os << ":";

                return os;
            }
        }

        namespace Put
        {
            inline namespace Affix
            {
                template <bool PrettyPrint, const char* Indent = twoSpaces>
                inline void indent(OutStreamType & os, size_t indentLevel)
                {
                    if constexpr ( PrettyPrint )
                    {
                        for ( size_t i=0; i<indentLevel; i++ )
                            os << Indent;
                    }
                }
                
                template <bool PrettyPrint, bool ContainsPrimitives, const char* Indent = twoSpaces>
                inline void arrayPrefix(OutStreamType & os, size_t indentLevel)
                {
                    if constexpr ( !PrettyPrint )
                        os << "[";
                    else if constexpr ( ContainsPrimitives )
                        os << "[ ";
                    else
                    {
                        os << "[" << Json::endl;
                        Put::indent<PrettyPrint, Indent>(os, indentLevel+1);
                    }
                }

                template <bool PrettyPrint, bool ContainsPrimitives, const char* Indent = twoSpaces>
                inline void arraySuffix(OutStreamType & os, size_t indentLevel)
                {
                    if constexpr ( !PrettyPrint )
                        os << "]";
                    else if constexpr ( ContainsPrimitives )
                        os << " ]";
                    else
                    {
                        os << Json::endl;
                        Put::indent<PrettyPrint, Indent>(os, indentLevel);
                        os << "]";
                    }
                }
                
                template <bool PrettyPrint, const char* Indent>
                inline void objectPrefix(OutStreamType & os)
                {
                    os << "{";
                }
                
                template <bool PrettyPrint, const char* Indent = twoSpaces>
                inline void objectSuffix(OutStreamType & os, bool isEmpty, size_t indentLevel)
                {
                    if constexpr ( PrettyPrint )
                    {
                        if ( !isEmpty )
                        {
                            os << Json::endl;
                            Put::indent<PrettyPrint, Indent>(os, indentLevel);
                            os << "}";
                        }
                        else
                            os << "}";
                    }
                    else
                        os << "}";
                }
                
                template <bool PrettyPrint, const char* Indent = twoSpaces>
                inline void fieldPrefix(OutStreamType & os, bool isFirst, size_t indentLevel)
                {
                    if constexpr ( PrettyPrint )
                    {
                        if ( isFirst )
                        {
                            os << Json::endl;
                            Put::indent<PrettyPrint, Indent>(os, indentLevel);
                        }
                        else
                        {
                            os << "," << Json::endl;
                            Put::indent<PrettyPrint, Indent>(os, indentLevel);
                        }
                    }
                    else if ( !isFirst )
                        os << ",";
                }

                template <bool PrettyPrint, bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* Indent = twoSpaces>
                inline void nestedPrefix(OutStreamType & os, bool isEmpty)
                {
                    if constexpr ( IsArray )
                    {
                        if ( isEmpty )
                            os << StaticAffix::ArrayPrefix<false, ContainsPrimitives, IndentLevel, Indent>;
                        else
                            os << StaticAffix::ArrayPrefix<PrettyPrint, ContainsPrimitives, IndentLevel, Indent>;
                    }
                    else
                    {
                        if ( isEmpty )
                            os << StaticAffix::ObjectPrefix<false, IndentLevel, Indent>;
                        else
                            os << StaticAffix::ObjectPrefix<PrettyPrint, IndentLevel, Indent>;
                    }
                }

                template <bool PrettyPrint, const char* Indent = twoSpaces>
                inline void nestedPrefix(OutStreamType & os, bool isArray, bool containsPrimitives, bool isEmpty, size_t indentLevel)
                {
                    if ( isArray )
                    {
                        if ( isEmpty )
                        {
                            if ( containsPrimitives )
                                Put::arrayPrefix<false, true, Indent>(os, indentLevel);
                            else
                                Put::arrayPrefix<false, false, Indent>(os, indentLevel);
                        }
                        else
                        {
                            if ( containsPrimitives )
                                Put::arrayPrefix<PrettyPrint, true, Indent>(os, indentLevel);
                            else
                                Put::arrayPrefix<PrettyPrint, false, Indent>(os, indentLevel);
                        }
                    }
                    else
                    {
                        if ( isEmpty )
                            Put::objectPrefix<false, Indent>(os);
                        else
                            Put::objectPrefix<PrettyPrint, Indent>(os);
                    }
                }
                
                template <bool PrettyPrint, bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* Indent = twoSpaces>
                inline void nestedSuffix(OutStreamType & os, bool isEmpty)
                {
                    if constexpr ( IsArray )
                    {
                        if ( isEmpty )
                            os << StaticAffix::ArraySuffix<false, ContainsPrimitives, IndentLevel, Indent>;
                        else
                            os << StaticAffix::ArraySuffix<PrettyPrint, ContainsPrimitives, IndentLevel, Indent>;
                    }
                    else
                    {
                        if ( isEmpty )
                            os << StaticAffix::ObjectSuffix<false, IndentLevel, Indent>;
                        else
                            os << StaticAffix::ObjectSuffix<PrettyPrint, IndentLevel, Indent>;
                    }
                }
                
                template <bool PrettyPrint, const char* Indent = twoSpaces>
                inline void nestedSuffix(OutStreamType & os, bool isArray, bool containsPrimitives, bool isEmpty, size_t indentLevel)
                {
                    if ( isArray )
                    {
                        if ( isEmpty )
                        {
                            if ( containsPrimitives )
                                Put::arraySuffix<false, true, Indent>(os, indentLevel);
                            else
                                Put::arraySuffix<false, false, Indent>(os, indentLevel);
                        }
                        else
                        {
                            if ( containsPrimitives )
                                Put::arraySuffix<PrettyPrint, true, Indent>(os, indentLevel);
                            else
                                Put::arraySuffix<PrettyPrint, false, Indent>(os, indentLevel);
                        }
                    }
                    else
                    {
                        if ( isEmpty )
                            Put::objectSuffix<false, Indent>(os, isEmpty, indentLevel);
                        else
                            Put::objectSuffix<PrettyPrint, Indent>(os, isEmpty, indentLevel);
                    }
                }
                
                template <bool PrettyPrint, bool IsJsonField, bool NestedSeparator, size_t IndentLevel, const char* Indent = twoSpaces>
                inline void separator(OutStreamType & os, bool isFirst)
                {
                    if constexpr ( IsJsonField )
                    {
                        if ( isFirst && PrettyPrint )
                            os << Json::endl << StaticAffix::indent<PrettyPrint, IndentLevel, Indent>;
                        else if constexpr ( PrettyPrint )
                            os << "," << Json::endl << StaticAffix::indent<PrettyPrint, IndentLevel, Indent>;
                        else if ( !isFirst )
                            os << ",";
                    }
                    else if ( !isFirst )
                    {
                        if constexpr ( NestedSeparator && PrettyPrint )
                            os << "," << Json::endl << StaticAffix::indent<PrettyPrint, IndentLevel, Indent>;
                        else if constexpr ( PrettyPrint )
                            os << ", ";
                        else
                            os << ",";
                    }
                }
                
                template <bool PrettyPrint, bool IsJsonField, bool NestedSeparator, const char* Indent = twoSpaces>
                inline void separator(OutStreamType & os, bool isFirst, size_t indentLevel)
                {
                    if constexpr ( IsJsonField )
                    {
                        if ( isFirst && PrettyPrint )
                        {
                            os << Json::endl;
                            Put::indent<PrettyPrint, Indent>(os, indentLevel);
                        }
                        else if constexpr ( PrettyPrint )
                        {
                            os << "," << Json::endl;
                            Put::indent<PrettyPrint, Indent>(os, indentLevel);
                        }
                        else if ( !isFirst )
                            os << ",";
                    }
                    else if ( !isFirst )
                    {
                        if constexpr ( NestedSeparator && PrettyPrint )
                        {
                            os << "," << Json::endl;
                            Put::indent<PrettyPrint, Indent>(os, indentLevel);
                        }
                        else if constexpr ( PrettyPrint )
                            os << ", ";
                        else
                            os << ",";
                    }
                }
            }

            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, typename Value>
            constexpr bool customization(OutStreamType & os, Context & context, const Object & obj, const Value & value)
            {
                if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, Member::index, Annotations, Member, statics,
                    PrettyPrint, IndentLevel, Indent>> )
                {
                    return Customize<Object, Value, Member::index, Annotations, Member, statics,
                        PrettyPrint, IndentLevel, Indent>::as(os, context, obj, value); // Customize fully specialized
                }
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, Member::index, Annotations, Member>> )
                    return Customize<Object, Value, Member::index, Annotations, Member>::as(os, context, obj, value); // Five Customize arguments specialized
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, Member::index, RareTs::NoNote, Member>> )
                    return Customize<Object, Value, Member::index, RareTs::NoNote, Member>::as(os, context, obj, value); // Customize<5args>, Annotations defaulted
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, Annotations, Member>> )
                    return Customize<Object, Value, noMemberIndex, Annotations, Member>::as(os, context, obj, value); // Customize<5args>, MemberIndex defaulted
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, RareTs::NoNote, Member>> )
                    return Customize<Object, Value, noMemberIndex, RareTs::NoNote, Member>::as(os, context, obj, value); // Customize<5args>, two args defaulted
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, Member::index, Annotations>> )
                    return Customize<Object, Value, Member::index, Annotations>::as(os, context, obj, value); // Four Customize arguments specialized
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, Annotations>> )
                    return Customize<Object, Value, noMemberIndex, Annotations>::as(os, context, obj, value); // Customize<4args>, MemberIndex defaulted
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, Member::index>> )
                    return Customize<Object, Value, Member::index>::as(os, context, obj, value); // Three Customize arguments specialized
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value>> )
                    return Customize<Object, Value>::as(os, context, obj, value); // Two Customize arguments specialized
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value, Annotations, Member, statics,
                    PrettyPrint, IndentLevel, Indent>> )
                {
                    return CustomizeType<Value, Annotations, Member, statics,
                        PrettyPrint, IndentLevel, Indent>::as(os, context, value); // CustomizeType fully specialized
                }
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value, Annotations, Member>> )
                    return CustomizeType<Value, Annotations, Member>::as(os, context, value); // Three CustomizeType arguments specialized
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value, RareTs::NoNote, Member>> )
                    return CustomizeType<Value, RareTs::NoNote, Member>::as(os, context, value); // CustomizeType<3args>, Annotations defaulted
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value, Annotations>> )
                    return CustomizeType<Value, Annotations>::as(os, context, value); // Two CustomizeType arguments specialized
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value>> )
                    return CustomizeType<Value>::as(os, context, value); // One CustomizeType argument specialized
                else
                    return false;
            }

            inline void string(OutStreamType & os, const std::string & str)
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
            inline void string(OutStreamType & os, const T & t)
            {
#ifdef USE_BUFFERED_STREAMS
                StringBuffer ss;
#else
                std::stringstream ss;
#endif
                ss << t;
                Put::string(os, ss.str());
            }

            template <typename Annotations, bool PrettyPrint, const char* Indent>
            inline void genericIterable(OutStreamType & os, Context & context, size_t indentLevel, const Generic::Value & iterable);
            
            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, bool IsFirst,
                bool IsComplexTuple = false, size_t TupleIndex = 0, typename T1, typename T2, typename ...Ts>
            constexpr void tuple(OutStreamType & os, Context & context, const Object & obj, const std::tuple<T1, T2, Ts...> & value);

            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, bool IsFirst, typename Key, typename T>
            constexpr void pair(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & value);

            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, typename IterableValue = uint_least8_t>
            constexpr void iterable(OutStreamType & os, Context & context, const Object & obj, const IterableValue & iterable);
            
            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* Indent, typename T>
            constexpr void object(OutStreamType & os, Context & context, const T & obj);

            template <typename Annotations, bool PrettyPrint, const char* Indent, bool IsFirst>
            inline void genericValue(OutStreamType & os, Context & context, size_t indentLevel, const Generic::Value & value)
            {
                switch ( value.type() )
                {
                    case Generic::Value::Type::Boolean:
                        os << (value.boolean() ? "true" : "false");
                        break;
                    case Generic::Value::Type::Number:
                        os << value.number();
                        break;
                    case Generic::Value::Type::String:
                        Put::string(os, value.string());
                        break;
                    case Generic::Value::Type::Object:
                    case Generic::Value::Type::NullArray:
                    case Generic::Value::Type::BoolArray:
                    case Generic::Value::Type::NumberArray:
                    case Generic::Value::Type::StringArray:
                    case Generic::Value::Type::ObjectArray:
                    case Generic::Value::Type::MixedArray:
                        Put::genericIterable<Annotations, PrettyPrint, Indent>(os, context, indentLevel, value);
                        break;
                    case Generic::Value::Type::FieldCluster:
                    {
                        const std::map<std::string, std::shared_ptr<Generic::Value>> & fieldCluster = value.object();
                        bool isFirst = IsFirst;
                        for ( const auto & field : fieldCluster )
                        {
                            Put::fieldPrefix<PrettyPrint, Indent>(os, isFirst, indentLevel);
                            Put::string(os, field.first);
                            os << fieldNameValueSeparator<PrettyPrint>;
                            if ( field.second == nullptr )
                                os << "null";
                            else
                            {
                                Put::genericValue<Annotations, PrettyPrint, Indent, false>(
                                    os, context, indentLevel, (Generic::Value &)*field.second);
                            }

                            isFirst = false;
                        }
                    }
                    break;
                    case Generic::Value::Type::None: case Generic::Value::Type::Null: case Generic::Value::Type::Array: break; // Unused cases
                }
            }
            
            template <typename Annotations, bool PrettyPrint, const char* Indent>
            inline void genericIterable(OutStreamType & os, Context & context, size_t indentLevel, const Generic::Value & iterable)
            {
                bool isObject = iterable.type() == Generic::Value::Type::Object;
                bool containsPrimitives = iterable.type() == Generic::Value::Type::BoolArray ||
                    iterable.type() == Generic::Value::Type::NumberArray || iterable.type() == Generic::Value::Type::StringArray;
                bool isEmpty = (isObject && iterable.object().empty()) || (!isObject && iterable.arraySize() == 0);

                size_t i=0;
                Put::nestedPrefix<PrettyPrint, Indent>(os, !isObject, containsPrimitives, isEmpty, indentLevel);
                switch ( iterable.type() )
                {
                    case Generic::Value::Type::Object:
                    {
                        const std::map<std::string, std::shared_ptr<Generic::Value>> & obj = iterable.object();
                        bool isFirst = true;
                        for ( const auto & field : obj )
                        {
                            Put::fieldPrefix<PrettyPrint, Indent>(os, isFirst, indentLevel+1);
                            Put::string(os, field.first);
                            os << fieldNameValueSeparator<PrettyPrint>;
                            if ( field.second == nullptr )
                                os << "null";
                            else
                            {
                                Put::genericValue<Annotations, PrettyPrint, Indent, false>(
                                    os, context, indentLevel+1, (Generic::Value &)*field.second);
                            }
                            isFirst = false;
                        }
                    }
                    break;
                    case Generic::Value::Type::NullArray:
                    {
                        for ( size_t j=0; j<iterable.nullArray(); j++ )
                        {
                            Put::separator<PrettyPrint, false, false, Indent>(os, 0 == j, indentLevel+1);
                            os << "null";
                        }
                    }
                    break;
                    case Generic::Value::Type::BoolArray:
                    {
                        const std::vector<bool> & array = iterable.boolArray();
                        for ( const auto & element : array )
                        {
                            Put::separator<PrettyPrint, false, false, Indent>(os, 0 == i++, indentLevel+1);
                            os << (element ? "true" : "false");
                        }
                    }
                    break;
                    case Generic::Value::Type::NumberArray:
                    {
                        const std::vector<std::string> & array = iterable.numberArray();
                        for ( const auto & element : array )
                        {
                            Put::separator<PrettyPrint, false, false, Indent>(os, 0 == i++, indentLevel+1);
                            os << element;
                        }
                    }
                    break;
                    case Generic::Value::Type::StringArray:
                    {
                        const std::vector<std::string> & array = iterable.stringArray();
                        for ( const auto & element : array )
                        {
                            Put::separator<PrettyPrint, false, false, Indent>(os, 0 == i++, indentLevel+1);
                            os << "\"" << element << "\"";
                        }
                    }
                    break;
                    case Generic::Value::Type::ObjectArray:
                    {
                        const std::vector<std::map<std::string, std::shared_ptr<Generic::Value>>> & array = iterable.objectArray();
                        for ( const std::map<std::string, std::shared_ptr<Generic::Value>> & obj : array )
                        {
                            Put::separator<PrettyPrint, false, true, Indent>(os, 0 == i++, indentLevel+1);
                            bool isFirst = true;
                            Put::objectPrefix<PrettyPrint, Indent>(os);
                            for ( const auto & field : obj )
                            {
                                Put::fieldPrefix<PrettyPrint, Indent>(os, isFirst, indentLevel+2);
                                Put::string(os, field.first);
                                os << fieldNameValueSeparator<PrettyPrint>;
                                if ( field.second == nullptr )
                                    os << "null";
                                else
                                {
                                    Put::genericValue<Annotations, PrettyPrint, Indent, false>(
                                        os, context, indentLevel+2, (Generic::Value &)*field.second);
                                }
                                isFirst = false;
                            }
                            Put::objectSuffix<PrettyPrint, Indent>(os, obj.empty(), indentLevel+1);
                        }
                    }
                    break;
                    case Generic::Value::Type::MixedArray:
                    {
                        const std::vector<std::shared_ptr<Generic::Value>> & array = iterable.mixedArray();
                        for ( const std::shared_ptr<Generic::Value> & element : array )
                        {
                            Put::separator<PrettyPrint, false, true, Indent>(os, 0 == i++, indentLevel+1);
                            if ( element == nullptr )
                                os << "null";
                            else
                                Put::genericValue<Annotations, PrettyPrint, Indent, false>(os, context, indentLevel+1, *element);
                        }
                    }
                    break;
                    case Generic::Value::Type::None: case Generic::Value::Type::Null: case Generic::Value::Type::Boolean: case Generic::Value::Type::Number:
                    case Generic::Value::Type::String: case Generic::Value::Type::Array: case Generic::Value::Type::FieldCluster: break; // Unused cases
                }
                Put::nestedSuffix<PrettyPrint, Indent>(os, !isObject, containsPrimitives, isEmpty, indentLevel);
            }

            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, bool IsFirst, typename T>
            constexpr void value(OutStreamType & os, Context & context, const Object & obj, const T & value)
            {
                if constexpr ( Customizers::haveSpecialization<Object, T, Member::index, Annotations, Member, statics,
                    PrettyPrint, IndentLevel, Indent> )
                {
                    if ( Put::customization<Annotations, Member, statics,
                        PrettyPrint, IndentLevel, Indent, Object, T>(os, context, obj, value) )
                    {
                        return;
                    }
                }

                if constexpr ( RareMapper::has_default_mapping_v<T, typename Member::Notes, Annotations> )
                {
                    using D = RareMapper::default_mapping_t<T, typename Member::Notes, Annotations>;
                    D d = RareMapper::map<D>(value);
                    Put::value<Annotations, MockMember<D>, statics, PrettyPrint, IndentLevel, Indent, Object, IsFirst>(os, context, obj, d);
                }
                else if constexpr ( RareTs::is_pointable_v<T> )
                {
                    if ( value == nullptr )
                        os << "null";
                    else
                    {
                        Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object, IsFirst>(
                            os, context, obj, *value);
                    }
                }
                else if constexpr ( std::is_base_of_v<Generic::Value, T> )
                {
                    Put::genericValue<Annotations, PrettyPrint, Indent, IsFirst>(os, context, IndentLevel, (const Generic::Value &)value);
                }
                else if constexpr ( RareTs::is_tuple_v<T> )
                {
                    if constexpr ( std::tuple_size_v<T> == 0 )
                        os << "null";
                    else if constexpr ( std::tuple_size_v<T> == 1 )
                    {
                        Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object, IsFirst>(
                            os, context, obj, std::get<0>(value));
                    }
                    else if constexpr ( std::tuple_size_v<T> >= 2 )
                    {
                        Put::tuple<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object, IsFirst>(
                            os, context, obj, value);
                    }
                }
                else if constexpr ( RareTs::is_pair_v<T> )
                    Put::pair<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object, IsFirst>(os, context, obj, value);
                else if constexpr ( std::is_same_v<std::string, std::remove_const_t<T>> && !Member::template hasNote<Json::UnstringType>() )
                    Put::string(os, value);
                else if constexpr ( RareTs::is_iterable_v<T> )
                    Put::iterable<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object>(os, context, obj, value);
                else if constexpr ( RareTs::is_reflected_v<T> )
                    Put::object<Annotations, statics, PrettyPrint, IndentLevel, Indent, T>(os, context, value);
                else if constexpr ( Member::template hasNote<Json::StringifyType>() )
                    Put::string(os, value);
                else if constexpr ( Member::template hasNote<Json::EnumIntType>() )
                    os << (typename RareTs::promote_char_t<std::underlying_type_t<T>>)value;
                else if constexpr ( std::is_same_v<bool, std::remove_const_t<T>> )
                    os << (value ? "true" : "false");
                else if constexpr ( std::is_enum_v<T> )
                    os << (RareTs::promote_char_t<std::underlying_type_t<T>>)value;
                else
                    os << (RareTs::promote_char_t<T>)value;
            }
            
            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, bool IsFirst,
                bool IsComplexTuple, size_t TupleIndex, typename T1, typename T2, typename ...Ts>
            constexpr void tuple(OutStreamType & os, Context & context, const Object & obj, const std::tuple<T1, T2, Ts...> & value)
            {
                constexpr bool isComplexTuple = TupleIndex == 0 ? is_non_primitive_tuple_v<std::tuple<T1, T2, Ts...>> : IsComplexTuple;
                constexpr size_t tupleSize = std::tuple_size_v<std::remove_reference_t<decltype(value)>>;
                if constexpr ( TupleIndex == 0 )
                    Put::arrayPrefix<PrettyPrint, !isComplexTuple, Indent>(os, IndentLevel);
                if constexpr ( TupleIndex < tupleSize )
                {
                    Put::separator<PrettyPrint, false, isComplexTuple, IndentLevel+1, Indent>(os, TupleIndex == 0);
                    Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, IsFirst>(
                        os, context, obj, std::get<TupleIndex>(value));
                    Put::tuple<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object, IsFirst,
                        isComplexTuple, TupleIndex+1>(os, context, obj, value);
                }
                else if constexpr ( TupleIndex == tupleSize )
                    Put::arraySuffix<PrettyPrint, !isComplexTuple, Indent>(os, IndentLevel);
            }
            
            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, bool IsFirst, typename Key, typename T>
            constexpr void pair(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & value)
            {
                constexpr bool isComplexPair = is_non_primitive_v<Key> || is_non_primitive_v<T>;
                Put::arrayPrefix<PrettyPrint, !isComplexPair, Indent>(os, IndentLevel);
                Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, IsFirst>(
                    os, context, obj, value.first);
                Put::separator<PrettyPrint, false, isComplexPair, IndentLevel+1, Indent>(os, false);
                Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, IsFirst>(
                    os, context, obj, value.second);
                Put::arraySuffix<PrettyPrint, !isComplexPair, Indent>(os, IndentLevel);
            }

            template<typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, bool IsFirst, typename Key, typename T>
            constexpr void keyValueObject(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & pair)
            {
                os << StaticAffix::ObjectPrefix<PrettyPrint, IndentLevel, Indent, statics>;
                os << StaticAffix::FieldPrefix<true, PrettyPrint, IndentLevel+1, Indent>;
                Put::string(os, "key");
                os << fieldNameValueSeparator<PrettyPrint>;
                Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, true>(
                    os, context, obj, pair.first);
                os << StaticAffix::FieldPrefix<false, PrettyPrint, IndentLevel+1, Indent>;
                Put::string(os, "value");
                os << fieldNameValueSeparator<PrettyPrint>;
                Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, false>(
                    os, context, obj, pair.second);
                os << StaticAffix::ObjectSuffix<PrettyPrint, IndentLevel, Indent, statics>;
            }
            
            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, bool IsFirst, typename T, typename Key>
            constexpr void fieldPair(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & pair)
            {
                Put::string(os, pair.first);
                os << fieldNameValueSeparator<PrettyPrint>;
                Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object, IsFirst>(
                    os, context, obj, pair.second);
            }

            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, typename IterableValue>
            constexpr void iterable(OutStreamType & os, Context & context, const Object & obj, const IterableValue & iterable)
            {
                using Element = RareTs::element_type_t<std::remove_cv_t<IterableValue>>;
                constexpr bool IsMap = RareTs::is_map_v<IterableValue>; // Simple maps are just json objects with keys as the field names, values as field values
                constexpr bool HasComplexKey = IsMap && is_non_primitive_v<RareTs::pair_lhs_t<Element>>;
                constexpr bool IsArray = !IsMap || HasComplexKey; // Maps with complex keys are arrays consisting of objects with two fields: "key" and "value"
                constexpr bool ContainsPrimitives = !is_non_primitive_v<Element>;

                size_t i=0;
                Put::nestedPrefix<PrettyPrint, IsArray, ContainsPrimitives, IndentLevel, Indent>(os, RareTs::isEmpty(iterable));
                if constexpr ( RareTs::has_begin_end_v<IterableValue> )
                {
                    for ( const auto & element : iterable )
                    {
                        Put::separator<PrettyPrint, !IsArray, !ContainsPrimitives, IndentLevel+1, Indent>(os, 0 == i++);
                        if constexpr ( !IsMap )
                        {
                            Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, false>(
                                os, context, obj, element);
                        }
                        else if constexpr ( HasComplexKey )
                        {
                            Put::keyValueObject<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, false>(
                                os, context, obj, element);
                        }
                        else
                        {
                            Put::fieldPair<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, false>(
                                os, context, obj, element);
                        }
                    }
                }
                else if constexpr ( RareTs::is_adaptor_v<IterableValue> )
                {
                    const auto & sequenceContainer = RareTs::baseContainer(iterable);
                    for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    {
                        Put::separator<PrettyPrint, !IsArray, !ContainsPrimitives, IndentLevel+1, Indent>(os, 0 == i++);
                        Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, false>(
                            os, context, obj, *it);
                    }
                }
                else if constexpr ( std::is_array_v<IterableValue> && std::extent<typename Member::type>::value > 0 )
                {
                    for ( ; i<std::extent_v<typename Member::type>; i++ )
                    {
                        Put::separator<PrettyPrint, !IsArray, !ContainsPrimitives, IndentLevel + 1, Indent>(os, 0 == i);
                        Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel+1, Indent, Object, false>(
                            os, context, obj, iterable[i]);
                    }
                }
                Put::nestedSuffix<PrettyPrint, IsArray, ContainsPrimitives, IndentLevel, Indent>(os, RareTs::isEmpty(iterable));
            }

            template <typename Annotations, typename Member, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, typename MemberName>
            constexpr void field(OutStreamType & os, Context & context, const Object & obj, MemberName fieldName,
                const typename Member::type & value)
            {
                if constexpr ( std::is_base_of_v<Generic::FieldCluster, RareTs::remove_pointer_t<typename Member::type>> )
                {
                    if constexpr ( Member::index == firstIndex<statics, Object>() )
                        throw Exception("Json::Generic::FieldCluster cannot be the first or the only field in an object");
                    else if constexpr ( RareTs::is_pointable_v<typename Member::type> )
                    {
                        if ( value != nullptr )
                            Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object, false>(os, context, obj, *value);
                    }
                    else
                        Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object, false>(os, context, obj, value);
                }
                else
                {
                    os << StaticAffix::FieldPrefix<Member::index == firstIndex<statics, Object>(), PrettyPrint, IndentLevel, Indent>;
                    Put::string(os, fieldName);
                    os << fieldNameValueSeparator<PrettyPrint>;
                    Put::value<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent,
                        Object, Member::index == firstIndex<statics, Object>()>(os, context, obj, value);
                }
            }
            
            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object>
            constexpr void fields(OutStreamType & os, Context & context, const Object & obj)
            {
                Reflect<Object>::Members::template forEach<IsUnignoredDataMatchingStatics, StaticType<statics>>(obj, [&](auto & member, auto & value) {
                    using Member = std::remove_reference_t<decltype(member)>;
                    if constexpr ( Member::template hasNote<Json::Name>() )
                    {
                        const auto & fieldName = member.template getNote<Json::Name>().value;
                        Put::field<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object>(os, context, obj, fieldName, value);
                    }
                    else
                        Put::field<Annotations, Member, statics, PrettyPrint, IndentLevel, Indent, Object>(os, context, obj, member.name, value);
                });
            }

            template <typename Annotations, size_t SuperIndex, typename T, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object, typename FieldName>
            constexpr void super(OutStreamType & os, Context & context, const Object & obj, const FieldName & superFieldName)
            {
                if constexpr ( hasFields<statics, T>() )
                {
                    constexpr bool IsFirst = !hasFields<statics, Object, false>() && SuperIndex == firstSuperIndex<statics, Object>();
                    os << StaticAffix::FieldPrefix<IsFirst, PrettyPrint, IndentLevel, Indent>;
                    Put::string(os, superFieldName);
                    os << fieldNameValueSeparator<PrettyPrint>;
                    Put::object<Annotations, statics, PrettyPrint, IndentLevel, Indent, T>(os, context, obj);
                }
            }
            
            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* Indent, typename Object>
            constexpr void supers(OutStreamType & os, Context & context, const Object & obj)
            {
                Reflect<Object>::Supers::forEach(obj, [&](auto superInfo, auto & superObj) {
                    using SuperInfo = decltype(superInfo);
                    using Super = RareTs::remove_cvref_t<decltype(superObj)>;
                    if constexpr ( !SuperInfo::template hasNote<Json::IgnoreType>() )
                    {
                        if constexpr ( SuperInfo::template hasNote<Json::Name>() )
                        {
                            const auto & superName = superInfo.template getNote<Json::Name>().value;
                            Put::super<Annotations, decltype(superInfo)::index, Super, statics, PrettyPrint, IndentLevel, Indent, Object>(
                                os, context, obj, superName);
                        }
                        else
                        {
                            Put::super<Annotations, decltype(superInfo)::index, Super, statics, PrettyPrint, IndentLevel, Indent, Object>(
                                os, context, obj, superTypeToJsonFieldName<Super>());
                        }
                    }
                });
            }

            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* Indent, typename T>
            constexpr void object(OutStreamType & os, Context & context, const T & obj)
            {
                os << StaticAffix::ObjectPrefix<PrettyPrint, IndentLevel, Indent, statics, T>;
                Put::fields<Annotations, statics, PrettyPrint, IndentLevel+1, Indent, T>(os, context, obj);
                Put::supers<Annotations, statics, PrettyPrint, IndentLevel+1, Indent, T>(os, context, obj);
                os << StaticAffix::ObjectSuffix<PrettyPrint, IndentLevel, Indent, statics, T>;
            }
        }
        
        template <typename Annotations = RareTs::NoNote, Statics statics = Statics::Excluded,
            bool PrettyPrint = false, size_t IndentLevel = 0, const char* Indent = twoSpaces, typename Object = uint_least8_t>
        class ReflectedObject
        {
        public:
            ReflectedObject(const Object & obj, std::shared_ptr<Context> context) : obj(obj), context(context) {}

            const Object & obj;
            std::shared_ptr<Context> context;
            
            constexpr OutStreamType & put(OutStreamType & os)
            {
                if ( context == nullptr )
                    context = std::make_shared<Context>();

                Put::value<Annotations, MockMember<>, statics, PrettyPrint, IndentLevel, Indent, Object, true, Object>(os, *context, obj, obj);

                return os;
            }
        };
        
#ifdef USE_BUFFERED_STREAMS
        template <Statics statics = Statics::Excluded, typename Annotations = RareTs::NoNote, bool PrettyPrint = false,
            size_t IndentLevel = 0, const char* Indent = twoSpaces, typename T = uint_least8_t>
        inline StringBuffer & operator<<(StringBuffer & os, Output::ReflectedObject<Annotations, statics, PrettyPrint, IndentLevel, Indent, T> object)
        {
            return object.put(os);
        }

        template <Statics statics = Statics::Excluded, typename Annotations = RareTs::NoNote, bool PrettyPrint = false,
            size_t IndentLevel = 0, const char* Indent = twoSpaces, typename T = uint_least8_t>
        inline std::ostream & operator<<(StringBufferPtr os, Output::ReflectedObject<Annotations, statics, PrettyPrint, IndentLevel, Indent, T> object)
        {
            return object.put(*os);
        }
#else
        template <Statics statics = Statics::Excluded, typename Annotations = RareTs::NoNote, bool PrettyPrint = false,
            size_t IndentLevel = 0, const char* Indent = twoSpaces, typename T = uint_least8_t>
        inline std::ostream & operator<<(std::ostream & os, Output::ReflectedObject<Annotations, statics, PrettyPrint, IndentLevel, Indent, T> object)
        {
            return object.put(os);
        }
#endif

        template <Statics statics = Statics::Excluded, typename Annotations = RareTs::NoNote,
            size_t IndentLevel = 0, const char* Indent = twoSpaces, typename T = uint_least8_t>
        inline Output::ReflectedObject<Annotations, statics, false, IndentLevel, Indent, T> out(const T & t, std::shared_ptr<Context> context = nullptr)
        {
            return Output::ReflectedObject<Annotations, statics, false, IndentLevel, Indent, T>(t, context);
        }
    
        template <Statics statics = Statics::Excluded, typename OpNotes = RareTs::NoNote,
            size_t IndentLevel = 0, const char* Indent = twoSpaces, typename T = uint_least8_t>
        inline Output::ReflectedObject<OpNotes, statics, true, IndentLevel, Indent, T> pretty(const T & t, std::shared_ptr<Context> context = nullptr)
        {
            return Output::ReflectedObject<OpNotes, statics, true, IndentLevel, Indent, T>(t, context);
        }
    }
    
    inline namespace Input
    {
        inline namespace Customizers
        {
            template <typename Object, typename Value, size_t MemberIndex = noMemberIndex, typename OpNotes = RareTs::NoNote, typename Member = NoMember>
            struct Customize : public RareTs::Unspecialized
            {
                // return false if you wish for the input to be re-parsed by the default JSON code, else return true
                // For invalid input you can throw an exception to end parsing immediately, or return true to continue parsing
                static bool as(std::istream & /*input*/, Context & /*context*/, const Object & /*object*/, Value & /*value*/) { return false; }
            };
            
            template <typename Value, typename OpNotes = RareTs::NoNote, typename Member = NoMember>
            struct CustomizeType : public RareTs::Unspecialized
            {
                // Should return true if you read any input, else you should leave input unchanged
                static bool as(std::istream & /*input*/, Context & /*context*/, Value & /*value*/) { return false; }
            };

            template <typename Object, typename Value, size_t MemberIndex = noMemberIndex, typename OpNotes = RareTs::NoNote, typename Member = NoMember>
            inline constexpr bool haveSpecialization =
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, OpNotes, Member>> || // Fully specialized
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, OpNotes>> || // Customize<4args> specialized
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex>> || // Customize<3args> specialized
                RareTs::is_specialized_v<Customize<Object, Value>> || // Customize<2args> specialized
                RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, RareTs::NoNote, Member>> || // Customize<5args>, OpNotes defaulted
                RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, OpNotes, Member>> || // Customize<5args>, MemberIndex defaulted
                RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, RareTs::NoNote, Member>> || // Customize<5args>, both defaulted
                RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, OpNotes>> || // Customize<4args>, MemberIndex defaulted
                RareTs::is_specialized_v<CustomizeType<Value, OpNotes, Member>> || // Fully specialized
                RareTs::is_specialized_v<CustomizeType<Value, OpNotes>> || // CustomizeType<2args> specialized
                RareTs::is_specialized_v<CustomizeType<Value>> || // CustomizeType<1arg> specialized
                RareTs::is_specialized_v<CustomizeType<Value, RareTs::NoNote, Member>>; // CustomizeType<3args>, OpNotes defaulted
        }

        inline namespace Exceptions
        {
            inline constexpr const char unicodeEscapeSequence[] = "\\uHHHH (where H's are hex characters)";

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

            class FieldNameUnexpectedLineEnding : public UnexpectedLineEnding
            {
            public:
                FieldNameUnexpectedLineEnding(const UnexpectedLineEnding & e) : UnexpectedLineEnding(
                    (std::string("Expected field name close quote, found line ending (\"") + e.what() + "\")").c_str()) {}
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

            class InvalidUnknownFieldValue : public Exception
            {
            public:
                InvalidUnknownFieldValue() : Exception("Expected field value (string, number, object, array, true, false, or null)") {}
            };

            class ArraySizeExceeded : public Exception
            {
            public:
                ArraySizeExceeded() : Exception("Array size exceeded!") {}
            };
        }

        inline namespace Cache
        {
            inline std::hash<std::string> strHash;

            inline std::map<std::type_index, std::multimap<size_t, JsonField>> classToNameHashToJsonField;
            
            template <typename T>
            inline std::multimap<size_t, JsonField> & getClassFieldCache()
            {
                using Members = typename Reflect<T>::Members;
                using Supers = typename Reflect<T>::Supers;
                auto found = classToNameHashToJsonField.find(std::type_index(typeid(T)));
                if ( found == classToNameHashToJsonField.end() )
                {
                    auto inserted = classToNameHashToJsonField.insert(std::pair<std::type_index, std::multimap<size_t, JsonField>>(
                        std::type_index(typeid(T)), std::multimap<size_t, JsonField>()));

                    if ( !inserted.second )
                        throw Exception("Failed to create cache for class!");

                    if constexpr ( Members::total > 0 )
                    {
                        for ( size_t memberIndex = 0; memberIndex < Members::total; memberIndex++ )
                        {
                            Members::at(memberIndex, [&](auto & member) {
                                using Member = std::remove_reference_t<decltype(member)>;
                                if constexpr ( std::is_base_of_v<Generic::FieldCluster, RareTs::remove_pointer_t<typename Member::type>> )
                                {
                                    std::string fieldName = fieldClusterToJsonFieldName();
                                    inserted.first->second.insert(std::pair<size_t, JsonField>(
                                        strHash(fieldName), JsonField(memberIndex, JsonField::Type::FieldCluster, fieldName)));
                                }
                                else if constexpr ( !Member::template hasNote<Json::IgnoreType>() )
                                {
                                    if constexpr ( Member::template hasNote<Json::Name>() )
                                    {
                                        std::string fieldName = std::string(member.template getNote<Json::Name>().value);
                                        inserted.first->second.insert(std::pair<size_t, JsonField>(
                                            strHash(fieldName.c_str()), JsonField(memberIndex, JsonField::Type::Regular, fieldName.c_str())));
                                    }
                                    else
                                    {
                                        Reflect<T>::Members::at(memberIndex, [&](auto & member) {
                                            inserted.first->second.insert(std::pair<size_t, JsonField>(
                                                strHash(member.name),
                                                JsonField(memberIndex, JsonField::Type::Regular, member.name)));
                                        });
                                    }
                                }
                            });
                        }
                    }

                    if constexpr ( Supers::total > 0 )
                    {
                        RareTs::forIndexes<Supers::total>([&](auto I) {
                            using SuperInfo = typename Supers::template SuperInfo<decltype(I)::value>;
                            using Super = typename SuperInfo::type;
                            if constexpr ( hasFields<Statics::Included, Super>() )
                            {
                                if constexpr ( SuperInfo::template hasNote<Json::Name>() )
                                {
                                    std::string superName = std::string(SuperInfo::template getNote<Json::Name>().value);
                                    inserted.first->second.insert(std::pair<size_t, JsonField>(
                                        strHash(superName), JsonField(decltype(I)::value, JsonField::Type::SuperClass, superName)));
                                }
                                else
                                {
                                    std::string superTypeFieldName = superTypeToJsonFieldName<Super>();
                                    inserted.first->second.insert(std::pair<size_t, JsonField>(
                                        strHash(superTypeFieldName), JsonField(decltype(I)::value, JsonField::Type::SuperClass, superTypeFieldName)));
                                }
                            }
                        });
                    }
                            
                    return inserted.first->second;
                }
                return found->second;
            }
            
            template <typename T>
            inline JsonField* getJsonField(const std::string & fieldName)
            {
                std::multimap<size_t, JsonField> & fieldNameToJsonField = getClassFieldCache<T>();
                size_t fieldNameHash = strHash(fieldName);
                auto fieldHashMatches = fieldNameToJsonField.equal_range(fieldNameHash);
                for ( auto it = fieldHashMatches.first; it != fieldHashMatches.second; ++it )
                {
                    if ( it->second.name.compare(fieldName) == 0 )
                        return &it->second;
                }
                return nullptr;
            }

            inline void putClassFieldCache(std::ostream & os)
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
                            << (fieldNameToJsonField.second.type == JsonField::Type::Regular ? "\"Regular\"" :
                                (fieldNameToJsonField.second.type == JsonField::Type::SuperClass ? "\"SuperClass\"" : "\"FieldCluster\""))
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
        }

        class Checked
        {
        public:
            static inline void peek(std::istream & is, char & c, const char* expectedDescription)
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
            
            static inline bool tryGet(std::istream & is, int character, const char* expectedDescription)
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
            static constexpr bool tryGet(std::istream & is, int character, int secondaryCharacter,
                const char* expectedDescription, const char* secondaryDescription)
            {
                if ( usePrimary )
                    return tryGet(is, character, expectedDescription);
                else
                    return tryGet(is, secondaryCharacter, secondaryDescription);
            }

            static inline bool getTrueFalse(std::istream & is, char trueChar, char falseChar, const char* expectedDescription)
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
            
            template <bool usePrimary>
            static constexpr bool getTrueFalse(std::istream & is, char trueChar, char falseChar, char secondaryFalseChar,
                const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    return getTrueFalse(is, trueChar, falseChar, expectedDescription);
                else
                    return getTrueFalse(is, trueChar, secondaryFalseChar, secondaryDescription);
            }

            template <bool IgnoreWhitespace = true>
            static inline void get(std::istream & is, char & c, char expectation, const char* expectedDescription)
            {
                if constexpr ( IgnoreWhitespace )
                    is >> c;
                else
                    is.get(c);

                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd(expectedDescription);
                    else
                        throw StreamReadFail();
                }
                else if ( c != expectation )
                    throw Exception((std::string("Expected: ") + expectedDescription).c_str());
            }

            template <bool IgnoreWhitespace = true>
            static inline void get(std::istream & is, char & c, const char* expectedDescription)
            {
                if constexpr ( IgnoreWhitespace )
                    is >> c;
                else
                    is.get(c);

                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd(expectedDescription);
                    else
                        throw StreamReadFail();
                }
            }

            template <bool IgnoreWhitespace = true>
            static inline bool tryGetEnd(std::istream& is, char& c)
            {
                if constexpr ( IgnoreWhitespace )
                    is >> c;
                else
                    is.get(c);

                if ( !is.good() )
                {
                    if ( is.eof() )
                        return true;
                    else
                        throw StreamReadFail();
                }
                return false;
            }

            template <bool usePrimary>
            static constexpr void get(std::istream & is, char & c, const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    get(is, c, expectedDescription);
                else
                    get(is, c, secondaryDescription);
            }

            template <bool usePrimary>
            static constexpr void get(std::istream & is, char & c, char expectation, char secondaryExpectation, const char* expectedDescription,
                const char* secondaryDescription)
            {
                is >> c;
                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd(expectedDescription);
                    else
                        throw StreamReadFail();
                }
                else if constexpr ( usePrimary )
                {
                    if ( c != expectation )
                        throw Exception((std::string("Expected: ") + expectedDescription).c_str());
                }
                else
                {
                    if ( c != secondaryExpectation )
                        throw Exception((std::string("Expected: ") + secondaryDescription).c_str());
                }
            }

            static inline bool unget(std::istream & is, char ungetting)
            {
                is.unget();
                if ( !is.good() )
                    throw StreamUngetFail(ungetting);

                return true;
            }

            static inline void escapeSequenceGet(std::istream & is, char & c, const char* hexEscapeSequence)
            {
                is.get(c);
                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw InvalidEscapeSequence((std::string(hexEscapeSequence)).c_str(), unicodeEscapeSequence);
                    else
                        throw StreamReadFail();
                }
            }

            static inline void consumeWhitespace(std::istream & is, const char* expectedDescription)
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
            static constexpr void consumeWhitespace(std::istream & is, const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    consumeWhitespace(is, expectedDescription);
                else
                    consumeWhitespace(is, secondaryDescription);
            }
        };

        namespace Consume
        {
            template <bool InArray>
            constexpr void null(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                int expectation[] = { 'n', 'u', 'l', 'l' };
                for ( size_t i=0; i<4; i++ )
                    Checked::get<false>(is, c, static_cast<char>(expectation[i]), "completion of field value");

                Checked::consumeWhitespace(is, "\",\" or \"}\"");
                Checked::peek(is, c, "\",\" or \"}\"");
                if constexpr ( InArray )
                {
                    if ( c != ',' && c != ']' )
                        throw Exception("Expected: \",\" or \"]\"");
                }
                else if ( c != ',' && c != '}' )
                    throw Exception("Expected: \",\" or \"}\"");
            }

            template <bool InArray>
            constexpr void null(std::istream & is, char & c, std::stringstream & ss)
            {
                Consume::null<InArray>(is, c);
                ss << "null";
            }

            template <bool InArray>
            constexpr bool tryNull(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "null or field value");
                Checked::peek(is, c, "null or field value");
                if ( c == 'n' )
                {
                    int expectation[] = { 'n', 'u', 'l', 'l' };
                    for ( size_t i=0; i<4; i++ )
                        Checked::get<false>(is, c, static_cast<char>(expectation[i]), "completion of \"null\"");

                    Checked::consumeWhitespace(is, "\",\" or \"}\"");
                    Checked::peek(is, c, "\",\" or \"}\"");
                    if constexpr ( InArray )
                    {
                        if ( c != ',' && c != ']' )
                            throw Exception("Expected: \",\" or \"]\"");
                    }
                    else if ( c != ',' && c != '}' )
                        throw Exception("Expected: \",\" or \"}\"");
                
                    return true;
                }
                return false;
            }
            
            template <bool InArray>
            constexpr void boolTrue(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                int expectation[] = { 't', 'r', 'u', 'e' };
                for ( size_t i=0; i<4; i++ )
                    Checked::get<false>(is, c, static_cast<char>(expectation[i]), "completion of field value");

                Checked::consumeWhitespace(is, "\",\" or \"}\"");
                Checked::peek(is, c, "\",\" or \"}\"");
                if constexpr ( InArray )
                {
                    if ( c != ',' && c != ']' )
                        throw Exception("Expected: \",\" or \"]\"");
                }
                else if (c != ',' && c != '}' )
                    throw Exception("Expected: \",\" or \"}\"");
            }

            template <bool InArray>
            constexpr void boolTrue(std::istream & is, char & c, std::stringstream & ss)
            {
                Consume::boolTrue<InArray>(is, c);
                ss << "true";
            }
            
            template <bool InArray>
            constexpr void boolFalse(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                int expectation[] = { 'f', 'a', 'l', 's', 'e' };
                for ( size_t i=0; i<5; i++ )
                    Checked::get<false>(is, c, static_cast<char>(expectation[i]), "completion of field value");

                Checked::consumeWhitespace(is, "\",\" or \"}\"");
                Checked::peek(is, c, "\",\" or \"}\"");
                if constexpr ( InArray )
                {
                    if ( c != ',' && c != ']' )
                        throw Exception("Expected: \",\" or \"]\"");
                }
                else if ( c != ',' && c != '}' )
                    throw Exception("Expected: \",\" or \"}\"");
            }

            template <bool InArray>
            constexpr void boolFalse(std::istream & is, char & c, std::stringstream & ss)
            {
                Consume::boolFalse<InArray>(is, c);
                ss << "false";
            }

            template <bool InArray>
            constexpr void number(std::istream & is, char & c)
            {
                bool decimal = false;
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

                            if constexpr ( InArray )
                            {
                                if ( c != ',' && c != ']' )
                                    throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"]\"" : "\".\", [0-9], \",\", or \"]\""));
                            }
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
            
            template <bool InArray>
            constexpr void number(std::istream & is, char & c, std::stringstream & ss)
            {
                bool decimal = false;
                Checked::get(is, c, "\"-\" or [0-9]");
                switch ( c )
                {
                    case '-': case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9': break;
                    default: throw InvalidNumericCharacter(c, "\"-\" or [0-9]");
                }
                ss.put(c);
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
                            {
                                decimal = true;
                                ss.put(c);
                            }
                            break;
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            ss.put(c);
                            break;
                        case ',': Checked::unget(is, ','); break;
                        case terminator: Checked::unget(is, terminator); break;
                        case ' ': case '\t': case '\n': case '\r':
                            Checked::consumeWhitespace<InArray>(is, decimal ? "[0-9], \",\", or \"]\"" : "\".\", [0-9], \",\", or \"]\"",
                                decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\"");
                            Checked::get<InArray>(is, c, decimal ? "[0-9], \",\", or \"]\"" : "\".\", [0-9], \",\", or \"]\"",
                                decimal ? "[0-9], \",\", or \"}\"" : "\".\", [0-9], \",\", or \"}\"");

                            if constexpr ( InArray )
                            {
                                if ( c != ',' && c != ']' )
                                    throw InvalidNumericCharacter(c, (decimal ? "[0-9], \",\", or \"]\"" : "\".\", [0-9], \",\", or \"]\""));
                            }
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

            inline void string(std::istream & is, char & c)
            {
                Checked::get(is, c, '\"', "string value open quote");
                do
                {
                    Checked::get<false>(is, c, "string value close quote");
                    switch ( c )
                    {
                        case '\\': // Escape sequence
                            Checked::get<false>(is, c, "completion of string escape sequence");
                            switch ( c )
                            {
                                case '\"': c = '\0'; break;
                                case '\\': case '/': case 'b': case 'f': case 'n': case 'r': case 't': break;
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
                    }
                } while ( c != '\"' );
            }

            inline void string(std::istream & is, char & c, std::stringstream & ss)
            {
                Checked::get(is, c, '\"', "string value open quote");
                ss.put('\"');
                do
                {
                    Checked::get<false>(is, c, "string value close quote");
                    ss.put(c);
                    switch ( c )
                    {
                        case '\\': // Escape sequence
                            Checked::get<false>(is, c, "completion of string escape sequence");
                            ss.put(c);
                            switch ( c )
                            {
                                case '\"': c = '\0'; break;
                                case '\\': break;
                                case '/': break;
                                case 'b': break;
                                case 'f': break;
                                case 'n': break;
                                case 'r': break;
                                case 't': break;
                                case 'u':
                                {
                                    char hexEscapeSequence[6] = { 'u', '\0', '\0', '\0', '\0', '\0' };
                                    for ( size_t i=1; i<5; i++ )
                                    {
                                        Checked::escapeSequenceGet(is, c, hexEscapeSequence);
                                        ss.put(c);
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
                    }
                } while ( c != '\"' );
            }
            
            template <bool IsArray>
            constexpr void iterable(std::istream & is, char & c);
            
            template <bool IsArray>
            constexpr void iterable(std::istream & is, char & c, std::stringstream & ss);

            template <bool InArray>
            constexpr void value(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                switch ( c )
                {
                    case '\"': Consume::string(is, c); break; // String or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9': Consume::number<InArray>(is, c); break; // Number or error
                    case '{': Consume::iterable<false>(is, c); break; // JSON object or error
                    case '[': Consume::iterable<true>(is, c); break; // JSON array or error
                    case 't': Consume::boolTrue<InArray>(is, c); break; // "true" or error
                    case 'f': Consume::boolFalse<InArray>(is, c); break; // "false" or error
                    case 'n': Consume::null<InArray>(is, c); break; // "null" or error
                    default: throw InvalidUnknownFieldValue(); break;
                }
            }
            
            template <bool InArray>
            constexpr void value(std::istream & is, char & c, std::stringstream & ss)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                switch ( c )
                {
                    case '\"': Consume::string(is, c, ss); break; // String or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9': Consume::number<InArray>(is, c, ss); break; // Number or error
                    case '{': Consume::iterable<false>(is, c, ss); break; // JSON object or error
                    case '[': Consume::iterable<true>(is, c, ss); break; // JSON array or error
                    case 't': Consume::boolTrue<InArray>(is, c, ss); break; // "true" or error
                    case 'f': Consume::boolFalse<InArray>(is, c, ss); break; // "false" or error
                    case 'n': Consume::null<InArray>(is, c, ss); break; // "null" or error
                    default: throw InvalidUnknownFieldValue(); break;
                }
            }

            template <bool IsArray>
            constexpr void iterable(std::istream & is, char & c)
            {
                Checked::get<IsArray>(is, c, '[', '{', "array opening \"[\"", "object opening \"{\"");
                if ( !Checked::tryGet<IsArray>(is, ']', '}', "array closing \"]\" or array element", "object closing \"}\" or field name opening \"") )
                {
                    do
                    {
                        if constexpr ( !IsArray )
                        {
                            Consume::string(is, c);
                            Checked::get(is, c, ':', "field name-value separator \":\"");
                        }
                        Consume::value<IsArray>(is, c);
                    }
                    while ( Checked::getTrueFalse<IsArray>(is, ',', ']', '}', "\",\" or array closing \"]\"", "\",\" or object closing \"}\"") );
                }
            }

            template <bool IsArray>
            constexpr void iterable(std::istream & is, char & c, std::stringstream & ss)
            {
                Checked::get<IsArray>(is, c, '[', '{', "array opening \"[\"", "object opening \"{\"");
                ss << (IsArray ? '[' : '{');
                if ( !Checked::tryGet<IsArray>(is, ']', '}', "array closing \"]\" or array element", "object closing \"}\" or field name opening \"") )
                {
                    bool isFirst = true;
                    do
                    {
                        if ( !isFirst )
                            ss << ',';
                        else
                            isFirst = false;

                        if constexpr ( !IsArray )
                        {
                            Consume::string(is, c, ss);
                            Checked::get(is, c, ':', "field name-value separator \":\"");
                            ss << ':';
                        }
                        Consume::value<IsArray>(is, c, ss);
                    }
                    while ( Checked::getTrueFalse<IsArray>(is, ',', ']', '}', "\",\" or array closing \"]\"", "\",\" or object closing \"}\"") );
                }
                ss << (IsArray ? ']' : '}');
            }
        }

        namespace Read
        {
            inline namespace Affix
            {
                inline void objectPrefix(std::istream & is, char & c)
                {
                    Checked::get(is, c, '{', "object opening \"{\"");
                }

                inline bool tryObjectSuffix(std::istream & is)
                {
                    return Checked::tryGet(is, '}', "object closing \"}\" or field name opening \"");
                }

                inline bool fieldSeparator(std::istream & is)
                {
                    return Checked::getTrueFalse(is, ',', '}', "\",\" or object closing \"}\"");
                }

                inline void fieldNameValueSeparator(std::istream & is, char & c)
                {
                    Checked::get(is, c, ':', "field name-value separator \":\"");
                }

                inline void arrayPrefix(std::istream & is, char & c)
                {
                    Checked::get(is, c, '[', "array opening \"[\"");
                }

                inline bool trySingularTupleArrayPrefix(std::istream & is, char &)
                {
                    return Checked::tryGet(is, '[', "array opening \"[\" or tuple value");
                }

                inline bool tryArraySuffix(std::istream & is)
                {
                    return Checked::tryGet(is, ']', "array closing \"]\" or array element");
                }

                template <bool IsObject>
                inline void iterablePrefix(std::istream & is, char & c)
                {
                    Checked::get<IsObject>(is, c, '{', '[', "object opening \"{\"", "array opening \"[\"");
                }

                inline bool iterablePrefix(std::istream & is, char & c)
                {
                    if ( Checked::tryGet(is, '{', "object opening \"{\" or array opening \"[\"") )
                        return true;

                    Checked::get<true>(is, c, '[', "object opening \"{\" or array opening \"[\"");
                    return false;
                }

                inline bool peekIterablePrefix(std::istream & is, char & c)
                {
                    Checked::peek(is, c, "object opening \"{\" or array opening \"[\"");
                    if ( c == '{' )
                        return true;
                    else if ( c == '[' )
                        return false;
                    else
                        throw Exception(std::string("Expected: object opening \"{\" or array opening \"[\"").c_str());
                }

                template <bool IsObject>
                inline bool tryIterableSuffix(std::istream & is)
                {
                    return Checked::tryGet<IsObject>(is, '}', ']', "object closing \"}\" or field name opening \"", "array closing \"]\" or array element");
                }

                inline bool tryIterableSuffix(std::istream & is, bool isObject)
                {
                    if ( isObject )
                        return Checked::tryGet(is, '}', "object closing \"}\" or field name opening \"");
                    else
                        return Checked::tryGet(is, ']', "array closing \"]\" or array element");
                }

                template <bool IsObject>
                inline bool iterableElementSeparator(std::istream & is)
                {
                    return Checked::getTrueFalse<IsObject>(is, ',', '}', ']', "\",\" or object closing \"}\"", "\",\" or array closing \"]\"");
                }
            }

            template <typename Object, typename Value, size_t MemberIndex, typename OpAnnotations, typename Member>
            constexpr bool customization(std::istream & is, Context & context, Object & obj, Value & value)
            {
                if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, OpAnnotations, Member>> )
                    return Customize<Object, Value, MemberIndex, OpAnnotations, Member>::as(is, context, obj, value); // Customize fully specialized
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, RareTs::NoNote, Member>> )
                    return Customize<Object, Value, MemberIndex, RareTs::NoNote, Member>::as(is, context, obj, value); // Customize<5args>, Notes defaulted
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, OpAnnotations, Member>> )
                    return Customize<Object, Value, noMemberIndex, OpAnnotations, Member>::as(is, context, obj, value); // Customize<5args>, MemberIndex defaulted
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, RareTs::NoNote, Member>> )
                    return Customize<Object, Value, noMemberIndex, RareTs::NoNote, Member>::as(is, context, obj, value); // Customize<5args>, both default
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, MemberIndex, OpAnnotations>> )
                    return Customize<Object, Value, MemberIndex, OpAnnotations>::as(is, context, obj, value); // Four Customize arguments specialized
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, noMemberIndex, OpAnnotations>> )
                    return Customize<Object, Value, noMemberIndex, OpAnnotations>::as(is, context, obj, value); // Customize<4args>, MemberIndex defaulted
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value, MemberIndex>> )
                    return Customize<Object, Value, MemberIndex>::as(is, context, obj, value); // Three Customize arguments specialized
                else if constexpr ( RareTs::is_specialized_v<Customize<Object, Value>> )
                    return Customize<Object, Value>::as(is, context, obj, value); // Two Customize arguments specialized
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value, OpAnnotations, Member>> )
                    return CustomizeType<Value, OpAnnotations, Member>::as(is, context, value); // CustomizeType fully specialized
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value, RareTs::NoNote, Member>> )
                    return CustomizeType<Value, RareTs::NoNote, Member>::as(is, context, value); // CustomizeType<3args>, OpAnnotations defaulted
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value, OpAnnotations>> )
                    return CustomizeType<Value, OpAnnotations>::as(is, context, value); // CustomizeType<2args> specialized
                else if constexpr ( RareTs::is_specialized_v<CustomizeType<Value>> )
                    return CustomizeType<Value>::as(is, context, value); // CustomizeType<1arg> specialized
                else
                    return false;
            }

            template <bool InArray>
            constexpr bool boolTrue(std::istream & is, char & c)
            {
                Consume::boolTrue<InArray>(is, c);
                return true;
            }

            template <bool InArray>
            constexpr bool boolFalse(std::istream & is, char & c)
            {
                Consume::boolFalse<InArray>(is, c);
                return false;
            }
            
            template <bool InArray, typename Value>
            constexpr void boolean(std::istream & is, char & c, Value & value)
            {
                Checked::consumeWhitespace(is, "true or false");
                Checked::peek(is, c, "true or false");
                if ( c == 't' )
                {
                    Consume::boolTrue<InArray>(is, c);
                    if constexpr ( !std::is_const_v<Value> )
                        value = true;
                }
                else if ( c == 'f' )
                {
                    Consume::boolFalse<InArray>(is, c);
                    if constexpr ( !std::is_const_v<Value> )
                        value = false;
                }
                else
                    throw Exception("Expected: \"true\" or \"false\"");
            }

            template <bool InArray>
            constexpr bool boolean(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "true or false");
                Checked::peek(is, c, "true or false");
                if ( c == 't' )
                {
                    Consume::boolTrue<InArray>(is, c);
                    return true;
                }
                else if ( c == 'f' )
                {
                    Consume::boolFalse<InArray>(is, c);
                    return false;
                }
                else
                    throw Exception("Expected: \"true\" or \"false\"");
            }

            template <bool InArray>
            inline std::string number(std::istream & is, char & c)
            {
                std::stringstream ss;
                Consume::value<InArray>(is, c, ss);
                return ss.str();
            }

            template <bool ExpectQuotes = true>
            inline void string(std::istream & is, char & c, std::stringstream & ss)
            {
                if constexpr ( ExpectQuotes )
                    Checked::get(is, c, '\"', "string value open quote");
                do
                {
                    if constexpr ( ExpectQuotes )
                        Checked::get<false>(is, c, "string value close quote");
                    else if ( Checked::tryGetEnd<false>(is, c) )
                        return;

                    switch ( c )
                    {
                        case '\\': // Escape sequence
                            Checked::get<false>(is, c, "completion of string escape sequence");
                            switch ( c )
                            {
                                case '\"': ss.put('\"'); c = '\0'; break;
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
                                    char highCharacter = char(0x10 * hexEscapeSequence[1] + hexEscapeSequence[2]);
                                    char lowCharacter = char(0x10 * hexEscapeSequence[3] + hexEscapeSequence[4]);
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
                } while ( c != '\"' && !is.eof() );
            }

            template <bool ExpectQuotes = true>
            inline void string(std::istream & is, char & c, std::string & str)
            {
                std::stringstream ss;
                Read::string<ExpectQuotes>(is, c, ss);
                str = ss.str();
            }

            template <typename T, bool ExpectQuotes = true>
            inline void string(std::istream & is, char & c, T & t)
            {
                std::stringstream ss;
                Read::string<ExpectQuotes>(is, c, ss);
                if constexpr ( !std::is_const_v<T> )
                {
                    if constexpr ( std::is_same_v<RareTs::remove_cvref_t<T>, std::string> )
                        t.assign(std::istreambuf_iterator<char>(ss), {});
                    else
                        ss >> t;
                }
            }
        
            template <bool ExpectQuotes = true>
            inline std::string string(std::istream & is, char & c)
            {
                std::string str;
                Read::string<ExpectQuotes>(is, c, str);
                return str;
            }
        
            template <bool ExpectQuotes = true>
            inline std::string string(std::istream & is)
            {
                char c = '\0';
                std::string str;
                Read::string<ExpectQuotes>(is, c, str);
                return str;
            }

            template <typename Value>
            constexpr void enumInt(std::istream & is, Value & value)
            {
                using EnumType = RareTs::promote_char_t<typename std::underlying_type_t<RareTs::remove_pointer_t<Value>>>;
                std::remove_const_t<EnumType> temp;
                is >> temp;
                if constexpr ( !std::is_const_v<EnumType> )
                    value = (RareTs::remove_pointer_t<Value>)temp;
            }
            
            inline std::string fieldName(std::istream & is, char & c)
            {
                std::string fieldName;
                try {
                    Read::string<>(is, c, fieldName);
                } catch ( UnexpectedLineEnding & e) {
                    throw FieldNameUnexpectedLineEnding(e);
                }
                return fieldName;
            }

            inline std::shared_ptr<Generic::Value::Assigner> genericObject(std::istream & is, Context & context, char & c);

            template <bool InArray>
            inline std::shared_ptr<Generic::Value::Assigner> genericArray(std::istream & is, Context & context, char & c);

            template <bool InArray>
            inline std::shared_ptr<Generic::Value::Assigner> genericValue(std::istream & is, Context & context, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                switch ( c )
                {
                    case '\"': return Generic::Value::Assigner::make<Generic::String>(Read::string<>(is, c)); // String or error
                    case '{': return Read::genericObject(is, context, c); // JSON object or error
                    case '[': return Read::genericArray<InArray>(is, context, c); // JSON array or error
                    case 't': return Generic::Value::Assigner::make<Generic::Bool>(Read::boolTrue<InArray>(is, c)); // "true" or error
                    case 'f': return Generic::Value::Assigner::make<Generic::Bool>(Read::boolFalse<InArray>(is, c)); // "false" or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9':
                        return Generic::Value::Assigner::make<Generic::Number>(Read::number<InArray>(is, c)); // Number or error
                    case 'n':
                        Consume::null<InArray>(is, c);
                        return Generic::Value::Assigner::make(nullptr); // "null" or error
                    default:
                        throw InvalidUnknownFieldValue();
                }
            }

            constexpr Generic::Value::Type valueType(const char & c)
            {
                switch ( c )
                {
                    case '\"': return Generic::Value::Type::String;
                    case '{': return Generic::Value::Type::Object;
                    case '[': return Generic::Value::Type::Array;
                    case 't': case 'f': return Generic::Value::Type::Boolean;
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9':
                        return Generic::Value::Type::Number;
                    case 'n': return Generic::Value::Type::Null;
                    default: throw InvalidUnknownFieldValue();
                }
            }

            template <bool InArray>
            inline std::shared_ptr<Generic::Value::Assigner> genericArray(std::istream & is, Context & context, char & c)
            {
                Read::arrayPrefix(is, c);
                if ( Read::tryArraySuffix(is) )
                    return Generic::Value::Assigner::make<Generic::NullArray>();

                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                
                Generic::Value::Type arrayElementType = Read::valueType(c);

                std::shared_ptr<Generic::Value::Assigner> result = nullptr;
                switch ( arrayElementType )
                {
                    case Generic::Value::Type::Null: result = Generic::Value::Assigner::make<Generic::NullArray>(); break;
                    case Generic::Value::Type::Boolean: result = Generic::Value::Assigner::make<Generic::BoolArray>(); break;
                    case Generic::Value::Type::Number: result = Generic::Value::Assigner::make<Generic::NumberArray>(); break;
                    case Generic::Value::Type::String: result = Generic::Value::Assigner::make<Generic::StringArray>(); break;
                    case Generic::Value::Type::Object: result = Generic::Value::Assigner::make<Generic::ObjectArray>(); break;
                    case Generic::Value::Type::Array: result = Generic::Value::Assigner::make<Generic::MixedArray>(); break;
                    case Generic::Value::Type::None: case Generic::Value::Type::NullArray: case Generic::Value::Type::BoolArray:
                    case Generic::Value::Type::NumberArray: case Generic::Value::Type::StringArray: case Generic::Value::Type::ObjectArray:
                    case Generic::Value::Type::MixedArray: case Generic::Value::Type::FieldCluster: break; // Unused cases
                }

                Generic::Value::Type elementType = Generic::Value::Type::None;
                do
                {
                    Checked::consumeWhitespace(is, "completion of field value");
                    Checked::peek(is, c, "completion of field value");
                    elementType = Read::valueType(c);

                    if ( elementType != arrayElementType && arrayElementType != Generic::Value::Type::Array ) // Promote to mixed array
                    {
                        std::shared_ptr<Generic::Value::Assigner> newResult = Generic::Value::Assigner::make<Generic::MixedArray>();
                        std::vector<std::shared_ptr<Generic::Value>> & mixedArray = newResult->get()->mixedArray();
                        switch ( arrayElementType )
                        {
                            case Generic::Value::Type::Null:
                            {
                                for ( size_t i=0; i<result->get()->nullArray(); i++ )
                                    mixedArray.push_back(nullptr);
                            }
                            break;
                            case Generic::Value::Type::Boolean:
                            {
                                const std::vector<bool> & boolArray = result->get()->boolArray();
                                for ( size_t i=0; i<boolArray.size(); i++ )
                                    mixedArray.push_back(std::make_shared<Generic::Bool>(boolArray[i]));
                            }
                            break;
                            case Generic::Value::Type::Number:
                            {
                                const std::vector<std::string> & numberArray = result->get()->numberArray();
                                for ( size_t i=0; i<numberArray.size(); i++ )
                                    mixedArray.push_back(std::make_shared<Generic::Number>(numberArray[i]));
                            }
                            break;
                            case Generic::Value::Type::String:
                            {
                                const std::vector<std::string> & stringArray = result->get()->stringArray();
                                for ( size_t i=0; i<stringArray.size(); i++ )
                                    mixedArray.push_back(std::make_shared<Generic::String>(stringArray[i]));
                            }
                            break;
                            case Generic::Value::Type::Object:
                            {
                                const std::vector<std::map<std::string, std::shared_ptr<Generic::Value>>> & objectArray = result->get()->objectArray();
                                for ( size_t i=0; i<objectArray.size(); i++ )
                                    mixedArray.push_back(std::make_shared<Generic::Object>(objectArray[i]));
                            }
                            break;
                            case Generic::Value::Type::None: case Generic::Value::Type::Array: case Generic::Value::Type::NullArray: // Unused cases
                            case Generic::Value::Type::BoolArray: case Generic::Value::Type::NumberArray: case Generic::Value::Type::StringArray:
                            case Generic::Value::Type::ObjectArray: case Generic::Value::Type::MixedArray: case Generic::Value::Type::FieldCluster: break;
                        }
                        arrayElementType = Generic::Value::Type::Array;
                        result = newResult;
                    }

                    if ( arrayElementType == Generic::Value::Type::Array )
                    {
                        switch ( elementType )
                        {
                            case Generic::Value::Type::Null: Consume::null<true>(is, c); result->get()->mixedArray().push_back(nullptr); break;
                            case Generic::Value::Type::Boolean:
                                result->get()->mixedArray().push_back(std::make_shared<Generic::Bool>(Read::boolean<true>(is, c))); break;
                            case Generic::Value::Type::Number:
                                result->get()->mixedArray().push_back(std::make_shared<Generic::Number>(Read::number<true>(is, c))); break;
                            case Generic::Value::Type::String:
                                result->get()->mixedArray().push_back(std::make_shared<Generic::String>(Read::string<>(is, c))); break;
                            case Generic::Value::Type::Object: result->get()->mixedArray().push_back(Read::genericObject(is, context, c)->out()); break;
                            case Generic::Value::Type::Array: result->get()->mixedArray().push_back(Read::genericArray<true>(is, context, c)->out()); break;
                            case Generic::Value::Type::None: case Generic::Value::Type::NullArray: // Unused cases
                            case Generic::Value::Type::BoolArray: case Generic::Value::Type::NumberArray: case Generic::Value::Type::StringArray:
                            case Generic::Value::Type::ObjectArray: case Generic::Value::Type::MixedArray: case Generic::Value::Type::FieldCluster: break;
                        }
                    }
                    else
                    {
                        switch ( elementType )
                        {
                            case Generic::Value::Type::Null: Consume::null<true>(is, c); result->get()->nullArray()++; break;
                            case Generic::Value::Type::Boolean: result->get()->boolArray().push_back(Read::boolean<true>(is, c)); break;
                            case Generic::Value::Type::Number: result->get()->numberArray().push_back(Read::number<true>(is, c)); break;
                            case Generic::Value::Type::String: result->get()->stringArray().push_back(Read::string<>(is, c)); break;
                            case Generic::Value::Type::Object: result->get()->objectArray().push_back(Read::genericObject(is, context, c)->out()->object());
                                break;
                            case Generic::Value::Type::Array: result->get()->mixedArray().push_back(Read::genericArray<true>(is, context, c)->out()); break;
                            case Generic::Value::Type::None: case Generic::Value::Type::NullArray: // Unused cases
                            case Generic::Value::Type::BoolArray: case Generic::Value::Type::NumberArray: case Generic::Value::Type::StringArray:
                            case Generic::Value::Type::ObjectArray: case Generic::Value::Type::MixedArray: case Generic::Value::Type::FieldCluster: break;
                        }
                    }
                }
                while ( Read::iterableElementSeparator<false>(is) );
                    
                return result;
            }
            
            inline std::shared_ptr<Generic::Value::Assigner> genericObject(std::istream & is, Context & context, char & c)
            {
                std::shared_ptr<Generic::Value::Assigner> result = std::make_shared<Generic::Value::Assigner>(std::make_unique<Generic::Object>());
                auto & obj = result->get()->object();

                Read::objectPrefix(is, c);
                if ( !Read::tryObjectSuffix(is) )
                {
                    do
                    {
                        std::string fieldName = Read::fieldName(is, c);
                        Read::fieldNameValueSeparator(is, c);
                        std::shared_ptr<Generic::Value> value = Read::genericValue<false>(is, context, c)->out();
                        obj.insert(std::pair<std::string, std::shared_ptr<Generic::Value>>(fieldName, value));
                    }
                    while ( Read::fieldSeparator(is) );
                }
                return result;
            }
            
            template <typename Annotations, typename Member, size_t TupleIndex = 0, typename Object, typename T1, typename T2, typename ...Ts>
            constexpr void tuple(std::istream & is, Context & context, char & c, Object & object, std::tuple<T1, T2, Ts...> & value);

            template <typename Annotations, typename Member, typename Object, typename Key, typename T>
            constexpr void pair(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value);

            template <typename Annotations, typename Member, typename T, typename Object>
            constexpr void iterable(std::istream & is, Context & context, char & c, Object & object, T & iterable);

            template <typename Annotations, typename T>
            inline void object(std::istream & is, Context & context, char & c, T & t);

            template <typename Annotations, bool InArray, typename Member, typename T, typename Object, bool AllowCustomization = true, bool ExpectQuotes = true>
            inline void value(std::istream & is, Context & context, char & c, Object & object, T & value)
            {
                if constexpr ( AllowCustomization && Customizers::haveSpecialization<Object, T, Member::index, Annotations, Member> ) // Input is specialized
                {
                    std::stringstream ss;
                    Json::Consume::value<InArray>(is, c, ss);
                    std::string preserved = ss.str();
                    if ( !Read::customization<Object, T, Member::index, Annotations, Member>(ss, context, object, value) )
                    {
                        std::stringstream subIs(preserved);
                        Read::value<Annotations, InArray, Member, T, Object, false>(subIs, context, c, object, value);
                    }
                    return;
                }

                if constexpr ( RareMapper::has_default_mapping_v<T, typename Member::Notes, Annotations> )
                {
                    using D = RareMapper::default_mapping_t<T, typename Member::Notes, Annotations>;
                    D d {};
                    Read::value<Annotations, InArray, MockMember<D>>(is, context, c, object, d);
                    RareMapper::map(value, d);
                }
                else if constexpr ( RareTs::is_pointable_v<T> )
                {
                    using Dereferenced = RareTs::remove_pointer_t<T>;
                    if constexpr ( std::is_base_of_v<Generic::Value, Dereferenced> )
                    {
                        if ( Consume::tryNull<InArray>(is, c) )
                        {
                            if ( !std::is_const_v<T> )
                                value = nullptr;
                        }
                        else if constexpr ( std::is_const_v<T> )
                            Consume::value<InArray>(is, c);
                        else
                            Read::genericValue<InArray>(is, context, c)->into(value);
                    }
                    else if ( value == nullptr ) // Value is a nullptr and not a Json::Generic
                    {
                        if constexpr ( std::is_same_v<std::shared_ptr<Dereferenced>, T> )
                        {
                            value = std::make_shared<Dereferenced>();
                            Read::value<Annotations, InArray, Member>(is, context, c, object, *value);
                        }
                        else if constexpr ( std::is_same_v<std::unique_ptr<Dereferenced>, T> )
                        {
                            value = std::make_unique<Dereferenced>();
                            Read::value<Annotations, InArray, Member>(is, context, c, object, *value);
                        }
                        else // Value pointed to is a nullptr and value is not a smart pointer or generic, only valid value is null
                            Consume::null<InArray>(is, c);
                    }
                    else if constexpr ( RareTs::is_pointable_v<Dereferenced> && !std::is_const_v<Dereferenced> )
                        Read::value<Annotations, InArray, Member>(is, context, c, object, *value);  // Only chance assigning nullptr to the more deeply nested pointer
                    else if ( Consume::tryNull<InArray>(is, c) ) // If value pointer is not nullptr, "null" is a possible value
                    {
                        if constexpr ( !std::is_const_v<T> )
                            value = nullptr;
                    }
                    else
                        Read::value<Annotations, InArray, Member>(is, context, c, object, *value);
                }
                else if constexpr ( std::is_base_of_v<Generic::Value, T> )
                {
                    if constexpr ( std::is_const_v<T> )
                        Consume::value<InArray>(is, c);
                    else
                        Read::genericValue<InArray>(is, context, c)->into(value);
                }
                else if constexpr ( RareTs::is_tuple_v<T> )
                {
                    if constexpr ( std::tuple_size_v<T> == 0 )
                    {
                        if ( !Consume::tryNull<InArray>(is, c) )
                            Consume::iterable<true>(is, c);
                    }
                    else if constexpr ( std::tuple_size_v<T> == 1 )
                    {
                        if ( Read::trySingularTupleArrayPrefix(is, c) )
                        {
                            if ( !Read::tryArraySuffix(is) )
                            {
                                Read::value<Annotations, true, Member>(is, context, c, object, std::get<0>(value));
                                while ( Read::iterableElementSeparator<false>(is) )
                                    Consume::value<true>(is, c);
                            }
                        }
                        else
                            Read::value<Annotations, InArray, Member>(is, context, c, object, std::get<0>(value));
                    }
                    else if constexpr ( std::tuple_size_v<T> >= 2 )
                        Read::tuple<Annotations, Member>(is, context, c, object, value);
                }
                else if constexpr ( RareTs::is_pair_v<T> )
                    Read::pair<Annotations, Member>(is, context, c, object, value);
                else if constexpr ( std::is_same_v<std::string, std::remove_const_t<T>> && !Member::template hasNote<Json::UnstringType>() )
                    Read::string<T, ExpectQuotes>(is, c, value);
                else if constexpr ( RareTs::is_iterable_v<T> )
                    Read::iterable<Annotations, Member, T>(is, context, c, object, value);
                else if constexpr ( RareTs::is_reflected_v<T> )
                    Read::object<Annotations>(is, context, c, value);
                else if constexpr ( Member::template hasNote<Json::StringifyType>() )
                    Read::string<T, ExpectQuotes>(is, c, value);
                else if constexpr ( Member::template hasNote<Json::EnumIntType>() )
                    Read::enumInt<T>(is, value);
                else if constexpr ( std::is_same_v<bool, std::remove_const_t<T>> )
                    Read::boolean<InArray>(is, c, value);
                else if constexpr ( std::is_const_v<T> )
                    Consume::value<InArray>(is, c);
                else
                    is >> value;
            }

            template <typename Annotations, typename Member, size_t TupleIndex, typename Object, typename T1, typename T2, typename ...Ts>
            constexpr void tuple(std::istream & is, Context & context, char & c, Object & object, std::tuple<T1, T2, Ts...> & value)
            {
                constexpr size_t tupleSize = std::tuple_size_v<std::remove_reference_t<decltype(value)>>;
                if constexpr ( TupleIndex == 0 )
                    Read::arrayPrefix(is, c);

                if ( !Read::tryArraySuffix(is) )
                {
                    if constexpr ( TupleIndex < tupleSize )
                    {
                        Read::value<Annotations, true, Member>(is, context, c, object, std::get<TupleIndex>(value));
                        if ( Read::iterableElementSeparator<false>(is) )
                            Read::tuple<Annotations, Member, TupleIndex+1, Object, T1, T2, Ts...>(is, context, c, object, value);
                    }
                    else
                    {
                        do
                        {
                            Consume::value<true>(is, c);
                        }
                        while ( Read::iterableElementSeparator<false>(is) );
                    }
                }
            }

            template <typename Annotations, typename Member, typename Object, typename Key, typename T>
            constexpr void pair(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value)
            {
                Read::arrayPrefix(is, c);
                if ( !Read::tryArraySuffix(is) )
                {
                    Read::value<Annotations, true, Member>(is, context, c, object, value.first);
                    if ( Read::iterableElementSeparator<false>(is) )
                    {
                        Read::value<Annotations, true, Member>(is, context, c, object, value.second);
                        while ( Read::iterableElementSeparator<false>(is) )
                            Consume::value<true>(is, c);
                    }
                }
            }

            template <typename Annotations, typename Member, typename Object, typename Key, typename T>
            inline void keyValueObject(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value)
            {
                Read::objectPrefix(is, c);
                if ( !Read::tryObjectSuffix(is) )
                {
                    do
                    {
                        std::string fieldName = Read::fieldName(is, c);
                        Read::fieldNameValueSeparator(is, c);
                        if ( fieldName.compare("key") == 0 )
                            Read::value<Annotations, false, Member>(is, context, c, object, value.first);
                        else if ( fieldName.compare("value") == 0 )
                            Read::value<Annotations, false, Member>(is, context, c, object, value.second);
                        else
                            Consume::value<false>(is, c);
                    }
                    while ( Read::fieldSeparator(is) );
                }
            }
            
            template <typename Annotations, typename Member, typename Object, typename Key, typename T>
            inline void fieldPair(std::istream & is, Context & context, char & c, Object & object, Key & key, T & value)
            {
                std::stringstream ss;
                Read::string<>(is, c, ss);
                Read::value<Annotations, false, Member, Key, Object, true, false>(ss, context, c, object, key);
                Read::fieldNameValueSeparator(is, c);
                Read::value<Annotations, false, Member>(is, context, c, object, value);
            }

            template <typename Annotations, typename Member, typename T, typename Object>
            constexpr void iterable(std::istream & is, Context & context, char & c, Object & object, T & iterable)
            {
                using Element = RareTs::element_type_t<std::remove_cv_t<T>>;
                constexpr bool IsMap = RareTs::is_map_v<T>; // Simple maps are just json objects with keys as the field names, values as field values
                constexpr bool HasComplexKey = IsMap && is_non_primitive_v<RareTs::pair_lhs_t<Element>>;
                constexpr bool JsonObjectCompatible = (IsMap && !HasComplexKey) ||
                    (!IsMap && (RareTs::is_pair_v<Element> || is_tuple_pair_v<Element>));

                RareTs::clear(iterable);
                size_t i=0;
                if constexpr ( JsonObjectCompatible )
                {
                    if ( Read::iterablePrefix(is, c) ) // Json Object
                    {
                        if ( !Read::tryObjectSuffix(is) )
                        {
                            do
                            {
                                if constexpr ( RareTs::is_static_array_v<T> )
                                {
                                    if ( i >= RareTs::static_array_size_v<T> )
                                        throw ArraySizeExceeded();
                                    else
                                        Read::fieldPair<Annotations, Member>(is, context, c, object, std::get<0>(iterable[i]), std::get<1>(iterable[i]));
                                    i++;
                                }
                                else // Appendable STL container
                                {
                                    Element value;
                                    Read::fieldPair<Annotations, Member>(is, context, c, object, std::get<0>(value), std::get<1>(value));
                                    RareTs::append(iterable, value);
                                }
                            }
                            while ( Read::iterableElementSeparator<true>(is) );
                        }
                        return; // Object read finished
                    }
                }
                else
                    Read::arrayPrefix(is, c); // Json Array

                if ( !Read::tryArraySuffix(is) )
                {
                    do
                    {
                        if constexpr ( RareTs::is_static_array_v<T> )
                        {
                            if ( i >= RareTs::static_array_size_v<T> )
                                throw ArraySizeExceeded();
                            else
                                Read::value<Annotations, true, Member>(is, context, c, object, iterable[i++]);
                        }
                        else // Appendable STL container
                        {
                            Element value;
                            if constexpr ( !IsMap )
                                Read::value<Annotations, true, Member>(is, context, c, object, value);
                            else if ( Read::peekIterablePrefix(is, c) ) // Json Object
                                Read::keyValueObject<Annotations, Member, Object>(is, context, c, object, value);
                            else // Json Array
                                Read::pair<Annotations, Member, Object>(is, context, c, object, value);
                            RareTs::append(iterable, value);
                        }
                    }
                    while ( Read::iterableElementSeparator<false>(is) );
                }
            }

            template <typename OpNotes = RareTs::NoNote, typename Object = void>
            constexpr void field(std::istream & is, Context & context, char & c, Object & object, const std::string & fieldName)
            {
                Read::fieldNameValueSeparator(is, c);
                JsonField* jsonField = getJsonField<Object>(fieldName);
                if ( jsonField != nullptr ) // Known field
                {
                    if ( jsonField->type == JsonField::Type::Regular )
                    {
                        Reflect<Object>::Members::at(jsonField->index, object, [&](auto & member, auto & value) {
                            using Member = std::remove_reference_t<decltype(member)>;
                            Read::value<OpNotes, false, Member>(is, context, c, object, value);
                        });
                    }
                    else if ( jsonField->type == JsonField::Type::SuperClass )
                    {
                        Reflect<Object>::Supers::superAt(jsonField->index, object, [&](auto & superObj) {
                            using Super = std::remove_reference_t<decltype(superObj)>;
                            Read::object<OpNotes, Super>(is, context, c, superObj);
                        });
                    }
                }
                else // Unknown field
                {
                    jsonField = getJsonField<Object>(fieldClusterToJsonFieldName());
                    if ( jsonField != nullptr ) // Has FieldCluster
                    {
                        Reflect<Object>::Values::at(jsonField->index, object, [&](auto & value) {
                            using ValueType = std::remove_reference_t<decltype(value)>;
                            if constexpr ( std::is_base_of_v<Generic::Object, RareTs::remove_pointer_t<ValueType>> )
                            {
                                if constexpr ( RareTs::is_pointable_v<ValueType> )
                                {
                                    using Dereferenced = RareTs::remove_pointer_t<ValueType>;
                                    if constexpr ( std::is_same_v<std::shared_ptr<Dereferenced>, ValueType> )
                                        value = std::make_shared<Dereferenced>();
                                    else if constexpr ( std::is_same_v<std::unique_ptr<Dereferenced>, ValueType> )
                                        value = std::make_unique<Dereferenced>();
                                    else
                                    {
                                        throw Exception(
                                            "Cannot assign a non-null value to a null pointer unless the type is std::shared_ptr or std::unique_ptr");
                                    }

                                    value->put(fieldName, Read::genericValue<false>(is, context, c)->out());
                                }
                                else
                                    value.put(fieldName, Read::genericValue<false>(is, context, c)->out());
                            }
                            else
                                throw Exception(std::string(RareTs::toStr<ValueType>()).c_str());
                        });
                    }
                    else // No FieldCluster
                        Consume::value<false>(is, c);
                }
            }

            template <typename Annotations, typename T>
            inline void object(std::istream & is, Context & context, char & c, T & t)
            {
                Read::objectPrefix(is, c);
                if ( !Read::tryObjectSuffix(is) )
                {
                    do
                    {
                        std::string fieldName = Read::fieldName(is, c);
                        Read::field<Annotations>(is, context, c, t, fieldName);
                    }
                    while ( Read::fieldSeparator(is) );
                }
            }
        }
        
        template <typename Annotations, typename T>
        class ReflectedObject
        {
        public:
            ReflectedObject(T & obj, std::shared_ptr<Context> context) : obj(obj), context(context) {}

            T & obj;
            std::shared_ptr<Context> context;

            std::istream & get(std::istream & is)
            {
                if ( context == nullptr )
                    context = std::make_shared<Context>();

                char c = '\0';
                Read::value<Annotations, false, MockMember<T>>(is, *context, c, obj, obj);
                return is;
            }
        };
        
        template <typename Annotations, typename T>
        inline std::istream & operator>>(std::istream & is, Json::Input::ReflectedObject<Annotations, T> object)
        {
            return object.get(is);
        }

        template <typename Annotations = RareTs::NoNote, typename T = void>
        inline Input::ReflectedObject<Annotations, T> in(T & t, std::shared_ptr<Context> context = nullptr)
        {
            return Input::ReflectedObject<Annotations, T>(t, context);
        }
    }

    inline Json::OutStreamType & operator<<(Json::OutStreamType & os, const Json::Generic::Value & value)
    {
        Json::Put::genericValue<RareTs::NoNote, true, Json::twoSpaces, true>(os, Json::defaultContext, 0, value);
        return os;
    }
}

#endif
