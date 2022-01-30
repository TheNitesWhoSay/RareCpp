#ifndef BINARY_H
#define BINARY_H
#ifndef REFLECT_H
#include "Reflect.h"
#endif
#include <istream>
#include <ostream>
#include <sstream>

namespace Binary
{
    using OutStreamType = std::ostream;

    using namespace ExtendedTypeSupport;
    using namespace Reflect;

    inline namespace Annotations
    {
        /// Field annotation telling Binary to skip a field during input and output
        struct IgnoreType {};
        static constexpr IgnoreType Ignore{};

        template <typename T> struct FieldPrefixTypeNote { using type = T; };
        template <typename T> static constexpr FieldPrefixTypeNote<T> FieldPrefixType{};

        struct PrefixFieldSizesType {};
        static constexpr PrefixFieldSizesType PrefixFieldSizes{};

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
            class_t<Object>::ForEachField([&](auto & field) {
                using Field = typename std::remove_reference<decltype(field)>::type;
                if constexpr ( (Field::template HasAnnotation<Binary::IgnoreType> || Field::IsFunction) && matches_statics<Field::IsStatic, statics>::value )
                    count ++;
            });
            return count;
        }

        template <Statics statics, typename Object, bool IncludeSupers = true>
        static constexpr bool HasFields()
        {
            constexpr bool MatchesStaticsExcluded = statics == Statics::Excluded &&
                class_t<Object>::TotalFields - class_t<Object>::TotalStaticFields - IgnoredFieldCount<statics, Object>() > 0;
            constexpr bool MatchesStaticsIncluded = statics == Statics::Included &&
                class_t<Object>::TotalFields - IgnoredFieldCount<statics, Object>() > 0;
            constexpr bool MatchesStaticsOnly = statics == Statics::Only &&
                class_t<Object>::TotalStaticFields - IgnoredFieldCount<statics, Object>() > 0;

            if constexpr ( MatchesStaticsExcluded || MatchesStaticsIncluded || MatchesStaticsOnly )
                return true;
            else if constexpr ( supers_t<Object>::TotalSupers == 0 || !IncludeSupers )
                return false;
            else
            {
                bool hasFields = false;
                supers_t<Object>::ForEach([&](auto superInfo) {
                    using Super = typename decltype(superInfo)::Type;
                    if constexpr ( HasFields<statics, Super>() )
                        hasFields = true;
                });
                return hasFields;
            }
        }

        constexpr size_t NoFieldIndex = std::numeric_limits<size_t>::max();
        using NoField = Fields::Field<>;

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
    }

    inline namespace Output
    {
        inline namespace Customizers
        {
            template <typename Object, typename Value,
                size_t FieldIndex = NoFieldIndex, typename OpAnnotations = NoAnnotation, typename Field = NoField, Statics statics = Statics::Excluded>
            struct Customize : public Unspecialized
            {
                /// Should return true if you put any output, else you should leave output unchanged
                static bool As(OutStreamType & output, Context & context, const Object & object, const Value & value) { return false; }
            };

            template <typename Value, typename OpAnnotations = NoAnnotation, typename Field = NoField, Statics statics = Statics::Excluded>
            struct CustomizeType : public Unspecialized
            {
                /// Should return true if you put any output, else you should leave output unchanged
                static bool As(OutStreamType & output, Context & context, const Value & value) { return false; }
            };

            template <typename Object, typename Value,
                size_t FieldIndex = NoFieldIndex, typename OpAnnotations = NoAnnotation, typename Field = NoField, Statics statics = Statics::Excluded>
            static constexpr bool HaveSpecialization =
                is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations, Field, statics>>::value || // Fully specialized
                is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations, Field>>::value || // Customize<5args> specialized
                is_specialized<Customize<Object, Value, FieldIndex, OpAnnotations>>::value || // Customize<4args> specialized
                is_specialized<Customize<Object, Value, FieldIndex>>::value || // Customize<3args> specialized
                is_specialized<Customize<Object, Value>>::value || // Customize<2args> specialized
                is_specialized<Customize<Object, Value, FieldIndex, NoAnnotation, Field>>::value || // Customize<5args>, OpAnnotations defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, OpAnnotations, Field>>::value || // Customize<5args>, FieldIndex defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, NoAnnotation, Field>>::value || // Customize<5args>, both defaulted
                is_specialized<Customize<Object, Value, NoFieldIndex, OpAnnotations>>::value || // Customize<4args>, FieldIndex defaulted
                is_specialized<CustomizeType<Value, OpAnnotations, Field, statics>>::value || // Fully specialized
                is_specialized<CustomizeType<Value, OpAnnotations, Field>>::value || // CustomizeType<3args> specialized
                is_specialized<CustomizeType<Value, OpAnnotations>>::value || // CustomizeType<2args> specialized
                is_specialized<CustomizeType<Value>>::value || // CustomizeType<1arg> specialized
                is_specialized<CustomizeType<Value, NoAnnotation, Field>>::value; // CustomizeType<3arg>, OpAnnotations defaulted
        }

        namespace Put
        {
            template <typename Annotations, typename Field, Statics statics, typename Object, typename Value>
            static constexpr inline size_t Customization(OutStreamType & os, Context & context, const Object & obj, const Value & value)
            {
                if constexpr ( is_specialized<Customize<Object, Value, Field::Index, Annotations, Field, statics>>::value )
                    return Customize<Object, Value, Field::Index, Annotations, Field, statics>::As(os, context, obj, value); // Customize fully specialized
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
                else if constexpr ( is_specialized<CustomizeType<Value, Annotations, Field, statics>>::value )
                    return CustomizeType<Value, Annotations, Field, statics>::As(os, context, value); // CustomizeType fully specialized
                else if constexpr ( is_specialized<CustomizeType<Value, Annotations, Field>>::value )
                    return CustomizeType<Value, Annotations, Field>::As(os, context, value); // Three CustomizeType arguments specialized
                else if constexpr ( is_specialized<CustomizeType<Value, NoAnnotation, Field>>::value )
                    return CustomizeType<Value, NoAnnotation, Field>::As(os, context, value); // CustomizeType<3args>, Annotations defaulted
                else if constexpr ( is_specialized<CustomizeType<Value, Annotations>>::value )
                    return CustomizeType<Value, Annotations>::As(os, context, value); // Two CustomizeType arguments specialized
                else if constexpr ( is_specialized<CustomizeType<Value>>::value )
                    return CustomizeType<Value>::As(os, context, value); // One CustomizeType argument specialized
                else
                    return std::numeric_limits<size_t>::max();
            }

            static void String(std::ostream & os, const std::string & str)
            {
                os.write(str.c_str(), (std::streamsize)str.size());
                os.put('\0');
            }

            template <typename T>
            static void String(OutStreamType & os, const T & t)
            {
                std::stringstream ss;
                ss << t;
                Put::String(os, ss.str());
            }

            template <typename Annotations, typename Field, Statics statics, typename Object,
                size_t TupleIndex = 0, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(OutStreamType & os, Context & context, const Object & obj, const std::tuple<T1, T2, Ts...> & value);
            
            template <typename Annotations, typename Field, Statics statics, typename Object, typename Key, typename T>
            static constexpr void Pair(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & value);
            
            template <typename Annotations, typename Field, Statics statics, typename Object, typename IterableValue>
            static constexpr void Iterable(OutStreamType & os, Context & context, const Object & obj, const IterableValue & iterable);

            template <typename Annotations, Statics statics, typename T>
            static constexpr void Object(OutStreamType & os, Context & context, const T & obj);

            template <typename Annotations, typename Field, Statics statics, typename Object, typename T>
            static constexpr void Value(OutStreamType & os, Context & context, const Object & obj, const T & value)
            {
                if constexpr ( Customizers::HaveSpecialization<Object, T, Field::Index, Annotations, Field, statics> )
                {
                    if ( Put::Customization<Annotations, Field, statics, Object, T>(os, context, obj, value) )
                        return;
                }

                if constexpr ( ObjectMapper::HasDefaultMapping<T, typename Field::Annotations, Annotations> )
                {
                    using D = ObjectMapper::GetDefaultMapping<T, typename Field::Annotations, Annotations>;
                    D d = ObjectMapper::map<D>(value);
                    Put::Value<Annotations, Fields::Field<D>, statics, Object>(os, context, obj, d);
                }
                else if constexpr ( is_pointable<T>::value )
                {
                    os.put(value == nullptr ? char(0) : char(1));
                    if ( value != nullptr )
                        Put::Value<Annotations, Field, statics, Object>(os, context, obj, *value);
                }
                else if constexpr ( is_tuple<T>::value )
                {
                    if constexpr ( std::tuple_size<T>::value == 1 )
                        Put::Value<Annotations, Field, statics, Object>(os, context, obj, std::get<0>(value));
                    else if constexpr ( std::tuple_size<T>::value >= 2 )
                        Put::Tuple<Annotations, Field, statics, Object>(os, context, obj, value);
                }
                else if constexpr ( is_pair<T>::value )
                    Put::Pair<Annotations, Field, statics, Object>(os, context, obj, value);
                else if constexpr ( is_iterable<T>::value )
                    Put::Iterable<Annotations, Field, statics, Object>(os, context, obj, value);
                else if constexpr ( is_reflected<T>::value )
                    Put::Object<Annotations, statics, T>(os, context, value);
                else if constexpr ( is_bool<T>::value )
                    os.put(value ? 1 : 0);
                else if constexpr ( is_string<T>::value )
                    Put::String(os, value);
                else if constexpr ( std::is_enum<T>::value )
                    os.write(reinterpret_cast<const char*>(&value), (std::streamsize)sizeof(std::underlying_type_t<T>));
                else
                    os.write(reinterpret_cast<const char*>(&value), (std::streamsize)sizeof(value));
            }

            template <typename Annotations, typename Field, Statics statics, typename Object,
                size_t TupleIndex, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(OutStreamType & os, Context & context, const Object & obj, const std::tuple<T1, T2, Ts...> & value)
            {
                if constexpr ( TupleIndex < std::tuple_size<typename std::remove_reference_t<decltype(value)>>::value )
                {
                    Put::Value<Annotations, Field, statics, Object>(os, context, obj, std::get<TupleIndex>(value));
                    Put::Tuple<Annotations, Field, statics, Object, TupleIndex+1>(os, context, obj, value);
                }
            }
            
            template <typename Annotations, typename Field, Statics statics, typename Object, typename Key, typename T>
            static constexpr void Pair(OutStreamType & os, Context & context, const Object & obj, const std::pair<Key, T> & value)
            {
                Put::Value<Annotations, Field, statics, Object>(os, context, obj, value.first);
                Put::Value<Annotations, Field, statics, Object>(os, context, obj, value.second);
            }
            
            template <typename Annotations, typename Field, Statics statics, typename Object, typename IterableValue>
            static constexpr void Iterable(OutStreamType & os, Context & context, const Object & obj, const IterableValue & iterable)
            {
                using ElementType = typename element_type<IterableValue>::type;
                if constexpr ( is_stl_iterable<IterableValue>::value )
                {
                    Put::Value<Annotations, Field, statics, Object>(os, context, obj, uint32_t(std::size(iterable)*sizeof(ElementType)));
                    for ( auto & element : iterable )
                        Put::Value<Annotations, Field, statics, Object>(os, context, obj, element);
                }
                else if constexpr ( is_adaptor<IterableValue>::value )
                {
                    Put::Value<Annotations, Field, statics, Object>(os, context, obj, uint32_t(std::size(iterable)*sizeof(ElementType)));
                    const auto & sequenceContainer = get_underlying_container(iterable);
                    for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                        Put::Value<Annotations, Field, statics, Object>(os, context, obj, *it);
                }
                else if constexpr ( std::is_array<IterableValue>::value && std::extent<typename Field::Type>::value > 0 )
                {
                    for ( size_t i=0; i<std::extent<typename Field::Type>::value; i++ )
                        Put::Value<Annotations, Field, statics, Object>(os, context, obj, iterable[i]);
                }
            }
            
            template <typename Annotations, Statics statics, typename Object>
            static constexpr void Fields(OutStreamType & os, Context & context, const Object & obj)
            {
                class_t<Object>::ForEachField(obj, [&](auto & field, auto & value) {
                    using Field = std::remove_reference_t<decltype(field)>;
                    using Value = std::remove_reference_t<decltype(value)>;
                    if constexpr ( type_list<class_notes_t<Object>>::template has_specialization_v<FieldPrefixTypeNote> )
                    {
                        Put::Value<Annotations, Field, statics>(os, context, obj,
                            field.template getAnnotation<typename type_list<class_notes_t<Object>>::template get_specialization_t<FieldPrefixTypeNote>::type>());
                    }
                    if constexpr ( matches_statics<Field::IsStatic, statics>::value && !Field::template HasAnnotation<Binary::IgnoreType> )
                    {
                        if constexpr ( type_list<class_notes_t<Object>>::template has_v<PrefixFieldSizesType> )
                        {
                            if constexpr ( is_iterable<Value>::value )
                                Put::Value<Annotations, Field, statics, Object>(os, context, obj, value);
                            else
                            {
                                std::stringstream subStream(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
                                auto start = subStream.tellp();
                                Put::Value<Annotations, Field, statics, Object>(subStream, context, obj, value);
                                auto size = subStream.tellp() - start;
                                Put::Value<Annotations, Field, statics, Object>(os, context, obj, int32_t(size));
                                if ( size > 0 )
                                    os << subStream.rdbuf();
                            }
                        }
                        else
                            Put::Value<Annotations, Field, statics, Object>(os, context, obj, value);
                    }
                });
            }
            
            template <typename Annotations, Statics statics, typename Object>
            static constexpr void Supers(OutStreamType & os, Context & context, const Object & obj)
            {
                supers_t<Object>::ForEach(obj, [&](auto superInfo, auto & superObj)
                {
                    using Super = typename std::remove_reference<decltype(superObj)>::type;
                    if constexpr ( HasFields<statics, Super>() )
                        Put::Object<Annotations, statics, Super>(os, context, (Super)obj);
                });
            }

            template <typename Annotations, Statics statics, typename T>
            static constexpr void Object(OutStreamType & os, Context & context, const T & obj)
            {
                Put::Supers<Annotations, statics, T>(os, context, obj);
                Put::Fields<Annotations, statics, T>(os, context, obj);
            }
        }

        template <typename Annotations = NoAnnotation, Statics statics = Statics::Excluded, typename Object = uint_least8_t>
        struct ReflectedObject
        {
            ReflectedObject(const Object & obj, std::shared_ptr<Context> context) : obj(obj), context(context) {}

            const Object & obj;
            std::shared_ptr<Context> context;

            constexpr OutStreamType & put(OutStreamType & os)
            {
                if ( context == nullptr )
                    context = std::make_shared<Context>();

                Put::Value<Annotations, Fields::Field<Object, void, std::nullptr_t, 0, NoAnnotation>,
                    statics, Object, Object>(os, *context, obj, obj);

                return os;
            }
        };

        template <Statics statics = Statics::Excluded, typename Annotations = NoAnnotation, typename T = uint_least8_t>
        std::ostream & operator<<(std::ostream & os, Output::ReflectedObject<Annotations, statics, T> object)
        {
            return object.put(os);
        }

        template <Statics statics = Statics::Excluded, typename Annotations = NoAnnotation, typename T = uint_least8_t>
        Output::ReflectedObject<Annotations, statics, T> out(const T & t, std::shared_ptr<Context> context = nullptr)
        {
            return Output::ReflectedObject<Annotations, statics, T>(t, context);
        }
    }

    inline namespace Input
    {
        inline namespace Customizers
        {
            template <typename Object, typename Value, size_t FieldIndex = NoFieldIndex, typename OpAnnotations = NoAnnotation, typename Field = NoField>
            struct Customize : public Unspecialized
            {
                /// return false if you wish for the input to be re-parsed by the default Binary input code, else return true
                /// For invalid input you can throw an exception to end parsing immediately, or return true to continue parsing
                static bool As(std::istream & input, Context & context, const Object & object, Value & value) { return false; }
            };
            
            template <typename Value, typename OpAnnotations = NoAnnotation, typename Field = NoField>
            struct CustomizeType : public Unspecialized
            {
                /// return false if you wish for the input to be re-parsed by the default JSON code, else return true
                /// For invalid input you can throw an exception to end parsing immediately, or return true to continue parsing
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

            class ArraySizeExceeded : public Exception
            {
            public:
                ArraySizeExceeded() : Exception("Array size exceeded!") {}
            };
        };
        
        namespace Checked
        {
            template <typename T>
            void read(std::istream & is, T & t) // T should be a non-iterable, non-reflected, non-pointer primitive or structure
            {
                is.read(&t, (std::streamsize)sizeof(T));
                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd(std::string("Expected: ") + std::to_string(sizeof(T)) + " bytes for " + TypeToStr<T>());
                    else
                        throw StreamReadFail();
                }
            }

            template <>
            void read<bool>(std::istream & is, bool & b)
            {
                b = is.get() != 0;
                if ( ! is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd(std::string("Expected: one byte for bool"));
                    else
                        throw StreamReadFail();
                }
            }

            static inline void get(std::istream & is, char & c, const char* expectedDescription)
            {
                is.get(c);
                if ( !is.good() )
                {
                    if ( is.eof() )
                        throw UnexpectedInputEnd(expectedDescription);
                    else
                        throw StreamReadFail();
                }
            }
        }

        namespace Consume
        {
            template <typename Object, typename Value, size_t FieldIndex, typename OpAnnotations, typename Field>
            static constexpr inline bool Customization(std::istream & is, Context & context, Object & obj)
            {
                typename element_type<Value>::type value;
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

            static inline void Bool(std::istream & is, char & c)
            {
                Checked::get(is, c, "1-byte boolean indicator");
            }

            static inline void Bool(std::istream & is, char & c, std::stringstream & ss)
            {
                Checked::get(is, c, "1-byte boolean indicator");
                ss.put(c);
            }

            static void String(std::istream & is, char & c)
            {
                do {
                    Checked::get(is, c, "string character or NUL-terminator");
                } while ( c != '\0' );
            }

            static void String(std::istream & is, char & c, std::stringstream & ss)
            {
                do {
                    Checked::get(is, c, "string character or NUL-terminator");
                    ss.put(c);
                } while ( c != '\0' );
            }

            template <typename Annotations, typename Field, size_t TupleIndex = 0, typename Object, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(std::istream & is, Context & context, char & c, Object & object, std::tuple<T1, T2, Ts...> & value);

            template <typename Annotations, typename Field, typename Object, typename Key, typename T>
            static constexpr void Pair(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value);
            
            template <typename Annotations, typename Field, typename T, typename Object>
            static constexpr void Iterable(std::istream & is, Context & context, Object & object);

            template <typename Annotations, typename T>
            static void Object(std::istream & is, Context & context, T & t);

            template <typename Annotations, typename Field, typename T, typename Object, bool AllowCustomization = true>
            static void Value(std::istream & is, Context & context, char & c, Object & object)
            {
                if constexpr ( AllowCustomization && Customizers::HaveSpecialization<Object, T, Field::Index, Annotations, Field> ) // Input is specialized
                {
                    if ( !Consume::Customization<Object, T, Field::Index, Annotations, Field>(is, context, object) )
                        Consume::Value<Annotations, Field, T, Object, false>(is, context, c, object);

                    return;
                }

                if constexpr ( ObjectMapper::HasDefaultMapping<T, typename Field::Annotations, Annotations> )
                {
                    using D = ObjectMapper::GetDefaultMapping<T, typename Field::Annotations, Annotations>;
                    D d {};
                    Consume::Value<Annotations, Fields::Field<D>>(is, context, c, object, d);
                }
                else if constexpr ( is_pointable<T>::value )
                {
                    using Dereferenced = typename remove_pointer<T>::type;
                    Consume::Value<Annotations, Field, char, object, false>(is, context, c, object); // Read null-indicator
                    if ( c != char(0) ) // Is non-null, something is there to consume
                        Consume::Value<Annotations, Field, Dereferenced, Object>(is, context, c, object);
                }
                else if constexpr ( is_tuple<T>::value )
                {
                    if constexpr ( std::tuple_size<T>::value == 1 )
                        Consume::Value<Annotations, true, Field>(is, context, c, object);
                    else if constexpr ( std::tuple_size<T>::value >= 2 )
                        Consume::Tuple<Annotations, Field>(is, context, c, object);
                }
                else if constexpr ( is_pair<T>::value )
                    Consume::Pair<Annotations, Field>(is, context, object);
                else if constexpr ( is_iterable<T>::value )
                    Consume::Iterable<Annotations, Field, T>(is, context, object);
                else if constexpr ( is_reflected<T>::value )
                    Consume::Object<Annotations>(is, context, object);
                else if constexpr ( is_bool<T>::value )
                    Consume::Bool(is, c);
                else if constexpr ( is_string<T>::value )
                    Consume::String(is, c);
                else
                    is.ignore(std::streamsize(sizeof(T)));
            }

            template <typename Annotations, typename Field, size_t TupleIndex, typename Object, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(std::istream & is, Context & context, char & c, Object & object)
            {
                constexpr size_t tupleSize = 2 + sizeof...(Ts);
                if constexpr ( TupleIndex < tupleSize )
                {
                    using TupleElement = std::tuple_element<TupleIndex, std::tuple<T1, T2, Ts...>>;
                    Consume::Value<Annotations, Field, TupleElement, Object>(is, context, c, object);
                    Consume::Tuple<Annotations, Field, TupleIndex+1, Object, T1, T2, Ts...>(is, context, c, object);
                }
            }

            template <typename Annotations, typename Field, typename Object, typename Key, typename T>
            static constexpr void Pair(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value)
            {
                Consume::Value<Annotations, true, Field>(is, context, c, object, value.first);
                Consume::Value<Annotations, true, Field>(is, context, c, object, value.second);
            }
            
            template <typename Annotations, typename Field, typename T, typename Object>
            static constexpr void Iterable(std::istream & is, Context & context, Object & object)
            {
                using ElementType = typename element_type<T>::type;
                if constexpr ( std::is_array<T>::value && std::extent<typename Field::Type>::value > 0 )
                {
                    size_t size = static_array_size<T>::value;
                    is.ignore(std::streamsize(size*sizeof(ElementType)));
                }
                else
                {
                    uint32_t size = 0;
                    is.read(reinterpret_cast<char*>(&size), std::streamsize(sizeof(uint32_t)));
                    is.ignore(std::streamsize(size_t(size)*sizeof(ElementType)));
                }
            }

            template <typename Annotations, typename T>
            static void Object(std::istream & is, Context & context, char & c, T & t)
            {
                supers_t<T>::ForEach(t, [&](auto superInfo, auto & superObj) {
                    using Super = typename decltype(superInfo)::Type;
                    Consume::Value<Annotations, NoField, Super, T>(is, context, c, t, (Super)t);
                });
                class_t<T>::ForEachField(t, [&](auto & field, auto & value) {
                    using Field = std::remove_reference_t<decltype(field)>;
                    Consume::Value<Annotations, Field>(is, context, c, t, value);
                });
            }
        };

        namespace Read
        {
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

            template <typename Value>
            static constexpr void Bool(std::istream & is, char & c, Value & value)
            {
                Checked::get(is, c, "1-byte boolean indicator");
                value = bool(c != 0);
            }

            static void String(std::istream & is, char & c, std::stringstream & ss)
            {
                Consume::String(is, c, ss);
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
        
            template <bool ExpectQuotes = true>
            static std::string String(std::istream & is, char & c)
            {
                std::string str;
                Read::String<ExpectQuotes>(is, c, str);
                return str;
            }
        
            template <bool ExpectQuotes = true>
            static std::string String(std::istream & is)
            {
                char c = '\0';
                std::string str;
                Read::String<ExpectQuotes>(is, c, str);
                return str;
            }

            template <typename Annotations, typename Field, size_t TupleIndex = 0, typename Object, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(std::istream & is, Context & context, char & c, Object & object, std::tuple<T1, T2, Ts...> & value);

            template <typename Annotations, typename Field, typename Object, typename Key, typename T>
            static constexpr void Pair(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value);

            template <typename Annotations, typename Field, typename T, typename Object>
            static constexpr void Iterable(std::istream & is, Context & context, char & c, Object & object, T & iterable);

            template <typename Annotations, typename T>
            static void Object(std::istream & is, Context & context, char & c, T & t);

            template <typename Annotations, typename Field, typename T, typename Object, bool AllowCustomization = true>
            static void Value(std::istream & is, Context & context, char & c, Object & object, T & value)
            {
                if constexpr ( AllowCustomization && Customizers::HaveSpecialization<Object, T, Field::Index, Annotations, Field> ) // Input is specialized
                {
                    if ( !Read::Customization<Object, T, Field::Index, Annotations, Field>(is, context, object, value) )
                        Read::Value<Annotations, Field, T, Object, false>(is, context, c, object, value);

                    return;
                }

                if constexpr ( ObjectMapper::HasDefaultMapping<T, typename Field::Annotations, Annotations> )
                {
                    using D = ObjectMapper::GetDefaultMapping<T, typename Field::Annotations, Annotations>;
                    D d {};
                    Read::Value<Annotations, Fields::Field<D>>(is, context, object, c, d);
                    ObjectMapper::map(value, d);
                }
                else if constexpr ( is_pointable<T>::value )
                {
                    using Dereferenced = typename remove_pointer<T>::type;
                    Read::Value<Annotations, Field, char, Object, false>(is, context, c, object, c); // Read null-indicator
                    if ( c == char(0) )
                    {
                        if ( value != nullptr )
                            value = nullptr;
                    }
                    else // Is non-null
                    {
                        if ( value != nullptr )
                            Read::Value<Annotations, Field>(is, context, c, object, *value);
                        else // !isNull && value == nullptr, allocate if possible, consume otherwise
                        {
                            if constexpr ( std::is_same<std::shared_ptr<Dereferenced>, T>::value )
                            {
                                value = std::make_shared<Dereferenced>();
                                Read::Value<Annotations, Field>(is, context, c, object, *value);
                            }
                            else if constexpr ( std::is_same<std::unique_ptr<Dereferenced>, T>::value )
                            {
                                value = std::make_unique<Dereferenced>();
                                Read::Value<Annotations, Field>(is, context, c, object, *value);
                            }
                            else // Not shared or unique, consume the value instead
                                Consume::Value<Annotations, Field, Dereferenced, Object>(is, context, c, object);
                        }
                    }
                }
                else if constexpr ( is_tuple<T>::value )
                {
                    if constexpr ( std::tuple_size<T>::value == 1 )
                        Read::Value<Annotations, true, Field>(is, context, c, object, std::get<0>(value));
                    else if constexpr ( std::tuple_size<T>::value >= 2 )
                        Read::Tuple<Annotations, Field>(is, context, c, object, value);
                }
                else if constexpr ( is_pair<T>::value )
                    Read::Pair<Annotations, Field>(is, context, c, object, value);
                else if constexpr ( is_iterable<T>::value )
                    Read::Iterable<Annotations, Field, T>(is, context, c, object, value);
                else if constexpr ( is_reflected<T>::value )
                    Read::Object<Annotations>(is, context, c, value);
                else if constexpr ( is_bool<T>::value )
                    Read::Bool(is, c, value);
                else if constexpr ( std::is_const<T>::value )
                    Consume::Value<Annotations, Field, T, Object, AllowCustomization>(is, context, c, object);
                else if constexpr ( is_string<T>::value && !Field::template HasAnnotation<Json::UnstringType> )
                    Read::String<T>(is, c, value);
                else
                    is.read(reinterpret_cast<char*>(&value), std::streamsize(sizeof(T)));
            }

            template <typename Annotations, typename Field, size_t TupleIndex, typename Object, typename T1, typename T2, typename ...Ts>
            static constexpr void Tuple(std::istream & is, Context & context, char & c, Object & object, std::tuple<T1, T2, Ts...> & value)
            {
                constexpr size_t tupleSize = std::tuple_size<typename std::remove_reference_t<decltype(value)>>::value;
                if constexpr ( TupleIndex < tupleSize )
                {
                    Read::Value<Annotations, true, Field>(is, context, c, object, std::get<TupleIndex>(value));
                    Read::Tuple<Annotations, Field, TupleIndex+1, Object, T1, T2, Ts...>(is, context, c, object, value);
                }
            }

            template <typename Annotations, typename Field, typename Object, typename Key, typename T>
            static constexpr void Pair(std::istream & is, Context & context, char & c, Object & object, std::pair<Key, T> & value)
            {
                Read::Value<Annotations, true, Field>(is, context, c, object, value.first);
                Read::Value<Annotations, true, Field>(is, context, c, object, value.second);
            }
            
            template <typename Annotations, typename Field, typename T, typename Object>
            static constexpr void Iterable(std::istream & is, Context & context, char & c, Object & object, T & iterable)
            {
                if constexpr ( std::is_array<T>::value && std::extent<typename Field::Type>::value > 0 )
                {
                    size_t size = static_array_size<T>::value;
                    for ( size_t i=0; i<size_t(size); i++ )
                        Read::Value<Annotations, Field>(is, context, c, object, iterable[i]);
                }
                else
                {
                    uint32_t size = 0;
                    Read::Value<Annotations, Fields::Field<uint32_t>, uint32_t, Object, false>(is, context, c, object, size);
                    typename element_type<T>::type value;
                    Clear(iterable);
                    for ( size_t i=0; i<size_t(size / sizeof(typename element_type<T>::type)); i++ )
                    {
                        Read::Value<Annotations, Field>(is, context, c, object, value);
                        Append<T, typename element_type<T>::type>(iterable, value);
                    }
                }
            }

            template <typename Annotations, typename Field, typename T, typename Object>
            static constexpr void Iterable(std::istream & is, Context & context, char & c, Object & object, T & iterable, size_t sizeInBytes)
            {
                if constexpr ( std::is_array<T>::value && std::extent<typename Field::Type>::value > 0 )
                {
                    size_t size = sizeInBytes / sizeof(typename element_type<T>::type);
                    for ( size_t i=0; i<size; i++ )
                        Read::Value<Annotations, Field>(is, context, c, object, iterable[i]);
                }
                else
                {
                    typename element_type<T>::type value;
                    Clear(iterable);
                    for ( size_t i=0; i<size_t(sizeInBytes / sizeof(typename element_type<T>::type)); i++ )
                    {
                        Read::Value<Annotations, Field>(is, context, c, object, value);
                        Append<T, typename element_type<T>::type>(iterable, value);
                    }
                }
            }

            template <typename Annotations, typename T>
            static void Object(std::istream & is, Context & context, char & c, T & t)
            {
                supers_t<T>::ForEach(t, [&](auto superInfo, auto & superObj) {
                    using Super = typename decltype(superInfo)::Type;
                    Read::Value<Annotations, NoField, Super, T>(is, context, c, t, (Super)t);
                });
                class_t<T>::ForEachField(t, [&](auto & field, auto & value) {
                    using Field = std::remove_reference_t<decltype(field)>;
                    Read::Value<Annotations, Field>(is, context, c, t, value);
                });
            }
        }
        
        template <typename Annotations, typename T, bool IsKnownSize = false>
        struct ReflectedObject
        {
            ReflectedObject(T & obj, std::shared_ptr<Context> context) : obj(obj), sizeInBytes(0), context(context) {}
            ReflectedObject(T & obj, size_t sizeInBytes, std::shared_ptr<Context> context) : obj(obj), sizeInBytes(sizeInBytes), context(context) {}

            T & obj;
            size_t sizeInBytes;
            std::shared_ptr<Context> context;

            std::istream & get(std::istream & is)
            {
                if ( context == nullptr )
                    context = std::make_shared<Context>();

                char c = '\0';
                if constexpr ( IsKnownSize && is_stl_iterable<T>::value )
                    Read::Iterable<Annotations, Fields::Field<T>>(is, *context, c, obj, obj, sizeInBytes);
                else if constexpr ( IsKnownSize )
                {
                    auto start = is.tellg();
                    Read::Value<Annotations, Fields::Field<T>>(is, *context, c, obj, obj);
                    auto end = is.tellg();
                    if ( end-start != sizeInBytes )
                        throw Exception(("Expected " + std::to_string(sizeInBytes) + " but found " + std::to_string(end-start) + " bytes").c_str());
                }
                else
                    Read::Value<Annotations, Fields::Field<T>>(is, *context, c, obj, obj);


                return is;
            }
        };
        
        template <typename Annotations, typename T, bool IsKnownSizeIterable>
        std::istream & operator>>(std::istream & is, Binary::Input::ReflectedObject<Annotations, T, IsKnownSizeIterable> object)
        {
            return object.get(is);
        }

        template <typename Annotations = NoAnnotation, typename T = void>
        Input::ReflectedObject<Annotations, T> in(T & t, std::shared_ptr<Context> context = nullptr)
        {
            return Input::ReflectedObject<Annotations, T>(t, context);
        }

        template <typename Annotations = NoAnnotation, typename T = void>
        Input::ReflectedObject<Annotations, T, true> in(T & t, size_t sizeInBytes, std::shared_ptr<Context> context = nullptr)
        {
            return Input::ReflectedObject<Annotations, T, true>(t, sizeInBytes, context);
        }
    }
}

#endif