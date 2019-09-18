#ifndef REFLECT_H
#define REFLECT_H

#ifndef enum_t
/// enum_t "enum type (scoped)" assumes the property of enum classes that encloses the enum values within a particular scope
/// e.g. MyClass::MyEnum::Value cannot be accessed via MyClass::Value (as it could with regular enums) and potentially cause redefinition errors
/// while avoiding the property of enum classes that removes the one-to-one relationship with the underlying type (which forces excessive type-casting)
///
/// Usage:
/// enum_t(name, type, {
///     enumerator = constexpr,
///     enumerator = constexpr,
///     ...
/// });
/* enum_t "enum type (scoped)" documentation minimized for expansion visibility, see definition for description and usage */
#define enum_t(name, type, ...) struct name ## _ { enum type ## _ : type __VA_ARGS__; }; using name = name ## _::type ## _;
#endif

/// Contains everything neccessary to loop over varadic macro arguments
namespace MacroLoops {

/// ArgMax: 62 (derived from the C spec limiting macros to 126 arguments, and the below macros having the largest argument expansion of __VA_ARGS__, MaxArgs, MaxArgs-1, MaxArgs-2, ..., 2, 1, 0)

/// MacroLoop_Expand (necessary due to bugs in VS macro handling)
#define ML_E(x) x

/// MacroLoop_Concatenate (necessary due to bugs in VS macro handling)
#define ML_C(x,y) x##y

/// MacroLoop_ForEach[1, ..., ArgMax]
#define ML_1(f,a,...) f(a)
#define ML_2(f,a,...) f(a) ML_E(ML_1(f,__VA_ARGS__))
#define ML_3(f,a,...) f(a) ML_E(ML_2(f,__VA_ARGS__))
#define ML_4(f,a,...) f(a) ML_E(ML_3(f,__VA_ARGS__))
#define ML_5(f,a,...) f(a) ML_E(ML_4(f,__VA_ARGS__))
#define ML_6(f,a,...) f(a) ML_E(ML_5(f,__VA_ARGS__))
#define ML_7(f,a,...) f(a) ML_E(ML_6(f,__VA_ARGS__))
#define ML_8(f,a,...) f(a) ML_E(ML_7(f,__VA_ARGS__))
#define ML_9(f,a,...) f(a) ML_E(ML_8(f,__VA_ARGS__))
#define ML_10(f,a,...) f(a) ML_E(ML_9(f,__VA_ARGS__))
#define ML_11(f,a,...) f(a) ML_E(ML_10(f,__VA_ARGS__))
#define ML_12(f,a,...) f(a) ML_E(ML_11(f,__VA_ARGS__))
#define ML_13(f,a,...) f(a) ML_E(ML_12(f,__VA_ARGS__))
#define ML_14(f,a,...) f(a) ML_E(ML_13(f,__VA_ARGS__))
#define ML_15(f,a,...) f(a) ML_E(ML_14(f,__VA_ARGS__))
#define ML_16(f,a,...) f(a) ML_E(ML_15(f,__VA_ARGS__))
#define ML_17(f,a,...) f(a) ML_E(ML_16(f,__VA_ARGS__))
#define ML_18(f,a,...) f(a) ML_E(ML_17(f,__VA_ARGS__))
#define ML_19(f,a,...) f(a) ML_E(ML_18(f,__VA_ARGS__))
#define ML_20(f,a,...) f(a) ML_E(ML_19(f,__VA_ARGS__))
#define ML_21(f,a,...) f(a) ML_E(ML_20(f,__VA_ARGS__))
#define ML_22(f,a,...) f(a) ML_E(ML_21(f,__VA_ARGS__))
#define ML_23(f,a,...) f(a) ML_E(ML_22(f,__VA_ARGS__))
#define ML_24(f,a,...) f(a) ML_E(ML_23(f,__VA_ARGS__))
#define ML_25(f,a,...) f(a) ML_E(ML_24(f,__VA_ARGS__))
#define ML_26(f,a,...) f(a) ML_E(ML_25(f,__VA_ARGS__))
#define ML_27(f,a,...) f(a) ML_E(ML_26(f,__VA_ARGS__))
#define ML_28(f,a,...) f(a) ML_E(ML_27(f,__VA_ARGS__))
#define ML_29(f,a,...) f(a) ML_E(ML_28(f,__VA_ARGS__))
#define ML_30(f,a,...) f(a) ML_E(ML_29(f,__VA_ARGS__))
#define ML_31(f,a,...) f(a) ML_E(ML_30(f,__VA_ARGS__))
#define ML_32(f,a,...) f(a) ML_E(ML_31(f,__VA_ARGS__))
#define ML_33(f,a,...) f(a) ML_E(ML_32(f,__VA_ARGS__))
#define ML_34(f,a,...) f(a) ML_E(ML_33(f,__VA_ARGS__))
#define ML_35(f,a,...) f(a) ML_E(ML_34(f,__VA_ARGS__))
#define ML_36(f,a,...) f(a) ML_E(ML_35(f,__VA_ARGS__))
#define ML_37(f,a,...) f(a) ML_E(ML_36(f,__VA_ARGS__))
#define ML_38(f,a,...) f(a) ML_E(ML_37(f,__VA_ARGS__))
#define ML_39(f,a,...) f(a) ML_E(ML_38(f,__VA_ARGS__))
#define ML_40(f,a,...) f(a) ML_E(ML_39(f,__VA_ARGS__))
#define ML_41(f,a,...) f(a) ML_E(ML_40(f,__VA_ARGS__))
#define ML_42(f,a,...) f(a) ML_E(ML_41(f,__VA_ARGS__))
#define ML_43(f,a,...) f(a) ML_E(ML_42(f,__VA_ARGS__))
#define ML_44(f,a,...) f(a) ML_E(ML_43(f,__VA_ARGS__))
#define ML_45(f,a,...) f(a) ML_E(ML_44(f,__VA_ARGS__))
#define ML_46(f,a,...) f(a) ML_E(ML_45(f,__VA_ARGS__))
#define ML_47(f,a,...) f(a) ML_E(ML_46(f,__VA_ARGS__))
#define ML_48(f,a,...) f(a) ML_E(ML_47(f,__VA_ARGS__))
#define ML_49(f,a,...) f(a) ML_E(ML_48(f,__VA_ARGS__))
#define ML_50(f,a,...) f(a) ML_E(ML_49(f,__VA_ARGS__))
#define ML_51(f,a,...) f(a) ML_E(ML_50(f,__VA_ARGS__))
#define ML_52(f,a,...) f(a) ML_E(ML_51(f,__VA_ARGS__))
#define ML_53(f,a,...) f(a) ML_E(ML_52(f,__VA_ARGS__))
#define ML_54(f,a,...) f(a) ML_E(ML_53(f,__VA_ARGS__))
#define ML_55(f,a,...) f(a) ML_E(ML_54(f,__VA_ARGS__))
#define ML_56(f,a,...) f(a) ML_E(ML_55(f,__VA_ARGS__))
#define ML_57(f,a,...) f(a) ML_E(ML_56(f,__VA_ARGS__))
#define ML_58(f,a,...) f(a) ML_E(ML_57(f,__VA_ARGS__))
#define ML_59(f,a,...) f(a) ML_E(ML_58(f,__VA_ARGS__))
#define ML_60(f,a,...) f(a) ML_E(ML_59(f,__VA_ARGS__))
#define ML_61(f,a,...) f(a) ML_E(ML_60(f,__VA_ARGS__))
#define ML_62(f,a,...) f(a) ML_E(ML_61(f,__VA_ARGS__))

/// MacroLoop_ForEachPair[2, 4, ..., ArgMax-ArgMax%2]
#define ML_P2(f,a,b,...) f(a,b)
#define ML_P4(f,a,b,...) f(a,b) ML_E(ML_P2(f,__VA_ARGS__))
#define ML_P6(f,a,b,...) f(a,b) ML_E(ML_P4(f,__VA_ARGS__))
#define ML_P8(f,a,b,...) f(a,b) ML_E(ML_P6(f,__VA_ARGS__))
#define ML_P10(f,a,b,...) f(a,b) ML_E(ML_P8(f,__VA_ARGS__))
#define ML_P12(f,a,b,...) f(a,b) ML_E(ML_P10(f,__VA_ARGS__))
#define ML_P14(f,a,b,...) f(a,b) ML_E(ML_P12(f,__VA_ARGS__))
#define ML_P16(f,a,b,...) f(a,b) ML_E(ML_P14(f,__VA_ARGS__))
#define ML_P18(f,a,b,...) f(a,b) ML_E(ML_P16(f,__VA_ARGS__))
#define ML_P20(f,a,b,...) f(a,b) ML_E(ML_P18(f,__VA_ARGS__))
#define ML_P22(f,a,b,...) f(a,b) ML_E(ML_P20(f,__VA_ARGS__))
#define ML_P24(f,a,b,...) f(a,b) ML_E(ML_P22(f,__VA_ARGS__))
#define ML_P26(f,a,b,...) f(a,b) ML_E(ML_P24(f,__VA_ARGS__))
#define ML_P28(f,a,b,...) f(a,b) ML_E(ML_P26(f,__VA_ARGS__))
#define ML_P30(f,a,b,...) f(a,b) ML_E(ML_P28(f,__VA_ARGS__))
#define ML_P32(f,a,b,...) f(a,b) ML_E(ML_P30(f,__VA_ARGS__))
#define ML_P34(f,a,b,...) f(a,b) ML_E(ML_P32(f,__VA_ARGS__))
#define ML_P36(f,a,b,...) f(a,b) ML_E(ML_P34(f,__VA_ARGS__))
#define ML_P38(f,a,b,...) f(a,b) ML_E(ML_P36(f,__VA_ARGS__))
#define ML_P40(f,a,b,...) f(a,b) ML_E(ML_P38(f,__VA_ARGS__))
#define ML_P42(f,a,b,...) f(a,b) ML_E(ML_P40(f,__VA_ARGS__))
#define ML_P44(f,a,b,...) f(a,b) ML_E(ML_P42(f,__VA_ARGS__))
#define ML_P46(f,a,b,...) f(a,b) ML_E(ML_P44(f,__VA_ARGS__))
#define ML_P48(f,a,b,...) f(a,b) ML_E(ML_P46(f,__VA_ARGS__))
#define ML_P50(f,a,b,...) f(a,b) ML_E(ML_P48(f,__VA_ARGS__))
#define ML_P52(f,a,b,...) f(a,b) ML_E(ML_P50(f,__VA_ARGS__))
#define ML_P54(f,a,b,...) f(a,b) ML_E(ML_P52(f,__VA_ARGS__))
#define ML_P56(f,a,b,...) f(a,b) ML_E(ML_P54(f,__VA_ARGS__))
#define ML_P58(f,a,b,...) f(a,b) ML_E(ML_P56(f,__VA_ARGS__))
#define ML_P60(f,a,b,...) f(a,b) ML_E(ML_P58(f,__VA_ARGS__))
#define ML_P62(f,a,b,...) f(a,b) ML_E(ML_P60(f,__VA_ARGS__))

/// MacroLoop_ForEachTriplet[3, 6, ..., ArgMax-ArgMax%3]
#define ML_T3(f,a,b,c,...) f(a,b,c)
#define ML_T6(f,a,b,c,...) f(a,b,c) ML_E(ML_T3(f,__VA_ARGS__))
#define ML_T9(f,a,b,c,...) f(a,b,c) ML_E(ML_T6(f,__VA_ARGS__))
#define ML_T12(f,a,b,c,...) f(a,b,c) ML_E(ML_T9(f,__VA_ARGS__))
#define ML_T15(f,a,b,c,...) f(a,b,c) ML_E(ML_T12(f,__VA_ARGS__))
#define ML_T18(f,a,b,c,...) f(a,b,c) ML_E(ML_T15(f,__VA_ARGS__))
#define ML_T21(f,a,b,c,...) f(a,b,c) ML_E(ML_T18(f,__VA_ARGS__))
#define ML_T24(f,a,b,c,...) f(a,b,c) ML_E(ML_T21(f,__VA_ARGS__))
#define ML_T27(f,a,b,c,...) f(a,b,c) ML_E(ML_T24(f,__VA_ARGS__))
#define ML_T30(f,a,b,c,...) f(a,b,c) ML_E(ML_T27(f,__VA_ARGS__))
#define ML_T33(f,a,b,c,...) f(a,b,c) ML_E(ML_T30(f,__VA_ARGS__))
#define ML_T36(f,a,b,c,...) f(a,b,c) ML_E(ML_T33(f,__VA_ARGS__))
#define ML_T39(f,a,b,c,...) f(a,b,c) ML_E(ML_T36(f,__VA_ARGS__))
#define ML_T42(f,a,b,c,...) f(a,b,c) ML_E(ML_T39(f,__VA_ARGS__))
#define ML_T45(f,a,b,c,...) f(a,b,c) ML_E(ML_T42(f,__VA_ARGS__))
#define ML_T48(f,a,b,c,...) f(a,b,c) ML_E(ML_T45(f,__VA_ARGS__))
#define ML_T51(f,a,b,c,...) f(a,b,c) ML_E(ML_T48(f,__VA_ARGS__))
#define ML_T54(f,a,b,c,...) f(a,b,c) ML_E(ML_T51(f,__VA_ARGS__))
#define ML_T57(f,a,b,c,...) f(a,b,c) ML_E(ML_T54(f,__VA_ARGS__))
#define ML_T60(f,a,b,c,...) f(a,b,c) ML_E(ML_T57(f,__VA_ARGS__))

/// MacroLoop_ArgumentCounts [ArgMax ... 0]
#define ML_G() 62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

/// MacroLoop_Select_Argument_At_Argument_Max [a0, ..., a(ArgMax-1), argAtArgMax]
#define ML_M(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,d0,d1,d2,d3,d4,d5,argAtArgMax,...) argAtArgMax

/// MacroLoop_Expand_Select_Argument_At_Argument_Max (necessary due to bugs in VS macro handling)
#define ML_S(...) ML_E(ML_M(__VA_ARGS__))

/// MacroLoop_ForEach_[(Singular), Pair, Triplet, Quartet]N
#define ML_N(N,f,...) ML_E(ML_C(ML_,N)(f,__VA_ARGS__))
#define ML_PN(N,f,...) ML_E(ML_C(ML_P,N)(f,__VA_ARGS__))
#define ML_TN(N,f,...) ML_E(ML_C(ML_T,N)(f,__VA_ARGS__))

/// Selects the count of varadic arguments
#define COUNT_ARGUMENTS(...) ML_S(__VA_ARGS__,ML_G())

/// Call "f" for each argument
#define FOR_EACH(f,...) ML_N(COUNT_ARGUMENTS(__VA_ARGS__),f,__VA_ARGS__)
#define FOR_EACH_PAIR(f,...) ML_PN(COUNT_ARGUMENTS(__VA_ARGS__),f,__VA_ARGS__)
#define FOR_EACH_TRIPLET(f,...) ML_TN(COUNT_ARGUMENTS(__VA_ARGS__),f,__VA_ARGS__)

};

/// ConditionalCall uses the template parameter "isCallable" partial template specialization to prevent compile-time generation of inappropriate calls (e.g. array access on non-array fields)
template <bool isCallable, typename Function, typename... Parameters> class ConditionalCall;

/// Calls function(parameters...)
template <typename Function, typename... Parameters> class ConditionalCall<true, Function, Parameters...> { public:
    /// Calls function(parameters...)
    constexpr static void call(Function function, Parameters... parameters) { function(parameters...); }
};

/// Does nothing
template <typename Function, typename... Parameters> class ConditionalCall<false, Function, Parameters...> { public:
    /// Does nothing
    constexpr static void call(Function function, Parameters... parameters) {}
};

namespace Reflect
{
#define EXPAND(x) x
/// B "basic-type": must be used for any type which already has an acceptable representation when streamed
#define B(type, name) type, name, false
/// R "reflected-type": must be used for any object which in turn relies on reflection to be streamed
#define R(type, name) type, name, true

#define GET_FIELD_NAME(ignored1, fieldName, ignored2) fieldName,
#define GET_FIELD_NAME_STRING(ignored1, fieldName, ignored2) #fieldName,
#define GET_TYPE_EXTENT(fieldType, ignored1, ignored2) std::extent<fieldType>::value,
#define IS_ARRAY(fieldType, ignored1, ignored2) std::is_array<fieldType>::value,
#define IS_FIELD_TYPE_REFLECTED(ignored1, ignored2, isReflected) isReflected,
#define USE_FIELD_AT(fieldType, fieldName, isReflected) case IndexOf::fieldName: function(object.fieldName); break;
#define USE_PRIMITIVE_FIELD(fieldType, fieldName, isReflected) \
    case IndexOf::fieldName: ConditionalCall<!isReflected && !std::is_array< ## fieldType ## >::value, Function, fieldType>::call(function, object.fieldName); break;
#define USE_PRIMITIVE_ARRAY_FIELD(fieldType, fieldName, isReflected) \
    case IndexOf::fieldName: ConditionalCall<!isReflected && std::is_array< ## fieldType ## >::value, Function, fieldType>::call(function, object.fieldName); break;
#define USE_OBJECT_FIELD(fieldType, fieldName, isReflected) \
    case IndexOf::fieldName: ConditionalCall<isReflected && !std::is_array< ## fieldType ## >::value, Function, fieldType>::call(function, object.fieldName); break;
#define USE_OBJECT_ARRAY_FIELD(fieldType, fieldName, isReflected) \
    case IndexOf::fieldName: ConditionalCall<isReflected && std::is_array< ## fieldType ## >::value, Function, fieldType>::call(function, object.fieldName); break;

/// One or more sets of (type, name, isReflected) must follow objectType, usually this is done by placing a couple B(...) and R(...)
#define REFLECT(objectType, ...) \
class Class { public: \
    using ObjectType = objectType; \
    static constexpr size_t totalFields = COUNT_ARGUMENTS(__VA_ARGS__)/3; \
    enum_t(IndexOf, size_t, { FOR_EACH_TRIPLET(GET_FIELD_NAME, __VA_ARGS__) }); \
    static constexpr const char* fieldNames[totalFields+1] = { FOR_EACH_TRIPLET(GET_FIELD_NAME_STRING, __VA_ARGS__) "" }; \
    static constexpr size_t arraySize[totalFields+1] = { FOR_EACH_TRIPLET(GET_TYPE_EXTENT, __VA_ARGS__) 0 }; \
    static constexpr bool isArray[totalFields+1] = { FOR_EACH_TRIPLET(IS_ARRAY, __VA_ARGS__) false }; \
    static constexpr bool isFieldTypeReflected[totalFields+1] = { FOR_EACH_TRIPLET(IS_FIELD_TYPE_REFLECTED, __VA_ARGS__) false }; \
    static constexpr bool isPrimitive(size_t fieldIndex) { return !isArray[fieldIndex] && !isFieldTypeReflected[fieldIndex]; }; \
    static constexpr bool isPrimitiveArray(size_t fieldIndex) { return isArray[fieldIndex] && !isFieldTypeReflected[fieldIndex]; }; \
    static constexpr bool isReflectedObject(size_t fieldIndex) { return !isArray[fieldIndex] && isFieldTypeReflected[fieldIndex]; }; \
    static constexpr bool isReflectedObjectArray(size_t fieldIndex) { return isArray[fieldIndex] && isFieldTypeReflected[fieldIndex]; }; \
    template <typename Function> static void GetField(ObjectType & object, size_t fieldIndex, Function function) { switch ( fieldIndex ) { FOR_EACH_TRIPLET(USE_FIELD_AT, __VA_ARGS__) } } \
    template <typename Function> static void GetPrimitiveField(ObjectType & object, size_t fieldIndex, Function function) { switch ( fieldIndex ) { FOR_EACH_TRIPLET(USE_PRIMITIVE_FIELD, __VA_ARGS__) } } \
    template <typename Function> static void GetPrimitiveArrayField(ObjectType & object, size_t fieldIndex, Function function) { switch ( fieldIndex) { FOR_EACH_TRIPLET(USE_PRIMITIVE_ARRAY_FIELD, __VA_ARGS__) } } \
    template <typename Function> static void GetObjectField(ObjectType & object, size_t fieldIndex, Function function) { switch ( fieldIndex ) { FOR_EACH_TRIPLET(USE_OBJECT_FIELD, __VA_ARGS__) } } \
    template <typename Function> static void GetObjectArrayField(ObjectType & object, size_t fieldIndex, Function function) { switch ( fieldIndex ) { FOR_EACH_TRIPLET(USE_OBJECT_ARRAY_FIELD, __VA_ARGS__) } } \
};

}

#endif