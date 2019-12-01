#ifndef JSON_H
#define JSON_H
#include <ostream>
#include <sstream>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <functional>
#include "Reflect.h"

namespace Json
{
    using namespace ExtendedTypeSupport;
    using namespace Reflect;
    
    inline namespace Annotations
    {
        /// Field annotation telling JSON to explicitly use the numeric value of an enum
        struct EnumInt {};

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
                (statics == Statics::Included || statics == Statics::Only) && IsStatic || statics == Statics::Excluded && !IsStatic;
        };

        template <Statics statics, typename Object>
        static constexpr size_t FirstIndex()
        {
            if constexpr ( statics == Statics::Only || statics == Statics::Excluded )
            {
                for ( size_t i=0; i<Object::Class::TotalFields; i++ )
                {
                    if constexpr ( statics == Statics::Only && Object::Class::Fields[i].IsStatic )
                        return i;
                    else if constexpr ( statics == Statics::Excluded && !Object::Class::Fields[i].IsStatic )
                        return i;
                }
            }
            return 0;
        }

        constexpr size_t NoFieldIndex = std::numeric_limits<size_t>::max();
        using NoField = Fields::Field<>;

        template <typename T>
        using ReflectedField = Fields::Field<T, void*, 0, IsRoot>;

        class Exception : public std::exception
        {
        public:
            Exception(const char* what) : std::exception(what) {}
        };

        inline namespace TypeNames
        {
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
        }

        inline namespace Generic
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
    };
    
    inline namespace Output
    {
        constexpr const char twoSpaces[] = "  ";
        
        inline namespace Customizers
        {
            template <typename Object, typename Value,
                size_t FieldIndex = NoFieldIndex, typename OpAnnotations = Annotate<>, typename Field = NoField, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t TotalParentIterables = 0, size_t IndentLevel = 0, const char* indent = twoSpaces>
            struct Customize : public Unspecialized
            {
                /// Should return true if you put any output, else you should leave output unchanged
                static bool As(std::ostream & output, const Object & object, const Value & value) { return false; }
            };

            template <typename Value, typename OpAnnotations = Annotate<>, typename Field = NoField, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t TotalParentIterables = 0, size_t IndentLevel = 0, const char* indent = twoSpaces>
            struct CustomizeType : public Unspecialized
            {
                /// Should return true if you put any output, else you should leave output unchanged
                static bool As(std::ostream & output, const Value & value) { return false; }
            };

            template <typename Object, typename Element,
                size_t FieldIndex = NoFieldIndex, typename OpAnnotations = Annotate<>, typename Field = NoField, Statics statics = Statics::Excluded,
                bool PrettyPrint = false, size_t TotalParentIterables = 0, size_t IndentLevel = 0, const char* indent = twoSpaces>
            static constexpr bool HaveSpecialization =
                is_specialized<Customize<Object, Element, Field::Index, OpAnnotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent>>::value || // Fully specialized
                is_specialized<Customize<Object, Element, Field::Index, OpAnnotations, Field>>::value || // Customize<5args> specialized
                is_specialized<Customize<Object, Element, Field::Index, OpAnnotations>>::value || // Customize<4args> specialized
                is_specialized<Customize<Object, Element, Field::Index>>::value || // Customize<3args> specialized
                is_specialized<Customize<Object, Element>>::value || // Customize<2args> specialized
                is_specialized<Customize<Object, Element, Field::Index, Annotate<>, Field>>::value || // Customize<5args>, OpAnnotations defaulted
                is_specialized<Customize<Object, Element, NoFieldIndex, OpAnnotations, Field>>::value || // Customize<5args>, FieldIndex defaulted
                is_specialized<Customize<Object, Element, NoFieldIndex, Annotate<>, Field>>::value || // Customize<5args>, both defaulted
                is_specialized<Customize<Object, Element, NoFieldIndex, OpAnnotations>>::value || // Customize<4args>, FieldIndex defaulted
                is_specialized<CustomizeType<Element, OpAnnotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent>>::value || // Fully specialized
                is_specialized<CustomizeType<Element, OpAnnotations, Field>>::value || // CustomizeType<3args> specialized
                is_specialized<CustomizeType<Element, OpAnnotations>>::value || // CustomizeType<2args> specialized
                is_specialized<CustomizeType<Element>>::value || // CustomizeType<1arg> specialized
                is_specialized<CustomizeType<Element, Annotate<>, Field>>::value; // CustomizeType<3arg>, OpAnnotations defaulted
        }

        inline namespace Affixes
        {
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            struct IndentType { };
            
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            IndentType<PrettyPrint, IndentLevel, indent> Indent;

            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces>
            std::ostream & operator<<(std::ostream & os, const IndentType<PrettyPrint, IndentLevel, indent> &)
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
            std::ostream & operator<<(std::ostream & os, const ArrayPrefixType<PrettyPrint, ContainsPrimitives, IndentLevel, indent> &)
            {
                if constexpr ( !PrettyPrint )
                    os << "[";
                else if constexpr ( ContainsPrimitives )
                    os << "[ ";
                else
                    os << "[" << std::endl << Indent<PrettyPrint, IndentLevel, indent>;

                return os;
            }

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            struct ArraySuffixType { };

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            ArraySuffixType<PrettyPrint, ContainsPrimitives, IndentLevel, indent> ArraySuffix;

            template <bool PrettyPrint, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
            std::ostream & operator<<(std::ostream & os, const ArraySuffixType<PrettyPrint, ContainsPrimitives, IndentLevel, indent> &)
            {
                if constexpr ( !PrettyPrint )
                    os << "]";
                else if constexpr ( ContainsPrimitives )
                    os << " ]";
                else
                    os << std::endl << Indent<PrettyPrint, IndentLevel, indent> << "]";

                return os;
            }

            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            struct ObjectPrefixType { };
                
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            ObjectPrefixType<PrettyPrint, IndentLevel, indent> ObjectPrefix;
                
            template <bool PrettyPrint, size_t IndentLevel, const char* indent, Statics statics, typename Object>
            std::ostream & operator<<(std::ostream & os, const ObjectPrefixType<PrettyPrint, IndentLevel, indent, statics, Object> &)
            {
                os << "{";
                return os;
            }

            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            struct ObjectSuffixType { };
                
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            ObjectSuffixType<PrettyPrint, IndentLevel, indent, statics, Object> ObjectSuffix;
                
            template <bool PrettyPrint, size_t IndentLevel, const char* indent = twoSpaces, Statics statics = Statics::Excluded, typename Object = void>
            std::ostream & operator<<(std::ostream & os, const ObjectSuffixType<PrettyPrint, IndentLevel, indent, statics, Object> &)
            {
                if constexpr ( std::is_same<void, Object>::value )
                {
                    if constexpr ( PrettyPrint )
                        os << std::endl << Indent<PrettyPrint, IndentLevel, indent> << "}";
                    else
                        os << "}";
                }
                else
                {
                    constexpr bool NotEmpty =
                        statics == Statics::Only && Object::Class::TotalStaticFields > 0 ||
                        statics == Statics::Included && Object::Class::TotalFields > 0 ||
                        statics == Statics::Excluded && Object::Class::TotalFields > 0 && Object::Class::TotalStaticFields < Object::Class::TotalFields ||
                        Object::Supers::TotalSupers > 0;

                    if constexpr ( PrettyPrint && NotEmpty )
                        os << std::endl << Indent<PrettyPrint, IndentLevel, indent> << "}";
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
            std::ostream & operator<<(std::ostream & os, const FieldPrefixType<IsFirst, PrettyPrint, IndentLevel, indent> &)
            {
                if constexpr ( PrettyPrint )
                {
                    if constexpr ( IsFirst )
                        os << std::endl << Indent<PrettyPrint, IndentLevel, indent>;
                    else 
                        os << "," << std::endl << Indent<PrettyPrint, IndentLevel, indent>;
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
            std::ostream & operator<<(std::ostream & os, const FieldNameValueSeparatorType<PrettyPrint> &)
            {
                if constexpr ( PrettyPrint )
                    os << ": ";
                else
                    os << ":";

                return os;
            }
        };

        template <typename Annotations = Annotate<>, Statics statics = Statics::Excluded,
            bool PrettyPrint = false, size_t IndentLevel = 0, const char* indent = twoSpaces, typename Object = uint_least8_t>
        class ReflectedObject;

        namespace Put
        {
            inline namespace Affix
            {
                template <bool PrettyPrint, bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
                void NestedPrefix(std::ostream & os, bool isEmpty)
                {
                    if constexpr ( IsArray )
                    {
                        if ( isEmpty )
                            os << ArrayPrefix<false, ContainsPrimitives, IndentLevel, indent>;
                        else
                            os << ArrayPrefix<PrettyPrint, ContainsPrimitives, IndentLevel, indent>;
                    }
                    else
                    {
                        if ( isEmpty )
                            os << ObjectPrefix<false, IndentLevel, indent>;
                        else
                            os << ObjectPrefix<PrettyPrint, IndentLevel, indent>;
                    }
                }

                template <bool PrettyPrint, bool IsArray, bool ContainsPrimitives, size_t IndentLevel, const char* indent = twoSpaces>
                void NestedSuffix(std::ostream & os, bool isEmpty)
                {
                    if constexpr ( IsArray )
                    {
                        if ( isEmpty )
                            os << ArraySuffix<false, ContainsPrimitives, IndentLevel, indent>;
                        else
                            os << ArraySuffix<PrettyPrint, ContainsPrimitives, IndentLevel, indent>;
                    }
                    else
                    {
                        if ( isEmpty )
                            os << ObjectSuffix<false, IndentLevel, indent>;
                        else
                            os << ObjectSuffix<PrettyPrint, IndentLevel, indent>;
                    }
                }

                template <bool PrettyPrint, bool IsJsonField, bool NestedSeparator, size_t IndentLevel, const char* indent = twoSpaces>
                void Separator(std::ostream & os, bool isFirst)
                {
                    if constexpr ( IsJsonField )
                    {
                        if ( isFirst && PrettyPrint )
                            os << std::endl << Indent<PrettyPrint, IndentLevel, indent>;
                        else if constexpr ( PrettyPrint )
                            os << "," << std::endl << Indent<PrettyPrint, IndentLevel, indent>;
                        else if ( !isFirst )
                            os << ",";
                    }
                    else if ( !isFirst )
                    {
                        if constexpr ( NestedSeparator && PrettyPrint )
                            os << "," << std::endl << Indent<PrettyPrint, IndentLevel, indent>;
                        else if constexpr ( PrettyPrint )
                            os << ", ";
                        else
                            os << ",";
                    }
                }
            };

            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, typename Element>
            static constexpr inline bool Customization(std::ostream & os, const Object & obj, const Element & element)
            {
                if constexpr ( is_specialized<Customize<Object, Element, Field::Index, Annotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent>>::value )
                {
                    return Customize<Object, Element, Field::Index, Annotations, Field, statics,
                        PrettyPrint, TotalParentIterables, IndentLevel, indent>(os, obj, element); // Customize fully specialized
                }
                else if constexpr ( is_specialized<Customize<Object, Element, Field::Index, Annotations, Field>>::value )
                    return Customize<Object, Element, Field::Index, Annotations, Field>(os, obj, element); // Five Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Element, Field::Index, Annotations>>::value )
                    return Customize<Object, Element, Field::Index, Annotations>(os, obj, element); // Four Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Element, Field::Index>>::value )
                    return Customize<Object, Element, Field::Index>::As(os, obj, element); // Three Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Element>>::value )
                    return Customize<Object, Element>::As(os, obj, element); // Two Customize arguments specialized
                else if constexpr ( is_specialized<Customize<Object, Element, Field::Index, Annotate<>, Field>>::value )
                    return Customize<Object, Element, Field::Index, Annotate<>, Field>(os, obj, element); // Customize<5args>, Annotations defaulted
                else if constexpr ( is_specialized<Customize<Object, Element, NoFieldIndex, Annotations, Field>>::value )
                    return Customize<Object, Element, NoFieldIndex, Annotations, Field>(os, obj, element); // Customize<5args>, FieldIndex defaulted
                else if constexpr ( is_specialized<Customize<Object, Element, NoFieldIndex, Annotate<>, Field>>::value )
                    return Customize<Object, Element, NoFieldIndex, Annotate<>, Field>(os, obj, element); // Customize<5args>, two args defaulted
                else if constexpr ( is_specialized<Customize<Object, Element, NoFieldIndex, Annotations>>::value )
                    return Customize<Object, Element, NoFieldIndex, Annotations>(os, obj, element); // Customize<4args>, FieldIndex defaulted
                else if constexpr ( is_specialized<CustomizeType<Element, Annotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent>>::value )
                {
                    return CustomizeType<Element, Annotations, Field, statics,
                        PrettyPrint, TotalParentIterables, IndentLevel, indent>(os, element); // CustomizeType fully specialized
                }
                else if constexpr ( is_specialized<CustomizeType<Element, Annotations, Field>>::value )
                    return CustomizeType<Element, Annotations, Field>(os, element); // Three CustomizeType arguments specialized
                else if constexpr ( is_specialized<CustomizeType<Element, Annotations>>::value )
                    return CustomizeType<Element, Annotations>(os, element); // Two CustomizeType arguments specialized
                else if constexpr ( is_specialized<CustomizeType<Element>>::value )
                    return CustomizeType<Element>(os, element); // One CustomizeType argument specialized
                else if constexpr ( is_specialized<CustomizeType<Element, Annotate<>, Field>>::value )
                    return CustomizeType<Element, Annotate<>, Field>>(os, element); // CustomizeType<3args>, Annotations defaulted
                else
                    return false;
            }

            static void String(std::ostream & os, const std::string & str)
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
            static constexpr void String(std::ostream & os, const T & t)
            {
                std::stringstream ss;
                ss << t;
                Put::String(os, ss.str());
            }
            
            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, typename Element>
            static constexpr void Value(std::ostream & os, const Object & obj, const Element & element)
            {
                if constexpr ( Customizers::HaveSpecialization<Object, Element, Field::Index, Annotations, Field, statics,
                    PrettyPrint, TotalParentIterables, IndentLevel, indent> )
                {
                    if ( Put::Customization<Annotations, Field, statics,
                        PrettyPrint, TotalParentIterables, IndentLevel, indent, Object, Element>(os, obj, element) )
                    {
                        return;
                    }
                }

                if constexpr ( is_pointable<Element>::value )
                {
                    if ( element == nullptr )
                        os << "null";
                    else
                        Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object>(os, obj, *element);
                }
                else if constexpr ( is_iterable<Element>::value )
                    Put::Iterable<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object>(os, obj, element);
                else if constexpr ( Field::template HasAnnotation<IsRoot> )
                    Put::Object<Annotations, statics, PrettyPrint, IndentLevel+TotalParentIterables, indent, Element>(os, element);
                else if constexpr ( Field::template HasAnnotation<Reflect::Reflected> )
                    Put::Object<Annotations, statics, PrettyPrint, IndentLevel+TotalParentIterables+1, indent, Element>(os, element);
                else if constexpr ( Field::template HasAnnotation<Json::String> )
                    Put::String(os, element);
                else if constexpr ( Field::template HasAnnotation<Json::EnumInt> )
                    os << (typename promote_char<typename std::underlying_type<Element>::type>::type)element;
                else if constexpr ( is_bool<Element>::value )
                    os << (element ? "true" : "false");
                else
                    os << (typename promote_char<Element>::type)element;
            }
            
            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, typename Element, typename Key>
            static constexpr void Value(std::ostream & os, const Object & obj, const std::pair<Key, Element> & pair)
            {
                Put::String(os, pair.first);
                os << FieldNameValueSeparator<PrettyPrint>;
                Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables, IndentLevel, indent, Object>(os, obj, pair.second);
            }

            template <typename Annotations, typename Field, Statics statics,
                bool PrettyPrint, size_t TotalParentIterables, size_t IndentLevel, const char* indent, typename Object, typename IterableValue = uint_least8_t>
            static constexpr void Iterable(std::ostream & os, const Object & obj, const IterableValue & iterable)
            {
                using Element = typename element_type<IterableValue>::type;
                constexpr bool ContainsIterables = is_iterable<typename pair_rhs<Element>::type>::value;
                constexpr bool ContainsPrimitives = !Field::template HasAnnotation<Reflect::Reflected> && !ContainsIterables;
                constexpr bool ContainsPairs = is_pair<Element>::value;
            
                size_t i=0;
                Put::NestedPrefix<PrettyPrint, !ContainsPairs, ContainsPrimitives, IndentLevel+TotalParentIterables+2, indent>(os, IsEmpty(iterable));
                if constexpr ( is_stl_iterable<IterableValue>::value )
                {
                    for ( auto & element : iterable )
                    {
                        Put::Separator<PrettyPrint, ContainsPairs, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(os, 0 == i++);
                        Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object>(os, obj, element);
                    }
                }
                else if constexpr ( is_adaptor<IterableValue>::value )
                {
                    const auto & sequenceContainer = get_underlying_container(iterable);
                    for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    {
                        Put::Separator<ContainsPairs, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(os, 0 == i++);
                        Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object>(os, obj, *it);
                    }
                }
                else if constexpr ( std::is_array<IterableValue>::value && std::extent<Field::Type>::value > 0 )
                {
                    for ( ; i<std::extent<Field::Type>::value; i++ )
                    {
                        Put::Separator<PrettyPrint, ContainsPairs, ContainsIterables, IndentLevel+TotalParentIterables+2, indent>(os, 0 == i);
                        Put::Value<Annotations, Field, statics, PrettyPrint, TotalParentIterables+1, IndentLevel, indent, Object>(os, obj, iterable[i]);
                    }
                }
                Put::NestedSuffix<PrettyPrint, !ContainsPairs, ContainsPrimitives, IndentLevel+TotalParentIterables+1, indent>(os, IsEmpty(iterable));
            }

            template <typename Annotations, typename FieldClass, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* indent, typename Object>
            static constexpr void Field(std::ostream & os, const Object & obj, const char* fieldName, const typename FieldClass::Type & value)
            {
                if constexpr ( matches_statics<FieldClass::IsStatic, statics>::value )
                {
                    os << FieldPrefix<FieldClass::Index == FirstIndex<statics, Object>(), PrettyPrint, IndentLevel+1, indent>;
                    Put::String(os, fieldName);
                    os << FieldNameValueSeparator<PrettyPrint>;
                    Put::Value<Annotations, FieldClass, statics, PrettyPrint, 0, IndentLevel, indent, Object>(os, obj, value);
                }
            }
            
            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* indent, typename Object>
            static constexpr void Fields(std::ostream & os, const Object & obj)
            {
                Object::Class::ForEachField(obj, [&](auto & field, auto & value)
                {
                    using Field = typename std::remove_reference<decltype(field)>::type;
                    Put::Field<Annotations, Field, statics, PrettyPrint, IndentLevel, indent, Object>(os, obj, field.name, value);
                });
            }

            template <typename Annotations, size_t SuperIndex, typename SuperType, Statics statics,
                bool PrettyPrint, size_t IndentLevel, const char* indent, typename Object>
            static constexpr void Super(std::ostream & os, const Object & obj, const std::string & superFieldName)
            {
                os << FieldPrefix<SuperIndex == 0, PrettyPrint, IndentLevel+1, indent>;
                Put::String(os, superFieldName);
                os << FieldNameValueSeparator<PrettyPrint>;
                Put::Object<Annotations, statics, PrettyPrint, IndentLevel+1, indent, SuperType>(os, obj);
            }
            
            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* indent, typename Object>
            static constexpr void Supers(std::ostream & os, const Object & obj)
            {
                Object::Supers::ForEach(obj, [&](auto index, auto & superObj)
                {
                    using Super = typename std::remove_reference<decltype(superObj)>::type;
                    Put::Super<Annotations, decltype(index)::Index, Super, statics, PrettyPrint, IndentLevel, indent, Object>(os, obj,
                        superTypeToJsonFieldName<Super>());
                });
            }

            template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* indent, typename T>
            static constexpr void Object(std::ostream & os, const T & obj)
            {
                os << ObjectPrefix<PrettyPrint, IndentLevel, indent, statics, T>;
                Put::Fields<Annotations, statics, PrettyPrint, IndentLevel, indent, T>(os, obj);
                Put::Supers<Annotations, statics, PrettyPrint, IndentLevel, indent, T>(os, obj);
                os << ObjectSuffix<PrettyPrint, IndentLevel, indent, statics, T>;
            }
        }

        template <typename Annotations, Statics statics, bool PrettyPrint, size_t IndentLevel, const char* indent, typename T>
        class ReflectedObject
        {
        public:
            ReflectedObject(const T & obj) : obj(obj) {}

            const T & obj;
            
            static constexpr std::ostream & put(std::ostream & os, const T & obj)
            {
                Put::Value<Annotations, ReflectedField<T>, statics, PrettyPrint, 0, IndentLevel, indent, T, T>(os, obj, obj);
                return os;
            }
        };
        
        template <Statics statics = Statics::Excluded, typename Annotations = Annotate<>, bool PrettyPrint = false,
            size_t IndentLevel = 0, const char* indent = twoSpaces, typename T = uint_least8_t>
        std::ostream & operator<<(std::ostream & os, Output::ReflectedObject<Annotations, statics, PrettyPrint, IndentLevel, indent, T> object)
        {
            return object.put(os, object.obj);
        }

        template <Statics statics = Statics::Excluded, typename Annotations = Annotate<>,
            size_t IndentLevel = 0, const char* indent = twoSpaces, typename T = uint_least8_t>
        constexpr Output::ReflectedObject<Annotations, statics, false, IndentLevel, indent, T> out(const T & t)
        {
            return Output::ReflectedObject<Annotations, statics, false, IndentLevel, indent, T>(t);
        }
    
        template <Statics statics = Statics::Excluded, typename Annotations = Annotate<>,
            size_t IndentLevel = 0, const char* indent = twoSpaces, typename T = uint_least8_t>
        constexpr Output::ReflectedObject<Annotations, statics, true, IndentLevel, indent, T> pretty(const T & t)
        {
            return Output::ReflectedObject<Annotations, statics, true, IndentLevel, indent, T>(t);
        }
    };
    
    inline namespace Input
    {
        template <typename Object, typename Value, size_t FieldIndex, typename FieldAnnotations = Annotate<>, typename OpAnnotations = Annotate<>>
        struct Customize : public Unspecialized
        {
            /// Should return true and update value accordingly if you consume any input, else you should return false and leave input and value unchanged
            /// If you run into any errors consuming input or rolling back input you should throw an exception
            static bool As(std::istream & input, const Object & object, Value & value) { return false; }
        };

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

        inline namespace Cache
        {
            extern std::hash<std::string> strHash;

            extern std::map<std::type_index, std::multimap<size_t, JsonField>> classToNameHashToJsonField;

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

            static bool get(std::istream & is, char trueChar, char falseChar, const char* expectedDescription)
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

            static inline void get(std::istream & is, char & c, const char* expectedDescription)
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

            static inline bool unget(std::istream & is, char ungetting)
            {
                is.unget();
                if ( !is.good() )
                    throw StreamUngetFail(ungetting);

                return true;
            }

            static inline void escapeSequenceGet(std::istream & is, char & c, const char* hexEscapeSequence)
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
                    Checked::get(is, c, expectation[i], "completion of field value");

                Checked::consumeWhitespace(is, "\",\" or \"}\"");
                Checked::peek(is, c, "\",\" or \"}\"");
                if ( InArray && c != ',' && c != ']' )
                    throw Exception("Expected: \",\" or \"]\"");
                else if ( !InArray && c != ',' && c != '}' )
                    throw Exception("Expected: \",\" or \"}\"");
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
            static constexpr void True(std::istream & is, char & c)
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
            static constexpr void False(std::istream & is, char & c)
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

            static void String(std::istream & is, char & c)
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

            template <typename Element>
            static constexpr void ConstPrimitive(std::istream & is)
            {
                typename std::remove_const<Element>::type placeholder;
                is >> placeholder;
            }
        };

        namespace Read
        {
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

            static void String(std::istream & is, char & c, std::stringstream & ss)
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

            static void String(std::istream & is, char & c, std::string & str)
            {
                std::stringstream ss;
                Read::String(is, c, ss);
                str = ss.str();
            }

            template <typename T>
            static constexpr void String(std::istream & is, char & c, T & t)
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

            template <typename Field, typename Element>
            static constexpr void EnumInt(std::istream & is, Element & element)
            {
                using EnumType = typename promote_char<typename std::underlying_type<typename remove_pointer<Element>::type>::type>::type;
                typename std::remove_const<EnumType>::type temp;
                is >> temp;
                if constexpr ( !std::is_const<EnumType>::value )
                    element = (typename remove_pointer<Element>::type)temp;
            }
        };

        template <typename T>
        class ReflectedObject
        {
        public:
            ReflectedObject(T & obj) : obj(obj) {}

            T & obj;

            template <bool InArray>
            static constexpr void consumeValue(std::istream & is, char & c)
            {
                Checked::consumeWhitespace(is, "completion of field value");
                Checked::peek(is, c, "completion of field value");
                switch ( c )
                {
                    case '\"': Consume::String(is, c); break; // String or error
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9': Consume::Number<InArray>(is, c); break; // Number or error
                    case '{': consumeIterable<false>(is, c); break; // JSON object or error
                    case '[': consumeIterable<true>(is, c); break; // JSON array or error
                    case 't': Consume::True<InArray>(is, c); break; // "true" or error
                    case 'f': Consume::False<InArray>(is, c); break; // "false" or error
                    case 'n': Consume::Null<InArray>(is, c); break; // "null" or error
                    default: throw InvalidUnknownFieldValue(); break;
                }
            }

            template <bool InArray, typename Field, typename Element>
            static constexpr void getValue(std::istream & is, char & c, T & t, Element & element)
            {
                if constexpr ( !std::is_base_of<Unspecialized, Customize<T, Element, Field::Index>>::value ) // Input for this is specialized
                {
                    if ( Customize<T, Element, Field::Index>::As(is, t, element) )
                        return; // If true was returned then custom input was used, if false, then default output was requested
                }

                if constexpr ( is_pointable<Element>::value )
                {
                    if ( element == nullptr ) // If element pointer is nullptr the only valid value is "null"
                        Consume::Null<InArray>(is, c);
                    else if constexpr ( is_pointable<std::remove_pointer<Element>::type>::value && // If value pointed to is also a pointer
                        !std::is_const<std::remove_pointer<Element>::type>::value ) // And value pointed to is not const
                    {
                        getValue<InArray, Field>(is, c, t, *element);  // Only take the chance of assigning nullptr to that more deeply nested pointer
                    }
                    else if ( Consume::TryNull<InArray>(is, c) ) // If element pointer is not nullptr, "null" is a possible value
                    {
                        if constexpr ( !std::is_const<Element>::value )
                            element = nullptr;
                    }
                    else
                        getValue<InArray, Field>(is, c, t, *element);
                }
                else if constexpr ( is_iterable<Element>::value )
                    getIterable<Field, Element>(is, c, t, element);
                else if constexpr ( Field::template HasAnnotation<Reflect::Reflected> )
                    ReflectedObject<Element>::get(is, c, element);
                else if constexpr ( Field::template HasAnnotation<Json::String> )
                    Read::String(is, c, element);
                else if constexpr ( Field::template HasAnnotation<Json::EnumInt> )
                    Read::EnumInt<Field, Element>(is, element);
                else if constexpr ( is_bool<Element>::value )
                    Read::Bool<InArray>(is, c, element);
                else if constexpr ( std::is_const<Element>::value )
                    Consume::ConstPrimitive<Element>(is);
                else
                    is >> element;
            }

            template <bool InArray, typename Field, typename Key, typename Element>
            static constexpr void getValue(std::istream & is, char & c, T & t, std::pair<Key, Element> & pair)
            {
                Read::String(is, c, pair.first);
                Checked::get(is, c, ':', "field name-value separator \":\"");
                getValue<InArray, Field, Element>(is, c, t, pair.second);
            }
        
            template <bool IsArray>
            static constexpr void consumeIterable(std::istream & is, char & c)
            {
                Checked::get<IsArray>(is, c, '[', '{', "array opening \"[\"", "object opening \"{\"");
                if ( !Checked::tryGet<IsArray>(is, ']', '}', "array closing \"]\" or array element", "object closing \"}\" or field name opening \"") )
                {
                    do
                    {
                        if ( !IsArray )
                        {
                            Consume::String(is, c);
                            Checked::get(is, c, ':', "field name-value separator \":\"");
                        }
                        consumeValue<IsArray>(is, c);
                    }
                    while ( Checked::get<IsArray>(is, ',', ']', '}', "\",\" or array closing \"]\"", "\",\" or object closing \"}\"") );
                }
            }

            template <typename Field, typename Iterable>
            static constexpr void getIterable(std::istream & is, char & c, T & t, Iterable & iterable)
            {
                using Element = typename element_type<Iterable>::type;
                constexpr bool ContainsPairs = is_pair<Element>::value;
            
                Checked::get<ContainsPairs>(is, c, '{', '[', "object opening \"{\"", "array opening \"[\"");
                if ( !Checked::tryGet<ContainsPairs>(is, '}', ']', "object closing \"}\" or field name opening \"", "array closing \"]\" or array element") )
                {
                    Clear(iterable);
                    size_t i=0;
                    do
                    {
                        if constexpr ( is_static_array<Iterable>::value )
                        {
                            if ( i >= static_array_size<Iterable>::value )
                                throw Exception("Array size exceeded!");
                            else
                                getValue<!ContainsPairs, Field>(is, c, t, iterable[i++]);
                        }
                        else // Appendable STL container
                        {
                            typename element_type<Iterable>::type value;
                            getValue<!ContainsPairs, Field>(is, c, t, value);
                            Append<Iterable, typename element_type<Iterable>::type>(iterable, value);
                        }
                    }
                    while ( Checked::get<ContainsPairs>(is, ',', '}', ']', "\",\" or object closing \"}\"", "\",\" or array closing \"]\"") );
                }
            }

            static std::istream & get(std::istream & is, char & c, T & t)
            {
                using Class = typename T::Class;
                using Supers = typename T::Supers;

                Checked::get(is, c, '{', "object opening \"{\"");
                if ( !Checked::tryGet(is, '}', "object closing \"}\" or field name opening \"") )
                {
                    do
                    {
                        std::string fieldName;
                        try {
                            Read::String(is, c, fieldName);
                        } catch ( UnexpectedLineEnding & e) {
                            throw Exception((std::string("Expected field name close quote, found line ending (\"") + e.what() + "\")").c_str());
                        }
                        Checked::get(is, c, ':', "field name-value separator \":\"");

                        JsonField* jsonField = getJsonField(t, fieldName);
                        if ( jsonField != nullptr ) // Known field
                        {
                            if ( jsonField->type == JsonField::Type::Regular )
                            {
                                Class::FieldAt(t, jsonField->index, [&](auto & field, auto & value) {

                                    using Field = typename std::remove_reference<decltype(field)>::type;

                                    getValue<false, Field>(is, c, t, value);
                                });
                            }
                            else
                            {
                                Supers::At(t, jsonField->index, [&](auto & superObj) {
                                
                                    using Super = typename std::remove_reference<decltype(superObj)>::type;
                                    ReflectedObject<Super>::get(is, c, superObj);
                                });
                            }
                        }
                        else // Unknown field
                            consumeValue<false>(is, c);
                    }
                    while ( Checked::get(is, ',', '}', "\",\" or object closing \"}\"") );
                }
                return is;
            }
        };
        
        template <typename T>
        std::istream & operator>>(std::istream & is, Json::Input::ReflectedObject<T> object)
        {
            char c = '\0';
            return object.get(is, c, object.obj);
        }

        template <typename T>
        constexpr Input::ReflectedObject<T> in(T & t)
        {
            return Input::ReflectedObject<T>(t);
        }
    }
    
};

#endif