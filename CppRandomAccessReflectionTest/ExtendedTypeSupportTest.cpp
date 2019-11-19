#include <gtest/gtest.h>
#include "../CppRandomAccessReflectionLib/Reflect.h"
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
    isPair = is_pair<std::pair<int, float>>::value;
    EXPECT_TRUE(isPair);
    
    EXPECT_FALSE(is_pair<const int>::value);
    EXPECT_FALSE(is_pair<const int*>::value);
    EXPECT_FALSE(is_pair<const int[2]>::value);
    EXPECT_FALSE(is_pair<const std::vector<int>>::value);
    isPair = is_pair<const std::map<int, float>>::value;
    EXPECT_FALSE(isPair);
    isPair = is_pair<const std::pair<int, float>>::value;
    EXPECT_TRUE(isPair);
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

TEST(ExtendedTypeSupportTest, HasType)
{
    bool hasType = HasType<bool>::value;
    EXPECT_FALSE(hasType);
    hasType = HasType<bool, int>::value;
    EXPECT_FALSE(hasType);
    hasType = HasType<bool, bool>::value;
    EXPECT_TRUE(hasType);
    hasType = HasType<bool, int, int>::value;
    EXPECT_FALSE(hasType);
    hasType = HasType<bool, int, bool>::value;
    EXPECT_TRUE(hasType);
    hasType = HasType<bool, bool, int>::value;
    EXPECT_TRUE(hasType);
    hasType = HasType<bool, bool, bool>::value;
    EXPECT_TRUE(hasType);
}

TEST(ExtendedTypeSupportTest, ConstexprTypeToStr)
{
    EXPECT_STREQ("int", type_to_str<int>::get().value);

    // Expecting some junk like how it's a struct or class, but it should still contain the pair text
    auto pairStrStruct = type_to_str<std::pair<int,int>>::get();
    std::string typeStr = pairStrStruct.value;
    typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
    EXPECT_TRUE(typeStr.find("pair<int,int") != std::string::npos);

    // Expecting some junk like how it's a struct or class and what allocator it's using, but should contain the map text
    auto mapStrStruct = type_to_str<std::map<int,int>>::get();
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
