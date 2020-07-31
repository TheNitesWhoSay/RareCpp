#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
using namespace Reflect;

struct SerializedName
{
    std::string_view value;
};

NOTE(TestSuper)
struct TestSuper {};
struct TestOtherSuper {};
struct TestAnnotation { int value; };
struct TestOtherAnnotation { int otherValue; };
struct ThirdAnnotation {};

NOTE(SingleSuper, Super<TestSuper>)
struct SingleSuper : TestSuper {};

NOTE(DoubleSuper, Super<TestSuper>, Super<TestOtherSuper>)
struct DoubleSuper : TestSuper, TestOtherSuper {};

TEST(ReflectInheritanceTest, SuperInfo)
{
    constexpr size_t superIndex = 5;
    constexpr size_t otherSuperIndex = 6;

    bool isEqual = std::is_same<TestSuper, SuperInfo<TestSuper, superIndex, NoAnnotation>::Type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<TestOtherSuper, SuperInfo<TestOtherSuper, otherSuperIndex, NoAnnotation>::Type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<NoAnnotation, SuperInfo<TestSuper, superIndex, NoAnnotation>::Annotations>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<NoAnnotation, SuperInfo<TestOtherSuper, otherSuperIndex, NoAnnotation>::Annotations>::value;
    EXPECT_TRUE(isEqual);
    isEqual = superIndex == SuperInfo<TestSuper, superIndex, NoAnnotation>::Index;
    EXPECT_TRUE(isEqual);
    isEqual = otherSuperIndex == SuperInfo<TestOtherSuper, otherSuperIndex, NoAnnotation>::Index;
    EXPECT_TRUE(isEqual);
    
    bool hasAnnotation = SuperInfo<TestSuper, superIndex, NoAnnotation>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<>>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<TestAnnotation>>::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<TestOtherAnnotation>>::template HasAnnotation<TestAnnotation>;
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<TestAnnotation, TestOtherAnnotation>>::template HasAnnotation<TestAnnotation>;
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = SuperInfo<TestSuper, superIndex, std::tuple<TestAnnotation, TestOtherAnnotation>>::template HasAnnotation<TestOtherAnnotation>;
    EXPECT_TRUE(hasAnnotation);

    NoAnnotation noNote{};
    SuperInfo<TestSuper, superIndex, NoAnnotation> testSuperNoAnnotations{noNote};

    TestAnnotation testAnnotation { 33 };
    SuperInfo<TestSuper, superIndex, std::tuple<TestAnnotation>> annotated{testAnnotation};
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
    SuperInfo<TestOtherSuper, otherSuperIndex, std::tuple<TestAnnotation, TestOtherAnnotation, TestOtherAnnotation>> otherAnnotated
        {std::tuple {testAnnotation, testOtherAnnotation, testOtherAnnotation55} };
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
NOTE(InheritSuper_S1)
struct InheritSuper_S1 { REFLECT_EMPTY(InheritSuper_S1) };
NOTE(InheritSuper_S2)
struct InheritSuper_S2 { REFLECT_EMPTY(InheritSuper_S2) };
NOTE(InheritSuper_S3)
struct InheritSuper_S3 { REFLECT_EMPTY(InheritSuper_S3) };

// Child classes
NOTE(InheritSuper_S0a, 0)
struct InheritSuper_S0a { REFLECT_EMPTY(InheritSuper_S0a) };
NOTE(InheritSuper_S0b, 0)
struct InheritSuper_S0b { REFLECT_EMPTY(InheritSuper_S0b) };
NOTE(InheritSuper_S0c, 0, 0)
struct InheritSuper_S0c { REFLECT_EMPTY(InheritSuper_S0c) };

NOTE(InheritSuper_S1a, Super<InheritSuper_S1>)
struct InheritSuper_S1a { REFLECT_EMPTY(InheritSuper_S1a) };
NOTE(InheritSuper_S1b, Super<InheritSuper_S1>('c', 'd'))
struct InheritSuper_S1b { REFLECT_EMPTY(InheritSuper_S1b) };
NOTE(InheritSuper_S1c, 0, Super<InheritSuper_S1>)
struct InheritSuper_S1c { REFLECT_EMPTY(InheritSuper_S1c) };
NOTE(InheritSuper_S1d, 0, Super<InheritSuper_S1>, 0, 0)
struct InheritSuper_S1d { REFLECT_EMPTY(InheritSuper_S1d) };

NOTE(InheritSuper_S2a, Super<InheritSuper_S1>, Super<InheritSuper_S2>)
struct InheritSuper_S2a { REFLECT_EMPTY(InheritSuper_S2a) };
NOTE(InheritSuper_S2b, Super<InheritSuper_S1>, Super<InheritSuper_S2>('e', 'f'))
struct InheritSuper_S2b { REFLECT_EMPTY(InheritSuper_S2b) };
NOTE(InheritSuper_S2c, 0, Super<InheritSuper_S1>, Super<InheritSuper_S2>)
struct InheritSuper_S2c { REFLECT_EMPTY(InheritSuper_S2c) };
NOTE(InheritSuper_S2d, 0, Super<InheritSuper_S1>, Super<InheritSuper_S2>('g', 'h'), 0, 0)
struct InheritSuper_S2d { REFLECT_EMPTY(InheritSuper_S2d) };

NOTE(InheritSuper_S3a, Super<InheritSuper_S1>, Super<InheritSuper_S2>, Super<InheritSuper_S3>)
struct InheritSuper_S3a { REFLECT_EMPTY(InheritSuper_S3a) };
NOTE(InheritSuper_S3b, Super<InheritSuper_S1>, Super<InheritSuper_S2>('i', 'j'), Super<InheritSuper_S3>)
struct InheritSuper_S3b { REFLECT_EMPTY(InheritSuper_S3b) };
NOTE(InheritSuper_S3c, 0, Super<InheritSuper_S1>, Super<InheritSuper_S2>, Super<InheritSuper_S3>)
struct InheritSuper_S3c { REFLECT_EMPTY(InheritSuper_S3c) };
NOTE(InheritSuper_S3d, 0, 0, Super<InheritSuper_S1>, 0, Super<InheritSuper_S2>('k', 'l'), Super<InheritSuper_S3>)
struct InheritSuper_S3d { REFLECT_EMPTY(InheritSuper_S3d) };

using InheritSuper_0a = Inherit<InheritSuper_S0a, const decltype(InheritSuper_S0a_note)>;
using InheritSuper_0b = Inherit<InheritSuper_S0b, const decltype(InheritSuper_S0b_note)>;
using InheritSuper_0c = Inherit<InheritSuper_S0c, const decltype(InheritSuper_S0c_note)>;

using InheritSuper_1a = Inherit<InheritSuper_S1a, const decltype(InheritSuper_S1a_note)>;
using InheritSuper_1b = Inherit<InheritSuper_S1b, const decltype(InheritSuper_S1b_note)>;
using InheritSuper_1c = Inherit<InheritSuper_S1c, const decltype(InheritSuper_S1c_note)>;
using InheritSuper_1d = Inherit<InheritSuper_S1d, const decltype(InheritSuper_S1d_note)>;

using InheritSuper_2a = Inherit<InheritSuper_S2a, const decltype(InheritSuper_S2a_note)>;
using InheritSuper_2b = Inherit<InheritSuper_S2b, const decltype(InheritSuper_S2b_note)>;
using InheritSuper_2c = Inherit<InheritSuper_S2c, const decltype(InheritSuper_S2c_note)>;
using InheritSuper_2d = Inherit<InheritSuper_S2d, const decltype(InheritSuper_S2d_note)>;

using InheritSuper_3a = Inherit<InheritSuper_S3a, const decltype(InheritSuper_S3a_note)>;
using InheritSuper_3b = Inherit<InheritSuper_S3b, const decltype(InheritSuper_S3b_note)>;
using InheritSuper_3c = Inherit<InheritSuper_S3c, const decltype(InheritSuper_S3c_note)>;
using InheritSuper_3d = Inherit<InheritSuper_S3d, const decltype(InheritSuper_S3d_note)>;

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
    EXPECT_EQ(0, InheritSuper_0a::TotalSupers);
    EXPECT_EQ(0, InheritSuper_0b::TotalSupers);
    EXPECT_EQ(0, InheritSuper_0c::TotalSupers);
    
    EXPECT_EQ(1, InheritSuper_1a::TotalSupers);
    EXPECT_EQ(1, InheritSuper_1b::TotalSupers);
    EXPECT_EQ(1, InheritSuper_1c::TotalSupers);
    EXPECT_EQ(1, InheritSuper_1d::TotalSupers);
    
    EXPECT_EQ(2, InheritSuper_2a::TotalSupers);
    EXPECT_EQ(2, InheritSuper_2b::TotalSupers);
    EXPECT_EQ(2, InheritSuper_2c::TotalSupers);
    EXPECT_EQ(2, InheritSuper_2d::TotalSupers);
    
    EXPECT_EQ(3, InheritSuper_3a::TotalSupers);
    EXPECT_EQ(3, InheritSuper_3b::TotalSupers);
    EXPECT_EQ(3, InheritSuper_3c::TotalSupers);
    EXPECT_EQ(3, InheritSuper_3d::TotalSupers);
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

TEST(ReflectInheritanceTest, InheritAtInstance)
{
    int visitCount = 0;
    InheritSuper_0a::At(inheritSuper_S0a, 0, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::At(inheritSuper_S0b, 0, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::At(inheritSuper_S0c, 0, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::At(inheritSuper_S1a, 0, [&](auto superInfo, auto & super) {
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
    InheritSuper_1b::At(inheritSuper_S1b, 0, [&](auto superInfo, auto & super) {
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
    InheritSuper_1c::At(inheritSuper_S1c, 0, [&](auto superInfo, auto & super) {
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
    InheritSuper_1d::At(inheritSuper_S1d, 0, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2a::TotalSupers; i++ )
    {
        InheritSuper_2a::At(inheritSuper_S2a, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2b::TotalSupers; i++ )
    {
        InheritSuper_2b::At(inheritSuper_S2b, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2c::TotalSupers; i++ )
    {
        InheritSuper_2c::At(inheritSuper_S2c, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2d::TotalSupers; i++ )
    {
        InheritSuper_2d::At(inheritSuper_S2d, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_3a::TotalSupers; i++ )
    {
        InheritSuper_3a::At(inheritSuper_S3a, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_3b::TotalSupers; i++ )
    {
        InheritSuper_3b::At(inheritSuper_S3b, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_3c::TotalSupers; i++ )
    {
        InheritSuper_3c::At(inheritSuper_S3c, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_3d::TotalSupers; i++ )
    {
        InheritSuper_3d::At(inheritSuper_S3d, i, [&](auto superInfo, auto & super) {
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
    InheritSuper_0a::At((const decltype(inheritSuper_S0a) &)inheritSuper_S0a, 0, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0b::At((const decltype(inheritSuper_S0b) &)inheritSuper_S0b, 0, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    visitCount = 0;
    InheritSuper_0c::At((const decltype(inheritSuper_S0c) &)inheritSuper_S0c, 0, [&](auto superInfo, auto & super) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    InheritSuper_1a::At((const decltype(inheritSuper_S1a) &)inheritSuper_S1a, 0, [&](auto superInfo, auto & super) {
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
    InheritSuper_1b::At((const decltype(inheritSuper_S1b) &)inheritSuper_S1b, 0, [&](auto superInfo, auto & super) {
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
    InheritSuper_1c::At((const decltype(inheritSuper_S1c) &)inheritSuper_S1c, 0, [&](auto superInfo, auto & super) {
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
    InheritSuper_1d::At((const decltype(inheritSuper_S1d) &)inheritSuper_S1d, 0, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2a::TotalSupers; i++ )
    {
        InheritSuper_2a::At((const decltype(inheritSuper_S2a) &)inheritSuper_S2a, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2b::TotalSupers; i++ )
    {
        InheritSuper_2b::At((const decltype(inheritSuper_S2b) &)inheritSuper_S2b, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2c::TotalSupers; i++ )
    {
        InheritSuper_2c::At((const decltype(inheritSuper_S2c) &)inheritSuper_S2c, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2d::TotalSupers; i++ )
    {
        InheritSuper_2d::At((const decltype(inheritSuper_S2d) &)inheritSuper_S2d, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_3a::TotalSupers; i++ )
    {
        InheritSuper_3a::At((const decltype(inheritSuper_S3a) &)inheritSuper_S3a, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_3b::TotalSupers; i++ )
    {
        InheritSuper_3b::At((const decltype(inheritSuper_S3b) &)inheritSuper_S3b, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_3c::TotalSupers; i++ )
    {
        InheritSuper_3c::At((const decltype(inheritSuper_S3c) &)inheritSuper_S3c, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_3d::TotalSupers; i++ )
    {
        InheritSuper_3d::At((const decltype(inheritSuper_S3d) &)inheritSuper_S3d, i, [&](auto superInfo, auto & super) {
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
    for ( int i=0; i<InheritSuper_2a::TotalSupers; i++ )
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
    for ( int i=0; i<InheritSuper_2b::TotalSupers; i++ )
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
    for ( int i=0; i<InheritSuper_2c::TotalSupers; i++ )
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
    for ( int i=0; i<InheritSuper_2d::TotalSupers; i++ )
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
    for ( int i=0; i<InheritSuper_3a::TotalSupers; i++ )
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
    for ( int i=0; i<InheritSuper_3b::TotalSupers; i++ )
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
    for ( int i=0; i<InheritSuper_3c::TotalSupers; i++ )
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
    for ( int i=0; i<InheritSuper_3d::TotalSupers; i++ )
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

NOTE(Grandparent)
struct Grandparent { REFLECT_EMPTY(Grandparent) };
NOTE(Parent, Super<Grandparent>)
struct Parent : Grandparent { REFLECT_EMPTY(Parent) };
NOTE(Child, Super<Parent>)
struct Child : Parent { REFLECT_EMPTY(Child) };

NOTE(InheritGrandparent, 'a')
struct InheritGrandparent { REFLECT_EMPTY(InheritGrandparent) };
NOTE(InheritParent, Super<InheritGrandparent>('b'), 'c')
struct InheritParent { REFLECT_EMPTY(InheritParent) };
NOTE(InheritChild, Super<InheritParent>('d'), 'e')
struct InheritChild { REFLECT_EMPTY(InheritChild) };

TEST(ReflectInheritanceTest, InheritRecursion)
{
    EXPECT_EQ(1, InheritChild::Supers::TotalSupers);
    InheritChild child = {};
    EXPECT_EQ('e', std::get<1>(InheritChild::Class::annotations));

    // Inherit for each instance
    int outerVisitCount = 0;
    int innerVisitCount = 0;
    InheritChild::Supers::ForEach(child, [&](auto superInfo, auto & super) {
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
        InfoType::Supers::ForEach(super, [&](auto superInfo, auto & super) {
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
    InheritChild::Supers::ForEach((const decltype(child) &)child, [&](auto superInfo, auto & super) {
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
        InfoType::Supers::ForEach((const decltype(super) &)super, [&](auto superInfo, auto & super) {
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
    InheritChild::Supers::ForEach([&](auto superInfo) {
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
        InfoType::Supers::ForEach([&](auto superInfo) {
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
    InheritChild::Supers::At(child, 0, [&](auto superInfo, auto & super) {
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
        InfoType::Supers::At(super, 0, [&](auto superInfo, auto & super) {
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
    InheritChild::Supers::At((const decltype(child) &)child, 0, [&](auto superInfo, auto & super) {
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
        InfoType::Supers::At((const decltype(super) &)super, 0, [&](auto superInfo, auto & super) {
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
    InheritChild::Supers::At(0, [&](auto superInfo) {
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
        InfoType::Supers::At(0, [&](auto superInfo) {
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
