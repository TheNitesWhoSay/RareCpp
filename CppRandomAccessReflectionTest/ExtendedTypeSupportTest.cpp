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
using namespace Reflect;

TEST(ReflectionSupportTest, TypeToStr)
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

TEST(ReflectionSupportTest, IsBool)
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
}

TEST(ReflectionSupportTest, IsPointable)
{
    EXPECT_FALSE(is_pointable<int>::value);
    EXPECT_FALSE(is_pointable<int[2]>::value);
    EXPECT_TRUE(is_pointable<int*>::value);
    EXPECT_TRUE(is_pointable<std::shared_ptr<int>>::value);
    EXPECT_TRUE(is_pointable<std::unique_ptr<int>>::value);
}

TEST(ReflectionSupportTest, RemovePointer)
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
}

TEST(ReflectionSupportTest, IsStlIterable)
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
}

TEST(ReflectionSupportTest, IsAdaptor)
{
    EXPECT_FALSE(is_adaptor<int>::value);
    EXPECT_FALSE(is_adaptor<std::vector<int>>::value);
    EXPECT_TRUE(is_adaptor<std::stack<int>>::value);
    EXPECT_TRUE(is_adaptor<std::queue<int>>::value);
    EXPECT_TRUE(is_adaptor<std::priority_queue<int>>::value);
}

TEST(ReflectionSupportTest, ElementType)
{
    bool isEqual = std::is_same<void, element_type<int>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<void, element_type<int*>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, element_type<int[2]>::type>::value;
    EXPECT_TRUE(isEqual);
    isEqual = std::is_same<int, element_type<std::vector<int>>::type>::value;
    EXPECT_TRUE(isEqual);
}

TEST(ReflectionSupportTest, GetUnderlyingContainer)
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

TEST(ReflectionSupportTest, FieldSimple)
{
    char fieldName[] = "fieldName";
    char fieldTypeStr[] = "int";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsReflected = false;

    Field<> field = { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsReflected };
    
    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
    EXPECT_EQ(fieldIsReflected, field.isReflected);
}

TEST(ReflectionSupportTest, FieldTemplated)
{
    constexpr size_t fieldIndex = 2;
    char fieldName[] = "fieldName";
    char fieldTypeStr[] = "int";
    size_t fieldArraySize = 0;
    bool fieldIsIterable = false;
    bool fieldIsReflected = false;

    Field<int, fieldIndex> field = { fieldName, fieldTypeStr, fieldArraySize, fieldIsIterable, fieldIsReflected };
    using IntField = decltype(field);

    EXPECT_STREQ(fieldName, field.name);
    EXPECT_STREQ(fieldTypeStr, field.typeStr);
    EXPECT_EQ(fieldArraySize, field.arraySize);
    EXPECT_EQ(fieldIsIterable, field.isIterable);
    EXPECT_EQ(fieldIsReflected, field.isReflected);
    
    bool isEqual = std::is_same<int, IntField::Type>::value;
    EXPECT_TRUE(isEqual);
    EXPECT_FALSE(IntField::IsReflected);
    EXPECT_EQ(fieldIndex, IntField::Index);
}
