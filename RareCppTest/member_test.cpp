#include <rarecpp/reflect.h>
#include <gtest/gtest.h>

struct TestStruct
{
    int testVal;
    static int testStaticVal;

    REFLECT(TestStruct, testVal, testStaticVal)
};
int TestStruct::testStaticVal = 0;

TEST(ReflectionMemberTest, MemberTemplated)
{
    RareTs::Member<TestStruct, 0> member {};
    using IntMember = decltype(member);

    EXPECT_STREQ("testVal", IntMember::name);
    EXPECT_STREQ("int", IntMember::typeStr);
    
    bool isEqual = std::is_same_v<TestStruct, IntMember::object_type>;
    EXPECT_TRUE(isEqual);
    
    isEqual = std::is_same_v<int, IntMember::type>;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same_v<decltype(&TestStruct::testVal), IntMember::pointer_type>;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same_v<RareTs::NoNote, IntMember::Notes>;
    EXPECT_TRUE(isEqual);

    EXPECT_EQ(member.pointer, &TestStruct::testVal);

    EXPECT_EQ(size_t(0), IntMember::index);
    EXPECT_FALSE(IntMember::isStatic);
    EXPECT_FALSE(IntMember::isFunction);
    EXPECT_TRUE(IntMember::hasOffset);
    constexpr size_t intMemberOffset = offsetof(TestStruct, testVal);
    EXPECT_EQ(intMemberOffset, IntMember::getOffset());

    RareTs::Member<TestStruct, 1> staticMember {};
    using StaticIntMember = decltype(staticMember);

    EXPECT_EQ(staticMember.pointer, &TestStruct::testStaticVal);
    EXPECT_TRUE(StaticIntMember::isStatic);
    EXPECT_FALSE(StaticIntMember::isFunction);
    EXPECT_FALSE(StaticIntMember::hasOffset);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), StaticIntMember::getOffset());
}

struct ReferencesTestStruct
{
    int & testVal;
    static constexpr int & testStaticVal = TestStruct::testStaticVal;

    REFLECT(ReferencesTestStruct, testVal, testStaticVal)
};

TEST(ReflectionMemberTest, ReferencesMemberTemplated)
{
    RareTs::Member<ReferencesTestStruct, 0> member {};
    using IntMember = decltype(member);

    EXPECT_STREQ("testVal", IntMember::name);
    EXPECT_STREQ(std::string(RareTs::toStr<IntMember::type>()).c_str(), IntMember::typeStr);
    
    bool isEqual = std::is_same_v<ReferencesTestStruct, IntMember::object_type>;
    EXPECT_TRUE(isEqual);
    
    isEqual = std::is_same_v<int&, IntMember::type>;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same_v<std::nullptr_t, IntMember::pointer_type>;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same_v<RareTs::NoNote, IntMember::Notes>;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(member.pointer == nullptr);

    EXPECT_EQ(size_t(0), IntMember::index);
    EXPECT_FALSE(IntMember::isStatic);
    EXPECT_FALSE(IntMember::isFunction);
    EXPECT_FALSE(IntMember::hasOffset);

    RareTs::Member<ReferencesTestStruct, 1> staticMember {};
    using StaticIntMember = decltype(staticMember);

    isEqual = std::is_same_v<StaticIntMember::pointer_type, std::nullptr_t>;
    EXPECT_TRUE(isEqual);

    // Clang and GCC allow constexpr pointers to static references while MSVC has version-dependent behavior
    #if !defined(_MSC_VER) || defined(__clang__)
        EXPECT_EQ(staticMember.pointer, &ReferencesTestStruct::testStaticVal);
    #endif
    EXPECT_EQ(size_t(1), StaticIntMember::index);
    EXPECT_TRUE(StaticIntMember::isStatic);
    EXPECT_FALSE(StaticIntMember::hasOffset);
}

struct TestFunctions
{
    std::string reflectedFunction() { return "Hit Reflected Function"; }
    static std::string staticReflectedFunction() { return "Hit Static Reflected Function"; };

    REFLECT(TestFunctions, reflectedFunction, staticReflectedFunction)
};

template <typename T> using TestFunctionsStaticPointer = decltype(&T::staticReflectedFunction);

TEST(ReflectionMemberTest, MethodsAndFunctions)
{
    size_t memberIndex = 0;

    RareTs::Member<TestFunctions, 0> member {};
    using FunctionMember = decltype(member);
    
    EXPECT_STREQ("reflectedFunction", FunctionMember::name);
    EXPECT_STREQ("void", FunctionMember::typeStr);
    
    bool isEqual = std::is_same_v<TestFunctions, FunctionMember::object_type>;
    EXPECT_TRUE(isEqual);
    
    isEqual = std::is_same_v<decltype(&TestFunctions::reflectedFunction), FunctionMember::type>;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same_v<decltype(&TestFunctions::reflectedFunction), FunctionMember::pointer_type>;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same_v<RareTs::NoNote, FunctionMember::Notes>;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(member.pointer == &TestFunctions::reflectedFunction);

    EXPECT_EQ(memberIndex, FunctionMember::index);
    EXPECT_FALSE(FunctionMember::isStatic);
    EXPECT_TRUE(FunctionMember::isFunction);
    EXPECT_FALSE(FunctionMember::hasOffset);


    size_t staticMemberIndex = 1;

    RareTs::Member<TestFunctions, 1> staticMember {};
    using StaticFunctionMember = decltype(staticMember);
    
    EXPECT_STREQ("staticReflectedFunction", StaticFunctionMember::name);
    EXPECT_STREQ(std::string(RareTs::toStr<StaticFunctionMember::type>()).c_str(), StaticFunctionMember::typeStr);
    
    isEqual = std::is_same_v<TestFunctions, StaticFunctionMember::object_type>;
    EXPECT_TRUE(isEqual);
    
    isEqual = std::is_same_v<decltype(TestFunctions::staticReflectedFunction), StaticFunctionMember::type>;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same_v<decltype(&TestFunctions::staticReflectedFunction), StaticFunctionMember::pointer_type>;
    EXPECT_TRUE(isEqual);

    isEqual = std::is_same_v<RareTs::NoNote, StaticFunctionMember::Notes>;
    EXPECT_TRUE(isEqual);

    EXPECT_TRUE(staticMember.pointer == &TestFunctions::staticReflectedFunction);

    EXPECT_EQ(staticMemberIndex, StaticFunctionMember::index);
    EXPECT_TRUE(StaticFunctionMember::isStatic);
    EXPECT_TRUE(StaticFunctionMember::isFunction);
    EXPECT_FALSE(StaticFunctionMember::hasOffset);
}

struct AnnotationTest
{
    int zeroAnnotations;

    NOTE(oneAnnotation, 'a')
    int oneAnnotation;

    NOTE(twoAnnotations, 'b', 'c')
    int twoAnnotations;

    NOTE(threeAnnotations, 'd', 1337, 'e')
    int threeAnnotations;

    REFLECT(AnnotationTest, zeroAnnotations, oneAnnotation, twoAnnotations, threeAnnotations)
};

TEST(ReflectionMemberTest, Annotations)
{
    RareTs::Member<AnnotationTest, 0> zeroNoteMember {};
    using ZeroNoteMember = decltype(zeroNoteMember);

    bool hasAnnotation = ZeroNoteMember::template hasNote<int>();
    EXPECT_FALSE(hasAnnotation);

    size_t visitCount = 0;
    zeroNoteMember.forEachNote<int>([&](auto) {
        visitCount++;
    });
    EXPECT_EQ(size_t(0), visitCount);

    zeroNoteMember.forEachNote([&](auto) {
        visitCount++;
    });
    EXPECT_EQ(size_t(0), visitCount);


    RareTs::Member<AnnotationTest, 1> oneNoteMember {};
    using OneNoteMember = decltype(oneNoteMember);

    hasAnnotation = OneNoteMember::template hasNote<int>();
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = OneNoteMember::template hasNote<char>();
    EXPECT_TRUE(hasAnnotation);

    EXPECT_EQ('a', oneNoteMember.getNote<char>());

    visitCount = 0;
    oneNoteMember.forEachNote<int>([&](auto) {
        visitCount++;
    });
    EXPECT_EQ(size_t(0), visitCount);

    visitCount = 0;
    oneNoteMember.forEachNote<char>([&](auto annotation) {
        EXPECT_EQ('a', annotation);
        visitCount++;
    });
    EXPECT_EQ(size_t(1), visitCount);

    visitCount = 0;
    oneNoteMember.forEachNote([&](auto annotation) {
        EXPECT_EQ('a', annotation);
        visitCount++;
    });
    EXPECT_EQ(size_t(1), visitCount);


    RareTs::Member<AnnotationTest, 2> twoNoteMember {};
    using TwoNoteMember = decltype(twoNoteMember);

    hasAnnotation = TwoNoteMember::template hasNote<int>();
    EXPECT_FALSE(hasAnnotation);
    hasAnnotation = TwoNoteMember::template hasNote<char>();
    EXPECT_TRUE(hasAnnotation);

    EXPECT_EQ('b', twoNoteMember.getNote<char>());

    visitCount = 0;
    twoNoteMember.forEachNote<int>([&](auto) {
        visitCount++;
    });
    EXPECT_EQ(size_t(0), visitCount);

    visitCount = 0;
    twoNoteMember.forEachNote<char>([&](auto annotation) {
        if ( visitCount == 0 ) {
            EXPECT_EQ('b', annotation);
        } else if ( visitCount == 1 ) {
            EXPECT_EQ('c', annotation);
        }
        visitCount++;
    });
    EXPECT_EQ(size_t(2), visitCount);

    visitCount = 0;
    twoNoteMember.forEachNote([&](auto annotation) {
        if ( visitCount == 0 ) {
            EXPECT_EQ('b', annotation);
        } else if ( visitCount == 1 ) {
            EXPECT_EQ('c', annotation);
        }
        visitCount++;
    });
    EXPECT_EQ(size_t(2), visitCount);


    RareTs::Member<AnnotationTest, 3> threeNoteMember {};
    using ThreeNoteMember = decltype(threeNoteMember);

    hasAnnotation = ThreeNoteMember::template hasNote<int>();
    EXPECT_TRUE(hasAnnotation);
    hasAnnotation = ThreeNoteMember::template hasNote<char>();
    EXPECT_TRUE(hasAnnotation);
    
    EXPECT_EQ('d', threeNoteMember.getNote<char>());
    EXPECT_EQ(1337, threeNoteMember.getNote<int>());

    visitCount = 0;
    threeNoteMember.forEachNote<int>([&](auto annotation) {
        EXPECT_EQ(1337, annotation);
        visitCount++;
    });
    EXPECT_EQ(size_t(1), visitCount);

    visitCount = 0;
    threeNoteMember.forEachNote<char>([&](auto annotation) {
        if ( visitCount == 0 ) {
            EXPECT_EQ('d', annotation);
        } else if ( visitCount == 2 ) {
            EXPECT_EQ('e', annotation);
        }
        visitCount++;
    });
    EXPECT_EQ(size_t(2), visitCount);

    visitCount = 0;
    threeNoteMember.forEachNote([&](auto annotation) {
        if ( visitCount == 0 ) {
            EXPECT_EQ('d', annotation);
        } else if ( visitCount == 1 ) {
            EXPECT_EQ(1337, annotation);
        } else if ( visitCount == 3 ) {
            EXPECT_EQ('e', annotation);
        }
        visitCount++;
    });
    EXPECT_EQ(size_t(3), visitCount);
}

template <typename T>
struct Specializable {};

struct SpecializedAnnotations
{
    NOTE(field, RareMapper::MappedBy<AnnotationTest>)
    int field;

    REFLECT(SpecializedAnnotations, field)
};

TEST(ReflectionMemberTest, SpecializedAnnotations)
{
    RareTs::Member<SpecializedAnnotations, 0> memberWithSpecializedNote {};

    bool hasSpecializedAnnotations = memberWithSpecializedNote.hasNote<RareMapper::MappedByType>();
    EXPECT_TRUE(hasSpecializedAnnotations);

    hasSpecializedAnnotations = memberWithSpecializedNote.hasNote<Specializable>();
    EXPECT_FALSE(hasSpecializedAnnotations);

    constexpr auto & mappedByAnnotationsTest = memberWithSpecializedNote.getNote<RareMapper::MappedByType>();
    using MappedByAnnotationsTest = RareTs::remove_cvref_t<decltype(mappedByAnnotationsTest)>;

    bool isSame = std::is_same_v<MappedByAnnotationsTest, RareMapper::MappedByType<AnnotationTest>>;
    EXPECT_TRUE(isSame);

    using SpecializationType = typename MappedByAnnotationsTest::DefaultMapping;
    isSame = std::is_same_v<SpecializationType, AnnotationTest>;
    EXPECT_TRUE(isSame);

    int visitCount = 0;
    decltype(memberWithSpecializedNote)::forEachNote<RareMapper::MappedByType>([&](auto & annotation){
        isSame = std::is_same_v<RareMapper::IsMappedBy<AnnotationTest>, RareTs::remove_cvref_t<decltype(annotation)>>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
}
