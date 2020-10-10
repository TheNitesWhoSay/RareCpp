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
#pragma warning(disable: 26812) // In the context of using enum_t, enum class is definitely not preferred, disable the warning in visual studios
#endif

/// Contains everything neccessary to loop over varadic macro arguments
namespace MacroLoops
{
/// ArgMax: 125 (derived from the C spec limiting macros to 126 arguments and the COUNT_ARGUMENTS helper macro "ML_M" requiring ArgMax+1 arguments)

/// Extractor_Expand
#define EO_E(x) x

/// Extractor_Concatenate
#define EO_C(x,y) x##y

/// MacroLoop_ForEach[1, ..., ArgMax]
#define ML_1(f,a,...) f(a)
#define ML_2(f,a,...) f(a) EO_E(ML_1(f,__VA_ARGS__))
#define ML_3(f,a,...) f(a) EO_E(ML_2(f,__VA_ARGS__))
#define ML_4(f,a,...) f(a) EO_E(ML_3(f,__VA_ARGS__))
#define ML_5(f,a,...) f(a) EO_E(ML_4(f,__VA_ARGS__))
#define ML_6(f,a,...) f(a) EO_E(ML_5(f,__VA_ARGS__))
#define ML_7(f,a,...) f(a) EO_E(ML_6(f,__VA_ARGS__))
#define ML_8(f,a,...) f(a) EO_E(ML_7(f,__VA_ARGS__))
#define ML_9(f,a,...) f(a) EO_E(ML_8(f,__VA_ARGS__))
#define ML_10(f,a,...) f(a) EO_E(ML_9(f,__VA_ARGS__))
#define ML_11(f,a,...) f(a) EO_E(ML_10(f,__VA_ARGS__))
#define ML_12(f,a,...) f(a) EO_E(ML_11(f,__VA_ARGS__))
#define ML_13(f,a,...) f(a) EO_E(ML_12(f,__VA_ARGS__))
#define ML_14(f,a,...) f(a) EO_E(ML_13(f,__VA_ARGS__))
#define ML_15(f,a,...) f(a) EO_E(ML_14(f,__VA_ARGS__))
#define ML_16(f,a,...) f(a) EO_E(ML_15(f,__VA_ARGS__))
#define ML_17(f,a,...) f(a) EO_E(ML_16(f,__VA_ARGS__))
#define ML_18(f,a,...) f(a) EO_E(ML_17(f,__VA_ARGS__))
#define ML_19(f,a,...) f(a) EO_E(ML_18(f,__VA_ARGS__))
#define ML_20(f,a,...) f(a) EO_E(ML_19(f,__VA_ARGS__))
#define ML_21(f,a,...) f(a) EO_E(ML_20(f,__VA_ARGS__))
#define ML_22(f,a,...) f(a) EO_E(ML_21(f,__VA_ARGS__))
#define ML_23(f,a,...) f(a) EO_E(ML_22(f,__VA_ARGS__))
#define ML_24(f,a,...) f(a) EO_E(ML_23(f,__VA_ARGS__))
#define ML_25(f,a,...) f(a) EO_E(ML_24(f,__VA_ARGS__))
#define ML_26(f,a,...) f(a) EO_E(ML_25(f,__VA_ARGS__))
#define ML_27(f,a,...) f(a) EO_E(ML_26(f,__VA_ARGS__))
#define ML_28(f,a,...) f(a) EO_E(ML_27(f,__VA_ARGS__))
#define ML_29(f,a,...) f(a) EO_E(ML_28(f,__VA_ARGS__))
#define ML_30(f,a,...) f(a) EO_E(ML_29(f,__VA_ARGS__))
#define ML_31(f,a,...) f(a) EO_E(ML_30(f,__VA_ARGS__))
#define ML_32(f,a,...) f(a) EO_E(ML_31(f,__VA_ARGS__))
#define ML_33(f,a,...) f(a) EO_E(ML_32(f,__VA_ARGS__))
#define ML_34(f,a,...) f(a) EO_E(ML_33(f,__VA_ARGS__))
#define ML_35(f,a,...) f(a) EO_E(ML_34(f,__VA_ARGS__))
#define ML_36(f,a,...) f(a) EO_E(ML_35(f,__VA_ARGS__))
#define ML_37(f,a,...) f(a) EO_E(ML_36(f,__VA_ARGS__))
#define ML_38(f,a,...) f(a) EO_E(ML_37(f,__VA_ARGS__))
#define ML_39(f,a,...) f(a) EO_E(ML_38(f,__VA_ARGS__))
#define ML_40(f,a,...) f(a) EO_E(ML_39(f,__VA_ARGS__))
#define ML_41(f,a,...) f(a) EO_E(ML_40(f,__VA_ARGS__))
#define ML_42(f,a,...) f(a) EO_E(ML_41(f,__VA_ARGS__))
#define ML_43(f,a,...) f(a) EO_E(ML_42(f,__VA_ARGS__))
#define ML_44(f,a,...) f(a) EO_E(ML_43(f,__VA_ARGS__))
#define ML_45(f,a,...) f(a) EO_E(ML_44(f,__VA_ARGS__))
#define ML_46(f,a,...) f(a) EO_E(ML_45(f,__VA_ARGS__))
#define ML_47(f,a,...) f(a) EO_E(ML_46(f,__VA_ARGS__))
#define ML_48(f,a,...) f(a) EO_E(ML_47(f,__VA_ARGS__))
#define ML_49(f,a,...) f(a) EO_E(ML_48(f,__VA_ARGS__))
#define ML_50(f,a,...) f(a) EO_E(ML_49(f,__VA_ARGS__))
#define ML_51(f,a,...) f(a) EO_E(ML_50(f,__VA_ARGS__))
#define ML_52(f,a,...) f(a) EO_E(ML_51(f,__VA_ARGS__))
#define ML_53(f,a,...) f(a) EO_E(ML_52(f,__VA_ARGS__))
#define ML_54(f,a,...) f(a) EO_E(ML_53(f,__VA_ARGS__))
#define ML_55(f,a,...) f(a) EO_E(ML_54(f,__VA_ARGS__))
#define ML_56(f,a,...) f(a) EO_E(ML_55(f,__VA_ARGS__))
#define ML_57(f,a,...) f(a) EO_E(ML_56(f,__VA_ARGS__))
#define ML_58(f,a,...) f(a) EO_E(ML_57(f,__VA_ARGS__))
#define ML_59(f,a,...) f(a) EO_E(ML_58(f,__VA_ARGS__))
#define ML_60(f,a,...) f(a) EO_E(ML_59(f,__VA_ARGS__))
#define ML_61(f,a,...) f(a) EO_E(ML_60(f,__VA_ARGS__))
#define ML_62(f,a,...) f(a) EO_E(ML_61(f,__VA_ARGS__))
#define ML_63(f,a,...) f(a) EO_E(ML_62(f,__VA_ARGS__))
#define ML_64(f,a,...) f(a) EO_E(ML_63(f,__VA_ARGS__))
#define ML_65(f,a,...) f(a) EO_E(ML_64(f,__VA_ARGS__))
#define ML_66(f,a,...) f(a) EO_E(ML_65(f,__VA_ARGS__))
#define ML_67(f,a,...) f(a) EO_E(ML_66(f,__VA_ARGS__))
#define ML_68(f,a,...) f(a) EO_E(ML_67(f,__VA_ARGS__))
#define ML_69(f,a,...) f(a) EO_E(ML_68(f,__VA_ARGS__))
#define ML_70(f,a,...) f(a) EO_E(ML_69(f,__VA_ARGS__))
#define ML_71(f,a,...) f(a) EO_E(ML_70(f,__VA_ARGS__))
#define ML_72(f,a,...) f(a) EO_E(ML_71(f,__VA_ARGS__))
#define ML_73(f,a,...) f(a) EO_E(ML_72(f,__VA_ARGS__))
#define ML_74(f,a,...) f(a) EO_E(ML_73(f,__VA_ARGS__))
#define ML_75(f,a,...) f(a) EO_E(ML_74(f,__VA_ARGS__))
#define ML_76(f,a,...) f(a) EO_E(ML_75(f,__VA_ARGS__))
#define ML_77(f,a,...) f(a) EO_E(ML_76(f,__VA_ARGS__))
#define ML_78(f,a,...) f(a) EO_E(ML_77(f,__VA_ARGS__))
#define ML_79(f,a,...) f(a) EO_E(ML_78(f,__VA_ARGS__))
#define ML_80(f,a,...) f(a) EO_E(ML_79(f,__VA_ARGS__))
#define ML_81(f,a,...) f(a) EO_E(ML_80(f,__VA_ARGS__))
#define ML_82(f,a,...) f(a) EO_E(ML_81(f,__VA_ARGS__))
#define ML_83(f,a,...) f(a) EO_E(ML_82(f,__VA_ARGS__))
#define ML_84(f,a,...) f(a) EO_E(ML_83(f,__VA_ARGS__))
#define ML_85(f,a,...) f(a) EO_E(ML_84(f,__VA_ARGS__))
#define ML_86(f,a,...) f(a) EO_E(ML_85(f,__VA_ARGS__))
#define ML_87(f,a,...) f(a) EO_E(ML_86(f,__VA_ARGS__))
#define ML_88(f,a,...) f(a) EO_E(ML_87(f,__VA_ARGS__))
#define ML_89(f,a,...) f(a) EO_E(ML_88(f,__VA_ARGS__))
#define ML_90(f,a,...) f(a) EO_E(ML_89(f,__VA_ARGS__))
#define ML_91(f,a,...) f(a) EO_E(ML_90(f,__VA_ARGS__))
#define ML_92(f,a,...) f(a) EO_E(ML_91(f,__VA_ARGS__))
#define ML_93(f,a,...) f(a) EO_E(ML_92(f,__VA_ARGS__))
#define ML_94(f,a,...) f(a) EO_E(ML_93(f,__VA_ARGS__))
#define ML_95(f,a,...) f(a) EO_E(ML_94(f,__VA_ARGS__))
#define ML_96(f,a,...) f(a) EO_E(ML_95(f,__VA_ARGS__))
#define ML_97(f,a,...) f(a) EO_E(ML_96(f,__VA_ARGS__))
#define ML_98(f,a,...) f(a) EO_E(ML_97(f,__VA_ARGS__))
#define ML_99(f,a,...) f(a) EO_E(ML_98(f,__VA_ARGS__))
#define ML_100(f,a,...) f(a) EO_E(ML_99(f,__VA_ARGS__))
#define ML_101(f,a,...) f(a) EO_E(ML_100(f,__VA_ARGS__))
#define ML_102(f,a,...) f(a) EO_E(ML_101(f,__VA_ARGS__))
#define ML_103(f,a,...) f(a) EO_E(ML_102(f,__VA_ARGS__))
#define ML_104(f,a,...) f(a) EO_E(ML_103(f,__VA_ARGS__))
#define ML_105(f,a,...) f(a) EO_E(ML_104(f,__VA_ARGS__))
#define ML_106(f,a,...) f(a) EO_E(ML_105(f,__VA_ARGS__))
#define ML_107(f,a,...) f(a) EO_E(ML_106(f,__VA_ARGS__))
#define ML_108(f,a,...) f(a) EO_E(ML_107(f,__VA_ARGS__))
#define ML_109(f,a,...) f(a) EO_E(ML_108(f,__VA_ARGS__))
#define ML_110(f,a,...) f(a) EO_E(ML_109(f,__VA_ARGS__))
#define ML_111(f,a,...) f(a) EO_E(ML_110(f,__VA_ARGS__))
#define ML_112(f,a,...) f(a) EO_E(ML_111(f,__VA_ARGS__))
#define ML_113(f,a,...) f(a) EO_E(ML_112(f,__VA_ARGS__))
#define ML_114(f,a,...) f(a) EO_E(ML_113(f,__VA_ARGS__))
#define ML_115(f,a,...) f(a) EO_E(ML_114(f,__VA_ARGS__))
#define ML_116(f,a,...) f(a) EO_E(ML_115(f,__VA_ARGS__))
#define ML_117(f,a,...) f(a) EO_E(ML_116(f,__VA_ARGS__))
#define ML_118(f,a,...) f(a) EO_E(ML_117(f,__VA_ARGS__))
#define ML_119(f,a,...) f(a) EO_E(ML_118(f,__VA_ARGS__))
#define ML_120(f,a,...) f(a) EO_E(ML_119(f,__VA_ARGS__))
#define ML_121(f,a,...) f(a) EO_E(ML_120(f,__VA_ARGS__))
#define ML_122(f,a,...) f(a) EO_E(ML_121(f,__VA_ARGS__))
#define ML_123(f,a,...) f(a) EO_E(ML_122(f,__VA_ARGS__))
#define ML_124(f,a,...) f(a) EO_E(ML_123(f,__VA_ARGS__))
#define ML_125(f,a,...) f(a) EO_E(ML_124(f,__VA_ARGS__))

/// MacroLoop_ArgumentCounts [ArgMax ... 0]
#define ML_G() 125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,\
95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,\
47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

/// MacroLoop_Select_Argument_At_Argument_Max [a0, ..., a(ArgMax-1), argAtArgMax]
#define ML_M(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,\
d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,g0,g1,g2,g3,g4,g5,g6,g7,g8,g9,h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,\
i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,j0,j1,j2,j3,j4,j5,j6,j7,j8,argAtArgMax,...) argAtArgMax

/// MacroLoop_Expand_Select_Argument_At_Argument_Max (necessary due to bugs in VS macro handling)
#define ML_S(...) EO_E(ML_M(__VA_ARGS__))

/// MacroLoop_ForEach_N
#define ML_N(N,f,...) EO_E(EO_C(ML_,N)(f,__VA_ARGS__))

/// Selects the count of varadic arguments
#define COUNT_ARGUMENTS(...) ML_S(__VA_ARGS__,ML_G())

/// Call "f" for each argument
#define FOR_EACH(f,...) ML_N(COUNT_ARGUMENTS(__VA_ARGS__),f,__VA_ARGS__)

};

/// Contains methods required for manipulating strings at compile time
namespace ConstexprStr
{
    template <size_t N> struct substr {
        constexpr substr(const char* s) : value() {
            for ( size_t i = 0; i < N; i++ )
                value[i] = s[i];

            value[N] = '\0';
        }
        char value[N + 1];
    };
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
    
    template <typename L, typename R>
    struct TypePair {
        using Left = L;
        using Right = R;
    };

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
    private:
        template <typename ...Ts> struct get_impl
        {
            template <size_t Index>
            static constexpr TypeToGet GetElementRecursion(const std::tuple<Ts...> & elements)
            {
                #pragma warning(suppress: 26444) // Warning incorrectly raised for unnamed local variable
                return {};
            }

            template <size_t Index, typename CurrentType, typename... NextTypes>
            static constexpr TypeToGet GetElementRecursion(const std::tuple<Ts...> & elements)
            {
                if constexpr ( std::is_same<TypeToGet, CurrentType>::value )
                    return std::get<Index>(elements);
                else
                    return GetElementRecursion<Index+1, NextTypes...>(elements);
            }

            static constexpr TypeToGet GetElement(const std::tuple<Ts...> & elements)
            {
                return GetElementRecursion<0, Ts...>(elements);
            }
        };

    public:
        template <typename ...Ts> static constexpr TypeToGet from(const std::tuple<Ts...> & elements) {
            return get_impl<Ts...>::GetElement(elements);
        }
    };

    template <typename TypeToGet>
    class for_each
    {
    private:
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
        constexpr TypeName() : value() {
            auto view = getTypeView<T>();
            for ( size_t i=0; i<view.size(); i++ )
                value[i] = view[i];

            value[view.size()] = '\0';
        }
        char value[getTypeView<T>().size()+1];
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
}

/// Contains support for working with reflected fields, the definition for the REFLECT macro and non-generic supporting macros
namespace Reflect
{
    #define NOTE(field, ...) static constexpr auto field##_note { std::tuple { __VA_ARGS__ } };
    using NoAnnotation = std::tuple<>;

    inline namespace Inheritance
    {
        template <typename T, size_t SuperIndex, typename SuperAnnotations>
        struct SuperInfo {
            using Type = T;
            using Annotations = SuperAnnotations;
            const Annotations & annotations;
            static constexpr size_t Index = SuperIndex;

            template <typename Annotation>
            static constexpr bool HasAnnotation = ExtendedTypeSupport::has_type<Annotation, Annotations>::value;

            template <typename Annotation>
            constexpr Annotation getAnnotation() const { return ExtendedTypeSupport::get<Annotation>::from(annotations); }

            template <typename Annotation, typename Function>
            constexpr void forEach(Function function) const { return ExtendedTypeSupport::for_each<Annotation>::in(annotations, function); }

            template <typename Function>
            constexpr void forEachAnnotation(Function function) const { return ExtendedTypeSupport::for_each_in(annotations, function); }
        };

        template <typename T, typename ...Ts>
        struct NotedSuper {
            const std::tuple<Ts...> annotations;
            using Annotations = decltype(annotations);
        };

        template <typename T>
        struct SuperClass {
            constexpr static NoAnnotation annotations{};
            using Annotations = decltype(annotations);

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

        template <size_t Count>
        static constexpr size_t CountSupersRecursion() { return Count; }

        template <size_t Count, typename CurrentType, typename... NextTypes>
        static constexpr size_t CountSupersRecursion()
        {
            if constexpr ( is_super<CurrentType>::value )
                return CountSupersRecursion<1+Count, NextTypes...>();
            else
                return CountSupersRecursion<Count, NextTypes...>();
        }

        template <typename ... Ts> struct count_supers { static constexpr size_t value = CountSupersRecursion<0, Ts...>(); };

        template <typename SubClass, typename ...Ts>
        struct Inherit;

        template <typename SubClass, typename ...Ts>
        struct Inherit<SubClass, const std::tuple<Ts...>>
        {
            template <size_t Index, size_t SuperIndex, typename Function>
            static void ForEachRecursion(const SubClass &, Function function) {} // Base case for recursion

            template <size_t Index, size_t SuperIndex, typename Function, typename CurrentAnnotation, typename ...NextAnnotations>
            static void ForEachRecursion(SubClass & object, Function function)
            {
                if constexpr ( is_super<CurrentAnnotation>::value )
                {
                    using SuperType = typename super_type<CurrentAnnotation>::type;
                    function(SuperInfo<SuperType, SuperIndex, typename CurrentAnnotation::Annotations>{std::get<Index>(SubClass::Class::annotations).annotations}, (SuperType &)object);
                    ForEachRecursion<Index+1, SuperIndex+1, Function, NextAnnotations...>(object, function);
                }
                else
                    ForEachRecursion<Index+1, SuperIndex, Function, NextAnnotations...>(object, function);
            }

            template <size_t Index, size_t SuperIndex, typename Function, typename CurrentAnnotation, typename ...NextAnnotations>
            static void ForEachRecursion(const SubClass & object, Function function)
            {
                if constexpr ( is_super<CurrentAnnotation>::value )
                {
                    using SuperType = typename super_type<CurrentAnnotation>::type;
                    function(SuperInfo<SuperType, SuperIndex, typename CurrentAnnotation::Annotations>{std::get<Index>(SubClass::Class::annotations).annotations}, (SuperType &)object);
                    ForEachRecursion<Index+1, SuperIndex+1, Function, NextAnnotations...>(object, function);
                }
                else
                    ForEachRecursion<Index+1, SuperIndex, Function, NextAnnotations...>(object, function);
            }

            template <typename Function>
            static void ForEach(SubClass & object, Function function) {
                ForEachRecursion<0, 0, Function, Ts...>(object, function);
            }

            template <typename Function>
            static void ForEach(const SubClass & object, Function function) {
                ForEachRecursion<0, 0, Function, Ts...>(object, function);
            }

            template <size_t Index, size_t SuperIndex, typename Function>
            static constexpr void ForEachRecursion(Function function) {} // Base case for recursion
            
            template <size_t Index, size_t SuperIndex, typename Function, typename CurrentAnnotation, typename... NextAnnotations>
            static constexpr void ForEachRecursion(Function function)
            {
                if constexpr ( is_super<CurrentAnnotation>::value )
                {
                    using SuperType = typename super_type<CurrentAnnotation>::type;
                    function(SuperInfo<SuperType, SuperIndex, typename CurrentAnnotation::Annotations>{std::get<Index>(SubClass::Class::annotations).annotations});
                    ForEachRecursion<Index+1, SuperIndex+1, Function, NextAnnotations...>(function);
                }
                else
                    ForEachRecursion<Index+1, SuperIndex, Function, NextAnnotations...>(function);
            }

            template <typename Function>
            static constexpr void ForEach(Function function) {
                ForEachRecursion<0, 0, Function, Ts ...>(function);
            }
            
            template <size_t Index, size_t SuperIndex, typename Function>
            static void AtRecursion(const SubClass & object, size_t superIndex, Function function) {} // Base case for recursion
        
            template <size_t Index, size_t SuperIndex, typename Function, typename CurrentAnnotation, typename... NextAnnotations>
            static void AtRecursion(SubClass & object, size_t superIndex, Function function) {
                if constexpr ( is_super<CurrentAnnotation>::value )
                {
                    if ( SuperIndex == superIndex )
                    {
                        using SuperType = typename super_type<CurrentAnnotation>::type;
                        function(SuperInfo<SuperType, SuperIndex, typename CurrentAnnotation::Annotations>{std::get<Index>(SubClass::Class::annotations).annotations}, (SuperType &)object);
                    }
                    else
                        AtRecursion<Index+1, SuperIndex+1, Function, NextAnnotations...>(object, superIndex, function);
                }
                else
                    AtRecursion<Index+1, SuperIndex, Function, NextAnnotations...>(object, superIndex, function);
            }
        
            template <size_t Index, size_t SuperIndex, typename Function, typename CurrentAnnotation, typename... NextAnnotations>
            static void AtRecursion(const SubClass & object, size_t superIndex, Function function) {
                if constexpr ( is_super<CurrentAnnotation>::value )
                {
                    if ( SuperIndex == superIndex )
                    {
                        using SuperType = typename super_type<CurrentAnnotation>::type;
                        function(SuperInfo<SuperType, SuperIndex, typename CurrentAnnotation::Annotations>{std::get<Index>(SubClass::Class::annotations).annotations}, (SuperType &)object);
                    }
                    else
                        AtRecursion<Index+1, SuperIndex+1, Function, NextAnnotations...>(object, superIndex, function);
                }
                else
                    AtRecursion<Index+1, SuperIndex, Function, NextAnnotations...>(object, superIndex, function);
            }

            template <typename Function>
            static void At(SubClass & object, size_t superIndex, Function function) {
                AtRecursion<0, 0, Function, Ts ...>(object, superIndex, function);
            }

            template <typename Function>
            static void At(const SubClass & object, size_t superIndex, Function function) {
                AtRecursion<0, 0, Function, Ts ...>(object, superIndex, function);
            }
        
            template <size_t Index, size_t SuperIndex, typename Function>
            static constexpr void AtRecursion(size_t superIndex, Function function) {} // Base case for recursion
        
            template <size_t Index, size_t SuperIndex, typename Function, typename CurrentAnnotation, typename... NextAnnotations>
            static constexpr void AtRecursion(size_t superIndex, Function function) {
                if constexpr ( is_super<CurrentAnnotation>::value )
                {
                    if ( SuperIndex == superIndex )
                    {
                        using SuperType = typename super_type<CurrentAnnotation>::type;
                        function(SuperInfo<SuperType, SuperIndex, typename CurrentAnnotation::Annotations>{std::get<Index>(SubClass::Class::annotations).annotations});
                    }
                    else
                        AtRecursion<Index+1, SuperIndex+1, Function, NextAnnotations...>(superIndex, function);
                }
                else
                    AtRecursion<Index+1, SuperIndex, Function, NextAnnotations...>(superIndex, function);
            }

            template <typename Function>
            static constexpr void At(size_t superIndex, Function function) {
                AtRecursion<0, 0, Function, Ts ...>(superIndex, function);
            }

            static constexpr size_t TotalSupers = count_supers<Ts...>::value;
        };

    }

    namespace Fields
    {
        template <typename T = void, typename FieldPointer = std::nullptr_t, size_t FieldIndex = 0, typename Annotations = NoAnnotation>
        struct Field;
    
        template <>
        struct Field<void, std::nullptr_t, 0, NoAnnotation> {
            const char* name;
            const char* typeStr;
        };

        template <typename T, typename FieldPointer, size_t FieldIndex, typename FieldAnnotations>
        struct Field {
            const char* name;
            const char* typeStr;

            using Type = T;
            using Pointer = std::conditional_t<std::is_reference_v<T>, std::nullptr_t, FieldPointer>;
            using Annotations = FieldAnnotations;

            Pointer p;
            const Annotations & annotations;
        
            static constexpr size_t Index = FieldIndex;
            static constexpr bool IsStatic = !std::is_member_pointer<FieldPointer>::value && !std::is_same<std::nullptr_t, FieldPointer>::value;
            static constexpr bool IsFunction = std::is_function_v<T> || std::is_same_v<T, FieldPointer>;

            template <typename Annotation>
            static constexpr bool HasAnnotation = ExtendedTypeSupport::has_type<Annotation, Annotations>::value;

            template <typename Annotation>
            constexpr Annotation getAnnotation() const { return ExtendedTypeSupport::get<Annotation>::from(annotations); }

            template <typename Annotation, typename Function>
            constexpr void forEach(Function function) const { return ExtendedTypeSupport::for_each<Annotation>::in(annotations, function); }

            template <typename Function>
            constexpr void forEachAnnotation(Function function) const { return ExtendedTypeSupport::for_each_in(annotations, function); }
        };
    }

#define GET_FIELD_NAME(x) x,

#ifdef __clang__
#define CLANG_ONLY(x) template <typename T, typename F, typename = decltype(T::x)> static constexpr void useInst(F f, T & t) { f(field, t.x); } \
    template <typename T, typename F, typename = decltype(T::x)> static constexpr void useInst(F f, const T & t) { f(field, t.x); } \
    template <typename T, typename F> static constexpr inline void useInst(F f, ...) { }
#define USE_FIELD_VALUE(x) if constexpr ( !x##_::Field::IsFunction ) x##_::useInst<ClassType>(function, object);
#define USE_FIELD_VALUE_AT(x) case IndexOf::x: if constexpr ( !x##_::Field::IsFunction ) x##_::useInst<ClassType>(function, object); break;
#else
#define CLANG_ONLY(x)
#define USE_FIELD_VALUE(x) if constexpr ( !x##_::Field::IsFunction ) function(x##_::field, object.x);
#define USE_FIELD_VALUE_AT(x) case IndexOf::x: if constexpr ( !x##_::Field::IsFunction ) function(x##_::field, object.x); break;
#endif

#define DESCRIBE_FIELD(x) struct x##_ { \
    template <typename T> static constexpr ExtendedTypeSupport::TypePair<decltype(T::x), decltype(&T::x)> identify(int); \
    template <typename T> static constexpr ExtendedTypeSupport::TypePair<decltype(T::x), std::nullptr_t> identify(unsigned int); \
    template <typename T> static constexpr ExtendedTypeSupport::TypePair<decltype(&T::x), decltype(&T::x)> identify(...); \
    using Type = decltype(identify<ClassType>(0))::Left; \
    using Pointer = decltype(identify<ClassType>(0))::Right; \
    template <typename T, bool IsReference> struct GetPointer { static constexpr auto value = &T::x; }; \
    template <typename T> struct GetPointer<T, true> { static constexpr std::nullptr_t value = nullptr; }; \
    static constexpr auto nameStr = ConstexprStr::substr<std::string_view(#x).size()>(&#x[0]); \
    static constexpr auto typeStr = ExtendedTypeSupport::TypeName<Type>(); \
    template <typename T> static constexpr decltype(T::x##_note) idNote(int); \
    template <typename T> static constexpr decltype(Class::NoNote) idNote(...); \
    template <typename T, bool NoNote> struct GetNote { static constexpr auto & value = Class::NoNote; }; \
    template <typename T> struct GetNote<T, false> { static constexpr auto & value = T::x##_note; }; \
    using NoteType = decltype(idNote<ClassType>(0)); \
    using Field = Reflect::Fields::Field<Type, Pointer, IndexOf::x, NoteType>; \
    static constexpr Field field = { &nameStr.value[0], &typeStr.value[0], GetPointer<ClassType, std::is_reference_v<Type>>::value, \
        GetNote<ClassType, std::is_same_v<decltype(Class::NoNote), NoteType>>::value }; \
    CLANG_ONLY(x) \
};

#define ADD_IF_STATIC(x) + ( x##_::Field::IsStatic ? 1 : 0 )
#define GET_FIELD(x) { Class::x##_::field.name, Class::x##_::field.typeStr },
#define USE_FIELD(x) function(x##_::field);
#define USE_FIELD_AT(x) case IndexOf::x: function(x##_::field); break;


#pragma warning(disable: 4003) // Not enough arguments warning generated despite macros working perfectly

/// After the objectType there needs to be at least 1 and at most 125 field names
/// e.g. REFLECT(MyObj, myInt, myString, myOtherObj)
#define REFLECT(objectType, ...) \
struct Class { \
    using ClassType = objectType; \
    enum_t(IndexOf, size_t, { FOR_EACH(GET_FIELD_NAME, __VA_ARGS__) }); \
    static constexpr Reflect::NoAnnotation NoNote {}; \
    using Annotations = decltype(NoNote); \
    static constexpr Annotations & annotations = NoNote; \
    FOR_EACH(DESCRIBE_FIELD, __VA_ARGS__) \
    static constexpr size_t TotalFields = COUNT_ARGUMENTS(__VA_ARGS__); \
    static constexpr size_t TotalStaticFields = 0 FOR_EACH(ADD_IF_STATIC, __VA_ARGS__); \
    static constexpr Reflect::Fields::Field<> Fields[TotalFields] = { FOR_EACH(GET_FIELD, __VA_ARGS__) }; \
    template <typename Function> constexpr static void ForEachField(Function function) { FOR_EACH(USE_FIELD, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(objectType & object, Function function) { FOR_EACH(USE_FIELD_VALUE, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(const objectType & object, Function function) { FOR_EACH(USE_FIELD_VALUE, __VA_ARGS__) } \
    template <typename Function> constexpr static void FieldAt(size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_AT, __VA_ARGS__) } } \
    template <typename Function> static void FieldAt(objectType & object, size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_VALUE_AT, __VA_ARGS__) } } \
}; \
using Supers = Reflect::Inherit<Class::ClassType, Class::Annotations>;


/// REFLECT_NOTED is exactly the same as REFLECT except this signals that objectType itself is annotated
#define REFLECT_NOTED(objectType, ...) \
struct Class { \
    using ClassType = objectType; \
    enum_t(IndexOf, size_t, { FOR_EACH(GET_FIELD_NAME, __VA_ARGS__) }); \
    static constexpr Reflect::NoAnnotation NoNote {}; \
    using Annotations = decltype(objectType##_note); \
    static constexpr Annotations & annotations = objectType##_note; \
    FOR_EACH(DESCRIBE_FIELD, __VA_ARGS__) \
    static constexpr size_t TotalFields = COUNT_ARGUMENTS(__VA_ARGS__); \
    static constexpr size_t TotalStaticFields = 0 FOR_EACH(ADD_IF_STATIC, __VA_ARGS__); \
    static constexpr Reflect::Fields::Field<> Fields[TotalFields] = { FOR_EACH(GET_FIELD, __VA_ARGS__) }; \
    template <typename Function> constexpr static void ForEachField(Function function) { FOR_EACH(USE_FIELD, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(objectType & object, Function function) { FOR_EACH(USE_FIELD_VALUE, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(const objectType & object, Function function) { FOR_EACH(USE_FIELD_VALUE, __VA_ARGS__) } \
    template <typename Function> constexpr static void FieldAt(size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_AT, __VA_ARGS__) } } \
    template <typename Function> static void FieldAt(objectType & object, size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_VALUE_AT, __VA_ARGS__) } } \
}; \
using Supers = Reflect::Inherit<Class::ClassType, Class::Annotations>;


/// REFLECT_EMPTY is used to reflect an annotated class with no reflected fields
#define REFLECT_EMPTY(objectType) \
struct Class { \
    using ClassType = objectType; \
    using Annotations = decltype(objectType##_note); \
    static constexpr Annotations & annotations = objectType##_note; \
    static constexpr size_t TotalFields = 0; \
    static constexpr size_t TotalStaticFields = 0; \
    static constexpr Reflect::Fields::Field<> Fields[1] = { { "", "" } }; \
    template <typename Function> constexpr static void ForEachField(Function function) {} \
    template <typename Function> static void ForEachField(objectType & object, Function function) {} \
    template <typename Function> static void ForEachField(const objectType & object, Function function) { } \
    template <typename Function> constexpr static void FieldAt(size_t fieldIndex, Function function) {} \
    template <typename Function> static void FieldAt(objectType & object, size_t fieldIndex, Function function) {} \
}; \
using Supers = Reflect::Inherit<Class::ClassType, Class::Annotations>;


    template <typename T, typename = decltype(T::Class::TotalFields)> static constexpr std::true_type typeHasReflection(int);
    template <typename T> static constexpr std::false_type typeHasReflection(unsigned int);
    
    template <typename T> struct is_reflected { static constexpr bool value = decltype(typeHasReflection<T>(0))::value; };
    template <typename T> struct is_reflected<const T> { static constexpr bool value = is_reflected<T>::value; };
}

#endif
