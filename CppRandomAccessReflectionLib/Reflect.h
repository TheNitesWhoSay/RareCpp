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

/// Contains everything neccessary to extract "lhs" and "rhs" from partially parameterized arguments which have the form "(lhs) rhs"
/// As well as everything neccessary to loop over varadic macro arguments
namespace ExtractorsAndMacroLoops
{
/// ArgMax: 125 (derived from the C spec limiting macros to 126 arguments and the COUNT_ARGUMENTS helper macro "ML_M" requiring ArgMax+1 arguments)

/// Extractor_Expand
#define EO_E(x) x

/// Extractor_Concatenate
#define EO_C(x,y) x##y

/// Extractor_Blank (when you append "EO_BLANK" with an expression of the form "(LHS) RHS", the "(LHS)" gets blanked out and you're left with just RHS
#define EO_B(...)

// If x takes the form "(LHS) RHS", then this macro returns RHS
#define RHS(x) EO_B x

#ifdef _MSC_VER // Visual studios or possibly clang

#ifndef __clang__ // Not clang

/// Extractor_First
#define EO_F(x, y) x

/// Extractor_WrapAndAppendComma
#define EO_W(x) (x),

/// Extractor_GetFirst
#define EO_G(x) EO_C(EO_F, x)

// If x takes the form "(LHS) RHS", then this macro returns LHS
#define LHS(x) EO_G((EO_E EO_W x))

#else // Clang via visual studios

/// Extractor_First
#define EO_F(x, ...) EO_E x

/// Extractor_WrapAndAppendComma
#define EO_W(x) (x),

/// Extractor_GetFirst
#define EO_G(x, y) EO_F(x, y)

// If x takes the form "(LHS) RHS", then this macro returns LHS
#define LHS(x) EO_G(EO_W x,)

#endif
#else // Not visual studios

/// Extractor_First
#define EO_F(x, ...) EO_E x

/// Extractor_WrapAndAppendComma
#define EO_W(x) (x),

/// Extractor_GetFirst
#define EO_G(x, y) EO_F(x, y)

// If x takes the form "(LHS) RHS", then this macro returns LHS
#define LHS(x) EO_G(EO_W x,)

#endif

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

    template <size_t N> constexpr size_t length_after_last(const char(&s)[N], const char character)
    {
        size_t pos = N-1;
        for ( ; pos < N && s[pos] != character; pos-- );
        return pos > N ? 0 : N-pos-2;
    }

    template <size_t N> constexpr size_t find_last_of(const char(&s)[N], const char character)
    {
        size_t pos = N-1;
        for ( ; pos < N && s[pos] != character; pos-- );
        return pos;
    }
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

    template <typename T> struct pair_rhs { using type = T; };
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
    
    template <typename T> struct remove_pointer { using type = typename std::remove_pointer<T>::type; };
    template <typename T> struct remove_pointer<const T> { using type = const typename remove_pointer<T>::type; };
    template <typename T> struct remove_pointer<std::unique_ptr<T>> { using type = T; };
    template <typename T> struct remove_pointer<std::shared_ptr<T>> { using type = T; };
    
    template <typename T> struct is_pointable { static constexpr bool value = std::is_pointer<T>::value; };
    template <typename T> struct is_pointable<const T> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T> struct is_pointable<std::unique_ptr<T>> { static constexpr bool value = true; };
    template <typename T> struct is_pointable<std::shared_ptr<T>> { static constexpr bool value = true; };

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

    template <typename T, typename ... Ts>
    struct HasType { static constexpr bool value = HasTypeRecursion<T, Ts...>(); };

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
    inline namespace Inheritance
    {
        template <size_t index>
        struct SuperIndex
        {
            static constexpr size_t Index = index;
        };

        template <typename T>
        struct SuperType
        {
            using type = T;
        };

        /// Inherit "inherit-from": used to denote a set of classes whose properties are being inherited by another reflected class
        template <typename ... Ts>
        struct Inherit;

        template <>
        struct Inherit<> {
        
            static constexpr size_t TotalSupers = 0;
        
            template <typename Function, typename SubClass>
            static void ForEach(SubClass & object, Function function) {}

            template <typename Function>
            static constexpr void ForEach(Function function) {}

            template <typename Function, typename SubClass>
            static void At(SubClass & object, size_t superIndex, Function function) {}

            template <typename Function>
            static constexpr void At(size_t superIndex, Function function) {}
        };

        template <>
        struct Inherit<Inherit<>> {
        
            static constexpr size_t TotalSupers = 0;

            template <typename Function, typename SubClass>
            static void ForEach(SubClass & object, Function function) {}

            template <typename Function>
            static constexpr void ForEach(Function function) {}

            template <typename Function, typename SubClass>
            static void At(SubClass & object, size_t superIndex, Function function) {}

            template <typename Function>
            static constexpr void At(size_t superIndex, Function function) {}
        };

        template <typename T>
        struct Inherit<T> {
        
            static constexpr size_t TotalSupers = 1;

            template <typename Function, typename SubClass>
            static void ForEach(SubClass & object, Function function) {
                function(SuperIndex<0>(), (T &)object);
            }

            template <typename Function>
            static constexpr void ForEach(Function function) {
                function(SuperIndex<0>(), SuperType<T>());
            }

            template <typename Function, typename SubClass>
            static void At(SubClass & object, size_t superIndex, Function function) {
                if ( superIndex == 0 )
                    function((T &)object);
            }

            template <typename Function>
            static constexpr void At(size_t superIndex, Function function) {
                if ( superIndex == 0 )
                    function(SuperType<T>());
            }
        };

        template <typename T>
        struct Inherit<Inherit<T>> {
        
            static constexpr size_t TotalSupers = 1;

            template <typename Function, typename SubClass>
            static void ForEach(SubClass & object, Function function) {
                function(SuperIndex<0>(), (T &)object);
            }

            template <typename Function>
            static constexpr void ForEach(Function function) {
                function(SuperIndex<0>(), SuperType<T>());
            }

            template <typename Function, typename SubClass>
            static void At(SubClass & object, size_t superIndex, Function function) {
                if ( superIndex == 0 )
                    function((T &)object);
            }

            template <typename Function>
            static constexpr void At(size_t superIndex, Function function) {
                if ( superIndex == 0 )
                    function(SuperType<T>());
            }
        };

        template <typename ... Ts>
        struct Inherit<Inherit<Ts ...>> {

            static constexpr size_t TotalSupers = sizeof...(Ts);

            template <size_t Index, typename Function, typename SubClass>
            static void ForEachRecursion(SubClass &, Function function) {
                // Base case for recursion
            }

            template <size_t Index, typename Function, typename SubClass, typename CurrentSuperClassType, typename... NextSuperClassTypes>
            static void ForEachRecursion(SubClass & object, Function function) {
                function(SuperIndex<Index>(), (CurrentSuperClassType &)object);
                ForEachRecursion<Index+1, Function, SubClass, NextSuperClassTypes...>(object, function);
            }

            template <typename Function, typename SubClass>
            static void ForEach(SubClass & object, Function function) {
                ForEachRecursion<0, Function, SubClass, Ts ...>(object, function);
            }

            template <size_t Index, typename Function>
            static constexpr void ForEachRecursion(Function function) {
                // Base case for recursion
            }

            template <size_t Index, typename Function, typename CurrentSuperClassType, typename... NextSuperClassTypes>
            static constexpr void ForEachRecursion(Function function) {
                function(SuperIndex<Index>(), SuperType<CurrentSuperClassType>());
                ForEachRecursion<Index+1, Function, NextSuperClassTypes...>(function);
            }

            template <typename Function>
            static constexpr void ForEach(Function function) {
                ForEachRecursion<0, Function, Ts ...>(function);
            }
        
            template <size_t Index, typename Function, typename SubClass>
            static void AtRecursion(SubClass & object, size_t superIndex, Function function) {
                // Base case for recursion
            }
        
            template <size_t Index, typename Function, typename SubClass, typename CurrentSuperClassType, typename... NextSuperClassTypes>
            static void AtRecursion(SubClass & object, size_t superIndex, Function function) {
                if ( Index == superIndex )
                    function((CurrentSuperClassType &)object);

                AtRecursion<Index+1, Function, SubClass, NextSuperClassTypes...>(object, superIndex, function);
            }

            template <typename Function, typename SubClass>
            static void At(SubClass & object, size_t superIndex, Function function) {
                AtRecursion<0, Function, SubClass, Ts ...>(object, superIndex, function);
            }
        
            template <size_t Index, typename Function>
            static constexpr void AtRecursion(size_t superIndex, Function function) {
                // Base case for recursion
            }
        
            template <size_t Index, typename Function, typename CurrentSuperClassType, typename... NextSuperClassTypes>
            static constexpr void AtRecursion(size_t superIndex, Function function) {
                if ( Index == superIndex )
                    function(SuperType<CurrentSuperClassType>());

                AtRecursion<Index+1, Function, NextSuperClassTypes...>(superIndex, function);
            }

            template <typename Function>
            static constexpr void At(size_t superIndex, Function function) {
                AtRecursion<0, Function, Ts ...>(superIndex, function);
            }
        };
    }
    
    inline namespace Annotation
    {
        template <typename ... Ts>
        struct Annotate {
            using Annotations = Annotate<Ts...>;

            template <typename T>
            static constexpr bool Has = ExtendedTypeSupport::HasType<T, Ts...>::value;
        };

        template <typename ... Ts>
        struct Annotate<Annotate<Ts ...>> {
            using Annotations = Annotate<Ts...>;

            template <typename T>
            static constexpr bool Has = ExtendedTypeSupport::HasType<T, Ts...>::value;
        };
    }

    namespace Fields
    {
        template <typename T = void, typename FieldPointer = void*, size_t FieldIndex = 0, typename Annotations = Annotate<>>
        struct Field;
    
        template <>
        struct Field<void, void*, 0, void> {
            const char* name;
            const char* typeStr;
            size_t arraySize;
            bool isIterable;
            bool isReflected;
        };

        template <typename T, typename FieldPointer, size_t FieldIndex, typename Annotations>
        struct Field {
            const char* name;
            const char* typeStr;
            size_t arraySize;
            bool isIterable;
            bool isReflected;

            using Type = T;
            using Pointer = std::conditional_t<std::is_reference_v<T>, void*, FieldPointer>;

            Pointer p;
        
            static constexpr size_t Index = FieldIndex;
            static constexpr bool IsStatic = !std::is_member_pointer<FieldPointer>::value && !std::is_same<void*, FieldPointer>::value;

            template <typename Annotation>
            static constexpr bool HasAnnotation = Annotate<Annotations>::template Has<Annotation>;
        };
    }

    /// The "Reflected" annotation denotes whether a given field is a type that is also reflected
    struct Reflected {};

#define ALIAS_TYPE(x) using RHS(x) = decltype(RHS(x));
#define GET_FIELD_NAME(x) RHS(x),
#define DESCRIBE_FIELD(x) struct RHS(x##_) { \
    static constexpr auto nameStr = ConstexprStr::substr<ConstexprStr::length_after_last(#x, ' ')>(&#x[0]+ConstexprStr::find_last_of(#x, ' ')+1); \
    static constexpr auto typeStr = ExtendedTypeSupport::TypeName<RHS(x)>(); \
    template <typename T> static constexpr decltype(&T::RHS(x)) GetPointerType(int); \
    template <typename T> static constexpr void* GetPointerType(...); \
    using Pointer = decltype(GetPointerType<ClassType>(0)); \
    using Field = Fields::Field<Class::RHS(x), Pointer, IndexOf::RHS(x), Annotate<LHS(x)>::Annotations>; \
    template <typename T, bool IsReference> struct GetPointer { static constexpr Field::Pointer value = &T::RHS(x); }; \
    template <typename T> struct GetPointer<T, true> { static constexpr Field::Pointer value = nullptr; }; \
    static constexpr Field field = { \
        &nameStr.value[0], &typeStr.value[0], ExtendedTypeSupport::static_array_size<RHS(x)>::value, \
        ExtendedTypeSupport::is_stl_iterable<ExtendedTypeSupport::remove_pointer<RHS(x)>::type>::value || \
            ExtendedTypeSupport::is_adaptor<ExtendedTypeSupport::remove_pointer<RHS(x)>::type>::value || \
            std::is_array<ExtendedTypeSupport::remove_pointer<RHS(x)>::type>::value, \
        Annotate<LHS(x)>::template Has<Reflected>, GetPointer<ClassType, std::is_reference_v<Class::RHS(x)>>::value }; \
};
#define GET_FIELD(x) { Class::RHS(x##_)::field.name, Class::RHS(x##_)::field.typeStr, Class::RHS(x##_)::field.arraySize, \
    Class::RHS(x##_)::field.isIterable, Class::RHS(x##_)::field.isReflected },
#define USE_FIELD(x) function(RHS(x##_)::field);
#define USE_FIELD_VALUE(x) function(RHS(x##_)::field, object.RHS(x));
#define USE_FIELD_AT(x) case IndexOf::RHS(x): function(RHS(x##_)::field); break;
#define USE_FIELD_VALUE_AT(x) case IndexOf::RHS(x): function(RHS(x##_)::field, object.RHS(x)); break;
#define ADD_IF_STATIC(x) + ( RHS(x##_)::Field::IsStatic ? 1 : 0 )


#pragma warning(disable: 4003) // Not enough arguments warning generated despite macros working perfectly

/// After the objectType there needs to be at least 1 and at most 123 fields, in the form "() fieldName" or "(Annotation) fieldName"
/// e.g. REFLECT(() myObj, () myInt, () myString, (Reflected) myOtherObj)
#define REFLECT(objectType, ...) \
struct Class { \
    using ClassType = RHS(objectType); \
    enum_t(IndexOf, size_t, { FOR_EACH(GET_FIELD_NAME, __VA_ARGS__) }); \
    FOR_EACH(ALIAS_TYPE, __VA_ARGS__) \
    FOR_EACH(DESCRIBE_FIELD, __VA_ARGS__) \
    static constexpr size_t TotalFields = COUNT_ARGUMENTS(__VA_ARGS__); \
    static constexpr size_t TotalStaticFields = 0 FOR_EACH(ADD_IF_STATIC, __VA_ARGS__); \
    static constexpr Fields::Field<> Fields[TotalFields] = { FOR_EACH(GET_FIELD, __VA_ARGS__) }; \
    template <typename Function> constexpr static void ForEachField(Function function) { FOR_EACH(USE_FIELD, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(RHS(objectType) & object, Function function) { FOR_EACH(USE_FIELD_VALUE, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(const RHS(objectType) & object, Function function) { FOR_EACH(USE_FIELD_VALUE, __VA_ARGS__) } \
    template <typename Function> static void FieldAt(RHS(objectType) & object, size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_VALUE_AT, __VA_ARGS__) } } \
    template <typename Function> constexpr static void FieldAt(size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_AT, __VA_ARGS__) } } \
}; \
using Supers = Inherit<LHS(objectType)>;

/// Used to reflect a class with no fields
#define REFLECT_EMPTY(objectType) \
struct Class { \
    using ClassType = RHS(objectType); \
    static constexpr size_t TotalFields = 0; \
    static constexpr size_t TotalStaticFields = 0; \
    static constexpr Fields::Field<> Fields[1] = { { "", "", 0, false, false } }; \
    template <typename Function> constexpr static void ForEachField(Function function) {} \
    template <typename Function> static void ForEachField(RHS(objectType) & object, Function function) {} \
    template <typename Function> static void ForEachField(const RHS(objectType) & object, Function function) { } \
    template <typename Function> static void FieldAt(RHS(objectType) & object, size_t fieldIndex, Function function) {} \
    template <typename Function> constexpr static void FieldAt(size_t fieldIndex, Function function) {} \
}; \
using Supers = Inherit<LHS(objectType)>;

}

#endif
