#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using namespace RareTs;

struct EnableIfConstexprTest
{
    static inline int runtimeInt = 0;
    static constexpr int constexprInt = 1;
    
    template <typename T, typename = RareTs::enable_if_constexpr<decltype(T::runtimeInt), T::runtimeInt>>
    static constexpr std::true_type isRuntimeIntConstexpr(int);

    template <typename T>
    static constexpr std::false_type isRuntimeIntConstexpr(...);
    
    template <typename T, typename = RareTs::enable_if_constexpr<decltype(T::constexprInt), T::constexprInt>>
    static constexpr std::true_type isConstexprIntConstexpr(int);

    template <typename T>
    static constexpr std::false_type isConstexprIntConstexpr(...);
};

TEST(RareTsTest, EnableIfConstexpr)
{
    bool isRuntimeIntConstexpr = decltype(EnableIfConstexprTest::isRuntimeIntConstexpr<EnableIfConstexprTest>(0))::value;
    bool isConstexprIntConstexpr = decltype(EnableIfConstexprTest::isConstexprIntConstexpr<EnableIfConstexprTest>(0))::value;
    EXPECT_FALSE(isRuntimeIntConstexpr);
    EXPECT_TRUE(isConstexprIntConstexpr);
}

TEST(RareTsTest, AsIndex)
{
    bool isSame = std::is_same_v<std::integral_constant<size_t, 0>, as_index<0>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::integral_constant<size_t, 1>, as_index<1>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::integral_constant<size_t, 2>, as_index<2, void>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::integral_constant<size_t, 3>, as_index<3, void, void>>;
    EXPECT_TRUE(isSame);
}

TEST(RareTsTest, RemoveCvRef)
{
    bool isSame = std::is_same_v<int, typename RareTs::remove_cvref<int>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<const int>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<volatile int>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<const volatile int>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<int &>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<const int &>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<volatile int &>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<const volatile int &>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<int &&>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<const int &&>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<volatile int &&>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, typename RareTs::remove_cvref<const volatile int &&>::type>;
    EXPECT_TRUE(isSame);

    isSame = std::is_same_v<int, RareTs::remove_cvref_t<int>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<const int>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<volatile int>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<const volatile int>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<int &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<const int &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<volatile int &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<const volatile int &>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<int &&>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<const int &&>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<volatile int &&>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<int, RareTs::remove_cvref_t<const volatile int &&>>;
    EXPECT_TRUE(isSame);
}

TEST(RareTsTest, TypeId)
{
    bool isSame = std::is_same_v<int, typename RareTs::type_id<int>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<float, typename RareTs::type_id<float>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<const int &&, typename RareTs::type_id<const int &&>::type>;
    EXPECT_TRUE(isSame);
}

TEST(RareTsTest, ReplaceVoid)
{
    bool replacedVoid = std::is_same_v<float, replace_void<void, float>::type>;
    EXPECT_TRUE(replacedVoid);
    replacedVoid = std::is_same_v<float, replace_void<int, float>::type>;
    EXPECT_FALSE(replacedVoid);
    bool alwaysVoid = std::is_same_v<void, replace_void<void, void>::type>;
    EXPECT_TRUE(alwaysVoid);
    replacedVoid = std::is_same_v<void, replace_void<int, void>::type>;
    EXPECT_FALSE(replacedVoid);

    replacedVoid = std::is_same_v<float, replace_void_t<void, float>>;
    EXPECT_TRUE(replacedVoid);
    replacedVoid = std::is_same_v<float, replace_void_t<int, float>>;
    EXPECT_FALSE(replacedVoid);
    alwaysVoid = std::is_same_v<void, replace_void_t<void, void>>;
    EXPECT_TRUE(alwaysVoid);
    replacedVoid = std::is_same_v<void, replace_void_t<int, void>>;
    EXPECT_FALSE(replacedVoid);
}

TEST(RareTsTest, PromoteChar)
{
    bool isSame = std::is_same_v<promote_char_t<char>, int>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<promote_char_t<signed char>, int>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<promote_char_t<unsigned char>, int>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<promote_char_t<const char>, const int>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<promote_char_t<const signed char>, const int>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<promote_char_t<const unsigned char>, const int>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<promote_char_t<short>, short>;
    EXPECT_TRUE(isSame);
}

TEST(RareTsTest, PairLhs)
{
    bool isSame = std::is_same_v<float, pair_lhs_t<std::pair<float, int>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<char, pair_lhs_t<std::pair<char, int>>>;
    EXPECT_TRUE(isSame);
}

TEST(RareTsTest, ElementType)
{
    bool isEqual = std::is_same_v<void, element_type_t<int>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<void, element_type_t<int*>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, element_type_t<int[2]>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, element_type_t<std::array<int, 2>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, element_type_t<std::vector<int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<std::pair<int, float>, element_type_t<std::map<int, float>>>;
    EXPECT_TRUE(isEqual);
    
    isEqual = std::is_same_v<void, element_type_t<const int>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<void, element_type_t<const int*>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, element_type_t<const int[2]>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, element_type_t<const std::array<int, 2>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, element_type_t<const std::vector<int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<std::pair<int, float>, element_type_t<const std::map<int, float>>>;
    EXPECT_TRUE(isEqual);
}

TEST(RareTsTest, IsPointable)
{
    EXPECT_FALSE(is_pointable_v<int>);
    EXPECT_FALSE(is_pointable_v<int[2]>);
    EXPECT_TRUE(is_pointable_v<int*>);
    EXPECT_TRUE(is_pointable_v<std::unique_ptr<int>>);
    EXPECT_TRUE(is_pointable_v<std::shared_ptr<int>>);
    EXPECT_TRUE(is_pointable_v<std::weak_ptr<int>>);
    
    EXPECT_FALSE(is_pointable_v<const int>);
    EXPECT_FALSE(is_pointable_v<const int[2]>);
    EXPECT_TRUE(is_pointable_v<const int*>);
    EXPECT_TRUE(is_pointable_v<const int* const>);
    EXPECT_TRUE(is_pointable_v<std::unique_ptr<const int>>);
    EXPECT_TRUE(is_pointable_v<const std::unique_ptr<const int>>);
    EXPECT_TRUE(is_pointable_v<std::shared_ptr<const int>>);
    EXPECT_TRUE(is_pointable_v<const std::shared_ptr<const int>>);
    EXPECT_TRUE(is_pointable_v<std::weak_ptr<const int>>);
    EXPECT_TRUE(is_pointable_v<const std::weak_ptr<const int>>);
}

TEST(RareTsTest, RemovePointer)
{
    bool isEqual = std::is_same_v<int, remove_pointer_t<int>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int[2], remove_pointer_t<int[2]>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, remove_pointer_t<int*>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, remove_pointer_t<std::unique_ptr<int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, remove_pointer_t<std::shared_ptr<int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int, remove_pointer_t<std::weak_ptr<int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<int*, remove_pointer_t<int**>>;
    EXPECT_TRUE(isEqual);
    
    isEqual = std::is_same_v<const int, remove_pointer_t<const int>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int[2], remove_pointer_t<const int[2]>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int, remove_pointer_t<const int*>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int, remove_pointer_t<const int* const>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int, remove_pointer_t<std::unique_ptr<const int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int, remove_pointer_t<const std::unique_ptr<const int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int, remove_pointer_t<std::shared_ptr<const int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int, remove_pointer_t<const std::shared_ptr<const int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int, remove_pointer_t<std::weak_ptr<const int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int, remove_pointer_t<const std::weak_ptr<const int>>>;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same_v<const int*, remove_pointer_t<const int**>>;
    EXPECT_TRUE(isEqual);
}

TEST(RareTsTest, IsNonNull)
{
    bool isNonNull = RareTs::is_non_null<nullptr_t>::value;
    EXPECT_FALSE(isNonNull);
    isNonNull = RareTs::is_non_null<const nullptr_t>::value;
    EXPECT_FALSE(isNonNull);
    isNonNull = RareTs::is_non_null<volatile nullptr_t>::value;
    EXPECT_FALSE(isNonNull);
    isNonNull = RareTs::is_non_null<const volatile nullptr_t>::value;
    EXPECT_FALSE(isNonNull);
    
    isNonNull = RareTs::is_non_null<void>::value;
    EXPECT_TRUE(isNonNull);
    isNonNull = RareTs::is_non_null<void*>::value;
    EXPECT_TRUE(isNonNull);

    isNonNull = RareTs::is_non_null<int>::value;
    EXPECT_TRUE(isNonNull);
    isNonNull = RareTs::is_non_null<const int>::value;
    EXPECT_TRUE(isNonNull);
    isNonNull = RareTs::is_non_null<volatile int>::value;
    EXPECT_TRUE(isNonNull);
    isNonNull = RareTs::is_non_null<const volatile int>::value;
    EXPECT_TRUE(isNonNull);
}

TEST(RareTsTest, StaticArraySize)
{
    EXPECT_EQ(size_t(2), static_array_size_v<int[2]>);
    EXPECT_EQ(size_t(3), static_array_size_v<int[3]>);
    size_t staticArraySize = static_array_size_v<std::array<int, 2>>;
    EXPECT_EQ(size_t(2), staticArraySize);
    staticArraySize = static_array_size_v<std::array<int, 3>>;
    EXPECT_EQ(size_t(3), staticArraySize);
    
    EXPECT_EQ(size_t(2), static_array_size_v<const int[2]>);
    EXPECT_EQ(size_t(3), static_array_size_v<const int[3]>);
    staticArraySize = static_array_size_v<const std::array<int, 2>>;
    EXPECT_EQ(size_t(2), staticArraySize);
    staticArraySize = static_array_size_v<const std::array<int, 3>>;
    EXPECT_EQ(size_t(3), staticArraySize);
}

TEST(RareTsTest, IsStaticArray)
{
    EXPECT_TRUE(is_static_array_v<int[2]>);
    EXPECT_FALSE(is_static_array_v<int*>);
    bool isStaticArray = is_static_array_v<std::array<int, 2>>;
    EXPECT_TRUE(isStaticArray);
    EXPECT_FALSE(is_static_array_v<std::vector<int>>);
    
    EXPECT_TRUE(is_static_array_v<const int[2]>);
    EXPECT_FALSE(is_static_array_v<const int*>);
    isStaticArray = is_static_array_v<const std::array<int, 2>>;
    EXPECT_TRUE(isStaticArray);
    EXPECT_FALSE(is_static_array_v<const std::vector<int>>);
}

TEST(RareTsTest, IsIterable)
{
    EXPECT_FALSE(is_iterable_v<int>);
    EXPECT_FALSE(is_iterable_v<int*>);
    EXPECT_TRUE(is_iterable_v<int[2]>);
    bool isIterable = is_iterable_v<std::array<int, 2>>;
    EXPECT_TRUE(isIterable);
    EXPECT_TRUE(is_iterable_v<std::vector<int>>);
    EXPECT_TRUE(is_iterable_v<std::deque<int>>);
    EXPECT_TRUE(is_iterable_v<std::list<int>>);
    EXPECT_TRUE(is_iterable_v<std::forward_list<int>>);
    EXPECT_TRUE(is_iterable_v<std::stack<int>>);
    EXPECT_TRUE(is_iterable_v<std::queue<int>>);
    EXPECT_TRUE(is_iterable_v<std::priority_queue<int>>);
    EXPECT_TRUE(is_iterable_v<std::set<int>>);
    EXPECT_TRUE(is_iterable_v<std::multiset<int>>);
    EXPECT_TRUE(is_iterable_v<std::unordered_set<int>>);
    EXPECT_TRUE(is_iterable_v<std::unordered_multiset<int>>);
    isIterable = is_iterable_v<std::map<int, float>>;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable_v<std::multimap<int, float>>;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable_v<std::unordered_map<int, float>>;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable_v<std::unordered_multimap<int, float>>;
    EXPECT_TRUE(isIterable);
    
    EXPECT_FALSE(is_iterable_v<const int>);
    EXPECT_FALSE(is_iterable_v<const int*>);
    EXPECT_TRUE(is_iterable_v<const int[2]>);
    isIterable = is_iterable_v<const std::array<int, 2>>;
    EXPECT_TRUE(isIterable);
    EXPECT_TRUE(is_iterable_v<const std::vector<int>>);
    EXPECT_TRUE(is_iterable_v<const std::deque<int>>);
    EXPECT_TRUE(is_iterable_v<const std::list<int>>);
    EXPECT_TRUE(is_iterable_v<const std::forward_list<int>>);
    EXPECT_TRUE(is_iterable_v<const std::stack<int>>);
    EXPECT_TRUE(is_iterable_v<const std::queue<int>>);
    EXPECT_TRUE(is_iterable_v<const std::priority_queue<int>>);
    EXPECT_TRUE(is_iterable_v<const std::set<int>>);
    EXPECT_TRUE(is_iterable_v<const std::multiset<int>>);
    EXPECT_TRUE(is_iterable_v<const std::unordered_set<int>>);
    EXPECT_TRUE(is_iterable_v<const std::unordered_multiset<int>>);
    isIterable = is_iterable_v<const std::map<int, float>>;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable_v<const std::multimap<int, float>>;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable_v<const std::unordered_map<int, float>>;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable_v<const std::unordered_multimap<int, float>>;
    EXPECT_TRUE(isIterable);
}

TEST(RareTsTest, IsMap)
{
    EXPECT_FALSE(is_map_v<int>);
    EXPECT_FALSE(is_map_v<int*>);
    EXPECT_FALSE(is_map_v<int[2]>);
    bool isMap = is_map_v<std::array<int, 2>>;
    EXPECT_FALSE(isMap);
    EXPECT_FALSE(is_map_v<std::vector<int>>);
    EXPECT_FALSE(is_map_v<std::deque<int>>);
    EXPECT_FALSE(is_map_v<std::list<int>>);
    EXPECT_FALSE(is_map_v<std::forward_list<int>>);
    EXPECT_FALSE(is_map_v<std::stack<int>>);
    EXPECT_FALSE(is_map_v<std::queue<int>>);
    EXPECT_FALSE(is_map_v<std::priority_queue<int>>);
    EXPECT_FALSE(is_map_v<std::set<int>>);
    EXPECT_FALSE(is_map_v<std::multiset<int>>);
    EXPECT_FALSE(is_map_v<std::unordered_set<int>>);
    EXPECT_FALSE(is_map_v<std::unordered_multiset<int>>);
    isMap = is_map_v<std::map<int, float>>;
    EXPECT_TRUE(isMap);
    isMap = is_map_v<std::multimap<int, float>>;
    EXPECT_TRUE(isMap);
    isMap = is_map_v<std::unordered_map<int, float>>;
    EXPECT_TRUE(isMap);
    isMap = is_map_v<std::unordered_multimap<int, float>>;
    EXPECT_TRUE(isMap);
    
    EXPECT_FALSE(is_map_v<const int>);
    EXPECT_FALSE(is_map_v<const int*>);
    EXPECT_FALSE(is_map_v<const int[2]>);
    isMap = is_map_v<const std::array<int, 2>>;
    EXPECT_FALSE(isMap);
    EXPECT_FALSE(is_map_v<const std::vector<int>>);
    EXPECT_FALSE(is_map_v<const std::deque<int>>);
    EXPECT_FALSE(is_map_v<const std::list<int>>);
    EXPECT_FALSE(is_map_v<const std::forward_list<int>>);
    EXPECT_FALSE(is_map_v<const std::stack<int>>);
    EXPECT_FALSE(is_map_v<const std::queue<int>>);
    EXPECT_FALSE(is_map_v<const std::priority_queue<int>>);
    EXPECT_FALSE(is_map_v<const std::set<int>>);
    EXPECT_FALSE(is_map_v<const std::multiset<int>>);
    EXPECT_FALSE(is_map_v<const std::unordered_set<int>>);
    EXPECT_FALSE(is_map_v<const std::unordered_multiset<int>>);
    isMap = is_map_v<const std::map<int, float>>;
    EXPECT_TRUE(isMap);
    isMap = is_map_v<const std::multimap<int, float>>;
    EXPECT_TRUE(isMap);
    isMap = is_map_v<const std::unordered_map<int, float>>;
    EXPECT_TRUE(isMap);
    isMap = is_map_v<const std::unordered_multimap<int, float>>;
    EXPECT_TRUE(isMap);
}

TEST(RareTsTest, IsAdaptor)
{
    EXPECT_FALSE(is_adaptor_v<int>);
    EXPECT_FALSE(is_adaptor_v<std::vector<int>>);
    EXPECT_TRUE(is_adaptor_v<std::stack<int>>);
    EXPECT_TRUE(is_adaptor_v<std::queue<int>>);
    EXPECT_TRUE(is_adaptor_v<std::priority_queue<int>>);
    
    EXPECT_FALSE(is_adaptor_v<const int>);
    EXPECT_FALSE(is_adaptor_v<const std::vector<int>>);
    EXPECT_TRUE(is_adaptor_v<const std::stack<int>>);
    EXPECT_TRUE(is_adaptor_v<const std::queue<int>>);
    EXPECT_TRUE(is_adaptor_v<const std::priority_queue<int>>);
}

TEST(RareTsTest, IsPair)
{
    EXPECT_FALSE(is_pair_v<int>);
    EXPECT_FALSE(is_pair_v<int*>);
    EXPECT_FALSE(is_pair_v<int[2]>);
    EXPECT_FALSE(is_pair_v<std::vector<int>>);
    bool isPair = is_pair_v<std::map<int, float>>;
    EXPECT_FALSE(isPair);
    isPair = is_pair_v<std::tuple<int, int>>;
    EXPECT_FALSE(isPair);
    isPair = is_pair_v<std::pair<int, float>>;
    EXPECT_TRUE(isPair);
    
    EXPECT_FALSE(is_pair_v<const int>);
    EXPECT_FALSE(is_pair_v<const int*>);
    EXPECT_FALSE(is_pair_v<const int[2]>);
    EXPECT_FALSE(is_pair_v<const std::vector<int>>);
    isPair = is_pair_v<const std::map<int, float>>;
    EXPECT_FALSE(isPair);
    isPair = is_pair_v<const std::tuple<int, int>>;
    EXPECT_FALSE(isPair);
    isPair = is_pair_v<const std::pair<int, float>>;
    EXPECT_TRUE(isPair);
}

TEST(RareTsTest, IsTuple)
{
    EXPECT_FALSE(is_tuple_v<int>);
    EXPECT_FALSE(is_tuple_v<int*>);
    EXPECT_FALSE(is_tuple_v<int[2]>);
    EXPECT_FALSE(is_tuple_v<std::vector<int>>);
    bool isTuple = is_tuple_v<std::map<int, float>>;
    EXPECT_FALSE(isTuple);
    isTuple = is_tuple_v<std::pair<int, float>>;
    EXPECT_FALSE(isTuple);
    isTuple = is_tuple_v<std::tuple<>>;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple_v<std::tuple<int>>;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple_v<std::tuple<int, int>>;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple_v<std::tuple<int, int, int>>;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple_v<std::tuple<int, int, int, int>>;
    EXPECT_TRUE(isTuple);
    
    EXPECT_FALSE(is_tuple_v<const int>);
    EXPECT_FALSE(is_tuple_v<const int*>);
    EXPECT_FALSE(is_tuple_v<const int[2]>);
    EXPECT_FALSE(is_tuple_v<const std::vector<int>>);
    isTuple = is_tuple_v<const std::pair<int, float>>;
    EXPECT_FALSE(isTuple);
    isTuple = is_tuple_v<const std::map<int, float>>;
    EXPECT_FALSE(isTuple);
    isTuple = is_tuple_v<const std::tuple<>>;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple_v<const std::tuple<int>>;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple_v<const std::tuple<int, int>>;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple_v<const std::tuple<int, int, int>>;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple_v<const std::tuple<int, int, int, int>>;
    EXPECT_TRUE(isTuple);
}

TEST(RareTsTest, HasPushBack)
{
    bool has = has_push_back_v<int, int>;
    EXPECT_FALSE(has);
    has = has_push_back_v<int[2], int>;
    EXPECT_FALSE(has);
    has = has_push_back_v<std::forward_list<int>, int>;
    EXPECT_FALSE(has);
    has = has_push_back_v<std::vector<int>, int>;
    EXPECT_TRUE(has);
    has = has_push_back_v<std::deque<int>, int>;
    EXPECT_TRUE(has);
    has = has_push_back_v<std::list<int>, int>;
    EXPECT_TRUE(has);

    has = has_push_back_v<const int, int>;
    EXPECT_FALSE(has);
    has = has_push_back_v<const int[2], int>;
    EXPECT_FALSE(has);
    has = has_push_back_v<const std::forward_list<int>, int>;
    EXPECT_FALSE(has);
    has = has_push_back_v<const std::vector<int>, int>;
    EXPECT_FALSE(has);
    has = has_push_back_v<const std::deque<int>, int>;
    EXPECT_FALSE(has);
    has = has_push_back_v<const std::list<int>, int>;
    EXPECT_FALSE(has);
}

TEST(RareTsTest, HasPush)
{
    bool has = has_push_v<int, int>;
    EXPECT_FALSE(has);
    has = has_push_v<int[2], int>;
    EXPECT_FALSE(has);
    has = has_push_v<std::vector<int>, int>;
    EXPECT_FALSE(has);
    has = has_push_v<std::stack<int>, int>;
    EXPECT_TRUE(has);
    has = has_push_v<std::queue<int>, int>;
    EXPECT_TRUE(has);
    has = has_push_v<std::priority_queue<int>, int>;
    EXPECT_TRUE(has);

    has = has_push_v<const int, int>;
    EXPECT_FALSE(has);
    has = has_push_v<const int[2], int>;
    EXPECT_FALSE(has);
    has = has_push_v<const std::vector<int>, int>;
    EXPECT_FALSE(has);
    has = has_push_v<const std::stack<int>, int>;
    EXPECT_FALSE(has);
    has = has_push_v<const std::queue<int>, int>;
    EXPECT_FALSE(has);
    has = has_push_v<const std::priority_queue<int>, int>;
    EXPECT_FALSE(has);
}

TEST(RareTsTest, HasInsert)
{
    bool has = has_insert_v<int, int>;
    EXPECT_FALSE(has);
    has = has_insert_v<int[2], int>;
    EXPECT_FALSE(has);
    has = has_insert_v<std::set<int>, int>;
    EXPECT_TRUE(has);
    has = has_insert_v<std::multiset<int>, int>;
    EXPECT_TRUE(has);
    has = has_insert_v<std::unordered_set<int>, int>;
    EXPECT_TRUE(has);
    has = has_insert_v<std::unordered_multiset<int>, int>;
    EXPECT_TRUE(has);
    has = has_insert_v<std::map<int, float>, std::pair<int, float>>;
    EXPECT_TRUE(has);
    has = has_insert_v<std::multimap<int, float>, std::pair<int, float>>;
    EXPECT_TRUE(has);
    has = has_insert_v<std::unordered_map<int, float>, std::pair<int, float>>;
    EXPECT_TRUE(has);
    has = has_insert_v<std::unordered_multimap<int, float>, std::pair<int, float>>;
    EXPECT_TRUE(has);
    
    has = has_insert_v<const int, int>;
    EXPECT_FALSE(has);
    has = has_insert_v<const int[2], int>;
    EXPECT_FALSE(has);
    has = has_insert_v<const std::set<int>, int>;
    EXPECT_FALSE(has);
    has = has_insert_v<const std::multiset<int>, int>;
    EXPECT_FALSE(has);
    has = has_insert_v<const std::unordered_set<int>, int>;
    EXPECT_FALSE(has);
    has = has_insert_v<const std::unordered_multiset<int>, int>;
    EXPECT_FALSE(has);
    has = has_insert_v<const std::map<int, float>, std::pair<int, float>>;
    EXPECT_FALSE(has);
    has = has_insert_v<const std::multimap<int, float>, std::pair<int, float>>;
    EXPECT_FALSE(has);
    has = has_insert_v<const std::unordered_map<int, float>, std::pair<int, float>>;
    EXPECT_FALSE(has);
    has = has_insert_v<const std::unordered_multimap<int, float>, std::pair<int, float>>;
    EXPECT_FALSE(has);
}

TEST(RareTsTest, HasClear)
{
    EXPECT_FALSE(has_clear_v<int>);
    EXPECT_FALSE(has_clear_v<int[2]>);
    bool hasClear = has_clear_v<std::array<int, 2>>;
    EXPECT_FALSE(hasClear);
    EXPECT_TRUE(has_clear_v<std::vector<int>>);
    EXPECT_TRUE(has_clear_v<std::deque<int>>);
    EXPECT_TRUE(has_clear_v<std::list<int>>);
    EXPECT_TRUE(has_clear_v<std::forward_list<int>>);
    EXPECT_FALSE(has_clear_v<std::stack<int>>);
    EXPECT_FALSE(has_clear_v<std::queue<int>>);
    EXPECT_FALSE(has_clear_v<std::priority_queue<int>>);
    EXPECT_TRUE(has_clear_v<std::set<int>>);
    EXPECT_TRUE(has_clear_v<std::multiset<int>>);
    EXPECT_TRUE(has_clear_v<std::unordered_set<int>>);
    EXPECT_TRUE(has_clear_v<std::unordered_multiset<int>>);
    using Map = std::map<int, float>;
    using Multimap = std::multimap<int, float>;
    using UnorderedMap = std::unordered_map<int, float>;
    using UnorderedMultimap = std::unordered_multimap<int, float>;
    EXPECT_TRUE(has_clear_v<Map>);
    EXPECT_TRUE(has_clear_v<Multimap>);
    EXPECT_TRUE(has_clear_v<UnorderedMap>);
    EXPECT_TRUE(has_clear_v<UnorderedMultimap>);
}

template <typename T> struct IsSpecTest { static constexpr bool IsPrimaryTemplate = true; };
template <> struct IsSpecTest<int> { static constexpr bool IsPrimaryTemplate = false; };
template <typename T> struct IsNotSpecTemplatedTest {};
struct IsNotSpecStructTest {};

TEST(RareTsTest, IsSpecialization)
{
    bool isSpec = is_specialization<IsSpecTest<int>, IsSpecTest>::value;
    EXPECT_TRUE(isSpec);
    isSpec = is_specialization<IsSpecTest<float>, IsSpecTest>::value;
    EXPECT_TRUE(isSpec);
    isSpec = is_specialization<IsNotSpecTemplatedTest<int>, IsSpecTest>::value;
    EXPECT_FALSE(isSpec);
    isSpec = is_specialization<IsNotSpecTemplatedTest<float>, IsSpecTest>::value;
    EXPECT_FALSE(isSpec);
    isSpec = is_specialization<IsNotSpecStructTest, IsSpecTest>::value;
    EXPECT_FALSE(isSpec);
    isSpec = is_specialization<int, IsSpecTest>::value;
    EXPECT_FALSE(isSpec);
    isSpec = is_specialization<float, IsSpecTest>::value;
    EXPECT_FALSE(isSpec);

    isSpec = is_specialization_v<IsSpecTest<int>, IsSpecTest>;
    EXPECT_TRUE(isSpec);
    isSpec = is_specialization_v<IsSpecTest<float>, IsSpecTest>;
    EXPECT_TRUE(isSpec);
    isSpec = is_specialization_v<IsNotSpecTemplatedTest<int>, IsSpecTest>;
    EXPECT_FALSE(isSpec);
    isSpec = is_specialization_v<IsNotSpecTemplatedTest<float>, IsSpecTest>;
    EXPECT_FALSE(isSpec);
    isSpec = is_specialization_v<IsNotSpecStructTest, IsSpecTest>;
    EXPECT_FALSE(isSpec);
    isSpec = is_specialization_v<int, IsSpecTest>;
    EXPECT_FALSE(isSpec);
    isSpec = is_specialization_v<float, IsSpecTest>;
    EXPECT_FALSE(isSpec);
}

TEST(RareTsTest, Append)
{
    std::vector<int> vector;
    std::deque<int> deque;
    std::forward_list<int> forwardList;
    std::list<int> list;
    std::stack<int> stack;
    std::queue<int> queue;
    std::priority_queue<int> priorityQueue;
    std::set<int> set;
    std::multiset<int> multiset;
    std::unordered_set<int> unorderedSet;
    std::unordered_multiset<int> unorderedMultiset;
    std::map<int, int> map;
    std::multimap<int, int> multimap;
    std::unordered_map<int, int> unorderedMap;
    std::unordered_multimap<int, int> unorderedMultimap;

    int value = 7;
    int key = 8;
    std::pair<decltype(key), decltype(value)> keyValuePair = std::pair(key, value);
    append(vector, value);
    append(deque, value);
    append(forwardList, value);
    append(list, value);
    append(stack, value);
    append(queue, value);
    append(priorityQueue, value);
    append(set, value);
    append(multiset, value);
    append(unorderedSet, value);
    append(unorderedMultiset, value);
    append(map, keyValuePair);
    append(multimap, keyValuePair);
    append(unorderedMap, keyValuePair);
    append(unorderedMultimap, keyValuePair);
    
    EXPECT_EQ(value, *vector.begin());
    EXPECT_EQ(value, *deque.begin());
    EXPECT_EQ(value, *forwardList.begin());
    EXPECT_EQ(value, *list.begin());
    EXPECT_EQ(value, stack.top());
    EXPECT_EQ(value, queue.front());
    EXPECT_EQ(value, priorityQueue.top());
    EXPECT_EQ(value, *set.begin());
    EXPECT_EQ(value, *multiset.begin());
    EXPECT_EQ(value, *unorderedSet.begin());
    EXPECT_EQ(value, *unorderedMultiset.begin());
    EXPECT_EQ(key, map.begin()->first);
    EXPECT_EQ(key, multimap.begin()->first);
    EXPECT_EQ(key, unorderedMap.begin()->first);
    EXPECT_EQ(key, unorderedMultimap.begin()->first);
    EXPECT_EQ(value, map.begin()->second);
    EXPECT_EQ(value, multimap.begin()->second);
    EXPECT_EQ(value, unorderedMap.begin()->second);
    EXPECT_EQ(value, unorderedMultimap.begin()->second);

    int newValue = 5;
    int newKey = 6;
    std::pair<decltype(newKey), decltype(newValue)> newKeyValuePair = std::pair(newKey, newValue);
    
    append(vector, newValue);
    append(deque, newValue);
    append(forwardList, newValue);
    append(list, newValue);
    append(stack, newValue);
    append(queue, newValue);
    append(priorityQueue, newValue);
    append(set, newValue);
    append(multiset, newValue);
    append(unorderedSet, newValue);
    append(unorderedMultiset, newValue);
    append(map, newKeyValuePair);
    append(multimap, newKeyValuePair);
    append(unorderedMap, newKeyValuePair);
    append(unorderedMultimap, newKeyValuePair);
    
    EXPECT_EQ(newValue, *++vector.begin());
    EXPECT_EQ(newValue, *++deque.begin());
    EXPECT_EQ(newValue, *++forwardList.begin());
    EXPECT_EQ(newValue, *++list.begin());

    EXPECT_EQ(newValue, stack.top());
    stack.pop();
    EXPECT_EQ(value, stack.top());
    EXPECT_EQ(newValue, queue.back());
    EXPECT_EQ(value, queue.front());
    EXPECT_EQ(value, priorityQueue.top());
    priorityQueue.pop();
    EXPECT_EQ(newValue, priorityQueue.top());

    EXPECT_EQ(newValue, *set.begin());
    EXPECT_EQ(newValue, *multiset.begin());
    
    EXPECT_NE(unorderedSet.end(), unorderedSet.find(newValue));
    EXPECT_NE(unorderedMultiset.end(), unorderedMultiset.find(newValue));
    EXPECT_EQ(newKey, (map.begin()++)->first);
    EXPECT_EQ(newKey, (multimap.begin()++)->first);
    EXPECT_EQ(newValue, (map.begin()++)->second);
    EXPECT_EQ(newValue, (multimap.begin()++)->second);

    EXPECT_NE(unorderedMap.end(), unorderedMap.find(newKey));
    EXPECT_EQ(newValue, unorderedMap.find(newKey)->second);

    EXPECT_NE(unorderedMultimap.end(), unorderedMultimap.find(newKey));
    EXPECT_EQ(newValue, unorderedMultimap.find(newKey)->second);
}

TEST(RareTsTest, IsEmpty)
{
    int basicArray[1] = { 0 };
    std::array<int, 1> stlArray = { 0 };

    EXPECT_FALSE(isEmpty(basicArray));
    EXPECT_FALSE(isEmpty(stlArray));

    std::vector<int> vector;
    std::deque<int> deque;
    std::forward_list<int> forwardList;
    std::list<int> list;
    std::stack<int> stack;
    std::queue<int> queue;
    std::priority_queue<int> priorityQueue;
    std::set<int> set;
    std::multiset<int> multiset;
    std::unordered_set<int> unorderedSet;
    std::unordered_multiset<int> unorderedMultiset;
    std::map<int, int> map;
    std::multimap<int, int> multimap;
    std::unordered_map<int, int> unorderedMap;
    std::unordered_multimap<int, int> unorderedMultimap;
    
    EXPECT_TRUE(isEmpty(vector));
    EXPECT_TRUE(isEmpty(deque));
    EXPECT_TRUE(isEmpty(forwardList));
    EXPECT_TRUE(isEmpty(list));
    EXPECT_TRUE(isEmpty(stack));
    EXPECT_TRUE(isEmpty(queue));
    EXPECT_TRUE(isEmpty(priorityQueue));
    EXPECT_TRUE(isEmpty(set));
    EXPECT_TRUE(isEmpty(multiset));
    EXPECT_TRUE(isEmpty(unorderedSet));
    EXPECT_TRUE(isEmpty(unorderedMultiset));
    EXPECT_TRUE(isEmpty(map));
    EXPECT_TRUE(isEmpty(multimap));
    EXPECT_TRUE(isEmpty(unorderedMap));
    EXPECT_TRUE(isEmpty(unorderedMultimap));

    vector.push_back(0);
    deque.push_back(0);
    forwardList.push_front(0);
    list.push_back(0);
    stack.push(0);
    queue.push(0);
    priorityQueue.push(0);
    set.insert(0);
    multiset.insert(0);
    unorderedSet.insert(0);
    unorderedMultiset.insert(0);
    map.insert(std::pair<int, int>(0, 0));
    multimap.insert(std::pair<int, int>(0, 0));
    unorderedMap.insert(std::pair<int, int>(0, 0));
    unorderedMultimap.insert(std::pair<int, int>(0, 0));
    
    EXPECT_FALSE(isEmpty(vector));
    EXPECT_FALSE(isEmpty(deque));
    EXPECT_FALSE(isEmpty(forwardList));
    EXPECT_FALSE(isEmpty(list));
    EXPECT_FALSE(isEmpty(stack));
    EXPECT_FALSE(isEmpty(queue));
    EXPECT_FALSE(isEmpty(priorityQueue));
    EXPECT_FALSE(isEmpty(set));
    EXPECT_FALSE(isEmpty(multiset));
    EXPECT_FALSE(isEmpty(unorderedSet));
    EXPECT_FALSE(isEmpty(unorderedMultiset));
    EXPECT_FALSE(isEmpty(map));
    EXPECT_FALSE(isEmpty(multimap));
    EXPECT_FALSE(isEmpty(unorderedMap));
    EXPECT_FALSE(isEmpty(unorderedMultimap));
}

TEST(RareTsTest, Clear)
{
    std::vector<int> vector = { 0, 0, 0 };
    std::deque<int> deque = { 0, 0, 0 };
    std::forward_list<int> forwardList = { 0, 0, 0 };
    std::list<int> list = { 0, 0, 0 };
    std::stack<int> stack;
    std::queue<int> queue;
    std::priority_queue<int> priorityQueue;
    for ( size_t i=0; i<3; i++ )
    {
        stack.push(0);
        queue.push(0);
        priorityQueue.push(0);
    }
    std::set<int> set = { 0, 0, 0 };
    std::multiset<int> multiset = { 0, 0, 0 };
    std::unordered_set<int> unorderedSet = { 0, 0, 0 };
    std::unordered_multiset<int> unorderedMultiset = { 0, 0, 0 };
    std::map<int, int> map = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
    std::multimap<int, int> multimap = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
    std::unordered_map<int, int> unorderedMap = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
    std::unordered_multimap<int, int> unorderedMultimap = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
    
    EXPECT_FALSE(vector.empty());
    EXPECT_FALSE(deque.empty());
    EXPECT_FALSE(forwardList.empty());
    EXPECT_FALSE(list.empty());
    EXPECT_FALSE(stack.empty());
    EXPECT_FALSE(queue.empty());
    EXPECT_FALSE(priorityQueue.empty());
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(multiset.empty());
    EXPECT_FALSE(unorderedSet.empty());
    EXPECT_FALSE(unorderedMultiset.empty());
    EXPECT_FALSE(map.empty());
    EXPECT_FALSE(multimap.empty());
    EXPECT_FALSE(unorderedMap.empty());
    EXPECT_FALSE(unorderedMultimap.empty());
    
    clear(vector);
    clear(deque);
    clear(forwardList);
    clear(list);
    clear(stack);
    clear(queue);
    clear(priorityQueue);
    clear(set);
    clear(multiset);
    clear(unorderedSet);
    clear(unorderedMultiset);
    clear(map);
    clear(multimap);
    clear(unorderedMap);
    clear(unorderedMultimap);
    
    EXPECT_TRUE(vector.empty());
    EXPECT_TRUE(deque.empty());
    EXPECT_TRUE(forwardList.empty());
    EXPECT_TRUE(list.empty());
    EXPECT_TRUE(stack.empty());
    EXPECT_TRUE(queue.empty());
    EXPECT_TRUE(priorityQueue.empty());
    EXPECT_TRUE(set.empty());
    EXPECT_TRUE(multiset.empty());
    EXPECT_TRUE(unorderedSet.empty());
    EXPECT_TRUE(unorderedMultiset.empty());
    EXPECT_TRUE(map.empty());
    EXPECT_TRUE(multimap.empty());
    EXPECT_TRUE(unorderedMap.empty());
    EXPECT_TRUE(unorderedMultimap.empty());
}

TEST(RareTsTest, IsAssignable)
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
    l1 = r1;
    EXPECT_EQ(l1.a, 1);
    EXPECT_EQ(r1.a, 1);

    bool isAssignable = RareTs::is_assignable_v<L1, R1>;
    EXPECT_TRUE(isAssignable); // Assignment operator should signal assignability
    isAssignable = RareTs::is_assignable_v<R1, L1>;
    EXPECT_FALSE(isAssignable);

    struct R2
    {
        int a;
    };
    struct L2
    {
        int a;
        L2 & operator=(const R2 && rhs) { a = rhs.a; return *this; };
    };

    isAssignable = RareTs::is_assignable_v<L2, R2>;
    EXPECT_FALSE(isAssignable); // Move-assignment operator should not signal assignability
    isAssignable = RareTs::is_assignable_v<R2, L2>;
    EXPECT_FALSE(isAssignable);

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
    isAssignable = RareTs::is_assignable_v<L3, R3>;
    EXPECT_TRUE(isAssignable); // Converting-constructor should signal assignability
    l3 = r3;
    EXPECT_EQ(l3.a, 2);
    isAssignable = RareTs::is_assignable_v<R3, L3>;
    EXPECT_FALSE(isAssignable);

    struct R4
    {
        int a;
    };
    struct L4
    {
        int a;
        L4() : a(0) {};
        L4(const R4 &&) { a = 2; };
    };

    isAssignable = RareTs::is_assignable_v<L4, R4>;
    EXPECT_FALSE(isAssignable); // Move-constructor should not signal assignability
    isAssignable = RareTs::is_assignable_v<R4, L4>;
    EXPECT_FALSE(isAssignable);

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

    isAssignable = RareTs::is_assignable_v<L5, R5>;
    EXPECT_TRUE(isAssignable); // Conversion op should signal assignability
    isAssignable = RareTs::is_assignable_v<R5, L5>;
    EXPECT_FALSE(isAssignable);
    l5 = r5;
    EXPECT_EQ(l5.a, 1);
    EXPECT_EQ(r5.a, 1);

    struct L6
    {
        int a;
    };
    struct R6
    {
        int a;
        explicit operator L6() const { return L6{ a }; }
    };

    L6 l6{ 0 };
    R6 r6{ 1 };
    EXPECT_EQ(l6.a, 0);
    EXPECT_EQ(r6.a, 1);

    isAssignable = RareTs::is_assignable_v<L6, R6>;
    EXPECT_FALSE(isAssignable); // Explicit conversion op should not signal assignability
    isAssignable = RareTs::is_assignable_v<R6, L6>;
    EXPECT_FALSE(isAssignable);
}

TEST(RareTsTest, IsStaticCastAssignable)
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

    bool isAssignable = RareTs::is_static_cast_assignable_v<L1, R1>;
    EXPECT_FALSE(isAssignable); // Assignment operator should not signal castability
    isAssignable = RareTs::is_static_cast_assignable_v<R1, L1>;
    EXPECT_FALSE(isAssignable);

    struct R2
    {
        int a;
    };
    struct L2
    {
        int a;
        L2 & operator=(const R2 && rhs) { a = rhs.a; return *this; };
    };

    isAssignable = RareTs::is_static_cast_assignable_v<L2, R2>;
    EXPECT_FALSE(isAssignable); // Move-assignment operator should not signal castability
    isAssignable = RareTs::is_static_cast_assignable_v<R2, L2>;
    EXPECT_FALSE(isAssignable);

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
    isAssignable = RareTs::is_static_cast_assignable_v<L3, R3>;
    EXPECT_TRUE(isAssignable); // Converting-constructor should signal static-cast assignability
    l3 = static_cast<L3>(r3);
    EXPECT_EQ(l3.a, 2);
    isAssignable = RareTs::is_static_cast_assignable_v<R3, L3>;
    EXPECT_FALSE(isAssignable);

    struct R4
    {
        int a;
    };
    struct L4
    {
        int a;
        L4() : a(0) {};
        L4(const R4 &&) { a = 2; };
    };

    isAssignable = RareTs::is_static_cast_assignable_v<L4, R4>;
    EXPECT_FALSE(isAssignable); // Move-constructor should not signal castability
    isAssignable = RareTs::is_static_cast_assignable_v<R4, L4>;
    EXPECT_FALSE(isAssignable);

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

    isAssignable = RareTs::is_static_cast_assignable_v<L5, R5>;
    EXPECT_TRUE(isAssignable); // Conversion op should signal static_cast assignability
    isAssignable = RareTs::is_static_cast_assignable_v<R5, L5>;
    EXPECT_FALSE(isAssignable);
    l5 = static_cast<L5>(r5);
    EXPECT_EQ(l5.a, 1);
    EXPECT_EQ(r5.a, 1);

    struct L6
    {
        int a;
    };
    struct R6
    {
        int a;
        explicit operator L6() const { return L6{ a }; }
    };

    L6 l6{ 0 };
    R6 r6{ 1 };
    EXPECT_EQ(l6.a, 0);
    EXPECT_EQ(r6.a, 1);

    isAssignable = RareTs::is_static_cast_assignable_v<L6, R6>;
    EXPECT_TRUE(isAssignable); // Explicit conversion op should signal static-cast assignability
    isAssignable = RareTs::is_static_cast_assignable_v<R6, L6>;
    EXPECT_FALSE(isAssignable);
    l6 = static_cast<L6>(r6);
    EXPECT_EQ(l6.a, 1);
    EXPECT_EQ(r6.a, 1);
}

struct IsSpecializedTest {};

struct IsUnspecializedTest : RareTs::Unspecialized {};

TEST(RareTsTest, IsSpecialized)
{
    EXPECT_TRUE(RareTs::is_specialized<IsSpecializedTest>::value);
    EXPECT_FALSE(RareTs::is_specialized<IsUnspecializedTest>::value);
    EXPECT_TRUE(RareTs::is_specialized_v<IsSpecializedTest>);
    EXPECT_FALSE(RareTs::is_specialized_v<IsUnspecializedTest>);
}

TEST(RareTsTest, ForIndex)
{
    bool visited = false;
    forIndex<0>(0, [&](auto) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    visited = false;
    forIndex<0>(1, [&](auto) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    int visitCount = 0;
    forIndex<1>(0, [&](auto I) {
        visitCount++;
        EXPECT_EQ(size_t(0), decltype(I)::value);
    });
    EXPECT_EQ(1, visitCount);

    visited = false;
    forIndex<1>(1, [&](auto) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    visitCount = 0;
    forIndex<2>(0, [&](auto I) {
        visitCount++;
        EXPECT_EQ(size_t(0), decltype(I)::value);
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    forIndex<2>(1, [&](auto I) {
        visitCount++;
        EXPECT_EQ(size_t(1), decltype(I)::value);
    });
    EXPECT_EQ(1, visitCount);

    visited = false;
    forIndex<2>(2, [&](auto) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    visitCount = 0;
    forIndex<256>(0, [&](auto I) {
        visitCount++;
        EXPECT_EQ(size_t(0), decltype(I)::value);
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    forIndex<256>(128, [&](auto I) {
        visitCount++;
        EXPECT_EQ(size_t(128), decltype(I)::value);
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    forIndex<256>(255, [&](auto I) {
        visitCount++;
        EXPECT_EQ(size_t(255), decltype(I)::value);
    });
    EXPECT_EQ(1, visitCount);

    visited = false;
    forIndex<256>(256, [&](auto) {
        visited = true;
    });
    EXPECT_FALSE(visited);
}

TEST(RareTsTest, ForIndexes)
{
    bool visited = false;
    forIndexes<0>([&](auto) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    int visitCount = 0;
    forIndexes<1>([&](auto I) {
        switch ( visitCount ) {
            case 0: EXPECT_EQ(size_t(0), decltype(I)::value); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
     
    visitCount = 0;
    forIndexes<2>([&](auto I) {
        switch ( visitCount ) {
            case 0: EXPECT_EQ(size_t(0), decltype(I)::value); break;
            case 1: EXPECT_EQ(size_t(1), decltype(I)::value); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);

    visitCount = 0;
    forIndexes<3>([&](auto I) {
        switch ( visitCount ) {
            case 0: EXPECT_EQ(size_t(0), decltype(I)::value); break;
            case 1: EXPECT_EQ(size_t(1), decltype(I)::value); break;
            case 2: EXPECT_EQ(size_t(2), decltype(I)::value); break;
            default: EXPECT_TRUE(false); break;
        }
        visitCount++;
    });
    EXPECT_EQ(3, visitCount);

    visitCount = 0;
    forIndexes<128>([&](auto I) {
        EXPECT_EQ(size_t(visitCount), decltype(I)::value);
        visitCount++;
    });
    EXPECT_EQ(128, visitCount);
}

TEST(RareTsTest, PackIndexes)
{
    bool visited = false;
    packIndexes<0>([&](auto) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    visited = false;
    packIndexes<1>([&](auto I) {
        EXPECT_EQ(0, decltype(I)::value);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    packIndexes<2>([&](auto zero, auto one) {
        EXPECT_EQ(0, decltype(zero)::value);
        EXPECT_EQ(1, decltype(one)::value);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    packIndexes<3>([&](auto zero, auto one, auto two) {
        EXPECT_EQ(0, decltype(zero)::value);
        EXPECT_EQ(1, decltype(one)::value);
        EXPECT_EQ(2, decltype(two)::value);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    packIndexes(std::index_sequence<>{}, [&]() {
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    packIndexes(std::index_sequence<0>{}, [&](auto I) {
        EXPECT_EQ(0, decltype(I)::value);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    packIndexes(std::index_sequence<0, 1>{}, [&](auto zero, auto one) {
        EXPECT_EQ(0, decltype(zero)::value);
        EXPECT_EQ(1, decltype(one)::value);
        visited = true;
    });
    EXPECT_TRUE(visited);

    visited = false;
    packIndexes(std::index_sequence<3, 0, 7, 9>{}, [&](auto three, auto zero, auto seven, auto nine) {
        EXPECT_EQ(3, decltype(three)::value);
        EXPECT_EQ(0, decltype(zero)::value);
        EXPECT_EQ(7, decltype(seven)::value);
        EXPECT_EQ(9, decltype(nine)::value);
        visited = true;
    });
    EXPECT_TRUE(visited);
}

TEST(RareTsTest, IndexOf)
{
    using Tup = std::tuple<int, char, bool, short>;
    auto tupIntIndex = RareTs::index_of_v<int, Tup>;
    auto tupCharIndex = RareTs::index_of_v<char, Tup>;
    auto tupBoolIndex = RareTs::index_of_v<bool, Tup>;
    auto tupShortIndex = RareTs::index_of_v<short, Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<const int, Tup>;
    tupCharIndex = RareTs::index_of_v<const char, Tup>;
    tupBoolIndex = RareTs::index_of_v<const bool, Tup>;
    tupShortIndex = RareTs::index_of_v<const short, Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<int &, Tup>;
    tupCharIndex = RareTs::index_of_v<char &, Tup>;
    tupBoolIndex = RareTs::index_of_v<bool &, Tup>;
    tupShortIndex = RareTs::index_of_v<short &, Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<const int &, Tup>;
    tupCharIndex = RareTs::index_of_v<const char &, Tup>;
    tupBoolIndex = RareTs::index_of_v<const bool &, Tup>;
    tupShortIndex = RareTs::index_of_v<const short &, Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<int, Tup>;
    tupCharIndex = RareTs::index_of_v<char, Tup>;
    tupBoolIndex = RareTs::index_of_v<bool, Tup>;
    tupShortIndex = RareTs::index_of_v<short, Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<const int, Tup>;
    tupCharIndex = RareTs::index_of_v<const char, Tup>;
    tupBoolIndex = RareTs::index_of_v<const bool, Tup>;
    tupShortIndex = RareTs::index_of_v<const short, Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<int &, Tup>;
    tupCharIndex = RareTs::index_of_v<char &, Tup>;
    tupBoolIndex = RareTs::index_of_v<bool &, Tup>;
    tupShortIndex = RareTs::index_of_v<short &, Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<const int &, Tup>;
    tupCharIndex = RareTs::index_of_v<const char &, Tup>;
    tupBoolIndex = RareTs::index_of_v<const bool &, Tup>;
    tupShortIndex = RareTs::index_of_v<const short &, Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<int, const Tup>;
    tupCharIndex = RareTs::index_of_v<char, const Tup>;
    tupBoolIndex = RareTs::index_of_v<bool, const Tup>;
    tupShortIndex = RareTs::index_of_v<short, const Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<const int, const Tup>;
    tupCharIndex = RareTs::index_of_v<const char, const Tup>;
    tupBoolIndex = RareTs::index_of_v<const bool, const Tup>;
    tupShortIndex = RareTs::index_of_v<const short, const Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<int &, const Tup>;
    tupCharIndex = RareTs::index_of_v<char &, const Tup>;
    tupBoolIndex = RareTs::index_of_v<bool &, const Tup>;
    tupShortIndex = RareTs::index_of_v<short &, const Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<const int &, const Tup>;
    tupCharIndex = RareTs::index_of_v<const char &, const Tup>;
    tupBoolIndex = RareTs::index_of_v<const bool &, const Tup>;
    tupShortIndex = RareTs::index_of_v<const short &, const Tup>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<int, Tup &>;
    tupCharIndex = RareTs::index_of_v<char, Tup &>;
    tupBoolIndex = RareTs::index_of_v<bool, Tup &>;
    tupShortIndex = RareTs::index_of_v<short, Tup &>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<const int, Tup &>;
    tupCharIndex = RareTs::index_of_v<const char, Tup &>;
    tupBoolIndex = RareTs::index_of_v<const bool, Tup &>;
    tupShortIndex = RareTs::index_of_v<const short, Tup &>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<int &, Tup &>;
    tupCharIndex = RareTs::index_of_v<char &, Tup &>;
    tupBoolIndex = RareTs::index_of_v<bool &, Tup &>;
    tupShortIndex = RareTs::index_of_v<short &, Tup &>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
    
    tupIntIndex = RareTs::index_of_v<const int &, const Tup &>;
    tupCharIndex = RareTs::index_of_v<const char &, const Tup &>;
    tupBoolIndex = RareTs::index_of_v<const bool &, const Tup &>;
    tupShortIndex = RareTs::index_of_v<const short &, const Tup &>;
    EXPECT_EQ(size_t(0), tupIntIndex);
    EXPECT_EQ(size_t(1), tupCharIndex);
    EXPECT_EQ(size_t(2), tupBoolIndex);
    EXPECT_EQ(size_t(3), tupShortIndex);
}

TEST(RareTsTest, MaskedIndexOf)
{
    size_t index = Masked<>::indexOf<void, std::tuple<>>;
    EXPECT_EQ(0, index); // Not found
    index = Masked<>::indexOf<void, std::tuple<void>>;
    EXPECT_EQ(1, index); // Not found
    index = Masked<>::indexOf<void, std::tuple<void, int>>;
    EXPECT_EQ(2, index); // Not found
    index = Masked<>::indexOf<void, std::tuple<int, void>>;
    EXPECT_EQ(2, index); // Not found
    
    index = Masked<0>::indexOf<void, std::tuple<void>>;
    EXPECT_EQ(0, index); // Found
    index = Masked<0>::indexOf<void, std::tuple<void, int>>;
    EXPECT_EQ(0, index); // Found
    index = Masked<0>::indexOf<void, std::tuple<int, void>>;
    EXPECT_EQ(2, index); // Not found
    
    index = Masked<1>::indexOf<void, std::tuple<void, int>>;
    EXPECT_EQ(2, index); // Not found
    index = Masked<1>::indexOf<void, std::tuple<int, void>>;
    EXPECT_EQ(1, index); // Found
    
    index = Masked<0, 1>::indexOf<void, std::tuple<void, int>>;
    EXPECT_EQ(0, index); // Found
    index = Masked<0, 1>::indexOf<void, std::tuple<int, void>>;
    EXPECT_EQ(1, index); // Found
    
    index = Masked<0, 2, 4, 6>::indexOf<int, std::tuple<float, int, float, int, float, int, float, int>>;
    EXPECT_EQ(8, index); // Not found

    index = Masked<0, 2, 4, 6>::indexOf<int, std::tuple<float, int, float, int, float, int, int, int>>;
    EXPECT_EQ(6, index); // Found
}

TEST(RareTsTest, MaskedIndexOfSpecialization)
{
    size_t index = Masked<>::indexOfSpecialization<RareTs::type_id, std::tuple<>>;
    EXPECT_EQ(0, index); // Not found
    index = Masked<>::indexOfSpecialization<RareTs::type_id, std::tuple<RareTs::type_id<void>>>;
    EXPECT_EQ(1, index); // Not found
    index = Masked<>::indexOfSpecialization<RareTs::type_id, std::tuple<RareTs::type_id<void>, int>>;
    EXPECT_EQ(2, index); // Not found
    index = Masked<>::indexOfSpecialization<RareTs::type_id, std::tuple<int, RareTs::type_id<void>>>;
    EXPECT_EQ(2, index); // Not found
    
    index = Masked<0>::indexOfSpecialization<RareTs::type_id, std::tuple<RareTs::type_id<void>>>;
    EXPECT_EQ(0, index); // Found
    index = Masked<0>::indexOfSpecialization<RareTs::type_id, std::tuple<RareTs::type_id<void>, int>>;
    EXPECT_EQ(0, index); // Found
    index = Masked<0>::indexOfSpecialization<RareTs::type_id, std::tuple<int, RareTs::type_id<void>>>;
    EXPECT_EQ(2, index); // Not found
    
    index = Masked<1>::indexOfSpecialization<RareTs::type_id, std::tuple<RareTs::type_id<void>, int>>;
    EXPECT_EQ(2, index); // Not found
    index = Masked<1>::indexOfSpecialization<RareTs::type_id, std::tuple<int, RareTs::type_id<void>>>;
    EXPECT_EQ(1, index); // Found
    
    index = Masked<0, 1>::indexOfSpecialization<RareTs::type_id, std::tuple<RareTs::type_id<void>, int>>;
    EXPECT_EQ(0, index); // Found
    index = Masked<0, 1>::indexOfSpecialization<RareTs::type_id, std::tuple<int, RareTs::type_id<void>>>;
    EXPECT_EQ(1, index); // Found
    
    index = Masked<0, 2, 4, 6>::indexOfSpecialization<RareTs::type_id, std::tuple<
        float, RareTs::type_id<void>, float, RareTs::type_id<void>,
        float, RareTs::type_id<void>, float, RareTs::type_id<void>
    >>;
    EXPECT_EQ(8, index); // Not found

    index = Masked<0, 2, 4, 6>::indexOfSpecialization<RareTs::type_id, std::tuple<
        float, RareTs::type_id<void>, float, RareTs::type_id<void>,
        float, RareTs::type_id<void>, RareTs::type_id<void>, RareTs::type_id<void>>>;
    EXPECT_EQ(6, index); // Found
}

TEST(RareTsTest, MaskedGet)
{
    constexpr auto tup = std::tuple { 'a', 'b', 1337 };
    auto findIntAll = Masked<0, 1, 2>::get<int>(tup);
    EXPECT_EQ(1337, findIntAll);
    auto findIntOneTwo = Masked<1, 2>::get<int>(tup);
    EXPECT_EQ(1337, findIntOneTwo);
    auto findIntTwo = Masked<2>::get<int>(tup);
    EXPECT_EQ(1337, findIntTwo);
    
    auto findCharAll = Masked<0, 1, 2>::get<char>(tup);
    EXPECT_EQ('a', findCharAll);
    auto findCharOneTwo = Masked<1, 2>::get<char>(tup);
    EXPECT_EQ('b', findCharOneTwo);
    auto findCharZeroTwo = Masked<0, 2>::get<char>(tup);
    EXPECT_EQ('a', findCharZeroTwo);

    constexpr auto specTup = std::tuple { RareTs::type_id<void>{}, 1337, RareTs::type_id<int>{} };
    auto findSpecsAll = Masked<0, 1, 2>::get<RareTs::type_id>(specTup);
    bool isSame = std::is_same_v<void, typename decltype(findSpecsAll)::type>;
    EXPECT_TRUE(isSame);
    auto findSpecsOneTwo = Masked<1, 2>::get<RareTs::type_id>(specTup);
    isSame = std::is_same_v<int, typename decltype(findSpecsOneTwo)::type>;
    EXPECT_TRUE(isSame);
}

TEST(RareTsTest, MaskedForEach)
{
    constexpr auto tup = std::tuple { 'a', 'b', 1337 };
    size_t visitCount = 0;
    Masked<0, 1, 2>::forEach<int>(tup, [&](auto value) {
        EXPECT_EQ(1337, value);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    Masked<2>::forEach<int>(tup, [&](auto value) {
        EXPECT_EQ(1337, value);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    Masked<0, 1>::forEach<int>(tup, [&](auto) {
        visitCount++;
    });
    EXPECT_EQ(0, visitCount);
    
    visitCount = 0;
    Masked<0, 1, 2>::forEach<char>(tup, [&](auto value) {
        EXPECT_EQ('a'+visitCount, value);
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    Masked<0, 1>::forEach<char>(tup, [&](auto value) {
        EXPECT_EQ('a'+visitCount, value);
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);
    
    visitCount = 0;
    Masked<1>::forEach<char>(tup, [&](auto value) {
        EXPECT_EQ('b', value);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
    
    visitCount = 0;
    Masked<0>::forEach<char>(tup, [&](auto value) {
        EXPECT_EQ('a', value);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);

    constexpr auto specTup = std::tuple { RareTs::type_id<void>{}, 1337, RareTs::type_id<int>{} };

    visitCount = 0;
    Masked<0, 1, 2>::forEach<RareTs::type_id>(specTup, [&](auto value) {
        bool isSame = visitCount == 0 ? std::is_same_v<void, typename decltype(value)::type> :
            std::is_same_v<int, typename decltype(value)::type>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);

    visitCount = 0;
    Masked<0, 1>::forEach<RareTs::type_id>(specTup, [&](auto value) {
        bool isSame = std::is_same_v<void, typename decltype(value)::type>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);

    visitCount = 0;
    Masked<1, 2>::forEach<RareTs::type_id>(specTup, [&](auto value) {
        bool isSame = std::is_same_v<int, typename decltype(value)::type>;
        EXPECT_TRUE(isSame);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);
}

TEST(RareTsTest, RemoveVoids)
{
    bool isSame = std::is_same_v<std::tuple<>, RareTs::remove_voids_t<std::tuple<>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<>, RareTs::remove_voids_t<std::tuple<void>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<>, RareTs::remove_voids_t<std::tuple<void, void>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<>, RareTs::remove_voids_t<std::tuple<void, void, void>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int>, RareTs::remove_voids_t<std::tuple<int>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int>, RareTs::remove_voids_t<std::tuple<int, void>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int>, RareTs::remove_voids_t<std::tuple<void, int>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, int>, RareTs::remove_voids_t<std::tuple<int, int>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, int>, RareTs::remove_voids_t<std::tuple<int, int, void>>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::tuple<int, int>, RareTs::remove_voids_t<std::tuple<void, int, int>>>;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same_v<std::tuple<char, int, float, double>, RareTs::remove_voids_t<std::tuple<char, int, void, float, double, void>>>;
    EXPECT_TRUE(isSame);
}

TEST(RareTsTest, TypeListHas)
{
    bool typeListHas = type_list<>::has<void>::value;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<void>::has<void>::value;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<void>::has<int>::value;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<int>::has<void>::value;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<int>::has<int>::value;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<void, void>::has<void>::value;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<void, void>::has<int>::value;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<void, int>::has<void>::value;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<void, int>::has<int>::value;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<int, void>::has<void>::value;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<int, void>::has<int>::value;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<int, int>::has<void>::value;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<int, int>::has<int>::value;
    EXPECT_TRUE(typeListHas);

    typeListHas = type_list<void>::has<IsSpecTest<float>>::value;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<IsSpecTest<float>>::has<void>::value;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<IsSpecTest<float>>::has<IsSpecTest<float>>::value;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<IsSpecTest<float>>::has<IsSpecTest<int>>::value;
    EXPECT_FALSE(typeListHas);

    typeListHas = type_list<>::has_v<void>;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<void>::has_v<void>;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<void>::has_v<int>;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<int>::has_v<void>;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<int>::has_v<int>;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<void, void>::has_v<void>;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<void, void>::has_v<int>;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<void, int>::has_v<void>;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<void, int>::has_v<int>;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<int, void>::has_v<void>;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<int, void>::has_v<int>;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<int, int>::has_v<void>;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<int, int>::has_v<int>;
    EXPECT_TRUE(typeListHas);

    typeListHas = type_list<void>::has_v<IsSpecTest<float>>;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<IsSpecTest<float>>::has_v<void>;
    EXPECT_FALSE(typeListHas);
    typeListHas = type_list<IsSpecTest<float>>::has_v<IsSpecTest<float>>;
    EXPECT_TRUE(typeListHas);
    typeListHas = type_list<IsSpecTest<float>>::has_v<IsSpecTest<int>>;
    EXPECT_FALSE(typeListHas);
}

TEST(RareTsTest, TypeListHasSpecialization)
{
    bool hasSpecialization = type_list<>::has_specialization<IsSpecTest>::value;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<void>::has_specialization<IsSpecTest>::value;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<IsSpecTest<int>>::has_specialization<IsSpecTest>::value;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<IsSpecTest<void>>::has_specialization<IsSpecTest>::value;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<IsNotSpecTemplatedTest<int>>::has_specialization<IsSpecTest>::value;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<IsNotSpecTemplatedTest<void>>::has_specialization<IsSpecTest>::value;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<IsNotSpecStructTest>::has_specialization<IsSpecTest>::value;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<int>::has_specialization<IsSpecTest>::value;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<void>::has_specialization<IsSpecTest>::value;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<void, void>::has_specialization<IsSpecTest>::value;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<void, IsSpecTest<void>>::has_specialization<IsSpecTest>::value;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<IsSpecTest<void>, void>::has_specialization<IsSpecTest>::value;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<IsSpecTest<void>, IsSpecTest<void>>::has_specialization<IsSpecTest>::value;
    EXPECT_TRUE(hasSpecialization);

    hasSpecialization = type_list<>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<void>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<IsSpecTest<int>>::has_specialization_v<IsSpecTest>;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<IsSpecTest<void>>::has_specialization_v<IsSpecTest>;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<IsNotSpecTemplatedTest<int>>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<IsNotSpecTemplatedTest<void>>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<IsNotSpecStructTest>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<int>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<void>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<void, void>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<void, IsSpecTest<void>>::has_specialization_v<IsSpecTest>;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<IsSpecTest<void>, void>::has_specialization_v<IsSpecTest>;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<IsSpecTest<void>, IsSpecTest<void>>::has_specialization_v<IsSpecTest>;
    EXPECT_TRUE(hasSpecialization);
}

TEST(RareTsTest, TypeListGetSpecialization)
{
    bool isVoid = std::is_same_v<void, type_list<>::get_specialization<IsSpecTest>::type>;
    EXPECT_TRUE(isVoid);
    isVoid = std::is_same_v<void, type_list<void>::get_specialization<IsSpecTest>::type>;
    EXPECT_TRUE(isVoid);
    isVoid = std::is_same_v<void, type_list<void, void>::get_specialization<IsSpecTest>::type>;
    EXPECT_TRUE(isVoid);

    bool isExpectedSpec = std::is_same_v<IsSpecTest<void>, type_list<IsSpecTest<void>>::get_specialization<IsSpecTest>::type>;
    EXPECT_TRUE(isExpectedSpec);
    isExpectedSpec = std::is_same_v<IsSpecTest<void>, type_list<void, IsSpecTest<void>>::get_specialization<IsSpecTest>::type>;
    EXPECT_TRUE(isExpectedSpec);
    isExpectedSpec = std::is_same_v<IsSpecTest<void>, type_list<IsSpecTest<void>, void>::get_specialization<IsSpecTest>::type>;
    EXPECT_TRUE(isExpectedSpec);

    bool isPrimaryTemplate = type_list<IsSpecTest<void>>::get_specialization<IsSpecTest>::type::IsPrimaryTemplate;
    EXPECT_TRUE(isPrimaryTemplate);
    isPrimaryTemplate = type_list<IsSpecTest<int>>::get_specialization<IsSpecTest>::type::IsPrimaryTemplate;
    EXPECT_FALSE(isPrimaryTemplate);
    isPrimaryTemplate = type_list<IsSpecTest<void>, IsSpecTest<int>>::get_specialization<IsSpecTest>::type::IsPrimaryTemplate;
    EXPECT_TRUE(isPrimaryTemplate);
    isPrimaryTemplate = type_list<IsSpecTest<int>, IsSpecTest<void>>::get_specialization<IsSpecTest>::type::IsPrimaryTemplate;
    EXPECT_FALSE(isPrimaryTemplate);
}

TEST(RareTsTest, TypeListTupleSpecializations)
{
    bool hasInt = type_list<std::tuple<void, void, void>>::has_v<int>;
    EXPECT_FALSE(hasInt);
    hasInt = type_list<const std::tuple<void, void, void>>::has_v<int>;
    EXPECT_FALSE(hasInt);
    hasInt = type_list<std::tuple<void, int, void>>::has_v<int>;
    EXPECT_TRUE(hasInt);
    hasInt = type_list<const std::tuple<void, int, void>>::has_v<int>;
    EXPECT_TRUE(hasInt);

    bool hasSpecialization = type_list<std::tuple<void, void, void>>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<const std::tuple<void, void, void>>::has_specialization_v<IsSpecTest>;
    EXPECT_FALSE(hasSpecialization);
    hasSpecialization = type_list<std::tuple<void, IsSpecTest<void>, void>>::has_specialization_v<IsSpecTest>;
    EXPECT_TRUE(hasSpecialization);
    hasSpecialization = type_list<const std::tuple<void, IsSpecTest<void>, void>>::has_specialization_v<IsSpecTest>;
    EXPECT_TRUE(hasSpecialization);

    bool isSame = std::is_same_v<void, type_list<std::tuple<void, void, void>>::get_specialization_t<IsSpecTest>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<void, type_list<const std::tuple<void, void, void>>::get_specialization_t<IsSpecTest>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<IsSpecTest<void>, type_list<std::tuple<void, IsSpecTest<void>, void>>::get_specialization_t<IsSpecTest>>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<IsSpecTest<void>, type_list<const std::tuple<void, IsSpecTest<void>, void>>::get_specialization_t<IsSpecTest>>;
    EXPECT_TRUE(isSame);
}

struct Unspecializable {};

template <typename T>
struct Specializable {
    T value;
};

TEST(RareTsTest, ToStr)
{
    EXPECT_STREQ("int", std::string(toStr<int>()).c_str());

    // Expecting some junk like how it's a struct or class, but it should still contain the pair text
    constexpr std::string_view typeView = toStr<std::pair<int,int>>();
    std::string typeStr = std::string(typeView);
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("pair<int,int") != std::string::npos);

    // Expecting some junk like how it's a struct or class and what allocator it's using, but should contain the map text
    constexpr std::string_view mapStrView = toStr<std::map<int,int>>();
    std::string mapStr = std::string(mapStrView);
    mapStr.erase(std::remove(mapStr.begin(), mapStr.end(), ' '), mapStr.end());
    EXPECT_TRUE(mapStr.find("map<int,int") != std::string::npos);
}

TEST(RareTsTest, ConstexprTypeToStr)
{
    EXPECT_STREQ("int", TypeName<int>().value);

    // Expecting some junk like how it's a struct or class, but it should still contain the pair text
    auto pairStrStruct = TypeName<std::pair<int,int>>();
    std::string typeStr = pairStrStruct.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("pair<int,int") != std::string::npos);

    // Expecting some junk like how it's a struct or class and what allocator it's using, but should contain the map text
    auto mapStrStruct = TypeName<std::map<int,int>>();
    std::string mapStr = mapStrStruct.value;
    mapStr.erase(std::remove(mapStr.begin(), mapStr.end(), ' '), mapStr.end());
    EXPECT_TRUE(mapStr.find("map<int,int") != std::string::npos);
}

TEST(RareTsTest, GetUnderlyingContainer)
{
    // Stacks
    std::stack<int> defaultStack;
    defaultStack.push(0);
    defaultStack.push(1);
    auto stackDefaultUnderlyingContainer = baseContainer(defaultStack);
    EXPECT_EQ(0, *stackDefaultUnderlyingContainer.begin());
    EXPECT_EQ(1, *++stackDefaultUnderlyingContainer.begin());

    std::stack<int, std::vector<int>> vectorStack;
    vectorStack.push(2);
    vectorStack.push(3);
    auto stackUnderlyingVector = baseContainer(vectorStack);
    bool isEqual = std::is_same_v<std::vector<int>, decltype(stackUnderlyingVector)>;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(2, *stackUnderlyingVector.begin());
    EXPECT_EQ(3, *++stackUnderlyingVector.begin());

    std::stack<int, std::deque<int>> dequeStack;
    dequeStack.push(4);
    dequeStack.push(5);
    auto stackUnderlyingDeque = baseContainer(dequeStack);
    isEqual = std::is_same_v<std::deque<int>, decltype(stackUnderlyingDeque)>;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(4, *stackUnderlyingDeque.begin());
    EXPECT_EQ(5, *++stackUnderlyingDeque.begin());

    std::stack<int, std::list<int>> listStack;
    listStack.push(6);
    listStack.push(7);
    auto stackUnderlyingList = baseContainer(listStack);
    isEqual = std::is_same_v<std::list<int>, decltype(stackUnderlyingList)>;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(6, *stackUnderlyingList.begin());
    EXPECT_EQ(7, *++stackUnderlyingList.begin());
    
    // Queues
    std::queue<int> defaultQueue;
    defaultQueue.push(0);
    defaultQueue.push(1);
    auto queueDefaultUnderlyingContainer = baseContainer(defaultQueue);
    EXPECT_EQ(0, *queueDefaultUnderlyingContainer.begin());
    EXPECT_EQ(1, *++queueDefaultUnderlyingContainer.begin());

    std::queue<int, std::vector<int>> vectorQueue;
    vectorQueue.push(2);
    vectorQueue.push(3);
    auto queueUnderlyingVector = baseContainer(vectorQueue);
    isEqual = std::is_same_v<std::vector<int>, decltype(queueUnderlyingVector)>;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(2, *queueUnderlyingVector.begin());
    EXPECT_EQ(3, *++queueUnderlyingVector.begin());

    std::queue<int, std::deque<int>> dequeQueue;
    dequeQueue.push(4);
    dequeQueue.push(5);
    auto queueUnderlyingDeque = baseContainer(dequeQueue);
    isEqual = std::is_same_v<std::deque<int>, decltype(queueUnderlyingDeque)>;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(4, *queueUnderlyingDeque.begin());
    EXPECT_EQ(5, *++queueUnderlyingDeque.begin());

    std::queue<int, std::list<int>> listQueue;
    listQueue.push(6);
    listQueue.push(7);
    auto queueUnderlyingList = baseContainer(listQueue);
    isEqual = std::is_same_v<std::list<int>, decltype(queueUnderlyingList)>;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(6, *queueUnderlyingList.begin());
    EXPECT_EQ(7, *++queueUnderlyingList.begin());

    // Priority Queues
    std::priority_queue<int> defaultPriorityQueue;
    defaultPriorityQueue.push(0);
    defaultPriorityQueue.push(1);
    auto priorityQueueDefaultUnderlyingContainer = baseContainer(defaultPriorityQueue);
    EXPECT_EQ(1, *priorityQueueDefaultUnderlyingContainer.begin());
    EXPECT_EQ(0, *++priorityQueueDefaultUnderlyingContainer.begin());

    std::priority_queue<int, std::vector<int>> vectorPriorityQueue;
    vectorPriorityQueue.push(2);
    vectorPriorityQueue.push(3);
    auto priorityQueueUnderlyingVector = baseContainer(vectorPriorityQueue);
    isEqual = std::is_same_v<std::vector<int>, decltype(priorityQueueUnderlyingVector)>;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(3, *priorityQueueUnderlyingVector.begin());
    EXPECT_EQ(2, *++priorityQueueUnderlyingVector.begin());

    std::priority_queue<int, std::deque<int>> dequePriorityQueue;
    dequePriorityQueue.push(4);
    dequePriorityQueue.push(5);
    auto priorityQueueUnderlyingDeque = baseContainer(dequePriorityQueue);
    isEqual = std::is_same_v<std::deque<int>, decltype(priorityQueueUnderlyingDeque)>;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(5, *priorityQueueUnderlyingDeque.begin());
    EXPECT_EQ(4, *++priorityQueueUnderlyingDeque.begin());
}

TEST(RareTsTest, Fnv1aHash)
{
    std::string_view empty = "";
    std::string_view a = "a";
    constexpr std::string_view c = "cexp";
    size_t emptyHash = fnv1aHash(empty);
    size_t aHash = fnv1aHash(a);
    constexpr size_t cHash = fnv1aHash(c);
    EXPECT_GT(emptyHash, size_t(0));
    EXPECT_GT(aHash, size_t(0));
    EXPECT_GT(cHash, size_t(0));
}

TEST(RareTsTest, StringIndexMapping)
{
    static constexpr const char a_[] = "a";
    static constexpr const char b_[] = "b";
    static constexpr const char c_[] = "c";
    static constexpr const char d_[] = "d";
    static constexpr const char e_[] = "e";
    static constexpr const char f_[] = "f";

    using ZeroStrings = StringIndexMap<>;
    EXPECT_EQ(size_t(0), ZeroStrings::total);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), ZeroStrings::indexOf(""));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), ZeroStrings::indexOf("a"));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), ZeroStrings::indexOf("asdf"));

    static constexpr const char emptyString_[] = "";
    using EmptyString = StringIndexMap<emptyString_>;
    constexpr size_t emptyStringTotal = EmptyString::total;
    EXPECT_EQ(size_t(1), emptyStringTotal);
    EXPECT_EQ(size_t(0), EmptyString::indexOf(""));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), EmptyString::indexOf("a"));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), EmptyString::indexOf("asdf"));
    
    using AString = StringIndexMap<a_>;
    constexpr size_t aStringTotal = AString::total;
    EXPECT_EQ(size_t(1), aStringTotal);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), AString::indexOf(""));
    EXPECT_EQ(size_t(0), AString::indexOf("a"));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), AString::indexOf("asdf"));
    
    static constexpr const char asdf_[] = "asdf";
    using AsdfString = StringIndexMap<asdf_>;
    constexpr size_t asdfStringTotal = AsdfString::total;
    EXPECT_EQ(size_t(1), asdfStringTotal);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), AsdfString::indexOf(""));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), AsdfString::indexOf("a"));
    EXPECT_EQ(size_t(0), AsdfString::indexOf("asdf"));

    using Ab = StringIndexMap<a_, b_>;
    constexpr size_t abTotal = Ab::total;
    EXPECT_EQ(size_t(2), abTotal);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Ab::indexOf(""));
    EXPECT_EQ(size_t(0), Ab::indexOf("a"));
    EXPECT_EQ(size_t(1), Ab::indexOf("b"));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Ab::indexOf("ab"));

    using Ba = StringIndexMap<b_, a_>;
    constexpr size_t baTotal = Ba::total;
    EXPECT_EQ(size_t(2), baTotal);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Ba::indexOf(""));
    EXPECT_EQ(size_t(0), Ba::indexOf("b"));
    EXPECT_EQ(size_t(1), Ba::indexOf("a"));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Ba::indexOf("ba"));

    using Abc = StringIndexMap<a_, b_, c_>;
    constexpr size_t abcTotal = Abc::total;
    EXPECT_EQ(size_t(3), abcTotal);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Abc::indexOf(""));
    EXPECT_EQ(size_t(0), Abc::indexOf("a"));
    EXPECT_EQ(size_t(1), Abc::indexOf("b"));
    EXPECT_EQ(size_t(2), Abc::indexOf("c"));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Abc::indexOf("ab"));

    using Bac = StringIndexMap<b_, a_, c_>;
    constexpr size_t bacTotal = Bac::total;
    EXPECT_EQ(size_t(3), bacTotal);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Bac::indexOf(""));
    EXPECT_EQ(size_t(0), Bac::indexOf("b"));
    EXPECT_EQ(size_t(1), Bac::indexOf("a"));
    EXPECT_EQ(size_t(2), Bac::indexOf("c"));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Bac::indexOf("B"));
    
    static constexpr const char A_[] = "A";
    static constexpr const char a_underscore[] = "a_";
    using Aaa_ = StringIndexMap<A_, a_, a_underscore>;
    constexpr size_t Aaa_Total = Aaa_::total;
    EXPECT_EQ(size_t(3), Aaa_Total);
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Aaa_::indexOf(""));
    EXPECT_EQ(size_t(0), Aaa_::indexOf("A"));
    EXPECT_EQ(size_t(1), Aaa_::indexOf("a"));
    EXPECT_EQ(size_t(2), Aaa_::indexOf("a_"));
    EXPECT_EQ(std::numeric_limits<size_t>::max(), Aaa_::indexOf("A_"));

    using FourArg = StringIndexMap<a_, b_, c_, d_>;
    EXPECT_EQ(size_t(4), FourArg::total);
    EXPECT_EQ(size_t(0), FourArg::indexOf("a"));
    EXPECT_EQ(size_t(1), FourArg::indexOf("b"));
    EXPECT_EQ(size_t(2), FourArg::indexOf("c"));
    EXPECT_EQ(size_t(3), FourArg::indexOf("d"));
    
    static constexpr const char Some_[] = "Some";
    static constexpr const char slightly_[] = "slightly";
    static constexpr const char moreComplex_[] = "moreComplex";
    static constexpr const char stringy_thingies_[] = "stringy_thingies";
    static constexpr const char dot[] = ".";
    using SlightlyComplex = StringIndexMap<Some_, slightly_, moreComplex_, stringy_thingies_, dot>;

    EXPECT_EQ(size_t(5), SlightlyComplex::total);
    EXPECT_EQ(size_t(0), SlightlyComplex::indexOf("Some"));
    EXPECT_EQ(size_t(1), SlightlyComplex::indexOf("slightly"));
    EXPECT_EQ(size_t(2), SlightlyComplex::indexOf("moreComplex"));
    EXPECT_EQ(size_t(3), SlightlyComplex::indexOf("stringy_thingies"));
    EXPECT_EQ(size_t(4), SlightlyComplex::indexOf("."));

    using SixArg = StringIndexMap<f_, e_, d_, c_, b_, a_>;
    
    EXPECT_EQ(size_t(6), SixArg::total);
    EXPECT_EQ(size_t(0), SixArg::indexOf("f"));
    EXPECT_EQ(size_t(1), SixArg::indexOf("e"));
    EXPECT_EQ(size_t(2), SixArg::indexOf("d"));
    EXPECT_EQ(size_t(3), SixArg::indexOf("c"));
    EXPECT_EQ(size_t(4), SixArg::indexOf("b"));
    EXPECT_EQ(size_t(5), SixArg::indexOf("a"));
    
    static constexpr const char g_[] = "g";
    static constexpr const char h_[] = "h";
    static constexpr const char i_[] = "i";
    static constexpr const char j_[] = "j";
    static constexpr const char k_[] = "k";
    static constexpr const char l_[] = "l";
    static constexpr const char m_[] = "m";
    static constexpr const char n_[] = "n";
    static constexpr const char o_[] = "o";
    static constexpr const char p_[] = "p";
    static constexpr const char q_[] = "q";
    static constexpr const char r_[] = "r";
    static constexpr const char s_[] = "s";
    static constexpr const char t_[] = "t";
    static constexpr const char u_[] = "u";
    static constexpr const char v_[] = "v";
    static constexpr const char w_[] = "w";
    static constexpr const char x_[] = "x";
    static constexpr const char y_[] = "y";
    static constexpr const char z_[] = "z";
    static constexpr const char a0[] = "a0";
    static constexpr const char a1[] = "a1";
    static constexpr const char a2[] = "a2";
    static constexpr const char a3[] = "a3";
    static constexpr const char a4[] = "a4";
    static constexpr const char a5[] = "a5";
    static constexpr const char a6[] = "a6";
    static constexpr const char a7[] = "a7";
    static constexpr const char a8[] = "a8";
    static constexpr const char a9[] = "a9";
    static constexpr const char b0[] = "b0";
    static constexpr const char b1[] = "b1";
    static constexpr const char b2[] = "b2";
    static constexpr const char b3[] = "b3";
    static constexpr const char b4[] = "b4";
    static constexpr const char b5[] = "b5";
    static constexpr const char b6[] = "b6";
    static constexpr const char b7[] = "b7";
    static constexpr const char b8[] = "b8";
    static constexpr const char b9[] = "b9";
    static constexpr const char c0[] = "c0";
    static constexpr const char c1[] = "c1";
    static constexpr const char c2[] = "c2";
    static constexpr const char c3[] = "c3";
    static constexpr const char c4[] = "c4";
    static constexpr const char c5[] = "c5";
    static constexpr const char c6[] = "c6";
    static constexpr const char c7[] = "c7";
    static constexpr const char c8[] = "c8";
    static constexpr const char c9[] = "c9";
    static constexpr const char d0[] = "d0";
    static constexpr const char d1[] = "d1";
    static constexpr const char d2[] = "d2";
    static constexpr const char d3[] = "d3";
    static constexpr const char d4[] = "d4";
    static constexpr const char d5[] = "d5";
    static constexpr const char d6[] = "d6";
    static constexpr const char d7[] = "d7";
    static constexpr const char d8[] = "d8";
    static constexpr const char d9[] = "d9";
    static constexpr const char e0[] = "e0";
    static constexpr const char e1[] = "e1";
    static constexpr const char e2[] = "e2";
    static constexpr const char e3[] = "e3";
    static constexpr const char e4[] = "e4";
    static constexpr const char e5[] = "e5";
    static constexpr const char e6[] = "e6";
    static constexpr const char e7[] = "e7";
    static constexpr const char e8[] = "e8";
    static constexpr const char e9[] = "e9";
    static constexpr const char f0[] = "f0";
    static constexpr const char f1[] = "f1";
    static constexpr const char f2[] = "f2";
    static constexpr const char f3[] = "f3";
    static constexpr const char f4[] = "f4";
    static constexpr const char f5[] = "f5";
    static constexpr const char f6[] = "f6";
    static constexpr const char f7[] = "f7";
    static constexpr const char f8[] = "f8";
    static constexpr const char f9[] = "f9";
    static constexpr const char g0[] = "g0";
    static constexpr const char g1[] = "g1";
    static constexpr const char g2[] = "g2";
    static constexpr const char g3[] = "g3";
    static constexpr const char g4[] = "g4";
    static constexpr const char g5[] = "g5";
    static constexpr const char g6[] = "g6";
    static constexpr const char g7[] = "g7";
    static constexpr const char g8[] = "g8";
    static constexpr const char g9[] = "g9";
    static constexpr const char h0[] = "h0";
    static constexpr const char h1[] = "h1";
    static constexpr const char h2[] = "h2";
    static constexpr const char h3[] = "h3";
    static constexpr const char h4[] = "h4";
    static constexpr const char h5[] = "h5";
    static constexpr const char h6[] = "h6";
    static constexpr const char h7[] = "h7";
    static constexpr const char h8[] = "h8";
    static constexpr const char h9[] = "h9";
    static constexpr const char i0[] = "i0";
    static constexpr const char i1[] = "i1";
    static constexpr const char i2[] = "i2";
    static constexpr const char i3[] = "i3";
    static constexpr const char i4[] = "i4";
    static constexpr const char i5[] = "i5";
    static constexpr const char i6[] = "i6";
    static constexpr const char i7[] = "i7";
    static constexpr const char i8[] = "i8";
    static constexpr const char i9[] = "i9";
    static constexpr const char j0[] = "j0";
    static constexpr const char j1[] = "j1";
    static constexpr const char j2[] = "j2";
    static constexpr const char j3[] = "j3";
    static constexpr const char j4[] = "j4";
    static constexpr const char j5[] = "j5";
    static constexpr const char j6[] = "j6";
    static constexpr const char j7[] = "j7";
    static constexpr const char last[] = "last";
    using ManyArgs = StringIndexMap<a_,b_,c_,d_,e_,f_,g_,h_,i_,j_,k_,l_,m_,n_,o_,p_,q_,r_,s_,t_,u_,v_,w_,x_,y_,z_,
        a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,
        d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,
        g0,g1,g2,g3,g4,g5,g6,g7,g8,g9,h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,
        j0,j1,j2,j3,j4,j5,j6,j7,last>;
    EXPECT_EQ(size_t(125), ManyArgs::total);
    
    constexpr std::string_view manyStrings[] {
        "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
        "a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","b0","b1","b2","b3","b4","b5","b6","b7","b8","b9",
        "c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","d0","d1","d2","d3","d4","d5","d6","d7","d8","d9",
        "e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","f0","f1","f2","f3","f4","f5","f6","f7","f8","f9",
        "g0","g1","g2","g3","g4","g5","g6","g7","g8","g9","h0","h1","h2","h3","h4","h5","h6","h7","h8","h9",
        "i0","i1","i2","i3","i4","i5","i6","i7","i8","i9","j0","j1","j2","j3","j4","j5","j6","j7","last"
    };
    EXPECT_EQ(size_t(0), ManyArgs::indexOf("a"));
    for ( size_t i=0; i<125; ++i ) {
        EXPECT_EQ(i, ManyArgs::indexOf(manyStrings[i]));
    }
}

TEST(RareTsTest, TypeMask)
{
    bool isSame = std::is_same_v<std::index_sequence<>, typename RareTs::type_mask<std::is_integral>::indexes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<>, typename RareTs::type_mask<std::is_integral, void>::indexes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0>, typename RareTs::type_mask<std::is_integral, int>::indexes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<>, typename RareTs::type_mask<std::is_integral, void, void>::indexes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<1>, typename RareTs::type_mask<std::is_integral, void, int>::indexes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0>, typename RareTs::type_mask<std::is_integral, int, void>::indexes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0, 1>, typename RareTs::type_mask<std::is_integral, int, int>::indexes>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<1, 4, 5, 7>,
        typename RareTs::type_mask<std::is_integral, void, int, void, void, int, int, void, int>::indexes>;
    EXPECT_TRUE(isSame);
}

TEST(RareTsTest, ReferenceTuple)
{
    auto tup = std::tuple { 0, 1, 2, std::string("a"), std::string("b") };

    auto evenTup = RareTs::reference_tuple(tup, std::index_sequence<0, 2, 4>{});
    bool isSame = std::is_same_v<decltype(evenTup), std::tuple<const int &, const int &, const std::string &>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<0>(evenTup));
    EXPECT_EQ(2, std::get<1>(evenTup));
    EXPECT_STREQ("b", std::get<2>(evenTup).c_str());
    
    auto oddTup = RareTs::reference_tuple(tup, std::index_sequence<1, 3>{});
    isSame = std::is_same_v<decltype(oddTup), std::tuple<const int &, const std::string &>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, std::get<0>(oddTup));
    EXPECT_STREQ("a", std::get<1>(oddTup).c_str());

    auto integralTup = RareTs::reference_tuple<std::is_integral>(tup);
    isSame = std::is_same_v<decltype(integralTup), std::tuple<const int &, const int &, const int &>>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(0, std::get<0>(integralTup));
    EXPECT_EQ(1, std::get<1>(integralTup));
    EXPECT_EQ(2, std::get<2>(integralTup));
}

TEST(RareTsTest, RemoveIndex)
{
    bool isSame = std::is_same_v<std::index_sequence<>, typename RareTs::remove_index<0, 0>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<>, typename RareTs::remove_index<0, 1>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<1>, typename RareTs::remove_index<0, 0, 1>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<1>, typename RareTs::remove_index<0, 1, 0>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0>, typename RareTs::remove_index<1, 0, 1>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0>, typename RareTs::remove_index<1, 1, 0>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0, 1>, typename RareTs::remove_index<7, 7, 0, 1>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0, 1>, typename RareTs::remove_index<7, 0, 7, 1>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0, 1>, typename RareTs::remove_index<7, 0, 1, 7>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0, 1, 2>, typename RareTs::remove_index<7, 7, 0, 1, 2>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0, 1, 2>, typename RareTs::remove_index<7, 0, 7, 1, 2>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0, 1, 2>, typename RareTs::remove_index<7, 0, 1, 7, 2>::type>;
    EXPECT_TRUE(isSame);
    isSame = std::is_same_v<std::index_sequence<0, 1, 2>, typename RareTs::remove_index<7, 0, 1, 2, 7>::type>;
    EXPECT_TRUE(isSame);
}
