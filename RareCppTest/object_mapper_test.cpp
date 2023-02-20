#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace Unreflected
{
    struct SourceNoArg {};
    struct DestNoArg {};
    struct Source_intA { int a; };
    struct Dest_intA { int a; };
}

namespace Reflected
{
    NOTE(SourceNoArg) struct SourceNoArg { REFLECT_NOTED(SourceNoArg) };
    NOTE(DestNoArg) struct DestNoArg { REFLECT_NOTED(DestNoArg) };
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

TEST(RareMapperTest, NoMapping)
{
    Unreflected::SourceNoArg srcNoArg_u;
    Unreflected::DestNoArg destNoArg_u;

    EXPECT_NO_THROW(RareMapper::map(srcNoArg_u, destNoArg_u));
    EXPECT_NO_THROW(RareMapper::map(srcNoArg_u, srcNoArg_u));

    Unreflected::Source_intA src_intA_u { 2 };
    Unreflected::Dest_intA dest_intA_u { 3 };

    EXPECT_EQ(src_intA_u.a, 2);
    EXPECT_EQ(dest_intA_u.a, 3);
    EXPECT_NO_THROW(RareMapper::map(srcNoArg_u, dest_intA_u));
    EXPECT_NO_THROW(RareMapper::map(srcNoArg_u, destNoArg_u));
    EXPECT_EQ(src_intA_u.a, 2);
    EXPECT_EQ(dest_intA_u.a, 3);


    Reflected::SourceNoArg srcNoArg_r;
    Reflected::DestNoArg destNoArg_r;

    EXPECT_NO_THROW(RareMapper::map(srcNoArg_r, destNoArg_r));
    EXPECT_NO_THROW(RareMapper::map(srcNoArg_r, srcNoArg_r));

    Reflected::Source_intA src_intA_r { 2 };
    Reflected::Dest_intA dest_intA_r { 3 };

    EXPECT_EQ(src_intA_r.a, 2);
    EXPECT_EQ(dest_intA_r.a, 3);
    EXPECT_NO_THROW(RareMapper::map(srcNoArg_r, dest_intA_r));
    EXPECT_NO_THROW(RareMapper::map(src_intA_r, destNoArg_r));
    EXPECT_EQ(src_intA_r.a, 2);
    EXPECT_EQ(dest_intA_r.a, 3);

    Reflected::Source_stringA source_stringA_r { "asdf" };
    EXPECT_STREQ(source_stringA_r.a.c_str(), "asdf");
    EXPECT_EQ(dest_intA_r.a, 3);
    EXPECT_NO_THROW(RareMapper::map(dest_intA_r, source_stringA_r));
    EXPECT_STREQ(source_stringA_r.a.c_str(), "asdf");
    EXPECT_EQ(dest_intA_r.a, 3);
}

TEST(RareMapperTest, MapParamSimpleReflected)
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

    RareMapper::map(destIntA, srcIntB);
    EXPECT_EQ(srcIntB.b, 2);
    EXPECT_EQ(destIntA.a, 3);

    destIntA = RareMapper::map<Reflected::Dest_intA>(srcIntB);
    EXPECT_EQ(srcIntB.b, 2);
    // Result from RareMapper::map was a Reflected::Dest_intA object with an undefined value for a

    RareMapper::map(destIntA, srcIntA);
    EXPECT_EQ(srcIntA.a, 1);
    EXPECT_EQ(destIntA.a, 1);
    RareMapper::map(destIntA, srcIntAB);
    EXPECT_EQ(srcIntAB.a, 4);
    EXPECT_EQ(srcIntAB.b, 5);
    EXPECT_EQ(destIntA.a, 4);

    Reflected::Dest_intA destIntA2 = RareMapper::map<Reflected::Dest_intA>(srcIntA);
    EXPECT_EQ(srcIntA.a, 1);
    EXPECT_EQ(destIntA2.a, 1);

    RareMapper::map(destIntAB, srcIntAB);
    RareMapper::map(destIntAC, srcIntAB);
    EXPECT_EQ(srcIntAB.a, 4);
    EXPECT_EQ(srcIntAB.b, 5);
    EXPECT_EQ(destIntAB.a, 4);
    EXPECT_EQ(destIntAB.b, 5);
    EXPECT_EQ(destIntAC.a, 4);
    EXPECT_EQ(destIntAC.c, 9);

    RareMapper::map(destIntAB, srcIntA);
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

template <> void RareMapper::map(Unreflected::SpecInt & specInt, const Unreflected::SpecString & specString)
{
    std::stringstream(std::string("2") + specString.a) >> specInt.a;
}

template <> void RareMapper::map(Unreflected::SpecString & specString, const Unreflected::SpecInt & specInt)
{
    specString.a = std::to_string(specInt.a+1);
}

TEST(RareMapperTest, MapSpecializations)
{
    Unreflected::SpecInt specInt { 0 };
    Unreflected::SpecString specString { "0" };
    EXPECT_EQ(specInt.a, 0);
    EXPECT_STREQ(specString.a.c_str(), "0");

    RareMapper::map(specString, specInt);
    EXPECT_EQ(specInt.a, 0);
    EXPECT_STREQ(specString.a.c_str(), "1");

    RareMapper::map(specInt, specString);
    EXPECT_EQ(specInt.a, 21);
    EXPECT_STREQ(specString.a.c_str(), "1");
}

namespace NoteMapped
{
    struct Dest_intB
    {
        int b;

        REFLECT(Dest_intB, b)
    };

    struct Source_intB
    {
        int b;

        REFLECT(Source_intB, b)

        NOTE(ObjectMappings, RareMapper::createMapping<Source_intB, Dest_intB>().b->b().unidirectional())
    };

    struct Dest_intCDEFG
    {
        int c;
        int d;
        int e;
        int f;
        int g;

        REFLECT(Dest_intCDEFG, c, d, e, f, g)
    };

    struct Source_intCDEHI
    {
        int c;
        int d;
        int e;
        int h;
        int i;

        REFLECT(Source_intCDEHI, c, d, e, h, i)

        NOTE(ObjectMappings, RareMapper::createMapping<Source_intCDEHI, Dest_intCDEFG>()
            .c->c()
            .d->d()
            .e->e()
            .h->f()
            .h->g()
            .unidirectional()
        )
    };

    struct Dest_intCDEFG_mapFrom
    {
        int c;
        int d;
        int e;
        int f;
        int g;

        REFLECT(Dest_intCDEFG_mapFrom, c, d, e, f, g)

        NOTE(ObjectMappings, RareMapper::createMapping<Source_intCDEHI, Dest_intCDEFG_mapFrom>()
            .c->c()
            .d->d()
            .e->e()
            .h->f()
            .h->g()
            .unidirectional()
        )
    };

    struct Dest_intCDEFG_mapWith
    {
        int c;
        int d;
        int e;
        int f;
        int g;

        REFLECT(Dest_intCDEFG_mapWith, c, d, e, f, g)

        NOTE(ObjectMappings,
            RareMapper::createMapping<Source_intCDEHI, Dest_intCDEFG_mapWith>().c->c().d->d().e->e().h->f().h->g().unidirectional(),
            RareMapper::createMapping<Dest_intCDEFG_mapWith, Source_intCDEHI>().c->c().d->d().e->e().g->h().unidirectional())
    };
}

TEST(RareMapperTest, MapToFromWithMacros)
{
    NoteMapped::Source_intB srcIntB { 1 };
    NoteMapped::Dest_intB destIntB { 0 };
    EXPECT_EQ(srcIntB.b, 1);
    EXPECT_EQ(destIntB.b, 0);

    RareMapper::map(destIntB, srcIntB);
    EXPECT_EQ(srcIntB.b, 1);
    EXPECT_EQ(destIntB.b, 1);

    NoteMapped::Source_intCDEHI sourceCDEHI { 1, 2, 3, 4, 5 };
    NoteMapped::Dest_intCDEFG destCDEFG { 0, 0, 0, 0, 0 };
    NoteMapped::Dest_intCDEFG_mapFrom destCDEFG_mapFrom { 0, 0, 0, 0, 0 };
    NoteMapped::Dest_intCDEFG_mapWith destCDEFG_mapWith { 0, 0, 0, 0, 0 };
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

    RareMapper::map(destCDEFG, sourceCDEHI);
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

    RareMapper::map(destCDEFG_mapFrom, sourceCDEHI);
    EXPECT_EQ(destCDEFG_mapFrom.c, 1);
    EXPECT_EQ(destCDEFG_mapFrom.d, 2);
    EXPECT_EQ(destCDEFG_mapFrom.e, 3);
    EXPECT_EQ(destCDEFG_mapFrom.f, 4);
    EXPECT_EQ(destCDEFG_mapFrom.g, 4);

    RareMapper::map(destCDEFG_mapWith, sourceCDEHI);
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
    RareMapper::map(sourceCDEHI, destCDEFG_mapWith);
    EXPECT_EQ(sourceCDEHI.c, 6);
    EXPECT_EQ(sourceCDEHI.d, 7);
    EXPECT_EQ(sourceCDEHI.e, 8);
    EXPECT_EQ(sourceCDEHI.h, 99);
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

TEST(RareMapperTest, MapToFromMethods)
{
    Unreflected::Source_intA sourceIntA { 1 };
    Unreflected::MapToFromMethods mapToFromMethods { 0 };
    EXPECT_EQ(sourceIntA.a, 1);
    EXPECT_EQ(mapToFromMethods.a, 0);

    RareMapper::map(mapToFromMethods, sourceIntA);
    EXPECT_EQ(sourceIntA.a, 1);
    EXPECT_EQ(mapToFromMethods.a, 3);

    RareMapper::map(sourceIntA, mapToFromMethods);
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

TEST(RareMapperTest, MapPointables)
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

    RareMapper::map(pointableDest, pointableSrc);

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

TEST(RareMapperTest, MapAssignables)
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
    RareMapper::map(l1, r1); // Assignment operator enables mapping
    EXPECT_EQ(l1.a, 1);
    EXPECT_EQ(r1.a, 1);
    l1.a = 0;
    r1.a = 1;
    RareMapper::map(r1, l1); // No assignment operator for reverse direction/no mapping
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
    RareMapper::map(l3, r3); // Converting-constructor enables assignability/mapping
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

    RareMapper::map(l5, r5); // Conversion op enables assignability/mapping
    EXPECT_EQ(l5.a, 1);
    EXPECT_EQ(r5.a, 1);
}

TEST(RareMapperTest, MapStaticCastAssignables)
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
    RareMapper::map(l3, r3); // Converting-constructor should signal static-cast assignability/enable mapping
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

    RareMapper::map(l5, r5); // Conversion op should signal static_cast assignability/enable mapping
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

    RareMapper::map(l6, r6); // Explicit conversion op should signal static-cast assignability/enable mapping
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

TEST(RareMapperTest, MapPair)
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

    RareMapper::map(pairDestination, pairSource);
    EXPECT_EQ(pairSource.a.first, 1);
    EXPECT_STREQ(pairSource.a.second.c_str(), "asdf");
    EXPECT_EQ(pairDestination.a.first, 1);
    EXPECT_STREQ(pairDestination.a.second.c_str(), "asdf");

    RareMapper::map(pairDestinationHalfMatch, pairSource);
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

TEST(RareMapperTest, MapTuple)
{
    Reflected::TupleSource tupleSource { {1, "asdf", 'b'} };
    Reflected::TupleDestination tupleDestination { {0, "", 'a'} };
    EXPECT_EQ(std::get<0>(tupleSource.a), 1);
    EXPECT_STREQ(std::get<1>(tupleSource.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleSource.a), 'b');
    EXPECT_EQ(std::get<0>(tupleDestination.a), 0);
    EXPECT_STREQ(std::get<1>(tupleDestination.a).c_str(), "");
    EXPECT_EQ(std::get<2>(tupleDestination.a), 'a');

    RareMapper::map(tupleDestination, tupleSource);
    EXPECT_EQ(std::get<0>(tupleSource.a), 1);
    EXPECT_STREQ(std::get<1>(tupleSource.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleSource.a), 'b');
    EXPECT_EQ(std::get<0>(tupleDestination.a), 1);
    EXPECT_STREQ(std::get<1>(tupleDestination.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleDestination.a), 'b');

    Reflected::EmptyTuple emptyTuple {}; // Mapping should have no effect
    EXPECT_NO_THROW(RareMapper::map(emptyTuple, tupleSource));
    EXPECT_NO_THROW(RareMapper::map(tupleDestination, emptyTuple));
    EXPECT_EQ(std::get<0>(tupleSource.a), 1);
    EXPECT_STREQ(std::get<1>(tupleSource.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleSource.a), 'b');
    EXPECT_EQ(std::get<0>(tupleDestination.a), 1);
    EXPECT_STREQ(std::get<1>(tupleDestination.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleDestination.a), 'b');

    Reflected::IntTuple intTuple { {5} }; // Mapping should effect only matching parts of tuple
    EXPECT_EQ(std::get<0>(intTuple.a), 5);
    RareMapper::map(tupleDestination, intTuple);
    EXPECT_EQ(std::get<0>(tupleDestination.a), 5);
    EXPECT_STREQ(std::get<1>(tupleDestination.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleDestination.a), 'b');

    RareMapper::map(intTuple, tupleSource);
    EXPECT_EQ(std::get<0>(intTuple.a), 1);
    EXPECT_EQ(std::get<0>(tupleSource.a), 1);
    EXPECT_STREQ(std::get<1>(tupleSource.a).c_str(), "asdf");
    EXPECT_EQ(std::get<2>(tupleSource.a), 'b');
}

TEST(RareMapperTest, MapIterables)
{
    std::vector<int> srcVec { 0, 2, 4 };
    auto destVec = RareMapper::map<std::vector<int>>(srcVec);

    EXPECT_EQ(size_t(3), destVec.size());
    EXPECT_EQ(0, destVec[0]);
    EXPECT_EQ(2, destVec[1]);
    EXPECT_EQ(4, destVec[2]);

    std::map<int, int> srcMap {{1, 2}, {2, 4}, {3, 6}};
    auto destMap = RareMapper::map<std::map<int, int>>(srcMap);
    EXPECT_EQ(size_t(3), destMap.size());
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
    RareMapper::map(destRay, srcRay);
    EXPECT_EQ(0, destRay[0]);
    EXPECT_EQ(2, destRay[1]);
    EXPECT_EQ(4, destRay[2]);

    auto rayVecDest = RareMapper::map<std::vector<int>>(srcRay);
    EXPECT_EQ(size_t(3), rayVecDest.size());
    EXPECT_EQ(0, rayVecDest[0]);
    EXPECT_EQ(2, rayVecDest[1]);
    EXPECT_EQ(4, rayVecDest[2]);

    destRay[0] = 0;
    destRay[1] = 0;
    destRay[2] = 0;
    RareMapper::map(destRay, srcVec);
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

TEST(RareMapperTest, MapRecurse)
{
    MapSource mapSource { 1, {2}, {3}, {{4}, {5}}, {{6}, {7}} };
    MapDest mapDest { 0, {0}, {""}, {}, {}};

    EXPECT_EQ(mapSource.a, 1);
    EXPECT_EQ(mapSource.b.a, 2);
    EXPECT_EQ(mapSource.c.a, 3);
    EXPECT_EQ(mapSource.d.size(), size_t(2));
    EXPECT_EQ(mapSource.d[0].a, 4);
    EXPECT_EQ(mapSource.d[1].a, 5);
    EXPECT_EQ(mapSource.e.size(), size_t(2));
    EXPECT_EQ(mapSource.e[0].a, 6);
    EXPECT_EQ(mapSource.e[1].a, 7);

    EXPECT_EQ(mapDest.a, 0);
    EXPECT_EQ(mapDest.b.a, 0);
    EXPECT_STREQ(mapDest.c.a.c_str(), "");
    EXPECT_EQ(mapDest.d.size(), size_t(0));
    EXPECT_EQ(mapDest.e.size(), size_t(0));

    RareMapper::map(mapDest, mapSource);

    EXPECT_EQ(mapSource.a, 1);
    EXPECT_EQ(mapSource.b.a, 2);
    EXPECT_EQ(mapSource.c.a, 3);
    EXPECT_EQ(mapSource.d.size(), size_t(2));
    EXPECT_EQ(mapSource.d[0].a, 4);
    EXPECT_EQ(mapSource.d[1].a, 5);
    EXPECT_EQ(mapSource.e.size(), size_t(2));
    EXPECT_EQ(mapSource.e[0].a, 6);
    EXPECT_EQ(mapSource.e[1].a, 7);

    EXPECT_EQ(mapDest.a, 1);
    EXPECT_EQ(mapDest.b.a, 2);
    EXPECT_STREQ(mapDest.c.a.c_str(), "4");
    EXPECT_EQ(mapDest.d.size(), size_t(2));
    EXPECT_EQ(mapDest.d[0].a, 4);
    EXPECT_EQ(mapDest.d[1].a, 5);
    EXPECT_EQ(mapDest.e.size(), size_t(2));
    EXPECT_EQ(mapDest.e[0].a, 6);
    EXPECT_EQ(mapDest.e[1].a, 7);
}

TEST(RareMapperAnnotationsTest, MappedByAnnotation)
{
    auto mappedByNote = RareMapper::MappedBy<int>;
    using MappedByNote = decltype(mappedByNote);
    bool defaultMappingIsInt = std::is_same_v<int, typename MappedByNote::DefaultMapping>;
    EXPECT_TRUE(defaultMappingIsInt);
}

TEST(RareMapperAnnotationsTest, UseMappingOpAnnotation)
{
    using OpUseMapping = RareMapper::UseMapping<int, float>;
    bool objectIsInt = std::is_same_v<int, typename OpUseMapping::Object>;
    EXPECT_TRUE(objectIsInt);
    bool defaultMappingIsFloat = std::is_same_v<float, typename OpUseMapping::DefaultMapping>;
    EXPECT_TRUE(defaultMappingIsFloat);
}

struct TypeMappedBy {};

struct ObjWithoutMapping {};

struct ObjMappedBySpecialization {};
template <> struct RareMapper::SetTags<ObjMappedBySpecialization> : IsMappedBy<TypeMappedBy> {};

struct ObjMappedByMacro {};
SET_DEFAULT_OBJECT_MAPPING(ObjMappedByMacro, TypeMappedBy)

NOTE(ObjMappedByClassNote, RareMapper::MappedBy<TypeMappedBy>)
struct ObjMappedByClassNote { REFLECT_NOTED(ObjMappedByClassNote) };

TEST(RareMapperAnnotationsTest, Tags)
{
    bool isSame = std::is_same_v<TypeMappedBy, RareMapper::GetTags<ObjMappedBySpecialization>::DefaultMapping>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<TypeMappedBy, RareMapper::GetTags<ObjMappedByMacro>::DefaultMapping>;
    EXPECT_TRUE(isSame);
}

TEST(RareMapperAnnotationsTest, HasDefaultMapping)
{
    bool hasDefaultMapping = RareMapper::hasDefaultMapping<TypeMappedBy>;
    EXPECT_FALSE(hasDefaultMapping);
    hasDefaultMapping = RareMapper::hasDefaultMapping<ObjMappedBySpecialization>;
    EXPECT_TRUE(hasDefaultMapping);
    hasDefaultMapping = RareMapper::hasDefaultMapping<ObjMappedByMacro>;
    EXPECT_TRUE(hasDefaultMapping);
    hasDefaultMapping = RareMapper::hasDefaultMapping<ObjMappedByClassNote>;
    EXPECT_TRUE(hasDefaultMapping);

    auto classOrMemberNoteWithMapping = std::tuple { RareMapper::MappedBy<TypeMappedBy> };
    using ClassOrMemberNoteWithMapping = decltype(classOrMemberNoteWithMapping);
    using OpNoteWithMapping = std::tuple<RareMapper::UseMapping<ObjWithoutMapping, TypeMappedBy>>;
    hasDefaultMapping = RareMapper::hasDefaultMapping<ObjWithoutMapping, ClassOrMemberNoteWithMapping>;
    EXPECT_TRUE(hasDefaultMapping);
    hasDefaultMapping = RareMapper::hasDefaultMapping<ObjWithoutMapping, void, OpNoteWithMapping>;
    EXPECT_TRUE(hasDefaultMapping);
    hasDefaultMapping = RareMapper::hasDefaultMapping<ObjWithoutMapping, ClassOrMemberNoteWithMapping, OpNoteWithMapping>;
    EXPECT_TRUE(hasDefaultMapping);
}

TEST(RareMapperAnnotationsTest, GetDefaultMapping)
{
    bool isVoid = std::is_same_v<void, RareMapper::GetDefaultMapping<TypeMappedBy>>;
    EXPECT_TRUE(isVoid);

    bool isTypeMappedBy = std::is_same_v<TypeMappedBy, RareMapper::GetDefaultMapping<ObjMappedBySpecialization>>;
    EXPECT_TRUE(isTypeMappedBy);
    isTypeMappedBy = std::is_same_v<TypeMappedBy, RareMapper::GetDefaultMapping<ObjMappedByMacro>>;
    EXPECT_TRUE(isTypeMappedBy);
    isTypeMappedBy = std::is_same_v<TypeMappedBy, RareMapper::GetDefaultMapping<ObjMappedByClassNote>>;
    EXPECT_TRUE(isTypeMappedBy);

    auto classOrMemberNoteWithMapping = std::tuple { RareMapper::MappedBy<TypeMappedBy> };
    using ClassOrMemberNoteWithMapping = decltype(classOrMemberNoteWithMapping);
    using OpNoteWithMapping = std::tuple<RareMapper::UseMapping<ObjWithoutMapping, TypeMappedBy>>;
    isTypeMappedBy = std::is_same_v<TypeMappedBy, RareMapper::GetDefaultMapping<ObjWithoutMapping, ClassOrMemberNoteWithMapping>>;
    EXPECT_TRUE(isTypeMappedBy);
    isTypeMappedBy = std::is_same_v<TypeMappedBy, RareMapper::GetDefaultMapping<ObjWithoutMapping, void, OpNoteWithMapping>>;
    EXPECT_TRUE(isTypeMappedBy);
    isTypeMappedBy = std::is_same_v<TypeMappedBy, RareMapper::GetDefaultMapping<ObjWithoutMapping, ClassOrMemberNoteWithMapping, OpNoteWithMapping>>;
    EXPECT_TRUE(isTypeMappedBy);
}

struct CopyConstructorTest
{

    int a = 0;
    int b = 0;

    REFLECT(CopyConstructorTest, a, b)

    CopyConstructorTest(int a, int b) : a(a), b(b) {}
    CopyConstructorTest(const CopyConstructorTest & other) { RareMapper::mapDefault(*this, other); }
};

TEST(RareMapperAutoMappingTest, CopyConstructors)
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

    int a = 0;
    int b = 0;

    REFLECT(AssignmentOperatorTestDest, a, b)

    void operator=(const AssignmentOperatorTestSrc & src) { RareMapper::mapDefault(*this, src); }
};

TEST(RareMapperAutoMappingTest, AssignmentOperator)
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
    int a = 0;
    int b = 0;

    REFLECT(MoveAssignmentOperatorTestDest, a, b)

    // Note that RareMapper doesn't use any moves internally
    void operator=(const MoveAssignmentOperatorTestSrc && src) { RareMapper::mapDefault(*this, src); }
};

TEST(RareMapperAutoMappingTest, MoveAssignmentOperator)
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
    int a = 0;
    int b = 0;

    REFLECT(ConvertingConstructorTestDest, a, b)

    ConvertingConstructorTestDest(ConvertingConstructorTestSrc & src) { RareMapper::mapDefault(*this, src); }
};

TEST(RareMapperAutoMappingTest, ConvertingConstructor)
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
    int a = 0;
    int b = 0;

    REFLECT(MoveConstructorTestDest, a, b)

    // Note that RareMapper doesn't use any moves internally
    MoveConstructorTestDest(MoveConstructorTestSrc && src) { RareMapper::mapDefault(*this, src); }
};

TEST(RareMapperAutoMappingTest, MoveConstructor)
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
    int a = 0;
    int b = 0;

    REFLECT(ConversionOperatorTestSrc, a, b)

    operator ConversionOperatorTestDest() const { return RareMapper::mapDefault<ConversionOperatorTestDest>(*this); }
};

TEST(RareMapperAutoMappingTest, ConversionOperator)
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
    int a = 0;
    int b = 0;

    REFLECT(ExplicitConversionOperatorTestSrc, a, b)

    operator ExplicitConversionOperatorTestDest() const { return RareMapper::mapDefault<ExplicitConversionOperatorTestDest>(*this); }
};

TEST(RareMapperAutoMappingTest, ExplicitConversionOperator)
{
    ExplicitConversionOperatorTestSrc src { 1, 2 };
    ExplicitConversionOperatorTestDest dest = static_cast<ExplicitConversionOperatorTestDest>(src);
    
    EXPECT_EQ(src.a, dest.a);
    EXPECT_EQ(src.b, dest.b);
}
