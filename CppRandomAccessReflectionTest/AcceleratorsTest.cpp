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

struct Obj
{
    int first;
    float second;
};

TEST(AcceleratorsTest, ForPrimitive)
{
    int intValue = 0;
    int* intPointer = &intValue;
    Obj objValue = {0, 0.0f};

    RfS::Field<int, false> intField = { 0, "intValue", "int", 0, false, false, false };
    RfS::Field<int*, false> intPointerField = { 0, "intPointer", "int*", 0, false, false, false };
    RfS::Field<Obj, true> objField = { 0, "objValue", "Obj", 0, false, false, false };

    int timesVisited = 0;
    intField.ForPrimitive(intValue, [&](auto primitive) {
        EXPECT_EQ(intValue, primitive);
        timesVisited++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    intPointerField.ForPrimitive(intPointer, [&](auto primitive) {
        EXPECT_EQ(intValue, primitive);
        timesVisited++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    objField.ForPrimitive(objValue, [&](auto primitive) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
}

TEST(AcceleratorsTest, ForObject)
{
    Obj objValue = {1, 2.2f};
    Obj* objPointerValue = &objValue;
    int intValue = 0;
    
    RfS::Field<Obj, true> objField = { 0, "objValue", "Obj", 0, false, false, true };
    RfS::Field<Obj*, true> objPointerField = { 0, "objPointerValue", "Obj*", 0, false, false, true };
    RfS::Field<int, false> intField = { 0, "intValue", "int", 0, false, false, true };

    int timesVisited = 0;
    objField.ForObject(objValue, [&](auto & object) {
        EXPECT_EQ(&objValue, &object);
        timesVisited++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    objPointerField.ForObject(objPointerValue, [&](auto & object) {
        EXPECT_EQ(&objValue, &object);
        timesVisited++;
    });
    EXPECT_EQ(1, timesVisited);

    timesVisited = 0;
    intField.ForObject(intValue, [&](auto & object) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
}

TEST(AcceleratorsTest, ForPrimitives)
{
    constexpr size_t arraySize = 3;
    using IntArray = int[arraySize];

    int intValue = 0;
    IntArray intArrayValue = { 20, 30, 40 };
    IntArray* intArrayPointerValue = &intArrayValue;
    std::vector<int> intVectorValue = {10, 20, 30};
    std::vector<int>* intVectorPointerValue = &intVectorValue;
    std::stack<int> intStackValue;
    intStackValue.push(10);
    intStackValue.push(20);
    intStackValue.push(30);
    std::stack<int>* intStackPointerValue = &intStackValue;
    
    RfS::Field<int, false> intField = { 0, "intValue", "int", 0, false, false, false };
    RfS::Field<IntArray, false> intArrayField = { 0, "intArrayValue", "int[3]", arraySize, true, false, false };
    RfS::Field<IntArray*, false> intArrayPointerField = { 0, "intArrayPointerValue", "int[3]*", arraySize, true, false, false };
    RfS::Field<std::vector<int>, false> intVectorField = { 0, "intVectorValue", "std::vector<int>", 0, true, false, false };
    RfS::Field<std::vector<int>*, false> intVectorPointerField = { 0, "intVectorPointerValue", "std::vector<int>*", 0, true, false, false };
    RfS::Field<std::stack<int>, false> intStackField = { 0, "intStackValue", "std::stack<int>", 0, true, false, false };
    RfS::Field<std::stack<int>*, false> intStackPointerField = { 0, "intStackPointerValue", "std::stack<int>*", 0, true, false, false };

    int timesVisited = 0;
    intField.ForPrimitives(intValue, [&](auto primitive) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    intArrayField.ForPrimitives(intArrayValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 20); break;
            case 1: EXPECT_EQ(primitive, 30); break;
            case 2: EXPECT_EQ(primitive, 40); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    intArrayPointerField.ForPrimitives(intArrayPointerValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 20); break;
            case 1: EXPECT_EQ(primitive, 30); break;
            case 2: EXPECT_EQ(primitive, 40); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intVectorField.ForPrimitives(intVectorValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 10); break;
            case 1: EXPECT_EQ(primitive, 20); break;
            case 2: EXPECT_EQ(primitive, 30); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intVectorPointerField.ForPrimitives(intVectorPointerValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 10); break;
            case 1: EXPECT_EQ(primitive, 20); break;
            case 2: EXPECT_EQ(primitive, 30); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intStackField.ForPrimitives(intStackValue, [&](auto index, auto primitive) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);

    timesVisited = 0;
    intStackPointerField.ForPrimitives(intStackPointerValue, [&](auto index, auto primitive) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
}

TEST(AcceleratorsTest, ForPrimitivesConst)
{
    constexpr size_t arraySize = 3;
    using IntArray = int[arraySize];

    int intValue = 0;
    IntArray intArrayValue = { 20, 30, 40 };
    IntArray* intArrayPointerValue = &intArrayValue;
    std::vector<int> intVectorValue = {10, 20, 30};
    std::vector<int>* intVectorPointerValue = &intVectorValue;
    std::stack<int> intStackValue;
    intStackValue.push(10);
    intStackValue.push(20);
    intStackValue.push(30);
    std::stack<int>* intStackPointerValue = &intStackValue;
    
    RfS::Field<int, false> intField = { 0, "intValue", "int", 0, false, false, false };
    RfS::Field<IntArray, false> intArrayField = { 0, "intArrayValue", "int[3]", arraySize, true, false, false };
    RfS::Field<IntArray*, false> intArrayPointerField = { 0, "intArrayPointerValue", "int[3]*", arraySize, true, false, false };
    RfS::Field<std::vector<int>, false> intVectorField = { 0, "intVectorValue", "std::vector<int>", 0, true, false, false };
    RfS::Field<std::vector<int>*, false> intVectorPointerField = { 0, "intVectorPointerValue", "std::vector<int>*", 0, true, false, false };
    RfS::Field<std::stack<int>, false> intStackField = { 0, "intStackValue", "std::stack<int>", 0, true, false, false };
    RfS::Field<std::stack<int>*, false> intStackPointerField = { 0, "intStackPointerValue", "std::stack<int>*", 0, true, false, false };

    int timesVisited = 0;
    intField.ForPrimitivesConst(intValue, [&](auto primitive) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    intArrayField.ForPrimitivesConst(intArrayValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 20); break;
            case 1: EXPECT_EQ(primitive, 30); break;
            case 2: EXPECT_EQ(primitive, 40); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    intArrayPointerField.ForPrimitivesConst(intArrayPointerValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 20); break;
            case 1: EXPECT_EQ(primitive, 30); break;
            case 2: EXPECT_EQ(primitive, 40); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intVectorField.ForPrimitivesConst(intVectorValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 10); break;
            case 1: EXPECT_EQ(primitive, 20); break;
            case 2: EXPECT_EQ(primitive, 30); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intVectorPointerField.ForPrimitivesConst(intVectorPointerValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 10); break;
            case 1: EXPECT_EQ(primitive, 20); break;
            case 2: EXPECT_EQ(primitive, 30); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intStackField.ForPrimitivesConst(intStackValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 10); break;
            case 1: EXPECT_EQ(primitive, 20); break;
            case 2: EXPECT_EQ(primitive, 30); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intStackPointerField.ForPrimitivesConst(intStackPointerValue, [&](auto index, auto primitive) {
        switch ( index ) {
            case 0: EXPECT_EQ(primitive, 10); break;
            case 1: EXPECT_EQ(primitive, 20); break;
            case 2: EXPECT_EQ(primitive, 30); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
}

TEST(AcceleratorsTest, ForObjects)
{
    constexpr size_t arraySize = 3;
    using ObjArray = Obj[arraySize];

    Obj objValue = { 0, 0.1f };
    ObjArray objArrayValue = { { 20, 20.1f }, { 30, 30.1f }, { 40, 40.1f } };
    ObjArray* objArrayPointerValue = &objArrayValue;
    std::vector<Obj> objVectorValue = { { 10, 10.1f }, { 20, 20.1f }, { 30, 30.1f } };
    std::vector<Obj>* objVectorPointerValue = &objVectorValue;
    std::stack<Obj> objStackValue;
    objStackValue.push(objArrayValue[0]);
    objStackValue.push(objArrayValue[1]);
    objStackValue.push(objArrayValue[2]);
    std::stack<Obj>* objStackPointerValue = &objStackValue;
    
    RfS::Field<Obj, true> objField = { 0, "objValue", "Obj", 0, false, false, true };
    RfS::Field<ObjArray, true> objArrayField = { 0, "objArrayValue", "Obj[3]", arraySize, true, false, true };
    RfS::Field<ObjArray*, true> objArrayPointerField = { 0, "objArrayPointerValue", "Obj[3]*", arraySize, true, false, true };
    RfS::Field<std::vector<Obj>, true> objVectorField = { 0, "objVectorValue", "std::vector<Obj>", 0, true, false, true };
    RfS::Field<std::vector<Obj>*, true> objVectorPointerField = { 0, "objVectorPointerValue", "std::vector<Obj>*", 0, true, false, true };
    RfS::Field<std::stack<Obj>, false> objStackField = { 0, "objStackValue", "std::stack<Obj>", 0, true, false, true };
    RfS::Field<std::stack<Obj>*, false> objStackPointerField = { 0, "objStackPointerValue", "std::stack<Obj>*", 0, true, false, true };

    int timesVisited = 0;
    objField.ForObjects(objValue, [&](auto & object) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    objArrayField.ForObjects(objArrayValue, [&](auto index, auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], &object); break;
            case 1: EXPECT_EQ(&objArrayValue[1], &object); break;
            case 2: EXPECT_EQ(&objArrayValue[2], &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    objArrayPointerField.ForObjects(objArrayPointerValue, [&](auto index, auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&(*objArrayPointerValue)[0], &object); break;
            case 1: EXPECT_EQ(&(*objArrayPointerValue)[1], &object); break;
            case 2: EXPECT_EQ(&(*objArrayPointerValue)[2], &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objVectorField.ForObjects(objVectorValue, [&](auto index, auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objVectorValue[0], &object); break;
            case 1: EXPECT_EQ(&objVectorValue[1], &object); break;
            case 2: EXPECT_EQ(&objVectorValue[2], &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objVectorPointerField.ForObjects(objVectorPointerValue, [&](auto index, auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&(*objVectorPointerValue)[0], &object); break;
            case 1: EXPECT_EQ(&(*objVectorPointerValue)[1], &object); break;
            case 2: EXPECT_EQ(&(*objVectorPointerValue)[2], &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objStackField.ForObjects(objStackValue, [&](auto index, auto & object) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);

    timesVisited = 0;
    objStackPointerField.ForObjects(objStackPointerValue, [&](auto index, auto & object) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
}

TEST(AcceleratorsTest, ForObjectsConst)
{
    constexpr size_t arraySize = 3;
    using ObjArray = Obj[arraySize];

    Obj objValue = { 0, 0.1f };
    ObjArray objArrayValue = { { 20, 20.1f }, { 30, 30.1f }, { 40, 40.1f } };
    ObjArray* objArrayPointerValue = &objArrayValue;
    std::vector<Obj> objVectorValue = { { 10, 10.1f }, { 20, 20.1f }, { 30, 30.1f } };
    std::vector<Obj>* objVectorPointerValue = &objVectorValue;
    std::stack<Obj> objStackValue;
    objStackValue.push(objArrayValue[0]);
    objStackValue.push(objArrayValue[1]);
    objStackValue.push(objArrayValue[2]);
    std::stack<Obj>* objStackPointerValue = &objStackValue;
    
    RfS::Field<Obj, true> objField = { 0, "objValue", "Obj", 0, false, false, true };
    RfS::Field<ObjArray, true> objArrayField = { 0, "objArrayValue", "Obj[3]", arraySize, true, false, true };
    RfS::Field<ObjArray*, true> objArrayPointerField = { 0, "objArrayPointerValue", "Obj[3]*", arraySize, true, false, true };
    RfS::Field<std::vector<Obj>, true> objVectorField = { 0, "objVectorValue", "std::vector<Obj>", 0, true, false, true };
    RfS::Field<std::vector<Obj>*, true> objVectorPointerField = { 0, "objVectorPointerValue", "std::vector<Obj>*", 0, true, false, true };
    RfS::Field<std::stack<Obj>, true> objStackField = { 0, "objStackValue", "std::stack<Obj>", 0, true, false, true };
    RfS::Field<std::stack<Obj>*, true> objStackPointerField = { 0, "objStackPointerValue", "std::stack<Obj>*", 0, true, false, true };

    int timesVisited = 0;
    objField.ForObjectsConst(objValue, [&](auto & object) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    objArrayField.ForObjectsConst(objArrayValue, [&](auto index, auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], &object); break;
            case 1: EXPECT_EQ(&objArrayValue[1], &object); break;
            case 2: EXPECT_EQ(&objArrayValue[2], &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    objArrayPointerField.ForObjectsConst(objArrayPointerValue, [&](auto index, auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&(*objArrayPointerValue)[0], &object); break;
            case 1: EXPECT_EQ(&(*objArrayPointerValue)[1], &object); break;
            case 2: EXPECT_EQ(&(*objArrayPointerValue)[2], &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objVectorField.ForObjectsConst(objVectorValue, [&](auto index, auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objVectorValue[0], &object); break;
            case 1: EXPECT_EQ(&objVectorValue[1], &object); break;
            case 2: EXPECT_EQ(&objVectorValue[2], &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objVectorPointerField.ForObjectsConst(objVectorPointerValue, [&](auto index, auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&(*objVectorPointerValue)[0], &object); break;
            case 1: EXPECT_EQ(&(*objVectorPointerValue)[1], &object); break;
            case 2: EXPECT_EQ(&(*objVectorPointerValue)[2], &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    const auto & stackUnderlyingContainer = RfS::get_underlying_container(objStackValue);
    objStackField.ForObjectsConst((const std::stack<Obj> &)objStackValue, [&](auto index, const auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&(*(stackUnderlyingContainer.begin())), &object); break;
            case 1: EXPECT_EQ(&(*(++(stackUnderlyingContainer.begin()))), &object); break;
            case 2: EXPECT_EQ(&(*(++(++(stackUnderlyingContainer.begin())))), &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objStackPointerField.ForObjectsConst((const std::stack<Obj>* &)objStackPointerValue, [&](auto index, const auto & object) {
        switch ( index ) {
            case 0: EXPECT_EQ(&(*(stackUnderlyingContainer.begin())), &object); break;
            case 1: EXPECT_EQ(&(*(++(stackUnderlyingContainer.begin()))), &object); break;
            case 2: EXPECT_EQ(&(*(++(++(stackUnderlyingContainer.begin())))), &object); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
}

TEST(AcceleratorsTest, ForPrimitivePairs)
{
    int intValue = 0;
    std::vector<std::pair<int, float>> intToFloatVectorValue = { { 20, 20.1f }, { 30, 30.1f }, { 40, 40.1f } };
    std::vector<std::pair<int, float>>* intToFloatVectorPointerValue = &intToFloatVectorValue;
    std::map<int, float> intToFloatMapValue = { { 10, 10.1f }, { 20, 20.1f }, { 30, 30.1f } };
    std::map<int, float>* intToFloatMapPointerValue = &intToFloatMapValue;
    
    RfS::Field<int, false> intField = { 0, "intValue", "int", 0, false, false, false };
    RfS::Field<std::vector<std::pair<int, float>>, false> intToFloatVectorField = { 0, "intToFloatVectorValue", "std::vector<std::pair<int,float>>", 0, true, true, false };
    RfS::Field<std::vector<std::pair<int, float>>*, false> intToFloatVectorPointerField = { 0, "intToFloatVectorPointerValue", "std::vector<std::pair<int,float>>*", 0, true, true, false };
    RfS::Field<std::map<int, float>, false> intToFloatMapField = { 0, "intToFloatMapValue", "std::map<int,float>", 0, true, true, false };
    RfS::Field<std::map<int, float>*, false> intToFloatMapPointerField = { 0, "intToFloatMapPointerValue", "std::map<int,float>*", 0, true, true, false };

    int timesVisited = 0;
    intField.ForPrimitivePairs(intValue, [&](auto index, auto first, auto second) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    intToFloatVectorField.ForPrimitivePairs(intToFloatVectorValue, [&](auto index, auto first, auto second) {
        switch ( index ) {
            case 0: EXPECT_EQ(intToFloatVectorValue[0].first, first); EXPECT_EQ(intToFloatVectorValue[0].second, second); break;
            case 1: EXPECT_EQ(intToFloatVectorValue[1].first, first); EXPECT_EQ(intToFloatVectorValue[1].second, second); break;
            case 2: EXPECT_EQ(intToFloatVectorValue[2].first, first); EXPECT_EQ(intToFloatVectorValue[2].second, second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    intToFloatVectorPointerField.ForPrimitivePairs(intToFloatVectorPointerValue, [&](auto index, auto first, auto second) {
        switch ( index ) {
            case 0: EXPECT_EQ((*intToFloatVectorPointerValue)[0].first, first); EXPECT_EQ((*intToFloatVectorPointerValue)[0].second, second); break;
            case 1: EXPECT_EQ((*intToFloatVectorPointerValue)[1].first, first); EXPECT_EQ((*intToFloatVectorPointerValue)[1].second, second); break;
            case 2: EXPECT_EQ((*intToFloatVectorPointerValue)[2].first, first); EXPECT_EQ((*intToFloatVectorPointerValue)[2].second, second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intToFloatMapField.ForPrimitivePairs(intToFloatMapValue, [&](auto index, auto first, auto second) {
        switch ( index ) {
            case 0: EXPECT_EQ(intToFloatMapValue.begin()->first, first); EXPECT_EQ(intToFloatMapValue.begin()->second, second); break;
            case 1: EXPECT_EQ((++intToFloatMapValue.begin())->first, first); EXPECT_EQ((++intToFloatMapValue.begin())->second, second); break;
            case 2: EXPECT_EQ((++(++intToFloatMapValue.begin()))->first, first); EXPECT_EQ((++(++intToFloatMapValue.begin()))->second, second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intToFloatMapPointerField.ForPrimitivePairs(intToFloatMapPointerValue, [&](auto index, auto first, auto second) {
        switch ( index ) {
            case 0: EXPECT_EQ((*intToFloatMapPointerValue).begin()->first, first); EXPECT_EQ((*intToFloatMapPointerValue).begin()->second, second); break;
            case 1: EXPECT_EQ((++(*intToFloatMapPointerValue).begin())->first, first); EXPECT_EQ((++(*intToFloatMapPointerValue).begin())->second, second); break;
            case 2: EXPECT_EQ((++(++(*intToFloatMapPointerValue).begin()))->first, first); EXPECT_EQ((++(++(*intToFloatMapPointerValue).begin()))->second, second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
}

TEST(AcceleratorsTest, ForObjectPairs)
{
    Obj objValue = { 0, 0.1f };
    std::vector<std::pair<int, Obj>> intToObjVectorValue = { { 20, { 21, 20.1f } }, { 30, { 31, 30.1f } }, { 40, { 41, 40.1f } } };
    std::vector<std::pair<int, Obj>>* intToObjVectorPointerValue = &intToObjVectorValue;
    std::map<int, Obj> intToObjMapValue = { { 10, { 11, 10.1f } }, { 20, { 21, 20.1f } }, { 30, { 31, 30.1f } } };
    std::map<int, Obj>* intToObjMapPointerValue = &intToObjMapValue;
    
    RfS::Field<Obj, true> objField = { 0, "objValue", "Obj", 0, false, false, true };
    RfS::Field<std::vector<std::pair<int, Obj>>, true> objToFloatVectorField = { 0, "intToObjVectorValue", "std::vector<std::pair<int,Obj>>", 0, true, true, true };
    RfS::Field<std::vector<std::pair<int, Obj>>*, true> objToFloatVectorPointerField = { 0, "intToObjVectorPointerValue", "std::vector<std::pair<int,Obj>>", 0, true, true, true };
    RfS::Field<std::map<int, Obj>, true> objToFloatMapField = { 0, "intToObjMapValue", "std::map<int,Obj>", 0, true, true, true };
    RfS::Field<std::map<int, Obj>*, true> objToFloatMapPointerField = { 0, "intToObjMapPointerValue", "std::map<int,Obj>", 0, true, true, true };

    int timesVisited = 0;
    objField.ForObjectPairs(objValue, [&](auto index, auto first, auto & second) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    objToFloatVectorField.ForObjectPairs(intToObjVectorValue, [&](auto index, auto first, auto & second) {
        switch ( index ) {
            case 0: EXPECT_EQ(intToObjVectorValue[0].first, first); EXPECT_EQ(&intToObjVectorValue[0].second, &second); break;
            case 1: EXPECT_EQ(intToObjVectorValue[1].first, first); EXPECT_EQ(&intToObjVectorValue[1].second, &second); break;
            case 2: EXPECT_EQ(intToObjVectorValue[2].first, first); EXPECT_EQ(&intToObjVectorValue[2].second, &second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    objToFloatVectorPointerField.ForObjectPairs(intToObjVectorPointerValue, [&](auto index, auto first, auto & second) {
        switch ( index ) {
            case 0: EXPECT_EQ((*intToObjVectorPointerValue)[0].first, first); EXPECT_EQ(&(*intToObjVectorPointerValue)[0].second, &second); break;
            case 1: EXPECT_EQ((*intToObjVectorPointerValue)[1].first, first); EXPECT_EQ(&(*intToObjVectorPointerValue)[1].second, &second); break;
            case 2: EXPECT_EQ((*intToObjVectorPointerValue)[2].first, first); EXPECT_EQ(&(*intToObjVectorPointerValue)[2].second, &second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objToFloatMapField.ForObjectPairs(intToObjMapValue, [&](auto index, auto first, auto & second) {
        switch ( index ) {
            case 0: EXPECT_EQ(intToObjMapValue.begin()->first, first); EXPECT_EQ(&intToObjMapValue.begin()->second, &second); break;
            case 1: EXPECT_EQ((++intToObjMapValue.begin())->first, first); EXPECT_EQ(&(++intToObjMapValue.begin())->second, &second); break;
            case 2: EXPECT_EQ((++(++intToObjMapValue.begin()))->first, first); EXPECT_EQ(&(++(++intToObjMapValue.begin()))->second, &second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objToFloatMapPointerField.ForObjectPairs(intToObjMapPointerValue, [&](auto index, auto first, auto & second) {
        switch ( index ) {
            case 0: EXPECT_EQ((*intToObjMapPointerValue).begin()->first, first); EXPECT_EQ(&(*intToObjMapPointerValue).begin()->second, &second); break;
            case 1: EXPECT_EQ((++(*intToObjMapPointerValue).begin())->first, first); EXPECT_EQ(&(++(*intToObjMapPointerValue).begin())->second, &second); break;
            case 2: EXPECT_EQ((++(++(*intToObjMapPointerValue).begin()))->first, first); EXPECT_EQ(&(++(++(*intToObjMapPointerValue).begin()))->second, &second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
}

TEST(AcceleratorsTest, ForPrimitivePointers)
{
    constexpr size_t arraySize = 3;
    using IntArray = int[arraySize];
    using IntPointerArray = int*[arraySize];

    int intValue = 0;
    IntArray intArrayValue = { 20, 30, 40 };
    IntPointerArray intPointerArrayValue = { &intArrayValue[0], &intArrayValue[1], &intArrayValue[2] };
    IntPointerArray* intPointerArrayPointerValue = &intPointerArrayValue;
    std::vector<int*> intPointerVectorValue = { &intArrayValue[0], &intArrayValue[1], &intArrayValue[2] };
    std::vector<int*>* intPointerVectorPointerValue = &intPointerVectorValue;
    std::stack<int*> intPointerStackValue;
    intPointerStackValue.push(&intArrayValue[0]);
    intPointerStackValue.push(&intArrayValue[1]);
    intPointerStackValue.push(&intArrayValue[2]);
    std::stack<int*>* intPointerStackPointerValue = &intPointerStackValue;
    
    RfS::Field<int, false> intField = { 0, "intValue", "int", 0, false, false, false };
    RfS::Field<IntPointerArray, false> intPointerArrayField = { 0, "intPointerArrayValue", "int*[3]", arraySize, true, false, true };
    RfS::Field<IntPointerArray*, false> intPointerArrayPointerField = { 0, "intPointerArrayPointerValue", "int*[3]*", arraySize, true, false, true };
    RfS::Field<std::vector<int*>, false> intPointerVectorField = { 0, "intPointerVectorValue", "std::vector<int*>", 0, true, false, true };
    RfS::Field<std::vector<int*>*, false> intPointerVectorPointerField = { 0, "intPointerVectorPointerValue", "std::vector<int*>*", 0, true, false, true };
    RfS::Field<std::stack<int*>, false> intPointerStackField = { 0, "intPointerStackValue", "std::stack<int*>", 0, true, false, false };
    RfS::Field<std::stack<int*>*, false> intPointerStackPointerField = { 0, "intPointerStackPointerValue", "std::stack<int*>*", 0, true, false, false };

    int timesVisited = 0;
    intField.ForPrimitivePointers(intValue, [&](auto index, auto primitivePointer) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    intPointerArrayField.ForPrimitivePointers(intPointerArrayValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    intPointerArrayPointerField.ForPrimitivePointers(intPointerArrayPointerValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intPointerVectorField.ForPrimitivePointers(intPointerVectorValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intPointerVectorPointerField.ForPrimitivePointers(intPointerVectorPointerValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intPointerStackField.ForPrimitivePointers(intPointerStackValue, [&](auto index, auto primitivePointer) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);

    timesVisited = 0;
    intPointerStackPointerField.ForPrimitivePointers(intPointerStackPointerValue, [&](auto index, auto primitivePointer) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
}

TEST(AcceleratorsTest, ForPrimitivePointersConst)
{
    constexpr size_t arraySize = 3;
    using IntArray = int[arraySize];
    using IntPointerArray = int*[arraySize];

    int intValue = 0;
    IntArray intArrayValue = { 20, 30, 40 };
    IntPointerArray intPointerArrayValue = { &intArrayValue[0], &intArrayValue[1], &intArrayValue[2] };
    IntPointerArray* intPointerArrayPointerValue = &intPointerArrayValue;
    std::vector<int*> intPointerVectorValue = { &intArrayValue[0], &intArrayValue[1], &intArrayValue[2] };
    std::vector<int*>* intPointerVectorPointerValue = &intPointerVectorValue;
    std::stack<int*> intPointerStackValue;
    intPointerStackValue.push(&intArrayValue[0]);
    intPointerStackValue.push(&intArrayValue[1]);
    intPointerStackValue.push(&intArrayValue[2]);
    std::stack<int*>* intPointerStackPointerValue = &intPointerStackValue;
    
    RfS::Field<int, false> intField = { 0, "intValue", "int", 0, false, false, false };
    RfS::Field<IntPointerArray, false> intPointerArrayField = { 0, "intPointerArrayValue", "int*[3]", arraySize, true, false, true };
    RfS::Field<IntPointerArray*, false> intPointerArrayPointerField = { 0, "intPointerArrayPointerValue", "int*[3]*", arraySize, true, false, true };
    RfS::Field<std::vector<int*>, false> intPointerVectorField = { 0, "intPointerVectorValue", "std::vector<int*>", 0, true, false, true };
    RfS::Field<std::vector<int*>*, false> intPointerVectorPointerField = { 0, "intPointerVectorPointerValue", "std::vector<int*>*", 0, true, false, true };
    RfS::Field<std::stack<int*>, false> intPointerStackField = { 0, "intPointerStackValue", "std::stack<int*>", 0, true, false, false };
    RfS::Field<std::stack<int*>*, false> intPointerStackPointerField = { 0, "intPointerStackPointerValue", "std::stack<int*>*", 0, true, false, false };

    int timesVisited = 0;
    intField.ForPrimitivePointersConst(intValue, [&](auto index, auto primitivePointer) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    intPointerArrayField.ForPrimitivePointersConst(intPointerArrayValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    intPointerArrayPointerField.ForPrimitivePointersConst(intPointerArrayPointerValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intPointerVectorField.ForPrimitivePointersConst(intPointerVectorValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intPointerVectorPointerField.ForPrimitivePointersConst(intPointerVectorPointerValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intPointerStackField.ForPrimitivePointersConst(intPointerStackValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intPointerStackPointerField.ForPrimitivePointersConst(intPointerStackPointerValue, [&](auto index, auto primitivePointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&intArrayValue[0], primitivePointer); break;
            case 1: EXPECT_EQ(&intArrayValue[1], primitivePointer); break;
            case 2: EXPECT_EQ(&intArrayValue[2], primitivePointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
}

TEST(AcceleratorsTest, ForObjectPointers)
{
    constexpr size_t arraySize = 3;
    using ObjArray = Obj[arraySize];
    using ObjPointerArray = Obj*[arraySize];

    Obj objValue = { 0, 0.1f };
    ObjArray objArrayValue = { { 20, 20.1f }, { 30, 30.1f }, { 40, 40.1f } };
    ObjPointerArray objPointerArrayValue = { &objArrayValue[0], &objArrayValue[1], &objArrayValue[2] };
    ObjPointerArray* objPointerArrayPointerValue = &objPointerArrayValue;
    std::vector<Obj*> objPointerVectorValue = { &objArrayValue[0], &objArrayValue[1], &objArrayValue[2] };
    std::vector<Obj*>* objPointerVectorPointerValue = &objPointerVectorValue;
    std::stack<Obj*> objPointerStackValue;
    objPointerStackValue.push(&objArrayValue[0]);
    objPointerStackValue.push(&objArrayValue[1]);
    objPointerStackValue.push(&objArrayValue[2]);
    std::stack<Obj*>* objPointerStackPointerValue = &objPointerStackValue;
    
    RfS::Field<Obj, true> objField = { 0, "objValue", "Obj", 0, false, false, true };
    RfS::Field<ObjPointerArray, true> objPointerArrayField = { 0, "objPointerArrayValue", "Obj*[3]", arraySize, true, false, true };
    RfS::Field<ObjPointerArray*, true> objPointerArrayPointerField = { 0, "objPointerArrayPointerValue", "Obj*[3]*", arraySize, true, false, true };
    RfS::Field<std::vector<Obj*>, true> objPointerVectorField = { 0, "objPointerVectorValue", "std::vector<Obj*>", 0, true, false, true };
    RfS::Field<std::vector<Obj*>*, true> objPointerVectorPointerField = { 0, "objPointerVectorPointerValue", "std::vector<Obj*>", 0, true, false, true };
    RfS::Field<std::stack<Obj*>, false> objPointerStackField = { 0, "objPointerStackValue", "std::stack<Obj*>", 0, true, false, true };
    RfS::Field<std::stack<Obj*>*, false> objPointerStackPointerField = { 0, "objPointerStackPointerValue", "std::stack<Obj*>*", 0, true, false, true };

    int timesVisited = 0;
    objField.ForObjectPointers(objValue, [&](auto index, auto & objectPointer) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    objPointerArrayField.ForObjectPointers(objPointerArrayValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    objPointerArrayPointerField.ForObjectPointers(objPointerArrayPointerValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objPointerVectorField.ForObjectPointers(objPointerVectorValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objPointerVectorPointerField.ForObjectPointers(objPointerVectorPointerValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objPointerStackField.ForObjectPointers(objPointerStackValue, [&](auto index, auto & objectPointer) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);

    timesVisited = 0;
    objPointerStackPointerField.ForObjectPointers(objPointerStackPointerValue, [&](auto index, auto & objectPointer) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
}

TEST(AcceleratorsTest, ForObjectPointersConst)
{
    constexpr size_t arraySize = 3;
    using ObjArray = Obj[arraySize];
    using ObjPointerArray = Obj*[arraySize];

    Obj objValue = { 0, 0.1f };
    ObjArray objArrayValue = { { 20, 20.1f }, { 30, 30.1f }, { 40, 40.1f } };
    ObjPointerArray objPointerArrayValue = { &objArrayValue[0], &objArrayValue[1], &objArrayValue[2] };
    ObjPointerArray* objPointerArrayPointerValue = &objPointerArrayValue;
    std::vector<Obj*> objPointerVectorValue = { &objArrayValue[0], &objArrayValue[1], &objArrayValue[2] };
    std::vector<Obj*>* objPointerVectorPointerValue = &objPointerVectorValue;
    std::stack<Obj*> objPointerStackValue;
    objPointerStackValue.push(&objArrayValue[0]);
    objPointerStackValue.push(&objArrayValue[1]);
    objPointerStackValue.push(&objArrayValue[2]);
    std::stack<Obj*>* objPointerStackPointerValue = &objPointerStackValue;
    
    RfS::Field<Obj, true> objField = { 0, "objValue", "Obj", 0, false, false, true };
    RfS::Field<ObjPointerArray, true> objPointerArrayField = { 0, "objPointerArrayValue", "Obj*[3]", arraySize, true, false, true };
    RfS::Field<ObjPointerArray*, true> objPointerArrayPointerField = { 0, "objPointerArrayPointerValue", "Obj*[3]*", arraySize, true, false, true };
    RfS::Field<std::vector<Obj*>, true> objPointerVectorField = { 0, "objPointerVectorValue", "std::vector<Obj*>", 0, true, false, true };
    RfS::Field<std::vector<Obj*>*, true> objPointerVectorPointerField = { 0, "objPointerVectorPointerValue", "std::vector<Obj*>", 0, true, false, true };
    RfS::Field<std::stack<Obj*>, false> objPointerStackField = { 0, "objPointerStackValue", "std::stack<Obj*>", 0, true, false, true };
    RfS::Field<std::stack<Obj*>*, false> objPointerStackPointerField = { 0, "objPointerStackPointerValue", "std::stack<Obj*>*", 0, true, false, true };

    int timesVisited = 0;
    objField.ForObjectPointers(objValue, [&](auto index, auto & objectPointer) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    objPointerArrayField.ForObjectPointersConst(objPointerArrayValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    objPointerArrayPointerField.ForObjectPointersConst(objPointerArrayPointerValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objPointerVectorField.ForObjectPointersConst(objPointerVectorValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objPointerVectorPointerField.ForObjectPointersConst(objPointerVectorPointerValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objPointerStackField.ForObjectPointersConst(objPointerStackValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);

    timesVisited = 0;
    objPointerStackPointerField.ForObjectPointersConst(objPointerStackPointerValue, [&](auto index, auto & objectPointer) {
        switch ( index ) {
            case 0: EXPECT_EQ(&objArrayValue[0], objectPointer); break;
            case 1: EXPECT_EQ(&objArrayValue[1], objectPointer); break;
            case 2: EXPECT_EQ(&objArrayValue[2], objectPointer); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
}

TEST(AcceleratorsTest, ForPrimitivePointerPairs)
{
    int intValue = 0;
    float floatValues[] = { 20.1f, 30.1f, 40.1f };
    std::vector<std::pair<int, float*>> intToFloatPointerVectorValue = { { 20, &floatValues[0] }, { 30, &floatValues[1] }, { 40, &floatValues[2] } };
    std::vector<std::pair<int, float*>>* intToFloatPointerVectorPointerValue = &intToFloatPointerVectorValue;
    std::map<int, float*> intToFloatPointerMapValue = { { 20, &floatValues[0] }, { 30, &floatValues[1] }, { 40, &floatValues[2] } };
    std::map<int, float*>* intToFloatPointerMapPointerValue = &intToFloatPointerMapValue;
    
    RfS::Field<int, false> intField = { 0, "intValue", "int", 0, false, false, false };
    RfS::Field<std::vector<std::pair<int, float*>>, false> intToFloatPointerVectorField = { 0, "intToFloatPointerVectorValue", "std::vector<std::pair<int,float*>>", 0, true, true, true };
    RfS::Field<std::vector<std::pair<int, float*>>*, false> intToFloatPointerVectorPointerField = { 0, "intToFloatPointerVectorPointerValue", "std::vector<std::pair<int,float*>>*", 0, true, true, true };
    RfS::Field<std::map<int, float*>, false> intToFloatPointerMapField = { 0, "intToFloatPointerMapValue", "std::map<int,float*>", 0, true, true, true };
    RfS::Field<std::map<int, float*>*, false> intToFloatPointerMapPointerField = { 0, "intToFloatPointerMapPointerValue", "std::map<int,float*>*", 0, true, true, true };

    int timesVisited = 0;
    intField.ForPrimitivePointerPairs(intValue, [&](auto index, auto first, auto second) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    intToFloatPointerVectorField.ForPrimitivePointerPairs(intToFloatPointerVectorValue, [&](auto index, auto first, auto second) {
        switch ( index ) {
            case 0: EXPECT_EQ(20, first); EXPECT_EQ(&floatValues[0], second); break;
            case 1: EXPECT_EQ(30, first); EXPECT_EQ(&floatValues[1], second); break;
            case 2: EXPECT_EQ(40, first); EXPECT_EQ(&floatValues[2], second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    intToFloatPointerVectorPointerField.ForPrimitivePointerPairs(intToFloatPointerVectorPointerValue, [&](auto index, auto first, auto second) {
        switch ( index ) {
            case 0: EXPECT_EQ(20, first); EXPECT_EQ(&floatValues[0], second); break;
            case 1: EXPECT_EQ(30, first); EXPECT_EQ(&floatValues[1], second); break;
            case 2: EXPECT_EQ(40, first); EXPECT_EQ(&floatValues[2], second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intToFloatPointerMapField.ForPrimitivePointerPairs(intToFloatPointerMapValue, [&](auto index, auto first, auto second) {
        switch ( index ) {
            case 0: EXPECT_EQ(20, first); EXPECT_EQ(&floatValues[0], second); break;
            case 1: EXPECT_EQ(30, first); EXPECT_EQ(&floatValues[1], second); break;
            case 2: EXPECT_EQ(40, first); EXPECT_EQ(&floatValues[2], second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    intToFloatPointerMapPointerField.ForPrimitivePointerPairs(intToFloatPointerMapPointerValue, [&](auto index, auto first, auto second) {
        switch ( index ) {
            case 0: EXPECT_EQ(20, first); EXPECT_EQ(&floatValues[0], second); break;
            case 1: EXPECT_EQ(30, first); EXPECT_EQ(&floatValues[1], second); break;
            case 2: EXPECT_EQ(40, first); EXPECT_EQ(&floatValues[2], second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
}

TEST(AcceleratorsTest, ForObjectPointerPairs)
{
    Obj objValue = { 0, 0.1f };
    Obj objArrayValue[] = { { 21, 20.1f }, { 31, 30.1f }, { 41, 40.1f } };
    std::vector<std::pair<int, Obj*>> intToObjPointerVectorValue = { { 20, &objArrayValue[0] }, { 30, &objArrayValue[1] }, { 40, &objArrayValue[2] } };
    std::vector<std::pair<int, Obj*>>* intToObjPointerVectorPointerValue = &intToObjPointerVectorValue;
    std::map<int, Obj*> intToObjPointerMapValue = { { 20, &objArrayValue[0] }, { 30, &objArrayValue[1] }, { 40, &objArrayValue[2] } };
    std::map<int, Obj*>* intToObjPointerMapPointerValue = &intToObjPointerMapValue;
    
    RfS::Field<Obj, true> objField = { 0, "objValue", "Obj", 0, false, false, false };
    RfS::Field<std::vector<std::pair<int, Obj*>>, true> objPointerToFloatVectorField = { 0, "intToObjPointerVectorValue", "std::vector<std::pair<int,Obj*>>", 0, true, true, true };
    RfS::Field<std::vector<std::pair<int, Obj*>>*, true> objPointerToFloatVectorPointerField = { 0, "intToObjPointerVectorPointerValue", "std::vector<std::pair<int,Obj*>>", 0, true, true, true };
    RfS::Field<std::map<int, Obj*>, true> objPointerToFloatMapField = { 0, "intToObjPointerMapValue", "std::map<int,Obj*>", 0, true, true, true };
    RfS::Field<std::map<int, Obj*>*, true> objPointerToFloatMapPointerField = { 0, "intToObjPointerMapPointerValue", "std::map<int,Obj*>*", 0, true, true, true };

    int timesVisited = 0;
    objField.ForObjectPointerPairs(objValue, [&](auto object) {
        timesVisited++;
    });
    EXPECT_EQ(0, timesVisited);
    
    timesVisited = 0;
    objPointerToFloatVectorField.ForObjectPointerPairs(intToObjPointerVectorValue, [&](auto index, auto first, auto & second) {
        switch ( index ) {
            case 0: EXPECT_EQ(20, first); EXPECT_EQ(&objArrayValue[0], second); break;
            case 1: EXPECT_EQ(30, first); EXPECT_EQ(&objArrayValue[1], second); break;
            case 2: EXPECT_EQ(40, first); EXPECT_EQ(&objArrayValue[2], second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
    
    timesVisited = 0;
    objPointerToFloatVectorPointerField.ForObjectPointerPairs(intToObjPointerVectorPointerValue, [&](auto index, auto first, auto & second) {
        switch ( index ) {
            case 0: EXPECT_EQ(20, first); EXPECT_EQ(&objArrayValue[0], second); break;
            case 1: EXPECT_EQ(30, first); EXPECT_EQ(&objArrayValue[1], second); break;
            case 2: EXPECT_EQ(40, first); EXPECT_EQ(&objArrayValue[2], second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objPointerToFloatMapField.ForObjectPointerPairs(intToObjPointerMapValue, [&](auto index, auto first, auto & second) {
        switch ( index ) {
            case 0: EXPECT_EQ(20, first); EXPECT_EQ(&objArrayValue[0], second); break;
            case 1: EXPECT_EQ(30, first); EXPECT_EQ(&objArrayValue[1], second); break;
            case 2: EXPECT_EQ(40, first); EXPECT_EQ(&objArrayValue[2], second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);

    timesVisited = 0;
    objPointerToFloatMapPointerField.ForObjectPointerPairs(intToObjPointerMapPointerValue, [&](auto index, auto first, auto & second) {
        switch ( index ) {
            case 0: EXPECT_EQ(20, first); EXPECT_EQ(&objArrayValue[0], second); break;
            case 1: EXPECT_EQ(30, first); EXPECT_EQ(&objArrayValue[1], second); break;
            case 2: EXPECT_EQ(40, first); EXPECT_EQ(&objArrayValue[2], second); break;
            default: EXPECT_TRUE(false); break;
        }
        timesVisited++;
    });
    EXPECT_EQ(3, timesVisited);
}
