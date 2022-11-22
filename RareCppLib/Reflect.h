#ifndef REFLECT_H
#define REFLECT_H
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
#include <memory>
#include <string_view>
#include <string>
#include <tuple>
#include <exception>

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
#define enum_t(name, type, ...) struct name ## _ { enum type ## _ : type __VA_ARGS__; }; using name = typename name ## _::type ## _;
#pragma warning(disable: 26812) // In the context of using enum_t, enum class is definitely not preferred, disable the warning in visual studios
#endif

/// Contains everything neccessary to loop over varadic macro arguments
namespace MacroLoops
{
/// ArgMax: 125 (derived from the C spec limiting macros to 126 arguments and the COUNT_ARGUMENTS helper macro "ML_M" requiring ArgMax+1 arguments)

/// MacroLoop_Expand
#define ML_E(x) x

/// MacroLoop_Concatenate
#define ML_C(x,y) x##y

/// MacroLoop_ForEach[1, ..., ArgMax]
#define ML_0(f,...)
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
#define ML_63(f,a,...) f(a) ML_E(ML_62(f,__VA_ARGS__))
#define ML_64(f,a,...) f(a) ML_E(ML_63(f,__VA_ARGS__))
#define ML_65(f,a,...) f(a) ML_E(ML_64(f,__VA_ARGS__))
#define ML_66(f,a,...) f(a) ML_E(ML_65(f,__VA_ARGS__))
#define ML_67(f,a,...) f(a) ML_E(ML_66(f,__VA_ARGS__))
#define ML_68(f,a,...) f(a) ML_E(ML_67(f,__VA_ARGS__))
#define ML_69(f,a,...) f(a) ML_E(ML_68(f,__VA_ARGS__))
#define ML_70(f,a,...) f(a) ML_E(ML_69(f,__VA_ARGS__))
#define ML_71(f,a,...) f(a) ML_E(ML_70(f,__VA_ARGS__))
#define ML_72(f,a,...) f(a) ML_E(ML_71(f,__VA_ARGS__))
#define ML_73(f,a,...) f(a) ML_E(ML_72(f,__VA_ARGS__))
#define ML_74(f,a,...) f(a) ML_E(ML_73(f,__VA_ARGS__))
#define ML_75(f,a,...) f(a) ML_E(ML_74(f,__VA_ARGS__))
#define ML_76(f,a,...) f(a) ML_E(ML_75(f,__VA_ARGS__))
#define ML_77(f,a,...) f(a) ML_E(ML_76(f,__VA_ARGS__))
#define ML_78(f,a,...) f(a) ML_E(ML_77(f,__VA_ARGS__))
#define ML_79(f,a,...) f(a) ML_E(ML_78(f,__VA_ARGS__))
#define ML_80(f,a,...) f(a) ML_E(ML_79(f,__VA_ARGS__))
#define ML_81(f,a,...) f(a) ML_E(ML_80(f,__VA_ARGS__))
#define ML_82(f,a,...) f(a) ML_E(ML_81(f,__VA_ARGS__))
#define ML_83(f,a,...) f(a) ML_E(ML_82(f,__VA_ARGS__))
#define ML_84(f,a,...) f(a) ML_E(ML_83(f,__VA_ARGS__))
#define ML_85(f,a,...) f(a) ML_E(ML_84(f,__VA_ARGS__))
#define ML_86(f,a,...) f(a) ML_E(ML_85(f,__VA_ARGS__))
#define ML_87(f,a,...) f(a) ML_E(ML_86(f,__VA_ARGS__))
#define ML_88(f,a,...) f(a) ML_E(ML_87(f,__VA_ARGS__))
#define ML_89(f,a,...) f(a) ML_E(ML_88(f,__VA_ARGS__))
#define ML_90(f,a,...) f(a) ML_E(ML_89(f,__VA_ARGS__))
#define ML_91(f,a,...) f(a) ML_E(ML_90(f,__VA_ARGS__))
#define ML_92(f,a,...) f(a) ML_E(ML_91(f,__VA_ARGS__))
#define ML_93(f,a,...) f(a) ML_E(ML_92(f,__VA_ARGS__))
#define ML_94(f,a,...) f(a) ML_E(ML_93(f,__VA_ARGS__))
#define ML_95(f,a,...) f(a) ML_E(ML_94(f,__VA_ARGS__))
#define ML_96(f,a,...) f(a) ML_E(ML_95(f,__VA_ARGS__))
#define ML_97(f,a,...) f(a) ML_E(ML_96(f,__VA_ARGS__))
#define ML_98(f,a,...) f(a) ML_E(ML_97(f,__VA_ARGS__))
#define ML_99(f,a,...) f(a) ML_E(ML_98(f,__VA_ARGS__))
#define ML_100(f,a,...) f(a) ML_E(ML_99(f,__VA_ARGS__))
#define ML_101(f,a,...) f(a) ML_E(ML_100(f,__VA_ARGS__))
#define ML_102(f,a,...) f(a) ML_E(ML_101(f,__VA_ARGS__))
#define ML_103(f,a,...) f(a) ML_E(ML_102(f,__VA_ARGS__))
#define ML_104(f,a,...) f(a) ML_E(ML_103(f,__VA_ARGS__))
#define ML_105(f,a,...) f(a) ML_E(ML_104(f,__VA_ARGS__))
#define ML_106(f,a,...) f(a) ML_E(ML_105(f,__VA_ARGS__))
#define ML_107(f,a,...) f(a) ML_E(ML_106(f,__VA_ARGS__))
#define ML_108(f,a,...) f(a) ML_E(ML_107(f,__VA_ARGS__))
#define ML_109(f,a,...) f(a) ML_E(ML_108(f,__VA_ARGS__))
#define ML_110(f,a,...) f(a) ML_E(ML_109(f,__VA_ARGS__))
#define ML_111(f,a,...) f(a) ML_E(ML_110(f,__VA_ARGS__))
#define ML_112(f,a,...) f(a) ML_E(ML_111(f,__VA_ARGS__))
#define ML_113(f,a,...) f(a) ML_E(ML_112(f,__VA_ARGS__))
#define ML_114(f,a,...) f(a) ML_E(ML_113(f,__VA_ARGS__))
#define ML_115(f,a,...) f(a) ML_E(ML_114(f,__VA_ARGS__))
#define ML_116(f,a,...) f(a) ML_E(ML_115(f,__VA_ARGS__))
#define ML_117(f,a,...) f(a) ML_E(ML_116(f,__VA_ARGS__))
#define ML_118(f,a,...) f(a) ML_E(ML_117(f,__VA_ARGS__))
#define ML_119(f,a,...) f(a) ML_E(ML_118(f,__VA_ARGS__))
#define ML_120(f,a,...) f(a) ML_E(ML_119(f,__VA_ARGS__))
#define ML_121(f,a,...) f(a) ML_E(ML_120(f,__VA_ARGS__))
#define ML_122(f,a,...) f(a) ML_E(ML_121(f,__VA_ARGS__))
#define ML_123(f,a,...) f(a) ML_E(ML_122(f,__VA_ARGS__))
#define ML_124(f,a,...) f(a) ML_E(ML_123(f,__VA_ARGS__))
#define ML_125(f,a,...) f(a) ML_E(ML_124(f,__VA_ARGS__))

/// MacroLoop_ArgumentCounts [ArgMax ... 0]
#define ML_G() 125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,\
95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,\
47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

/// MacroLoop_TruesFalse [T[125], F]
#define ML_T() T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,\
T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,F

/// MacroLoop_Select_Argument_At_Argument_Max [a0, ..., a(ArgMax-1), argAtArgMax]
#define ML_M(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,\
d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,g0,g1,g2,g3,g4,g5,g6,g7,g8,g9,h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,\
i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,j0,j1,j2,j3,j4,j5,j6,j7,j8,argAtArgMax,...) argAtArgMax

/// MacroLoop_Expand_Select_Argument_At_Argument_Max (necessary due to bugs in VS macro handling)
#define ML_S(...) ML_E(ML_M(__VA_ARGS__))

/// MacroLoop_ForEach_N
#define ML_N(N,f,...) ML_E(ML_C(ML_,N)(f,__VA_ARGS__))

/// MacroLoop_ForEachTrue & MacroLoop_ForEachFalse
#define ML_TF_F(h,t,...)
#define ML_TF_T(h,t,a,...) h(a) ML_N(COUNT_ARGUMENTS(__VA_ARGS__),t,__VA_ARGS__)

// MacroLoop_ForEach_Boolean_Call_True_Or_False_macro
#define ML_B(b,h,t,...) ML_E(ML_C(ML_TF_,b)(h,t,__VA_ARGS__))

/// Selects the count of varadic arguments (can count 0 arguments, but cannot count parenthesized arguments)
#define COUNT_ARGUMENTS(...) ML_S(ML##__VA_ARGS__##_G(),ML_G())

/// Selects the count of varadic arguments (cannot count 0 arguments, but can count parenthesized arguments)
#define COUNT_POSITIVE_ARGUMENTS(...) ML_S(__VA_ARGS__,ML_G())

/// Call "f" for each argument
#define FOR_EACH(f,...) ML_N(COUNT_ARGUMENTS(__VA_ARGS__),f,__VA_ARGS__)

/// Call "h" for the first argument (if it exists) and "t" for the subsequent arguments (if they exist)
#define FOR_HEAD_TAIL(h,t,...) ML_B(ML_S(ML##__VA_ARGS__##_T(),ML_T()),h,t,__VA_ARGS__)

#define FOR_EACH_POSITIVE(f,...) ML_N(COUNT_POSITIVE_ARGUMENTS(__VA_ARGS__),f,__VA_ARGS__)

};

namespace ExtendedTypeSupport
{
    template <typename T> struct promote_char { using type = T; };
    template <> struct promote_char<char> { using type = int; };
    template <> struct promote_char<signed char> { using type = int; };
    template <> struct promote_char<unsigned char> { using type = int; };
    template <> struct promote_char<const char> { using type = const int; };
    template <> struct promote_char<const signed char> { using type = const int; };
    template <> struct promote_char<const unsigned char> { using type = const int; };

    template <typename T> struct pair_lhs { using type = void; };
    template <typename L, typename R> struct pair_lhs<std::pair<L, R>> { using type = L; };
    template <typename L, typename R> struct pair_lhs<const std::pair<L, R>> { using type = L; };

    template <typename T> struct pair_rhs { using type = void; };
    template <typename L, typename R> struct pair_rhs<std::pair<L, R>> { using type = R; };
    template <typename L, typename R> struct pair_rhs<const std::pair<L, R>> { using type = R; };

    template <typename T> struct element_type { using type = void; };
    template <typename T> struct element_type<const T> { using type = typename element_type<T>::type; };
    template <typename T, size_t N> struct element_type<T[N]> { using type = T; };
    template <typename T, size_t N> struct element_type<const T[N]> { using type = T; };
    template <typename T, size_t N> struct element_type<std::array<T, N>> { using type = T; };
    template <typename T, typename A> struct element_type<std::vector<T, A>> { using type = T; };
    template <typename T, typename A> struct element_type<std::deque<T, A>> { using type = T; };
    template <typename T, typename A> struct element_type<std::list<T, A>> { using type = T; };
    template <typename T, typename A> struct element_type<std::forward_list<T, A>> { using type = T; };
    template <typename T, typename C> struct element_type<std::stack<T, C>> { using type = T; };
    template <typename T, typename C> struct element_type<std::queue<T, C>> { using type = T; };
    template <typename T, typename C, typename P> struct element_type<std::priority_queue<T, C, P>> { using type = T; };
    template <typename K, typename C, typename A> struct element_type<std::set<K, C, A>> { using type = K; };
    template <typename K, typename C, typename A> struct element_type<std::multiset<K, C, A>> { using type = K; };
    template <typename K, typename H, typename E, typename A> struct element_type<std::unordered_set<K, H, E, A>> { using type = K; };
    template <typename K, typename H, typename E, typename A> struct element_type<std::unordered_multiset<K, H, E, A>> { using type = K; };
    template <typename K, typename T, typename C, typename A> struct element_type<std::map<K, T, C, A>> { using type = typename std::pair<K, T>; };
    template <typename K, typename T, typename C, typename A> struct element_type<std::multimap<K, T, C, A>> { using type = typename std::pair<K, T>; };
    template <typename K, typename T, typename H, typename E, typename A> struct element_type<std::unordered_map<K, T, H, E, A>>
    { using type = typename std::pair<K, T>; };
    template <typename K, typename T, typename H, typename E, typename A> struct element_type<std::unordered_multimap<K, T, H, E, A>>
    { using type = typename std::pair<K, T>; };

    template <typename T> struct is_pointable { static constexpr bool value = std::is_pointer<T>::value; };
    template <typename T> struct is_pointable<std::unique_ptr<T>> { static constexpr bool value = true; };
    template <typename T> struct is_pointable<std::shared_ptr<T>> { static constexpr bool value = true; };
    template <typename T> struct is_pointable<std::weak_ptr<T>> { static constexpr bool value = true; };
    template <typename T> struct is_pointable<const T> { static constexpr bool value = is_pointable<T>::value; };
    
    template <typename T> struct remove_pointer { using type = typename std::remove_pointer<T>::type; };
    template <typename T> struct remove_pointer<std::unique_ptr<T>> { using type = T; };
    template <typename T> struct remove_pointer<std::shared_ptr<T>> { using type = T; };
    template <typename T> struct remove_pointer<std::weak_ptr<T>> { using type = T; };
    template <typename T> struct remove_pointer<const T>
    { using type = std::conditional_t<is_pointable<T>::value, typename remove_pointer<T>::type, const T>; };

    template <typename T> struct static_array_size { static constexpr size_t value = 0; };
    template <typename T, size_t N> struct static_array_size<T[N]> { static constexpr size_t value = N; };
    template <typename T, size_t N> struct static_array_size<const T[N]> { static constexpr size_t value = N; };
    template <typename T, size_t N> struct static_array_size<std::array<T, N>> { static constexpr size_t value = N; };
    template <typename T, size_t N> struct static_array_size<const std::array<T, N>> { static constexpr size_t value = N; };

    template <typename T> struct is_static_array { static constexpr bool value = false; };
    template <typename T, size_t N> struct is_static_array<T[N]> { static constexpr bool value = true; };
    template <typename T, size_t N> struct is_static_array<const T[N]> { static constexpr bool value = true; };
    template <typename T, size_t N> struct is_static_array<std::array<T, N>> { static constexpr bool value = true; };
    template <typename T, size_t N> struct is_static_array<const std::array<T, N>> { static constexpr bool value = true; };

    template <typename T> struct is_iterable { static constexpr bool value = !std::is_same<void, typename element_type<T>::type>::value; };

    template <typename T> struct is_map { static constexpr bool value = false; };
    template <typename T> struct is_map<const T> { static constexpr bool value = is_map<T>::value; };
    template <typename K, typename T, typename C, typename A> struct is_map<std::map<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct is_map<std::multimap<K, T, C, A>>
    { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct is_map<std::unordered_map<K, T, H, E, A>>
    { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct is_map<std::unordered_multimap<K, T, H, E, A>>
    { static constexpr bool value = true; };

    template <typename T> struct is_stl_iterable { static constexpr bool value = false; };
    template <typename T> struct is_stl_iterable<const T> { static constexpr bool value = is_stl_iterable<T>::value; };
    template <typename T, size_t N> struct is_stl_iterable<std::array<T, N>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_stl_iterable<std::vector<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_stl_iterable<std::deque<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_stl_iterable<std::list<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_stl_iterable<std::forward_list<T, A>> { static constexpr bool value = true; };
    template <typename K, typename C, typename A> struct is_stl_iterable<std::set<K, C, A>> { static constexpr bool value = true; };
    template <typename K, typename C, typename A> struct is_stl_iterable<std::multiset<K, C, A>> { static constexpr bool value = true; };
    template <typename K, typename H, typename E, typename A> struct is_stl_iterable<std::unordered_set<K, H, E, A>>
    { static constexpr bool value = true; };
    template <typename K, typename H, typename E, typename A> struct is_stl_iterable<std::unordered_multiset<K, H, E, A>>
    { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct is_stl_iterable<std::map<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct is_stl_iterable<std::multimap<K, T, C, A>>
    { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct is_stl_iterable<std::unordered_map<K, T, H, E, A>>
    { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct is_stl_iterable<std::unordered_multimap<K, T, H, E, A>>
    { static constexpr bool value = true; };
    
    template <typename T> struct is_adaptor { static constexpr bool value = false; };
    template <typename T> struct is_adaptor<const T> { static constexpr bool value = is_adaptor<T>::value; };
    template <typename T, typename C> struct is_adaptor<std::stack<T, C>> { static constexpr bool value = true; };
    template <typename T, typename C> struct is_adaptor<std::queue<T, C>> { static constexpr bool value = true; };
    template <typename T, typename C, typename P> struct is_adaptor<std::priority_queue<T, C, P>> { static constexpr bool value = true; };
    
    template <typename T> struct is_forward_list { static constexpr bool value = false; };
    template <typename T, typename A> struct is_forward_list<std::forward_list<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_forward_list<const std::forward_list<T, A>> { static constexpr bool value = true; };

    template <typename T> struct is_pair { static constexpr bool value = false; };
    template <typename L, typename R> struct is_pair<std::pair<L, R>> { static constexpr bool value = true; };
    template <typename L, typename R> struct is_pair<const std::pair<L, R>> { static constexpr bool value = true; };

    template <typename T> struct is_tuple { static constexpr bool value = false; };
    template <typename ...Ts> struct is_tuple<std::tuple<Ts...>> { static constexpr bool value = true; };
    template <typename ...Ts> struct is_tuple<const std::tuple<Ts...>> { static constexpr bool value = true; };
    template <typename ...Ts> struct is_tuple<std::tuple<Ts...> &> { static constexpr bool value = true; };
    template <typename ...Ts> struct is_tuple<const std::tuple<Ts...> &> { static constexpr bool value = true; };

    template <typename T> struct is_bool { static constexpr bool value = std::is_same<bool, typename std::remove_const<T>::type>::value; };
    
    template <typename T> struct is_string { static constexpr bool value = std::is_same<std::string, typename std::remove_const<T>::type>::value; };

    template <typename T> struct has_push_back { static constexpr bool value = false; };
    template <typename T> struct has_push_back<const T> { static constexpr bool value = has_push_back<T>::value; };
    template <typename T, typename A> struct has_push_back<std::vector<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct has_push_back<std::deque<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct has_push_back<std::list<T, A>> { static constexpr bool value = true; };

    template <typename T> struct has_push { static constexpr bool value = false; };
    template <typename T> struct has_push<const T> { static constexpr bool value = has_push<T>::value; };
    template <typename T, typename C> struct has_push<std::stack<T, C>> { static constexpr bool value = true; };
    template <typename T, typename C> struct has_push<std::queue<T, C>> { static constexpr bool value = true; };
    template <typename T, typename C, typename P> struct has_push<std::priority_queue<T, C, P>> { static constexpr bool value = true; };

    template <typename T> struct has_insert { static constexpr bool value = false; };
    template <typename T> struct has_insert<const T> { static constexpr bool value = has_insert<T>::value; };
    template <typename K, typename C, typename A> struct has_insert<std::set<K, C, A>> { static constexpr bool value = true; };
    template <typename K, typename C, typename A> struct has_insert<std::multiset<K, C, A>> { static constexpr bool value = true; };
    template <typename K, typename H, typename E, typename A> struct has_insert<std::unordered_set<K, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename H, typename E, typename A> struct has_insert<std::unordered_multiset<K, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct has_insert<std::map<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct has_insert<std::multimap<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct has_insert<std::unordered_map<K, T, H, E, A>>
    { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct has_insert<std::unordered_multimap<K, T, H, E, A>>
    { static constexpr bool value = true; };

    template <typename T> struct has_clear { static constexpr bool value = false; };
    template <typename T> struct has_clear<const T> { static constexpr bool value = has_clear<T>::value; };
    template <typename T, typename A> struct has_clear<std::vector<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct has_clear<std::deque<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct has_clear<std::list<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct has_clear<std::forward_list<T, A>> { static constexpr bool value = true; };
    template <typename K, typename C, typename A> struct has_clear<std::set<K, C, A>> { static constexpr bool value = true; };
    template <typename K, typename C, typename A> struct has_clear<std::multiset<K, C, A>> { static constexpr bool value = true; };
    template <typename K, typename H, typename E, typename A> struct has_clear<std::unordered_set<K, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename H, typename E, typename A> struct has_clear<std::unordered_multiset<K, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct has_clear<std::map<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct has_clear<std::multimap<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct has_clear<std::unordered_map<K, T, H, E, A>>
    { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct has_clear<std::unordered_multimap<K, T, H, E, A>>
    { static constexpr bool value = true; };
    
    template <typename Iterable, typename Element>
    static constexpr void Append(Iterable & iterable, Element & element)
    {
        if constexpr ( !std::is_const<Iterable>::value )
        {
            if constexpr ( has_push_back<Iterable>::value )
                iterable.push_back(element);
            else if constexpr ( is_forward_list<Iterable>::value )
            {
                auto last = iterable.before_begin();
                for ( auto curr = last; curr != iterable.end(); last = curr++);
                iterable.insert_after(last, element);
            }
            else if constexpr ( has_push<Iterable>::value )
                iterable.push(element);
            else if constexpr ( has_insert<Iterable>::value )
                iterable.insert(element);
        }
    }

    template <typename Iterable>
    static constexpr bool IsEmpty(const Iterable & iterable)
    {
        if constexpr ( std::is_array<Iterable>::value )
            return std::extent<Iterable>::value == 0;
        else
            return iterable.empty();
    }

    template <typename Iterable>
    static constexpr void Clear(Iterable & iterable)
    {
        if constexpr ( !std::is_const<Iterable>::value )
        {
            if constexpr ( has_clear<Iterable>::value )
                iterable.clear();
            else if constexpr ( is_adaptor<Iterable>::value )
            {
                while ( !iterable.empty() )
                    iterable.pop();
            }
        }
    }

    inline namespace OpDetection {
        template <class AlwaysVoid, template<class...> class Op, class... Args> struct OpExists { static constexpr bool value = false; };
        template <template<class...> class Op, class... Args> struct OpExists<std::void_t<Op<Args...>>, Op, Args...> { static constexpr bool value = true; };

        namespace OpDetectImpl {
            template <typename L, typename R> using AssignmentOp = decltype(std::declval<L>() = std::declval<const R &>());
            template <typename L, typename R> using StaticCastAssignmentOp = decltype(
                std::declval<L>() = static_cast<std::remove_reference_t<L>>(std::declval<const R &>()));
            template <typename L, typename R> using MapToOp = decltype(std::declval<L>().map_to(std::declval<R &>()));
            template <typename L, typename R> using MapFromOp = decltype(std::declval<L>().map_from(std::declval<const R &>()));
        }

        template <typename L, typename R> static constexpr bool IsAssignable = OpExists<void, OpDetectImpl::AssignmentOp, L, R>::value;
        template <typename L, typename R> static constexpr bool IsStaticCastAssignable = OpExists<void, OpDetectImpl::StaticCastAssignmentOp, L, R>::value;
        template <typename L, typename R> static constexpr bool HasMapTo = OpExists<void, OpDetectImpl::MapToOp, L, R>::value;
        template <typename L, typename R> static constexpr bool HasMapFrom = OpExists<void, OpDetectImpl::MapFromOp, L, R>::value;
    };
    
    template <typename L, typename R>
    struct TypePair {
        using Left = L;
        using Right = R;
    };

    template <typename T, template<typename ...> class Of> struct is_specialization : std::false_type {};
    template <template<typename ...> class Of, typename ...Ts> struct is_specialization<Of<Ts...>, Of> : std::true_type {};
    template <typename T, template<typename ...> class Of> inline constexpr bool is_specialization_v = is_specialization<T, Of>::value;
    template <typename T, template<typename ...> class Of> struct is_specialization_t : is_specialization<T, Of> {};

    template <typename ...Ts> struct type_list {
        template <typename T> struct has : std::bool_constant<(std::is_same_v<Ts, T> || ...)> {};
        template <typename T> static inline constexpr bool has_v = has<T>::value;
        template <template <typename ...> class Of> struct has_specialization : std::bool_constant<(is_specialization_v<Ts, Of> || ...)> {};
        template <template <typename ...> class Of> static inline constexpr bool has_specialization_v = has_specialization<Of>::value;
        template <template <typename ...> class Of> class get_specialization {
            template <typename ...Us> struct get { using type = void; };
            template <typename U, typename ...Us> struct get<U, Us...> {
                using type = std::conditional_t<is_specialization_v<U, Of>, U, typename get<Us...>::type>;
            };
        public:
            using type = typename get<Ts...>::type;
        };
        template <template <typename ...> class Of> using get_specialization_t = typename get_specialization<Of>::type;
    };
    template <typename ...Ts> struct type_list<const std::tuple<Ts...>> : type_list<Ts...> {};
    template <typename ...Ts> struct type_list<std::tuple<Ts...>> : type_list<Ts...> {};

    struct Unspecialized {}; // Primary templates (and not specializations), should inherit from Unspecialized
    template <typename T> struct is_specialized { static constexpr bool value = !std::is_base_of<Unspecialized, T>::value; };

    template <typename T, typename TypeIfVoid> struct if_void { using type = T; };
    template <typename TypeIfVoid> struct if_void<void, TypeIfVoid> { using type = TypeIfVoid; };
    template <typename T, typename TypeIfVoid> using if_void_t = typename if_void<T, TypeIfVoid>::type;

    template <typename F, size_t ...Is>
    constexpr void ForIntegralConstant(size_t i, std::index_sequence<Is...>, F && f) {
        (void)((i == Is && (std::forward<F>(f)(std::integral_constant<size_t, Is>{}), true)) || ...);
    }

    template <size_t Max, typename F>
    constexpr void ForIntegralConstant(size_t i, F && f) {
        ForIntegralConstant(i, std::make_index_sequence<Max>{}, std::forward<F>(f));
    }

    template <typename F, size_t ...Is>
    constexpr void ForIntegralConstants(std::index_sequence<Is...>, F && f) {
        (void)((std::forward<F>(f)(std::integral_constant<size_t, Is>{}), true) && ...);
    }

    // For integral constants 0 to Max-1
    template <size_t Max, typename F>
    constexpr void ForIntegralConstants(F && f) {
        if constexpr ( Max > 0 )
            ForIntegralConstants(std::make_index_sequence<Max>{}, std::forward<F>(f));
    }

    template <typename T>
    constexpr bool HasTypeRecursion() {
        return false;
    }

    template <typename T, typename CurrentType, typename... NextTypes>
    constexpr bool HasTypeRecursion() {
        if constexpr ( std::is_same<T, CurrentType>::value )
            return true;
        else
            return HasTypeRecursion<T, NextTypes...>();
    }
    
    template <typename T, typename ... Ts> struct has_type { static constexpr bool value = HasTypeRecursion<T, Ts...>(); };
    template <typename T, typename ... Ts> struct has_type<T, std::tuple<Ts...>> { static constexpr bool value = HasTypeRecursion<T, Ts...>(); };
    template <typename T, typename ... Ts> struct has_type<T, const std::tuple<Ts...>> { static constexpr bool value = HasTypeRecursion<T, Ts...>(); };
    
    template <typename TypeToGet>
    class get
    {
        template <typename ...Ts> struct get_impl
        {
            template <size_t Index>
            static constexpr const TypeToGet & GetElementRecursion(const std::tuple<Ts...> & elements)
            {
                throw std::exception(); // Tuple must contain the type provided
            }

            template <size_t Index, typename CurrentType, typename... NextTypes>
            static constexpr const TypeToGet & GetElementRecursion(const std::tuple<Ts...> & elements)
            {
                if constexpr ( std::is_same<TypeToGet, CurrentType>::value )
                    return std::get<Index>(elements);
                else
                    return GetElementRecursion<Index+1, NextTypes...>(elements);
            }

            static constexpr const TypeToGet & GetElement(const std::tuple<Ts...> & elements)
            {
                return GetElementRecursion<0, Ts...>(elements);
            }
        };

    public:
        template <typename ...Ts> static constexpr const TypeToGet & from(const std::tuple<Ts...> & elements) {
            return get_impl<Ts...>::GetElement(elements);
        }
    };

    template <typename TypeToGet>
    class for_each
    {
        template <typename ...Ts> struct for_each_impl
        {
            template <size_t Index, typename Function>
            static constexpr void ForEachElementRecursion(const std::tuple<Ts...> & elements, Function function) {}

            template <size_t Index, typename Function, typename CurrentType, typename... NextTypes>
            static constexpr void ForEachElementRecursion(const std::tuple<Ts...> & elements, Function function)
            {
                if constexpr ( std::is_same<TypeToGet, CurrentType>::value )
                    function(std::get<Index>(elements));

                ForEachElementRecursion<Index+1, Function, NextTypes...>(elements, function);
            }

            template <typename Function>
            static constexpr void ForEachElement(const std::tuple<Ts...> & elements, Function function)
            {
                return ForEachElementRecursion<0, Function, Ts...>(elements, function);
            }
        };

    public:
        template <typename Function, typename ...Ts>
        static constexpr void in(const std::tuple<Ts...> & elements, Function function)
        {
            for_each_impl<Ts...>::ForEachElement(elements, function);
        }
    };
    
    template <template <typename ...> class Of>
    class for_each_specialization
    {
        template <typename ...Ts> struct for_each_impl
        {
            template <size_t Index, typename Function>
            static constexpr void ForEachElementRecursion(const std::tuple<Ts...> & elements, Function function) {}

            template <size_t Index, typename Function, typename CurrentType, typename... NextTypes>
            static constexpr void ForEachElementRecursion(const std::tuple<Ts...> & elements, Function function)
            {
                if constexpr ( is_specialization_v<CurrentType, Of> )
                    function(std::get<Index>(elements));

                ForEachElementRecursion<Index+1, Function, NextTypes...>(elements, function);
            }

            template <typename Function>
            static constexpr void ForEachElement(const std::tuple<Ts...> & elements, Function function)
            {
                return ForEachElementRecursion<0, Function, Ts...>(elements, function);
            }
        };

    public:
        template <typename Function, typename ...Ts>
        static constexpr void in(const std::tuple<Ts...> & elements, Function function)
        {
            for_each_impl<Ts...>::ForEachElement(elements, function);
        }
    };

    template <typename ...Ts>
    struct ForEachIn
    {
        template <size_t Index, typename Function>
        static constexpr void ForEachElementRecursion(const std::tuple<Ts...> & elements, Function function) {}

        template <size_t Index, typename Function, typename CurrentType, typename... NextTypes>
        static constexpr void ForEachElementRecursion(const std::tuple<Ts...> & elements, Function function)
        {
            function(std::get<Index>(elements));
            ForEachElementRecursion<Index+1, Function, NextTypes...>(elements, function);
        }

        template <typename Function>
        static constexpr void ForEachElement(const std::tuple<Ts...> & elements, Function function)
        {
            ForEachElementRecursion<0, Function, Ts...>(elements, function);
        }
    };

    template <typename Function, typename ...Ts>
    static constexpr void for_each_in(const std::tuple<Ts...> & elements, Function function)
    {
        ForEachIn<Ts...>::ForEachElement(elements, function);
    }

    template <typename T>
    constexpr auto getTypeView()
    {
        std::string_view view;
#ifdef _MSC_VER
#ifndef __clang__
        view = __FUNCSIG__;
        view.remove_prefix(view.find_first_of("<")+1);
        view.remove_suffix(view.size()-view.find_last_of(">"));
#else
        view = __PRETTY_FUNCTION__;
        view.remove_prefix(view.find_first_of("=")+1);
        view.remove_prefix(view.find_first_not_of(" "));
        view.remove_suffix(view.size()-view.find_last_of("]"));
#endif
#else
#ifdef __clang__
        view = __PRETTY_FUNCTION__;
        view.remove_prefix(view.find_first_of("=")+1);
        view.remove_prefix(view.find_first_not_of(" "));
        view.remove_suffix(view.size()-view.find_last_of("]"));
#else
#ifdef __GNUC__
        view = __PRETTY_FUNCTION__;
        view.remove_prefix(view.find_first_of("=")+1);
        view.remove_prefix(view.find_first_not_of(" "));
        view.remove_suffix(view.size()-view.find_last_of("]"));
#else
        view = "unknown";
#endif
#endif
#endif
        return view;
    }

    template <typename T>
    struct TypeName
    {
        static constexpr size_t length = getTypeView<T>().size();

        constexpr TypeName() : value() {
            auto view = getTypeView<T>();
            for ( size_t i=0; i<length; i++ )
                value[i] = view[i];

            value[length] = '\0';
        }
        char value[length+1];

        constexpr operator const char*() const { return &value[0]; };
    };
    
    template <typename T>
    std::string TypeToStr() {
        return std::string(TypeName<T>().value);
    }

    template <class Adaptor>
    const typename Adaptor::container_type & get_underlying_container(const Adaptor & adaptor) {
        struct AdaptorSubClass : Adaptor {
            static const typename Adaptor::container_type & get(const Adaptor & adaptor) {
                return adaptor.*(&AdaptorSubClass::c);
            }
        };
        return AdaptorSubClass::get(adaptor);
    }

    template <typename T, typename ... Ts>
    struct index_of
    {
        static constexpr size_t value = [](){
            size_t index = sizeof...(Ts);
            ExtendedTypeSupport::ForIntegralConstants<sizeof...(Ts)>([&](auto I) {
                if constexpr ( std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>,
                        std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<decltype(I)::value, std::tuple<Ts...>>>>> )
                {
                    if ( index == sizeof...(Ts) )
                        index = decltype(I)::value;
                }
            });
            return index;
        }();
    };

    template <typename T, typename ... Ts> struct index_of<T, std::tuple<Ts...>> : index_of<T, Ts...> {};
    template <typename T, typename ... Ts> struct index_of<T, const std::tuple<Ts...>> : index_of<T, Ts...> {};
    template <typename T, typename ... Ts> struct index_of<T, std::tuple<Ts...> &> : index_of<T, Ts...> {};
    template <typename T, typename ... Ts> struct index_of<T, const std::tuple<Ts...> &> : index_of<T, Ts...> {};

	static constexpr inline size_t fnv1a_hash(const std::string_view & str) noexcept { // Returns a fnv1a hash of the given string
		constexpr size_t seed = sizeof(size_t) >= 8 ? size_t(14695981039346656037ULL) : size_t(2166136261U);
		constexpr size_t factor = sizeof(size_t) >= 8 ? size_t(1099511628211ULL) : size_t(16777619U);
		size_t hash = seed;
		for ( auto c : str ) {
			hash ^= static_cast<size_t>(c);
			hash *= factor;
		}
		return hash;
	}
    
    template <const char* ... s>
    struct StringIndexMap { // Constexpr map from a set of unique strings to the indexes at which they're passed in

        static constexpr size_t total = sizeof...(s); // The total number of strings in the map

    private:
        static constexpr size_t totalBuckets = [](){
            constexpr size_t extraBits = 0; // Increasing this may trade memory for performance
            if constexpr ( total <= 8 ) return size_t(8) << extraBits;
            else {
                size_t n = 1;
                for ( size_t remainder = total-1; remainder > 0; n <<= 1 ) remainder >>= 1;
                return n << extraBits;
            }
        }();
        static constexpr size_t mask = totalBuckets-1;
    
        struct Bucket { size_t begin = 0; size_t end = 0; }; // The hash value and the [begin, end) indexes of the matching StringIndexes
        struct StringIndex { std::string_view str; size_t index; }; // The string and the index with which it was passed in to the map
        struct HashMap {
            Bucket stringBucket[totalBuckets] {};
            StringIndex stringIndex[total];

            static constexpr void swap(size_t & l, size_t & r, StringIndex & l2, StringIndex & r2) noexcept { // sort's swap helper
                size_t t = std::move(l); l = std::move(r); r = std::move(t); StringIndex t2 = std::move(l2); l2 = std::move(r2); r2 = std::move(t2);
            }

            constexpr inline void sort(size_t (&hashes)[total], int l = 0, int r = int(total)-1) noexcept { // sort hashes, keep indexes parallel
                if ( l < r ) {
                    int i = l+(r-l)/2; // Take median index as the pivot, swap pivot with last element
                    swap(hashes[i], hashes[r], stringIndex[i], stringIndex[r]);
                    size_t pivotValue = hashes[r];
                    i = l;
                    for ( int j=l; j<r; ++j ) {
                        if ( hashes[j] <= pivotValue ) { // Swap elements less than pivot to the earliest indexes
                            swap(hashes[i], hashes[j], stringIndex[i], stringIndex[j]);
                            ++i;
                        }
                    }
                    swap(hashes[size_t(i)], hashes[r], stringIndex[size_t(i)], stringIndex[r]);
                    sort(hashes, l, i-1); // Sort left sub-array (elements <= pivotValue)
                    sort(hashes, i+1, r); // Sort right sub-array (elements > pivotValue)
                }
            }
        };

        template <size_t ... I> static constexpr auto buildHashMap(std::index_sequence<I...>) noexcept {
            HashMap hashMap {{}, {{std::string_view(s), I}...}};
            size_t dupedHashes[total] { (fnv1a_hash(std::string_view(s)) & mask)... };
            
            hashMap.sort(dupedHashes); // Sort stringBucket, mirroring any swaps performed to stringBucket in stringIndexes and strings)
            size_t last = !dupedHashes[0]; // Set last to anything other than the value of the first hash
            for ( size_t i=0; i<total; ++i ) { // Build array without duplicate hashes, and a parallel array with the range of strings per each unique hash
                size_t hash = dupedHashes[i];
                if ( last == hash ) { ++(hashMap.stringBucket[hash].end); } // Duplicate hash, add this string to the hash match range
                else { hashMap.stringBucket[hash] = Bucket{i, i+1}; } // New hash
                last = hash;
            }
            return hashMap;
        }

        static constexpr HashMap hashMap = buildHashMap(std::make_index_sequence<total>());
        static constexpr size_t dupedHashes[total] { (fnv1a_hash(std::string_view(s)) & mask)... };

    public:
        static constexpr size_t indexOf(const std::string_view & str) noexcept {
            const Bucket & bucket = hashMap.stringBucket[fnv1a_hash(str) & mask];
            const StringIndex & firstMatch = hashMap.stringIndex[bucket.begin];
            if ( str == firstMatch.str ) return firstMatch.index;
            for ( size_t i=bucket.begin+1; i<bucket.end; ++i ) {
                if ( str == hashMap.stringIndex[i].str )
                    return hashMap.stringIndex[i].index;
            }
            return std::numeric_limits<size_t>::max();
        }
    };
    template <> struct StringIndexMap<> { // Maps strings to the indexes at which they're passed in, constexpr friendly
        static constexpr size_t total = 0; // The total number of strings in the map (0)
        static constexpr size_t indexOf(const std::string_view & str) noexcept { return std::numeric_limits<size_t>::max(); }
    };
}

/// Contains support for working with reflected fields, the definition for the REFLECT macro and non-generic supporting macros
namespace Reflection
{
    #define NOTE(field, ...) static constexpr auto field##_note { std::tuple { __VA_ARGS__ } };
    using NoAnnotation = std::tuple<>;
    static constexpr NoAnnotation NoNote {};

    struct Unproxied {};

    template <typename T>
    struct Proxy : public Unproxied {};

    template <typename T>
    struct is_proxied : std::bool_constant<!std::is_base_of<Unproxied, Proxy<T>>::value> {};
    
    template <typename Type> struct unproxy { using T = Type; };
    template <typename Type> struct unproxy<Proxy<Type>> { using T = Type; };
    template <typename Type> using unproxy_t = typename unproxy<Type>::T;

    template <typename T, typename = decltype(T::Class::Total)> static constexpr std::true_type typeHasReflection(int);
    template <typename T> static constexpr std::false_type typeHasReflection(unsigned int);
    
    template <typename T> struct is_reflected { static constexpr bool value = is_proxied<T>::value || decltype(typeHasReflection<T>(0))::value; };
    template <typename T> struct is_reflected<const T> { static constexpr bool value = is_reflected<T>::value; };
    template <typename T> inline constexpr bool is_reflected_v = is_reflected<T>::value;

    template <typename T> struct reflected_type { using type = typename std::conditional_t<is_proxied<T>::value, Proxy<T>, T>; };
    template <typename T> using reflected_type_t = typename reflected_type<T>::type;

    template <typename Type> struct clazz { using type = typename reflected_type_t<Type>::Class; };
    template <typename Type> using class_t = typename clazz<Type>::type;

    template <typename T, typename Enable = void> struct class_notes { using type = Reflection::NoAnnotation; };
    template <typename T> struct class_notes<T, std::enable_if_t<is_reflected_v<T>>> { using type = typename class_t<T>::Annotations; };
    template <typename T> using class_notes_t = typename class_notes<T>::type;

    inline namespace Inheritance
    {
        template <typename T, typename ...Ts>
        struct NotedSuper {
            std::tuple<Ts...> annotations;
            using Annotations = decltype(annotations);
        };

        template <typename T>
        struct SuperClass {
            constexpr static NoAnnotation annotations{};
            using Annotations = NoAnnotation;

            template <typename ...Ts>
            constexpr NotedSuper<T, Ts...> operator() (const Ts & ... args) const { return NotedSuper<T, Ts...>{std::tuple<Ts...>(args...)}; } 
        };

        template <typename T>
        static constexpr SuperClass<T> Super{};
        
        template <typename T> struct is_super { static constexpr bool value = false; };
        template <typename T> struct is_super<const T> { static constexpr bool value = is_super<T>::value; };
        template <typename T> struct is_super<SuperClass<T>> { static constexpr bool value = true; };
        template <typename T, typename ...Ts> struct is_super<NotedSuper<T, Ts...>> { static constexpr bool value = true; };
        
        template <typename T> struct super_type { using type = void; };
        template <typename T> struct super_type<const T> { using type = typename super_type<T>::type; };
        template <typename T> struct super_type<SuperClass<T>> { using type = T; };
        template <typename T, typename ...Ts> struct super_type<NotedSuper<T, Ts...>> { using type = T; };

        template <typename ...Ts> struct count_supers {
            static constexpr size_t value = size_t(0 + (ExtendedTypeSupport::is_specialization_v<std::remove_const_t<Ts>, SuperClass> + ...) +
                (ExtendedTypeSupport::is_specialization_v<std::remove_const_t<Ts>, NotedSuper> + ...)); };
        template<> struct count_supers<> { static constexpr size_t value = 0; };

        template <size_t SuperIndex, typename ...Ts>
        struct super_note_index {
            static constexpr size_t value = [](){
                constexpr size_t TotalNotes = sizeof...(Ts);
                size_t superNoteIndex = TotalNotes;
                size_t superCount = 0;
                ExtendedTypeSupport::ForIntegralConstants<TotalNotes>([&](auto NoteIndex) {
                    if constexpr ( is_super<std::tuple_element_t<decltype(NoteIndex)::value, std::tuple<Ts...>>>::value )
                    {
                        if ( superCount == SuperIndex )
                            superNoteIndex = decltype(NoteIndex)::value;

                        ++superCount;
                    }
                });
                return superNoteIndex;
            }();
        };
        template <size_t SuperIndex, typename ...Ts> struct super_note_index<SuperIndex, std::tuple<Ts...>> : super_note_index<SuperIndex, Ts...> {};
        template <size_t SuperIndex, typename ...Ts> struct super_note_index<SuperIndex, const std::tuple<Ts...>> : super_note_index<SuperIndex, Ts...> {};

        template <size_t NoteIndex, typename ...Ts>
        struct note_super_index {
            static constexpr size_t value = [](){
                constexpr size_t TotalNotes = sizeof...(Ts);
                size_t noteSuperIndex = TotalNotes;
                size_t superCount = 0;
                ExtendedTypeSupport::ForIntegralConstants<TotalNotes>([&](auto I) {
                    if constexpr ( is_super<std::tuple_element_t<decltype(I)::value, std::tuple<Ts...>>>::value )
                    {
                        if constexpr ( NoteIndex == decltype(I)::value )
                            noteSuperIndex = superCount;

                        ++superCount;
                    }
                });
                return noteSuperIndex;
            }();
        };
        template <size_t NoteIndex, typename ...Ts> struct note_super_index<NoteIndex, std::tuple<Ts...>> : note_super_index<NoteIndex, Ts...> {};
        template <size_t NoteIndex, typename ...Ts> struct note_super_index<NoteIndex, const std::tuple<Ts...>> : note_super_index<NoteIndex, Ts...> {};

        template <size_t SuperIndex, typename SubClass>
        struct SuperInfo {

            static constexpr size_t Index = SuperIndex;
            static constexpr size_t SuperNoteIndex = super_note_index<Index, typename class_t<SubClass>::Annotations>::value;
            using SuperNote = std::tuple_element_t<SuperNoteIndex, typename class_t<SubClass>::Annotations>;
            static constexpr auto & superNote = std::get<SuperNoteIndex>(class_t<SubClass>::annotations);

            using Type = typename super_type<SuperNote>::type;
            using Annotations = typename SuperNote::Annotations;
            static constexpr auto & annotations = superNote.annotations;
            
            template <typename Annotation>
            static constexpr bool HasAnnotation = ExtendedTypeSupport::has_type<Annotation, Annotations>::value;

            template <template <typename ...> class Of>
            static constexpr bool HasSpecializedAnnotation = ExtendedTypeSupport::type_list<Annotations>::template has_specialization_v<Of>;

            template <typename Annotation>
            static constexpr Annotation getAnnotation() { return ExtendedTypeSupport::get<Annotation>::from(annotations); }

            template <template <typename ...> class Of>
            static constexpr auto getAnnotation() {
                return ExtendedTypeSupport::get<typename ExtendedTypeSupport::type_list<Annotations>::template get_specialization_t<Of>>::from(annotations);
            }

            template <typename Function>
            static constexpr void forEachAnnotation(Function function) { return ExtendedTypeSupport::for_each_in(annotations, function); }
            
            // function(annotation) [filtered to type 'Annotation']
            template <typename Annotation, typename Function>
            static constexpr void forEach(Function function) { return ExtendedTypeSupport::for_each<Annotation>::in(annotations, function); }

            // function(annotation) [filtered to annotations specializing 'Of']
            template <template <typename ...> class Of, typename Function>
            static constexpr void forEach(Function function) { return ExtendedTypeSupport::for_each_specialization<Of>::in(annotations, function); }
        };

        template <typename SubClass, typename ...Ts>
        struct Inherit
        {
            static constexpr size_t Total = count_supers<Ts...>::value;

            template <size_t SuperIndex> using SuperInfo = SuperInfo<SuperIndex, SubClass>;

            template <typename Function>
            static constexpr void ForEach(Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<sizeof...(Ts)>([&](auto I) {
                    using Note = std::tuple_element_t<decltype(I)::value, std::tuple<Ts...>>;
                    using SuperType = typename super_type<Note>::type;
                    if constexpr ( !std::is_void_v<SuperType> ) {
                        constexpr size_t SuperIndex = note_super_index<decltype(I)::value, Ts...>::value;
                        function(Inheritance::SuperInfo<SuperIndex, SubClass>{});
                    }
                });
            }

            template <typename Function, typename U, typename = std::enable_if_t<std::is_same_v<SubClass,std::decay_t<U>>>>
            static constexpr void ForEach(U && object, Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<sizeof...(Ts)>([&](auto I) {
                    using Note = std::tuple_element_t<decltype(I)::value, std::tuple<Ts...>>;
                    using SuperType = typename super_type<Note>::type;
                    if constexpr ( !std::is_void_v<SuperType> ) {
                        static constexpr size_t SuperIndex = note_super_index<decltype(I)::value, Ts...>::value;
                        if constexpr ( std::is_const_v<decltype(std::forward<U>(object))> )
                            function(Inheritance::SuperInfo<SuperIndex, SubClass>{}, (const SuperType &)object);
                        else
                            function(Inheritance::SuperInfo<SuperIndex, SubClass>{}, (SuperType &)object);
                    }
                });
            }

            template <typename Function, typename U, typename = std::enable_if_t<std::is_same_v<SubClass,std::decay_t<U>>>>
            static constexpr void ForEachSuper(U && object, Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<sizeof...(Ts)>([&](auto I) {
                    using Note = std::tuple_element_t<decltype(I)::value, std::tuple<Ts...>>;
                    using SuperType = typename super_type<Note>::type;
                    if constexpr ( !std::is_void_v<SuperType> )
                    {
                        if constexpr ( std::is_const_v<decltype(std::forward<U>(object))> )
                            function((const SuperType &)object);
                        else
                            function((SuperType &)object);
                    }
                });
            }

            template <typename Function>
            static constexpr void At(size_t superIndex, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<sizeof...(Ts)>(superIndex, [&](auto SuperIndex) {
                    constexpr size_t I = super_note_index<decltype(SuperIndex)::value, Ts...>::value;
                    if constexpr ( I < sizeof...(Ts) ) {
                        using SuperType = typename super_type<std::tuple_element_t<I, std::tuple<Ts...>>>::type;
                        if constexpr ( !std::is_void_v<SuperType> )
                        {
                            function(Inheritance::SuperInfo<decltype(SuperIndex)::value, SubClass>{});
                        }
                    }
                });
            }

            template <typename Function, typename U, typename = std::enable_if_t<std::is_same_v<SubClass,std::decay_t<U>>>>
            static constexpr void At(size_t superIndex, U && object, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<sizeof...(Ts)>(superIndex, [&](auto SuperIndex) {
                    constexpr size_t I = super_note_index<decltype(SuperIndex)::value, Ts...>::value;
                    if constexpr ( I < sizeof...(Ts) ) {
                        using SuperType = typename super_type<std::tuple_element_t<I, std::tuple<Ts...>>>::type;
                        if constexpr ( !std::is_void_v<SuperType> )
                        {
                            auto superInfo = Inheritance::SuperInfo<decltype(SuperIndex)::value, SubClass>{};
                            if constexpr ( std::is_const_v<decltype(std::forward<U>(object))> )
                                function(superInfo, (const SuperType &)object);
                            else
                                function(superInfo, (SuperType &)object);
                        }
                    }
                });
            }

            template <typename Function, typename U, typename = std::enable_if_t<std::is_same_v<SubClass,std::decay_t<U>>>>
            static constexpr void SuperAt(size_t superIndex, U && object, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<sizeof...(Ts)>(superIndex, [&](auto SuperIndex) {
                    constexpr size_t I = super_note_index<decltype(SuperIndex)::value, Ts...>::value;
                    if constexpr ( I < sizeof...(Ts) ) {
                        using SuperType = typename super_type<std::tuple_element_t<I, std::tuple<Ts...>>>::type;
                        if constexpr ( !std::is_void_v<SuperType> )
                        {
                            if constexpr ( std::is_const_v<decltype(std::forward<U>(object))> )
                                function((const SuperType &)object);
                            else
                                function((SuperType &)object);
                        }
                    }
                });
            }
        };

        template <typename SubClass, typename ...Ts> struct Inherit<SubClass, std::tuple<Ts...>> : Inherit<SubClass, Ts...> {};
        template <typename SubClass, typename ...Ts> struct Inherit<SubClass, const std::tuple<Ts...>> : Inherit<SubClass, Ts...> {};
    }

    namespace Fields
    {
        template <typename T = void, typename Base = std::false_type, typename FieldDescr = void, typename FieldPointer = std::nullptr_t, size_t FieldIndex = 0,
            typename Notes = NoAnnotation>
        struct Field;
    
        template <>
        struct Field<void, std::false_type, void, std::nullptr_t, 0, NoAnnotation> {
            const char* name;
            const char* typeStr;
        };

        template <typename T, typename Base, typename FieldDescr, typename FieldPointer, size_t FieldIndex, typename FieldAnnotations>
        struct Field : public Base {

            using Type = T;
            using Pointer = std::conditional_t<std::is_reference_v<T>, std::nullptr_t, FieldPointer>;
            using Annotations = FieldAnnotations;
        
            static constexpr size_t Index = FieldIndex;
            static constexpr bool IsStatic = !std::is_member_pointer<FieldPointer>::value && !std::is_same<std::nullptr_t, FieldPointer>::value;
            static constexpr bool IsFunction = std::is_function_v<T> || std::is_same_v<T, FieldPointer>;
            static constexpr bool HasOffset = !IsStatic && !IsFunction && !std::is_reference_v<T>;
            static constexpr size_t getOffset() { return FieldDescr::template Get<HasOffset>::offset(); } // Returns 0 if HasOffset is false

            template <typename Annotation>
            static constexpr bool HasAnnotation = ExtendedTypeSupport::has_type<Annotation, Annotations>::value;

            template <template <typename ...> class Of>
            static constexpr bool HasSpecializedAnnotation = ExtendedTypeSupport::type_list<Annotations>::template has_specialization_v<Of>;

            template <typename Annotation>
            static constexpr const auto & getAnnotation() { return ExtendedTypeSupport::get<Annotation>::from(Base::annotations); }

            template <template <typename ...> class Of>
            static constexpr const auto & getAnnotation() {
                return ExtendedTypeSupport::get<typename ExtendedTypeSupport::type_list<Annotations>::template get_specialization_t<Of>>::from(Base::annotations);
            }

            template <typename Function>
            static constexpr void forEachAnnotation(Function function) { return ExtendedTypeSupport::for_each_in(Base::annotations, function); }
            
            // function(annotation) [filtered to type 'Annotation']
            template <typename Annotation, typename Function>
            static constexpr void forEach(Function function) { return ExtendedTypeSupport::for_each<Annotation>::in(Base::annotations, function); }

            // function(annotation) [filtered to annotations specializing 'Of']
            template <template <typename ...> class Of, typename Function>
            static constexpr void forEach(Function function) { return ExtendedTypeSupport::for_each_specialization<Of>::in(Base::annotations, function); }
        };
    }

#if _MSC_VER && !__clang__ && !__GNUC__
#define STATIC_VALUE_HELPER(x)
#define PASS_STATIC_VALUE(x) []() -> decltype(auto) { if constexpr ( x##_::Field::IsStatic && !x##_::Field::IsFunction ) return std::add_lvalue_reference_t<decltype(ClassType::x)>(ClassType::x); else return std::add_lvalue_reference_t<typename x##_::Field::Pointer>(x##_::Field::p); }()
#else
#define STATIC_VALUE_HELPER(x) \
    template<bool IsStaticData, typename Field, typename GetNull, typename T_> static constexpr std::enable_if_t<IsStaticData, std::add_lvalue_reference_t<decltype(T_::x)>> getStatic(int) { return std::add_lvalue_reference_t<decltype(T_::x)>(T_::x); } \
    template<bool IsStaticData, typename Field, typename GetNull, typename T_> static constexpr std::enable_if_t<!IsStaticData, std::add_lvalue_reference_t<decltype(&T_::x)>> getStatic(unsigned) { return std::add_lvalue_reference_t<decltype(&T_::x)>(Field::p); } \
    template<bool IsStaticData, typename Field, typename GetNull, typename T_> static constexpr std::nullptr_t & getStatic(...) { return (std::nullptr_t &)GetNull::value; }
#define PASS_STATIC_VALUE(x) FieldDescr<IndexOf::x>::template getStatic<x##_::Field::IsStatic && !x##_::Field::IsFunction, x##_::Field, x##_::template GetPointer<nullptr_t, true>, ClassType>(0)
#endif
#define PASS_STATIC_VALUE_(x) ,PASS_STATIC_VALUE(x)

#define DESCRIBE_FIELD(x) template <typename U_> struct FieldDescr<IndexOf::x, U_> { \
    template <typename T_> static constexpr ExtendedTypeSupport::TypePair<decltype(T_::x), decltype(&T_::x)> identify(int); \
    template <typename T_> static constexpr ExtendedTypeSupport::TypePair<decltype(T_::x), std::nullptr_t> identify(unsigned int); \
    template <typename T_> static constexpr ExtendedTypeSupport::TypePair<decltype(&T_::x), decltype(&T_::x)> identify(...); \
    using Type = typename decltype(identify<ProxyType>(0))::Left; \
    using Pointer = typename decltype(identify<ProxyType>(0))::Right; \
    template <typename T_, bool IsReference> struct GetPointer { static constexpr auto value = &T_::x; }; \
    template <typename T_> struct GetPointer<T_, true> { static constexpr std::nullptr_t value = nullptr; }; \
    template <typename T_> static constexpr decltype(T_::x##_note) idNote(int); \
    template <typename T_> static constexpr decltype(Reflection::NoNote) idNote(...); \
    template <typename T_, bool NoNote> struct GetNote { static constexpr auto & value = Reflection::NoNote; }; \
    template <typename T_> struct GetNote<T_, false> { static constexpr auto & value = T_::x##_note; }; \
    template <bool HasOffset, typename T_ = ClassType> struct Get { static constexpr size_t offset() { return offsetof(T_, x); } }; \
    template <typename T_> struct Get<false, T_> { static constexpr size_t offset() { return size_t(0); } }; \
    static constexpr auto typeStr_ = ExtendedTypeSupport::TypeName<Type>(); \
    struct FieldBase { \
        static constexpr const char* typeStr = typeStr_; \
        static constexpr const char name[] = #x; \
        template <bool IncludeField, typename Function, typename T_ = ClassType> static constexpr void callback(Function && function) { \
            if constexpr ( Field::IsFunction || !Field::IsStatic ) { if constexpr ( IncludeField ) { function(field, field.p); } else { function(field.p); } } \
            else if constexpr ( IncludeField ) { function(field, T_::x); } else { function(T_::x); } } \
        template <bool IncludeField, typename Function, typename T_ = ClassType> static constexpr void callback(T_ && t, Function && function) { \
            if constexpr ( Field::IsFunction ) { if constexpr ( IncludeField ) { function(field, field.p); } else { function(field.p); } } \
            else if constexpr ( IncludeField ) { function(field, std::forward<T_>(t).x); } else { function(std::forward<T_>(t).x); } } \
        static constexpr auto & annotations = GetNote<ProxyType, std::is_same_v<decltype(Reflection::NoNote), decltype(idNote<ProxyType>(0))>>::value; \
        static constexpr Pointer p { GetPointer<ProxyType, std::is_reference_v<Type>>::value }; }; \
    using Field = Reflection::Fields::Field<Type, FieldBase, FieldDescr<IndexOf::x, U_>, Pointer, IndexOf::x, decltype(idNote<ProxyType>(0))>; \
    static constexpr Field field {}; \
    STATIC_VALUE_HELPER(x) \
}; using x##_ = FieldDescr<IndexOf::x>;

#define GET_FIELD(x) { Class::x##_::field.name, Class::x##_::field.typeStr },
#define PASS_FIELD(x) x##_::field
#define PASS_FIELD_(x) ,x##_::field
#define PASS_VALUE(x) [&t]() -> decltype(auto) { if constexpr ( x##_::Field::IsFunction ) { return std::add_lvalue_reference_t<typename x##_::Field::Pointer>(x##_::Field::p); } else return std::add_lvalue_reference_t<decltype(t.x)>(t.x); }()
#define PASS_VALUE_(x) ,[&t]() -> decltype(auto) { if constexpr ( x##_::Field::IsFunction ) { return std::add_lvalue_reference_t<typename x##_::Field::Pointer>(x##_::Field::p); } else return std::add_lvalue_reference_t<decltype(t.x)>(t.x); }()
#define FIELD_ADAPTER(x) U_<IndexOf::x,Ts...> x;

/// After the objectType can be 0 to 124 field names
/// e.g. REFLECT(MyObj, myInt, myString, myOtherObj)
#define REFLECT(objectType, ...) \
struct Class { \
    using ProxyType = objectType; \
    using ClassType = Reflection::unproxy_t<ProxyType>; \
    static constexpr size_t Total = COUNT_ARGUMENTS(__VA_ARGS__); \
    enum_t(IndexOf, size_t, { __VA_ARGS__ }); \
    using Annotations = Reflection::NoAnnotation; \
    static constexpr auto & annotations = Reflection::NoNote; \
    template <size_t FieldIndex, typename U_ = void> struct FieldDescr; FOR_EACH(DESCRIBE_FIELD, __VA_ARGS__) \
    template <template<size_t I,class...> class U_, class ... Ts> struct Adapter { FOR_EACH(FIELD_ADAPTER, __VA_ARGS__) }; \
    static constexpr Reflection::Fields::Field<> Fields[Total+1] { FOR_EACH(GET_FIELD, __VA_ARGS__) }; \
    template <typename Function> static constexpr auto field_pack(Function && function) { \
        return function(FOR_HEAD_TAIL(PASS_FIELD, PASS_FIELD_, __VA_ARGS__)); } \
    template <typename Function, typename U_> static constexpr auto value_pack(U_ && t, Function && function) { \
        return function(FOR_HEAD_TAIL(PASS_VALUE, PASS_VALUE_, __VA_ARGS__)); } \
    template <typename Function> static constexpr auto value_pack(Function && function) { \
        return function(FOR_HEAD_TAIL(PASS_STATIC_VALUE, PASS_STATIC_VALUE_, __VA_ARGS__)); } \
};


/// REFLECT_NOTED is exactly the same as REFLECT except this signals that objectType itself is annotated
#define REFLECT_NOTED(objectType, ...) \
struct Class { \
    using ProxyType = objectType; \
    using ClassType = Reflection::unproxy_t<ProxyType>; \
    static constexpr size_t Total = COUNT_ARGUMENTS(__VA_ARGS__); \
    enum_t(IndexOf, size_t, { __VA_ARGS__ }); \
    static constexpr Reflection::NoAnnotation NoNote {}; \
    using Annotations = decltype(objectType##_note); \
    static constexpr Annotations & annotations = objectType##_note; \
    template <size_t FieldIndex, typename U_ = void> struct FieldDescr; FOR_EACH(DESCRIBE_FIELD, __VA_ARGS__) \
    template <template<size_t I,class...> class U_, class ... Ts> struct Adapter { FOR_EACH(FIELD_ADAPTER, __VA_ARGS__) }; \
    static constexpr Reflection::Fields::Field<> Fields[Total+1] { FOR_EACH(GET_FIELD, __VA_ARGS__) }; \
    template <typename Function> static constexpr auto field_pack(Function && function) { \
        return function(FOR_HEAD_TAIL(PASS_FIELD, PASS_FIELD_, __VA_ARGS__)); } \
    template <typename Function, typename U> static constexpr auto value_pack(U && t, Function && function) { \
        return function(FOR_HEAD_TAIL(PASS_VALUE, PASS_VALUE_, __VA_ARGS__)); } \
    template <typename Function> static constexpr auto value_pack(Function && function) { \
        return function(FOR_HEAD_TAIL(PASS_STATIC_VALUE, PASS_STATIC_VALUE_, __VA_ARGS__)); } \
};

    template <typename T, typename = typename T::Type, typename = typename T::Annotations, typename = decltype(T::Index)> static constexpr std::true_type typeIsField(int);
    template <typename T> static constexpr std::false_type typeIsField(unsigned int);

    template <typename T> struct is_field { static constexpr bool value = decltype(typeIsField<T>(0))::value; };
    template <typename T> inline constexpr bool is_field_v = is_field<T>::value;

    template <typename T> using enable_if_field_t = std::enable_if_t<is_field_v<T>>;
    
    template <template <typename...> class Filter, typename ...Ts> static constexpr Filter<Ts...> filterResult(int);
    template <template <typename...> class Filter, typename T, typename ...Ts> static constexpr std::false_type filterResult(unsigned);

    /** filter_result uses Filter<Field, Ts...> if it results in a valid expression, otherwise it uses Filter<Field::Type, Ts...>
        This allows the filter to be based on field metadata (e.g. whether a variable is static), or operate purely on the field type */
    template <template <typename...> class Filter, typename Field, typename ...Ts> struct filter_result :
        std::bool_constant<decltype(filterResult<Filter, Field, Ts...>(0))::value || decltype(filterResult<Filter, typename Field::Type, Ts...>(0))::value> {};
    template <template <typename...> class Filter, typename T, typename ...Ts> inline constexpr bool passes_filter = filter_result<Filter, T, Ts...>::value;

    struct Filter {
        template <typename Field, typename = enable_if_field_t<Field>> struct None : std::bool_constant<true> {};
        template <typename Field, typename = enable_if_field_t<Field>> struct IsData : std::bool_constant<!Field::IsFunction> {}; // Data is defined as non-functions (raw data, pointers, references)
        template <typename Field, typename = enable_if_field_t<Field>> struct IsFunction : std::bool_constant<Field::IsFunction> {};
        template <typename Field, typename = enable_if_field_t<Field>> struct IsInstanceField : std::bool_constant<!Field::IsStatic> {};
        template <typename Field, typename = enable_if_field_t<Field>> struct IsStaticField : std::bool_constant<Field::IsStatic> {};
        template <typename Field, typename = enable_if_field_t<Field>> struct IsInstanceData : std::bool_constant<!Field::IsStatic && !Field::IsFunction> {};
        template <typename Field, typename = enable_if_field_t<Field>> struct IsInstanceFunction : std::bool_constant<!Field::IsStatic && Field::IsFunction> {};
        template <typename Field, typename = enable_if_field_t<Field>> struct IsStaticData : std::bool_constant<Field::IsStatic && !Field::IsFunction> {};
        template <typename Field, typename = enable_if_field_t<Field>> struct IsStaticFunction : std::bool_constant<Field::IsStatic && Field::IsFunction> {};
		
        template <template <typename...> class Of> struct Is {
            template <typename Field, typename = enable_if_field_t<Field>> struct Specialization : std::bool_constant<
                ExtendedTypeSupport::is_specialization_v<typename Field::Type, Of>> {};
        };
    };

    template <typename T>
    struct Reflect
    {
        using Type = T;
        using Class = Reflection::class_t<T>; // Access to the underlying T::Class (or Proxy<T>::Class) reflection structure
        using Supers = Reflection::Inherit<unproxy_t<T>, typename Class::Annotations>; // Super-class info for type T

        struct Fields // Access to field information
        {
            static constexpr size_t Total = Class::Total; // The total number of reflected fields
            
            template <size_t FieldIndex> using Field = typename Class::template FieldDescr<FieldIndex>::Field; // Field<I> - field type by index

            // field<I> - field instance by index
            template <size_t FieldIndex> static constexpr const Field<FieldIndex> & field = Class::template FieldDescr<FieldIndex>::field;

        protected:
            template <size_t ... I> static constexpr auto fieldIndexMap(std::index_sequence<I...>) {
				if constexpr ( sizeof...(I) > 0 )
					return ExtendedTypeSupport::StringIndexMap<(Reflect<T>::Fields::Field<I>::name)...>();
				else
					return ExtendedTypeSupport::StringIndexMap<>();
            }

            using FieldNames = decltype(fieldIndexMap(std::make_index_sequence<Reflect<T>::Fields::Total>()));

            template <size_t I = 0, typename F>
            static constexpr std::true_type uses_field_only(F && f, std::conditional_t<true, int, decltype(std::forward<F>(f)(field<I>))> = 0) { return {}; }
            template <size_t I = 0> static constexpr std::false_type uses_field_only(...) { return {}; }

        public:
            // Gets the index of a field using the field name
            template <class U=T> static constexpr size_t IndexOf(std::string_view fieldName) {
                return FieldNames::indexOf(fieldName);
            }

            // Forwards all fields to function as a parameter pack
            template <typename Function> static constexpr auto Pack(Function && function) { return Class::field_pack(std::forward<Function>(function)); }

            // The total number of reflected fields after the given filter is applied
            template <template <typename ...> class Filter, typename ...FilterArgs>
            static constexpr size_t FilteredCount() {
                if constexpr ( Total == 0 )
                    return 0;
                else
                {
                    size_t count = 0;
                    ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                        if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                            ++count;
                    });
                    return count;
                }
            };
            
            // function(field) or...
            // function(field, value) [statics only]
            template <typename Function> static constexpr void ForEach(Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                    if constexpr ( decltype(uses_field_only(std::forward<Function>(function)))::value )
                        function(field<decltype(I)::value>);
                    else if constexpr ( Field<decltype(I)::value>::IsStatic )
                        Field<decltype(I)::value>::template callback<true>(std::forward<Function>(function));
                });
            }

            // function(field) [filtered] or...
            // function(field, value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function> static constexpr void ForEach(Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                    if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                    {
                        if constexpr ( decltype(uses_field_only<decltype(I)::value>(std::forward<Function>(function)))::value )
                            function(field<decltype(I)::value>);
                        else if constexpr ( Field<decltype(I)::value>::IsStatic )
                            Field<decltype(I)::value>::template callback<true>(std::forward<Function>(function));
                    }
                });
            }

            // t, function(field, value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void ForEach(U && t, Function && function) {
                
                ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                    Field<decltype(I)::value>::template callback<true>(std::forward<U>(t), std::forward<Function>(function));
                });
            }

            // t, function(field, value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static void ForEach(U && t, Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                    if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<true>(std::forward<U>(t), std::forward<Function>(function));
                });
            }
            
            // function(field) or...
            // function(field, value) [statics only]
            template <typename Function> static constexpr void At(size_t fieldIndex, Function && function) {
				ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
					if constexpr ( decltype(uses_field_only(std::forward<Function>(function)))::value )
						function(field<decltype(I)::value>);
                    else if constexpr ( Field<decltype(I)::value>::IsStatic )
						Field<decltype(I)::value>::template callback<true>(std::forward<Function>(function));
				});
            }
            
            // function(field) [filtered] or...
            // function(field, value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function> static constexpr void At(size_t fieldIndex, Function && function) {
				ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
					if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                    {
					    if constexpr ( decltype(uses_field_only<decltype(I)::value>(std::forward<Function>(function)))::value )
							function(field<decltype(I)::value>);
                        else if constexpr ( Field<decltype(I)::value>::IsStatic )
							Field<decltype(I)::value>::template callback<true>(std::forward<Function>(function));
                    }
				});
            }

            // t, function(field, value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void At(size_t fieldIndex, U && t, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    Field<decltype(I)::value>::template callback<true>(std::forward<U>(t), std::forward<Function>(function));
                });
            }

            // t, function(field, value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void At(size_t fieldIndex, U && t, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<true>(std::forward<U>(t), std::forward<Function>(function));
                });
            }
            
            // fieldName, function(field) or...
            // fieldName, function(field, value) [statics only]
            template <typename Function> static constexpr void Named(std::string_view fieldName, Function && function) {
			    size_t fieldIndex = IndexOf<>(fieldName);
			    ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
				    if constexpr ( decltype(Reflect<T>::Fields::uses_field_only(std::forward<Function>(function)))::value )
					    function(field<decltype(I)::value>);
				    else if constexpr ( Field<decltype(I)::value>::IsStatic )
					    Field<decltype(I)::value>::template callback<true>(std::forward<Function>(function));
			    });
            }
            
            // fieldName, function(field) [filtered] or...
            // fieldName, function(field, value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function> static constexpr void Named(std::string_view fieldName, Function && function) {
			    size_t fieldIndex = IndexOf<>(fieldName);
			    ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
				    if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
				    {
					    if constexpr ( decltype(Reflect<T>::Fields::uses_field_only<decltype(I)::value>(std::forward<Function>(function)))::value )
						    function(field<decltype(I)::value>);
					    else if constexpr ( Field<decltype(I)::value>::IsStatic )
						    Field<decltype(I)::value>::template callback<true>(std::forward<Function>(function));
				    }
			    });
            }

            // t, fieldName, function(field, value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void Named(std::string_view fieldName, U && t, Function && function) {
                size_t fieldIndex = IndexOf<>(fieldName);
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    Field<decltype(I)::value>::template callback<true>(std::forward<U>(t), std::forward<Function>(function));
                });
            }

            // t, fieldName, function(field, value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void Named(std::string_view fieldName, U && t, Function && function) {
                size_t fieldIndex = IndexOf<>(fieldName);
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<true>(std::forward<U>(t), std::forward<Function>(function));
                });
            }
        };

        class Values // Access to field-value information
        {
            template <size_t FieldIndex> using Field = typename Fields::template Field<FieldIndex>;

        public:
            static constexpr size_t Total = Fields::Total; // The total number of reflected fields

            // Forwards the static value of all fields (member pointers if instance field) to function as a parameter pack
            template <typename Function> static constexpr auto Pack(Function && function) { return Class::value_pack(function); }
            
            // Forwards the value of all fields to function as a parameter pack
            template <typename Function, class U> static constexpr auto Pack(U && t, Function && function) { return Class::value_pack(std::forward<U>(t), std::forward<Function>(function)); }

            // function(value) [statics only]
            template <typename Function> static constexpr void ForEach(Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                    if constexpr ( Field<decltype(I)::value>::IsStatic )
                        Field<decltype(I)::value>::template callback<false>(std::forward<Function>(function));
                });
            }

            // function(value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function> static constexpr void ForEach(Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                    if constexpr ( Field<decltype(I)::value>::IsStatic && passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<false>(std::forward<Function>(function));
                });
            }

            // t, function(value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void ForEach(U && t, Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                    Field<decltype(I)::value>::template callback<false>(std::forward<U>(t), std::forward<Function>(function));
                });
            }

            // t, function(value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void ForEach(U && t, Function && function) {
                ExtendedTypeSupport::ForIntegralConstants<Total>([&](auto I) {
                    if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<false>(std::forward<U>(t), std::forward<Function>(function));
                });
            }
            
            // function(value) [statics only]
            template <typename Function> static constexpr void At(size_t fieldIndex, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    if constexpr ( Field<decltype(I)::value>::IsStatic )
                        Field<decltype(I)::value>::template callback<false>(std::forward<Function>(function));
                });
            }
            
            // function(value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function> static constexpr void At(size_t fieldIndex, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    if constexpr ( Field<decltype(I)::value>::IsStatic && passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<false>(std::forward<Function>(function));
                });
            }

            // t, function(value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void At(size_t fieldIndex, U && t, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    Field<decltype(I)::value>::template callback<false>(std::forward<U>(t), std::forward<Function>(function));
                });
            }
            
            // t, function(value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void At(size_t fieldIndex, U && t, Function && function) {
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<false>(std::forward<U>(t), std::forward<Function>(function));
                });
            }

            // fieldName, function(value) [statics only]
            template <typename Function> static constexpr void Named(std::string_view fieldName, Function && function) {
                size_t fieldIndex = Fields::template IndexOf<>(fieldName);
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    if constexpr ( Field<decltype(I)::value>::IsStatic )
                        Field<decltype(I)::value>::template callback<false>(std::forward<Function>(function));
                });
            }

            // fieldName, function(value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function> static constexpr void Named(std::string_view fieldName, Function && function) {
                size_t fieldIndex = Fields::template IndexOf<>(fieldName);
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    if constexpr ( Field<decltype(I)::value>::IsStatic && passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<false>(std::forward<Function>(function));
                });
            }
            
            // t, fieldName, function(value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void Named(std::string_view fieldName, U && t, Function && function) {
                size_t fieldIndex = Fields::template IndexOf<>(fieldName);
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    Field<decltype(I)::value>::template callback<false>(std::forward<U>(t), std::forward<Function>(function));
                });
            }

            // t, fieldName, function(value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void Named(std::string_view fieldName, U && t, Function && function) {
                size_t fieldIndex = Fields::template IndexOf<>(fieldName);
                ExtendedTypeSupport::ForIntegralConstant<Total>(fieldIndex, [&](auto I) {
                    if constexpr ( passes_filter<Filter, Field<decltype(I)::value>, FilterArgs...> )
                        Field<decltype(I)::value>::template callback<false>(std::forward<U>(t), std::forward<Function>(function));
                });
            }
        };

        /*struct Experimental // May have better support in the future, but for now this is experimental
        {
            struct Members
            {
                T & t;
            
                constexpr Members(T & t) : t(t) {}

                // Members are experimental 
                struct Member
                {
                    T & t;
                    size_t index;
                    const char* typeStr;
                    const char* name;
                    Member & value;

                    constexpr Member(T & t, size_t index, const char* typeStr, const char* name)
                        : t(t), index(index), typeStr(typeStr), name(name), value(*this) {}

                    template <typename Function> constexpr void operator()(Function function) const { Fields::ValueAt(t, index, function); }
                    template <typename V> constexpr void operator=(V v) { Fields::ValueAt(t, index, [&](auto & value){ value = v; }); }
                    friend std::ostream & operator<<(std::ostream & os, const Member & m)
                    { Fields::ValueAt(m.t, m.index, [&](auto & value) { os << value; }); return os; }
                };

                constexpr auto operator[](size_t i) const { return Member{t, i, Class::Fields[i].typeStr, Class::Fields[i].name}; }
                constexpr auto operator[](int i) const { return Member{t, size_t(i), Class::Fields[i].typeStr, Class::Fields[i].name}; }
                constexpr auto size() { return Fields::Total; }

                struct Iterator
                {
                    T & t;
                    size_t i;

                    void operator++() { ++this->i; }
                    constexpr Member operator->() { return Member{t, size_t(i), Class::Fields[i].typeStr, Class::Fields[i].name}; }
                    constexpr Member operator*() { return Member{t, size_t(i), Class::Fields[i].typeStr, Class::Fields[i].name}; }
                    bool operator!=(const Iterator & other) { return this->i != other.i; }
                };

                constexpr auto begin() { return Iterator{t, 0}; }
                constexpr auto end() { return Iterator{t, Fields::Total}; }
            };
        };*/

        // Specifically the class-level annotations of the reflected type, not field-level annotations
        struct Notes
        {
            // function(annotation)
            template <typename Function>
            static constexpr void ForEach(Function function) { return ExtendedTypeSupport::for_each_in(Class::annotations, function); }

            // function(annotation) [filtered to type 'Annotation']
            template <typename Annotation, typename Function>
            static constexpr void ForEach(Function function) { return ExtendedTypeSupport::for_each<Annotation>::in(Class::annotations, function); }

            // function(annotation) [filtered to annotations specializing 'Of']
            template <template <typename ...> class Of, typename Function>
            static constexpr void ForEach(Function function) { return ExtendedTypeSupport::for_each_specialization<Of>::in(Class::annotations, function); }

            template <typename Annotation>
            static constexpr bool Has = ExtendedTypeSupport::has_type<Annotation, typename Class::Annotations>::value;

            template <template <typename ...> class Of>
            static constexpr bool HasSpecialization = ExtendedTypeSupport::type_list<typename Class::Annotations>::template has_specialization_v<Of>;

            template <typename Annotation>
            static constexpr auto & Get() { return ExtendedTypeSupport::get<Annotation>::from(Class::annotations); }

            template <template <typename ...> class Of>
            static constexpr auto & Get() {
                return ExtendedTypeSupport::get<typename ExtendedTypeSupport::type_list<typename Class::Annotations>::template get_specialization_t<Of>>::from(Class::annotations);
            }
        };
    };
}
using Reflection::Reflect;

namespace ObjectMapper
{
#define OM_R(l,r) ObjectMapper::map(o.r,this->l);
#define OM_S(l,r) ObjectMapper::map(this->l,o.r);
#define OM_O(a) OM_R a
#define OM_T(a) OM_S a

/// Defines a mapping from "this" to objectType
/// After the objectType there needs to be at least 1 and at most 124 parenthesized field mappings ("this" fields on left, objectType fields on right)
/// e.g. MAP_TO(target, (a, a), (b, c), (d, d))
#define MAP_TO(objectType, ...) void map_to(objectType & o) const { FOR_EACH_POSITIVE(OM_O, __VA_ARGS__) }

/// Defines a mapping from objectType to "this"
/// After the objectType there needs to be at least 1 and at most 124 parenthesized field mappings ("this" fields on left, objectType fields on right)
/// e.g. MAP_FROM(target, (a, a), (b, c), (d, d))
#define MAP_FROM(objectType, ...) void map_from(const objectType & o) { FOR_EACH_POSITIVE(OM_T, __VA_ARGS__) }

/// Defines a bi-directional mapping between "this" and objectType
/// After the objectType there needs to be at least 1 and at most 124 parenthesized field mappings ("this" fields on left, objectType fields on right)
/// e.g. MAP_WITH(target, (a, a), (b, c), (d, d))
#define MAP_WITH(objectType, ...) MAP_TO(objectType, __VA_ARGS__) MAP_FROM(objectType, __VA_ARGS__)

    /// Default mapping implementation, safe to call from ObjectMapper::map specializations, assignment & conversion operators, and map_to/map_from methods
    /// Do not specialize this method
    template <typename To, typename From>
    constexpr inline void map_default(To &, const From &);
    
    /// Default mapping helper, safe to call from ObjectMapper::map specializations, assignment & conversion operators, and map_to/map_from methods
    /// Do not specialize this method
    template<typename To, typename From>
    constexpr inline To map_default(const From & from)
    {
        To to;
        ObjectMapper::map_default(to, from);
        return to;
    }

    /// If any mapping exists from "from" to "to", "to" is assigned mapped values from "from", if no mapping exists, "to" is unchanged and nothing is thrown
    /// A mapping may exist if...
    /// - Both "To" and "From" are reflected objects and have fields with identical names and compatible types
    /// - Both "To" and "From" are compatible types ("to = from" or "to = static_cast<To>(from)" is valid)
    /// - Both "To" and "From" are compatible pairs, tuples, array, or STL containers
    /// - An assignment operator, converting constructor, or conversion operator exists
    /// - A map_to or map_from member method exists in "to" or "from" (which may or may not have been generated by the MAP_TO/MAP_FROM/MAP_WITH macros)
    /// - This method was specialized
    /// 
    /// To avoid infinite recursion, call ObjectMapper::map_default instead of ObjectMapper::map when you want default mappings in...
    /// ObjectMapper::map specializations, assignment operators, conversion operators, converting constructors, map_to, or map_from methods with the same types
    template <typename To, typename From>
    constexpr inline void map(To & to, const From & from)
    {
        if constexpr ( std::is_const_v<To> )
            return;
        else if constexpr ( ExtendedTypeSupport::HasMapFrom<To, From> )
            to.map_from(from);
        else if constexpr ( ExtendedTypeSupport::HasMapTo<From, To> )
            from.map_to(to);
        else if constexpr ( ExtendedTypeSupport::IsAssignable<decltype(to), decltype(from)> )
            to = from;
        else if constexpr ( ExtendedTypeSupport::IsStaticCastAssignable<decltype(to), decltype(from)> )
            to = static_cast<To>(from);
        else
            ObjectMapper::map_default(to, from);
    }

    /// Helper method for ObjectMapper::map(To &, From &); do not specialize this method
    template <typename To, typename From>
    constexpr inline To map(const From & from)
    {
        To to;
        ObjectMapper::map(to, from);
        return to;
    }

    /// Helper method for ObjectMapper::map_default(To &, From &); do not specialize this method
    template <size_t Index, typename ...To, typename ...From>
    constexpr inline void map_tuple(std::tuple<To...> & to, const std::tuple<From...> & from)
    {
        if constexpr ( Index < sizeof...(To) && Index < sizeof...(From) )
        {
            ObjectMapper::map(std::get<Index>(to), std::get<Index>(from));
            ObjectMapper::map_tuple<Index+1>(to, from);
        }
    }

    template <typename To, typename From>
    constexpr inline void map_default(To & to, const From & from)
    {
        if constexpr ( std::is_const_v<To> )
            return;
        else if constexpr ( ExtendedTypeSupport::is_pointable<To>::value )
        {
            using ToDereferenced = typename ExtendedTypeSupport::remove_pointer<To>::type;
            if ( to == nullptr )
            {
                if constexpr ( ExtendedTypeSupport::is_pointable<From>::value )
                {
                    if ( from == nullptr )
                        return;
                    else if constexpr ( std::is_same_v<std::shared_ptr<ToDereferenced>, To> )
                    {
                        if constexpr ( std::is_same_v<From, To> )
                            to = from; // Share shared pointer
                        else
                        {
                            to = std::make_shared<ToDereferenced>();
                            ObjectMapper::map(*to, *from);
                        }
                    }
                    else if constexpr ( std::is_same_v<std::unique_ptr<ToDereferenced>, To> )
                    {
                        to = std::make_unique<ToDereferenced>();
                        ObjectMapper::map(*to, *from);
                    }
                }
                else if constexpr ( std::is_same_v<std::shared_ptr<ToDereferenced>, To> )
                {
                    to = std::make_shared<ToDereferenced>();
                    ObjectMapper::map(*to, from);
                }
                else if constexpr ( std::is_same_v<std::unique_ptr<ToDereferenced>, To> )
                {
                    to = std::make_unique<ToDereferenced>();
                    ObjectMapper::map(*to, from);
                }
            }
            else // to != nullptr
            {
                if constexpr ( ExtendedTypeSupport::is_pointable<From>::value )
                {
                    if ( from == nullptr )
                        to = nullptr;
                    else
                        ObjectMapper::map(*to, *from);
                }
                else
                    ObjectMapper::map(*to, from);
            }
        }
        else if constexpr ( ExtendedTypeSupport::is_pointable<From>::value )
        {
            if ( from != nullptr )
                ObjectMapper::map(to, *from);
        }
        else if constexpr ( ExtendedTypeSupport::is_pair<To>::value && ExtendedTypeSupport::is_pair<From>::value )
        {
            ObjectMapper::map(to.first, from.first);
            ObjectMapper::map(to.second, from.second);
        }
        else if constexpr ( ExtendedTypeSupport::is_tuple<To>::value && ExtendedTypeSupport::is_tuple<From>::value )
        {
            ObjectMapper::map_tuple<0>(to, from);
        }
        else if constexpr ( ExtendedTypeSupport::is_iterable<To>::value && ExtendedTypeSupport::is_iterable<From>::value )
        {
            using ToElementType = typename ExtendedTypeSupport::element_type<To>::type;
            using FromElementType = typename ExtendedTypeSupport::element_type<From>::type;
            if constexpr ( (ExtendedTypeSupport::is_stl_iterable<To>::value || ExtendedTypeSupport::is_adaptor<To>::value) &&
                (ExtendedTypeSupport::is_stl_iterable<From>::value || ExtendedTypeSupport::is_adaptor<From>::value) )
            {
                ExtendedTypeSupport::Clear(to);
                for ( auto & fromElement : from )
                {
                    ToElementType toElement;
                    ObjectMapper::map(toElement, fromElement);
                    ExtendedTypeSupport::Append(to, toElement);
                }
            }
            else if constexpr ( ExtendedTypeSupport::is_static_array<From>::value && ExtendedTypeSupport::static_array_size<From>::value > 0 )
            {
                if constexpr ( ExtendedTypeSupport::is_static_array<To>::value && ExtendedTypeSupport::static_array_size<To>::value > 0 )
                {
                    constexpr size_t limit = std::min(ExtendedTypeSupport::static_array_size<To>::value, ExtendedTypeSupport::static_array_size<From>::value);
                    for ( size_t i=0; i<limit; i++ )
                        ObjectMapper::map(to[i], from[i]);
                }
                else
                {
                    ExtendedTypeSupport::Clear(to);
                    for ( size_t i=0; i<ExtendedTypeSupport::static_array_size<From>::value; i++ )
                    {
                        ToElementType toElement;
                        ObjectMapper::map(toElement, from[i]);
                        ExtendedTypeSupport::Append(to, toElement);
                    }
                }
            }
            else if constexpr ( ExtendedTypeSupport::is_static_array<To>::value && ExtendedTypeSupport::static_array_size<To>::value > 0 )
            {
                size_t i=0;
                for ( auto & element : from )
                {
                    ObjectMapper::map(to[i], element);
                    if ( ++i == ExtendedTypeSupport::static_array_size<To>::value )
                        break;
                }
            }
        }
        else if constexpr ( Reflection::is_reflected<To>::value && Reflection::is_reflected<From>::value )
        {
            Reflect<To>::Fields::ForEach(to, [&](auto & toField, auto & toValue) {
                Reflect<From>::Fields::ForEach(from, [&](auto & fromField, auto & fromValue) {
                    if constexpr ( std::string_view(toField.name) == std::string_view(fromField.name) )
                        ObjectMapper::map(toValue, fromValue);
                });
            });
        }
    }

    inline namespace Annotations
    {
        template <typename MappedBy, typename Type = void> struct MappedByType { using Object = Type; using DefaultMapping = MappedBy; };
        template <typename MappedBy> struct MappedByType<MappedBy, void> { using DefaultMapping = MappedBy; };

        /// Field or class-level annotation saying a field or class should be mapped to "T" for activities like serialization
        template <typename T> static constexpr MappedByType<T, void> MappedBy{};

        /// Operation annotation saying type "T" should be mapped to type "MappedBy" for activities like serialization
        template <typename T, typename MappedBy> using UseMapping = MappedByType<MappedBy, T>;

        template <typename T> inline constexpr bool IsMappedByNotes = ExtendedTypeSupport::type_list<T>::template has_specialization_v<MappedByType>;
        template <typename T> inline constexpr bool IsMappedByClassNote = IsMappedByNotes<Reflection::class_notes_t<T>>;

        template <typename T, typename Enable = void> struct GetMappingFromNotes { using type = void; };
        template <typename T> struct GetMappingFromNotes<T, std::enable_if_t<IsMappedByNotes<T>>>
        { using type = typename ExtendedTypeSupport::type_list<T>::template get_specialization_t<MappedByType>::DefaultMapping; };

        template <typename T, typename Enable = void> struct GetMappingByClassNote { using type = void; };
        template <typename T> struct GetMappingByClassNote<T, std::enable_if_t<IsMappedByClassNote<T>>>
        { using type = typename GetMappingFromNotes<Reflection::class_notes_t<T>>::type; };

        template <typename T> struct SetTags { using DefaultMapping = void; }; // e.g. struct ObjectMapper::SetTags<Foo> : IsMappedBy<Bar> {};
        template <typename T> using GetTags = SetTags<T>; // e.g. ObjectMapper::GetProperty<Foo>::MappedBy::DefaultMapping
        template <typename T> using IsMappedBy = MappedByType<T>; // Tags a type "T" to be mapped to for activities like serialization
/// Sets default type "mappedBy" which this object "object" should be mapped to for activities like serialization
#define SET_DEFAULT_OBJECT_MAPPING(object, mappedBy) template <> struct ObjectMapper::SetTags<object> : IsMappedBy<mappedBy> {};

        template <typename T> inline constexpr bool IsMappedByTags = !std::is_void_v<typename GetTags<T>::DefaultMapping>;

        template <typename T, typename Note, typename Enable = void> struct IsUseMappingNote { static constexpr bool value = false; };
        template <typename T, typename Note> struct IsUseMappingNote<T, Note, std::enable_if_t<ExtendedTypeSupport::is_specialization_v<Note, MappedByType>>>
        { static constexpr bool value = std::is_same_v<T, typename Note::Object>; };

        template <typename T, typename Notes> struct HasUseMappingNote { static constexpr bool value = false; };
        template <typename T, typename ...Ts> struct HasUseMappingNote<T, std::tuple<Ts...>>
        { static constexpr bool value = (IsUseMappingNote<T, Ts>::value || ...); };

        template <typename T, typename Notes, typename Enable = void> struct GetOpNoteMapping { using type = void; };
        template <typename T, typename Notes> struct GetOpNoteMapping<T, Notes, std::enable_if_t<HasUseMappingNote<T, Notes>::value>>
        { using type = typename ExtendedTypeSupport::type_list<Notes>::template get_specialization_t<MappedByType>::DefaultMapping; };

        /// Checks whether the type "T" has a type to use for default mappings for activies like serialization
        template <typename T, typename FieldNotes = void, typename OpNotes = void> inline constexpr bool HasDefaultMapping =
            HasUseMappingNote<T, OpNotes>::value || IsMappedByNotes<FieldNotes> || IsMappedByTags<T> || IsMappedByClassNote<T>;
        
        /// Gets the type which "T" should be mapped to for activities like serialization (or void if no default exists)
        template <typename T, typename FieldNotes = void, typename OpNotes = void>
        using GetDefaultMapping = ExtendedTypeSupport::if_void_t<typename GetOpNoteMapping<T, OpNotes>::type,
            ExtendedTypeSupport::if_void_t<typename GetMappingFromNotes<FieldNotes>::type,
            ExtendedTypeSupport::if_void_t<typename GetTags<T>::DefaultMapping, typename GetMappingByClassNote<T>::type>>>;
    }
};

#endif