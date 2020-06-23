#include <gtest/gtest.h>
#include "../RareCppLib/Reflect.h"
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <type_traits>
using namespace ExtendedTypeSupport;

TEST(ExtendedTypeSupportTest, PromoteChar)
{
    bool isSame = std::is_same<promote_char<char>::type, int>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<promote_char<signed char>::type, int>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<promote_char<unsigned char>::type, int>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<promote_char<const char>::type, const int>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<promote_char<const signed char>::type, const int>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<promote_char<const unsigned char>::type, const int>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<promote_char<short>::type, short>::value;
    EXPECT_TRUE(isSame);
}

TEST(ExtendedTypeSupportTest, PairLhs)
{
    bool isSame = std::is_same<float, pair_lhs<std::pair<float, int>>::type>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<char, pair_lhs<std::pair<char, int>>::type>::value;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same<float, pair_lhs<const std::pair<float, int>>::type>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<char, pair_lhs<const std::pair<char, int>>::type>::value;
    EXPECT_TRUE(isSame);
}

TEST(ExtendedTypeSupportTest, PairRhs)
{
    bool isSame = std::is_same<float, pair_rhs<std::pair<int, float>>::type>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<char, pair_rhs<std::pair<int, char>>::type>::value;
    EXPECT_TRUE(isSame);
    
    isSame = std::is_same<float, pair_rhs<const std::pair<int, float>>::type>::value;
    EXPECT_TRUE(isSame);
    isSame = std::is_same<char, pair_rhs<const std::pair<int, char>>::type>::value;
    EXPECT_TRUE(isSame);
}

TEST(ExtendedTypeSupportTest, ElementType)
{
    bool isEqual = std::is_same<void, element_type<int>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<void, element_type<int*>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, element_type<int[2]>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, element_type<std::array<int, 2>>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, element_type<std::vector<int>>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<std::pair<int, float>, element_type<std::map<int, float>>::type>::value;
    EXPECT_TRUE(isEqual);
    
    isEqual = std::is_same<void, element_type<const int>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<void, element_type<const int*>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, element_type<const int[2]>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, element_type<const std::array<int, 2>>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, element_type<const std::vector<int>>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<std::pair<int, float>, element_type<const std::map<int, float>>::type>::value;
    EXPECT_TRUE(isEqual);
}

TEST(ExtendedTypeSupportTest, RemovePointer)
{
    bool isEqual = std::is_same<int, remove_pointer<int>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int[2], remove_pointer<int[2]>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, remove_pointer<int*>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, remove_pointer<std::shared_ptr<int>>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, remove_pointer<std::unique_ptr<int>>::type>::value;
    EXPECT_TRUE(isEqual);
    
    isEqual = std::is_same<const int, remove_pointer<const int>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<const int[2], remove_pointer<const int[2]>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<const int, remove_pointer<const int*>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<const int, remove_pointer<std::shared_ptr<const int>>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<const int, remove_pointer<std::unique_ptr<const int>>::type>::value;
    EXPECT_TRUE(isEqual);
}

TEST(ExtendedTypeSupportTest, IsPointable)
{
    EXPECT_FALSE(is_pointable<int>::value);
    EXPECT_FALSE(is_pointable<int[2]>::value);
    EXPECT_TRUE(is_pointable<int*>::value);
    EXPECT_TRUE(is_pointable<std::shared_ptr<int>>::value);
    EXPECT_TRUE(is_pointable<std::unique_ptr<int>>::value);
    
    EXPECT_FALSE(is_pointable<const int>::value);
    EXPECT_FALSE(is_pointable<const int[2]>::value);
    EXPECT_TRUE(is_pointable<const int*>::value);
    EXPECT_TRUE(is_pointable<std::shared_ptr<const int>>::value);
    EXPECT_TRUE(is_pointable<std::unique_ptr<const int>>::value);
}

TEST(ExtendedTypeSupportTest, StaticArraySize)
{
    EXPECT_EQ(2, static_array_size<int[2]>::value);
    EXPECT_EQ(3, static_array_size<int[3]>::value);
    size_t staticArraySize = static_array_size<std::array<int, 2>>::value;
    EXPECT_EQ(2, staticArraySize);
    staticArraySize = static_array_size<std::array<int, 3>>::value;
    EXPECT_EQ(3, staticArraySize);
    
    EXPECT_EQ(2, static_array_size<const int[2]>::value);
    EXPECT_EQ(3, static_array_size<const int[3]>::value);
    staticArraySize = static_array_size<const std::array<int, 2>>::value;
    EXPECT_EQ(2, staticArraySize);
    staticArraySize = static_array_size<const std::array<int, 3>>::value;
    EXPECT_EQ(3, staticArraySize);
}

TEST(ExtendedTypeSupportTest, IsStaticArray)
{
    EXPECT_TRUE(is_static_array<int[2]>::value);
    EXPECT_FALSE(is_static_array<int*>::value);
    bool isStaticArray = is_static_array<std::array<int, 2>>::value;
    EXPECT_TRUE(isStaticArray);
    EXPECT_FALSE(is_static_array<std::vector<int>>::value);
    
    EXPECT_TRUE(is_static_array<const int[2]>::value);
    EXPECT_FALSE(is_static_array<const int*>::value);
    isStaticArray = is_static_array<const std::array<int, 2>>::value;
    EXPECT_TRUE(isStaticArray);
    EXPECT_FALSE(is_static_array<const std::vector<int>>::value);
}

TEST(ExtendedTypeSupportTest, IsIterable)
{
    EXPECT_FALSE(is_iterable<int>::value);
    EXPECT_FALSE(is_iterable<int*>::value);
    EXPECT_TRUE(is_iterable<int[2]>::value);
    bool isIterable = is_iterable<std::array<int, 2>>::value;
    EXPECT_TRUE(isIterable);
    EXPECT_TRUE(is_iterable<std::vector<int>>::value);
    EXPECT_TRUE(is_iterable<std::deque<int>>::value);
    EXPECT_TRUE(is_iterable<std::list<int>>::value);
    EXPECT_TRUE(is_iterable<std::forward_list<int>>::value);
    EXPECT_TRUE(is_iterable<std::stack<int>>::value);
    EXPECT_TRUE(is_iterable<std::queue<int>>::value);
    EXPECT_TRUE(is_iterable<std::priority_queue<int>>::value);
    EXPECT_TRUE(is_iterable<std::set<int>>::value);
    EXPECT_TRUE(is_iterable<std::multiset<int>>::value);
    EXPECT_TRUE(is_iterable<std::unordered_set<int>>::value);
    EXPECT_TRUE(is_iterable<std::unordered_multiset<int>>::value);
    isIterable = is_iterable<std::map<int, float>>::value;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable<std::multimap<int, float>>::value;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable<std::unordered_map<int, float>>::value;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable<std::unordered_multimap<int, float>>::value;
    EXPECT_TRUE(isIterable);
    
    EXPECT_FALSE(is_iterable<const int>::value);
    EXPECT_FALSE(is_iterable<const int*>::value);
    EXPECT_TRUE(is_iterable<const int[2]>::value);
    isIterable = is_iterable<const std::array<int, 2>>::value;
    EXPECT_TRUE(isIterable);
    EXPECT_TRUE(is_iterable<const std::vector<int>>::value);
    EXPECT_TRUE(is_iterable<const std::deque<int>>::value);
    EXPECT_TRUE(is_iterable<const std::list<int>>::value);
    EXPECT_TRUE(is_iterable<const std::forward_list<int>>::value);
    EXPECT_TRUE(is_iterable<const std::stack<int>>::value);
    EXPECT_TRUE(is_iterable<const std::queue<int>>::value);
    EXPECT_TRUE(is_iterable<const std::priority_queue<int>>::value);
    EXPECT_TRUE(is_iterable<const std::set<int>>::value);
    EXPECT_TRUE(is_iterable<const std::multiset<int>>::value);
    EXPECT_TRUE(is_iterable<const std::unordered_set<int>>::value);
    EXPECT_TRUE(is_iterable<const std::unordered_multiset<int>>::value);
    isIterable = is_iterable<const std::map<int, float>>::value;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable<const std::multimap<int, float>>::value;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable<const std::unordered_map<int, float>>::value;
    EXPECT_TRUE(isIterable);
    isIterable = is_iterable<const std::unordered_multimap<int, float>>::value;
    EXPECT_TRUE(isIterable);
}

TEST(ExtendedTypeSupportTest, IsMap)
{
    EXPECT_FALSE(is_map<int>::value);
    EXPECT_FALSE(is_map<int*>::value);
    EXPECT_FALSE(is_map<int[2]>::value);
    bool isMap = is_map<std::array<int, 2>>::value;
    EXPECT_FALSE(isMap);
    EXPECT_FALSE(is_map<std::vector<int>>::value);
    EXPECT_FALSE(is_map<std::deque<int>>::value);
    EXPECT_FALSE(is_map<std::list<int>>::value);
    EXPECT_FALSE(is_map<std::forward_list<int>>::value);
    EXPECT_FALSE(is_map<std::stack<int>>::value);
    EXPECT_FALSE(is_map<std::queue<int>>::value);
    EXPECT_FALSE(is_map<std::priority_queue<int>>::value);
    EXPECT_FALSE(is_map<std::set<int>>::value);
    EXPECT_FALSE(is_map<std::multiset<int>>::value);
    EXPECT_FALSE(is_map<std::unordered_set<int>>::value);
    EXPECT_FALSE(is_map<std::unordered_multiset<int>>::value);
    isMap = is_map<std::map<int, float>>::value;
    EXPECT_TRUE(isMap);
    isMap = is_map<std::multimap<int, float>>::value;
    EXPECT_TRUE(isMap);
    isMap = is_map<std::unordered_map<int, float>>::value;
    EXPECT_TRUE(isMap);
    isMap = is_map<std::unordered_multimap<int, float>>::value;
    EXPECT_TRUE(isMap);
    
    EXPECT_FALSE(is_map<const int>::value);
    EXPECT_FALSE(is_map<const int*>::value);
    EXPECT_FALSE(is_map<const int[2]>::value);
    isMap = is_map<const std::array<int, 2>>::value;
    EXPECT_FALSE(isMap);
    EXPECT_FALSE(is_map<const std::vector<int>>::value);
    EXPECT_FALSE(is_map<const std::deque<int>>::value);
    EXPECT_FALSE(is_map<const std::list<int>>::value);
    EXPECT_FALSE(is_map<const std::forward_list<int>>::value);
    EXPECT_FALSE(is_map<const std::stack<int>>::value);
    EXPECT_FALSE(is_map<const std::queue<int>>::value);
    EXPECT_FALSE(is_map<const std::priority_queue<int>>::value);
    EXPECT_FALSE(is_map<const std::set<int>>::value);
    EXPECT_FALSE(is_map<const std::multiset<int>>::value);
    EXPECT_FALSE(is_map<const std::unordered_set<int>>::value);
    EXPECT_FALSE(is_map<const std::unordered_multiset<int>>::value);
    isMap = is_map<const std::map<int, float>>::value;
    EXPECT_TRUE(isMap);
    isMap = is_map<const std::multimap<int, float>>::value;
    EXPECT_TRUE(isMap);
    isMap = is_map<const std::unordered_map<int, float>>::value;
    EXPECT_TRUE(isMap);
    isMap = is_map<const std::unordered_multimap<int, float>>::value;
    EXPECT_TRUE(isMap);
}

TEST(ExtendedTypeSupportTest, IsStlIterable)
{
    EXPECT_FALSE(is_stl_iterable<int>::value);
    EXPECT_FALSE(is_stl_iterable<int*>::value);
    EXPECT_FALSE(is_stl_iterable<int[2]>::value);
    using ExampleArrayType = std::array<int, 2>;
    EXPECT_TRUE(is_stl_iterable<ExampleArrayType>::value);
    EXPECT_TRUE(is_stl_iterable<std::vector<int>>::value);
    EXPECT_TRUE(is_stl_iterable<std::deque<int>>::value);
    EXPECT_TRUE(is_stl_iterable<std::forward_list<int>>::value);
    EXPECT_TRUE(is_stl_iterable<std::list<int>>::value);
    
    EXPECT_FALSE(is_stl_iterable<std::stack<int>>::value);
    EXPECT_FALSE(is_stl_iterable<std::queue<int>>::value);
    EXPECT_FALSE(is_stl_iterable<std::priority_queue<int>>::value);

    EXPECT_TRUE(is_stl_iterable<std::set<int>>::value);
    EXPECT_TRUE(is_stl_iterable<std::multiset<int>>::value);
    using ExampleMapType = std::map<int, int>;
    EXPECT_TRUE(is_stl_iterable<ExampleMapType>::value);
    using ExampleMultiMapType = std::multimap<int, int>;
    EXPECT_TRUE(is_stl_iterable<ExampleMultiMapType>::value);
    EXPECT_TRUE(is_stl_iterable<std::unordered_set<int>>::value);
    EXPECT_TRUE(is_stl_iterable<std::unordered_multiset<int>>::value);
    using ExampleUnorderedMapType = std::map<int, int>;
    EXPECT_TRUE(is_stl_iterable<ExampleUnorderedMapType>::value);
    using ExampleUnorderedMultiMapType = std::multimap<int, int>;
    EXPECT_TRUE(is_stl_iterable<ExampleUnorderedMultiMapType>::value);
    

    EXPECT_FALSE(is_stl_iterable<const int>::value);
    EXPECT_FALSE(is_stl_iterable<const int*>::value);
    EXPECT_FALSE(is_stl_iterable<const int[2]>::value);
    using ExampleArrayType = std::array<int, 2>;
    EXPECT_TRUE(is_stl_iterable<const ExampleArrayType>::value);
    EXPECT_TRUE(is_stl_iterable<const std::vector<int>>::value);
    EXPECT_TRUE(is_stl_iterable<const std::deque<int>>::value);
    EXPECT_TRUE(is_stl_iterable<const std::forward_list<int>>::value);
    EXPECT_TRUE(is_stl_iterable<const std::list<int>>::value);
    
    EXPECT_FALSE(is_stl_iterable<const std::stack<int>>::value);
    EXPECT_FALSE(is_stl_iterable<const std::queue<int>>::value);
    EXPECT_FALSE(is_stl_iterable<const std::priority_queue<int>>::value);

    EXPECT_TRUE(is_stl_iterable<const std::set<int>>::value);
    EXPECT_TRUE(is_stl_iterable<const std::multiset<int>>::value);
    EXPECT_TRUE(is_stl_iterable<const ExampleMapType>::value);
    EXPECT_TRUE(is_stl_iterable<const ExampleMultiMapType>::value);
    EXPECT_TRUE(is_stl_iterable<const std::unordered_set<int>>::value);
    EXPECT_TRUE(is_stl_iterable<const std::unordered_multiset<int>>::value);
    EXPECT_TRUE(is_stl_iterable<const ExampleUnorderedMapType>::value);
    EXPECT_TRUE(is_stl_iterable<const ExampleUnorderedMultiMapType>::value);
}

TEST(ExtendedTypeSupportTest, IsAdaptor)
{
    EXPECT_FALSE(is_adaptor<int>::value);
    EXPECT_FALSE(is_adaptor<std::vector<int>>::value);
    EXPECT_TRUE(is_adaptor<std::stack<int>>::value);
    EXPECT_TRUE(is_adaptor<std::queue<int>>::value);
    EXPECT_TRUE(is_adaptor<std::priority_queue<int>>::value);
    
    EXPECT_FALSE(is_adaptor<const int>::value);
    EXPECT_FALSE(is_adaptor<const std::vector<int>>::value);
    EXPECT_TRUE(is_adaptor<const std::stack<int>>::value);
    EXPECT_TRUE(is_adaptor<const std::queue<int>>::value);
    EXPECT_TRUE(is_adaptor<const std::priority_queue<int>>::value);
}

TEST(ExtendedTypeSupportTest, IsForwardList)
{
    EXPECT_FALSE(is_forward_list<int>::value);
    EXPECT_FALSE(is_forward_list<int*>::value);
    EXPECT_FALSE(is_forward_list<int[2]>::value);
    bool isForwardList = is_forward_list<std::array<int, 2>>::value;
    EXPECT_FALSE(isForwardList);
    EXPECT_FALSE(is_forward_list<std::vector<int>>::value);
    EXPECT_TRUE(is_forward_list<std::forward_list<int>>::value);
    
    EXPECT_FALSE(is_forward_list<const int>::value);
    EXPECT_FALSE(is_forward_list<const int*>::value);
    EXPECT_FALSE(is_forward_list<const int[2]>::value);
    isForwardList = is_forward_list<std::array<const int, 2>>::value;
    EXPECT_FALSE(isForwardList);
    EXPECT_FALSE(is_forward_list<std::vector<const int>>::value);
    EXPECT_TRUE(is_forward_list<std::forward_list<const int>>::value);
}

TEST(ExtendedTypeSupportTest, IsPair)
{
    EXPECT_FALSE(is_pair<int>::value);
    EXPECT_FALSE(is_pair<int*>::value);
    EXPECT_FALSE(is_pair<int[2]>::value);
    EXPECT_FALSE(is_pair<std::vector<int>>::value);
    bool isPair = is_pair<std::map<int, float>>::value;
    EXPECT_FALSE(isPair);
    isPair = is_pair<std::tuple<int, int>>::value;
    EXPECT_FALSE(isPair);
    isPair = is_pair<std::pair<int, float>>::value;
    EXPECT_TRUE(isPair);
    
    EXPECT_FALSE(is_pair<const int>::value);
    EXPECT_FALSE(is_pair<const int*>::value);
    EXPECT_FALSE(is_pair<const int[2]>::value);
    EXPECT_FALSE(is_pair<const std::vector<int>>::value);
    isPair = is_pair<const std::map<int, float>>::value;
    EXPECT_FALSE(isPair);
    isPair = is_pair<const std::tuple<int, int>>::value;
    EXPECT_FALSE(isPair);
    isPair = is_pair<const std::pair<int, float>>::value;
    EXPECT_TRUE(isPair);
}

TEST(ExtendedTypeSupportTest, IsTuple)
{
    EXPECT_FALSE(is_tuple<int>::value);
    EXPECT_FALSE(is_tuple<int*>::value);
    EXPECT_FALSE(is_tuple<int[2]>::value);
    EXPECT_FALSE(is_tuple<std::vector<int>>::value);
    bool isTuple = is_tuple<std::map<int, float>>::value;
    EXPECT_FALSE(isTuple);
    isTuple = is_tuple<std::pair<int, float>>::value;
    EXPECT_FALSE(isTuple);
    isTuple = is_tuple<std::tuple<>>::value;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple<std::tuple<int>>::value;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple<std::tuple<int, int>>::value;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple<std::tuple<int, int, int>>::value;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple<std::tuple<int, int, int, int>>::value;
    EXPECT_TRUE(isTuple);
    
    EXPECT_FALSE(is_tuple<const int>::value);
    EXPECT_FALSE(is_tuple<const int*>::value);
    EXPECT_FALSE(is_tuple<const int[2]>::value);
    EXPECT_FALSE(is_tuple<const std::vector<int>>::value);
    isTuple = is_tuple<const std::pair<int, float>>::value;
    EXPECT_FALSE(isTuple);
    isTuple = is_tuple<const std::map<int, float>>::value;
    EXPECT_FALSE(isTuple);
    isTuple = is_tuple<const std::tuple<>>::value;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple<const std::tuple<int>>::value;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple<const std::tuple<int, int>>::value;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple<const std::tuple<int, int, int>>::value;
    EXPECT_TRUE(isTuple);
    isTuple = is_tuple<const std::tuple<int, int, int, int>>::value;
    EXPECT_TRUE(isTuple);
}

TEST(ExtendedTypeSupportTest, IsBool)
{
    EXPECT_TRUE(is_bool<bool>::value);
    EXPECT_FALSE(is_bool<uint8_t>::value);
    EXPECT_FALSE(is_bool<uint16_t>::value);
    EXPECT_FALSE(is_bool<uint32_t>::value);
    EXPECT_FALSE(is_bool<uint64_t>::value);
    EXPECT_FALSE(is_bool<int8_t>::value);
    EXPECT_FALSE(is_bool<int16_t>::value);
    EXPECT_FALSE(is_bool<int32_t>::value);
    EXPECT_FALSE(is_bool<int64_t>::value);
    EXPECT_FALSE(is_bool<char>::value);
    EXPECT_FALSE(is_bool<short>::value);
    EXPECT_FALSE(is_bool<int>::value);
    EXPECT_FALSE(is_bool<long>::value);
    EXPECT_FALSE(is_bool<unsigned char>::value);
    EXPECT_FALSE(is_bool<unsigned short>::value);
    EXPECT_FALSE(is_bool<unsigned int>::value);
    EXPECT_FALSE(is_bool<unsigned long>::value);
    EXPECT_FALSE(is_bool<float>::value);
    EXPECT_FALSE(is_bool<double>::value);
    EXPECT_FALSE(is_bool<char*>::value);
    EXPECT_FALSE(is_bool<std::string>::value);
    
    EXPECT_TRUE(is_bool<const bool>::value);
    EXPECT_FALSE(is_bool<const uint8_t>::value);
    EXPECT_FALSE(is_bool<const uint16_t>::value);
    EXPECT_FALSE(is_bool<const uint32_t>::value);
    EXPECT_FALSE(is_bool<const uint64_t>::value);
    EXPECT_FALSE(is_bool<const int8_t>::value);
    EXPECT_FALSE(is_bool<const int16_t>::value);
    EXPECT_FALSE(is_bool<const int32_t>::value);
    EXPECT_FALSE(is_bool<const int64_t>::value);
    EXPECT_FALSE(is_bool<const char>::value);
    EXPECT_FALSE(is_bool<const short>::value);
    EXPECT_FALSE(is_bool<const int>::value);
    EXPECT_FALSE(is_bool<const long>::value);
    EXPECT_FALSE(is_bool<const unsigned char>::value);
    EXPECT_FALSE(is_bool<const unsigned short>::value);
    EXPECT_FALSE(is_bool<const unsigned int>::value);
    EXPECT_FALSE(is_bool<const unsigned long>::value);
    EXPECT_FALSE(is_bool<const float>::value);
    EXPECT_FALSE(is_bool<const double>::value);
    EXPECT_FALSE(is_bool<const char*>::value);
    EXPECT_FALSE(is_bool<const std::string>::value);
}

TEST(ExtendedTypeSupportTest, IsString)
{
    EXPECT_FALSE(is_string<bool>::value);
    EXPECT_FALSE(is_string<uint8_t>::value);
    EXPECT_FALSE(is_string<uint16_t>::value);
    EXPECT_FALSE(is_string<uint32_t>::value);
    EXPECT_FALSE(is_string<uint64_t>::value);
    EXPECT_FALSE(is_string<int8_t>::value);
    EXPECT_FALSE(is_string<int16_t>::value);
    EXPECT_FALSE(is_string<int32_t>::value);
    EXPECT_FALSE(is_string<int64_t>::value);
    EXPECT_FALSE(is_string<char>::value);
    EXPECT_FALSE(is_string<short>::value);
    EXPECT_FALSE(is_string<int>::value);
    EXPECT_FALSE(is_string<long>::value);
    EXPECT_FALSE(is_string<unsigned char>::value);
    EXPECT_FALSE(is_string<unsigned short>::value);
    EXPECT_FALSE(is_string<unsigned int>::value);
    EXPECT_FALSE(is_string<unsigned long>::value);
    EXPECT_FALSE(is_string<float>::value);
    EXPECT_FALSE(is_string<double>::value);
    EXPECT_FALSE(is_string<char*>::value);
    EXPECT_TRUE(is_string<std::string>::value);
    
    EXPECT_FALSE(is_string<const bool>::value);
    EXPECT_FALSE(is_string<const uint8_t>::value);
    EXPECT_FALSE(is_string<const uint16_t>::value);
    EXPECT_FALSE(is_string<const uint32_t>::value);
    EXPECT_FALSE(is_string<const uint64_t>::value);
    EXPECT_FALSE(is_string<const int8_t>::value);
    EXPECT_FALSE(is_string<const int16_t>::value);
    EXPECT_FALSE(is_string<const int32_t>::value);
    EXPECT_FALSE(is_string<const int64_t>::value);
    EXPECT_FALSE(is_string<const char>::value);
    EXPECT_FALSE(is_string<const short>::value);
    EXPECT_FALSE(is_string<const int>::value);
    EXPECT_FALSE(is_string<const long>::value);
    EXPECT_FALSE(is_string<const unsigned char>::value);
    EXPECT_FALSE(is_string<const unsigned short>::value);
    EXPECT_FALSE(is_string<const unsigned int>::value);
    EXPECT_FALSE(is_string<const unsigned long>::value);
    EXPECT_FALSE(is_string<const float>::value);
    EXPECT_FALSE(is_string<const double>::value);
    EXPECT_FALSE(is_string<const char*>::value);
    EXPECT_TRUE(is_string<const std::string>::value);
}

TEST(ExtendedTypeSupportTest, HasPushBack)
{
    EXPECT_FALSE(has_push_back<int>::value);
    EXPECT_FALSE(has_push_back<int[2]>::value);
    EXPECT_FALSE(has_push_back<std::forward_list<int>>::value);
    EXPECT_TRUE(has_push_back<std::vector<int>>::value);
    EXPECT_TRUE(has_push_back<std::deque<int>>::value);
    EXPECT_TRUE(has_push_back<std::list<int>>::value);

    EXPECT_FALSE(has_push_back<const int>::value);
    EXPECT_FALSE(has_push_back<const int[2]>::value);
    EXPECT_FALSE(has_push_back<const std::forward_list<int>>::value);
    EXPECT_TRUE(has_push_back<const std::vector<int>>::value);
    EXPECT_TRUE(has_push_back<const std::deque<int>>::value);
    EXPECT_TRUE(has_push_back<const std::list<int>>::value);
}

TEST(ExtendedTypeSupportTest, HasPush)
{
    EXPECT_FALSE(has_push<int>::value);
    EXPECT_FALSE(has_push<int[2]>::value);
    EXPECT_FALSE(has_push<std::vector<int>>::value);
    EXPECT_TRUE(has_push<std::stack<int>>::value);
    EXPECT_TRUE(has_push<std::queue<int>>::value);
    EXPECT_TRUE(has_push<std::priority_queue<int>>::value);

    EXPECT_FALSE(has_push<const int>::value);
    EXPECT_FALSE(has_push<const int[2]>::value);
    EXPECT_FALSE(has_push<const std::vector<int>>::value);
    EXPECT_TRUE(has_push<const std::stack<int>>::value);
    EXPECT_TRUE(has_push<const std::queue<int>>::value);
    EXPECT_TRUE(has_push<const std::priority_queue<int>>::value);
}

TEST(ExtendedTypeSupportTest, HasInsert)
{
    EXPECT_FALSE(has_insert<int>::value);
    EXPECT_FALSE(has_insert<int[2]>::value);
    EXPECT_TRUE(has_insert<std::set<int>>::value);
    EXPECT_TRUE(has_insert<std::multiset<int>>::value);
    EXPECT_TRUE(has_insert<std::unordered_set<int>>::value);
    EXPECT_TRUE(has_insert<std::unordered_multiset<int>>::value);
    using Map = std::map<int, float>;
    using Multimap = std::multimap<int, float>;
    using UnorderedMap = std::unordered_map<int, float>;
    using UnorderedMultimap = std::unordered_multimap<int, float>;
    EXPECT_TRUE(has_insert<Map>::value);
    EXPECT_TRUE(has_insert<Multimap>::value);
    EXPECT_TRUE(has_insert<UnorderedMap>::value);
    EXPECT_TRUE(has_insert<UnorderedMultimap>::value);
    
    EXPECT_FALSE(has_insert<const int>::value);
    EXPECT_FALSE(has_insert<const int[2]>::value);
    EXPECT_TRUE(has_insert<const std::set<int>>::value);
    EXPECT_TRUE(has_insert<const std::multiset<int>>::value);
    EXPECT_TRUE(has_insert<const std::unordered_set<int>>::value);
    EXPECT_TRUE(has_insert<const std::unordered_multiset<int>>::value);
    using Map = std::map<int, float>;
    using Multimap = std::multimap<int, float>;
    using UnorderedMap = std::unordered_map<int, float>;
    using UnorderedMultimap = std::unordered_multimap<int, float>;
    EXPECT_TRUE(has_insert<const Map>::value);
    EXPECT_TRUE(has_insert<const Multimap>::value);
    EXPECT_TRUE(has_insert<const UnorderedMap>::value);
    EXPECT_TRUE(has_insert<const UnorderedMultimap>::value);
}

TEST(ExtendedTypeSupportTest, HasClear)
{
    EXPECT_FALSE(has_clear<int>::value);
    EXPECT_FALSE(has_clear<int[2]>::value);
    bool hasClear = has_clear<std::array<int, 2>>::value;
    EXPECT_TRUE(has_clear<std::vector<int>>::value);
    EXPECT_TRUE(has_clear<std::deque<int>>::value);
    EXPECT_TRUE(has_clear<std::list<int>>::value);
    EXPECT_TRUE(has_clear<std::forward_list<int>>::value);
    EXPECT_FALSE(has_clear<std::stack<int>>::value);
    EXPECT_FALSE(has_clear<std::queue<int>>::value);
    EXPECT_FALSE(has_clear<std::priority_queue<int>>::value);
    EXPECT_TRUE(has_clear<std::set<int>>::value);
    EXPECT_TRUE(has_clear<std::multiset<int>>::value);
    EXPECT_TRUE(has_clear<std::unordered_set<int>>::value);
    EXPECT_TRUE(has_clear<std::unordered_multiset<int>>::value);
    using Map = std::map<int, float>;
    using Multimap = std::multimap<int, float>;
    using UnorderedMap = std::unordered_map<int, float>;
    using UnorderedMultimap = std::unordered_multimap<int, float>;
    EXPECT_TRUE(has_clear<Map>::value);
    EXPECT_TRUE(has_clear<Multimap>::value);
    EXPECT_TRUE(has_clear<UnorderedMap>::value);
    EXPECT_TRUE(has_clear<UnorderedMultimap>::value);
}

TEST(ExtendedTypeSupportTest, Append)
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
    Append(vector, value);
    Append(deque, value);
    Append(forwardList, value);
    Append(list, value);
    Append(stack, value);
    Append(queue, value);
    Append(priorityQueue, value);
    Append(set, value);
    Append(multiset, value);
    Append(unorderedSet, value);
    Append(unorderedMultiset, value);
    Append(map, keyValuePair);
    Append(multimap, keyValuePair);
    Append(unorderedMap, keyValuePair);
    Append(unorderedMultimap, keyValuePair);
    
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
    
    Append(vector, newValue);
    Append(deque, newValue);
    Append(forwardList, newValue);
    Append(list, newValue);
    Append(stack, newValue);
    Append(queue, newValue);
    Append(priorityQueue, newValue);
    Append(set, newValue);
    Append(multiset, newValue);
    Append(unorderedSet, newValue);
    Append(unorderedMultiset, newValue);
    Append(map, newKeyValuePair);
    Append(multimap, newKeyValuePair);
    Append(unorderedMap, newKeyValuePair);
    Append(unorderedMultimap, newKeyValuePair);
    
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

TEST(ExtendedTypeSupportTest, IsEmpty)
{
    int basicArray[1] = { 0 };
    std::array<int, 1> stlArray = { 0 };

    EXPECT_FALSE(IsEmpty(basicArray));
    EXPECT_FALSE(IsEmpty(stlArray));

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
    
    EXPECT_TRUE(IsEmpty(vector));
    EXPECT_TRUE(IsEmpty(deque));
    EXPECT_TRUE(IsEmpty(forwardList));
    EXPECT_TRUE(IsEmpty(list));
    EXPECT_TRUE(IsEmpty(stack));
    EXPECT_TRUE(IsEmpty(queue));
    EXPECT_TRUE(IsEmpty(priorityQueue));
    EXPECT_TRUE(IsEmpty(set));
    EXPECT_TRUE(IsEmpty(multiset));
    EXPECT_TRUE(IsEmpty(unorderedSet));
    EXPECT_TRUE(IsEmpty(unorderedMultiset));
    EXPECT_TRUE(IsEmpty(map));
    EXPECT_TRUE(IsEmpty(multimap));
    EXPECT_TRUE(IsEmpty(unorderedMap));
    EXPECT_TRUE(IsEmpty(unorderedMultimap));

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
    
    EXPECT_FALSE(IsEmpty(vector));
    EXPECT_FALSE(IsEmpty(deque));
    EXPECT_FALSE(IsEmpty(forwardList));
    EXPECT_FALSE(IsEmpty(list));
    EXPECT_FALSE(IsEmpty(stack));
    EXPECT_FALSE(IsEmpty(queue));
    EXPECT_FALSE(IsEmpty(priorityQueue));
    EXPECT_FALSE(IsEmpty(set));
    EXPECT_FALSE(IsEmpty(multiset));
    EXPECT_FALSE(IsEmpty(unorderedSet));
    EXPECT_FALSE(IsEmpty(unorderedMultiset));
    EXPECT_FALSE(IsEmpty(map));
    EXPECT_FALSE(IsEmpty(multimap));
    EXPECT_FALSE(IsEmpty(unorderedMap));
    EXPECT_FALSE(IsEmpty(unorderedMultimap));
}

TEST(ExtendedTypeSupportTest, Clear)
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
    
    Clear(vector);
    Clear(deque);
    Clear(forwardList);
    Clear(list);
    Clear(stack);
    Clear(queue);
    Clear(priorityQueue);
    Clear(set);
    Clear(multiset);
    Clear(unorderedSet);
    Clear(unorderedMultiset);
    Clear(map);
    Clear(multimap);
    Clear(unorderedMap);
    Clear(unorderedMultimap);
    
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

TEST(ExtendedTypeSupportTest, HasType)
{
    bool hasType = has_type<bool>::value;
    EXPECT_FALSE(hasType);
    hasType = has_type<bool, int>::value;
    EXPECT_FALSE(hasType);
    hasType = has_type<bool, std::tuple<>>::value;
    EXPECT_FALSE(hasType);
    hasType = has_type<bool, std::tuple<int>>::value;
    EXPECT_FALSE(hasType);
    hasType = has_type<bool, std::tuple<bool>>::value;
    EXPECT_TRUE(hasType);
    hasType = has_type<bool, std::tuple<int, int>>::value;
    EXPECT_FALSE(hasType);
    hasType = has_type<bool, std::tuple<int, bool>>::value;
    EXPECT_TRUE(hasType);
    hasType = has_type<bool, std::tuple<bool, int>>::value;
    EXPECT_TRUE(hasType);
    hasType = has_type<bool, std::tuple<bool, bool>>::value;
    EXPECT_TRUE(hasType);
    hasType = has_type<bool, const std::tuple<>>::value;
    EXPECT_FALSE(hasType);
    hasType = has_type<bool, const std::tuple<int>>::value;
    EXPECT_FALSE(hasType);
    hasType = has_type<bool, const std::tuple<bool>>::value;
    EXPECT_TRUE(hasType);
    hasType = has_type<bool, const std::tuple<int, int>>::value;
    EXPECT_FALSE(hasType);
    hasType = has_type<bool, const std::tuple<int, bool>>::value;
    EXPECT_TRUE(hasType);
    hasType = has_type<bool, const std::tuple<bool, int>>::value;
    EXPECT_TRUE(hasType);
    hasType = has_type<bool, const std::tuple<bool, bool>>::value;
    EXPECT_TRUE(hasType);
}

TEST(ExtendedTypeSupportTest, GetFirstTupleElementOfType)
{
    struct S { int a; };

    std::tuple<int> integer { 1337 };
    EXPECT_EQ(1337, get<int>::from(integer));
    EXPECT_NO_THROW(get<S>::from(integer));

    std::tuple<int, char> intChar { 1337, '\0' };
    EXPECT_EQ(1337, get<int>::from(intChar));
    EXPECT_NO_THROW(get<S>::from(intChar));
    std::tuple<char, int> charInt { '\0', 1337 };
    EXPECT_EQ(1337, get<int>::from(charInt));
    EXPECT_NO_THROW(get<S>::from(charInt));

    std::tuple<int, char, char> intCharChar { 1337, '\0', '\0' };
    EXPECT_EQ(1337, get<int>::from(intCharChar));
    EXPECT_NO_THROW(get<S>::from(intCharChar));
    std::tuple<char, int, char> charIntChar { '\0', 1337, '\0' };
    EXPECT_EQ(1337, get<int>::from(charIntChar));
    EXPECT_NO_THROW(get<S>::from(charIntChar));
    std::tuple<char, char, int> charCharInt { '\0', '\0', 1337 };
    EXPECT_EQ(1337, get<int>::from(charCharInt));
    EXPECT_NO_THROW(get<S>::from(charCharInt));

    
    S s { 9001 };
    std::tuple<S> es { s };
    EXPECT_EQ(9001, get<S>::from(es).a);
    EXPECT_NO_THROW(get<int>::from(es));

    std::tuple<S, char> sChar { s, '\0' };
    EXPECT_EQ(9001, get<S>::from(sChar).a);
    EXPECT_NO_THROW(get<int>::from(sChar));
    std::tuple<char, S> charS { '\0', s };
    EXPECT_EQ(9001, get<S>::from(charS).a);
    EXPECT_NO_THROW(get<int>::from(charS));

    std::tuple<S, char, char> sCharChar { s, '\0', '\0' };
    EXPECT_EQ(9001, get<S>::from(sCharChar).a);
    EXPECT_NO_THROW(get<int>::from(sCharChar));
    std::tuple<char, S, char> charEsChar { '\0', s, '\0' };
    EXPECT_EQ(9001, get<S>::from(charEsChar).a);
    EXPECT_NO_THROW(get<int>::from(charEsChar));
    std::tuple<char, char, S> charCharS { '\0', '\0', s };
    EXPECT_EQ(9001, get<S>::from(charCharS).a);
    EXPECT_NO_THROW(get<int>::from(charCharS));

    std::tuple<> empty {};
    EXPECT_NO_THROW(get<int>::from(empty));
    EXPECT_NO_THROW(get<S>::from(empty));
}

TEST(ExtendeTypeSupportTest, ForEach)
{
    std::tuple<> empty;
    bool visited = false;
    for_each<int>::in(empty, [&](auto & element) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    visited = false;
    std::tuple<int> integer { 555 };
    for_each<char>::in(integer, [&](auto & element) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    visited = false;
    for_each<int>::in(integer, [&](auto & element) {
        EXPECT_EQ(555, element);
        visited = true;
    });
    EXPECT_TRUE(visited);

    std::tuple<char, int> charInt { '\0', 1 };
    int visitCount = 0;
    for_each<int>::in(charInt, [&](auto & element) {
        EXPECT_EQ(1, element);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);

    std::tuple<int, char> intChar { 1, '\0' };
    visitCount = 0;
    for_each<int>::in(intChar, [&](auto & element) {
        EXPECT_EQ(1, element);
        visitCount++;
    });
    EXPECT_EQ(1, visitCount);

    std::tuple<int, int> intInt { 0, 1 };
    visitCount = 0;
    for_each<int>::in(intInt, [&](auto & element) {
        EXPECT_EQ(visitCount, element);
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);
    
    std::tuple<int, char, char, int, int> intCharCharInt { 0, 'a', 'b', 1, 2 };
    visitCount = 0;
    for_each<int>::in(intCharCharInt, [&](auto & element) {
        EXPECT_EQ(visitCount, element);
        visitCount++;
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ExtendedTypeSupportTest, ForEachIn)
{
    std::tuple<> empty {};
    bool visited = false;
    for_each_in(empty, [&](auto & element) {
        visited = true;
    });
    EXPECT_FALSE(visited);

    std::tuple<int> integer { 1 };
    visited = false;
    for_each_in(integer, [&](auto & element) {
        EXPECT_EQ(1, element);
        visited = true;
    });

    std::tuple<int, int> intInt { 0, 1 };
    int visitCount = 0;
    for_each_in(intInt, [&](auto & element) {
        EXPECT_EQ(visitCount, element);
        visitCount++;
    });
    EXPECT_EQ(2, visitCount);

    std::tuple<int, int, int> intIntInt { 0, 1, 2 };
    visitCount = 0;
    for_each_in(intIntInt, [&](auto & element) {
        EXPECT_EQ(visitCount, element);
        visitCount++;
    });
    EXPECT_EQ(3, visitCount);
}

TEST(ExtendedTypeSupportTest, ConstexprTypeToStr)
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

TEST(ExtendedTypeSupportTest, TypeToStr)
{
    EXPECT_STREQ("int", TypeToStr<int>().c_str());

    // Expecting some junk like how it's a struct or class, but it should still contain the pair text
    std::string typeStr = TypeToStr<std::pair<int,int>>();
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("pair<int,int") != std::string::npos);

    // Expecting some junk like how it's a struct or class and what allocator it's using, but should contain the map text
    std::string mapStr = TypeToStr<std::map<int,int>>();
    mapStr.erase(std::remove(mapStr.begin(), mapStr.end(), ' '), mapStr.end());
    EXPECT_TRUE(mapStr.find("map<int,int") != std::string::npos);
}

TEST(ExtendedTypeSupportTest, GetUnderlyingContainer)
{
    // Stacks
    std::stack<int> defaultStack;
    defaultStack.push(0);
    defaultStack.push(1);
    auto stackDefaultUnderlyingContainer = get_underlying_container(defaultStack);
    EXPECT_EQ(0, *stackDefaultUnderlyingContainer.begin());
    EXPECT_EQ(1, *++stackDefaultUnderlyingContainer.begin());

    std::stack<int, std::vector<int>> vectorStack;
    vectorStack.push(2);
    vectorStack.push(3);
    auto stackUnderlyingVector = get_underlying_container(vectorStack);
    bool isEqual = std::is_same<std::vector<int>, decltype(stackUnderlyingVector)>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(2, *stackUnderlyingVector.begin());
    EXPECT_EQ(3, *++stackUnderlyingVector.begin());

    std::stack<int, std::deque<int>> dequeStack;
    dequeStack.push(4);
    dequeStack.push(5);
    auto stackUnderlyingDeque = get_underlying_container(dequeStack);
    isEqual = std::is_same<std::deque<int>, decltype(stackUnderlyingDeque)>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(4, *stackUnderlyingDeque.begin());
    EXPECT_EQ(5, *++stackUnderlyingDeque.begin());

    std::stack<int, std::list<int>> listStack;
    listStack.push(6);
    listStack.push(7);
    auto stackUnderlyingList = get_underlying_container(listStack);
    isEqual = std::is_same<std::list<int>, decltype(stackUnderlyingList)>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(6, *stackUnderlyingList.begin());
    EXPECT_EQ(7, *++stackUnderlyingList.begin());
    
    // Queues
    std::queue<int> defaultQueue;
    defaultQueue.push(0);
    defaultQueue.push(1);
    auto queueDefaultUnderlyingContainer = get_underlying_container(defaultQueue);
    EXPECT_EQ(0, *queueDefaultUnderlyingContainer.begin());
    EXPECT_EQ(1, *++queueDefaultUnderlyingContainer.begin());

    std::queue<int, std::vector<int>> vectorQueue;
    vectorQueue.push(2);
    vectorQueue.push(3);
    auto queueUnderlyingVector = get_underlying_container(vectorQueue);
    isEqual = std::is_same<std::vector<int>, decltype(queueUnderlyingVector)>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(2, *queueUnderlyingVector.begin());
    EXPECT_EQ(3, *++queueUnderlyingVector.begin());

    std::queue<int, std::deque<int>> dequeQueue;
    dequeQueue.push(4);
    dequeQueue.push(5);
    auto queueUnderlyingDeque = get_underlying_container(dequeQueue);
    isEqual = std::is_same<std::deque<int>, decltype(queueUnderlyingDeque)>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(4, *queueUnderlyingDeque.begin());
    EXPECT_EQ(5, *++queueUnderlyingDeque.begin());

    std::queue<int, std::list<int>> listQueue;
    listQueue.push(6);
    listQueue.push(7);
    auto queueUnderlyingList = get_underlying_container(listQueue);
    isEqual = std::is_same<std::list<int>, decltype(queueUnderlyingList)>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(6, *queueUnderlyingList.begin());
    EXPECT_EQ(7, *++queueUnderlyingList.begin());

    // Priority Queues
    std::priority_queue<int> defaultPriorityQueue;
    defaultPriorityQueue.push(0);
    defaultPriorityQueue.push(1);
    auto priorityQueueDefaultUnderlyingContainer = get_underlying_container(defaultPriorityQueue);
    EXPECT_EQ(1, *priorityQueueDefaultUnderlyingContainer.begin());
    EXPECT_EQ(0, *++priorityQueueDefaultUnderlyingContainer.begin());

    std::priority_queue<int, std::vector<int>> vectorPriorityQueue;
    vectorPriorityQueue.push(2);
    vectorPriorityQueue.push(3);
    auto priorityQueueUnderlyingVector = get_underlying_container(vectorPriorityQueue);
    isEqual = std::is_same<std::vector<int>, decltype(priorityQueueUnderlyingVector)>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(3, *priorityQueueUnderlyingVector.begin());
    EXPECT_EQ(2, *++priorityQueueUnderlyingVector.begin());

    std::priority_queue<int, std::deque<int>> dequePriorityQueue;
    dequePriorityQueue.push(4);
    dequePriorityQueue.push(5);
    auto priorityQueueUnderlyingDeque = get_underlying_container(dequePriorityQueue);
    isEqual = std::is_same<std::deque<int>, decltype(priorityQueueUnderlyingDeque)>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_EQ(5, *priorityQueueUnderlyingDeque.begin());
    EXPECT_EQ(4, *++priorityQueueUnderlyingDeque.begin());
}
