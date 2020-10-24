#ifndef WORKINGANNOTATE_H
#define WORKINGANNOTATE_H
#include "../RareCppLib/Reflect.h"
#include "../RareCppLib/Json.h"
#include <string_view>
#include <cstddef>
#include <tuple>

#define ANNOTATE(t, ...) struct t##_resolver { \
    static constexpr uint32_t file = ConstexprStr::hash(__FILE__); \
    template <uint32_t line> static constexpr bool isNoteLine = \
        std::tuple_element_t<0, decltype(note_line<file, line>(Reflect::line_t<file, line>{}, Reflect::adl{}))>::value; \
    static constexpr uint32_t scope = ExtendedTypeSupport::getScopeHash<t##_resolver>(); \
    static constexpr uint32_t line = Reflect::getPrevAnnotationLine<t##_resolver, NoteResolution, 0, __LINE__>(); \
    template <uint32_t NoteLine> struct GetNote \
    { static constexpr auto value = std::get<2>(note_line<file, line>(Reflect::line_t<file, line>{}, Reflect::adl{})); }; \
    template <> struct GetNote<0> { static constexpr auto value = Reflect::NoAnnotation{}; }; \
    static constexpr auto prev_note = t##_resolver::GetNote<line>::value; \
    static constexpr auto note { std::tuple { __VA_ARGS__ } }; \
}; \
template <uint32_t file, uint32_t line> static constexpr std::tuple<std::true_type, std::string_view, decltype(t##_resolver::note)> \
note_line(Reflect::line_t<ConstexprStr::hash(__FILE__), __LINE__>, Reflect::adl) { return {{}, #t, t##_resolver::note}; } \
using NoteResolution = std::integral_constant<uint32_t, t##_resolver::scope>;


#define NEW_DESCRIBE_FIELD(x) struct x##_ { \
    template <typename T> static constexpr ExtendedTypeSupport::TypePair<decltype(T::x), decltype(&T::x)> identify(int); \
    template <typename T> static constexpr ExtendedTypeSupport::TypePair<decltype(T::x), std::nullptr_t> identify(unsigned int); \
    template <typename T> static constexpr ExtendedTypeSupport::TypePair<decltype(&T::x), decltype(&T::x)> identify(...); \
    using Type = decltype(identify<ClassType>(0))::Left; \
    using Pointer = decltype(identify<ClassType>(0))::Right; \
    template <typename T, bool IsReference> struct GetPointer { static constexpr auto value = &T::x; }; \
    template <typename T> struct GetPointer<T, true> { static constexpr std::nullptr_t value = nullptr; }; \
    static constexpr auto nameStr = ConstexprStr::substr<std::string_view(#x).size()>(&#x[0]); \
    static constexpr auto typeStr = ExtendedTypeSupport::TypeName<Type>(); \
    template <typename T> static constexpr decltype(T::x##_resolver::note) idNote(int); \
    template <typename T> static constexpr decltype(Class::NoNote) idNote(...); \
    template <typename T, bool NoNote> struct GetNote { static constexpr auto & value = Class::NoNote; }; \
    template <typename T> struct GetNote<T, false> { static constexpr auto & value = T::x##_resolver::note; }; \
    template <typename T, bool NoNote> struct GetPrevNote { static constexpr auto value = Class::NoNote; }; \
    template <typename T> struct GetPrevNote<T, false> { static constexpr auto value = T::x##_resolver::prev_note; }; \
    using NoteType = decltype(idNote<ClassType>(0)); \
    static constexpr auto prev_note = GetPrevNote<ClassType, std::is_same_v<decltype(Class::NoNote), NoteType>>::value; \
    using Field = Reflect::Fields::Field<Type, Pointer, IndexOf::x, NoteType>; \
    static constexpr Field field = { &nameStr.value[0], &typeStr.value[0], GetPointer<ClassType, std::is_reference_v<Type>>::value, \
        GetNote<ClassType, std::is_same_v<decltype(Class::NoNote), NoteType>>::value }; \
    CLANG_ONLY(x) \
}; \
template <> struct GetClassNote<(int)IndexOf::x> { static constexpr auto value = x##_::prev_note; };

#define REFLECTED(objectType, ...) \
struct Class { \
    using ClassType = objectType; \
    static constexpr auto ClassTypeName = std::string_view(#objectType); \
    enum_t(IndexOf, size_t, { FOR_EACH(GET_FIELD_NAME, __VA_ARGS__) }); \
    static constexpr Reflect::NoAnnotation NoNote {}; \
    template <int FieldIndex> struct GetClassNote { \
        static constexpr uint32_t file = ConstexprStr::hash(__FILE__); \
        template <uint32_t line> static constexpr bool isNoteLine = \
            std::tuple_element_t<0, decltype(note_line<file, line>(Reflect::line_t<file, line>{}, Reflect::adl{}))>::value; \
        template <uint32_t line> static constexpr bool isClassName = \
            std::get<1>(note_line<file, line>(Reflect::line_t<file, line>{}, Reflect::adl{})) == ClassTypeName; \
        template <typename T, uint32_t NoteLine> struct GetNote \
        { static constexpr auto value = std::get<2>(note_line<file, NoteLine>(Reflect::line_t<file, NoteLine>{}, Reflect::adl{})); }; \
        template <typename T> struct GetNote<T, 0> { static constexpr auto & value = NoNote; }; \
        static constexpr auto value = GetNote<ClassType, Reflect::getClassAnnotationLine<GetClassNote<FieldIndex>, 0, __LINE__>()>::value; \
    }; \
    FOR_EACH(NEW_DESCRIBE_FIELD, __VA_ARGS__) \
    static constexpr size_t TotalFields = COUNT_ARGUMENTS(__VA_ARGS__); \
    static constexpr size_t TotalStaticFields = 0 FOR_EACH(ADD_IF_STATIC, __VA_ARGS__); \
    static constexpr Reflect::Fields::Field<> Fields[TotalFields] = { FOR_EACH(GET_FIELD, __VA_ARGS__) }; \
    template <typename Function> constexpr static auto ForEachField(Function function) { FOR_EACH(USE_FIELD, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(objectType & object, Function function) { FOR_EACH(USE_FIELD_VALUE, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(const objectType & object, Function function) { FOR_EACH(USE_FIELD_VALUE, __VA_ARGS__) } \
    template <typename Function> constexpr static void FieldAt(size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_AT, __VA_ARGS__) } } \
    template <typename Function> static void FieldAt(objectType & object, size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_VALUE_AT, __VA_ARGS__) } } \
    static constexpr int FirstAnnotatedFieldIndex = []{ FOR_EACH(GET_FIELD_IF_ANNOTATED, __VA_ARGS__) return -1; }(); \
    static constexpr auto annotations = GetClassNote<FirstAnnotatedFieldIndex>::value; \
    using Annotations = decltype(annotations); \
};

namespace ns {

    struct A
    {
        int one;

        int two;

        REFLECTED(A, one, two)
    };

    ANNOTATE(B, Json::Name{"RenameB"})
    struct B
    {
        ANNOTATE(one, Json::Name{"RenameOne"})
        int one;

        ANNOTATE(two, Json::Ignore)
        int two;

        REFLECTED(B, one, two)
    };
    
    ANNOTATE(C, Json::Name{"RenameC"})
    struct C// : B //-- The Sticking Point
    {
        ANNOTATE(one, Json::Name{"RenameOne"})
        int one;
        
        ANNOTATE(two, Json::Name{"RenameOne"})
        int two;

        REFLECTED(C, one, two)
    };

    struct D
    {
        int one;
        int two;

        REFLECTED(D, one, two)
    };
}

void putAnnotateTests()
{
    std::cout << ExtendedTypeSupport::TypeToStr<ns::C::Class>() << std::endl;
    std::cout << ExtendedTypeSupport::TypeToStr<ns::A::Class::Annotations>() << std::endl;
    std::cout << ExtendedTypeSupport::TypeToStr<ns::B::Class::Annotations>() << std::endl;
    std::cout << ExtendedTypeSupport::TypeToStr<ns::C::Class::Annotations>() << std::endl;
    std::cout << ExtendedTypeSupport::TypeToStr<ns::D::Class::Annotations>() << std::endl;
    
    std::cout << std::get<0>(ns::B::Class::annotations).value << std::endl;
    //std::cout << std::get<0>(ns::C::Class::annotations).value << std::endl;
}

#endif