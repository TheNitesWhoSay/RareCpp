#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#ifndef RARE_NO_CPP_20

namespace ReflectAggregateTest {

struct Empty {};
struct Int { int a; };
struct Ref { int & a; };
struct IntInt { int a; int b; };
struct IntRef { int a; int & b; };
struct RefInt { int & a; int b; };
struct RefRef { int & a; int & b; };
struct Composed { Int a; Int b; };

class Priv { int a; public: int getA() { return a; } };
class Prot { protected: int a; };
struct Construct { Construct() {} };
struct NonInheritedConstruct : Construct {};
struct InheritedConstruct : Construct { using Construct::Construct; };
struct IndirectPriv : Priv {};
class PrivInherit : Empty {};
struct IndirectPrivInherit : PrivInherit {};
struct Virtual { virtual ~Virtual() {} };
struct InheritedVirtual : Virtual {};

TEST(AggregateTest, StdIsAggregate)
{
    EXPECT_FALSE(std::is_aggregate_v<void>);
    EXPECT_FALSE(std::is_aggregate_v<int>);
    EXPECT_FALSE(std::is_aggregate_v<int &>);
    EXPECT_FALSE(std::is_aggregate_v<int &&>);
    EXPECT_FALSE(std::is_aggregate_v<const int>);
    EXPECT_FALSE(std::is_aggregate_v<const int &>);
    EXPECT_FALSE(std::is_aggregate_v<const int && >);
    EXPECT_FALSE(std::is_aggregate_v<int*>);
    EXPECT_FALSE(std::is_aggregate_v<int* const>);
    EXPECT_FALSE(std::is_aggregate_v<const int*>);
    EXPECT_FALSE(std::is_aggregate_v<const int* const>);

    EXPECT_TRUE(std::is_aggregate_v<int[]>);
    EXPECT_TRUE(std::is_aggregate_v<int[1]>);
    EXPECT_TRUE(std::is_aggregate_v<int[2]>);

    EXPECT_TRUE(std::is_aggregate_v<Empty>);
    EXPECT_TRUE(std::is_aggregate_v<Int>);
    EXPECT_TRUE(std::is_aggregate_v<Ref>);
    EXPECT_TRUE(std::is_aggregate_v<IntInt>);
    EXPECT_TRUE(std::is_aggregate_v<IntRef>);
    EXPECT_TRUE(std::is_aggregate_v<RefInt>);
    EXPECT_TRUE(std::is_aggregate_v<RefRef>);
    EXPECT_TRUE(std::is_aggregate_v<Composed>);

    EXPECT_FALSE(std::is_aggregate_v<Priv>);
    EXPECT_FALSE(std::is_aggregate_v<Prot>);
    EXPECT_FALSE(std::is_aggregate_v<Construct>);
    EXPECT_TRUE(std::is_aggregate_v<NonInheritedConstruct>);
    EXPECT_FALSE(std::is_aggregate_v<InheritedConstruct>);
    EXPECT_TRUE(std::is_aggregate_v<IndirectPriv>);
    EXPECT_FALSE(std::is_aggregate_v<PrivInherit>);
    EXPECT_TRUE(std::is_aggregate_v<IndirectPrivInherit>);
    EXPECT_FALSE(std::is_aggregate_v<Virtual>);
    EXPECT_FALSE(std::is_aggregate_v<InheritedVirtual>);
}

TEST(AggregateTest, StructuredBindable)
{
    int one = 1;
    Int int_ {2};
    Ref ref_ {one};
    IntInt intInt {3, 4};
    IntRef intRef {5, one};
    RefInt refInt {one, 6};
    RefRef refRef {one, one};
    Composed composed {{2}, {2}};

    //auto [] = one; // No
    //auto [e] = one; // No
    //auto & [e] = one; // No

    //Empty empty {};
    //auto [] = empty; // No
    //auto [e] = empty; // No
    //auto & [e] = empty; // No

    {
        auto [a] = int_;
        EXPECT_EQ(2, a);
        a = 3;
        EXPECT_EQ(2, int_.a);
        EXPECT_EQ(3, a);
    }

    {
        auto & [a] = int_;
        EXPECT_EQ(2, a);
        a = 3;
        EXPECT_EQ(3, int_.a);
        EXPECT_EQ(3, a);
        int_.a = 2;
        EXPECT_EQ(2, a);
    }

    { auto [a] = int_; EXPECT_EQ(a, 2); }
    { auto [a] = ref_; EXPECT_EQ(a, 1); }
    { auto [a, b] = intInt; EXPECT_EQ(a, 3); EXPECT_EQ(b, 4); }
    { auto [a, b] = intRef; EXPECT_EQ(a, 5); EXPECT_EQ(b, 1); }
    { auto [a, b] = refInt; EXPECT_EQ(a, 1); EXPECT_EQ(b, 6); }
    { auto [a, b] = refRef; EXPECT_EQ(a, 1); EXPECT_EQ(b, 1); }
    { auto [a, b] = composed; EXPECT_EQ(a.a, 2); EXPECT_EQ(b.a, 2); }

    { auto & [a] = int_; EXPECT_EQ(a, 2); }
    { auto & [a] = ref_; EXPECT_EQ(a, 1); }
    { auto & [a, b] = intInt; EXPECT_EQ(a, 3); EXPECT_EQ(b, 4); }
    { auto & [a, b] = intRef; EXPECT_EQ(a, 5); EXPECT_EQ(b, 1); }
    { auto & [a, b] = refInt; EXPECT_EQ(a, 1); EXPECT_EQ(b, 6); }
    { auto & [a, b] = refRef; EXPECT_EQ(a, 1); EXPECT_EQ(b, 1); }
    { auto & [a, b] = composed; EXPECT_EQ(a.a, 2); EXPECT_EQ(b.a, 2); }
}

TEST(AggregateTest, UnrefTupleElements)
{
    bool isSame = std::is_same_v<std::tuple<>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, float>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int, float>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int &>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, float>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int &, float>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, float>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int, float &>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, float>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int &, float &>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, float>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int &&, float>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, float>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int, float &&>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, float>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int &&, float &&>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char*>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<char*>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<const char*>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<const char*>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<char* const>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<char* const>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<const char* const>, RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<const char* const>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, float, const char* const, std::string, Empty>,
        RareTs::Aggregates::detail::unref_tuple_elements_t<std::tuple<int, float &&, const char* const &, std::string, Empty &>>>;
    EXPECT_TRUE(isSame);
}

TEST(AggregateTest, StoredName)
{
    auto emptyStorage = RareTs::Aggregates::detail::stored_name<0>{""};
    EXPECT_EQ('\0', emptyStorage.value[0]);
    EXPECT_EQ(std::string_view{""}, (const char*)emptyStorage.value);
    auto a = RareTs::Aggregates::detail::stored_name<1>{"a"};
    EXPECT_EQ('a', a.value[0]);
    EXPECT_EQ('\0', a.value[1]);
    EXPECT_EQ(std::string_view{"a"}, (const char*)a.value);
    auto ab = RareTs::Aggregates::detail::stored_name<2>{"ab"};
    EXPECT_EQ('a', ab.value[0]);
    EXPECT_EQ('b', ab.value[1]);
    EXPECT_EQ('\0', ab.value[2]);
    EXPECT_EQ(std::string_view{"ab"}, (const char*)ab.value);

    constexpr auto constexprEmptyStorage = RareTs::Aggregates::detail::stored_name<0>{""};
    static_assert('\0' == constexprEmptyStorage.value[0]);
    static_assert(std::string_view{""} == (const char*)constexprEmptyStorage.value);
    constexpr auto constexprA = RareTs::Aggregates::detail::stored_name<1>{"a"};
    static_assert('a' == constexprA.value[0]);
    static_assert('\0' == constexprA.value[1]);
    static_assert(std::string_view{"a"} == (const char*)constexprA.value);
    constexpr auto constexprAB = RareTs::Aggregates::detail::stored_name<2>{"ab"};
    static_assert('a' == constexprAB.value[0]);
    static_assert('b' == constexprAB.value[1]);
    static_assert('\0' == constexprAB.value[2]);
    static_assert(std::string_view{"ab"} == (const char*)constexprAB.value);
}

struct EmptyBase : Empty {};
struct OneBaseMember : Int {};
struct TwoBaseMembers : IntInt {};
struct TwoBases : Int, Empty {};
struct TwoBasesThreeMembers : Int, IntInt {};
struct FiveBases : EmptyBase, OneBaseMember, TwoBaseMembers, TwoBases, TwoBasesThreeMembers {};

struct One { int a; };
struct Two { int a; int b; };
struct Three { int a; int b; int c; };
struct OneRay { int a[2]; };
struct TwoRay { int a[2]; int b[2]; };
struct ThreeRay { int a[2]; int b[2]; int c[2]; };
struct OneObj { Empty a; };
struct TwoObj { Empty a; Empty b; };
struct ThreeObj { Empty a; Empty b; Empty c; };
struct OneObjRay { Empty a[2]; };
struct TwoObjRay { Empty a[2]; Empty b[2]; };
struct ThreeObjRay { Empty a[2]; Empty b[2]; Empty c[2]; };

struct HT1 { int a[12]; int b; int c; int d; };
struct HT2 { int a; int b; int c; int d[12]; };
struct HT3 { Empty a[12]; int b; int c; int d; };
struct HT4 { int a[12]; int b; int c; Empty d[12]; };
struct HT5 { Empty a[12]; int b[12]; Empty c; int d; Empty e[12]; };

TEST(AggregateTest, MemberCount)
{
    EXPECT_EQ(0, RareTs::Aggregates::member_count<Empty>);
    EXPECT_EQ(1, RareTs::Aggregates::member_count<EmptyBase>);
    EXPECT_EQ(1, RareTs::Aggregates::member_count<Int>);
    EXPECT_EQ(2, RareTs::Aggregates::member_count<IntInt>);
    EXPECT_EQ(1, RareTs::Aggregates::member_count<OneBaseMember>);
    EXPECT_EQ(1, RareTs::Aggregates::member_count<TwoBaseMembers>);
    EXPECT_EQ(2, RareTs::Aggregates::member_count<TwoBases>);
    EXPECT_EQ(2, RareTs::Aggregates::member_count<TwoBasesThreeMembers>);
    EXPECT_EQ(5, RareTs::Aggregates::member_count<FiveBases>);

    EXPECT_EQ(1, RareTs::Aggregates::member_count<One>);
    EXPECT_EQ(2, RareTs::Aggregates::member_count<Two>);
    EXPECT_EQ(3, RareTs::Aggregates::member_count<Three>);
    EXPECT_EQ(1, RareTs::Aggregates::member_count<OneRay>);
    EXPECT_EQ(2, RareTs::Aggregates::member_count<TwoRay>);
    EXPECT_EQ(3, RareTs::Aggregates::member_count<ThreeRay>);
    EXPECT_EQ(1, RareTs::Aggregates::member_count<OneObj>);
    EXPECT_EQ(2, RareTs::Aggregates::member_count<TwoObj>);
    EXPECT_EQ(3, RareTs::Aggregates::member_count<ThreeObj>);
    EXPECT_EQ(1, RareTs::Aggregates::member_count<OneObjRay>);
    EXPECT_EQ(2, RareTs::Aggregates::member_count<TwoObjRay>);
    EXPECT_EQ(3, RareTs::Aggregates::member_count<ThreeObjRay>);

    EXPECT_EQ(4, RareTs::Aggregates::member_count<HT1>);
    EXPECT_EQ(4, RareTs::Aggregates::member_count<HT2>);
    EXPECT_EQ(4, RareTs::Aggregates::member_count<HT3>);
    EXPECT_EQ(4, RareTs::Aggregates::member_count<HT4>);
    EXPECT_EQ(5, RareTs::Aggregates::member_count<HT5>);

    #ifndef __INTELLISENSE__ // Intellisense can't process member counts and ergo most of this section correctly
    static_assert(0 == RareTs::Aggregates::member_count<Empty>);
    static_assert(1 == RareTs::Aggregates::member_count<EmptyBase>);
    static_assert(1 == RareTs::Aggregates::member_count<Int>);
    static_assert(2 == RareTs::Aggregates::member_count<IntInt>);
    static_assert(1 == RareTs::Aggregates::member_count<OneBaseMember>);
    static_assert(1 == RareTs::Aggregates::member_count<TwoBaseMembers>);
    static_assert(2 == RareTs::Aggregates::member_count<TwoBases>);
    static_assert(2 == RareTs::Aggregates::member_count<TwoBasesThreeMembers>);
    static_assert(5 == RareTs::Aggregates::member_count<FiveBases>);

    static_assert(1 == RareTs::Aggregates::member_count<One>);
    static_assert(2 == RareTs::Aggregates::member_count<Two>);
    static_assert(3 == RareTs::Aggregates::member_count<Three>);
    static_assert(1 == RareTs::Aggregates::member_count<OneRay>);
    static_assert(2 == RareTs::Aggregates::member_count<TwoRay>);
    static_assert(3 == RareTs::Aggregates::member_count<ThreeRay>);
    static_assert(1 == RareTs::Aggregates::member_count<OneObj>);
    static_assert(2 == RareTs::Aggregates::member_count<TwoObj>);
    static_assert(3 == RareTs::Aggregates::member_count<ThreeObj>);
    static_assert(1 == RareTs::Aggregates::member_count<OneObjRay>);
    static_assert(2 == RareTs::Aggregates::member_count<TwoObjRay>);
    static_assert(3 == RareTs::Aggregates::member_count<ThreeObjRay>);

    static_assert(4 == RareTs::Aggregates::member_count<HT1>);
    static_assert(4 == RareTs::Aggregates::member_count<HT2>);
    static_assert(4 == RareTs::Aggregates::member_count<HT3>);
    static_assert(4 == RareTs::Aggregates::member_count<HT4>);
    static_assert(5 == RareTs::Aggregates::member_count<HT5>);
    #endif
}

TEST(AggregateTest, MembersOf)
{
    #ifndef __INTELLISENSE__ // Intellisense can't process member counts and ergo most of this section correctly
    bool isSame = std::is_same_v<std::tuple<>, decltype(RareTs::Aggregates::members_of(Empty{}))>;
    EXPECT_TRUE(isSame);
    // Note: the counts for aggregates inheriting from other types tends *not* to match the count of structured binding arguments
    // it may be worth improving the counter in the future, but for now aggregates with base classes are off limits
    isSame = std::is_same_v<std::tuple<int &>, decltype(RareTs::Aggregates::members_of(One{}))>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::get<0>(RareTs::Aggregates::members_of(One{1})));
    isSame = std::is_same_v<std::tuple<int &, int &>, decltype(RareTs::Aggregates::members_of(Two{}))>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::get<0>(RareTs::Aggregates::members_of(Two{1, 2})));
    EXPECT_EQ(2, std::get<1>(RareTs::Aggregates::members_of(Two{1, 2})));
    isSame = std::is_same_v<std::tuple<int &, int &, int &>, decltype(RareTs::Aggregates::members_of(Three{}))>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::get<0>(RareTs::Aggregates::members_of(Three{1, 2, 3})));
    EXPECT_EQ(2, std::get<1>(RareTs::Aggregates::members_of(Three{1, 2, 3})));
    EXPECT_EQ(3, std::get<2>(RareTs::Aggregates::members_of(Three{1, 2, 3})));

    isSame = std::is_same_v<std::tuple<int (&)[2]>, decltype(RareTs::Aggregates::members_of(OneRay{}))>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::get<0>(RareTs::Aggregates::members_of(OneRay{{1, 2}}))[0]);
    EXPECT_EQ(2, std::get<0>(RareTs::Aggregates::members_of(OneRay{{1, 2}}))[1]);
    isSame = std::is_same_v<std::tuple<int (&)[2], int (&)[2]>, decltype(RareTs::Aggregates::members_of(TwoRay{}))>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::get<0>(RareTs::Aggregates::members_of(TwoRay{{1, 2},{3, 4}}))[0]);
    EXPECT_EQ(2, std::get<0>(RareTs::Aggregates::members_of(TwoRay{{1, 2},{3, 4}}))[1]);
    EXPECT_EQ(3, std::get<1>(RareTs::Aggregates::members_of(TwoRay{{1, 2},{3, 4}}))[0]);
    EXPECT_EQ(4, std::get<1>(RareTs::Aggregates::members_of(TwoRay{{1, 2},{3, 4}}))[1]);

    isSame = std::is_same_v<std::tuple<Empty (&)[12], int (&)[12], Empty &, int &, Empty (&)[12]>, decltype(RareTs::Aggregates::members_of(HT5{}))>;
    EXPECT_TRUE(isSame);
    HT5 ht5 {
        {},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        {},
        13,
        {}
    };
    auto ht5Members = RareTs::Aggregates::members_of(ht5);
    EXPECT_EQ(1, std::get<1>(ht5Members)[0]);
    EXPECT_EQ(6, std::get<1>(ht5Members)[5]);
    EXPECT_EQ(12, std::get<1>(ht5Members)[11]);
    EXPECT_EQ(13, std::get<3>(ht5Members));

    static_assert(std::is_same_v<std::tuple<>, decltype(RareTs::Aggregates::members_of(Empty{}))>);
    static_assert(std::is_same_v<std::tuple<int &>, decltype(RareTs::Aggregates::members_of(One{}))>);
    static_assert(1 == std::get<0>(RareTs::Aggregates::members_of(One{1})));
    static_assert(std::is_same_v<std::tuple<int &, int &>, decltype(RareTs::Aggregates::members_of(Two{}))>);
    static_assert(1 == std::get<0>(RareTs::Aggregates::members_of(Two{1, 2})));
    static_assert(2 == std::get<1>(RareTs::Aggregates::members_of(Two{1, 2})));
    static_assert(std::is_same_v<std::tuple<int &, int &, int &>, decltype(RareTs::Aggregates::members_of(Three{}))>);
    static_assert(1 == std::get<0>(RareTs::Aggregates::members_of(Three{1, 2, 3})));
    static_assert(2 == std::get<1>(RareTs::Aggregates::members_of(Three{1, 2, 3})));
    static_assert(3 == std::get<2>(RareTs::Aggregates::members_of(Three{1, 2, 3})));

    static_assert(std::is_same_v<std::tuple<int (&)[2]>, decltype(RareTs::Aggregates::members_of(OneRay{}))>);
    static_assert(1 == std::get<0>(RareTs::Aggregates::members_of(OneRay{{1, 2}}))[0]);
    static_assert(2 == std::get<0>(RareTs::Aggregates::members_of(OneRay{{1, 2}}))[1]);
    static_assert(std::is_same_v<std::tuple<int (&)[2], int (&)[2]>, decltype(RareTs::Aggregates::members_of(TwoRay{}))>);
    static_assert(1 == std::get<0>(RareTs::Aggregates::members_of(TwoRay{{1, 2},{3, 4}}))[0]);
    static_assert(2 == std::get<0>(RareTs::Aggregates::members_of(TwoRay{{1, 2},{3, 4}}))[1]);
    static_assert(3 == std::get<1>(RareTs::Aggregates::members_of(TwoRay{{1, 2},{3, 4}}))[0]);
    static_assert(4 == std::get<1>(RareTs::Aggregates::members_of(TwoRay{{1, 2},{3, 4}}))[1]);

    static_assert(std::is_same_v<std::tuple<Empty (&)[12], int (&)[12], Empty &, int &, Empty (&)[12]>, decltype(RareTs::Aggregates::members_of(HT5{}))>);
    static constexpr HT5 constexprHt5 {
        {},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        {},
        13,
        {}
    };
    static constexpr auto constexprHt5Members = RareTs::Aggregates::members_of(constexprHt5);
    static_assert(1 == std::get<1>(constexprHt5Members)[0]);
    static_assert(6 == std::get<1>(constexprHt5Members)[5]);
    static_assert(12 == std::get<1>(constexprHt5Members)[11]);
    static_assert(13 == std::get<3>(constexprHt5Members));
    #endif
}

TEST(AggregateTest, MemberTypes)
{
    bool isSame = std::is_same_v<std::tuple<>, RareTs::Aggregates::member_types<Empty>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<std::tuple<int>, RareTs::Aggregates::member_types<One>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, int>, RareTs::Aggregates::member_types<Two>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, int, int>, RareTs::Aggregates::member_types<Three>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<std::tuple<int[2]>, RareTs::Aggregates::member_types<OneRay>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int[2], int[2]>, RareTs::Aggregates::member_types<TwoRay>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<std::tuple<Empty[12], int[12], Empty, int, Empty[12]>, RareTs::Aggregates::member_types<HT5>>;
    EXPECT_TRUE(isSame);

    #ifndef __INTELLISENSE__ // Intellisense can't process member counts and ergo most of this section correctly
    static_assert(std::is_same_v<std::tuple<>, RareTs::Aggregates::member_types<Empty>>);

    static_assert(std::is_same_v<std::tuple<int>, RareTs::Aggregates::member_types<One>>);
    static_assert(std::is_same_v<std::tuple<int, int>, RareTs::Aggregates::member_types<Two>>);
    static_assert(std::is_same_v<std::tuple<int, int, int>, RareTs::Aggregates::member_types<Three>>);

    static_assert(std::is_same_v<std::tuple<int[2]>, RareTs::Aggregates::member_types<OneRay>>);
    static_assert(std::is_same_v<std::tuple<int[2], int[2]>, RareTs::Aggregates::member_types<TwoRay>>);

    static_assert(std::is_same_v<std::tuple<Empty[12], int[12], Empty, int, Empty[12]>, RareTs::Aggregates::member_types<HT5>>);
    #endif
}

TEST(AggregateTest, MemberType)
{
    bool isSame = std::is_same_v<int, RareTs::Aggregates::member_type<0, One>>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<int[12], RareTs::Aggregates::member_type<0, HT4>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::Aggregates::member_type<1, HT4>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::Aggregates::member_type<2, HT4>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Empty[12], RareTs::Aggregates::member_type<3, HT4>>;
    EXPECT_TRUE(isSame);

    #ifndef __INTELLISENSE__ // Intellisense can't process member counts and ergo most of this section correctly
    static_assert(std::is_same_v<int, RareTs::Aggregates::member_type<0, One>>);

    static_assert(std::is_same_v<int[12], RareTs::Aggregates::member_type<0, HT4>>);
    static_assert(std::is_same_v<int, RareTs::Aggregates::member_type<1, HT4>>);
    static_assert(std::is_same_v<int, RareTs::Aggregates::member_type<2, HT4>>);
    static_assert(std::is_same_v<Empty[12], RareTs::Aggregates::member_type<3, HT4>>);
    #endif
}

TEST(AggregateTest, MemberRef)
{
    bool isSame = std::is_same_v<int &, decltype(RareTs::Aggregates::member_ref<0>(One{}))>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, RareTs::Aggregates::member_ref<0>(One{1}));
    isSame = std::is_same_v<int &, decltype(RareTs::Aggregates::member_ref<0>(Two{}))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int &, decltype(RareTs::Aggregates::member_ref<1>(Two{}))>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, RareTs::Aggregates::member_ref<0>(Two{1, 2}));
    EXPECT_EQ(2, RareTs::Aggregates::member_ref<1>(Two{1, 2}));

    isSame = std::is_same_v<int (&)[2], decltype(RareTs::Aggregates::member_ref<0>(TwoRay{}))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int (&)[2], decltype(RareTs::Aggregates::member_ref<1>(TwoRay{}))>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, RareTs::Aggregates::member_ref<0>(TwoRay{{1, 2},{3, 4}})[0]);
    EXPECT_EQ(2, RareTs::Aggregates::member_ref<0>(TwoRay{{1, 2},{3, 4}})[1]);
    EXPECT_EQ(3, RareTs::Aggregates::member_ref<1>(TwoRay{{1, 2},{3, 4}})[0]);
    EXPECT_EQ(4, RareTs::Aggregates::member_ref<1>(TwoRay{{1, 2},{3, 4}})[1]);

    isSame = std::is_same_v<Empty (&)[12], decltype(RareTs::Aggregates::member_ref<0>(HT5{}))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int (&)[12], decltype(RareTs::Aggregates::member_ref<1>(HT5{}))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Empty &, decltype(RareTs::Aggregates::member_ref<2>(HT5{}))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int &, decltype(RareTs::Aggregates::member_ref<3>(HT5{}))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<Empty (&)[12], decltype(RareTs::Aggregates::member_ref<4>(HT5{}))>;
    EXPECT_TRUE(isSame);
    HT5 ht5{
        {},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        {},
        13,
        {}
    };
    EXPECT_EQ(1, RareTs::Aggregates::member_ref<1>(ht5)[0]);
    EXPECT_EQ(6, RareTs::Aggregates::member_ref<1>(ht5)[5]);
    EXPECT_EQ(12, RareTs::Aggregates::member_ref<1>(ht5)[11]);
    EXPECT_EQ(13, RareTs::Aggregates::member_ref<3>(ht5));

    #ifndef __INTELLISENSE__ // Intellisense can't process member counts and ergo most of this section correctly
    static_assert(std::is_same_v<int &, decltype(RareTs::Aggregates::member_ref<0>(One{}))>);
    static_assert(1 == RareTs::Aggregates::member_ref<0>(One{1}));
    static_assert(std::is_same_v<int &, decltype(RareTs::Aggregates::member_ref<0>(Two{}))>);
    static_assert(std::is_same_v<int &, decltype(RareTs::Aggregates::member_ref<1>(Two{}))>);
    static_assert(1 == RareTs::Aggregates::member_ref<0>(Two{1, 2}));
    static_assert(2 == RareTs::Aggregates::member_ref<1>(Two{1, 2}));

    static_assert(std::is_same_v<int (&)[2], decltype(RareTs::Aggregates::member_ref<0>(TwoRay{}))>);
    static_assert(std::is_same_v<int (&)[2], decltype(RareTs::Aggregates::member_ref<1>(TwoRay{}))>);
    static_assert(1 == RareTs::Aggregates::member_ref<0>(TwoRay{{1, 2},{3, 4}})[0]);
    static_assert(2 == RareTs::Aggregates::member_ref<0>(TwoRay{{1, 2},{3, 4}})[1]);
    static_assert(3 == RareTs::Aggregates::member_ref<1>(TwoRay{{1, 2},{3, 4}})[0]);
    static_assert(4 == RareTs::Aggregates::member_ref<1>(TwoRay{{1, 2},{3, 4}})[1]);

    static_assert(std::is_same_v<Empty (&)[12], decltype(RareTs::Aggregates::member_ref<0>(HT5{}))>);
    static_assert(std::is_same_v<int (&)[12], decltype(RareTs::Aggregates::member_ref<1>(HT5{}))>);
    static_assert(std::is_same_v<Empty &, decltype(RareTs::Aggregates::member_ref<2>(HT5{}))>);
    static_assert(std::is_same_v<int &, decltype(RareTs::Aggregates::member_ref<3>(HT5{}))>);
    static_assert(std::is_same_v<Empty (&)[12], decltype(RareTs::Aggregates::member_ref<4>(HT5{}))>);
    constexpr HT5 constexprHt5{
        {},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        {},
        13,
        {}
    };
    static_assert(1 == RareTs::Aggregates::member_ref<1>(constexprHt5)[0]);
    static_assert(6 == RareTs::Aggregates::member_ref<1>(constexprHt5)[5]);
    static_assert(12 == RareTs::Aggregates::member_ref<1>(constexprHt5)[11]);
    static_assert(13 == RareTs::Aggregates::member_ref<3>(constexprHt5));
    #endif
}

struct NameA { int a; };
struct NameAB { int ab; };
struct NameAB_C { int ab; int c; };
struct ComplexName { int _sOME_fairly_long_complex_012345_identifier_; };
struct VariousNames { int d; int _sOME_fairly_long_complex_012345_identifier_; int ef; int qwerty; };

TEST(AggregateTest, MemberName)
{
    bool isSame = std::string_view{"a"} == RareTs::Aggregates::member_name<0, NameA>;
    EXPECT_TRUE(isSame);
    isSame = std::string_view{"ab"} == RareTs::Aggregates::member_name<0, NameAB>;
    EXPECT_TRUE(isSame);
    isSame = std::string_view{"ab"} == RareTs::Aggregates::member_name<0, NameAB_C>;
    EXPECT_TRUE(isSame);
    isSame = std::string_view{"c"} == RareTs::Aggregates::member_name<1, NameAB_C>;
    EXPECT_TRUE(isSame);
    isSame = std::string_view{"_sOME_fairly_long_complex_012345_identifier_"} == RareTs::Aggregates::member_name<0, ComplexName>;
    EXPECT_TRUE(isSame);
    isSame = std::string_view{"d"} == RareTs::Aggregates::member_name<0, VariousNames>;
    EXPECT_TRUE(isSame);
    isSame = std::string_view{"_sOME_fairly_long_complex_012345_identifier_"} == RareTs::Aggregates::member_name<1, VariousNames>;
    EXPECT_TRUE(isSame);
    isSame = std::string_view{"ef"} == RareTs::Aggregates::member_name<2, VariousNames>;
    EXPECT_TRUE(isSame);
    isSame = std::string_view{"qwerty"} == RareTs::Aggregates::member_name<3, VariousNames>;
    EXPECT_TRUE(isSame);

    static_assert(std::string_view{"a"} == RareTs::Aggregates::member_name<0, NameA>);
    static_assert(std::string_view{"ab"} == RareTs::Aggregates::member_name<0, NameAB>);
    static_assert(std::string_view{"ab"} == RareTs::Aggregates::member_name<0, NameAB_C>);
    static_assert(std::string_view{"c"} == RareTs::Aggregates::member_name<1, NameAB_C>);
    static_assert(std::string_view{"_sOME_fairly_long_complex_012345_identifier_"} == RareTs::Aggregates::member_name<0, ComplexName>);
    static_assert(std::string_view{"d"} == RareTs::Aggregates::member_name<0, VariousNames>);
    static_assert(std::string_view{"_sOME_fairly_long_complex_012345_identifier_"} == RareTs::Aggregates::member_name<1, VariousNames>);
    static_assert(std::string_view{"ef"} == RareTs::Aggregates::member_name<2, VariousNames>);
    static_assert(std::string_view{"qwerty"} == RareTs::Aggregates::member_name<3, VariousNames>);
}

TEST(AggregateTest, ReflectEmpty)
{
    static_assert(0 == RareTs::Members<Empty>::total);
    size_t total = 0;
    RareTs::Members<Empty>::forEach([&](auto) {
        ++total;
    });
    EXPECT_EQ(0, total);
}

struct TrivialObj
{
    int a = 33;
    std::string_view _sOMe_fairly_long_complex445_identifieR_{"asdf"};
};

TEST(AggregateTest, ReflectTrivial)
{
    #ifndef __INTELLISENSE__ // Intellisense can't process member counts and ergo most of this section correctly
    static_assert(2 == RareTs::Members<TrivialObj>::total);
    static_assert(std::is_same_v<int, typename RareTs::Member<TrivialObj, 0>::type>);
    static_assert(std::is_same_v<std::string_view, typename RareTs::Member<TrivialObj, 1>::type>);
    static_assert(RareTs::Member<TrivialObj, 0>::value(TrivialObj{}) == 33);
    static_assert(RareTs::Member<TrivialObj, 1>::value(TrivialObj{}) == "asdf");
    static_assert(std::string_view{"a"} == RareTs::Member<TrivialObj, 0>::name);
    static_assert(std::string_view{"_sOMe_fairly_long_complex445_identifieR_"} == RareTs::Member<TrivialObj, 1>::name);
    #endif

    int visitCount = 0;
    RareTs::Members<TrivialObj>::forEach(TrivialObj{}, [&](auto member, auto & value) {
        bool visited = false;
        if constexpr ( member.index == 0 )
        {
            EXPECT_EQ(0, visitCount);
            bool isSame = std::is_same_v<int, typename decltype(member)::type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(33, value);
            EXPECT_STREQ("a", member.name);
            visited = true;
        }
        else if constexpr ( member.index == 1 )
        {
            EXPECT_EQ(1, visitCount);
            bool isSame = std::is_same_v<std::string_view, typename decltype(member)::type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ("asdf", value);
            EXPECT_STREQ("_sOMe_fairly_long_complex445_identifieR_", member.name);
            visited = true;
        }
        EXPECT_TRUE(visited);
        ++visitCount;
    });
    EXPECT_EQ(2, visitCount);


    auto unpaddedSize = RareTs::Members<TrivialObj>::pack([&](auto ... member) {
        if constexpr ( sizeof...(member) > 0 )
            return (sizeof(typename decltype(member)::type) + ...);
        else
            return 0;
    });
    EXPECT_EQ(unpaddedSize, sizeof(int) + sizeof(std::string_view));


    visitCount = 0;
    for ( size_t i=0; i<RareTs::Members<TrivialObj>::total; ++i )
    {
        RareTs::Members<TrivialObj>::at(i, TrivialObj{}, [&](auto member, auto & value) {
            if constexpr ( member.index == 0 )
            {
                EXPECT_EQ(0, visitCount);
                bool isSame = std::is_same_v<int, typename decltype(member)::type>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ(33, value);
                EXPECT_STREQ("a", member.name);
                ++visitCount;
            }
            else if constexpr ( member.index == 1 )
            {
                EXPECT_EQ(1, visitCount);
                bool isSame = std::is_same_v<std::string_view, typename decltype(member)::type>;
                EXPECT_TRUE(isSame);
                EXPECT_EQ("asdf", value);
                EXPECT_STREQ("_sOMe_fairly_long_complex445_identifieR_", member.name);
                ++visitCount;
            }
        });
    }
    EXPECT_EQ(2, visitCount);

    visitCount = 0;
    std::string a = "a";
    RareTs::Members<TrivialObj>::named(a, TrivialObj{}, [&](auto member, auto & value) {
        if constexpr ( member.index == 0 )
        {
            EXPECT_EQ(0, visitCount);
            bool isSame = std::is_same_v<int, typename decltype(member)::type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(33, value);
            EXPECT_STREQ("a", member.name);
            ++visitCount;
        }
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    RareTs::Members<TrivialObj>::named("_sOMe_fairly_long_complex445_identifieR_", TrivialObj{}, [&](auto member, auto & value) {
        if constexpr ( member.index == 1 )
        {
            EXPECT_EQ(0, visitCount);
            bool isSame = std::is_same_v<std::string_view, typename decltype(member)::type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ("asdf", value);
            EXPECT_STREQ("_sOMe_fairly_long_complex445_identifieR_", member.name);
            ++visitCount;
        }
    });
    EXPECT_EQ(1, visitCount);
}


struct TrivialArrays
{
    int a[2] {1, 2};
    int b[3] {3, 4, 5};
};

TEST(AggregateTest, ReflectArrays)
{
    #ifndef __INTELLISENSE__ // Intellisense can't process member counts and ergo most of this section correctly
    static_assert(2 == RareTs::Members<TrivialArrays>::total);
    static_assert(std::is_same_v<int[2], typename RareTs::Member<TrivialArrays, 0>::type>);
    static_assert(std::is_same_v<int[3], typename RareTs::Member<TrivialArrays, 1>::type>);
    static_assert(RareTs::Member<TrivialArrays, 0>::value(TrivialArrays{})[0] == 1);
    static_assert(RareTs::Member<TrivialArrays, 0>::value(TrivialArrays{})[1] == 2);
    static_assert(RareTs::Member<TrivialArrays, 1>::value(TrivialArrays{})[0] == 3);
    static_assert(RareTs::Member<TrivialArrays, 1>::value(TrivialArrays{})[1] == 4);
    static_assert(RareTs::Member<TrivialArrays, 1>::value(TrivialArrays{})[2] == 5);
    static_assert(std::string_view{"a"} == RareTs::Member<TrivialArrays, 0>::name);
    static_assert(std::string_view{"b"} == RareTs::Member<TrivialArrays, 1>::name);
    #endif

    int visitCount = 0;
    RareTs::Members<TrivialArrays>::forEach(TrivialArrays{}, [&](auto member, auto & value) {
        if constexpr ( member.index == 0 )
        {
            EXPECT_EQ(0, visitCount);
            bool isSame = std::is_same_v<int[2], typename decltype(member)::type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(1, value[0]);
            EXPECT_EQ(2, value[1]);
            EXPECT_STREQ("a", member.name);
            ++visitCount;
        }
        else if constexpr ( member.index == 1 )
        {
            EXPECT_EQ(1, visitCount);
            bool isSame = std::is_same_v<int[3], typename decltype(member)::type>;
            EXPECT_TRUE(isSame);
            EXPECT_EQ(3, value[0]);
            EXPECT_EQ(4, value[1]);
            EXPECT_EQ(5, value[2]);
            EXPECT_STREQ("b", member.name);
            ++visitCount;
        }
    });
    EXPECT_EQ(2, visitCount);
}

struct MixedObj
{
    TrivialObj trivialObj {};
    TrivialArrays trivialArrays {};
    int a = 6;
    std::string_view bStr = "b";
    int ray[3] {7, 8, 9};
};

TEST(AggregateTest, ReflectMixed)
{
    #ifndef __INTELLISENSE__ // Intellisense can't process member counts and ergo most of this section correctly
    static_assert(5 == RareTs::Members<MixedObj>::total);
    static_assert(std::is_same_v<TrivialObj, typename RareTs::Member<MixedObj, 0>::type>);
    static_assert(std::is_same_v<TrivialArrays, typename RareTs::Member<MixedObj, 1>::type>);
    static_assert(std::is_same_v<int, typename RareTs::Member<MixedObj, 2>::type>);
    static_assert(std::is_same_v<std::string_view, typename RareTs::Member<MixedObj, 3>::type>);
    static_assert(std::is_same_v<int[3], typename RareTs::Member<MixedObj, 4>::type>);
    static_assert(std::string_view{"trivialObj"} == RareTs::Member<MixedObj, 0>::name);
    static_assert(std::string_view{"trivialArrays"} == RareTs::Member<MixedObj, 1>::name);
    static_assert(std::string_view{"a"} == RareTs::Member<MixedObj, 2>::name);
    static_assert(std::string_view{"bStr"} == RareTs::Member<MixedObj, 3>::name);
    static_assert(std::string_view{"ray"} == RareTs::Member<MixedObj, 4>::name);
    static_assert(RareTs::Member<MixedObj, 0>::value(MixedObj{}).a == 33);
    static_assert(RareTs::Member<MixedObj, 0>::value(MixedObj{})._sOMe_fairly_long_complex445_identifieR_ == "asdf");
    static_assert(RareTs::Member<MixedObj, 1>::value(MixedObj{}).a[0] == 1);
    static_assert(RareTs::Member<MixedObj, 1>::value(MixedObj{}).a[1] == 2);
    static_assert(RareTs::Member<MixedObj, 1>::value(MixedObj{}).b[0] == 3);
    static_assert(RareTs::Member<MixedObj, 1>::value(MixedObj{}).b[1] == 4);
    static_assert(RareTs::Member<MixedObj, 1>::value(MixedObj{}).b[2] == 5);
    static_assert(RareTs::Member<MixedObj, 2>::value(MixedObj{}) == 6);
    static_assert(RareTs::Member<MixedObj, 3>::value(MixedObj{}) == "b");
    static_assert(RareTs::Member<MixedObj, 4>::value(MixedObj{})[0] == 7);
    static_assert(RareTs::Member<MixedObj, 4>::value(MixedObj{})[1] == 8);
    static_assert(RareTs::Member<MixedObj, 4>::value(MixedObj{})[2] == 9);
    #endif

    int visitCount = 0;
    RareTs::Members<MixedObj>::forEach(MixedObj{}, [&](auto, auto &) {
        ++visitCount;
    });
    EXPECT_EQ(5, visitCount);
}

struct LimitTest
{
    int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,c0,c1,
    c2,c3,c4,c5,c6,c7,c8,c9,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,g0,g1,g2,g3,g4,g5,
    g6,g7,g8,g9,h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,j0,j1,j2,j3,j4;
};

TEST(AggregateTest, ReflectLimit)
{
    int total = 0;
    RareTs::Members<LimitTest>::forEach([&](auto) {
        ++total;
    });
    EXPECT_EQ(121, total);
}

}
#endif