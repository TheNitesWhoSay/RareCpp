#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"

namespace Unreflected
{
    struct SourceNoArg {};
    struct DestNoArg {};
    struct Source_intA { int a; };
    struct Dest_intA { int a; };
}

namespace Reflected
{
    NOTE(SourceNoArg) struct SourceNoArg { REFLECT_EMPTY(SourceNoArg)  };
    NOTE(DestNoArg) struct DestNoArg { REFLECT_EMPTY(DestNoArg) };
    struct Source_intA {
        int a;
        REFLECT(Source_intA, a)
    };
    struct Source_intB {
        int b;
        REFLECT(Source_intB, b)
    };
    struct Source_stringA {
        std::string a;
        REFLECT(Source_stringA, a)
    };
    struct Dest_intA {
        int a = 0;
        REFLECT(Dest_intA, a)
    };
    struct Source_intAB {
        int a;
        int b;
        REFLECT(Source_intAB, a, b)
    };
    struct Dest_intAB {
        int a;
        int b;
        REFLECT(Dest_intAB, a, b)
    };
    struct Dest_intAC {
        int a;
        int c;
        REFLECT(Dest_intAC, a, c)
    };
}

TEST(ObjectMapperTest, NoMapping)
{
    Unreflected::SourceNoArg srcNoArg_u;
    Unreflected::DestNoArg destNoArg_u;

    EXPECT_NO_THROW(ObjectMapper::map(srcNoArg_u, destNoArg_u));
    EXPECT_NO_THROW(ObjectMapper::map(srcNoArg_u, srcNoArg_u));

    Unreflected::Source_intA src_intA_u { 2 };
    Unreflected::Dest_intA dest_intA_u { 3 };

    EXPECT_EQ(src_intA_u.a, 2);
    EXPECT_EQ(dest_intA_u.a, 3);
    EXPECT_NO_THROW(ObjectMapper::map(srcNoArg_u, dest_intA_u));
    EXPECT_NO_THROW(ObjectMapper::map(srcNoArg_u, destNoArg_u));
    EXPECT_EQ(src_intA_u.a, 2);
    EXPECT_EQ(dest_intA_u.a, 3);


    Reflected::SourceNoArg srcNoArg_r;
    Reflected::DestNoArg destNoArg_r;

    EXPECT_NO_THROW(ObjectMapper::map(srcNoArg_r, destNoArg_r));
    EXPECT_NO_THROW(ObjectMapper::map(srcNoArg_r, srcNoArg_r));

    Reflected::Source_intA src_intA_r { 2 };
    Reflected::Dest_intA dest_intA_r { 3 };

    EXPECT_EQ(src_intA_r.a, 2);
    EXPECT_EQ(dest_intA_r.a, 3);
    EXPECT_NO_THROW(ObjectMapper::map(srcNoArg_r, dest_intA_r));
    EXPECT_NO_THROW(ObjectMapper::map(src_intA_r, destNoArg_r));
    EXPECT_EQ(src_intA_r.a, 2);
    EXPECT_EQ(dest_intA_r.a, 3);

    Reflected::Source_stringA source_stringA_r { "asdf" };
    EXPECT_STREQ(source_stringA_r.a.c_str(), "asdf");
    EXPECT_EQ(dest_intA_r.a, 3);
    EXPECT_NO_THROW(ObjectMapper::map(dest_intA_r, source_stringA_r));
    EXPECT_STREQ(source_stringA_r.a.c_str(), "asdf");
    EXPECT_EQ(dest_intA_r.a, 3);
}

TEST(ObjectMapperTest, MapParamSimpleReflected)
{
    Reflected::Source_intA srcIntA { 1 };
    Reflected::Source_intB srcIntB { 2 };
    Reflected::Dest_intA destIntA { 3 };
    Reflected::Source_intAB srcIntAB { 4, 5 };
    Reflected::Dest_intAB destIntAB { 6, 7 };
    Reflected::Dest_intAC destIntAC { 8, 9 };

    EXPECT_EQ(srcIntA.a, 1);
    EXPECT_EQ(srcIntB.b, 2);
    EXPECT_EQ(destIntA.a, 3);
    EXPECT_EQ(srcIntAB.a, 4);
    EXPECT_EQ(srcIntAB.b, 5);
    EXPECT_EQ(destIntAB.a, 6);
    EXPECT_EQ(destIntAB.b, 7);
    EXPECT_EQ(destIntAC.a, 8);
    EXPECT_EQ(destIntAC.c, 9);

    ObjectMapper::map(destIntA, srcIntB);
    EXPECT_EQ(srcIntB.b, 2);
    EXPECT_EQ(destIntA.a, 3);

    destIntA = ObjectMapper::map<Reflected::Dest_intA>(srcIntB);
    EXPECT_EQ(srcIntB.b, 2);
    // Result from ObjectMapper::map was a Reflected::Dest_intA object with an undefined value for a

    ObjectMapper::map(destIntA, srcIntA);
    EXPECT_EQ(srcIntA.a, 1);
    EXPECT_EQ(destIntA.a, 1);
    ObjectMapper::map(destIntA, srcIntAB);
    EXPECT_EQ(srcIntAB.a, 4);
    EXPECT_EQ(srcIntAB.b, 5);
    EXPECT_EQ(destIntA.a, 4);

    Reflected::Dest_intA destIntA2 = ObjectMapper::map<Reflected::Dest_intA>(srcIntA);
    EXPECT_EQ(srcIntA.a, 1);
    EXPECT_EQ(destIntA2.a, 1);

    ObjectMapper::map(destIntAB, srcIntAB);
    ObjectMapper::map(destIntAC, srcIntAB);
    EXPECT_EQ(srcIntAB.a, 4);
    EXPECT_EQ(srcIntAB.b, 5);
    EXPECT_EQ(destIntAB.a, 4);
    EXPECT_EQ(destIntAB.b, 5);
    EXPECT_EQ(destIntAC.a, 4);
    EXPECT_EQ(destIntAC.c, 9);

    ObjectMapper::map(destIntAB, srcIntA);
    EXPECT_EQ(srcIntA.a, 1);
    EXPECT_EQ(destIntAB.a, 1);
    EXPECT_EQ(destIntAB.b, 5);
}

namespace Unreflected
{
    struct SpecInt {
        int a;
    };
    struct SpecString {
        std::string a;
    };
}

template <> void ObjectMapper::map(Unreflected::SpecInt & specInt, const Unreflected::SpecString & specString)
{
    std::stringstream(std::string("2") + specString.a) >> specInt.a;
}

template <> void ObjectMapper::map(Unreflected::SpecString & specString, const Unreflected::SpecInt & specInt)
{
    specString.a = std::to_string(specInt.a+1);
}

TEST(ObjectMapperTest, MapSpecializations)
{
    Unreflected::SpecInt specInt { 0 };
    Unreflected::SpecString specString { "0" };
    EXPECT_EQ(specInt.a, 0);
    EXPECT_STREQ(specString.a.c_str(), "0");

    ObjectMapper::map(specString, specInt);
    EXPECT_EQ(specInt.a, 0);
    EXPECT_STREQ(specString.a.c_str(), "1");

    ObjectMapper::map(specInt, specString);
    EXPECT_EQ(specInt.a, 21);
    EXPECT_STREQ(specString.a.c_str(), "1");
}

namespace Unreflected
{
    struct Dest_intB
    {
        int b;
    };

    struct Source_intB
    {
        int b;

        MAP_TO(Dest_intB,
            (b, b)
        )
    };

    struct Dest_intCDEFG
    {
        int c;
        int d;
        int e;
        int f;
        int g;
    };

    struct Source_intCDEHI
    {
        int c;
        int d;
        int e;
        int h;
        int i;

        MAP_TO(Dest_intCDEFG,
            (c, c),
            (d, d),
            (e, e),
            (h, f),
            (h, g)
        )
    };

    struct Dest_intCDEFG_mapFrom
    {
        int c;
        int d;
        int e;
        int f;
        int g;

        MAP_FROM(Source_intCDEHI,
            (c, c),
            (d, d),
            (e, e),
            (f, h),
            (g, h)
        )
    };

    struct Dest_intCDEFG_mapWith
    {
        int c;
        int d;
        int e;
        int f;
        int g;

        MAP_WITH(Source_intCDEHI,
            (c, c),
            (d, d),
            (e, e),
            (f, h),
            (g, h)
        )
    };
}

TEST(ObjectMapperTest, MapToFromWithMacros)
{
    Unreflected::Source_intB srcIntB { 1 };
    Unreflected::Dest_intB destIntB { 0 };
    EXPECT_EQ(srcIntB.b, 1);
    EXPECT_EQ(destIntB.b, 0);

    ObjectMapper::map(destIntB, srcIntB);
    EXPECT_EQ(srcIntB.b, 1);
    EXPECT_EQ(destIntB.b, 1);

    Unreflected::Source_intCDEHI sourceCDEHI { 1, 2, 3, 4, 5 };
    Unreflected::Dest_intCDEFG destCDEFG { 0, 0, 0, 0, 0 };
    Unreflected::Dest_intCDEFG_mapFrom destCDEFG_mapFrom { 0, 0, 0, 0, 0 };
    Unreflected::Dest_intCDEFG_mapWith destCDEFG_mapWith { 0, 0, 0, 0, 0 };
    EXPECT_EQ(sourceCDEHI.c, 1);
    EXPECT_EQ(sourceCDEHI.d, 2);
    EXPECT_EQ(sourceCDEHI.e, 3);
    EXPECT_EQ(sourceCDEHI.h, 4);
    EXPECT_EQ(sourceCDEHI.i, 5);
    EXPECT_EQ(destCDEFG.c, 0);
    EXPECT_EQ(destCDEFG.d, 0);
    EXPECT_EQ(destCDEFG.e, 0);
    EXPECT_EQ(destCDEFG.f, 0);
    EXPECT_EQ(destCDEFG.g, 0);

    ObjectMapper::map(destCDEFG, sourceCDEHI);
    EXPECT_EQ(destCDEFG.c, 1);
    EXPECT_EQ(destCDEFG.d, 2);
    EXPECT_EQ(destCDEFG.e, 3);
    EXPECT_EQ(destCDEFG.f, 4);
    EXPECT_EQ(destCDEFG.g, 4);

    EXPECT_EQ(destCDEFG_mapFrom.c, 0);
    EXPECT_EQ(destCDEFG_mapFrom.d, 0);
    EXPECT_EQ(destCDEFG_mapFrom.e, 0);
    EXPECT_EQ(destCDEFG_mapFrom.f, 0);
    EXPECT_EQ(destCDEFG_mapFrom.g, 0);

    ObjectMapper::map(destCDEFG_mapFrom, sourceCDEHI);
    EXPECT_EQ(destCDEFG_mapFrom.c, 1);
    EXPECT_EQ(destCDEFG_mapFrom.d, 2);
    EXPECT_EQ(destCDEFG_mapFrom.e, 3);
    EXPECT_EQ(destCDEFG_mapFrom.f, 4);
    EXPECT_EQ(destCDEFG_mapFrom.g, 4);

    ObjectMapper::map(destCDEFG_mapWith, sourceCDEHI);
    EXPECT_EQ(destCDEFG_mapWith.c, 1);
    EXPECT_EQ(destCDEFG_mapWith.d, 2);
    EXPECT_EQ(destCDEFG_mapWith.e, 3);
    EXPECT_EQ(destCDEFG_mapWith.f, 4);
    EXPECT_EQ(destCDEFG_mapWith.g, 4);

    destCDEFG_mapWith.c = 6;
    destCDEFG_mapWith.d = 7;
    destCDEFG_mapWith.e = 8;
    destCDEFG_mapWith.f = 9;
    destCDEFG_mapWith.g = 99;
    ObjectMapper::map(sourceCDEHI, destCDEFG_mapWith); // Once now in reverse to confirm the map with worked
    EXPECT_EQ(sourceCDEHI.c, 6);
    EXPECT_EQ(sourceCDEHI.d, 7);
    EXPECT_EQ(sourceCDEHI.e, 8);
    EXPECT_EQ(sourceCDEHI.h, 99); // though both f and g are mapped onto h, g gets mapped onto h last
    EXPECT_EQ(sourceCDEHI.i, 5);
}

namespace Unreflected
{
    struct MapToFromMethods
    {
        int a;

        void map_to(Source_intA & o) const
        {
            o.a = this->a+1;
        }

        void map_from(const Source_intA & o)
        {
            this->a = o.a+2;
        }
    };
}

TEST(ObjectMapperTest, MapToFromMethods)
{
    Unreflected::Source_intA sourceIntA { 1 };
    Unreflected::MapToFromMethods mapToFromMethods { 0 };
    EXPECT_EQ(sourceIntA.a, 1);
    EXPECT_EQ(mapToFromMethods.a, 0);

    ObjectMapper::map(mapToFromMethods, sourceIntA);
    EXPECT_EQ(sourceIntA.a, 1);
    EXPECT_EQ(mapToFromMethods.a, 3);

    ObjectMapper::map(sourceIntA, mapToFromMethods);
    EXPECT_EQ(sourceIntA.a, 4);
    EXPECT_EQ(mapToFromMethods.a, 3);
}

struct PointableSrc
{
    int a;
    int b;
    int* c;
    int** d;
    std::unique_ptr<int> e;
    std::shared_ptr<int> f;
    int g;
    int h;
    int i;
    int j;
    int k;
    int* l;
    std::unique_ptr<int> m;
    std::shared_ptr<int> n;

    REFLECT(PointableSrc, a, b, c, d, e, f, g, h, i, j, k, l, m, n)
};

struct PointableDest
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int* h;
    int** i;
    std::unique_ptr<int> j;
    std::shared_ptr<int> k;
    int* l;
    std::unique_ptr<int> m;
    std::shared_ptr<int> n;

    REFLECT(PointableDest, a, b, c, d, e, f, g, h, i, j, k, l, m, n)
};

TEST(ObjectMapperTest, MapPointables)
{
    int anInt = 100;
    int* anIntPtr = &anInt;
    int anotherInt = 200;
    int* anotherIntPtr = &anotherInt;
    PointableSrc pointableSrc {1, 2, nullptr, nullptr, nullptr, nullptr, 3, 4, 5, 6, 7, nullptr, nullptr, nullptr};
    pointableSrc.c = &pointableSrc.b;
    pointableSrc.d = &(anIntPtr);
    pointableSrc.e = std::make_unique<int>(8);
    pointableSrc.f = std::make_shared<int>(9);

    PointableDest pointableDest {0, 0, 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    pointableDest.h = &pointableDest.b;
    pointableDest.i = &anotherIntPtr;
    pointableDest.l = &anInt;
    pointableDest.m = std::make_unique<int>(0);
    pointableDest.n = std::make_shared<int>(0);

    EXPECT_EQ(pointableSrc.a, 1);
    EXPECT_EQ(pointableSrc.b, 2);
    EXPECT_FALSE(pointableSrc.c == nullptr);
    EXPECT_EQ(*pointableSrc.c, 2);
    EXPECT_FALSE(pointableSrc.d == nullptr);
    EXPECT_EQ(**pointableSrc.d, 100);
    EXPECT_FALSE(pointableSrc.e == nullptr);
    EXPECT_EQ(*pointableSrc.e, 8);
    EXPECT_FALSE(pointableSrc.f == nullptr);
    EXPECT_EQ(*pointableSrc.f, 9);
    EXPECT_EQ(pointableSrc.g, 3);
    EXPECT_EQ(pointableSrc.h, 4);
    EXPECT_EQ(pointableSrc.i, 5);
    EXPECT_EQ(pointableSrc.j, 6);
    EXPECT_EQ(pointableSrc.k, 7);
    EXPECT_TRUE(pointableSrc.l == nullptr);
    EXPECT_TRUE(pointableSrc.m == nullptr);
    EXPECT_TRUE(pointableSrc.n == nullptr);

    EXPECT_EQ(pointableDest.a, 0);
    EXPECT_EQ(pointableDest.b, 0);
    EXPECT_EQ(pointableDest.c, 0);
    EXPECT_EQ(pointableDest.d, 0);
    EXPECT_EQ(pointableDest.e, 0);
    EXPECT_EQ(pointableDest.f, 0);
    EXPECT_EQ(pointableDest.g, 0);
    EXPECT_FALSE(pointableDest.h == nullptr);
    EXPECT_EQ(*pointableDest.h, 0);
    EXPECT_FALSE(pointableDest.i == nullptr);
    EXPECT_EQ(**pointableDest.i, 200);
    EXPECT_TRUE(pointableDest.j == nullptr);
    EXPECT_TRUE(pointableDest.k == nullptr);
    EXPECT_FALSE(pointableDest.l == nullptr);
    EXPECT_EQ(*pointableDest.l, 100);
    EXPECT_FALSE(pointableDest.m == nullptr);
    EXPECT_EQ(*pointableDest.m, 0);
    EXPECT_FALSE(pointableDest.n == nullptr);
    EXPECT_EQ(*pointableDest.n, 0);

    ObjectMapper::map(pointableDest, pointableSrc);

    EXPECT_EQ(pointableDest.a, 1);
    EXPECT_EQ(pointableDest.b, 4);
    EXPECT_EQ(pointableDest.c, 2);
    EXPECT_EQ(pointableDest.d, 100);
    EXPECT_EQ(pointableDest.e, 8);
    EXPECT_EQ(pointableDest.f, 9);
    EXPECT_EQ(pointableDest.g, 3);
    EXPECT_FALSE(pointableDest.h == nullptr);
    EXPECT_EQ(*pointableDest.h, 4);
    EXPECT_FALSE(pointableDest.i == nullptr);
    EXPECT_EQ(**pointableDest.i, 5);
    EXPECT_FALSE(pointableDest.j == nullptr);
    EXPECT_EQ(*pointableDest.j, 6);
    EXPECT_FALSE(pointableDest.k == nullptr);
    EXPECT_EQ(*pointableDest.k, 7);
    EXPECT_TRUE(pointableDest.l == nullptr);
    EXPECT_TRUE(pointableDest.m == nullptr);
    EXPECT_TRUE(pointableDest.n == nullptr);
}

TEST(ObjectMapperTest, MapAssignables)
{
    struct R1
    {
        int a;
    };
    struct L1
    {
        int a;
        L1 & operator=(const R1 & rhs) { a = rhs.a; return *this; }
    };
    L1 l1 { 0 };
    R1 r1 { 1 };
    EXPECT_EQ(l1.a, 0);
    EXPECT_EQ(r1.a, 1);
    ObjectMapper::map(l1, r1); // Assignment operator enables mapping
    EXPECT_EQ(l1.a, 1);
    EXPECT_EQ(r1.a, 1);
    l1.a = 0;
    r1.a = 1;
    ObjectMapper::map(r1, l1); // No assignment operator for reverse direction/no mapping
    EXPECT_EQ(l1.a, 0);
    EXPECT_EQ(r1.a, 1);

    struct R3
    {
        int a;
    };
    struct L3
    {
        int a;
        L3() : a(0) {};
        L3(const R3 &) { a = 2; };
    };

    L3 l3;
    EXPECT_EQ(l3.a, 0);
    R3 r3{1};
    ObjectMapper::map(l3, r3); // Converting-constructor enables assignability/mapping
    EXPECT_EQ(l3.a, 2);

    struct L5
    {
        int a;
    };
    struct R5
    {
        int a;
        operator L5() const { return L5{ a }; }
    };

    L5 l5{ 0 };
    R5 r5{ 1 };
    EXPECT_EQ(l5.a, 0);
    EXPECT_EQ(r5.a, 1);

    ObjectMapper::map(l5, r5); // Conversion op enables assignability/mapping
    EXPECT_EQ(l5.a, 1);
    EXPECT_EQ(r5.a, 1);
}

TEST(ObjectMapperTest, MapStaticCastAssignables)
{
    struct R3
    {
        int a;
    };
    struct L3
    {
        int a;
        L3() : a(0) {};
        L3(const R3 &) { a = 2; };
    };

    L3 l3;
    EXPECT_EQ(l3.a, 0);
    R3 r3{1};
    ObjectMapper::map(l3, r3); // Converting-constructor should signal static-cast assignability/enable mapping
    EXPECT_EQ(l3.a, 2);

    struct L5
    {
        int a;
    };
    struct R5
    {
        int a;
        operator L5() const { return L5{ a }; }
    };

    L5 l5{ 0 };
    R5 r5{ 1 };
    EXPECT_EQ(l5.a, 0);
    EXPECT_EQ(r5.a, 1);

    ObjectMapper::map(l5, r5); // Conversion op should signal static_cast assignability/enable mapping
    EXPECT_EQ(l5.a, 1);
    EXPECT_EQ(r5.a, 1);

    struct L6
    {
        int a;
    };
    struct R6
    {
        int a;
        explicit operator L6() const { return L6{ 1 }; }
    };

    L6 l6{ 0 };
    R6 r6{ 1 };
    EXPECT_EQ(l6.a, 0);
    EXPECT_EQ(r6.a, 1);

    ObjectMapper::map(l6, r6); // Explicit conversion op should signal static-cast assignability/enable mapping
    EXPECT_EQ(l6.a, 1);
    EXPECT_EQ(r6.a, 1);
}

namespace Reflected
{
    struct PairSource
    {
        std::pair<int, std::string> a;
        REFLECT(PairSource, a)
    };
    struct PairDestination
    {
        std::pair<int, std::string> a;
        REFLECT(PairDestination, a)
    };
    struct PairDestinationHalfMatch
    {
        std::pair<int, int> a;
        REFLECT(PairDestinationHalfMatch, a)
    };
}

TEST(ObjectMapperTest, MapPair)
{
    Reflected::PairSource pairSource { {1, "asdf"} };
    Reflected::PairDestination pairDestination { {0, ""} };
    Reflected::PairDestinationHalfMatch pairDestinationHalfMatch { {0, 0} };
    EXPECT_EQ(pairSource.a.first, 1);
    EXPECT_STREQ(pairSource.a.second.c_str(), "asdf");
    EXPECT_EQ(pairDestination.a.first, 0);
    EXPECT_STREQ(pairDestination.a.second.c_str(), "");
    EXPECT_EQ(pairDestinationHalfMatch.a.first, 0);
    EXPECT_EQ(pairDestinationHalfMatch.a.second, 0);

    ObjectMapper::map(pairDestination, pairSource);
    EXPECT_EQ(pairSource.a.first, 1);
    EXPECT_STREQ(pairSource.a.second.c_str(), "asdf");
    EXPECT_EQ(pairDestination.a.first, 1);
    EXPECT_STREQ(pairDestination.a.second.c_str(), "asdf");

    ObjectMapper::map(pairDestinationHalfMatch, pairSource);
    EXPECT_EQ(pairSource.a.first, 1);
    EXPECT_STREQ(pairSource.a.second.c_str(), "asdf");
    EXPECT_EQ(pairDestinationHalfMatch.a.first, 1);
    EXPECT_EQ(pairDestinationHalfMatch.a.second, 0);

}

namespace Reflected
{
    struct EmptyTuple
    {
        std::tuple<> a;
        REFLECT(EmptyTuple, a)
    };
    struct IntTuple
    {
        std::tuple<int> a;
        REFLECT(IntTuple, a)
    };
    struct TupleSource
    {
        std::tuple<int, std::string, char> a;
        REFLECT(TupleSource, a)
    };
    struct TupleDestination
    {
        std::tuple<int, std::string, char> a;
        REFLECT(TupleDestination, a)
    };
}

TEST(ObjectMapperTest, MapTuple)
{
    Reflected::TupleSource tupleSource { {1, "asdf", 'b'} };
    Reflected::TupleDestination tupleDestination { {0, "", 'a'} };
    EXPECT_EQ(std::get<0>(tupleSource.a), 1);
    EXPECT_STREQ(std::get<1>(tupleSource.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleSource.a), 'b');
    EXPECT_EQ(std::get<0>(tupleDestination.a), 0);
    EXPECT_STREQ(std::get<1>(tupleDestination.a).c_str(), "");
    EXPECT_EQ(std::get<2>(tupleDestination.a), 'a');

    ObjectMapper::map(tupleDestination, tupleSource);
    EXPECT_EQ(std::get<0>(tupleSource.a), 1);
    EXPECT_STREQ(std::get<1>(tupleSource.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleSource.a), 'b');
    EXPECT_EQ(std::get<0>(tupleDestination.a), 1);
    EXPECT_STREQ(std::get<1>(tupleDestination.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleDestination.a), 'b');

    Reflected::EmptyTuple emptyTuple {}; // Mapping should have no effect
    EXPECT_NO_THROW(ObjectMapper::map(emptyTuple, tupleSource));
    EXPECT_NO_THROW(ObjectMapper::map(tupleDestination, emptyTuple));
    EXPECT_EQ(std::get<0>(tupleSource.a), 1);
    EXPECT_STREQ(std::get<1>(tupleSource.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleSource.a), 'b');
    EXPECT_EQ(std::get<0>(tupleDestination.a), 1);
    EXPECT_STREQ(std::get<1>(tupleDestination.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleDestination.a), 'b');

    Reflected::IntTuple intTuple { {5} }; // Mapping should effect only matching parts of tuple
    EXPECT_EQ(std::get<0>(intTuple.a), 5);
    ObjectMapper::map(tupleDestination, intTuple);
    EXPECT_EQ(std::get<0>(tupleDestination.a), 5);
    EXPECT_STREQ(std::get<1>(tupleDestination.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleDestination.a), 'b');

    ObjectMapper::map(intTuple, tupleSource);
    EXPECT_EQ(std::get<0>(intTuple.a), 1);
    EXPECT_EQ(std::get<0>(tupleSource.a), 1);
    EXPECT_STREQ(std::get<1>(tupleSource.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleSource.a), 'b');
}

TEST(ObjectMapperTest, MapIterables)
{
    std::vector<int> srcVec { 0, 2, 4 };
    auto destVec = ObjectMapper::map<std::vector<int>>(srcVec);

    EXPECT_EQ(3, destVec.size());
    EXPECT_EQ(0, destVec[0]);
    EXPECT_EQ(2, destVec[1]);
    EXPECT_EQ(4, destVec[2]);

    std::map<int, int> srcMap {{1, 2}, {2, 4}, {3, 6}};
    auto destMap = ObjectMapper::map<std::map<int, int>>(srcMap);
    EXPECT_EQ(3, destMap.size());
    auto firstMapEntry = destMap.find(1);
    auto secondMapEntry = destMap.find(2);
    auto thirdMapEntry = destMap.find(3);
    EXPECT_TRUE(firstMapEntry != destMap.end());
    EXPECT_TRUE(secondMapEntry != destMap.end());
    EXPECT_TRUE(thirdMapEntry != destMap.end());
    EXPECT_EQ(1, firstMapEntry->first);
    EXPECT_EQ(2, secondMapEntry->first);
    EXPECT_EQ(3, thirdMapEntry->first);
    EXPECT_EQ(2, firstMapEntry->second);
    EXPECT_EQ(4, secondMapEntry->second);
    EXPECT_EQ(6, thirdMapEntry->second);

    int srcRay[] = { 0, 2, 4 };
    int destRay[] = { 0, 0, 0 };
    ObjectMapper::map(destRay, srcRay);
    EXPECT_EQ(0, destRay[0]);
    EXPECT_EQ(2, destRay[1]);
    EXPECT_EQ(4, destRay[2]);

    auto rayVecDest = ObjectMapper::map<std::vector<int>>(srcRay);
    EXPECT_EQ(3, rayVecDest.size());
    EXPECT_EQ(0, rayVecDest[0]);
    EXPECT_EQ(2, rayVecDest[1]);
    EXPECT_EQ(4, rayVecDest[2]);

    destRay[0] = 0;
    destRay[1] = 0;
    destRay[2] = 0;
    ObjectMapper::map(destRay, srcVec);
    EXPECT_EQ(0, destRay[0]);
    EXPECT_EQ(2, destRay[1]);
    EXPECT_EQ(4, destRay[2]);
}

struct MapSource
{
    int a;
    Reflected::Source_intA b;
    Unreflected::SpecInt c;
    std::vector<Reflected::Source_intA> d;
    std::vector<Unreflected::SpecInt> e;

    REFLECT(MapSource, a, b, c, d, e)
};

struct MapDest
{
    int a;
    Reflected::Dest_intA b;
    Unreflected::SpecString c;
    std::vector<Reflected::Dest_intA> d;
    std::vector<Unreflected::SpecInt> e;

    REFLECT(MapDest, a, b, c, d, e)
};

TEST(ObjectMapperTest, MapRecurse)
{
    MapSource mapSource { 1, {2}, {3}, {{4}, {5}}, {{6}, {7}} };
    MapDest mapDest { 0, {0}, {""}, {}, {}};

    EXPECT_EQ(mapSource.a, 1);
    EXPECT_EQ(mapSource.b.a, 2);
    EXPECT_EQ(mapSource.c.a, 3);
    EXPECT_EQ(mapSource.d.size(), 2);
    EXPECT_EQ(mapSource.d[0].a, 4);
    EXPECT_EQ(mapSource.d[1].a, 5);
    EXPECT_EQ(mapSource.e.size(), 2);
    EXPECT_EQ(mapSource.e[0].a, 6);
    EXPECT_EQ(mapSource.e[1].a, 7);

    EXPECT_EQ(mapDest.a, 0);
    EXPECT_EQ(mapDest.b.a, 0);
    EXPECT_STREQ(mapDest.c.a.c_str(), "");
    EXPECT_EQ(mapDest.d.size(), 0);
    EXPECT_EQ(mapDest.e.size(), 0);

    ObjectMapper::map(mapDest, mapSource);

    EXPECT_EQ(mapSource.a, 1);
    EXPECT_EQ(mapSource.b.a, 2);
    EXPECT_EQ(mapSource.c.a, 3);
    EXPECT_EQ(mapSource.d.size(), 2);
    EXPECT_EQ(mapSource.d[0].a, 4);
    EXPECT_EQ(mapSource.d[1].a, 5);
    EXPECT_EQ(mapSource.e.size(), 2);
    EXPECT_EQ(mapSource.e[0].a, 6);
    EXPECT_EQ(mapSource.e[1].a, 7);

    EXPECT_EQ(mapDest.a, 1);
    EXPECT_EQ(mapDest.b.a, 2);
    EXPECT_STREQ(mapDest.c.a.c_str(), "4");
    EXPECT_EQ(mapDest.d.size(), 2);
    EXPECT_EQ(mapDest.d[0].a, 4);
    EXPECT_EQ(mapDest.d[1].a, 5);
    EXPECT_EQ(mapDest.e.size(), 2);
    EXPECT_EQ(mapDest.e[0].a, 6);
    EXPECT_EQ(mapDest.e[1].a, 7);
}

struct MapMacroLimitSrc
{
    int a000; int a001; int a002; int a003; int a004; int a005; int a006; int a007; int a008; int a009;
    int a010; int a011; int a012; int a013; int a014; int a015; int a016; int a017; int a018; int a019;
    int a020; int a021; int a022; int a023; int a024; int a025; int a026; int a027; int a028; int a029;
    int a030; int a031; int a032; int a033; int a034; int a035; int a036; int a037; int a038; int a039;
    int a040; int a041; int a042; int a043; int a044; int a045; int a046; int a047; int a048; int a049;

    int a050; int a051; int a052; int a053; int a054; int a055; int a056; int a057; int a058; int a059;
    int a060; int a061; int a062; int a063; int a064; int a065; int a066; int a067; int a068; int a069;
    int a070; int a071; int a072; int a073; int a074; int a075; int a076; int a077; int a078; int a079;
    int a080; int a081; int a082; int a083; int a084; int a085; int a086; int a087; int a088; int a089;
    int a090; int a091; int a092; int a093; int a094; int a095; int a096; int a097; int a098; int a099;

    int a100; int a101; int a102; int a103; int a104; int a105; int a106; int a107; int a108; int a109;
    int a110; int a111; int a112; int a113; int a114; int a115; int a116; int a117; int a118; int a119;
    int a120; int a121; int a122; int a123; int a124;
};

struct MapMacroLimitDest
{
    int b000; int b001; int b002; int b003; int b004; int b005; int b006; int b007; int b008; int b009;
    int b010; int b011; int b012; int b013; int b014; int b015; int b016; int b017; int b018; int b019;
    int b020; int b021; int b022; int b023; int b024; int b025; int b026; int b027; int b028; int b029;
    int b030; int b031; int b032; int b033; int b034; int b035; int b036; int b037; int b038; int b039;
    int b040; int b041; int b042; int b043; int b044; int b045; int b046; int b047; int b048; int b049;

    int b050; int b051; int b052; int b053; int b054; int b055; int b056; int b057; int b058; int b059;
    int b060; int b061; int b062; int b063; int b064; int b065; int b066; int b067; int b068; int b069;
    int b070; int b071; int b072; int b073; int b074; int b075; int b076; int b077; int b078; int b079;
    int b080; int b081; int b082; int b083; int b084; int b085; int b086; int b087; int b088; int b089;
    int b090; int b091; int b092; int b093; int b094; int b095; int b096; int b097; int b098; int b099;

    int b100; int b101; int b102; int b103; int b104; int b105; int b106; int b107; int b108; int b109;
    int b110; int b111; int b112; int b113; int b114; int b115; int b116; int b117; int b118; int b119;
    int b120; int b121; int b122; int b123; int b124;

    MAP_WITH(MapMacroLimitSrc,
        (b000, a000),(b001, a001),(b002, a002),(b003, a003),(b004, a004),
        (b005, a005),(b006, a006),(b007, a007),(b008, a008),(b009, a009),

        (b010, a010),(b011, a011),(b012, a012),(b013, a013),(b014, a014),
        (b015, a015),(b016, a016),(b017, a017),(b018, a018),(b019, a019),

        (b020, a020),(b021, a021),(b022, a022),(b023, a023),(b024, a024),
        (b025, a025),(b026, a026),(b027, a027),(b028, a028),(b029, a029),

        (b030, a030),(b031, a031),(b032, a032),(b033, a033),(b034, a034),
        (b035, a035),(b036, a036),(b037, a037),(b038, a038),(b039, a039),

        (b040, a040),(b041, a041),(b042, a042),(b043, a043),(b044, a044),
        (b045, a045),(b046, a046),(b047, a047),(b048, a048),(b049, a049),

        (b050, a050),(b051, a051),(b052, a052),(b053, a053),(b054, a054),
        (b055, a055),(b056, a056),(b057, a057),(b058, a058),(b059, a059),

        (b060, a060),(b061, a061),(b062, a062),(b063, a063),(b064, a064),
        (b065, a065),(b066, a066),(b067, a067),(b068, a068),(b069, a069),

        (b070, a070),(b071, a071),(b072, a072),(b073, a073),(b074, a074),
        (b075, a075),(b076, a076),(b077, a077),(b078, a078),(b079, a079),

        (b080, a080),(b081, a081),(b082, a082),(b083, a083),(b084, a084),
        (b085, a085),(b086, a086),(b087, a087),(b088, a088),(b089, a089),

        (b090, a090),(b091, a091),(b092, a092),(b093, a093),(b094, a094),
        (b095, a095),(b096, a096),(b097, a097),(b098, a098),(b099, a099),
        
        (b100, a100),(b101, a101),(b102, a102),(b103, a103),(b104, a104),
        (b105, a105),(b106, a106),(b107, a107),(b108, a108),(b109, a109),

        (b110, a110),(b111, a111),(b112, a112),(b113, a113),(b114, a114),
        (b115, a115),(b116, a116),(b117, a117),(b118, a118),(b119, a119),

        (b120, a120),(b121, a121),(b122, a122),(b123, a123)
        //,(b124, a124) // This will exceed the visual studios macro nesting limit (C1009)
    )
};

TEST(ObjectMapperTest, MapMacroLimit)
{
    MapMacroLimitSrc src {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47, 48, 49,

        50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
        70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
        90, 91, 92, 93, 94, 95, 96, 97, 98, 99,

        100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
        110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 123, 124
    };
    MapMacroLimitDest dest {};
    dest.b124 = 0;
    ObjectMapper::map(dest, src);
    EXPECT_EQ(src.a000, dest.b000);
    EXPECT_EQ(src.a001, dest.b001);
    EXPECT_EQ(src.a002, dest.b002);
    EXPECT_EQ(src.a003, dest.b003);
    EXPECT_EQ(src.a004, dest.b004);
    EXPECT_EQ(src.a005, dest.b005);
    EXPECT_EQ(src.a006, dest.b006);
    EXPECT_EQ(src.a007, dest.b007);
    EXPECT_EQ(src.a008, dest.b008);
    EXPECT_EQ(src.a009, dest.b009);

    EXPECT_EQ(src.a010, dest.b010);
    EXPECT_EQ(src.a011, dest.b011);
    EXPECT_EQ(src.a012, dest.b012);
    EXPECT_EQ(src.a013, dest.b013);
    EXPECT_EQ(src.a014, dest.b014);
    EXPECT_EQ(src.a015, dest.b015);
    EXPECT_EQ(src.a016, dest.b016);
    EXPECT_EQ(src.a017, dest.b017);
    EXPECT_EQ(src.a018, dest.b018);
    EXPECT_EQ(src.a019, dest.b019);

    EXPECT_EQ(src.a020, dest.b020);
    EXPECT_EQ(src.a021, dest.b021);
    EXPECT_EQ(src.a022, dest.b022);
    EXPECT_EQ(src.a023, dest.b023);
    EXPECT_EQ(src.a024, dest.b024);
    EXPECT_EQ(src.a025, dest.b025);
    EXPECT_EQ(src.a026, dest.b026);
    EXPECT_EQ(src.a027, dest.b027);
    EXPECT_EQ(src.a028, dest.b028);
    EXPECT_EQ(src.a029, dest.b029);

    EXPECT_EQ(src.a030, dest.b030);
    EXPECT_EQ(src.a031, dest.b031);
    EXPECT_EQ(src.a032, dest.b032);
    EXPECT_EQ(src.a033, dest.b033);
    EXPECT_EQ(src.a034, dest.b034);
    EXPECT_EQ(src.a035, dest.b035);
    EXPECT_EQ(src.a036, dest.b036);
    EXPECT_EQ(src.a037, dest.b037);
    EXPECT_EQ(src.a038, dest.b038);
    EXPECT_EQ(src.a039, dest.b039);

    EXPECT_EQ(src.a040, dest.b040);
    EXPECT_EQ(src.a041, dest.b041);
    EXPECT_EQ(src.a042, dest.b042);
    EXPECT_EQ(src.a043, dest.b043);
    EXPECT_EQ(src.a044, dest.b044);
    EXPECT_EQ(src.a045, dest.b045);
    EXPECT_EQ(src.a046, dest.b046);
    EXPECT_EQ(src.a047, dest.b047);
    EXPECT_EQ(src.a048, dest.b048);
    EXPECT_EQ(src.a049, dest.b049);

    EXPECT_EQ(src.a050, dest.b050);
    EXPECT_EQ(src.a051, dest.b051);
    EXPECT_EQ(src.a052, dest.b052);
    EXPECT_EQ(src.a053, dest.b053);
    EXPECT_EQ(src.a054, dest.b054);
    EXPECT_EQ(src.a055, dest.b055);
    EXPECT_EQ(src.a056, dest.b056);
    EXPECT_EQ(src.a057, dest.b057);
    EXPECT_EQ(src.a058, dest.b058);
    EXPECT_EQ(src.a059, dest.b059);

    EXPECT_EQ(src.a060, dest.b060);
    EXPECT_EQ(src.a061, dest.b061);
    EXPECT_EQ(src.a062, dest.b062);
    EXPECT_EQ(src.a063, dest.b063);
    EXPECT_EQ(src.a064, dest.b064);
    EXPECT_EQ(src.a065, dest.b065);
    EXPECT_EQ(src.a066, dest.b066);
    EXPECT_EQ(src.a067, dest.b067);
    EXPECT_EQ(src.a068, dest.b068);
    EXPECT_EQ(src.a069, dest.b069);

    EXPECT_EQ(src.a070, dest.b070);
    EXPECT_EQ(src.a071, dest.b071);
    EXPECT_EQ(src.a072, dest.b072);
    EXPECT_EQ(src.a073, dest.b073);
    EXPECT_EQ(src.a074, dest.b074);
    EXPECT_EQ(src.a075, dest.b075);
    EXPECT_EQ(src.a076, dest.b076);
    EXPECT_EQ(src.a077, dest.b077);
    EXPECT_EQ(src.a078, dest.b078);
    EXPECT_EQ(src.a079, dest.b079);

    EXPECT_EQ(src.a080, dest.b080);
    EXPECT_EQ(src.a081, dest.b081);
    EXPECT_EQ(src.a082, dest.b082);
    EXPECT_EQ(src.a083, dest.b083);
    EXPECT_EQ(src.a084, dest.b084);
    EXPECT_EQ(src.a085, dest.b085);
    EXPECT_EQ(src.a086, dest.b086);
    EXPECT_EQ(src.a087, dest.b087);
    EXPECT_EQ(src.a088, dest.b088);
    EXPECT_EQ(src.a089, dest.b089);

    EXPECT_EQ(src.a090, dest.b090);
    EXPECT_EQ(src.a091, dest.b091);
    EXPECT_EQ(src.a092, dest.b092);
    EXPECT_EQ(src.a093, dest.b093);
    EXPECT_EQ(src.a094, dest.b094);
    EXPECT_EQ(src.a095, dest.b095);
    EXPECT_EQ(src.a096, dest.b096);
    EXPECT_EQ(src.a097, dest.b097);
    EXPECT_EQ(src.a098, dest.b098);
    EXPECT_EQ(src.a099, dest.b099);

    EXPECT_EQ(src.a100, dest.b100);
    EXPECT_EQ(src.a101, dest.b101);
    EXPECT_EQ(src.a102, dest.b102);
    EXPECT_EQ(src.a103, dest.b103);
    EXPECT_EQ(src.a104, dest.b104);
    EXPECT_EQ(src.a105, dest.b105);
    EXPECT_EQ(src.a106, dest.b106);
    EXPECT_EQ(src.a107, dest.b107);
    EXPECT_EQ(src.a108, dest.b108);
    EXPECT_EQ(src.a109, dest.b109);

    EXPECT_EQ(src.a110, dest.b110);
    EXPECT_EQ(src.a111, dest.b111);
    EXPECT_EQ(src.a112, dest.b112);
    EXPECT_EQ(src.a113, dest.b113);
    EXPECT_EQ(src.a114, dest.b114);
    EXPECT_EQ(src.a115, dest.b115);
    EXPECT_EQ(src.a116, dest.b116);
    EXPECT_EQ(src.a117, dest.b117);
    EXPECT_EQ(src.a118, dest.b118);
    EXPECT_EQ(src.a119, dest.b119);

    EXPECT_EQ(src.a120, dest.b120);
    EXPECT_EQ(src.a121, dest.b121);
    EXPECT_EQ(src.a122, dest.b122);
    EXPECT_EQ(src.a123, dest.b123);
    EXPECT_NE(src.a124, dest.b124);
    EXPECT_EQ(src.a124, 124);
    EXPECT_EQ(dest.b124, 0);
}

TEST(ObjectMapperAnnotationsTest, MappedByAnnotation)
{
    auto mappedByNote = ObjectMapper::MappedBy<int>;
    using MappedByNote = decltype(mappedByNote);
    bool defaultMappingIsInt = std::is_same_v<int, typename MappedByNote::DefaultMapping>;
    EXPECT_TRUE(defaultMappingIsInt);
}

TEST(ObjectMapperAnnotationsTest, UseMappingOpAnnotation)
{
    using OpUseMapping = ObjectMapper::UseMapping<int, float>;
    bool objectIsInt = std::is_same_v<int, typename OpUseMapping::Object>;
    EXPECT_TRUE(objectIsInt);
    bool defaultMappingIsFloat = std::is_same_v<float, typename OpUseMapping::DefaultMapping>;
    EXPECT_TRUE(defaultMappingIsFloat);
}

struct TypeMappedBy {};

struct ObjWithoutMapping {};

struct ObjMappedBySpecialization {};
template <> struct ObjectMapper::SetTags<ObjMappedBySpecialization> : IsMappedBy<TypeMappedBy> {};

struct ObjMappedByMacro {};
SET_DEFAULT_OBJECT_MAPPING(ObjMappedByMacro, TypeMappedBy)

NOTE(ObjMappedByClassNote, ObjectMapper::MappedBy<TypeMappedBy>)
struct ObjMappedByClassNote { REFLECT_EMPTY(ObjMappedByClassNote) };

TEST(ObjectMapperAnnotationsTest, Tags)
{
    bool isSame = std::is_same_v<TypeMappedBy, ObjectMapper::GetTags<ObjMappedBySpecialization>::DefaultMapping>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<TypeMappedBy, ObjectMapper::GetTags<ObjMappedByMacro>::DefaultMapping>;
    EXPECT_TRUE(isSame);
}

TEST(ObjectMapperAnnotationsTest, HasDefaultMapping)
{
    bool hasDefaultMapping = ObjectMapper::HasDefaultMapping<TypeMappedBy>;
    EXPECT_FALSE(hasDefaultMapping);
    hasDefaultMapping = ObjectMapper::HasDefaultMapping<ObjMappedBySpecialization>;
    EXPECT_TRUE(hasDefaultMapping);
    hasDefaultMapping = ObjectMapper::HasDefaultMapping<ObjMappedByMacro>;
    EXPECT_TRUE(hasDefaultMapping);
    using ClassNote = Reflect::class_notes_t<ObjMappedByClassNote>;
    hasDefaultMapping = ObjectMapper::HasDefaultMapping<ObjMappedByClassNote>;
    EXPECT_TRUE(hasDefaultMapping);

    auto classOrFieldNoteWithMapping = std::tuple { ObjectMapper::MappedBy<TypeMappedBy> };
    using ClassOrFieldNoteWithMapping = decltype(classOrFieldNoteWithMapping);
    using OpNoteWithMapping = std::tuple<ObjectMapper::UseMapping<ObjWithoutMapping, TypeMappedBy>>;
    hasDefaultMapping = ObjectMapper::HasDefaultMapping<ObjWithoutMapping, ClassOrFieldNoteWithMapping>;
    EXPECT_TRUE(hasDefaultMapping);
    hasDefaultMapping = ObjectMapper::HasDefaultMapping<ObjWithoutMapping, void, OpNoteWithMapping>;
    EXPECT_TRUE(hasDefaultMapping);
    hasDefaultMapping = ObjectMapper::HasDefaultMapping<ObjWithoutMapping, ClassOrFieldNoteWithMapping, OpNoteWithMapping>;
    EXPECT_TRUE(hasDefaultMapping);
}

TEST(ObjectMapperAnnotationsTest, GetDefaultMapping)
{
    bool isVoid = std::is_same_v<void, ObjectMapper::GetDefaultMapping<TypeMappedBy>>;
    EXPECT_TRUE(isVoid);

    bool isTypeMappedBy = std::is_same_v<TypeMappedBy, ObjectMapper::GetDefaultMapping<ObjMappedBySpecialization>>;
    EXPECT_TRUE(isTypeMappedBy);
    isTypeMappedBy = std::is_same_v<TypeMappedBy, ObjectMapper::GetDefaultMapping<ObjMappedByMacro>>;
    EXPECT_TRUE(isTypeMappedBy);
    isTypeMappedBy = std::is_same_v<TypeMappedBy, ObjectMapper::GetDefaultMapping<ObjMappedByClassNote>>;
    EXPECT_TRUE(isTypeMappedBy);

    auto classOrFieldNoteWithMapping = std::tuple { ObjectMapper::MappedBy<TypeMappedBy> };
    using ClassOrFieldNoteWithMapping = decltype(classOrFieldNoteWithMapping);
    using OpNoteWithMapping = std::tuple<ObjectMapper::UseMapping<ObjWithoutMapping, TypeMappedBy>>;
    isTypeMappedBy = std::is_same_v<TypeMappedBy, ObjectMapper::GetDefaultMapping<ObjWithoutMapping, ClassOrFieldNoteWithMapping>>;
    EXPECT_TRUE(isTypeMappedBy);
    isTypeMappedBy = std::is_same_v<TypeMappedBy, ObjectMapper::GetDefaultMapping<ObjWithoutMapping, void, OpNoteWithMapping>>;
    EXPECT_TRUE(isTypeMappedBy);
    isTypeMappedBy = std::is_same_v<TypeMappedBy, ObjectMapper::GetDefaultMapping<ObjWithoutMapping, ClassOrFieldNoteWithMapping, OpNoteWithMapping>>;
    EXPECT_TRUE(isTypeMappedBy);
}

struct CopyConstructorTest
{
    CopyConstructorTest(int a, int b) : a(a), b(b) {}
    CopyConstructorTest(const CopyConstructorTest & other) { ObjectMapper::map_default(*this, other); }

    int a = 0;
    int b = 0;

    REFLECT(CopyConstructorTest, a, b)
};

TEST(ObjectMapperAutoMappingTest, CopyConstructors)
{
    CopyConstructorTest cct { 1, 2 };
    auto copy = cct;
    EXPECT_EQ(1, copy.a);
    EXPECT_EQ(2, copy.b);
}

struct AssignmentOperatorTestSrc
{
    int a = 0;
    int b = 0;

    REFLECT(AssignmentOperatorTestSrc, a, b)
};

struct AssignmentOperatorTestDest
{
    void operator=(const AssignmentOperatorTestSrc & src) { ObjectMapper::map_default(*this, src); }

    int a = 0;
    int b = 0;

    REFLECT(AssignmentOperatorTestDest, a, b)
};

TEST(ObjectMapperAutoMappingTest, AssignmentOperator)
{
    AssignmentOperatorTestSrc src {1, 2};
    AssignmentOperatorTestDest dest {};
    dest = src;
    
    EXPECT_EQ(src.a, dest.a);
    EXPECT_EQ(src.b, dest.b);
}

struct MoveAssignmentOperatorTestSrc
{
    int a = 0;
    int b = 0;

    REFLECT(MoveAssignmentOperatorTestSrc, a, b)
};

struct MoveAssignmentOperatorTestDest
{
    // Note that ObjectMapper doesn't use any moves internally
    void operator=(const MoveAssignmentOperatorTestSrc && src) { ObjectMapper::map_default(*this, src); }

    int a = 0;
    int b = 0;

    REFLECT(MoveAssignmentOperatorTestDest, a, b)
};

TEST(ObjectMapperAutoMappingTest, MoveAssignmentOperator)
{
    MoveAssignmentOperatorTestSrc src {1, 2};
    MoveAssignmentOperatorTestDest dest {};
    dest = std::move(src);
    
    EXPECT_EQ(1, dest.a);
    EXPECT_EQ(2, dest.b);
}

struct ConvertingConstructorTestSrc
{
    int a = 0;
    int b = 0;

    REFLECT(ConvertingConstructorTestSrc, a, b)
};

struct ConvertingConstructorTestDest
{
    ConvertingConstructorTestDest(ConvertingConstructorTestSrc & src) { ObjectMapper::map_default(*this, src); }

    int a = 0;
    int b = 0;

    REFLECT(ConvertingConstructorTestDest, a, b)
};

TEST(ObjectMapperAutoMappingTest, ConvertingConstructor)
{
    ConvertingConstructorTestSrc src { 1, 2 };
    ConvertingConstructorTestDest dest(src);
    
    EXPECT_EQ(src.a, dest.a);
    EXPECT_EQ(src.b, dest.b);
}

struct MoveConstructorTestSrc
{
    int a = 0;
    int b = 0;

    REFLECT(MoveConstructorTestSrc, a, b)
};

struct MoveConstructorTestDest
{
    // Note that ObjectMapper doesn't use any moves internally
    MoveConstructorTestDest(MoveConstructorTestSrc && src) { ObjectMapper::map_default(*this, src); }

    int a = 0;
    int b = 0;

    REFLECT(MoveConstructorTestDest, a, b)
};

TEST(ObjectMapperAutoMappingTest, MoveConstructor)
{
    MoveConstructorTestSrc src { 1, 2 };
    MoveConstructorTestDest dest(std::move(src));
    
    EXPECT_EQ(1, dest.a);
    EXPECT_EQ(2, dest.b);
}

struct ConversionOperatorTestDest
{
    int a = 0;
    int b = 0;

    REFLECT(ConversionOperatorTestDest, a, b)
};

struct ConversionOperatorTestSrc
{
    operator ConversionOperatorTestDest() const { return ObjectMapper::map_default<ConversionOperatorTestDest>(*this); }

    int a = 0;
    int b = 0;

    REFLECT(ConversionOperatorTestSrc, a, b)
};

TEST(ObjectMapperAutoMappingTest, ConversionOperator)
{
    ConversionOperatorTestSrc src { 1, 2 };
    ConversionOperatorTestDest dest = src;
    
    EXPECT_EQ(src.a, dest.a);
    EXPECT_EQ(src.b, dest.b);
}

struct ExplicitConversionOperatorTestDest
{

    int a = 0;
    int b = 0;

    REFLECT(ExplicitConversionOperatorTestDest, a, b)
};

struct ExplicitConversionOperatorTestSrc
{
    operator ExplicitConversionOperatorTestDest() const { return ObjectMapper::map_default<ExplicitConversionOperatorTestDest>(*this); }

    int a = 0;
    int b = 0;

    REFLECT(ExplicitConversionOperatorTestSrc, a, b)
};

TEST(ObjectMapperAutoMappingTest, ExplicitConversionOperator)
{
    ExplicitConversionOperatorTestSrc src { 1, 2 };
    ExplicitConversionOperatorTestDest dest = static_cast<ExplicitConversionOperatorTestDest>(src);
    
    EXPECT_EQ(src.a, dest.a);
    EXPECT_EQ(src.b, dest.b);
}
