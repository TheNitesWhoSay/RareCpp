#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
using namespace RareTs;

struct SerializedName
{
    std::string_view value;
};

template <typename T>
struct Specializable {
    T value;
};

struct TestSuper {};
struct TestOtherSuper {};
struct TestAnnotation { int value; };
struct TestOtherAnnotation { int otherValue; };
struct ThirdAnnotation {};

NOTE(SingleSuper, Super<TestSuper>)
struct SingleSuper : TestSuper { REFLECT_NOTED(SingleSuper) };

NOTE(DoubleSuper, Super<TestSuper>, Super<TestOtherSuper>)
struct DoubleSuper : TestSuper, TestOtherSuper { REFLECT_NOTED(DoubleSuper) };

NOTE(SingleSuperNote, Super<TestSuper>(TestAnnotation{33}))
struct SingleSuperNote : TestSuper { REFLECT_NOTED(SingleSuperNote) };

NOTE(SingleSuperOtherNote, Super<TestSuper>(TestOtherAnnotation{2}))
struct SingleSuperOtherNote : TestSuper { REFLECT_NOTED(SingleSuperOtherNote) };

NOTE(DoubleSuperNotes,
    Super<TestSuper>(TestAnnotation{33}, TestOtherAnnotation{44}, TestOtherAnnotation{55}),
    Super<TestOtherSuper>(TestAnnotation{1}, TestOtherAnnotation{2}))
struct DoubleSuperNotes : TestSuper, TestOtherSuper { REFLECT_NOTED(DoubleSuperNotes) };

NOTE(SpecializedSuperAnnotation,
    Super<TestSuper>('a', Specializable<int>{0}, 'b', Specializable<int>{1}))
struct SpecializedSuperAnnotation : TestSuper { REFLECT_NOTED(SpecializedSuperAnnotation) };

using SingleSuper_SuperNotes = RareTs::Reflection::Inheritance::detail::SuperNotes<SingleSuper>;
using DoubleSuper_SuperNotes = RareTs::Reflection::Inheritance::detail::SuperNotes<DoubleSuper>;
using SingleSuperNote_SuperNotes = RareTs::Reflection::Inheritance::detail::SuperNotes<SingleSuperNote>;
using SingleSuperOtherNote_SuperNotes = RareTs::Reflection::Inheritance::detail::SuperNotes<SingleSuperOtherNote>;
using DoubleSuperNotes_SuperNotes = RareTs::Reflection::Inheritance::detail::SuperNotes<DoubleSuperNotes>;
using SpecializedSuperAnnotation_SuperNotes = RareTs::Reflection::Inheritance::detail::SuperNotes<SpecializedSuperAnnotation>;

using SingleSuper_TestSuper_Note = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<SingleSuper_SuperNotes, 0>(std::make_index_sequence<0>()));
using DoubleSuper_TestSuper_Note = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<DoubleSuper_SuperNotes, 0>(std::make_index_sequence<0>()));
using DoubleSuper_TestOtherSuper_Note = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<DoubleSuper_SuperNotes, 1>(std::make_index_sequence<0>()));
using SingleSuperNote_TestSuper_Note = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<SingleSuperNote_SuperNotes, 0>(std::make_index_sequence<1>()));
using SingleSuperOtherNote_TestSuper_Note = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<SingleSuperOtherNote_SuperNotes, 0>(std::make_index_sequence<1>()));
using DoubleSuperNotes_TestSuper_Note = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<DoubleSuperNotes_SuperNotes, 0>(std::make_index_sequence<3>()));
using DoubleSuperNotes_TestOtherSuper_Note = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<DoubleSuperNotes_SuperNotes, 1>(std::make_index_sequence<2>()));
using SpecializedSuperAnnotation_TestSuper_Note = decltype(RareTs::Reflection::Annotations::detail::subAnnotationsType<SpecializedSuperAnnotation_SuperNotes, 0>(std::make_index_sequence<4>()));

TEST(ReflectInheritanceTest, SuperInfo)
{
    bool isEqual = std::is_same_v<TestSuper, SuperInfo<0, TestSuper, SingleSuper_TestSuper_Note>::type>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<TestOtherSuper, SuperInfo<1, TestOtherSuper, DoubleSuper_TestOtherSuper_Note>::type>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<NoNote, SuperInfo<0, TestSuper, SingleSuper_TestSuper_Note>::Notes>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<NoNote, SuperInfo<1, TestOtherSuper, DoubleSuper_TestOtherSuper_Note>::Notes>;
    EXPECT_TRUE(isEqual);
    isEqual = 0 == SuperInfo<0, TestSuper, DoubleSuper_TestSuper_Note>::index;
    EXPECT_TRUE(isEqual);
    isEqual = 1 == SuperInfo<1, TestOtherSuper, DoubleSuper_TestOtherSuper_Note>::index;
    EXPECT_TRUE(isEqual);
    
    bool hasAnnotation = SuperInfo<0, TestSuper, SingleSuper_TestSuper_Note>::template hasNote<TestAnnotation>();
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<0, TestSuper, SingleSuper_TestSuper_Note>::template hasNote<TestAnnotation>();
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<0, TestSuper, SingleSuperNote_TestSuper_Note>::template hasNote<TestAnnotation>();
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = SuperInfo<0, TestSuper, SingleSuperOtherNote_TestSuper_Note>::template hasNote<TestAnnotation>();
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<0, TestSuper, DoubleSuperNotes_TestSuper_Note>::template hasNote<TestAnnotation>();
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = SuperInfo<0, TestSuper, DoubleSuperNotes_TestSuper_Note>::template hasNote<TestOtherAnnotation>();
    EXPECT_TRUE(hasAnnotation);

    SuperInfo<0, TestSuper, SingleSuper_TestSuper_Note> testSuperNoAnnotations{};
    using Unnoted = decltype(testSuperNoAnnotations);
    hasAnnotation = Unnoted::template hasNote<TestAnnotation>();
    EXPECT_FALSE(hasAnnotation);

    SuperInfo<0, TestSuper, SingleSuperNote_TestSuper_Note> annotated{};
    using Annotated = decltype(annotated);
    hasAnnotation = Annotated::template hasNote<TestAnnotation>();
    EXPECT_TRUE(hasAnnotation);

    EXPECT_EQ(33, annotated.getNote<TestAnnotation>().value);

    int timesVisited = 0;
    annotated.forEachNote<TestAnnotation>([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    annotated.forEachNote([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    SuperInfo<0, TestSuper, DoubleSuperNotes_TestSuper_Note> otherAnnotated{};
    using OtherAnnotated = decltype(otherAnnotated);
    hasAnnotation = OtherAnnotated::template hasNote<ThirdAnnotation>();
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = OtherAnnotated::template hasNote<TestAnnotation>();
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = OtherAnnotated::template hasNote<TestOtherAnnotation>();
    EXPECT_TRUE(hasAnnotation);

    auto otherAnnotatedTestAnnotation = otherAnnotated.getNote<TestAnnotation>();
    EXPECT_EQ(33, otherAnnotatedTestAnnotation.value);
    auto otherAnnotatedTestOtherAnnotation = otherAnnotated.getNote<TestOtherAnnotation>();
    EXPECT_EQ(44, otherAnnotatedTestOtherAnnotation.otherValue);

    timesVisited = 0;
    otherAnnotated.forEachNote<TestAnnotation>([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEachNote<TestOtherAnnotation>([&](auto & annotation) {
        if ( timesVisited == 0 ) {
            EXPECT_EQ(44, annotation.otherValue);
        } else if ( timesVisited == 1 ) {
            EXPECT_EQ(55, annotation.otherValue);
        }

        timesVisited++;
    });
    EXPECT_EQ(2, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEachNote<ThirdAnnotation>([&](auto &) {
        timesVisited ++;
    });
    EXPECT_EQ(0, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEachNote([&](auto & annotation) {
        using AnnotationType = RareTs::remove_cvref_t<decltype(annotation)>;
        constexpr bool isTestAnnotation = std::is_same_v<TestAnnotation, AnnotationType>;
        constexpr bool isTestOtherAnnotation = std::is_same_v<TestOtherAnnotation, AnnotationType>;
        EXPECT_TRUE(isTestAnnotation || isTestOtherAnnotation);
        if constexpr ( isTestAnnotation )
        {
            EXPECT_EQ(33, annotation.value);
        }
        else if constexpr ( isTestOtherAnnotation )
        {
            if ( timesVisited == 1 ) {
                EXPECT_EQ(44, annotation.otherValue);
            } else if ( timesVisited == 2 ) {
                EXPECT_EQ(55, annotation.otherValue);
            }
        }
        timesVisited ++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    using SpecializedNotes = SuperInfo<0, TestSuper, SpecializedSuperAnnotation_TestSuper_Note>;
    SpecializedNotes::forEachNote<Specializable>([&](auto & annotation) {
        EXPECT_EQ(timesVisited, annotation.value);
        timesVisited++;
    });
    EXPECT_EQ(2, timesVisited);
}

NOTE(BaseClass, Super<TestSuper>, Super<TestOtherSuper>(SerializedName{"testOther"}, 'a'), 'b')
struct BaseClass : TestSuper, TestOtherSuper {};

TEST(ReflectInheritanceTest, SuperAnnotation)
{
    EXPECT_TRUE(is_super_v<std::remove_reference_t<decltype(std::get<0>(BaseClass_note))>>);
    EXPECT_TRUE(is_super_v<std::remove_reference_t<decltype(std::get<1>(BaseClass_note))>>);
    EXPECT_FALSE(is_super_v<std::remove_reference_t<decltype(std::get<2>(BaseClass_note))>>);

    auto & firstSuper = std::get<0>(BaseClass_note);
    auto & secondSuper = std::get<1>(BaseClass_note);
    using FirstSuperWrapper = std::remove_reference_t<decltype(firstSuper)>;
    using SecondSuperWrapper = std::remove_reference_t<decltype(secondSuper)>;
    using FirstSuper = typename FirstSuperWrapper::type;
    using SecondSuper = typename SecondSuperWrapper::type;
    
    bool isSame = std::is_same_v<TestSuper, FirstSuper>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<TestOtherSuper, SecondSuper>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<NoNote, std::remove_const_t<FirstSuperWrapper::Notes>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<NoNote, std::remove_const_t<decltype(FirstSuperWrapper::notes)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(size_t(0), std::tuple_size_v<decltype(FirstSuperWrapper::notes)>);

    isSame = std::is_same_v<std::tuple<SerializedName, char>, std::remove_const_t<SecondSuperWrapper::Notes>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<SerializedName, char>, std::remove_const_t<decltype(SecondSuperWrapper::notes)>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(size_t(2), std::tuple_size_v<decltype(SecondSuperWrapper::notes)>);
    EXPECT_STREQ("testOther", std::string(std::get<0>(secondSuper.notes).value).c_str());
    EXPECT_EQ('a', std::get<1>(secondSuper.notes));
    EXPECT_EQ('b', std::get<2>(BaseClass_note));
}

// Parent classes
struct InheritSuper_S1 { REFLECT(InheritSuper_S1) };
struct InheritSuper_S2 { REFLECT(InheritSuper_S2) };
struct InheritSuper_S3 { REFLECT(InheritSuper_S3) };

// Child classes
struct InheritSuper_S0a { REFLECT(InheritSuper_S0a) };
struct InheritSuper_S0b { REFLECT(InheritSuper_S0b) };
struct InheritSuper_S0c { REFLECT(InheritSuper_S0c) };

NOTE(InheritSuper_S1a, Super<InheritSuper_S1>)
struct InheritSuper_S1a { REFLECT_NOTED(InheritSuper_S1a) };
NOTE(InheritSuper_S1b, Super<InheritSuper_S1>('c', 'd'))
struct InheritSuper_S1b { REFLECT_NOTED(InheritSuper_S1b) };
NOTE(InheritSuper_S1c, 0, Super<InheritSuper_S1>)
struct InheritSuper_S1c { REFLECT_NOTED(InheritSuper_S1c) };
NOTE(InheritSuper_S1d, 0, Super<InheritSuper_S1>, 0, 0)
struct InheritSuper_S1d { REFLECT_NOTED(InheritSuper_S1d) };

NOTE(InheritSuper_S2a, Super<InheritSuper_S1>, Super<InheritSuper_S2>)
struct InheritSuper_S2a { REFLECT_NOTED(InheritSuper_S2a) };
NOTE(InheritSuper_S2b, Super<InheritSuper_S1>, Super<InheritSuper_S2>('e', 'f'))
struct InheritSuper_S2b { REFLECT_NOTED(InheritSuper_S2b) };
NOTE(InheritSuper_S2c, 0, Super<InheritSuper_S1>, Super<InheritSuper_S2>)
struct InheritSuper_S2c { REFLECT_NOTED(InheritSuper_S2c) };
NOTE(InheritSuper_S2d, 0, Super<InheritSuper_S1>, Super<InheritSuper_S2>('g', 'h'), 0, 0)
struct InheritSuper_S2d { REFLECT_NOTED(InheritSuper_S2d) };

NOTE(InheritSuper_S3a, Super<InheritSuper_S1>, Super<InheritSuper_S2>, Super<InheritSuper_S3>)
struct InheritSuper_S3a { REFLECT_NOTED(InheritSuper_S3a) };
NOTE(InheritSuper_S3b, Super<InheritSuper_S1>, Super<InheritSuper_S2>('i', 'j'), Super<InheritSuper_S3>)
struct InheritSuper_S3b { REFLECT_NOTED(InheritSuper_S3b) };
NOTE(InheritSuper_S3c, 0, Super<InheritSuper_S1>, Super<InheritSuper_S2>, Super<InheritSuper_S3>)
struct InheritSuper_S3c { REFLECT_NOTED(InheritSuper_S3c) };
NOTE(InheritSuper_S3d, 0, 0, Super<InheritSuper_S1>, 0, Super<InheritSuper_S2>('k', 'l'), Super<InheritSuper_S3>)
struct InheritSuper_S3d { REFLECT_NOTED(InheritSuper_S3d) };

using InheritSuper_0a = Inherit<InheritSuper_S0a>;
using InheritSuper_0b = Inherit<InheritSuper_S0b>;
using InheritSuper_0c = Inherit<InheritSuper_S0c>;

using InheritSuper_1a = Inherit<InheritSuper_S1a, 0>;
using InheritSuper_1b = Inherit<InheritSuper_S1b, 0>;
using InheritSuper_1c = Inherit<InheritSuper_S1c, 0>;
using InheritSuper_1d = Inherit<InheritSuper_S1d, 0>;

using InheritSuper_2a = Inherit<InheritSuper_S2a, 0, 1>;
using InheritSuper_2b = Inherit<InheritSuper_S2b, 0, 1>;
using InheritSuper_2c = Inherit<InheritSuper_S2c, 0, 1>;
using InheritSuper_2d = Inherit<InheritSuper_S2d, 0, 1>;

using InheritSuper_3a = Inherit<InheritSuper_S3a, 0, 1, 2>;
using InheritSuper_3b = Inherit<InheritSuper_S3b, 0, 1, 2>;
using InheritSuper_3c = Inherit<InheritSuper_S3c, 0, 1, 2>;
using InheritSuper_3d = Inherit<InheritSuper_S3d, 0, 1, 2>;

InheritSuper_S0a inheritSuper_S0a;
InheritSuper_S0b inheritSuper_S0b;
InheritSuper_S0c inheritSuper_S0c;
    
InheritSuper_S1a inheritSuper_S1a;
InheritSuper_S1b inheritSuper_S1b;
InheritSuper_S1c inheritSuper_S1c;
InheritSuper_S1d inheritSuper_S1d;
    
InheritSuper_S2a inheritSuper_S2a;
InheritSuper_S2b inheritSuper_S2b;
InheritSuper_S2c inheritSuper_S2c;
InheritSuper_S2d inheritSuper_S2d;
    
InheritSuper_S3a inheritSuper_S3a;
InheritSuper_S3b inheritSuper_S3b;
InheritSuper_S3c inheritSuper_S3c;
InheritSuper_S3d inheritSuper_S3d;

TEST(ReflectInheritanceTest, InheritTotalSupers)
{
    EXPECT_EQ(size_t(0), InheritSuper_0a::total);
    EXPECT_EQ(size_t(0), InheritSuper_0b::total);
    EXPECT_EQ(size_t(0), InheritSuper_0c::total);
    
    EXPECT_EQ(size_t(1), InheritSuper_1a::total);
    EXPECT_EQ(size_t(1), InheritSuper_1b::total);
    EXPECT_EQ(size_t(1), InheritSuper_1c::total);
    EXPECT_EQ(size_t(1), InheritSuper_1d::total);
    
    EXPECT_EQ(size_t(2), InheritSuper_2a::total);
    EXPECT_EQ(size_t(2), InheritSuper_2b::total);
    EXPECT_EQ(size_t(2), InheritSuper_2c::total);
    EXPECT_EQ(size_t(2), InheritSuper_2d::total);
    
    EXPECT_EQ(size_t(3), InheritSuper_3a::total);
    EXPECT_EQ(size_t(3), InheritSuper_3b::total);
    EXPECT_EQ(size_t(3), InheritSuper_3c::total);
    EXPECT_EQ(size_t(3), InheritSuper_3d::total);
}

TEST(ReflectInheritanceTest, InheritForEachStatic)
{
    int visitCount = 0;
    InheritSuper_0a::forEach([&](auto) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::forEach([&](auto) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::forEach([&](auto) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.notes));
        EXPECT_EQ('d', std::get<1>(superInfo.notes));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('e', std::get<0>(superInfo.notes));
            EXPECT_EQ('f', std::get<1>(superInfo.notes));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('g', std::get<0>(superInfo.notes));
            EXPECT_EQ('h', std::get<1>(superInfo.notes));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('i', std::get<0>(superInfo.notes));
            EXPECT_EQ('j', std::get<1>(superInfo.notes));
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('k', std::get<0>(superInfo.notes));
            EXPECT_EQ('l', std::get<1>(superInfo.notes));
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritForEachInstance)
{
    int visitCount = 0;
    InheritSuper_0a::forEach(inheritSuper_S0a, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::forEach(inheritSuper_S0b, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::forEach(inheritSuper_S0c, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::forEach(inheritSuper_S1a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::forEach(inheritSuper_S1b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.notes));
        EXPECT_EQ('d', std::get<1>(superInfo.notes));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::forEach(inheritSuper_S1c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::forEach(inheritSuper_S1d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::forEach(inheritSuper_S2a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::forEach(inheritSuper_S2b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('e', std::get<0>(superInfo.notes));
            EXPECT_EQ('f', std::get<1>(superInfo.notes));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::forEach(inheritSuper_S2c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::forEach(inheritSuper_S2d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('g', std::get<0>(superInfo.notes));
            EXPECT_EQ('h', std::get<1>(superInfo.notes));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::forEach(inheritSuper_S3a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::forEach(inheritSuper_S3b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('i', std::get<0>(superInfo.notes));
            EXPECT_EQ('j', std::get<1>(superInfo.notes));
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::forEach(inheritSuper_S3c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::forEach(inheritSuper_S3d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('k', std::get<0>(superInfo.notes));
            EXPECT_EQ('l', std::get<1>(superInfo.notes));
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritForEachInstanceConst)
{
    int visitCount = 0;
    InheritSuper_0a::forEach((const decltype(inheritSuper_S0a) &)inheritSuper_S0a, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::forEach((const decltype(inheritSuper_S0b) &)inheritSuper_S0b, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::forEach((const decltype(inheritSuper_S0c) &)inheritSuper_S0c, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::forEach((const decltype(inheritSuper_S1a) &)inheritSuper_S1a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::forEach((const decltype(inheritSuper_S1b) &)inheritSuper_S1b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.notes));
        EXPECT_EQ('d', std::get<1>(superInfo.notes));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::forEach((const decltype(inheritSuper_S1c) &)inheritSuper_S1c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::forEach((const decltype(inheritSuper_S1d) &)inheritSuper_S1d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::forEach((const decltype(inheritSuper_S2a) &)inheritSuper_S2a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::forEach((const decltype(inheritSuper_S2b) &)inheritSuper_S2b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('e', std::get<0>(superInfo.notes));
            EXPECT_EQ('f', std::get<1>(superInfo.notes));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::forEach((const decltype(inheritSuper_S2c) &)inheritSuper_S2c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::forEach((const decltype(inheritSuper_S2d) &)inheritSuper_S2d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('g', std::get<0>(superInfo.notes));
            EXPECT_EQ('h', std::get<1>(superInfo.notes));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::forEach((const decltype(inheritSuper_S3a) &)inheritSuper_S3a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::forEach((const decltype(inheritSuper_S3b) &)inheritSuper_S3b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('i', std::get<0>(superInfo.notes));
            EXPECT_EQ('j', std::get<1>(superInfo.notes));
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::forEach((const decltype(inheritSuper_S3c) &)inheritSuper_S3c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::forEach((const decltype(inheritSuper_S3d) &)inheritSuper_S3d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(visitCount), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        if constexpr ( superInfo.index == size_t(0) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S1, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(1) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S2, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('k', std::get<0>(superInfo.notes));
            EXPECT_EQ('l', std::get<1>(superInfo.notes));
            visitCount++;
        }
        else if constexpr ( superInfo.index == size_t(2) )
        {
            bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritSuper_S3, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritForEachSuper)
{
    int visitCount = 0;
    InheritSuper_0a::forEachSuper(inheritSuper_S0a, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::forEachSuper(inheritSuper_S0b, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::forEachSuper(inheritSuper_S0c, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::forEachSuper(inheritSuper_S1a, [&](auto & super) {
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::forEachSuper(inheritSuper_S1b, [&](auto & super) {
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::forEachSuper(inheritSuper_S1c, [&](auto & super) {
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::forEachSuper(inheritSuper_S1d, [&](auto & super) {
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::forEachSuper(inheritSuper_S2a, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::forEachSuper(inheritSuper_S2b, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::forEachSuper(inheritSuper_S2c, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::forEachSuper(inheritSuper_S2d, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::forEachSuper(inheritSuper_S3a, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::forEachSuper(inheritSuper_S3b, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::forEachSuper(inheritSuper_S3c, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::forEachSuper(inheritSuper_S3d, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritForEachSuperConst)
{
    int visitCount = 0;
    InheritSuper_0a::forEachSuper((const decltype(inheritSuper_S0a) &)inheritSuper_S0a, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::forEachSuper((const decltype(inheritSuper_S0b) &)inheritSuper_S0b, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::forEachSuper((const decltype(inheritSuper_S0c) &)inheritSuper_S0c, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::forEachSuper((const decltype(inheritSuper_S1a) &)inheritSuper_S1a, [&](auto & super) {
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::forEachSuper((const decltype(inheritSuper_S1b) &)inheritSuper_S1b, [&](auto & super) {
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::forEachSuper((const decltype(inheritSuper_S1c) &)inheritSuper_S1c, [&](auto & super) {
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::forEachSuper((const decltype(inheritSuper_S1d) &)inheritSuper_S1d, [&](auto & super) {
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::forEachSuper((const decltype(inheritSuper_S2a) &)inheritSuper_S2a, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::forEachSuper((const decltype(inheritSuper_S2b) &)inheritSuper_S2b, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::forEachSuper((const decltype(inheritSuper_S2c) &)inheritSuper_S2c, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::forEachSuper((const decltype(inheritSuper_S2d) &)inheritSuper_S2d, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::forEachSuper((const decltype(inheritSuper_S3a) &)inheritSuper_S3a, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::forEachSuper((const decltype(inheritSuper_S3b) &)inheritSuper_S3b, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::forEachSuper((const decltype(inheritSuper_S3c) &)inheritSuper_S3c, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::forEachSuper((const decltype(inheritSuper_S3d) &)inheritSuper_S3d, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritAtStatic)
{
    int visitCount = 0;
    InheritSuper_0a::at(0, [&](auto) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::at(0, [&](auto) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::at(0, [&](auto) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::at(0, [&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::at(0, [&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.notes));
        EXPECT_EQ('d', std::get<1>(superInfo.notes));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::at(0, [&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::at(0, [&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2a::total; i++ )
    {
        InheritSuper_2a::at(i, [&](auto superInfo) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2b::total; i++ )
    {
        InheritSuper_2b::at(i, [&](auto superInfo) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('e', std::get<0>(superInfo.notes));
                EXPECT_EQ('f', std::get<1>(superInfo.notes));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2c::total; i++ )
    {
        InheritSuper_2c::at(i, [&](auto superInfo) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2d::total; i++ )
    {
        InheritSuper_2d::at(i, [&](auto superInfo) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('g', std::get<0>(superInfo.notes));
                EXPECT_EQ('h', std::get<1>(superInfo.notes));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3a::total; i++ )
    {
        InheritSuper_3a::at(i, [&](auto superInfo) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3b::total; i++ )
    {
        InheritSuper_3b::at(i, [&](auto superInfo) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('i', std::get<0>(superInfo.notes));
                EXPECT_EQ('j', std::get<1>(superInfo.notes));
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3c::total; i++ )
    {
        InheritSuper_3c::at(i, [&](auto superInfo) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3d::total; i++ )
    {
        InheritSuper_3d::at(i, [&](auto superInfo) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('k', std::get<0>(superInfo.notes));
                EXPECT_EQ('l', std::get<1>(superInfo.notes));
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritAtInstance)
{
    int visitCount = 0;
    InheritSuper_0a::at(0, inheritSuper_S0a, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::at(0, inheritSuper_S0b, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::at(0, inheritSuper_S0c, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::at(0, inheritSuper_S1a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::at(0, inheritSuper_S1b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.notes));
        EXPECT_EQ('d', std::get<1>(superInfo.notes));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::at(0, inheritSuper_S1c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::at(0, inheritSuper_S1d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2a::total; i++ )
    {
        InheritSuper_2a::at(i, inheritSuper_S2a, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2b::total; i++ )
    {
        InheritSuper_2b::at(i, inheritSuper_S2b, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('e', std::get<0>(superInfo.notes));
                EXPECT_EQ('f', std::get<1>(superInfo.notes));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2c::total; i++ )
    {
        InheritSuper_2c::at(i, inheritSuper_S2c, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2d::total; i++ )
    {
        InheritSuper_2d::at(i, inheritSuper_S2d, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('g', std::get<0>(superInfo.notes));
                EXPECT_EQ('h', std::get<1>(superInfo.notes));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3a::total; i++ )
    {
        InheritSuper_3a::at(i, inheritSuper_S3a, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3b::total; i++ )
    {
        InheritSuper_3b::at(i, inheritSuper_S3b, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('i', std::get<0>(superInfo.notes));
                EXPECT_EQ('j', std::get<1>(superInfo.notes));
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3c::total; i++ )
    {
        InheritSuper_3c::at(i, inheritSuper_S3c, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3d::total; i++ )
    {
        InheritSuper_3d::at(i, inheritSuper_S3d, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('k', std::get<0>(superInfo.notes));
                EXPECT_EQ('l', std::get<1>(superInfo.notes));
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritAtInstanceConst)
{
    int visitCount = 0;
    InheritSuper_0a::at(0, (const decltype(inheritSuper_S0a) &)inheritSuper_S0a, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::at(0, (const decltype(inheritSuper_S0b) &)inheritSuper_S0b, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::at(0, (const decltype(inheritSuper_S0c) &)inheritSuper_S0c, [&](auto, auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::at(0, (const decltype(inheritSuper_S1a) &)inheritSuper_S1a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::at(0, (const decltype(inheritSuper_S1b) &)inheritSuper_S1b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.notes));
        EXPECT_EQ('d', std::get<1>(superInfo.notes));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::at(0, (const decltype(inheritSuper_S1c) &)inheritSuper_S1c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::at(0, (const decltype(inheritSuper_S1d) &)inheritSuper_S1d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritSuper_S1, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2a::total; i++ )
    {
        InheritSuper_2a::at(i, (const decltype(inheritSuper_S2a) &)inheritSuper_S2a, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2b::total; i++ )
    {
        InheritSuper_2b::at(i, (const decltype(inheritSuper_S2b) &)inheritSuper_S2b, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('e', std::get<0>(superInfo.notes));
                EXPECT_EQ('f', std::get<1>(superInfo.notes));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2c::total; i++ )
    {
        InheritSuper_2c::at(i, (const decltype(inheritSuper_S2c) &)inheritSuper_S2c, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2d::total; i++ )
    {
        InheritSuper_2d::at(i, (const decltype(inheritSuper_S2d) &)inheritSuper_S2d, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('g', std::get<0>(superInfo.notes));
                EXPECT_EQ('h', std::get<1>(superInfo.notes));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3a::total; i++ )
    {
        InheritSuper_3a::at(i, (const decltype(inheritSuper_S3a) &)inheritSuper_S3a, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3b::total; i++ )
    {
        InheritSuper_3b::at(i, (const decltype(inheritSuper_S3b) &)inheritSuper_S3b, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('i', std::get<0>(superInfo.notes));
                EXPECT_EQ('j', std::get<1>(superInfo.notes));
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3c::total; i++ )
    {
        InheritSuper_3c::at(i, (const decltype(inheritSuper_S3c) &)inheritSuper_S3c, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3d::total; i++ )
    {
        InheritSuper_3d::at(i, (const decltype(inheritSuper_S3d) &)inheritSuper_S3d, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(visitCount), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            if constexpr ( superInfo.index == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S1, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.index == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S2, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<char, char>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('k', std::get<0>(superInfo.notes));
                EXPECT_EQ('l', std::get<1>(superInfo.notes));
                visitCount++;
            }
            else if constexpr ( superInfo.index == 2 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<InheritSuper_S3, InfoType>;
                EXPECT_TRUE(isSame);
                isSame = std::is_same_v<std::tuple<>, InfoAnnotations>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritSuperAt)
{
    int visitCount = 0;
    InheritSuper_0a::superAt(0, inheritSuper_S0a, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::superAt(0, inheritSuper_S0b, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::superAt(0, inheritSuper_S0c, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::superAt(0, inheritSuper_S1a, [&](auto & super) {
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::superAt(0, inheritSuper_S1b, [&](auto & super) {
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::superAt(0, inheritSuper_S1c, [&](auto & super) {
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::superAt(0, inheritSuper_S1d, [&](auto & super) {
        bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2a::total; i++ )
    {
        InheritSuper_2a::superAt(i, inheritSuper_S2a, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2b::total; i++ )
    {
        InheritSuper_2b::superAt(i, inheritSuper_S2b, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2c::total; i++ )
    {
        InheritSuper_2c::superAt(i, inheritSuper_S2c, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2d::total; i++ )
    {
        InheritSuper_2d::superAt(i, inheritSuper_S2d, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3a::total; i++ )
    {
        InheritSuper_3a::superAt(i, inheritSuper_S3a, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3b::total; i++ )
    {
        InheritSuper_3b::superAt(i, inheritSuper_S3b, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3c::total; i++ )
    {
        InheritSuper_3c::superAt(i, inheritSuper_S3c, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3d::total; i++ )
    {
        InheritSuper_3d::superAt(i, inheritSuper_S3d, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same_v<InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritSuperAtConst)
{
    int visitCount = 0;
    InheritSuper_0a::superAt(0, (const decltype(inheritSuper_S0a) &)inheritSuper_S0a, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::superAt(0, (const decltype(inheritSuper_S0b) &)inheritSuper_S0b, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::superAt(0, (const decltype(inheritSuper_S0c) &)inheritSuper_S0c, [&](auto &) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::superAt(0, (const decltype(inheritSuper_S1a) &)inheritSuper_S1a, [&](auto & super) {
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::superAt(0, (const decltype(inheritSuper_S1b) &)inheritSuper_S1b, [&](auto & super) {
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::superAt(0, (const decltype(inheritSuper_S1c) &)inheritSuper_S1c, [&](auto & super) {
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::superAt(0, (const decltype(inheritSuper_S1d) &)inheritSuper_S1d, [&](auto & super) {
        bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2a::total; i++ )
    {
        InheritSuper_2a::superAt(i, (const decltype(inheritSuper_S2a) &)inheritSuper_S2a, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2b::total; i++ )
    {
        InheritSuper_2b::superAt(i, (const decltype(inheritSuper_S2b) &)inheritSuper_S2b, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2c::total; i++ )
    {
        InheritSuper_2c::superAt(i, (const decltype(inheritSuper_S2c) &)inheritSuper_S2c, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_2d::total; i++ )
    {
        InheritSuper_2d::superAt(i, (const decltype(inheritSuper_S2d) &)inheritSuper_S2d, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3a::total; i++ )
    {
        InheritSuper_3a::superAt(i, (const decltype(inheritSuper_S3a) &)inheritSuper_S3a, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3b::total; i++ )
    {
        InheritSuper_3b::superAt(i, (const decltype(inheritSuper_S3b) &)inheritSuper_S3b, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3c::total; i++ )
    {
        InheritSuper_3c::superAt(i, (const decltype(inheritSuper_S3c) &)inheritSuper_S3c, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( size_t i=0; i<InheritSuper_3d::total; i++ )
    {
        InheritSuper_3d::superAt(i, (const decltype(inheritSuper_S3d) &)inheritSuper_S3d, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S1, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S2, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same_v<const InheritSuper_S3, std::remove_reference_t<decltype(super)>>;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
}

NOTE(Grandparent)
struct Grandparent { REFLECT_NOTED(Grandparent) };
NOTE(Parent, Super<Grandparent>)
struct Parent : Grandparent { REFLECT_NOTED(Parent) };
NOTE(Child, Super<Parent>)
struct Child : Parent { REFLECT_NOTED(Child) };

NOTE(InheritGrandparent, 'a')
struct InheritGrandparent { REFLECT_NOTED(InheritGrandparent) };
NOTE(InheritParent, Super<InheritGrandparent>('b'), 'c')
struct InheritParent { REFLECT_NOTED(InheritParent) };
NOTE(InheritChild, Super<InheritParent>('d'), 'e')
struct InheritChild { REFLECT_NOTED(InheritChild) };

TEST(ReflectInheritanceTest, InheritRecursion)
{
    EXPECT_EQ(size_t(1), Reflect<InheritChild>::Supers::total);
    InheritChild child = {};
    EXPECT_EQ('e', std::get<1>(Class::class_t<InheritChild>::notes));

    // Inherit for each instance
    int outerVisitCount = 0;
    int innerVisitCount = 0;
    Reflect<InheritChild>::Supers::forEach(child, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritParent, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritParent, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.notes));
        
        EXPECT_EQ('c', std::get<1>(Class::class_t<InfoType>::notes));
        Reflect<InfoType>::Supers::forEach(super, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(0), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            bool isSame = std::is_same_v<InheritGrandparent, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritGrandparent, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.notes));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);
    
    // Inherit for each instance const
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::forEach((const decltype(child) &)child, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritParent, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritParent, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.notes));
        
        EXPECT_EQ('c', std::get<1>(Class::class_t<InfoType>::notes));
        Reflect<InfoType>::Supers::forEach((const decltype(super) &)super, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(0), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            bool isSame = std::is_same_v<const InheritGrandparent, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritGrandparent, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.notes));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);

    // Inherit for each static
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::forEach([&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritParent, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.notes));
        
        EXPECT_EQ('c', std::get<1>(Class::class_t<InfoType>::notes));
        Reflect<InfoType>::Supers::forEach([&](auto superInfo) {
            EXPECT_EQ(size_t(0), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            bool isSame = std::is_same_v<InheritGrandparent, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.notes));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);

    // Inherit at instance
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::at(0, child, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritParent, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritParent, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.notes));
        
        EXPECT_EQ('c', std::get<1>(Class::class_t<InfoType>::notes));
        Reflect<InfoType>::Supers::at(0, super, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(0), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            bool isSame = std::is_same_v<InheritGrandparent, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritGrandparent, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.notes));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);

    // Inherit at instance const
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::at(0, (const decltype(child) &)child, [&](auto superInfo, auto & super) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<const InheritParent, std::remove_reference_t<decltype(super)>>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<InheritParent, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.notes));
        
        EXPECT_EQ('c', std::get<1>(Class::class_t<InfoType>::notes));
        Reflect<InfoType>::Supers::at(0, (const decltype(super) &)super, [&](auto superInfo, auto & super) {
            EXPECT_EQ(size_t(0), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            bool isSame = std::is_same_v<const InheritGrandparent, std::remove_reference_t<decltype(super)>>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<InheritGrandparent, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.notes));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);

    // Inherit at static
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::at(0, [&](auto superInfo) {
        EXPECT_EQ(size_t(0), superInfo.index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::type;
        using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
        bool isSame = std::is_same_v<InheritParent, InfoType>;
        EXPECT_TRUE(isSame);
        isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.notes));
        
        EXPECT_EQ('c', std::get<1>(Class::class_t<InfoType>::notes));
        Reflect<InfoType>::Supers::at(0, [&](auto superInfo) {
            EXPECT_EQ(size_t(0), superInfo.index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::type;
            using InfoAnnotations = RareTs::remove_cvref_t<typename Info::Notes>;
            bool isSame = std::is_same_v<InheritGrandparent, InfoType>;
            EXPECT_TRUE(isSame);
            isSame = std::is_same_v<std::tuple<char>, InfoAnnotations>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.notes));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);
}
