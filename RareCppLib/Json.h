#ifndef JSON_H
#define JSON_H
#include <istream>
#include <ostream>
#include <sstream>
#include <typeindex>
#include <type_traits>
#include <functional>
#include <cstring>
#ifndef REFLECT_H
#include "Reflect.h"
#endif
#ifdef USE_BUFFERED_STREAMS
#include "StringBuffer.h"
#endif

namespace Json
{
#ifdef USE_BUFFERED_STREAMS
    using namespace BufferedStream;
    using OutStreamType = StringBuffer;
    static constexpr EndL osEndl = {};
#else
    using OutStreamType = std::ostream;
    #define osEndl std::endl
#endif

    using namespace ExtendedTypeSupport;
    using namespace Reflect;
    
    inline namespace Annotations
    {
        /// Field annotation telling JSON to explicitly use the numeric value of an enum
        struct EnumIntType {};
        static constexpr EnumIntType EnumInt{};

        /// Field annotation telling JSON to skip a field during input and output
        struct IgnoreType {};
        static constexpr IgnoreType Ignore{};

        /// Field annotation telling JSON to treat a std::string field as though it wasn't a string
        struct UnstringType {};
        static constexpr UnstringType Unstring{};

        struct StringifyType {};
        static constexpr StringifyType Stringify{};

        /// Field or Super annotation telling JSON to use a different name for the field
        struct Name
        {
             std::string_view value;
        };

        struct Unspecialized {};

        template <typename T>
        struct is_specialized
        {
            static constexpr bool value = !std::is_base_of<Unspecialized, T>::value;
        };

        struct IsRoot {};
    }
    
    inline namespace Shared
    {
        enum class Statics
        {
            Excluded = 0,
            Included = 1,
            Only = 2
        };

        template <bool IsStatic, Statics statics>
        struct matches_statics
        {
            constexpr static bool value =
                statics == Statics::Included ||
                (statics == Statics::Excluded && !IsStatic) ||
                (statics == Statics::Only && IsStatic);
        };

        template <Statics statics, typename Object>
        static constexpr size_t IgnoredFieldCount()
        {
            size_t count = 0;
            Object::Class::ForEachField([&](auto & field) {
                using Field = typename std::remove_reference<decltype(field)>::type;
                if constexpr ( (Field::template HasAnnotation<Json::IgnoreType> || Field::IsFunction) && matches_statics<Field::IsStatic, statics>::value )
                    count ++;
            });
            return count;
        }

        template <Statics statics, typename Object, bool IncludeSupers = true>
        static constexpr bool HasFields()
        {
            constexpr bool MatchesStaticsExcluded = statics == Statics::Excluded &&
                Object::Class::TotalFields - Object::Class::TotalStaticFields - IgnoredFieldCount<statics, Object>() > 0;
            constexpr bool MatchesStaticsIncluded = statics == Statics::Included &&
                Object::Class::TotalFields - IgnoredFieldCount<statics, Object>() > 0;
            constexpr bool MatchesStaticsOnly = statics == Statics::Only &&
                Object::Class::TotalStaticFields - IgnoredFieldCount<statics, Object>() > 0;

            if constexpr ( MatchesStaticsExcluded || MatchesStaticsIncluded || MatchesStaticsOnly )
                return true;
            else if constexpr ( Object::Supers::TotalSupers == 0 || !IncludeSupers )
                return false;
            else
            {
                bool hasFields = false;
                Object::Supers::ForEach([&](auto superInfo) {
                    using Super = typename decltype(superInfo)::Type;
                    if constexpr ( HasFields<statics, Super>() )
                        hasFields = true;
                });
                return hasFields;
            }
        }

        template <Statics statics, typename Object, size_t Index = 0>
        static constexpr size_t FirstIndex()
        {
            size_t firstIndex = 0;
            Object::Class::FieldAt(Index, [&](auto & field) {
                using Field = typename std::remove_reference<decltype(field)>::type;
                if constexpr ( !Field::template HasAnnotation<Json::IgnoreType> && matches_statics<Field::IsStatic, statics>::value )
                    firstIndex = Field::Index;
                else if constexpr ( Index < Object::Class::TotalFields )
                    firstIndex = FirstIndex<statics, Object, Index+1>();
            });
            return firstIndex;
        }

        template <Statics statics, typename Object, size_t Index = 0>
        static constexpr size_t FirstSuperIndex()
        {
            size_t firstSuperIndex = 0;
            Object::Supers::At(Index, [&](auto superInfo) {
                using Super = typename decltype(superInfo)::Type;
                if constexpr ( HasFields<statics, Super>() )
                    firstSuperIndex = Index;
                else if constexpr ( Index < Object::Supers::TotalSupers )
                    firstSuperIndex = FirstSuperIndex<statics, Object, Index+1>();
            });
            return firstSuperIndex;
        }

        constexpr size_t NoFieldIndex = std::numeric_limits<size_t>::max();
        using NoField = Fields::Field<>;

        template <typename T>
        using ReflectedField = Fields::Field<T, std::nullptr_t, 0, IsRoot>;

        struct Context
        {
            virtual ~Context() {}
        };
        static inline Context context;

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
            static inline std::string simplifyTypeStr(const std::string & superTypeStr)
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
                        simpleTypeStr += std::toupper(rawSimpleTypeStr[i]);
                }
                return simpleTypeStr;
            }

            template <typename T>
            std::string superTypeToJsonFieldName()
            {
                return std::string("__") + simplifyTypeStr(TypeToStr<T>());
            }

            static inline std::string fieldClusterToJsonFieldName()
            {
                return std::string("____fieldCluster");
            }
        }
    };

    inline namespace Generic
    {
        struct JsonField
        {
            enum_t(Type, size_t, {
                Regular = 0,
                SuperClass = 1,
                FieldCluster = 2
            });
        
            JsonField() : index(0), type(Type::Regular), name("") {}
            JsonField(size_t index, Type fieldType, const std::string & name) : index(index), type(fieldType), name(name) {}
        
            size_t index;
            Type type;
            std::string name;
        };
        
        class Value {
        public:

            enum_t(Type, uint8_t, {
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
            });
            
            class TypeMismatch : public Exception
            {
            public:
                static std::string getTypeStr(Value::Type type)
                {
                    switch ( type )
                    {
                        case Value::Type::None: return "None";
                        case Value::Type::Boolean: return "Boolean";
                        case Value::Type::Number: return "Number";
                        case Value::Type::String: return "String";
                        case Value::Type::Object: return "Object";
                        case Value::Type::Array: return "Array";
                        case Value::Type::BoolArray: return "BoolArray";
                        case Value::Type::NumberArray: return "NumberArray";
                        case Value::Type::StringArray: return "StringArray";
                        case Value::Type::ObjectArray: return "ObjectArray";
                        case Value::Type::MixedArray: return "MixedArray";
                    }
                    return "Unknown";
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
                Assigner(Value* allocatedValue) : allocatedValue(allocatedValue) {}
                ~Assigner() { if ( allocatedValue != nullptr ) delete allocatedValue; }

                static std::shared_ptr<Assigner> Make(Value* allocatedValue) { return std::shared_ptr<Assigner>(new Assigner(allocatedValue)); }
                
                Value* get() { return allocatedValue; };

                /// Assigns the stored Json::Generic::Value to the value passed to the method, then discards the stored Json::Generic::Value
                template <typename T>
                void into(T & value)
                {
                    if constexpr ( is_pointable<T>::value )
                    {
                        if ( allocatedValue != nullptr )
                        {
                            using Dereferenced = typename remove_pointer<T>::type;
                            if ( value == nullptr )
                            {
                                Dereferenced* casted = dynamic_cast<Dereferenced*>(allocatedValue);
                                if ( casted == nullptr )
                                    throw TypeMismatch(Value::Type::None, allocatedValue->type());
                                else if constexpr ( std::is_same<std::shared_ptr<Dereferenced>, T>::value )
                                    value = std::shared_ptr<Dereferenced>(casted);
                                else if constexpr ( std::is_same<std::unique_ptr<Dereferenced>, T>::value )
                                    value = std::unique_ptr<Dereferenced>(casted);
                                else
                                    throw Exception("Cannot assign a non-null value to a null pointer unless the type is std::shared_ptr or std::unique_ptr");
                            }
                            else if ( value->type() != allocatedValue->type() ) // value != nullptr
                                throw TypeMismatch(value->type(), allocatedValue->type());
                            else if constexpr ( std::is_same<std::shared_ptr<Dereferenced>, T>::value ) // && value != nullptr && types match
                                value = std::shared_ptr<Dereferenced>(dynamic_cast<Dereferenced*>(allocatedValue));
                            else if constexpr ( std::is_same<std::unique_ptr<Dereferenced>, T>::value ) // && value != nullptr && types match
                                value = std::unique_ptr<Dereferenced>(dynamic_cast<Dereferenced*>(allocatedValue));
                            else // value != nullptr && value->type() == allocatedValue->type()
                            {
                                *value = *allocatedValue; // Non-null but not a smart pointer you can trust to perform cleanup, use value assignment
                                delete allocatedValue;
                            }

                            allocatedValue = nullptr;
                        }
                        else if ( value != nullptr ) // allocatedValue == nullptr
                            value = nullptr;
                    }
                    else if constexpr ( std::is_base_of<Value, T>::value )
                    {
                        if ( allocatedValue == nullptr ) // !is_pointable<T>::value
                            throw NullUnassignable();
                        else if ( value.type() != allocatedValue->type() ) // !is_pointable<T>::value && allocatedValue != nullptr
                            throw TypeMismatch(value.type(), allocatedValue->type());
                        else // !is_pointable<T>::value && allocatedValue != nullptr && value.type() == allocatedValue->type()
                        {
                            value = *allocatedValue;
                            allocatedValue = nullptr;
                        }
                    }
                    else
                        throw Exception("Cannot assign generic value to any type except Json::Value");
                }

                std::shared_ptr<Value> out()
                {
                    std::shared_ptr<Value> output = std::shared_ptr<Value>(allocatedValue);
                    allocatedValue = nullptr;
                    return output;
                }

            private:
                Value* allocatedValue;
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
        using ObjectValuePtr = std::shared_ptr<ObjectValue>;

        class Bool : public Value {
        public:
            Bool() : value(false) {}
            Bool(bool value) : value(value) {}
            Bool(const Bool & other) : value(other.value) {}
            virtual ~Bool() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new Bool()); }
            static std::shared_ptr<Value> Make(bool value) { return std::shared_ptr<Value>(new Bool(value)); }
            static std::shared_ptr<Value> Make(const Bool & other) { return std::shared_ptr<Value>(new Bool(other)); }
            
            Bool & operator=(const Value & other) { value = other.boolean(); return *this; }

            virtual Type type() const { return Value::Type::Boolean; }
            
            virtual bool & boolean() { return value; }
            virtual std::string & number() { throw TypeMismatch(Value::Type::Boolean, Value::Type::Number, "number"); }
            virtual std::string & string() { throw TypeMismatch(Value::Type::Boolean, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::Boolean, Value::Type::Object, "object"); }

            virtual const bool & boolean() const { return value; }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::Boolean, Value::Type::Number, "number"); }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::Boolean, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::Boolean, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { throw TypeMismatch(Value::Type::Boolean, Value::Type::Array, "arraySize"); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::Boolean, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::Boolean, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::Boolean, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::Boolean, Value::Type::StringArray, "stringArray"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::Boolean, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::Boolean, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::Boolean, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::Boolean, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const { throw TypeMismatch(Value::Type::Boolean, Value::Type::NumberArray, "numberArray"); }
            virtual const std::vector<std::string> & stringArray() const { throw TypeMismatch(Value::Type::Boolean, Value::Type::StringArray, "stringArray"); }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::Boolean, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
                throw TypeMismatch(Value::Type::Boolean, Value::Type::MixedArray, "mixedArray");
            }

        private:
	        bool value;
        };
        class Number : public Value {
        public:
            Number() : value("0") {}
            Number(const std::string & value) : value(value) {}
            Number(const Number & other) : value(other.value) {}
            virtual ~Number() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new Number()); }
            static std::shared_ptr<Value> Make(const char* value) { return std::shared_ptr<Value>(new Number(value)); }
            static std::shared_ptr<Value> Make(const std::string & value) { return std::shared_ptr<Value>(new Number(value)); }
            static std::shared_ptr<Value> Make(const Number & other) { return std::shared_ptr<Value>(new Number(other)); }
            template <typename T> static std::shared_ptr<Value> Make(const T & number) { return std::shared_ptr<Value>(new Number(std::to_string(number))); }
            
            Number & operator=(const Value & other) { value = other.number(); return *this; }
            
            virtual Type type() const { return Value::Type::Number; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::Number, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { return value; }
            virtual std::string & string() { throw TypeMismatch(Value::Type::Number, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::Number, Value::Type::Object, "object"); }

            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::Number, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { return value; }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::Number, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::Number, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { throw TypeMismatch(Value::Type::Number, Value::Type::Array, "arraySize"); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::Number, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::Number, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::Number, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::Number, Value::Type::StringArray, "stringArray"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::Number, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::Number, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::Number, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::Number, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const { throw TypeMismatch(Value::Type::Number, Value::Type::NumberArray, "numberArray"); }
            virtual const std::vector<std::string> & stringArray() const { throw TypeMismatch(Value::Type::Number, Value::Type::StringArray, "stringArray"); }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::Number, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
                throw TypeMismatch(Value::Type::Number, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
	        std::string value;
        };
        class String : public Value {
        public:
            String() : value() {}
            String(const std::string & value) : value(value) {}
            String(const String & other) : value(other.value) {}
            virtual ~String() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new String()); }
            static std::shared_ptr<Value> Make(const std::string & value) { return std::shared_ptr<Value>(new String(value)); }
            static std::shared_ptr<Value> Make(const String & other) { return std::shared_ptr<Value>(new String(other)); }
            
            String & operator=(const Value & other) { value = other.string(); return *this; }
            
            virtual Type type() const { return Value::Type::String; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::String, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { throw TypeMismatch(Value::Type::String, Value::Type::Number, "number"); }
            virtual std::string & string() { return value; }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::String, Value::Type::Object, "object"); }

            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::String, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::String, Value::Type::Number, "number"); }
            virtual const std::string & string() const { return value; }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::String, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { throw TypeMismatch(Value::Type::String, Value::Type::Array, "arraySize"); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::String, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::String, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::String, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::String, Value::Type::StringArray, "stringArray"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::String, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::String, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::String, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::String, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const { throw TypeMismatch(Value::Type::String, Value::Type::NumberArray, "numberArray"); }
            virtual const std::vector<std::string> & stringArray() const { throw TypeMismatch(Value::Type::String, Value::Type::StringArray, "stringArray"); }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::String, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
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
            virtual ~Object() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new Object()); }
            static std::shared_ptr<Value> Make(const std::map<std::string, std::shared_ptr<Value>> & value) {
                return std::shared_ptr<Value>(new Object(value));
            }
            static std::shared_ptr<Value> Make(const Object & other) { return std::shared_ptr<Value>(new Object(other)); }
            
            Object & operator=(const Value & other) { value = other.object(); return *this; }
            
            virtual Type type() const { return Value::Type::Object; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::Object, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { throw TypeMismatch(Value::Type::Object, Value::Type::Number, "number"); }
            virtual std::string & string() { throw TypeMismatch(Value::Type::Object, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { return value; }
            
            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::Object, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::Object, Value::Type::Number, "number"); }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::Object, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const { return value; }

            virtual size_t arraySize() const { throw TypeMismatch(Value::Type::Object, Value::Type::Array, "arraySize"); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::Object, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::Object, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::Object, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::Object, Value::Type::StringArray, "stringArray"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::Object, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::Object, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::Object, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::Object, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const { throw TypeMismatch(Value::Type::Object, Value::Type::NumberArray, "numberArray"); }
            virtual const std::vector<std::string> & stringArray() const { throw TypeMismatch(Value::Type::Object, Value::Type::StringArray, "stringArray"); }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::Object, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
                throw TypeMismatch(Value::Type::Object, Value::Type::MixedArray, "mixedArray");
            }
            
            virtual void put(std::string fieldName, std::shared_ptr<Value> value)
            {
                this->value.insert_or_assign(fieldName, value);
            }
            
        private:
	        std::map<std::string, std::shared_ptr<Value>> value;
        };
	    
        class NullArray : public Value {
        public:
            NullArray() : nullCount(0) {}
            NullArray(size_t nullCount) : nullCount(nullCount) {}
            NullArray(const NullArray & other) : nullCount(other.nullCount) {}
            virtual ~NullArray() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new NullArray()); }
            static std::shared_ptr<Value> Make(size_t nullCount) { return std::shared_ptr<Value>(new NullArray(nullCount)); }
            static std::shared_ptr<Value> Make(const NullArray & other) { return std::shared_ptr<Value>(new NullArray(other)); }
            
            NullArray & operator=(const Value & other) { nullCount = other.nullArray(); return *this; }
            
            virtual Type type() const { return Value::Type::NullArray; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::NullArray, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { throw TypeMismatch(Value::Type::NullArray, Value::Type::Number, "number"); }
            virtual std::string & string() { throw TypeMismatch(Value::Type::NullArray, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::NullArray, Value::Type::Object, "object"); }

            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::NullArray, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::NullArray, Value::Type::Number, "number"); }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::NullArray, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { return nullCount; }
            
            virtual size_t & nullArray() { return nullCount; }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::NullArray, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::NullArray, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::NullArray, Value::Type::StringArray, "stringArray"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::NullArray, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { return nullCount; }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::NullArray, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::NumberArray, "numberArray");
            }
            virtual const std::vector<std::string> & stringArray() const {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::StringArray, "stringArray");
            }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
                throw TypeMismatch(Value::Type::NullArray, Value::Type::MixedArray, "mixedArray");
            }

        private:
	        size_t nullCount;
        };
        class BoolArray : public Value {
        public:
            BoolArray() : values() {}
            BoolArray(const std::vector<bool> & values) : values(values) {}
            BoolArray(const BoolArray & other) : values(other.values) {}
            virtual ~BoolArray() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new BoolArray()); }
            static std::shared_ptr<Value> Make(const std::vector<bool> & values) { return std::shared_ptr<Value>(new BoolArray(values)); }
            static std::shared_ptr<Value> Make(const BoolArray & other) { return std::shared_ptr<Value>(new BoolArray(other)); }
            
            BoolArray & operator=(const Value & other) { values = other.boolArray(); return *this; }

            virtual Type type() const { return Value::Type::BoolArray; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Number, "number"); }
            virtual std::string & string() { throw TypeMismatch(Value::Type::BoolArray, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Object, "object"); }
            
            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::BoolArray, Value::Type::Number, "number"); }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::BoolArray, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { return values.size(); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::BoolArray, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { return values; }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::BoolArray, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::BoolArray, Value::Type::StringArray, "stringArray"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::BoolArray, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::BoolArray, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { return values; }
            virtual const std::vector<std::string> & numberArray() const {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::NumberArray, "numberArray");
            }
            virtual const std::vector<std::string> & stringArray() const {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::StringArray, "stringArray");
            }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
                throw TypeMismatch(Value::Type::BoolArray, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
	        std::vector<bool> values;
        };
        class NumberArray : public Value {
        public:
            NumberArray() : values() {}
            NumberArray(const std::vector<std::string> & values) : values(values) {}
            NumberArray(const NumberArray & other) : values(other.values) {}
            virtual ~NumberArray() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new NumberArray()); }
            static std::shared_ptr<Value> Make(const std::vector<std::string> & values) { return std::shared_ptr<Value>(new NumberArray(values)); }
            static std::shared_ptr<Value> Make(const NumberArray & other) { return std::shared_ptr<Value>(new NumberArray(other)); }
            
            NumberArray & operator=(const Value & other) { values = other.numberArray(); return *this; }
            
            virtual Type type() const { return Value::Type::NumberArray; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Number, "number"); }
            virtual std::string & string() { throw TypeMismatch(Value::Type::NumberArray, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Object, "object"); }
            
            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::NumberArray, Value::Type::Number, "number"); }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::NumberArray, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { return values.size(); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::NumberArray, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::NumberArray, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { return values; }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::NumberArray, Value::Type::StringArray, "stringArray"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::NumberArray, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::NumberArray, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::NumberArray, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const { return values; }
            virtual const std::vector<std::string> & stringArray() const {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::StringArray, "stringArray");
            }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
                throw TypeMismatch(Value::Type::NumberArray, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
	        std::vector<std::string> values;
        };
        class StringArray : public Value {
        public:
            StringArray() : values() {}
            StringArray(const std::vector<std::string> & other) : values(other) {}
            StringArray(const StringArray & other) : values(other.values) {}
            virtual ~StringArray() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new StringArray()); }
            static std::shared_ptr<Value> Make(const std::vector<std::string> & values) { return std::shared_ptr<Value>(new StringArray(values)); }
            static std::shared_ptr<Value> Make(const StringArray & other) { return std::shared_ptr<Value>(new StringArray(other)); }
            
            StringArray & operator=(const Value & other) { values = other.stringArray(); return *this; }
            
            virtual Type type() const { return Value::Type::StringArray; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::StringArray, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { throw TypeMismatch(Value::Type::StringArray, Value::Type::Number, "number"); }
            virtual std::string & string() { throw TypeMismatch(Value::Type::StringArray, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::StringArray, Value::Type::Object, "object"); }
            
            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::StringArray, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::StringArray, Value::Type::Number, "number"); }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::StringArray, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { return values.size(); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::StringArray, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::StringArray, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::StringArray, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { return values; }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::StringArray, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::StringArray, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::StringArray, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::NumberArray, "numberArray");
            }
            virtual const std::vector<std::string> & stringArray() const { return values; }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
                throw TypeMismatch(Value::Type::StringArray, Value::Type::MixedArray, "mixedArray");
            }

        private:
	        std::vector<std::string> values;
        };
        class ObjectArray : public Value {
        public:
            ObjectArray() : values() {}
            ObjectArray(const std::vector<std::map<std::string, std::shared_ptr<Value>>> & values) : values(values) {}
            ObjectArray(const ObjectArray & other) : values(other.values) {}
            virtual ~ObjectArray() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new ObjectArray()); }
            static std::shared_ptr<Value> Make(const std::vector<std::map<std::string, std::shared_ptr<Value>>> & values) {
                return std::shared_ptr<Value>(new ObjectArray(values));
            }
            static std::shared_ptr<Value> Make(const ObjectArray & other) { return std::shared_ptr<Value>(new ObjectArray(other)); }
            
            ObjectArray & operator=(const Value & other) { values = other.objectArray(); return *this; }
            
            virtual Type type() const { return Value::Type::ObjectArray; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Number, "number"); }
            virtual std::string & string() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Object, "object"); }
            
            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Number, "number"); }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::ObjectArray, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { return values.size(); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::StringArray, "stringArray"); }
            virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() { return values; }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::MixedArray, "mixedArray"); }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::ObjectArray, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const {
                throw TypeMismatch(Value::Type::ObjectArray, Value::Type::NumberArray, "numberArray");
            }
            virtual const std::vector<std::string> & stringArray() const {
                throw TypeMismatch(Value::Type::ObjectArray, Value::Type::StringArray, "stringArray");
            }
            virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const { return values; }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const {
                throw TypeMismatch(Value::Type::ObjectArray, Value::Type::MixedArray, "mixedArray");
            }
            
        private:
	        std::vector<std::map<std::string, std::shared_ptr<Value>>> values;
        };
        class MixedArray : public Value {
        public:
            MixedArray() : values() {}
            MixedArray(const std::vector<std::shared_ptr<Value>> & values) : values(values) {}
            MixedArray(const MixedArray & other) : values(other.values) {}
            virtual ~MixedArray() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new MixedArray()); }
            static std::shared_ptr<Value> Make(const std::vector<std::shared_ptr<Value>> & values) { return std::shared_ptr<Value>(new MixedArray(values)); }
            static std::shared_ptr<Value> Make(const MixedArray & other) { return std::shared_ptr<Value>(new MixedArray(other)); }
            
            MixedArray & operator=(const Value & other) { values = other.mixedArray(); return *this; }
            
            virtual Type type() const { return Value::Type::MixedArray; }
            
            virtual bool & boolean() { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Boolean, "bool"); }
            virtual std::string & number() { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Number, "number"); }
            virtual std::string & string() { throw TypeMismatch(Value::Type::MixedArray, Value::Type::String, "string"); }
            virtual std::map<std::string, std::shared_ptr<Value>> & object() { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Object, "object"); }
            
            virtual const bool & boolean() const { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Boolean, "bool"); }
            virtual const std::string & number() const { throw TypeMismatch(Value::Type::MixedArray, Value::Type::Number, "number"); }
            virtual const std::string & string() const { throw TypeMismatch(Value::Type::MixedArray, Value::Type::String, "string"); }
            virtual const std::map<std::string, std::shared_ptr<Value>> & object() const {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::Object, "object");
            }

            virtual size_t arraySize() const { return values.size(); }
            
            virtual size_t & nullArray() { throw TypeMismatch(Value::Type::MixedArray, Value::Type::NullArray, "nullArray"); }
            virtual std::vector<bool> & boolArray() { throw TypeMismatch(Value::Type::MixedArray, Value::Type::BoolArray, "boolArray"); }
            virtual std::vector<std::string> & numberArray() { throw TypeMismatch(Value::Type::MixedArray, Value::Type::NumberArray, "numberArray"); }
            virtual std::vector<std::string> & stringArray() { throw TypeMismatch(Value::Type::MixedArray, Value::Type::StringArray, "stringArray"); }
	        virtual std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual std::vector<std::shared_ptr<Value>> & mixedArray() { return values; }
            
            virtual const size_t & nullArray() const { throw TypeMismatch(Value::Type::MixedArray, Value::Type::NullArray, "nullArray"); }
            virtual const std::vector<bool> & boolArray() const { throw TypeMismatch(Value::Type::MixedArray, Value::Type::BoolArray, "boolArray"); }
            virtual const std::vector<std::string> & numberArray() const {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::NumberArray, "numberArray");
            }
            virtual const std::vector<std::string> & stringArray() const {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::StringArray, "stringArray");
            }
	        virtual const std::vector<std::map<std::string, std::shared_ptr<Value>>> & objectArray() const {
                throw TypeMismatch(Value::Type::MixedArray, Value::Type::ObjectArray, "objectArray");
            }
            virtual const std::vector<std::shared_ptr<Value>> & mixedArray() const { return values; }

        private:
            std::vector<std::shared_ptr<Value>> values;
        };

        class FieldCluster : public Object
        {
        public:
            FieldCluster() : Object() {}
            FieldCluster(const std::map<std::string, std::shared_ptr<Value>> & value) : Object(value) {}
            FieldCluster(const FieldCluster & other) : Object(other) {}
            virtual ~FieldCluster() {}
            
            static std::shared_ptr<Value> Make() { return std::shared_ptr<Value>(new FieldCluster()); }
            static std::shared_ptr<Value> Make(const std::map<std::string, std::shared_ptr<Value>> & value) {
                return std::shared_ptr<Value>(new FieldCluster(value));
            }
            static std::shared_ptr<Value> Make(const FieldCluster & other) { return std::shared_ptr<Value>(new FieldCluster(other)); }
            
            FieldCluster & operator=(const Value & other) { object() = other.object(); return *this; }
            
            virtual Type type() const { return Value::Type::FieldCluster; }
        };
    };
    
    inline namespace Identifiers
    {
        template <typename T> struct is_non_primitive {
            static constexpr bool value = is_tuple<T>::value || is_pair<T>::value || is_iterable<T>::value || is_reflected<T>::value;
        };
        template <typename T> struct is_non_primitive<const T> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<T*> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<T &> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<T &&> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<std::unique_ptr<T>> { static constexpr bool value = is_non_primitive<T>::value; };
        template <typename T> struct is_non_primitive<std::shared_ptr<T>> { static constexpr bool value = is_non_primitive<T>::value; };
        template <> struct is_non_primitive<Json::Generic::Object> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::NullArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::BoolArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::NumberArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::StringArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::ObjectArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::MixedArray> { static constexpr bool value = true; };
        template <> struct is_non_primitive<Json::Generic::FieldCluster> { static constexpr bool value = true; };

        template <size_t Index, typename ...Ts> struct is_non_primitive_tuple_element;
        template <typename ...Ts> struct is_non_primitive_tuple_element<0, std::tuple<Ts...>> { static constexpr bool value = false; };
        template <size_t Index, typename ...Ts> struct is_non_primitive_tuple_element<Index, std::tuple<Ts...>> {
            using T = typename std::tuple_element_t<Index-1, std::tuple<Ts...>>;
            static constexpr bool value = is_non_primitive<T>::value || is_non_primitive_tuple_element<Index-1, std::tuple<Ts...>>::value;
        };

        template <typename T> struct is_non_primitive_tuple { static constexpr bool value = false; };
        template <typename ...Ts> struct is_non_primitive_tuple<std::tuple<Ts...>> {
            static constexpr bool value = is_non_primitive_tuple_element<sizeof...(Ts), std::tuple<Ts...>>::value;
        };

        template <typename T> struct is_tuple_pair { static constexpr bool value = false; };
        template <typename T1, typename T2> struct is_tuple_pair<std::tuple<T1, T2>> { static constexpr bool value = true; };
    }

    inline namespace Output
    {
        constexpr const char twoSpaces[] = "  ";
        
        inline namespace Customizers
        {
            template <typename Object, typename Value,
                size_t FieldIndex = NoFieldIndex, typename OpAnnotations = NoAnnotation, typename Field = NoField, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t TotalParentIterables = 0, size_t IndentLevel = 0, const char* indent = twoSpaces>
            struct Customize : public Unspecialized
            {
                /// Should return true if you put any output, else you should leave output unchanged
                static bool As(OutStreamType & output, Context & context, const Object & object, const Value & value) { return false; }
            };

            template <typename Value, typename OpAnnotations = NoAnnotation, typename Field = NoField, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t TotalParentIterables = 0, size_t IndentLevel = 0, const char* indent = twoSpaces>
            struct CustomizeType : public Unspecialized
            {
                /// Should return true if you put any output, else you should leave output unchanged
                static bool As(OutStreamType & output, Context & context, const Value & value) { return false; }
            };

            template <typename Object, typename Value,
                size_t FieldIndex = NoFieldIndex, typename OpAnnotations = NoAnnotation, typename Field = NoField, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t TotalParentIterables = 0, size_t IndentLevel = 0, const char* indent = twoSpaces>
            static constexpr bool HaveSpecialization =
                is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent>>::value || // Fully specialized
                is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations, Field>>::value || // Customize<5args> specialized
                is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations>>::value || // Customize<4args> specialized
                is_specialized<Customize<Object, Value, FieldIndex>>::value || // Customize<3args> specialized
                is_specialized<Customize<Object, Value>>::value || // Customize<2args> specialized
                is_specialized<Customize<Object, Value, FieldIndex, NoAnnotation, Field>>::value || // Customize<5args>, OpAnnotations defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, OpAnnotations, Field>>::value || // Customize<5args>, FieldIndex defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, NoAnnotation, Field>>::value || // Customize<5args>, both defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, OpAnnotations>>::value || // Customize<4args>, FieldIndex defaulted
                is_specialized<CustomizeType<Value, OpAnnotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent>>::value || // Fully specialized
                is_specialized<CustomizeType<Value, OpAnnotations, Field>>::value || // CustomizeType<3args> specialized
                is_specialized<CustomizeType<Value, OpAnnotations>>::value || // CustomizeType<2args> specialized
                is_specialized<CustomizeType<Value>>::value || // CustomizeType<1arg> specialized
                is_specialized<CustomizeType<Value, NoAnnotation, Field>>::value; // CustomizeType<3arg>, OpAnnotations defaulted
        }

        inline namespace StaticAffix
        {
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            struct IndentType { };
            
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            IndentType<PrettyPrint, IndentLevel, indent> Indent;

            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            OutStreamType & operator<<(OutStreamType & os, const IndentType<PrettyPrint, IndentLevel, indent> &)
            {
                if constexpr ( IndentLevel > 0 && PrettyPrint )
                {
                    for ( size_t i=0; i<IndentLevel; i++ )
                        os << indent;
                }

                return os;
            }
            
            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            struct ArrayPrefixType { };

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            ArrayPrefixType<PrettyPrint, ContainsPrimitives, IndentLevel, indent> ArrayPrefix;

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            OutStreamType & operator<<(OutStreamType & os, const ArrayPrefixType<PrettyPrint, ContainsPrimitives, IndentLevel, indent> &)
            {
                if constexpr ( !PrettyPrint )
                    os << "[";
                else if constexpr ( ContainsPrimitives )
                    os << "[ ";
                else
                    os << "[" << osEndl << Indent<PrettyPrint, IndentLevel, indent>;

                return os;
            }

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            struct ArraySuffixType { };

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            ArraySuffixType<PrettyPrint, ContainsPrimitives, IndentLevel, indent> ArraySuffix;

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            OutStreamType & operator<<(OutStreamType & os, const ArraySuffixType<PrettyPrint, ContainsPrimitives, IndentLevel, indent> &)
            {
                if constexpr ( !PrettyPrint )
                    os << "]";
                else if constexpr ( ContainsPrimitives )
                    os << " ]";
                else
                    os << osEndl << Indent<PrettyPrint, IndentLevel, indent> << "]";

                return os;
            }

            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            struct ObjectPrefixType { };
                
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            ObjectPrefixType<PrettyPrint, IndentLevel, indent> ObjectPrefix;
                
            template <bool PrettyPrint, size_t IndentLevel, const char* indent, Statics statics, typename Object>
            OutStreamType & operator<<(OutStreamType & os, const ObjectPrefixType<PrettyPrint, IndentLevel, indent, statics, Object> &)
            {
                os << "{";
                return os;
            }

            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            struct ObjectSuffixType { };
                
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            ObjectSuffixType<PrettyPrint, IndentLevel, indent, statics, Object> ObjectSuffix;
                
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            OutStreamType & operator<<(OutStreamType & os, const ObjectSuffixType<PrettyPrint, IndentLevel, indent, statics, Object> &)
            {
                if constexpr ( std::is_same<void, Object>::value )
                {
                    if constexpr ( PrettyPrint )
                        os << osEndl << Indent<PrettyPrint, IndentLevel, indent> << "}";
                    else
                        os << "}";
                }
                else
                {
                    constexpr bool NotEmpty = HasFields<statics, Object>();
                    if constexpr ( PrettyPrint && NotEmpty )
                        os << osEndl << Indent<PrettyPrint, IndentLevel, indent> << "}";
                    else
                        os << "}";
                }
                return os;
            }
                
            template <bool IsFirst, bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            struct FieldPrefixType { };
                
            template <bool IsFirst, bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            FieldPrefixType<IsFirst, PrettyPrint, IndentLevel, indent> FieldPrefix;
                
            template <bool IsFirst, bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            OutStreamType & operator<<(OutStreamType & os, const FieldPrefixType<IsFirst, PrettyPrint, IndentLevel, indent> &)
            {
                if constexpr ( PrettyPrint )
                {
                    if constexpr ( IsFirst )
                        os << osEndl << Indent<PrettyPrint, IndentLevel, indent>;
                    else 
                        os << "," << osEndl << Indent<PrettyPrint, IndentLevel, indent>;
                }
                else if constexpr ( !IsFirst )
                    os << ",";

                return os;
            }
                
            template <bool PrettyPrint>
            struct FieldNameValueSeparatorType { };

            template <bool PrettyPrint>
            FieldNameValueSeparatorType<PrettyPrint> FieldNameValueSeparator;
                
            template <bool PrettyPrint>
            OutStreamType & operator<<(OutStreamType & os, const FieldNameValueSeparatorType<PrettyPrint> &)
            {
                if constexpr ( PrettyPrint )
                    os << ": ";
                else
                    os << ":";

                return os;
            }
        };

        namespace Put
        {
            inline namespace Affix
            {
                template <bool PrettyPrint, const char* indent = twoSpaces>
                void Indent(OutStreamType & os, size_t indentLevel)
                {
                    if constexpr ( PrettyPrint )
                    {
                        for ( size_t i=0; i<indentLevel; i++ )
                            os << indent;
                    }
                }
                
                template <bool PrettyPrint, bool ContainsPrimitives, const char* indent = twoSpaces>
                void ArrayPrefix(OutStreamType & os, size_t indentLevel)
                {
                    if constexpr ( !PrettyPrint )
                        os << "[";
                    else if constexpr ( ContainsPrimitives )
                        os << "[ ";
                    else
                    {
                        os << "[" << osEndl;
                        Put::Indent<PrettyPrint, indent>(os, indentLevel);
                    }
                }

                template <bool PrettyPrint, bool ContainsPrimitives, const char* indent = twoSpaces>
                void ArraySuffix(OutStreamType & os, size_t indentLevel)
                {
                    if constexpr ( !PrettyPrint )
                        os << "]";
                    else if constexpr ( ContainsPrimitives )
                        os << " ]";
                    else
                    {
                        os << osEndl;
                        Put::Indent<PrettyPrint, indent>(os, indentLevel);
                        os << "]";
                    }
                }
                
                template <bool PrettyPrint, const char* indent>
                void ObjectPrefix(OutStreamType & os, size_t indentLevel)
                {
                    os << "{";
                }
                
                template <bool PrettyPrint, const char* indent = twoSpaces>
                void ObjectSuffix(OutStreamType & os, bool isEmpty, size_t indentLevel)
                {
                    if ( PrettyPrint && !isEmpty )
                    {
                        os << osEndl;
                        Put::Indent<PrettyPrint, indent>(os, indentLevel);
                        os << "}";
                    }
                    else
                        os << "}";
                }
                
                template <bool PrettyPrint, const char* indent = twoSpaces>
                void FieldPrefix(OutStreamType & os, bool isFirst, size_t indentLevel)
                {
                    if constexpr ( PrettyPrint )
                    {
                        if ( isFirst )
                        {
                            os << osEndl;
                            Put::Indent<PrettyPrint, indent>(os, indentLevel);
                        }
                        else
                        {
                            os << "," << osEndl;
                            Put::Indent<PrettyPrint, indent>(os, indentLevel);
                        }
                    }
                    else if ( !isFirst )
                        os << ",";
                }

                template <bool PrettyPrint, bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
                void NestedPrefix(OutStreamType & os, bool isEmpty)
                {
                    if constexpr ( IsArray )
                    {
                        if ( isEmpty )
                            os << StaticAffix::ArrayPrefix<false, ContainsPrimitives, IndentLevel, indent>;
                        else
                            os << StaticAffix::ArrayPrefix<PrettyPrint, ContainsPrimitives, IndentLevel, indent>;
                    }
                    else
                    {
                        if ( isEmpty )
                            os << StaticAffix::ObjectPrefix<false, IndentLevel, indent>;
                        else
                            os << StaticAffix::ObjectPrefix<PrettyPrint, IndentLevel, indent>;
                    }
                }

                template <bool PrettyPrint, const char* indent = twoSpaces>
                void NestedPrefix(OutStreamType & os, bool isArray, bool containsPrimitives, bool isEmpty, size_t indentLevel)
                {
                    if ( isArray )
                    {
                        if ( isEmpty )
                        {
                            if ( containsPrimitives )
                                Put::ArrayPrefix<false, true, indent>(os, indentLevel);
                            else
                                Put::ArrayPrefix<false, false, indent>(os, indentLevel);
                        }
                        else
                        {
                            if ( containsPrimitives )
                                Put::ArrayPrefix<PrettyPrint, true, indent>(os, indentLevel);
                            else
                                Put::ArrayPrefix<PrettyPrint, false, indent>(os, indentLevel);
                        }
                    }
                    else
                    {
                        if ( isEmpty )
                            Put::ObjectPrefix<false, indent>(os, indentLevel);
                        else
                            Put::ObjectPrefix<PrettyPrint, indent>(os, indentLevel);
                    }
                }
                
                template <bool PrettyPrint, bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
                void NestedSuffix(OutStreamType & os, bool isEmpty)
                {
                    if constexpr ( IsArray )
                    {
                        if ( isEmpty )
                            os << StaticAffix::ArraySuffix<false, ContainsPrimitives, IndentLevel, indent>;
                        else
                            os << StaticAffix::ArraySuffix<PrettyPrint, ContainsPrimitives, IndentLevel, indent>;
                    }
                    else
                    {
                        if ( isEmpty )
                            os << StaticAffix::ObjectSuffix<false, IndentLevel, indent>;
                        else
                            os << StaticAffix::ObjectSuffix<PrettyPrint, IndentLevel, indent>;
                    }
                }
                
                template <bool PrettyPrint, const char* indent = twoSpaces>
                void NestedSuffix(OutStreamType & os, bool isArray, bool containsPrimitives, bool isEmpty, size_t indentLevel)
                {
                    if ( isArray )
                    {
                        if ( isEmpty )
                        {
                            if ( containsPrimitives )
                                Put::ArraySuffix<false, true, indent>(os, indentLevel);
                            else
                                Put::ArraySuffix<false, false, indent>(os, indentLevel);
                        }
                        else
                        {
                            if ( containsPrimitives )
                                Put::ArraySuffix<PrettyPrint, true, indent>(os, indentLevel);
                            else
                                Put::ArraySuffix<PrettyPrint, false, indent>(os, indentLevel);
                        }
                    }
                    else
                    {
                        if ( isEmpty )
                            Put::ObjectSuffix<false, indent>(os, isEmpty, indentLevel);
                        else
                            Put::ObjectSuffix<PrettyPrint, indent>(os, isEmpty, indentLevel);
                    }
                }
                
                template <bool PrettyPrint, bool IsJsonField, bool NestedSeparator, size_t IndentLevel, const char* indent = twoSpaces>
                void Separator(OutStreamType & os, bool isFirst)
                {
                    if constexpr ( IsJsonField )
                    {
                        if ( isFirst && PrettyPrint )
                            os << osEndl << StaticAffix::Indent<PrettyPrint, IndentLevel, indent>;
                        else if constexpr ( PrettyPrint )
                            os << "," << osEndl << StaticAffix::Indent<PrettyPrint, IndentLevel, indent>;
                        else if ( !isFirst )
                            os << ",";
                    }
                    else if ( !isFirst )
                    {
                        if constexpr ( NestedSeparator && PrettyPrint )
                            os << "," << osEndl << StaticAffix::Indent<PrettyPrint, IndentLevel, indent>;
                        else if constexpr ( PrettyPrint )
                            os << ", ";
                        else
                            os << ",";
                    }
                }
                
                template <bool PrettyPrint, bool IsJsonField, bool NestedSeparator, const char* indent = twoSpaces>
                void Separator(OutStreamType & os, bool isFirst, size_t indentLevel)
                {
                    if constexpr ( IsJsonField )
                    {
                        if ( isFirst && PrettyPrint )
                        {
                            os << osEndl;
                            Put::Indent<PrettyPrint, indent>(os, indentLevel);
                        }
                        else if constexpr ( PrettyPrint )
                        {
                            os << "," << osEndl;
                            Put::Indent<PrettyPrint, indent>(os, indentLevel);
                        }
                        else if ( !isFirst )
                            os << ",";
                    }
                    else if ( !isFirst )
                    {
                        if constexpr ( NestedSeparator && PrettyPrint )
                        {
                            os << "," << osEndl;
                            Put::Indent<PrettyPrint, indent>(os, indentLevel);
                        }
                        else if constexpr ( PrettyPrint )
                            os << ", ";
                        else
                            os << ",";
                    }
                }
            };

            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, typename Value>
            static constexpr inline bool Customization(OutStreamType & os, Context & context, const Object & obj, const Value & value)
            {
                if constexpr ( is_specialized<Customize<Object, Value, Field::Index, Annotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent>>::value )
                {
                    return Customize<Object, Value, Field::Index, Annotations, Field, statics,
                        PrettyPrint, TotalParentIterables, IndentLevel, indent>::As(os, context, obj, value); // Customize fully specialized
                }
                else if constexpr ( is_specialized<Customize<Object, Value, Field::Index, Annotations, Field>>::value )
                    return Customize<Object, Value, Field::Index, Annotations, Field>::As(os, context, obj, value); // Five Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Value, Field::Index, NoAnnotation, Field>>::value )
                    return Customize<Object, Value, Field::Index, NoAnnotation, Field>::As(os, context, obj, value); // Customize<5args>, Annotations defaulted
                else if constexpr ( is_specialized<Customize<Object, Value, NoFieldIndex, Annotations, Field>>::value )
                    return Customize<Object, Value, NoFieldIndex, Annotations, Field>::As(os, context, obj, value); // Customize<5args>, FieldIndex defaulted
                else if constexpr ( is_specialized<Customize<Object, Value, NoFieldIndex, NoAnnotation, Field>>::value )
                    return Customize<Object, Value, NoFieldIndex, NoAnnotation, Field>::As(os, context, obj, value); // Customize<5args>, two args defaulted
                else if constexpr ( is_specialized<Customize<Object, Value, Field::Index, Annotations>>::value )
                    return Customize<Object, Value, Field::Index, Annotations>::As(os, context, obj, value); // Four Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Value, NoFieldIndex, Annotations>>::value )
                    return Customize<Object, Value, NoFieldIndex, Annotations>::As(os, context, obj, value); // Customize<4args>, FieldIndex defaulted
                else if constexpr ( is_specialized<Customize<Object, Value, Field::Index>>::value )
                    return Customize<Object, Value, Field::Index>::As(os, context, obj, value); // Three Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Value>>::value )
                    return Customize<Object, Value>::As(os, context, obj, value); // Two Customize arguments specialized
                else if constexpr ( is_specialized<CustomizeType<Value, Annotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent>>::value )
                {
                    return CustomizeType<Value, Annotations, Field, statics,
                        PrettyPrint, TotalParentIterables, IndentLevel, indent>::As(os, context, value); // CustomizeType fully specialized
                }
                else if constexpr ( is_specialized<CustomizeType<Value, Annotations, Field>>::value )
                    return CustomizeType<Value, Annotations, Field>::As(os, context, value); // Three CustomizeType arguments specialized
                else if constexpr ( is_specialized<CustomizeType<Value, NoAnnotation, Field>>::value )
                    return CustomizeType<Value, NoAnnotation, Field>::As(os, context, value); // CustomizeType<3args>, Annotations defaulted
                else if constexpr ( is_specialized<CustomizeType<Value, Annotations>>::value )
                    return CustomizeType<Value, Annotations>::As(os, context, value); // Two CustomizeType arguments specialized
                else if constexpr ( is_specialized<CustomizeType<Value>>::value )
                    return CustomizeType<Value>::As(os, context, value); // One CustomizeType argument specialized
                else
                    return false;
            }

            static void String(OutStreamType & os, const std::string & str)
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
            static void String(OutStreamType & os, const T & t)
            {
#ifdef USE_BUFFERED_STREAMS
                StringBuffer ss;
#else
                std::stringstream ss;
#endif
                ss << t;
                Put::String(os, ss.str());
            }

            template <typename Annotations, bool PrettyPrint, const char* indent>
            static void GenericIterable(OutStreamType & os, Context & context,
                size_t totalParentIterables, size_t indentLevel, const Generic::Value & iterable);
            
            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, bool IsFirst,
                bool IsComplexTuple = false, size_t TupleIndex = 0, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(OutStreamType & os, Context & context, const Object & obj, const std::tuple<T1, T2, Ts...> & value);

            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, bool IsFirst, typename Key, typename T>
            static constexpr void Pair(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & value);

            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, typename IterableValue = uint_least8_t>
            static constexpr void Iterable(OutStreamType & os, Context & context, const Object & obj, const IterableValue & iterable);
            
            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* indent, typename T>
            static constexpr void Object(OutStreamType & os, Context & context, const T & obj);

            template <typename Annotations, bool PrettyPrint, const char* indent, bool IsFirst>
            static void GenericValue(OutStreamType & os, Context & context,
                size_t totalParentIterables, size_t indentLevel, const Generic::Value & value)
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
                        Put::String(os, value.string());
                        break;
                    case Generic::Value::Type::Object:
                    case Generic::Value::Type::NullArray:
                    case Generic::Value::Type::BoolArray:
                    case Generic::Value::Type::NumberArray:
                    case Generic::Value::Type::StringArray:
                    case Generic::Value::Type::ObjectArray:
                    case Generic::Value::Type::MixedArray:
                        Put::GenericIterable<Annotations, PrettyPrint, indent>(os, context, totalParentIterables, indentLevel, value);
                        break;
                    case Generic::Value::Type::FieldCluster:
                    {
                        const std::map<std::string, std::shared_ptr<Generic::Value>> & fieldCluster = value.object();
                        bool isFirst = IsFirst;
                        for ( const auto & field : fieldCluster )
                        {
                            Put::FieldPrefix<PrettyPrint, indent>(os, isFirst, indentLevel+totalParentIterables);
                            Put::String(os, field.first);
                            os << FieldNameValueSeparator<PrettyPrint>;
                            if ( field.second == nullptr )
                                os << "null";
                            else
                            {
                                Put::GenericValue<Annotations, PrettyPrint, indent, false>(
                                    os, context, 0, indentLevel+totalParentIterables, (Generic::Value &)*field.second);
                            }

                            isFirst = false;
                        }
                    }
                    break;
                }
            }
            
            template <typename Annotations, bool PrettyPrint, const char* indent>
            static void GenericIterable(OutStreamType & os, Context & context,
                size_t totalParentIterables, size_t indentLevel, const Generic::Value & iterable)
            {
                bool isObject = iterable.type() == Generic::Value::Type::Object;
                bool containsPrimitives = iterable.type() == Generic::Value::Type::BoolArray ||
                    iterable.type() == Generic::Value::Type::NumberArray || iterable.type() == Generic::Value::Type::StringArray;
                bool isEmpty = (isObject && iterable.object().empty()) || (!isObject && iterable.arraySize() == 0);

                size_t i=0;
                Put::NestedPrefix<PrettyPrint, indent>(os, !isObject, containsPrimitives, isEmpty, indentLevel+totalParentIterables+1);
                switch ( iterable.type() )
                {
                    case Generic::Value::Type::Object:
                    {
                        const std::map<std::string, std::shared_ptr<Generic::Value>> & obj = iterable.object();
                        bool isFirst = true;
                        for ( const auto & field : obj )
                        {
                            Put::FieldPrefix<PrettyPrint, indent>(os, isFirst, indentLevel+totalParentIterables+1);
                            Put::String(os, field.first);
                            os << FieldNameValueSeparator<PrettyPrint>;
                            Put::GenericValue<Annotations, PrettyPrint, indent, false>(
                                os, context, 0, indentLevel+totalParentIterables+1, (Generic::Value &)*field.second);
                            isFirst = false;
                        }
                    }
                    break;
                    case Generic::Value::Type::NullArray:
                    {
                        for ( size_t i=0; i<iterable.nullArray(); i++ )
                        {
                            Put::Separator<PrettyPrint, false, false, indent>(os, 0 == i, indentLevel+totalParentIterables+1);
                            os << "null";
                        }
                    }
                    break;
                    case Generic::Value::Type::BoolArray:
                    {
                        const std::vector<bool> & array = iterable.boolArray();
                        for ( const auto & element : array )
                        {
                            Put::Separator<PrettyPrint, false, false, indent>(os, 0 == i++, indentLevel+totalParentIterables+2);
                            os << (element ? "true" : "false");
                        }
                    }
                    break;
                    case Generic::Value::Type::NumberArray:
                    {
                        const std::vector<std::string> & array = iterable.numberArray();
                        for ( const auto & element : array )
                        {
                            Put::Separator<PrettyPrint, false, false, indent>(os, 0 == i++, indentLevel+totalParentIterables+2);
                            os << element;
                        }
                    }
                    break;
                    case Generic::Value::Type::StringArray:
                    {
                        const std::vector<std::string> & array = iterable.stringArray();
                        for ( const auto & element : array )
                        {
                            Put::Separator<PrettyPrint, false, false, indent>(os, 0 == i++, indentLevel+totalParentIterables+2);
                            os << "\"" << element << "\"";
                        }
                    }
                    break;
                    case Generic::Value::Type::ObjectArray:
                    {
                        const std::vector<std::map<std::string, std::shared_ptr<Generic::Value>>> & array = iterable.objectArray();
                        for ( const std::map<std::string, std::shared_ptr<Generic::Value>> & obj : array )
                        {
                            Put::Separator<PrettyPrint, false, true, indent>(os, 0 == i++, indentLevel+totalParentIterables+2);
                            bool isFirst = true;
                            Put::ObjectPrefix<PrettyPrint, indent>(os, totalParentIterables+indentLevel);
                            for ( const auto & field : obj )
                            {
                                Put::FieldPrefix<PrettyPrint, indent>(os, isFirst, totalParentIterables+indentLevel+2);
                                Put::String(os, field.first);
                                os << FieldNameValueSeparator<PrettyPrint>;
                                Put::GenericValue<Annotations, PrettyPrint, indent, false>(
                                    os, context, totalParentIterables+1, indentLevel, (Generic::Value &)*field.second);
                                isFirst = false;
                            }
                            Put::ObjectSuffix<PrettyPrint, indent>(os, obj.empty(), totalParentIterables+indentLevel+1);
                        }
                    }
                    break;
                    case Generic::Value::Type::MixedArray:
                    {
                        const std::vector<std::shared_ptr<Generic::Value>> & array = iterable.mixedArray();
                        for ( const std::shared_ptr<Generic::Value> & element : array )
                        {
                            Put::Separator<PrettyPrint, false, true, indent>(os, 0 == i++, indentLevel+totalParentIterables+1);
                            if ( element == nullptr )
                                os << "null";
                            else
                                Put::GenericValue<Annotations, PrettyPrint, indent, false>(os, context, totalParentIterables+1, indentLevel, *element);
                        }
                    }
                    break;
                }
                Put::NestedSuffix<PrettyPrint, indent>(os, !isObject, containsPrimitives, isEmpty, indentLevel+totalParentIterables);
            }

            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, bool IsFirst, typename T>
            static constexpr void Value(OutStreamType & os, Context & context, const Object & obj, const T & value)
            {
                if constexpr ( Customizers::HaveSpecialization<Object, T, Field::Index, Annotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent> )
                {
                    if ( Put::Customization<Annotations, Field, statics,
                        PrettyPrint, TotalParentIterables, IndentLevel, indent, Object, T>(os, context, obj, value) )
                    {
                        return;
                    }
                }

                if constexpr ( is_pointable<T>::value )
                {
                    if ( value == nullptr )
                        os << "null";
                    else
                    {
                        Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object, IsFirst>(
                            os, context, obj, *value);
                    }
                }
                else if constexpr ( std::is_base_of<Generic::Value, T>::value )
                {
                    Put::GenericValue<Annotations, PrettyPrint, indent, IsFirst>(os, context, TotalParentIterables,
                        IndentLevel+((Field::template HasAnnotation<IsRoot> ) ? 0 : 1), (const Generic::Value &)value);
                }
                else if constexpr ( is_tuple<T>::value )
                {
                    if constexpr ( std::tuple_size<T>::value == 0 )
                        os << "null";
                    else if constexpr ( std::tuple_size<T>::value == 1 )
                    {
                        Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object, IsFirst>(
                            os, context, obj, std::get<0>(value));
                    }
                    else if constexpr ( std::tuple_size<T>::value >= 2 )
                    {
                        Put::Tuple<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object, IsFirst>(
                            os, context, obj, value);
                    }
                }
                else if constexpr ( is_pair<T>::value )
                    Put::Pair<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object, IsFirst>(os, context, obj, value);
                else if constexpr ( is_iterable<T>::value )
                    Put::Iterable<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object>(os, context, obj, value);
                else if constexpr ( Field::template HasAnnotation<IsRoot> )
                    Put::Object<Annotations, statics, PrettyPrint, IndentLevel+TotalParentIterables, indent, T>(os, context, value);
                else if constexpr ( is_reflected<T>::value )
                    Put::Object<Annotations, statics, PrettyPrint, IndentLevel+TotalParentIterables+1, indent, T>(os, context, value);
                else if constexpr ( Field::template HasAnnotation<Json::StringifyType> )
                    Put::String(os, value);
                else if constexpr ( Field::template HasAnnotation<Json::EnumIntType> )
                    os << (typename promote_char<typename std::underlying_type<T>::type>::type)value;
                else if constexpr ( is_bool<T>::value )
                    os << (value ? "true" : "false");
                else if constexpr ( is_string<T>::value && !Field::template HasAnnotation<Json::UnstringType> )
                    Put::String(os, value);
                else if constexpr ( std::is_enum<T>::value )
                    os << (typename promote_char<typename std::underlying_type<T>::type>::type)value;
                else
                    os << (typename promote_char<T>::type)value;
            }
            
            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, bool IsFirst,
                bool IsComplexTuple, size_t TupleIndex, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(OutStreamType & os, Context & context, const Object & obj, const std::tuple<T1, T2, Ts...> & value)
            {
                constexpr bool isComplexTuple = TupleIndex == 0 ? is_non_primitive_tuple<std::tuple<T1, T2, Ts...>>::value : IsComplexTuple;
                constexpr size_t tupleSize = std::tuple_size<typename std::remove_reference_t<decltype(value)>>::value;
                if constexpr ( TupleIndex == 0 )
                    Put::ArrayPrefix<PrettyPrint, !isComplexTuple, indent>(os, IndentLevel+TotalParentIterables+2);
                if constexpr ( TupleIndex < tupleSize )
                {
                    Put::Separator<PrettyPrint, false, isComplexTuple, IndentLevel+TotalParentIterables+2, indent>(os, TupleIndex == 0);
                    Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, IsFirst>(
                        os, context, obj, std::get<TupleIndex>(value));
                    Put::Tuple<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object, IsFirst,
                        isComplexTuple, TupleIndex+1>(os, context, obj, value);
                }
                else if constexpr ( TupleIndex == tupleSize )
                    Put::ArraySuffix<PrettyPrint, !isComplexTuple, indent>(os, IndentLevel+TotalParentIterables+1);
            }
            
            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, bool IsFirst, typename Key, typename T>
            static constexpr void Pair(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & value)
            {
                constexpr bool isComplexPair = is_non_primitive<Key>::value || is_non_primitive<T>::value;
                Put::ArrayPrefix<PrettyPrint, !isComplexPair, indent>(os, IndentLevel+TotalParentIterables+2);
                Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, IsFirst>(
                    os, context, obj, value.first);
                Put::Separator<PrettyPrint, false, isComplexPair, IndentLevel+TotalParentIterables+2, indent>(os, false);
                Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, IsFirst>(
                    os, context, obj, value.second);
                Put::ArraySuffix<PrettyPrint, !isComplexPair, indent>(os, IndentLevel+TotalParentIterables+1);
            }

            template<typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, bool IsFirst, typename Key, typename T>
            static constexpr void KeyValueObject(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & pair)
            {
                os << StaticAffix::ObjectPrefix<PrettyPrint, IndentLevel, indent, statics>;
                os << StaticAffix::FieldPrefix<true, PrettyPrint, TotalParentIterables+IndentLevel+2, indent>;
                Put::String(os, "key");
                os << FieldNameValueSeparator<PrettyPrint>;
                Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, true>(
                    os, context, obj, pair.first);
                os << StaticAffix::FieldPrefix<false, PrettyPrint, TotalParentIterables+IndentLevel+2, indent>;
                Put::String(os, "value");
                os << FieldNameValueSeparator<PrettyPrint>;
                Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, false>(
                    os, context, obj, pair.second);
                os << StaticAffix::ObjectSuffix<PrettyPrint, TotalParentIterables+IndentLevel+1, indent, statics>;
            }
            
            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, bool IsFirst, typename T, typename Key>
            static constexpr void FieldPair(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & pair)
            {
                Put::String(os, pair.first);
                os << FieldNameValueSeparator<PrettyPrint>;
                Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object, IsFirst>(
                    os, context, obj, pair.second);
            }

            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, typename IterableValue>
            static constexpr void Iterable(OutStreamType & os, Context & context, const Object & obj, const IterableValue & iterable)
            {
                using Element = typename element_type<IterableValue>::type;
                constexpr bool IsMap = is_map<IterableValue>::value; // Simple maps are just json objects with keys as the field names, values as field values
                constexpr bool HasComplexKey = IsMap && is_non_primitive<typename pair_lhs<Element>::type>::value;
                constexpr bool IsArray = !IsMap || HasComplexKey; // Maps with complex keys are arrays consisting of objects with two fields: "key" and "value"
                constexpr bool ContainsIterables = is_iterable<typename pair_rhs<Element>::type>::value || HasComplexKey;
                constexpr bool ContainsPrimitives = !is_reflected<Element>::value && !ContainsIterables;

                size_t i=0;
                Put::NestedPrefix<PrettyPrint, IsArray, ContainsPrimitives, IndentLevel+TotalParentIterables+2, indent>(os, IsEmpty(iterable));
                if constexpr ( is_stl_iterable<IterableValue>::value )
                {
                    for ( auto & element : iterable )
                    {
                        Put::Separator<PrettyPrint, !IsArray, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(os, 0 == i++);
                        if constexpr ( !IsMap )
                        {
                            Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, false>(
                                os, context, obj, element);
                        }
                        else if constexpr ( HasComplexKey )
                        {
                            Put::KeyValueObject<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, false>(
                                os, context, obj, element);
                        }
                        else
                        {
                            Put::FieldPair<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, false>(
                                os, context, obj, element);
                        }
                    }
                }
                else if constexpr ( is_adaptor<IterableValue>::value )
                {
                    const auto & sequenceContainer = get_underlying_container(iterable);
                    for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    {
                        Put::Separator<PrettyPrint, !IsArray, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(os, 0 == i++);
                        Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, false>(
                            os, context, obj, *it);
                    }
                }
                else if constexpr ( std::is_array<IterableValue>::value && std::extent<typename Field::Type>::value > 0 )
                {
                    for ( ; i<std::extent<typename Field::Type>::value; i++ )
                    {
                        Put::Separator<PrettyPrint, !IsArray, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(os, 0 == i);
                        Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object, false>(
                            os, context, obj, iterable[i]);
                    }
                }
                Put::NestedSuffix<PrettyPrint, IsArray, ContainsPrimitives, IndentLevel+TotalParentIterables+1, indent>(os, IsEmpty(iterable));
            }

            template <typename Annotations, typename FieldClass, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* indent, typename Object, typename FieldName>
            static constexpr void Field(OutStreamType & os, Context & context, const Object & obj, FieldName fieldName,
                const typename FieldClass::Type & value)
            {
                if constexpr ( matches_statics<FieldClass::IsStatic, statics>::value )
                {
                    if constexpr ( std::is_base_of<Generic::FieldCluster, typename remove_pointer<typename FieldClass::Type>::type>::value )
                    {
                        if constexpr ( FieldClass::Index == FirstIndex<statics, Object>() )
                            throw Exception("Json::Generic::FieldCluster cannot be the first or the only field in an object");
                        else if constexpr ( is_pointable<typename FieldClass::Type>::value )
                        {
                            if ( value != nullptr )
                                Put::Value<Annotations, FieldClass, statics, PrettyPrint, 0, IndentLevel, indent, Object, false>(os, context, obj, *value);
                        }
                        else
                            Put::Value<Annotations, FieldClass, statics, PrettyPrint, 0, IndentLevel, indent, Object, false>(os, context, obj, value);
                    }
                    else if constexpr ( !FieldClass::template HasAnnotation<Json::IgnoreType> )
                    {
                        os << StaticAffix::FieldPrefix<FieldClass::Index == FirstIndex<statics, Object>(), PrettyPrint, IndentLevel+1, indent>;
                        Put::String(os, fieldName);
                        os << FieldNameValueSeparator<PrettyPrint>;
                        Put::Value<Annotations, FieldClass, statics, PrettyPrint, 0, IndentLevel, indent,
                            Object, FieldClass::Index == FirstIndex<statics, Object>()>(os, context, obj, value);
                    }
                }
            }
            
            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* indent, typename Object>
            static constexpr void Fields(OutStreamType & os, Context & context, const Object & obj)
            {
                Object::Class::ForEachField(obj, [&](auto & field, auto & value)
                {
                    using Field = typename std::remove_reference<decltype(field)>::type;
                    if constexpr ( Field::template HasAnnotation<Json::Name> )
                    {
                        const auto & fieldName = field.template getAnnotation<Json::Name>().value;
                        Put::Field<Annotations, Field, statics, PrettyPrint, IndentLevel, indent, Object>(os, context, obj, fieldName, value);
                    }
                    else
                        Put::Field<Annotations, Field, statics, PrettyPrint, IndentLevel, indent, Object>(os, context, obj, field.name, value);
                });
            }

            template <typename Annotations, size_t SuperIndex, typename T, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* indent, typename Object, typename FieldName>
            static constexpr void Super(OutStreamType & os, Context & context, const Object & obj, const FieldName & superFieldName)
            {
                if constexpr ( HasFields<statics, T>() )
                {
                    constexpr bool IsFirst = !HasFields<statics, Object, false>() && SuperIndex == FirstSuperIndex<statics, Object>();
                    os << StaticAffix::FieldPrefix<IsFirst, PrettyPrint, IndentLevel+1, indent>;
                    Put::String(os, superFieldName);
                    os << FieldNameValueSeparator<PrettyPrint>;
                    Put::Object<Annotations, statics, PrettyPrint, IndentLevel+1, indent, T>(os, context, obj);
                }
            }
            
            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* indent, typename Object>
            static constexpr void Supers(OutStreamType & os, Context & context, const Object & obj)
            {
                Object::Supers::ForEach(obj, [&](auto superInfo, auto & superObj)
                {
                    using SuperInfo = decltype(superInfo);
                    using Super = typename std::remove_reference<decltype(superObj)>::type;
                    if constexpr ( SuperInfo::template HasAnnotation<Json::Name> )
                    {
                        const auto & superName = superInfo.template getAnnotation<Json::Name>().value;
                        Put::Super<Annotations, decltype(superInfo)::Index, Super, statics, PrettyPrint, IndentLevel, indent, Object>(
                            os, context, obj, superName);
                    }
                    else
                    {
                        Put::Super<Annotations, decltype(superInfo)::Index, Super, statics, PrettyPrint, IndentLevel, indent, Object>(
                            os, context, obj, superTypeToJsonFieldName<Super>());
                    }
                });
            }

            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* indent, typename T>
            static constexpr void Object(OutStreamType & os, Context & context, const T & obj)
            {
                os << StaticAffix::ObjectPrefix<PrettyPrint, IndentLevel, indent, statics, T>;
                Put::Fields<Annotations, statics, PrettyPrint, IndentLevel, indent, T>(os, context, obj);
                Put::Supers<Annotations, statics, PrettyPrint, IndentLevel, indent, T>(os, context, obj);
                os << StaticAffix::ObjectSuffix<PrettyPrint, IndentLevel, indent, statics, T>;
            }
        }
        
        template <typename Annotations = NoAnnotation, Statics statics = Statics::Excluded,
            bool PrettyPrint = false, size_t IndentLevel = 0, const char* indent = twoSpaces, typename Object = uint_least8_t>
        class ReflectedObject
        {
        public:
            ReflectedObject(const Object & obj, std::shared_ptr<Context> context) : obj(obj), context(context) {}

            const Object & obj;
            std::shared_ptr<Context> context;
            
            constexpr OutStreamType & put(OutStreamType & os)
            {
                if ( context == nullptr )
                    context = std::shared_ptr<Context>(new Context());

                Put::Value<Annotations, ReflectedField<Object>, statics, PrettyPrint, 0, IndentLevel, indent, Object, true, Object>(os, *context, obj, obj);
                return os;
            }
        };
        
#ifdef USE_BUFFERED_STREAMS
        template <Statics statics = Statics::Excluded, typename Annotations = NoAnnotation, bool PrettyPrint = false,
            size_t IndentLevel = 0, const char* indent = twoSpaces, typename T = uint_least8_t>
        StringBuffer & operator<<(StringBuffer & os, Output::ReflectedObject<Annotations, statics, PrettyPrint, IndentLevel, indent, T> object)
        {
            return object.put(os);
        }

        template <Statics statics = Statics::Excluded, typename Annotations = NoAnnotation, bool PrettyPrint = false,
            size_t IndentLevel = 0, const char* indent = twoSpaces, typename T = uint_least8_t>
        std::ostream & operator<<(StringBufferPtr os, Output::ReflectedObject<Annotations, statics, PrettyPrint, IndentLevel, indent, T> object)
        {
            return object.put(*os);
        }
#else
        template <Statics statics = Statics::Excluded, typename Annotations = NoAnnotation, bool PrettyPrint = false,
            size_t IndentLevel = 0, const char* indent = twoSpaces, typename T = uint_least8_t>
        std::ostream & operator<<(std::ostream & os, Output::ReflectedObject<Annotations, statics, PrettyPrint, IndentLevel, indent, T> object)
        {
            return object.put(os);
        }
#endif

        template <Statics statics = Statics::Excluded, typename Annotations = NoAnnotation,
            size_t IndentLevel = 0, const char* indent = twoSpaces, typename T = uint_least8_t>
        Output::ReflectedObject<Annotations, statics, false, IndentLevel, indent, T> out(const T & t, std::shared_ptr<Context> context = nullptr)
        {
            return Output::ReflectedObject<Annotations, statics, false, IndentLevel, indent, T>(t, context);
        }
    
        template <Statics statics = Statics::Excluded, typename Annotations = NoAnnotation,
            size_t IndentLevel = 0, const char* indent = twoSpaces, typename T = uint_least8_t>
        Output::ReflectedObject<Annotations, statics, true, IndentLevel, indent, T> pretty(const T & t, std::shared_ptr<Context> context = nullptr)
        {
            return Output::ReflectedObject<Annotations, statics, true, IndentLevel, indent, T>(t, context);
        }
    };
    
    inline namespace Input
    {
        inline namespace Customizers
        {
            template <typename Object, typename Value, size_t FieldIndex = NoFieldIndex, typename OpAnnotations = NoAnnotation, typename Field = NoField>
            struct Customize : public Unspecialized
            {
                /// return false if you wish for the input to be re-parsed by the default JSON code, else return true
                /// For invalid input you can throw an exception to end parsing immediately, or return true to continue parsing
                static bool As(std::istream & input, Context & context, const Object & object, Value & value) { return false; }
            };
            
            template <typename Value, typename OpAnnotations = NoAnnotation, typename Field = NoField>
            struct CustomizeType : public Unspecialized
            {
                /// Should return true if you put any output, else you should leave output unchanged
                static bool As(std::istream & input, Context & context, Value & value) { return false; }
            };

            template <typename Object, typename Value, size_t FieldIndex = NoFieldIndex, typename OpAnnotations = NoAnnotation, typename Field = NoField>
            static constexpr bool HaveSpecialization =
                is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations, Field>>::value || // Fully specialized
                is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations>>::value || // Customize<4args> specialized
                is_specialized<Customize<Object, Value, FieldIndex>>::value || // Customize<3args> specialized
                is_specialized<Customize<Object, Value>>::value || // Customize<2args> specialized
                is_specialized<Customize<Object, Value, FieldIndex, NoAnnotation, Field>>::value || // Customize<5args>, OpAnnotations defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, OpAnnotations, Field>>::value || // Customize<5args>, FieldIndex defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, NoAnnotation, Field>>::value || // Customize<5args>, both defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, OpAnnotations>>::value || // Customize<4args>, FieldIndex defaulted
                is_specialized<CustomizeType<Value, OpAnnotations, Field>>::value || // Fully specialized
                is_specialized<CustomizeType<Value, OpAnnotations>>::value || // CustomizeType<2args> specialized
                is_specialized<CustomizeType<Value>>::value || // CustomizeType<1arg> specialized
                is_specialized<CustomizeType<Value, NoAnnotation, Field>>::value; // CustomizeType<3args>, OpAnnotations defaulted
        }

        inline namespace Exceptions
        {
            static constexpr const char unicodeEscapeSequence[] = "\\uHHHH (where H's are hex characters)";

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
        };

        inline namespace Cache
        {
            static inline std::hash<std::string> strHash;

            static inline std::map<std::type_index, std::multimap<size_t, JsonField>> classToNameHashToJsonField;

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
                            Class::FieldAt(fieldIndex, [&](auto & field) {
                                using Field = typename std::remove_reference<decltype(field)>::type;
                                if constexpr ( std::is_base_of<Generic::FieldCluster, typename remove_pointer<typename Field::Type>::type>::value )
                                {
                                    std::string fieldName = fieldClusterToJsonFieldName();
                                    inserted.first->second.insert(std::pair<size_t, JsonField>(
                                        strHash(fieldName), JsonField(fieldIndex, JsonField::Type::FieldCluster, fieldName)));
                                }
                                else if constexpr ( !Field::template HasAnnotation<Json::IgnoreType> )
                                {
                                    if constexpr ( Field::template HasAnnotation<Json::Name> )
                                    {
                                        std::string fieldName = std::string(field.template getAnnotation<Json::Name>().value);
                                        inserted.first->second.insert(std::pair<size_t, JsonField>(
                                            strHash(fieldName.c_str()), JsonField(fieldIndex, JsonField::Type::Regular, fieldName.c_str())));
                                    }
                                    else
                                    {
                                        inserted.first->second.insert(std::pair<size_t, JsonField>(
                                            strHash(Class::Fields[fieldIndex].name),
                                            JsonField(fieldIndex, JsonField::Type::Regular, Class::Fields[fieldIndex].name)));
                                    }
                                }
                            });
                        }
                    }

                    if constexpr ( Supers::TotalSupers > 0 )
                    {
                        for ( size_t superIndex = 0; superIndex < Supers::TotalSupers; superIndex++ )
                        {
                            Supers::At(t, superIndex, [&](auto superInfo, auto & superObj) {
                                using SuperInfo = decltype(superInfo);
                                using Super = typename std::remove_reference<decltype(superObj)>::type;
                                if constexpr ( HasFields<Statics::Included, Super>() )
                                {
                                    if constexpr ( SuperInfo::template HasAnnotation<Json::Name> )
                                    {
                                        std::string superName = std::string(superInfo.template getAnnotation<Json::Name>().value);
                                        inserted.first->second.insert(std::pair<size_t, JsonField>(
                                            strHash(superName), JsonField(superIndex, JsonField::Type::SuperClass, superName)));
                                    }
                                    else
                                    {
                                        std::string superTypeFieldName = superTypeToJsonFieldName<Super>();
                                        inserted.first->second.insert(std::pair<size_t, JsonField>(
                                            strHash(superTypeFieldName), JsonField(superIndex, JsonField::Type::SuperClass, superTypeFieldName)));
                                    }
                                }
                            });
                        }
                    }
                            
                    return inserted.first->second;
                }
                return found->second;
            }

            template <typename T>
            static JsonField* getJsonField(T & t, const std::string & fieldName)
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
        };

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
            static constexpr inline bool tryGet(std::istream & is, int character, int secondaryCharacter,
                const char* expectedDescription, const char* secondaryDescription)
            {
                if ( usePrimary )
                    return tryGet(is, character, expectedDescription);
                else
                    return tryGet(is, secondaryCharacter, secondaryDescription);
            }

            static bool getTrueFalse(std::istream & is, char trueChar, char falseChar, const char* expectedDescription)
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
            static constexpr inline bool getTrueFalse(std::istream & is, char trueChar, char falseChar, char secondaryFalseChar,
                const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    return getTrueFalse(is, trueChar, falseChar, expectedDescription);
                else
                    return getTrueFalse(is, trueChar, secondaryFalseChar, secondaryDescription);
            }

            template <bool IgnoreWhitespace = true>
            static void get(std::istream & is, char & c, char expectation, const char* expectedDescription)
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

            template <bool usePrimary>
            static constexpr inline void get(std::istream & is, char & c, const char* expectedDescription, const char* secondaryDescription)
            {
                if constexpr ( usePrimary )
                    get(is, c, expectedDescription);
                else
                    get(is, c, secondaryDescription);
            }

            template <bool usePrimary>
            static constexpr inline void get(std::istream & is, char & c, char expectation, char secondaryExpectation, const char* expectedDescription,
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
            static constexpr inline void consumeWhitespace(std::istream & is, const char* expectedDescription, const char* secondaryDescription)
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
            static constexpr void Null(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                int expectation[] = { 'n', 'u', 'l', 'l' };
                for ( size_t i=0; i<4; i++ )
                    Checked::get<false>(is, c, expectation[i], "completion of field value");

                Checked::consumeWhitespace(is, "\",\" or \"}\"");
                Checked::peek(is, c, "\",\" or \"}\"");
                if ( InArray && c != ',' && c != ']' )
                    throw Exception("Expected: \",\" or \"]\"");
                else if ( !InArray && c != ',' && c != '}' )
                    throw Exception("Expected: \",\" or \"}\"");
            }

            template <bool InArray>
            static constexpr void Null(std::istream & is, char & c, std::stringstream & ss)
            {
                Consume::Null<InArray>(is, c);
                ss << "null";
            }

            template <bool InArray>
            static constexpr bool TryNull(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "null or field value");
                Checked::peek(is, c, "null or field value");
                if ( c == 'n' )
                {
                    int expectation[] = { 'n', 'u', 'l', 'l' };
                    for ( size_t i=0; i<4; i++ )
                        Checked::get<false>(is, c, expectation[i], "completion of \"null\"");

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
            static constexpr void True(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                int expectation[] = { 't', 'r', 'u', 'e' };
                for ( size_t i=0; i<4; i++ )
                    Checked::get<false>(is, c, expectation[i], "completion of field value");

                Checked::consumeWhitespace(is, "\",\" or \"}\"");
                Checked::peek(is, c, "\",\" or \"}\"");
                if ( InArray && c != ',' && c != ']' )
                    throw Exception("Expected: \",\" or \"]\"");
                else if ( !InArray && c != ',' && c != '}' )
                    throw Exception("Expected: \",\" or \"}\"");
            }

            template <bool InArray>
            static constexpr void True(std::istream & is, char & c, std::stringstream & ss)
            {
                Consume::True<InArray>(is, c);
                ss << "true";
            }
            
            template <bool InArray>
            static constexpr void False(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                int expectation[] = { 'f', 'a', 'l', 's', 'e' };
                for ( size_t i=0; i<5; i++ )
                    Checked::get<false>(is, c, expectation[i], "completion of field value");

                Checked::consumeWhitespace(is, "\",\" or \"}\"");
                Checked::peek(is, c, "\",\" or \"}\"");
                if ( InArray && c != ',' && c != ']' )
                    throw Exception("Expected: \",\" or \"]\"");
                else if ( !InArray && c != ',' && c != '}' )
                    throw Exception("Expected: \",\" or \"}\"");
            }

            template <bool InArray>
            static constexpr void False(std::istream & is, char & c, std::stringstream & ss)
            {
                Consume::False<InArray>(is, c);
                ss << "false";
            }

            template <bool InArray>
            static constexpr void Number(std::istream & is, char & c)
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
            
            template <bool InArray>
            static constexpr void Number(std::istream & is, char & c, std::stringstream & ss)
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

            static void String(std::istream & is, char & c)
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
                        default: break;
                    }
                } while ( c != '\"' );
            }

            static void String(std::istream & is, char & c, std::stringstream & ss)
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
                        default: break;
                    }
                } while ( c != '\"' );
            }
            
            template <bool IsArray>
            static constexpr void Iterable(std::istream & is, char & c);
            
            template <bool IsArray>
            static constexpr void Iterable(std::istream & is, char & c, std::stringstream & ss);

            template <bool InArray>
            static constexpr void Value(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                switch ( c )
                {
                    case '\"': Consume::String(is, c); break; // String or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9': Consume::Number<InArray>(is, c); break; // Number or error
                    case '{': Consume::Iterable<false>(is, c); break; // JSON object or error
                    case '[': Consume::Iterable<true>(is, c); break; // JSON array or error
                    case 't': Consume::True<InArray>(is, c); break; // "true" or error
                    case 'f': Consume::False<InArray>(is, c); break; // "false" or error
                    case 'n': Consume::Null<InArray>(is, c); break; // "null" or error
                    default: throw InvalidUnknownFieldValue(); break;
                }
            }
            
            template <bool InArray>
            static constexpr void Value(std::istream & is, char & c, std::stringstream & ss)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                switch ( c )
                {
                    case '\"': Consume::String(is, c, ss); break; // String or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9': Consume::Number<InArray>(is, c, ss); break; // Number or error
                    case '{': Consume::Iterable<false>(is, c, ss); break; // JSON object or error
                    case '[': Consume::Iterable<true>(is, c, ss); break; // JSON array or error
                    case 't': Consume::True<InArray>(is, c, ss); break; // "true" or error
                    case 'f': Consume::False<InArray>(is, c, ss); break; // "false" or error
                    case 'n': Consume::Null<InArray>(is, c, ss); break; // "null" or error
                    default: throw InvalidUnknownFieldValue(); break;
                }
            }

            template <bool IsArray>
            static constexpr void Iterable(std::istream & is, char & c)
            {
                Checked::get<IsArray>(is, c, '[', '{', "array opening \"[\"", "object opening \"{\"");
                if ( !Checked::tryGet<IsArray>(is, ']', '}', "array closing \"]\" or array element", "object closing \"}\" or field name opening \"") )
                {
                    do
                    {
                        if constexpr ( !IsArray )
                        {
                            Consume::String(is, c);
                            Checked::get(is, c, ':', "field name-value separator \":\"");
                        }
                        Consume::Value<IsArray>(is, c);
                    }
                    while ( Checked::getTrueFalse<IsArray>(is, ',', ']', '}', "\",\" or array closing \"]\"", "\",\" or object closing \"}\"") );
                }
            }

            template <bool IsArray>
            static constexpr void Iterable(std::istream & is, char & c, std::stringstream & ss)
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
                            Consume::String(is, c, ss);
                            Checked::get(is, c, ':', "field name-value separator \":\"");
                            ss << ':';
                        }
                        Consume::Value<IsArray>(is, c, ss);
                    }
                    while ( Checked::getTrueFalse<IsArray>(is, ',', ']', '}', "\",\" or array closing \"]\"", "\",\" or object closing \"}\"") );
                }
                ss << (IsArray ? ']' : '}');
            }
        };

        namespace Read
        {
            inline namespace Affix
            {
                inline void ObjectPrefix(std::istream & is, char & c)
                {
                    Checked::get(is, c, '{', "object opening \"{\"");
                }

                inline bool TryObjectSuffix(std::istream & is)
                {
                    return Checked::tryGet(is, '}', "object closing \"}\" or field name opening \"");
                }

                inline bool FieldSeparator(std::istream & is)
                {
                    return Checked::getTrueFalse(is, ',', '}', "\",\" or object closing \"}\"");
                }

                inline void FieldNameValueSeparator(std::istream & is, char & c)
                {
                    Checked::get(is, c, ':', "field name-value separator \":\"");
                }

                inline void ArrayPrefix(std::istream & is, char & c)
                {
                    Checked::get(is, c, '[', "array opening \"[\"");
                }

                inline bool TrySingularTupleArrayPrefix(std::istream & is, char & c)
                {
                    return Checked::tryGet(is, '[', "array opening \"[\" or tuple value");
                }

                inline bool TryArraySuffix(std::istream & is)
                {
                    return Checked::tryGet(is, ']', "array closing \"]\" or array element");
                }

                template <bool IsObject>
                inline void IterablePrefix(std::istream & is, char & c)
                {
                    Checked::get<IsObject>(is, c, '{', '[', "object opening \"{\"", "array opening \"[\"");
                }

                inline bool IterablePrefix(std::istream & is, char & c)
                {
                    if ( Checked::tryGet(is, '{', "object opening \"{\" or array opening \"[\"") )
                        return true;

                    Checked::get<true>(is, c, '[', "object opening \"{\" or array opening \"[\"");
                    return false;
                }

                inline bool PeekIterablePrefix(std::istream & is, char & c)
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
                inline bool TryIterableSuffix(std::istream & is)
                {
                    return Checked::tryGet<IsObject>(is, '}', ']', "object closing \"}\" or field name opening \"", "array closing \"]\" or array element");
                }

                inline bool TryIterableSuffix(std::istream & is, bool isObject)
                {
                    if ( isObject )
                        return Checked::tryGet(is, '}', "object closing \"}\" or field name opening \"");
                    else
                        return Checked::tryGet(is, ']', "array closing \"]\" or array element");
                }

                template <bool IsObject>
                inline bool IterableElementSeparator(std::istream & is)
                {
                    return Checked::getTrueFalse<IsObject>(is, ',', '}', ']', "\",\" or object closing \"}\"", "\",\" or array closing \"]\"");
                }
            }

            template <typename Object, typename Value, size_t FieldIndex, typename OpAnnotations, typename Field>
            static constexpr inline bool Customization(std::istream & is, Context & context, Object & obj, Value & value)
            {
                if constexpr ( is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations, Field>>::value )
                    return Customize<Object, Value, FieldIndex, OpAnnotations, Field>::As(is, context, obj, value); // Customize fully specialized
                else if constexpr ( is_specialized<Customize<Object, Value, FieldIndex, NoAnnotation, Field>>::value )
                    return Customize<Object, Value, FieldIndex, NoAnnotation, Field>::As(is, context, obj, value); // Customize<5args>, OpAnnotations defaulted
                else if constexpr ( is_specialized<Customize<Object, Value, NoFieldIndex, OpAnnotations, Field>>::value )
                    return Customize<Object, Value, NoFieldIndex, OpAnnotations, Field>::As(is, context, obj, value); // Customize<5args>, FieldIndex defaulted
                else if constexpr ( is_specialized<Customize<Object, Value, NoFieldIndex, NoAnnotation, Field>>::value )
                    return Customize<Object, Value, NoFieldIndex, NoAnnotation, Field>::As(is, context, obj, value); // Customize<5args>, both defaulted
                else if constexpr ( is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations>>::value )
                    return Customize<Object, Value, FieldIndex, OpAnnotations>::As(is, context, obj, value); // Four Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Value, NoFieldIndex, OpAnnotations>>::value )
                    return Customize<Object, Value, NoFieldIndex, OpAnnotations>::As(is, context, obj, value); // Customize<4args>, FieldIndex defaulted
                else if constexpr ( is_specialized<Customize<Object, Value, FieldIndex>>::value )
                    return Customize<Object, Value, FieldIndex>::As(is, context, obj, value); // Three Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Value>>::value )
                    return Customize<Object, Value>::As(is, context, obj, value); // Two Customize arguments specialized
                else if constexpr ( is_specialized<CustomizeType<Value, OpAnnotations, Field>>::value )
                    return CustomizeType<Value, OpAnnotations, Field>::As(is, context, value); // CustomizeType fully specialized
                else if constexpr ( is_specialized<CustomizeType<Value, NoAnnotation, Field>>::value )
                    return CustomizeType<Value, NoAnnotation, Field>::As(is, context, value); // CustomizeType<3args>, OpAnnotations defaulted
                else if constexpr ( is_specialized<CustomizeType<Value, OpAnnotations>>::value )
                    return CustomizeType<Value, OpAnnotations>::As(is, context, value); // CustomizeType<2args> specialized
                else if constexpr ( is_specialized<CustomizeType<Value>>::value )
                    return CustomizeType<Value>::As(is, context, value); // CustomizeType<1arg> specialized
                else
                    return false;
            }

            template <bool InArray>
            static constexpr bool True(std::istream & is, char & c)
            {
                Consume::True<InArray>(is, c);
                return true;
            }

            template <bool InArray>
            static constexpr bool False(std::istream & is, char & c)
            {
                Consume::False<InArray>(is, c);
                return false;
            }
            
            template <bool InArray, typename Value>
            static constexpr void Bool(std::istream & is, char & c, Value & value)
            {
                Checked::consumeWhitespace(is, "true or false");
                Checked::peek(is, c, "true or false");
                if ( c == 't' )
                {
                    Consume::True<InArray>(is, c);
                    if constexpr ( !std::is_const<Value>::value )
                        value = true;
                }
                else if ( c == 'f' )
                {
                    Consume::False<InArray>(is, c);
                    if constexpr ( !std::is_const<Value>::value )
                        value = false;
                }
                else
                    throw Exception("Expected: \"true\" or \"false\"");
            }

            template <bool InArray>
            static constexpr bool Bool(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "true or false");
                Checked::peek(is, c, "true or false");
                if ( c == 't' )
                {
                    Consume::True<InArray>(is, c);
                    return true;
                }
                else if ( c == 'f' )
                {
                    Consume::False<InArray>(is, c);
                    return false;
                }
                else
                    throw Exception("Expected: \"true\" or \"false\"");
            }

            template <bool InArray>
            static std::string Number(std::istream & is, char & c)
            {
                std::stringstream ss;
                Consume::Value<InArray>(is, c, ss);
                return ss.str();
            }

            static void String(std::istream & is, char & c, std::stringstream & ss)
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

            static void String(std::istream & is, char & c, std::string & str)
            {
                std::stringstream ss;
                Read::String(is, c, ss);
                str = ss.str();
            }

            template <typename T>
            static void String(std::istream & is, char & c, T & t)
            {
                std::stringstream ss;
                Read::String(is, c, ss);
                if constexpr ( !std::is_const<T>::value )
                    ss >> t;
            }
        
            static std::string String(std::istream & is, char & c)
            {
                std::string str;
                Read::String(is, c, str);
                return str;
            }
        
            static std::string String(std::istream & is)
            {
                char c = '\0';
                std::string str;
                Read::String(is, c, str);
                return str;
            }

            template <typename Value>
            static constexpr void EnumInt(std::istream & is, Value & value)
            {
                using EnumType = typename promote_char<typename std::underlying_type<typename remove_pointer<Value>::type>::type>::type;
                typename std::remove_const<EnumType>::type temp;
                is >> temp;
                if constexpr ( !std::is_const<EnumType>::value )
                    value = (typename remove_pointer<Value>::type)temp;
            }
            
            static inline std::string FieldName(std::istream & is, char & c)
            {
                std::string fieldName;
                try {
                    Read::String(is, c, fieldName);
                } catch ( UnexpectedLineEnding & e) {
                    throw FieldNameUnexpectedLineEnding(e);
                }
                return fieldName;
            }

            static std::shared_ptr<Generic::Value::Assigner> GenericObject(std::istream & is, Context & context, char & c);

            template <bool InArray>
            static std::shared_ptr<Generic::Value::Assigner> GenericArray(std::istream & is, Context & context, char & c);

            template <bool InArray>
            static std::shared_ptr<Generic::Value::Assigner> GenericValue(std::istream & is, Context & context, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                switch ( c )
                {
                    case '\"': return Generic::Value::Assigner::Make(new Generic::String(Read::String(is, c))); // String or error
                    case '{': return Read::GenericObject(is, context, c); // JSON object or error
                    case '[': return Read::GenericArray<InArray>(is, context, c); // JSON array or error
                    case 't': return Generic::Value::Assigner::Make(new Generic::Bool(Read::True<InArray>(is, c))); // "true" or error
                    case 'f': return Generic::Value::Assigner::Make(new Generic::Bool(Read::False<InArray>(is, c))); // "false" or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9':
                        return Generic::Value::Assigner::Make(new Generic::Number(Read::Number<InArray>(is, c))); // Number or error
                    case 'n':
                        Consume::Null<InArray>(is, c);
                        return Generic::Value::Assigner::Make(nullptr); // "null" or error
                    default:
                        throw InvalidUnknownFieldValue();
                }
            }

            static constexpr Generic::Value::Type ValueType(const char & c)
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
            static std::shared_ptr<Generic::Value::Assigner> GenericArray(std::istream & is, Context & context, char & c)
            {
                Read::ArrayPrefix(is, c);
                if ( Read::TryArraySuffix(is) )
                    return Generic::Value::Assigner::Make(new Generic::NullArray());

                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                
                Generic::Value::Type arrayElementType = Read::ValueType(c);

                std::shared_ptr<Generic::Value::Assigner> result = nullptr;
                switch ( arrayElementType )
                {
                    case Generic::Value::Type::Null: result = Generic::Value::Assigner::Make(new Generic::NullArray()); break;
                    case Generic::Value::Type::Boolean: result = Generic::Value::Assigner::Make(new Generic::BoolArray()); break;
                    case Generic::Value::Type::Number: result = Generic::Value::Assigner::Make(new Generic::NumberArray()); break;
                    case Generic::Value::Type::String: result = Generic::Value::Assigner::Make(new Generic::StringArray()); break;
                    case Generic::Value::Type::Object: result = Generic::Value::Assigner::Make(new Generic::ObjectArray()); break;
                    case Generic::Value::Type::Array: result = Generic::Value::Assigner::Make(new Generic::MixedArray()); break;
                }

                Generic::Value::Type elementType = Generic::Value::Type::None;
                do
                {
                    Checked::consumeWhitespace(is, "completion of field value");
                    Checked::peek(is, c, "completion of field value");
                    elementType = Read::ValueType(c);

                    if ( elementType != arrayElementType && arrayElementType != Generic::Value::Type::Array ) // Promote to mixed array
                    {
                        std::shared_ptr<Generic::Value::Assigner> newResult = Generic::Value::Assigner::Make(new Generic::MixedArray());
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
                                    mixedArray.push_back(Generic::Bool::Make(boolArray[i]));
                            }
                            break;
                            case Generic::Value::Type::Number:
                            {
                                const std::vector<std::string> & numberArray = result->get()->numberArray();
                                for ( size_t i=0; i<numberArray.size(); i++ )
                                    mixedArray.push_back(Generic::Number::Make(numberArray[i]));
                            }
                            break;
                            case Generic::Value::Type::String:
                            {
                                const std::vector<std::string> & stringArray = result->get()->stringArray();
                                for ( size_t i=0; i<stringArray.size(); i++ )
                                    mixedArray.push_back(Generic::String::Make(stringArray[i]));
                            }
                            break;
                            case Generic::Value::Type::Object:
                            {
                                const std::vector<std::map<std::string, std::shared_ptr<Generic::Value>>> & objectArray = result->get()->objectArray();
                                for ( size_t i=0; i<objectArray.size(); i++ )
                                    mixedArray.push_back(Generic::Object::Make(objectArray[i]));
                            }
                            break;
                        }
                        arrayElementType = Generic::Value::Type::Array;
                        result = newResult;
                    }

                    if ( arrayElementType == Generic::Value::Type::Array )
                    {
                        switch ( elementType )
                        {
                            case Generic::Value::Type::Null: Consume::Null<true>(is, c); result->get()->mixedArray().push_back(nullptr); break;
                            case Generic::Value::Type::Boolean: result->get()->mixedArray().push_back(Generic::Bool::Make(Read::Bool<true>(is, c))); break;
                            case Generic::Value::Type::Number: result->get()->mixedArray().push_back(Generic::Number::Make(Read::Number<true>(is, c))); break;
                            case Generic::Value::Type::String: result->get()->mixedArray().push_back(Generic::String::Make(Read::String(is, c))); break;
                            case Generic::Value::Type::Object: result->get()->mixedArray().push_back(Read::GenericObject(is, context, c)->out()); break;
                            case Generic::Value::Type::Array: result->get()->mixedArray().push_back(Read::GenericArray<true>(is, context, c)->out()); break;
                        }
                    }
                    else
                    {
                        switch ( elementType )
                        {
                            case Generic::Value::Type::Null: Consume::Null<true>(is, c); result->get()->nullArray()++; break;
                            case Generic::Value::Type::Boolean: result->get()->boolArray().push_back(Read::Bool<true>(is, c)); break;
                            case Generic::Value::Type::Number: result->get()->numberArray().push_back(Read::Number<true>(is, c)); break;
                            case Generic::Value::Type::String: result->get()->stringArray().push_back(Read::String(is, c)); break;
                            case Generic::Value::Type::Object: result->get()->objectArray().push_back(Read::GenericObject(is, context, c)->out()->object());
                                break;
                            case Generic::Value::Type::Array: result->get()->mixedArray().push_back(Read::GenericArray<true>(is, context, c)->out()); break;
                        }
                    }
                }
                while ( Read::IterableElementSeparator<false>(is) );
                    
                return result;
            }
            
            static std::shared_ptr<Generic::Value::Assigner> GenericObject(std::istream & is, Context & context, char & c)
            {
                std::shared_ptr<Generic::Value::Assigner> result = Generic::Value::Assigner::Make(new Generic::Object());
                auto & obj = result->get()->object();

                Read::ObjectPrefix(is, c);
                if ( !Read::TryObjectSuffix(is) )
                {
                    do
                    {
                        std::string fieldName = Read::FieldName(is, c);
                        Read::FieldNameValueSeparator(is, c);
                        std::shared_ptr<Generic::Value> value = Read::GenericValue<false>(is, context, c)->out();
                        obj.insert(std::pair<std::string, std::shared_ptr<Generic::Value>>(fieldName, value));
                    }
                    while ( Read::FieldSeparator(is) );
                }
                return result;
            }
            
            template <typename Field, size_t TupleIndex = 0, typename Object, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(std::istream & is, Context & context, char & c, Object & object, std::tuple<T1, T2, Ts...> & value);

            template <typename Field, typename Object, typename Key, typename T>
            static constexpr void Pair(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value);

            template <typename Field, typename T, typename Object>
            static constexpr void Iterable(std::istream & is, Context & context, char & c, Object & object, T & iterable);

            template <typename T>
            static void Object(std::istream & is, Context & context, char & c, T & t);

            template <bool InArray, typename Field, typename T, typename Object, bool AllowCustomization = true>
            static void Value(std::istream & is, Context & context, char & c, Object & object, T & value)
            {
                if constexpr ( AllowCustomization && Customizers::HaveSpecialization<Object, T, Field::Index, NoAnnotation, Field> ) // Input is specialized
                {
                    std::stringstream ss;
                    Json::Consume::Value<InArray>(is, c, ss);
                    std::string preserved = ss.str();
                    if ( !Read::Customization<Object, T, Field::Index, NoAnnotation, Field>(ss, context, object, value) )
                    {
                        std::stringstream subIs(preserved);
                        Read::Value<InArray, Field, T, Object, false>(subIs, context, c, object, value);
                    }
                    return;
                }

                if constexpr ( is_pointable<T>::value )
                {
                    using Dereferenced = typename remove_pointer<T>::type;
                    if constexpr ( std::is_base_of<Generic::Value, Dereferenced>::value )
                    {
                        if ( Consume::TryNull<InArray>(is, c) )
                        {
                            if ( !std::is_const<T>::value )
                                value = nullptr;
                        }
                        else if ( value == nullptr )
                        {
                            if constexpr ( std::is_const<T>::value )
                                Consume::Value<InArray>(is, c);
                            else
                                Read::GenericValue<InArray>(is, context, c)->into(value);
                        }
                    }
                    else if ( value == nullptr ) // Value is a nullptr and not a Json::Generic
                    {
                        if constexpr ( std::is_same<std::shared_ptr<Dereferenced>, T>::value )
                        {
                            value = std::shared_ptr<Dereferenced>(new Dereferenced());
                            Read::Value<InArray, Field>(is, context, c, object, *value);
                        }
                        else if constexpr ( std::is_same<std::unique_ptr<Dereferenced>, T>::value )
                        {
                            value = std::unique_ptr<Dereferenced>(new Dereferenced());
                            Read::Value<InArray, Field>(is, context, c, object, *value);
                        }
                        else // Value pointed to is a nullptr and value is not a smart pointer or generic, only valid value is null
                            Consume::Null<InArray>(is, c);
                    }
                    else if constexpr ( is_pointable<Dereferenced>::value && !std::is_const<Dereferenced>::value )
                        Read::Value<InArray, Field>(is, context, c, object, *value);  // Only chance assigning nullptr to the more deeply nested pointer
                    else if ( Consume::TryNull<InArray>(is, c) ) // If value pointer is not nullptr, "null" is a possible value
                    {
                        if constexpr ( !std::is_const<T>::value )
                            value = nullptr;
                    }
                    else
                        Read::Value<InArray, Field>(is, context, c, object, *value);
                }
                else if constexpr ( std::is_base_of<Generic::Value, T>::value )
                {
                    if constexpr ( std::is_const<T>::value )
                        Consume::Value<InArray>(is, c);
                    else
                        Read::GenericValue<InArray>(is, context, c)->into(value);
                }
                else if constexpr ( is_tuple<T>::value )
                {
                    if constexpr ( std::tuple_size<T>::value == 0 )
                    {
                        if ( !Consume::TryNull<InArray>(is, c) )
                            Consume::Iterable<true>(is, c);
                    }
                    else if constexpr ( std::tuple_size<T>::value == 1 )
                    {
                        if ( Read::TrySingularTupleArrayPrefix(is, c) )
                        {
                            if ( !Read::TryArraySuffix(is) )
                            {
                                Read::Value<true, Field>(is, context, c, object, std::get<0>(value));
                                while ( Read::IterableElementSeparator<false>(is) )
                                    Consume::Value<true>(is, c);
                            }
                        }
                        else
                            Read::Value<InArray, Field>(is, context, c, object, std::get<0>(value));
                    }
                    else if constexpr ( std::tuple_size<T>::value >= 2 )
                        Read::Tuple<Field>(is, context, c, object, value);
                }
                else if constexpr ( is_pair<T>::value )
                    Read::Pair<Field>(is, context, c, object, value);
                else if constexpr ( is_iterable<T>::value )
                    Read::Iterable<Field, T>(is, context, c, object, value);
                else if constexpr ( Field::template HasAnnotation<IsRoot> )
                    Read::Object<T>(is, context, c, value);
                else if constexpr ( is_reflected<T>::value )
                    Read::Object(is, context, c, value);
                else if constexpr ( Field::template HasAnnotation<Json::StringifyType> )
                    Read::String(is, c, value);
                else if constexpr ( Field::template HasAnnotation<Json::EnumIntType> )
                    Read::EnumInt<T>(is, value);
                else if constexpr ( is_bool<T>::value )
                    Read::Bool<InArray>(is, c, value);
                else if constexpr ( std::is_const<T>::value )
                    Consume::Value<InArray>(is, c);
                else if constexpr ( is_string<T>::value && !Field::template HasAnnotation<Json::UnstringType> )
                    Read::String(is, c, value);
                else
                    is >> value;
            }

            template <typename Field, size_t TupleIndex, typename Object, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(std::istream & is, Context & context, char & c, Object & object, std::tuple<T1, T2, Ts...> & value)
            {
                constexpr size_t tupleSize = std::tuple_size<typename std::remove_reference_t<decltype(value)>>::value;
                if constexpr ( TupleIndex == 0 )
                    Read::ArrayPrefix(is, c);

                if ( !Read::TryArraySuffix(is) )
                {
                    if constexpr ( TupleIndex < tupleSize )
                    {
                        Read::Value<true, Field>(is, context, c, object, std::get<TupleIndex>(value));
                        if ( Read::IterableElementSeparator<false>(is) )
                            Read::Tuple<Field, TupleIndex+1, Object, T1, T2, Ts...>(is, context, c, object, value);
                    }
                    else
                    {
                        do
                        {
                            Consume::Value<true>(is, c);
                        }
                        while ( Read::IterableElementSeparator<false>(is) );
                    }
                }
            }

            template <typename Field, typename Object, typename Key, typename T>
            static constexpr void Pair(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value)
            {
                Read::ArrayPrefix(is, c);
                if ( !Read::TryArraySuffix(is) )
                {
                    Read::Value<true, Field>(is, context, c, object, value.first);
                    if ( Read::IterableElementSeparator<false>(is) )
                    {
                        Read::Value<true, Field>(is, context, c, object, value.second);
                        while ( Read::IterableElementSeparator<false>(is) )
                            Consume::Value<true>(is, c);
                    }
                }
            }

            template <typename Field, typename Object, typename Key, typename T>
            static void KeyValueObject(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value)
            {
                Read::ObjectPrefix(is, c);
                if ( !Read::TryObjectSuffix(is) )
                {
                    do
                    {
                        std::string fieldName = Read::FieldName(is, c);
                        Read::FieldNameValueSeparator(is, c);
                        if ( fieldName.compare("key") == 0 )
                            Read::Value<false, Field>(is, context, c, object, value.first);
                        else if ( fieldName.compare("value") == 0 )
                            Read::Value<false, Field>(is, context, c, object, value.second);
                        else
                            Consume::Value<false>(is, c);
                    }
                    while ( Read::FieldSeparator(is) );
                }
            }
            
            template <typename Field, typename Object, typename Key, typename T>
            static void FieldPair(std::istream & is, Context & context, char & c, Object & object, Key & key, T & value)
            {
                std::stringstream ss;
                Read::String(is, c, ss);
                Read::Value<false, Field>(ss, context, c, object, key);
                Read::FieldNameValueSeparator(is, c);
                Read::Value<false, Field>(is, context, c, object, value);
            }

            template <typename Field, typename T, typename Object>
            static constexpr void Iterable(std::istream & is, Context & context, char & c, Object & object, T & iterable)
            {
                using Element = typename element_type<T>::type;
                constexpr bool IsMap = is_map<T>::value; // Simple maps are just json objects with keys as the field names, values as field values
                constexpr bool HasComplexKey = IsMap && is_non_primitive<typename pair_lhs<Element>::type>::value;
                constexpr bool JsonObjectCompatible = (IsMap && !HasComplexKey) || (!IsMap && (is_pair<Element>::value || is_tuple_pair<Element>::value));

                Clear(iterable);
                size_t i=0;
                if constexpr ( JsonObjectCompatible )
                {
                    if ( Read::IterablePrefix(is, c) ) // Json Object
                    {
                        if ( !Read::TryObjectSuffix(is) )
                        {
                            do
                            {
                                if constexpr ( is_static_array<T>::value )
                                {
                                    if ( i >= static_array_size<T>::value )
                                        throw ArraySizeExceeded();
                                    else
                                        Read::FieldPair<Field, Object>(is, context, c, object, std::get<0>(iterable[i]), std::get<1>(iterable[i]));
                                    i++;
                                }
                                else // Appendable STL container
                                {
                                    typename element_type<T>::type value;
                                    Read::FieldPair<Field, Object>(is, context, c, object, std::get<0>(value), std::get<1>(value));
                                    Append<T, typename element_type<T>::type>(iterable, value);
                                }
                            }
                            while ( Read::IterableElementSeparator<true>(is) );
                        }
                        return; // Object read finished
                    }
                }
                else
                    Read::ArrayPrefix(is, c); // Json Array

                if ( !Read::TryArraySuffix(is) )
                {
                    do
                    {
                        if constexpr ( is_static_array<T>::value )
                        {
                            if ( i >= static_array_size<T>::value )
                                throw ArraySizeExceeded();
                            else
                                Read::Value<true, Field>(is, context, c, object, iterable[i++]);
                        }
                        else // Appendable STL container
                        {
                            typename element_type<T>::type value;
                            if constexpr ( !IsMap )
                                Read::Value<true, Field>(is, context, c, object, value);
                            else if ( Read::PeekIterablePrefix(is, c) ) // Json Object
                                Read::KeyValueObject<Field, Object>(is, context, c, object, value);
                            else // Json Array
                                Read::Pair<Field, Object>(is, context, c, object, value);
                            Append<T, typename element_type<T>::type>(iterable, value);
                        }
                    }
                    while ( Read::IterableElementSeparator<false>(is) );
                }
            }

            template <typename Object>
            static constexpr void Field(std::istream & is, Context & context, char & c, Object & object, const std::string & fieldName)
            {
                Read::FieldNameValueSeparator(is, c);
                JsonField* jsonField = getJsonField(object, fieldName);
                if ( jsonField != nullptr ) // Known field
                {
                    if ( jsonField->type == JsonField::Type::Regular )
                    {
                        Object::Class::FieldAt(object, jsonField->index, [&](auto & field, auto & value) {
                            using FieldType = typename std::remove_reference<decltype(field)>::type;
                            Read::Value<false, FieldType>(is, context, c, object, value);
                        });
                    }
                    else if ( jsonField->type == JsonField::Type::SuperClass )
                    {
                        Object::Supers::At(object, jsonField->index, [&](auto superInfo, auto & superObj) {
                            using Super = typename std::remove_reference<decltype(superObj)>::type;
                            Read::Object<Super>(is, context, c, superObj);
                        });
                    }
                }
                else // Unknown field
                {
                    jsonField = getJsonField(object, fieldClusterToJsonFieldName());
                    if ( jsonField != nullptr ) // Has FieldCluster
                    {
                        Object::Class::FieldAt(object, jsonField->index, [&](auto & field, auto & value) {
                            using ValueType = typename std::remove_reference<decltype(value)>::type;
                            if constexpr ( std::is_base_of<Generic::Object, typename remove_pointer<ValueType>::type>::value )
                            {
                                if constexpr ( is_pointable<ValueType>::value )
                                {
                                    using Dereferenced = typename remove_pointer<ValueType>::type;
                                    if constexpr ( std::is_same<std::shared_ptr<Dereferenced>, ValueType>::value )
                                        value = std::shared_ptr<Dereferenced>(new Dereferenced());
                                    else if constexpr ( std::is_same<std::unique_ptr<Dereferenced>, ValueType>::value )
                                        value = std::unique_ptr<Dereferenced>(new Dereferenced());
                                    else
                                    {
                                        throw Exception(
                                            "Cannot assign a non-null value to a null pointer unless the type is std::shared_ptr or std::unique_ptr");
                                    }

                                    value->put(fieldName, Read::GenericValue<false>(is, context, c)->out());
                                }
                                else
                                    value.put(fieldName, Read::GenericValue<false>(is, context, c)->out());
                            }
                            else
                                throw Exception(TypeToStr<ValueType>().c_str());
                        });
                    }
                    else // No FieldCluster
                        Consume::Value<false>(is, c);
                }
            }

            template <typename T>
            static void Object(std::istream & is, Context & context, char & c, T & t)
            {
                Read::ObjectPrefix(is, c);
                if ( !Read::TryObjectSuffix(is) )
                {
                    do
                    {
                        std::string fieldName = Read::FieldName(is, c);
                        Read::Field(is, context, c, t, fieldName);
                    }
                    while ( Read::FieldSeparator(is) );
                }
            }
        };
        
        template <typename T>
        class ReflectedObject
        {
        public:
            ReflectedObject(T & obj, std::shared_ptr<Context> context) : obj(obj), context(context) {}

            T & obj;
            std::shared_ptr<Context> context;

            std::istream & get(std::istream & is)
            {
                if ( context == nullptr )
                    context = std::shared_ptr<Context>(new Context());

                char c = '\0';
                Read::Value<false, ReflectedField<T>>(is, *context, c, obj, obj);
                return is;
            }
        };
        
        template <typename T>
        std::istream & operator>>(std::istream & is, Json::Input::ReflectedObject<T> object)
        {
            return object.get(is);
        }

        template <typename T>
        Input::ReflectedObject<T> in(T & t, std::shared_ptr<Context> context = nullptr)
        {
            return Input::ReflectedObject<T>(t, context);
        }
    }

    inline Json::OutStreamType & operator<<(Json::OutStreamType & os, const Json::Generic::Value & value)
    {
        Json::Put::GenericValue<Json::NoAnnotation, true, Json::twoSpaces, true>(os, Json::context, 0, 0, value);
        return os;
    }
};

#undef osEndl

#endif
