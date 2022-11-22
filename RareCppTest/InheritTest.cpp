#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
using namespace Reflection;

struct SerializedName
{
    std::string_view value;
};

template <typename T>
struct Specializable {
    T value;
};

NOTE(TestSuper)
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

TEST(ReflectInheritanceTest, SuperInfo)
{
    bool isEqual = std::is_same_v<TestSuper, SuperInfo<0, SingleSuper>::Type>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<TestOtherSuper, SuperInfo<1, DoubleSuper>::Type>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<NoAnnotation, SuperInfo<0, SingleSuper>::Annotations>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<NoAnnotation, SuperInfo<1, DoubleSuper>::Annotations>;
    EXPECT_TRUE(isEqual);
    isEqual = 0 == SuperInfo<0, DoubleSuper>::Index;
    EXPECT_TRUE(isEqual);
    isEqual = 1 == SuperInfo<1, DoubleSuper>::Index;
    EXPECT_TRUE(isEqual);
    
    bool hasAnnotation = SuperInfo<0, SingleSuper>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<0, SingleSuper>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<0, SingleSuperNote>::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = SuperInfo<0, SingleSuperOtherNote>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<0, DoubleSuperNotes>::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = SuperInfo<0, DoubleSuperNotes>::template HasAnnotation<TestOtherAnnotation>;
    EXPECT_TRUE(hasAnnotation);

    NoAnnotation noNote{};
    SuperInfo<0, SingleSuper> testSuperNoAnnotations{};

    TestAnnotation testAnnotation { 33 };
    SuperInfo<0, SingleSuperNote> annotated{};
    using Annotated = decltype(annotated);
    hasAnnotation = Annotated::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);

    EXPECT_EQ(33, annotated.getAnnotation<TestAnnotation>().value);

    int timesVisited = 0;
    annotated.forEach<TestAnnotation>([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    annotated.forEachAnnotation([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    TestOtherAnnotation testOtherAnnotation { 44 };
    TestOtherAnnotation testOtherAnnotation55 { 55 };
    SuperInfo<0, DoubleSuperNotes> otherAnnotated{};
    using OtherAnnotated = decltype(otherAnnotated);
    hasAnnotation = OtherAnnotated::template HasAnnotation<ThirdAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = OtherAnnotated::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = OtherAnnotated::template HasAnnotation<TestOtherAnnotation>;
    EXPECT_TRUE(hasAnnotation);

    auto otherAnnotatedTestAnnotation = otherAnnotated.getAnnotation<TestAnnotation>();
    EXPECT_EQ(33, otherAnnotatedTestAnnotation.value);
    auto otherAnnotatedTestOtherAnnotation = otherAnnotated.getAnnotation<TestOtherAnnotation>();
    EXPECT_EQ(44, otherAnnotatedTestOtherAnnotation.otherValue);

    timesVisited = 0;
    otherAnnotated.forEach<TestAnnotation>([&](auto & annotation) {
        EXPECT_EQ(33, annotation.value);
        timesVisited ++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEach<TestOtherAnnotation>([&](auto & annotation) {
        if ( timesVisited == 0 )
            EXPECT_EQ(44, annotation.otherValue);
        else if ( timesVisited == 1 )
            EXPECT_EQ(55, annotation.otherValue);

        timesVisited++;
    });
    EXPECT_EQ(2, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEach<ThirdAnnotation>([&](auto & annotation) {
        timesVisited ++;
    });
    EXPECT_EQ(0, timesVisited);

    timesVisited = 0;
    otherAnnotated.forEachAnnotation([&](auto & annotation) {
        using AnnotationType = typename std::remove_const<typename std::remove_reference<decltype(annotation)>::type>::type;
        constexpr bool isTestAnnotation = std::is_same_v<TestAnnotation, AnnotationType>;
        constexpr bool isTestOtherAnnotation = std::is_same_v<TestOtherAnnotation, AnnotationType>;
        EXPECT_TRUE(isTestAnnotation || isTestOtherAnnotation);
        if constexpr ( isTestAnnotation )
        {
            EXPECT_EQ(33, annotation.value);
        }
        else if constexpr ( isTestOtherAnnotation )
        {
            if ( timesVisited == 1 )
                EXPECT_EQ(44, annotation.otherValue);
            else if ( timesVisited == 2 )
                EXPECT_EQ(55, annotation.otherValue);
        }
        timesVisited ++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    using SpecializedNotes = SuperInfo<0, SpecializedSuperAnnotation>;
    SpecializedNotes::forEach<Specializable>([&](auto & annotation) {
        EXPECT_EQ(timesVisited, annotation.value);
        timesVisited++;
    });
    EXPECT_EQ(2, timesVisited);
}

NOTE(BaseClass, Super<TestSuper>, Super<TestOtherSuper>(SerializedName{"testOther"}, 'a'), 'b')
struct BaseClass : TestSuper, TestOtherSuper {};

TEST(ReflectInheritanceTest, SuperAnnotation)
{
    EXPECT_TRUE(is_super<typename std::remove_reference<decltype(std::get<0>(BaseClass_note))>::type>::value);
    EXPECT_TRUE(is_super<typename std::remove_reference<decltype(std::get<1>(BaseClass_note))>::type>::value);
    EXPECT_FALSE(is_super<typename std::remove_reference<decltype(std::get<2>(BaseClass_note))>::type>::value);

    auto & firstSuper = std::get<0>(BaseClass_note);
    auto & secondSuper = std::get<1>(BaseClass_note);
    using FirstSuperWrapper = typename std::remove_reference<decltype(firstSuper)>::type;
    using SecondSuperWrapper = typename std::remove_reference<decltype(secondSuper)>::type;
    using FirstSuper = typename super_type<FirstSuperWrapper>::type;
    using SecondSuper = typename super_type<SecondSuperWrapper>::type;
    
    bool isSame = std::is_same<TestSuper, FirstSuper>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<TestOtherSuper, SecondSuper>::value;
    EXPECT_TRUE(isSame);

    isSame = std::is_same<NoAnnotation, typename std::remove_const<FirstSuperWrapper::Annotations>::type>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<NoAnnotation, typename std::remove_const<decltype(FirstSuperWrapper::annotations)>::type>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::tuple_size<decltype(FirstSuperWrapper::annotations)>::value);

    isSame = std::is_same<std::tuple<SerializedName, char>, typename std::remove_const<SecondSuperWrapper::Annotations>::type>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<std::tuple<SerializedName, char>, typename std::remove_const<decltype(SecondSuperWrapper::annotations)>::type>::value;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(2, std::tuple_size<decltype(SecondSuperWrapper::annotations)>::value);
    EXPECT_STREQ("testOther", std::string(std::get<0>(secondSuper.annotations).value).c_str());
    EXPECT_EQ('a', std::get<1>(secondSuper.annotations));
    EXPECT_EQ('b', std::get<2>(BaseClass_note));
}

TEST(ReflectInheritanceTest, CountSupers)
{
    size_t superCount = count_supers<>::value;
    EXPECT_EQ(0, superCount);
    superCount = count_supers<int>::value;
    EXPECT_EQ(0, superCount);
    superCount = count_supers<int, int>::value;
    EXPECT_EQ(0, superCount);
    superCount = count_supers<int, int, int>::value;
    EXPECT_EQ(0, superCount);
    
    superCount = count_supers<decltype(Super<TestSuper>)>::value;
    EXPECT_EQ(1, superCount);
    superCount = count_supers<int, decltype(Super<TestSuper>)>::value;
    EXPECT_EQ(1, superCount);
    superCount = count_supers<decltype(Super<TestSuper>), int>::value;
    EXPECT_EQ(1, superCount);
    
    superCount = count_supers<decltype(Super<TestSuper>), decltype(Super<TestOtherSuper>)>::value;
    EXPECT_EQ(2, superCount);
    superCount = count_supers<decltype(Super<TestSuper>), int, decltype(Super<TestOtherSuper>)>::value;
    EXPECT_EQ(2, superCount);
    superCount = count_supers<int, int, decltype(Super<TestSuper>), decltype(Super<TestOtherSuper>)>::value;
    EXPECT_EQ(2, superCount);
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

using InheritSuper_0a = Inherit<InheritSuper_S0a, Reflection::NoAnnotation>;
using InheritSuper_0b = Inherit<InheritSuper_S0b, Reflection::NoAnnotation>;
using InheritSuper_0c = Inherit<InheritSuper_S0c, Reflection::NoAnnotation>;

using InheritSuper_1a = Inherit<InheritSuper_S1a, decltype(InheritSuper_S1a_note)>;
using InheritSuper_1b = Inherit<InheritSuper_S1b, decltype(InheritSuper_S1b_note)>;
using InheritSuper_1c = Inherit<InheritSuper_S1c, decltype(InheritSuper_S1c_note)>;
using InheritSuper_1d = Inherit<InheritSuper_S1d, decltype(InheritSuper_S1d_note)>;

using InheritSuper_2a = Inherit<InheritSuper_S2a, decltype(InheritSuper_S2a_note)>;
using InheritSuper_2b = Inherit<InheritSuper_S2b, decltype(InheritSuper_S2b_note)>;
using InheritSuper_2c = Inherit<InheritSuper_S2c, decltype(InheritSuper_S2c_note)>;
using InheritSuper_2d = Inherit<InheritSuper_S2d, decltype(InheritSuper_S2d_note)>;

using InheritSuper_3a = Inherit<InheritSuper_S3a, decltype(InheritSuper_S3a_note)>;
using InheritSuper_3b = Inherit<InheritSuper_S3b, decltype(InheritSuper_S3b_note)>;
using InheritSuper_3c = Inherit<InheritSuper_S3c, decltype(InheritSuper_S3c_note)>;
using InheritSuper_3d = Inherit<InheritSuper_S3d, decltype(InheritSuper_S3d_note)>;

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
    EXPECT_EQ(0, InheritSuper_0a::Total);
    EXPECT_EQ(0, InheritSuper_0b::Total);
    EXPECT_EQ(0, InheritSuper_0c::Total);
    
    EXPECT_EQ(1, InheritSuper_1a::Total);
    EXPECT_EQ(1, InheritSuper_1b::Total);
    EXPECT_EQ(1, InheritSuper_1c::Total);
    EXPECT_EQ(1, InheritSuper_1d::Total);
    
    EXPECT_EQ(2, InheritSuper_2a::Total);
    EXPECT_EQ(2, InheritSuper_2b::Total);
    EXPECT_EQ(2, InheritSuper_2c::Total);
    EXPECT_EQ(2, InheritSuper_2d::Total);
    
    EXPECT_EQ(3, InheritSuper_3a::Total);
    EXPECT_EQ(3, InheritSuper_3b::Total);
    EXPECT_EQ(3, InheritSuper_3c::Total);
    EXPECT_EQ(3, InheritSuper_3d::Total);
}

TEST(ReflectInheritanceTest, InheritForEachStatic)
{
    int visitCount = 0;
    InheritSuper_0a::ForEach([&](auto superInfo) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::ForEach([&](auto superInfo) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::ForEach([&](auto superInfo) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::ForEach([&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::ForEach([&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.annotations));
        EXPECT_EQ('d', std::get<1>(superInfo.annotations));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::ForEach([&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::ForEach([&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::ForEach([&](auto superInfo) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::ForEach([&](auto superInfo) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('e', std::get<0>(superInfo.annotations));
            EXPECT_EQ('f', std::get<1>(superInfo.annotations));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::ForEach([&](auto superInfo) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::ForEach([&](auto superInfo) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('g', std::get<0>(superInfo.annotations));
            EXPECT_EQ('h', std::get<1>(superInfo.annotations));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::ForEach([&](auto superInfo) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::ForEach([&](auto superInfo) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('i', std::get<0>(superInfo.annotations));
            EXPECT_EQ('j', std::get<1>(superInfo.annotations));
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::ForEach([&](auto superInfo) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::ForEach([&](auto superInfo) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('k', std::get<0>(superInfo.annotations));
            EXPECT_EQ('l', std::get<1>(superInfo.annotations));
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritForEachInstance)
{
    int visitCount = 0;
    InheritSuper_0a::ForEach(inheritSuper_S0a, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::ForEach(inheritSuper_S0b, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::ForEach(inheritSuper_S0c, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::ForEach(inheritSuper_S1a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::ForEach(inheritSuper_S1b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.annotations));
        EXPECT_EQ('d', std::get<1>(superInfo.annotations));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::ForEach(inheritSuper_S1c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::ForEach(inheritSuper_S1d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::ForEach(inheritSuper_S2a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::ForEach(inheritSuper_S2b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('e', std::get<0>(superInfo.annotations));
            EXPECT_EQ('f', std::get<1>(superInfo.annotations));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::ForEach(inheritSuper_S2c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::ForEach(inheritSuper_S2d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            std::cout << "|||" << ExtendedTypeSupport::TypeToStr<decltype(superInfo.annotations)>() << std::endl;
            EXPECT_EQ('g', std::get<0>(superInfo.annotations));
            EXPECT_EQ('h', std::get<1>(superInfo.annotations));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::ForEach(inheritSuper_S3a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::ForEach(inheritSuper_S3b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('i', std::get<0>(superInfo.annotations));
            EXPECT_EQ('j', std::get<1>(superInfo.annotations));
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::ForEach(inheritSuper_S3c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::ForEach(inheritSuper_S3d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('k', std::get<0>(superInfo.annotations));
            EXPECT_EQ('l', std::get<1>(superInfo.annotations));
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritForEachInstanceConst)
{
    int visitCount = 0;
    InheritSuper_0a::ForEach((const decltype(inheritSuper_S0a) &)inheritSuper_S0a, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::ForEach((const decltype(inheritSuper_S0b) &)inheritSuper_S0b, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::ForEach((const decltype(inheritSuper_S0c) &)inheritSuper_S0c, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::ForEach((const decltype(inheritSuper_S1a) &)inheritSuper_S1a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::ForEach((const decltype(inheritSuper_S1b) &)inheritSuper_S1b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.annotations));
        EXPECT_EQ('d', std::get<1>(superInfo.annotations));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::ForEach((const decltype(inheritSuper_S1c) &)inheritSuper_S1c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::ForEach((const decltype(inheritSuper_S1d) &)inheritSuper_S1d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::ForEach((const decltype(inheritSuper_S2a) &)inheritSuper_S2a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::ForEach((const decltype(inheritSuper_S2b) &)inheritSuper_S2b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('e', std::get<0>(superInfo.annotations));
            EXPECT_EQ('f', std::get<1>(superInfo.annotations));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::ForEach((const decltype(inheritSuper_S2c) &)inheritSuper_S2c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::ForEach((const decltype(inheritSuper_S2d) &)inheritSuper_S2d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('g', std::get<0>(superInfo.annotations));
            EXPECT_EQ('h', std::get<1>(superInfo.annotations));
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::ForEach((const decltype(inheritSuper_S3a) &)inheritSuper_S3a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::ForEach((const decltype(inheritSuper_S3b) &)inheritSuper_S3b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('i', std::get<0>(superInfo.annotations));
            EXPECT_EQ('j', std::get<1>(superInfo.annotations));
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::ForEach((const decltype(inheritSuper_S3c) &)inheritSuper_S3c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::ForEach((const decltype(inheritSuper_S3d) &)inheritSuper_S3d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(visitCount, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        if constexpr ( superInfo.Index == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S1, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S2, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('k', std::get<0>(superInfo.annotations));
            EXPECT_EQ('l', std::get<1>(superInfo.annotations));
            visitCount++;
        }
        else if constexpr ( superInfo.Index == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritSuper_S3, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritForEachSuper)
{
    int visitCount = 0;
    InheritSuper_0a::ForEachSuper(inheritSuper_S0a, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::ForEachSuper(inheritSuper_S0b, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::ForEachSuper(inheritSuper_S0c, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::ForEachSuper(inheritSuper_S1a, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::ForEachSuper(inheritSuper_S1b, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::ForEachSuper(inheritSuper_S1c, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::ForEachSuper(inheritSuper_S1d, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::ForEachSuper(inheritSuper_S2a, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::ForEachSuper(inheritSuper_S2b, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::ForEachSuper(inheritSuper_S2c, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::ForEachSuper(inheritSuper_S2d, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::ForEachSuper(inheritSuper_S3a, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::ForEachSuper(inheritSuper_S3b, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::ForEachSuper(inheritSuper_S3c, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::ForEachSuper(inheritSuper_S3d, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritForEachSuperConst)
{
    int visitCount = 0;
    InheritSuper_0a::ForEachSuper((const decltype(inheritSuper_S0a) &)inheritSuper_S0a, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::ForEachSuper((const decltype(inheritSuper_S0b) &)inheritSuper_S0b, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::ForEachSuper((const decltype(inheritSuper_S0c) &)inheritSuper_S0c, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::ForEachSuper((const decltype(inheritSuper_S1a) &)inheritSuper_S1a, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::ForEachSuper((const decltype(inheritSuper_S1b) &)inheritSuper_S1b, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::ForEachSuper((const decltype(inheritSuper_S1c) &)inheritSuper_S1c, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::ForEachSuper((const decltype(inheritSuper_S1d) &)inheritSuper_S1d, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    InheritSuper_2a::ForEachSuper((const decltype(inheritSuper_S2a) &)inheritSuper_S2a, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2b::ForEachSuper((const decltype(inheritSuper_S2b) &)inheritSuper_S2b, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2c::ForEachSuper((const decltype(inheritSuper_S2c) &)inheritSuper_S2c, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    InheritSuper_2d::ForEachSuper((const decltype(inheritSuper_S2d) &)inheritSuper_S2d, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    InheritSuper_3a::ForEachSuper((const decltype(inheritSuper_S3a) &)inheritSuper_S3a, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3b::ForEachSuper((const decltype(inheritSuper_S3b) &)inheritSuper_S3b, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3c::ForEachSuper((const decltype(inheritSuper_S3c) &)inheritSuper_S3c, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    InheritSuper_3d::ForEachSuper((const decltype(inheritSuper_S3d) &)inheritSuper_S3d, [&](auto & super) {
        if ( visitCount == 0 )
        {
            bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 1 )
        {
            bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
        else if ( visitCount == 2 )
        {
            bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            visitCount++;
        }
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ReflectInheritanceTest, InheritAtStatic)
{
    int visitCount = 0;
    InheritSuper_0a::At(0, [&](auto superInfo) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::At(0, [&](auto superInfo) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::At(0, [&](auto superInfo) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::At(0, [&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::At(0, [&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.annotations));
        EXPECT_EQ('d', std::get<1>(superInfo.annotations));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::At(0, [&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::At(0, [&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2a::Total; i++ )
    {
        InheritSuper_2a::At(i, [&](auto superInfo) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2b::Total; i++ )
    {
        InheritSuper_2b::At(i, [&](auto superInfo) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('e', std::get<0>(superInfo.annotations));
                EXPECT_EQ('f', std::get<1>(superInfo.annotations));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2c::Total; i++ )
    {
        InheritSuper_2c::At(i, [&](auto superInfo) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2d::Total; i++ )
    {
        InheritSuper_2d::At(i, [&](auto superInfo) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('g', std::get<0>(superInfo.annotations));
                EXPECT_EQ('h', std::get<1>(superInfo.annotations));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3a::Total; i++ )
    {
        InheritSuper_3a::At(i, [&](auto superInfo) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3b::Total; i++ )
    {
        InheritSuper_3b::At(i, [&](auto superInfo) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('i', std::get<0>(superInfo.annotations));
                EXPECT_EQ('j', std::get<1>(superInfo.annotations));
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3c::Total; i++ )
    {
        InheritSuper_3c::At(i, [&](auto superInfo) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3d::Total; i++ )
    {
        InheritSuper_3d::At(i, [&](auto superInfo) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('k', std::get<0>(superInfo.annotations));
                EXPECT_EQ('l', std::get<1>(superInfo.annotations));
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
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
    InheritSuper_0a::At(0, inheritSuper_S0a, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::At(0, inheritSuper_S0b, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::At(0, inheritSuper_S0c, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::At(0, inheritSuper_S1a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::At(0, inheritSuper_S1b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.annotations));
        EXPECT_EQ('d', std::get<1>(superInfo.annotations));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::At(0, inheritSuper_S1c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::At(0, inheritSuper_S1d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2a::Total; i++ )
    {
        InheritSuper_2a::At(i, inheritSuper_S2a, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2b::Total; i++ )
    {
        InheritSuper_2b::At(i, inheritSuper_S2b, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('e', std::get<0>(superInfo.annotations));
                EXPECT_EQ('f', std::get<1>(superInfo.annotations));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2c::Total; i++ )
    {
        InheritSuper_2c::At(i, inheritSuper_S2c, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2d::Total; i++ )
    {
        InheritSuper_2d::At(i, inheritSuper_S2d, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('g', std::get<0>(superInfo.annotations));
                EXPECT_EQ('h', std::get<1>(superInfo.annotations));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3a::Total; i++ )
    {
        InheritSuper_3a::At(i, inheritSuper_S3a, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3b::Total; i++ )
    {
        InheritSuper_3b::At(i, inheritSuper_S3b, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('i', std::get<0>(superInfo.annotations));
                EXPECT_EQ('j', std::get<1>(superInfo.annotations));
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3c::Total; i++ )
    {
        InheritSuper_3c::At(i, inheritSuper_S3c, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3d::Total; i++ )
    {
        InheritSuper_3d::At(i, inheritSuper_S3d, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('k', std::get<0>(superInfo.annotations));
                EXPECT_EQ('l', std::get<1>(superInfo.annotations));
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
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
    InheritSuper_0a::At(0, (const decltype(inheritSuper_S0a) &)inheritSuper_S0a, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::At(0, (const decltype(inheritSuper_S0b) &)inheritSuper_S0b, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::At(0, (const decltype(inheritSuper_S0c) &)inheritSuper_S0c, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::At(0, (const decltype(inheritSuper_S1a) &)inheritSuper_S1a, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::At(0, (const decltype(inheritSuper_S1b) &)inheritSuper_S1b, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('c', std::get<0>(superInfo.annotations));
        EXPECT_EQ('d', std::get<1>(superInfo.annotations));
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::At(0, (const decltype(inheritSuper_S1c) &)inheritSuper_S1c, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::At(0, (const decltype(inheritSuper_S1d) &)inheritSuper_S1d, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritSuper_S1, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2a::Total; i++ )
    {
        InheritSuper_2a::At(i, (const decltype(inheritSuper_S2a) &)inheritSuper_S2a, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2b::Total; i++ )
    {
        InheritSuper_2b::At(i, (const decltype(inheritSuper_S2b) &)inheritSuper_S2b, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('e', std::get<0>(superInfo.annotations));
                EXPECT_EQ('f', std::get<1>(superInfo.annotations));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2c::Total; i++ )
    {
        InheritSuper_2c::At(i, (const decltype(inheritSuper_S2c) &)inheritSuper_S2c, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2d::Total; i++ )
    {
        InheritSuper_2d::At(i, (const decltype(inheritSuper_S2d) &)inheritSuper_S2d, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('g', std::get<0>(superInfo.annotations));
                EXPECT_EQ('h', std::get<1>(superInfo.annotations));
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3a::Total; i++ )
    {
        InheritSuper_3a::At(i, (const decltype(inheritSuper_S3a) &)inheritSuper_S3a, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3b::Total; i++ )
    {
        InheritSuper_3b::At(i, (const decltype(inheritSuper_S3b) &)inheritSuper_S3b, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('i', std::get<0>(superInfo.annotations));
                EXPECT_EQ('j', std::get<1>(superInfo.annotations));
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3c::Total; i++ )
    {
        InheritSuper_3c::At(i, (const decltype(inheritSuper_S3c) &)inheritSuper_S3c, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3d::Total; i++ )
    {
        InheritSuper_3d::At(i, (const decltype(inheritSuper_S3d) &)inheritSuper_S3d, [&](auto superInfo, auto & super) {
            EXPECT_EQ(visitCount, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            if constexpr ( superInfo.Index == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S1, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S2, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<char, char>, InfoAnnotations>::value;
                EXPECT_TRUE(isSame);
                EXPECT_EQ('k', std::get<0>(superInfo.annotations));
                EXPECT_EQ('l', std::get<1>(superInfo.annotations));
                visitCount++;
            }
            else if constexpr ( superInfo.Index == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<InheritSuper_S3, InfoType>::value;
                EXPECT_TRUE(isSame);
                isSame = std::is_same<std::tuple<>, InfoAnnotations>::value;
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
    InheritSuper_0a::SuperAt(0, inheritSuper_S0a, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::SuperAt(0, inheritSuper_S0b, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::SuperAt(0, inheritSuper_S0c, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::SuperAt(0, inheritSuper_S1a, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::SuperAt(0, inheritSuper_S1b, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::SuperAt(0, inheritSuper_S1c, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::SuperAt(0, inheritSuper_S1d, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2a::Total; i++ )
    {
        InheritSuper_2a::SuperAt(i, inheritSuper_S2a, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2b::Total; i++ )
    {
        InheritSuper_2b::SuperAt(i, inheritSuper_S2b, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2c::Total; i++ )
    {
        InheritSuper_2c::SuperAt(i, inheritSuper_S2c, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2d::Total; i++ )
    {
        InheritSuper_2d::SuperAt(i, inheritSuper_S2d, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3a::Total; i++ )
    {
        InheritSuper_3a::SuperAt(i, inheritSuper_S3a, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3b::Total; i++ )
    {
        InheritSuper_3b::SuperAt(i, inheritSuper_S3b, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3c::Total; i++ )
    {
        InheritSuper_3c::SuperAt(i, inheritSuper_S3c, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3d::Total; i++ )
    {
        InheritSuper_3d::SuperAt(i, inheritSuper_S3d, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
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
    InheritSuper_0a::SuperAt(0, (const decltype(inheritSuper_S0a) &)inheritSuper_S0a, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::SuperAt(0, (const decltype(inheritSuper_S0b) &)inheritSuper_S0b, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::SuperAt(0, (const decltype(inheritSuper_S0c) &)inheritSuper_S0c, [&](auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::SuperAt(0, (const decltype(inheritSuper_S1a) &)inheritSuper_S1a, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1b::SuperAt(0, (const decltype(inheritSuper_S1b) &)inheritSuper_S1b, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1c::SuperAt(0, (const decltype(inheritSuper_S1c) &)inheritSuper_S1c, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    visitCount = 0;
    InheritSuper_1d::SuperAt(0, (const decltype(inheritSuper_S1d) &)inheritSuper_S1d, [&](auto & super) {
        bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2a::Total; i++ )
    {
        InheritSuper_2a::SuperAt(i, (const decltype(inheritSuper_S2a) &)inheritSuper_S2a, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2b::Total; i++ )
    {
        InheritSuper_2b::SuperAt(i, (const decltype(inheritSuper_S2b) &)inheritSuper_S2b, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2c::Total; i++ )
    {
        InheritSuper_2c::SuperAt(i, (const decltype(inheritSuper_S2c) &)inheritSuper_S2c, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_2d::Total; i++ )
    {
        InheritSuper_2d::SuperAt(i, (const decltype(inheritSuper_S2d) &)inheritSuper_S2d, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3a::Total; i++ )
    {
        InheritSuper_3a::SuperAt(i, (const decltype(inheritSuper_S3a) &)inheritSuper_S3a, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3b::Total; i++ )
    {
        InheritSuper_3b::SuperAt(i, (const decltype(inheritSuper_S3b) &)inheritSuper_S3b, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3c::Total; i++ )
    {
        InheritSuper_3c::SuperAt(i, (const decltype(inheritSuper_S3c) &)inheritSuper_S3c, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
        });
    }
    EXPECT_EQ(3, visitCount);
    visitCount = 0;
    for ( int i=0; i<InheritSuper_3d::Total; i++ )
    {
        InheritSuper_3d::SuperAt(i, (const decltype(inheritSuper_S3d) &)inheritSuper_S3d, [&](auto & super) {
            if ( visitCount == 0 )
            {
                bool isSame = std::is_same<InheritSuper_S1, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 1 )
            {
                bool isSame = std::is_same<InheritSuper_S2, typename std::remove_reference<decltype(super)>::type>::value;
                EXPECT_TRUE(isSame);
                visitCount++;
            }
            else if ( visitCount == 2 )
            {
                bool isSame = std::is_same<InheritSuper_S3, typename std::remove_reference<decltype(super)>::type>::value;
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
    EXPECT_EQ(1, Reflect<InheritChild>::Supers::Total);
    InheritChild child = {};
    EXPECT_EQ('e', std::get<1>(InheritChild::Class::annotations));

    // Inherit for each instance
    int outerVisitCount = 0;
    int innerVisitCount = 0;
    Reflect<InheritChild>::Supers::ForEach(child, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritParent, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritParent, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.annotations));
        
        EXPECT_EQ('c', std::get<1>(InfoType::Class::annotations));
        Reflect<InfoType>::Supers::ForEach(super, [&](auto superInfo, auto & super) {
            EXPECT_EQ(0, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            bool isSame = std::is_same<InheritGrandparent, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritGrandparent, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.annotations));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);
    
    // Inherit for each instance const
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::ForEach((const decltype(child) &)child, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritParent, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritParent, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.annotations));
        
        EXPECT_EQ('c', std::get<1>(InfoType::Class::annotations));
        Reflect<InfoType>::Supers::ForEach((const decltype(super) &)super, [&](auto superInfo, auto & super) {
            EXPECT_EQ(0, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            bool isSame = std::is_same<InheritGrandparent, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritGrandparent, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.annotations));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);

    // Inherit for each static
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::ForEach([&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritParent, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.annotations));
        
        EXPECT_EQ('c', std::get<1>(InfoType::Class::annotations));
        Reflect<InfoType>::Supers::ForEach([&](auto superInfo) {
            EXPECT_EQ(0, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            bool isSame = std::is_same<InheritGrandparent, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.annotations));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);

    // Inherit at instance
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::At(0, child, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritParent, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritParent, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.annotations));
        
        EXPECT_EQ('c', std::get<1>(InfoType::Class::annotations));
        Reflect<InfoType>::Supers::At(0, super, [&](auto superInfo, auto & super) {
            EXPECT_EQ(0, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            bool isSame = std::is_same<InheritGrandparent, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritGrandparent, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.annotations));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);

    // Inherit at instance const
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::At(0, (const decltype(child) &)child, [&](auto superInfo, auto & super) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritParent, typename std::remove_reference<decltype(super)>::type>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<InheritParent, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.annotations));
        
        EXPECT_EQ('c', std::get<1>(InfoType::Class::annotations));
        Reflect<InfoType>::Supers::At(0, (const decltype(super) &)super, [&](auto superInfo, auto & super) {
            EXPECT_EQ(0, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            bool isSame = std::is_same<InheritGrandparent, typename std::remove_reference<decltype(super)>::type>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<InheritGrandparent, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.annotations));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);

    // Inherit at static
    outerVisitCount = 0;
    innerVisitCount = 0;
    Reflect<InheritChild>::Supers::At(0, [&](auto superInfo) {
        EXPECT_EQ(0, superInfo.Index);
        using Info = decltype(superInfo);
        using InfoType = typename Info::Type;
        using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
        bool isSame = std::is_same<InheritParent, InfoType>::value;
        EXPECT_TRUE(isSame);
        isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
        EXPECT_TRUE(isSame);
        EXPECT_EQ('d', std::get<0>(superInfo.annotations));
        
        EXPECT_EQ('c', std::get<1>(InfoType::Class::annotations));
        Reflect<InfoType>::Supers::At(0, [&](auto superInfo) {
            EXPECT_EQ(0, superInfo.Index);
            using Info = decltype(superInfo);
            using InfoType = typename Info::Type;
            using InfoAnnotations = typename std::remove_reference<typename std::remove_const<typename Info::Annotations>::type>::type;
            bool isSame = std::is_same<InheritGrandparent, InfoType>::value;
            EXPECT_TRUE(isSame);
            isSame = std::is_same<std::tuple<char>, InfoAnnotations>::value;
            EXPECT_TRUE(isSame);
            EXPECT_EQ('b', std::get<0>(superInfo.annotations));
            innerVisitCount++;
        });

        outerVisitCount++;
    });
    EXPECT_EQ(1, outerVisitCount);
    EXPECT_EQ(1, innerVisitCount);
}
