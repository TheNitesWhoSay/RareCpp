#ifndef BINARY_H
#define BINARY_H
#ifndef REFLECT_H
#include <istream>
#include <ostream>
#include "Reflect.h"
#endif

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
            static constexpr inline bool Customization(OutStreamType & os, Context & context, const Object & obj, const Value & value)
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
                    return false;
            }

            static void String(std::ostream & os, const std::string & str)
            {
                os.write(str.c_str(), str.size());
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
                    /*if ( value == nullptr ) // TODO: Pointers
                    {
                        for ( size_t i=0; i<sizeof(remove_pointer<T>); i++ )
                            os.put('\0');
                    }
                    else
                        Put::Value<Annotations, Field, statics, Object>(os, context, obj, *value);*/
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
                {
                    if ( value )
                        os.put(1);
                    else
                        os.put(0);
                }
                else if constexpr ( is_string<T>::value )
                    Put::String(os, value);
                else if constexpr ( std::is_enum<T>::value )
                    os.write((typename std::underlying_type<T>::type)value, sizeof(typename std::underlying_type<T>::type));
                else
                    os.write((const char*)&value, sizeof(value)); // TODO: Better byte representation than char cast
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
                if constexpr ( is_stl_iterable<IterableValue>::value )
                {
                    for ( auto & element : iterable )
                        Put::Value<Annotations, Field, statics, Object>(os, context, obj, element);
                }
                else if constexpr ( is_adaptor<IterableValue>::value )
                {
                    const auto & sequenceContainer = get_underlying_container(iterable);
                    for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                        Put::Value<Annotations, Field, statics, Object, false>(os, context, obj, *it);
                }
                else if constexpr ( std::is_array<IterableValue>::value && std::extent<typename Field::Type>::value > 0 )
                {
                    for ( size_t i=0; i<std::extent<typename Field::Type>::value; i++ )
                        Put::Value<Annotations, Field, statics, Object, false>(os, context, obj, iterable[i]);
                }
            }
            
            template <typename Annotations, Statics statics, typename Object>
            static constexpr void Fields(OutStreamType & os, Context & context, const Object & obj)
            {
                class_t<Object>::ForEachField(obj, [&](auto & field, auto & value)
                {
                    using Field = typename std::remove_reference<decltype(field)>::type;
                    if constexpr ( matches_statics<Field::IsStatic, statics>::value && !Field::template HasAnnotation<Binary::IgnoreType> )
                        Put::Value<Annotations, Field, statics, Object>(os, context, obj, value);
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
                Put::Fields<Annotations, statics, T>(os, context, obj);
                Put::Supers<Annotations, statics, T>(os, context, obj);
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
        namespace Read
        {

        }
    }
}

#endif