#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <map>
#include <vector>
#include <stack>

struct TupEmpty
{
    REFLECT(TupEmpty)
};

struct TupTrivial
{
    int a = 1337;

    REFLECT(TupTrivial, a)
};

struct TupObj
{
    TupObj() : primitive(0), object({}), primitiveReference(primitive) { primitiveArray[0] = 0; primitiveArray[1] = 1; }

    int primitive;
    TupTrivial object;
    int primitiveArray[2];
    std::map<int,float> map;
    std::vector<TupTrivial> objCollection;
    std::stack<int> stack;
    static int staticPrimitive;
    int & primitiveReference;
    static int & staticPrimitiveReference;
    int memberMethod() { return 1; }
    static int staticMethod() { return 2; }

    REFLECT(TupObj, primitive, object, primitiveArray, map, objCollection, stack, staticPrimitive, primitiveReference, staticPrimitiveReference, memberMethod, staticMethod)
};

int TupObj::staticPrimitive = 0;
int & TupObj::staticPrimitiveReference = TupObj::staticPrimitive;

TEST(TuplesTest, MemberTypeTuple)
{
    bool isSame = std::is_same_v<std::tuple<>, RareTs::member_type_tuple_t<TupEmpty>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int>, RareTs::member_type_tuple_t<TupTrivial>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, TupTrivial, int[2], std::map<int, float>, std::vector<TupTrivial>, std::stack<int>, int, int &, int &, decltype(&TupObj::memberMethod), decltype(TupObj::staticMethod)>, RareTs::member_type_tuple_t<TupObj>>;
    EXPECT_TRUE(isSame);
}

TEST(TuplesTest, Tuplify)
{
    TupEmpty tupEmpty {};
    auto tuplifyEmpty = RareTs::tuplify(tupEmpty);
    using TuplifyEmpty = decltype(tuplifyEmpty);
    EXPECT_EQ(0, std::tuple_size_v<TuplifyEmpty>);

    TupTrivial tupTrivial {};
    auto tuplifyTrivial = RareTs::tuplify(tupTrivial);
    using TuplifyTrivial = decltype(tuplifyTrivial);
    bool isSame = std::is_same_v<TupTrivial &, decltype(tuplifyTrivial.object)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::tuple_size_v<TuplifyTrivial>);
    isSame = std::is_same_v<int, std::tuple_element_t<0, TuplifyTrivial>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1337, std::get<0>(tuplifyTrivial));
    std::get<0>(tuplifyTrivial) = 42;
    EXPECT_EQ(42, tupTrivial.a);
    isSame = std::is_same_v<int &, decltype(std::get<0>(tuplifyTrivial))>;
    EXPECT_TRUE(isSame);

    // Check that structured bindings work
    auto [a] = tupTrivial;
    EXPECT_EQ(42, a);
    const auto [a_c] = tupTrivial;
    EXPECT_EQ(42, a_c);
    auto & [a_r] = tupTrivial;
    EXPECT_EQ(42, a_r);
    const auto & [a_cr] = tupTrivial;
    EXPECT_EQ(42, a_cr);
    a_r = 99;
    EXPECT_EQ(99, tupTrivial.a);

    const TupTrivial constTupTrivial {};
    auto constTuplifyTrivial = RareTs::tuplify(constTupTrivial);
    using ConstTuplifyTrivial = decltype(constTuplifyTrivial);
    isSame = std::is_same_v<const TupTrivial &, decltype(constTuplifyTrivial.object)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::tuple_size_v<ConstTuplifyTrivial>);
    isSame = std::is_same_v<int, std::tuple_element_t<0, ConstTuplifyTrivial>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1337, std::get<0>(constTuplifyTrivial));
    isSame = std::is_same_v<const int &, decltype(std::get<0>(constTuplifyTrivial))>;
    EXPECT_TRUE(isSame);

    volatile TupTrivial volatileTupTrivial {};
    auto volatileTuplifyTrivial = RareTs::tuplify(volatileTupTrivial);
    using VolatileTuplifyTrivial = decltype(volatileTuplifyTrivial);
    isSame = std::is_same_v<volatile TupTrivial &, decltype(volatileTuplifyTrivial.object)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::tuple_size_v<VolatileTuplifyTrivial>);
    isSame = std::is_same_v<int, std::tuple_element_t<0, VolatileTuplifyTrivial>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1337, std::get<0>(volatileTuplifyTrivial));
    std::get<0>(volatileTuplifyTrivial) = 42;
    EXPECT_EQ(42, volatileTupTrivial.a);
    isSame = std::is_same_v<volatile int &, decltype(std::get<0>(volatileTuplifyTrivial))>;
    EXPECT_TRUE(isSame);

    const volatile TupTrivial cvTupTrivial {};
    auto cvTuplifyTrivial = RareTs::tuplify(cvTupTrivial);
    using CvTuplifyTrivial = decltype(cvTuplifyTrivial);
    isSame = std::is_same_v<const volatile TupTrivial &, decltype(cvTuplifyTrivial.object)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::tuple_size_v<CvTuplifyTrivial>);
    isSame = std::is_same_v<int, std::tuple_element_t<0, CvTuplifyTrivial>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1337, std::get<0>(cvTuplifyTrivial));
    isSame = std::is_same_v<const volatile int &, decltype(std::get<0>(cvTuplifyTrivial))>;
    EXPECT_TRUE(isSame);

    auto tempTuplifyTrivial = RareTs::tuplify(TupTrivial{});
    using TempTuplifyTrivial = decltype(tempTuplifyTrivial);
    isSame = std::is_same_v<TupTrivial, decltype(tempTuplifyTrivial.object)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::tuple_size_v<TempTuplifyTrivial>);
    isSame = std::is_same_v<int, std::tuple_element_t<0, TempTuplifyTrivial>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1337, std::get<0>(tempTuplifyTrivial));
    std::get<0>(tempTuplifyTrivial) = 42;
    EXPECT_EQ(42, tempTuplifyTrivial.object.a);
    isSame = std::is_same_v<int &, decltype(std::get<0>(tempTuplifyTrivial))>;
    EXPECT_TRUE(isSame);

    TupObj tupObj {};
    auto tuplifyObj = RareTs::tuplify(tupObj);
    using TuplifyObj = decltype(tuplifyObj);
    EXPECT_EQ(RareTs::Members<TupObj>::total, std::tuple_size_v<TuplifyObj>);

    isSame = std::is_same_v<int, std::tuple_element_t<0, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<0>(tuplifyObj));
    isSame = std::is_same_v<TupTrivial, std::tuple_element_t<1, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(TupTrivial{}.a, std::get<1>(tuplifyObj).a);
    isSame = std::is_same_v<int[2], std::tuple_element_t<2, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<2>(tuplifyObj)[0]);
    EXPECT_EQ(1, std::get<2>(tuplifyObj)[1]);
    isSame = std::is_same_v<std::map<int, float>, std::tuple_element_t<3, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::map<int, float> &, decltype(std::get<3>(tuplifyObj))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::vector<TupTrivial>, std::tuple_element_t<4, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::vector<TupTrivial> &, decltype(std::get<4>(tuplifyObj))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::stack<int>, std::tuple_element_t<5, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::stack<int> &, decltype(std::get<5>(tuplifyObj))>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, std::tuple_element_t<6, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<6>(tuplifyObj));
    isSame = std::is_same_v<int &, std::tuple_element_t<7, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<7>(tuplifyObj));
    isSame = std::is_same_v<int &, std::tuple_element_t<8, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<8>(tuplifyObj));
    isSame = std::is_same_v<decltype(&TupObj::memberMethod), std::tuple_element_t<9, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(&TupObj::memberMethod, std::get<9>(tuplifyObj));
    isSame = std::is_same_v<decltype(TupObj::staticMethod), std::tuple_element_t<10, TuplifyObj>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(&TupObj::staticMethod, std::get<10>(tuplifyObj));
}
