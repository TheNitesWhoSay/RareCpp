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

/// Extractor_First
#define EO_F(x, y) x

/// Extractor_WrapAndAppendComma
#define EO_W(x) (x),

/// Extractor_GetFirst
#define EO_G(x) EO_C(EO_F, x)

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

// If x takes the form "(LHS) RHS", then this macro returns LHS
#define LHS(x) EO_G((EO_E EO_W x))

// If x takes the form "(LHS) RHS", then this macro returns RHS
#define RHS(x) EO_B x

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

    template <size_t N> constexpr size_t length_between(const char(&s)[N], const char openChar, const char closeChar)
    {
        size_t openPos = 0, closePos = 0;
        for ( ; openPos < N && s[openPos] != openChar ; openPos++ );
        for ( closePos = N-1; closePos > openPos && s[closePos] != closeChar; closePos-- );
        return openPos == N || closePos == openPos ? 0 : closePos - openPos - 1;
    }

    template <size_t N> constexpr size_t length_after_last(const char(&s)[N], const char character)
    {
        size_t pos = N-1;
        for ( ; pos < N && s[pos] != character; pos-- );
        return pos > N ? 0 : N-pos-2;
    }

    template <size_t N> constexpr size_t find(const char(&s)[N], const char character)
    {
        size_t pos = 0;
        for ( ; pos < N && s[pos] != character; pos++ );
        return pos;
    }

    template <size_t N> constexpr size_t find_last_of(const char(&s)[N], const char character)
    {
        size_t pos = N-1;
        for ( ; pos < N && s[pos] != character; pos-- );
        return pos;
    }
};

/// "ReflectionSupport" Contains classes used in Reflect but not meant to be directly referenced outside Reflect
namespace RfS
{
    template <typename T>
    struct is_pointable { static constexpr bool value = std::is_pointer<T>::value; };
    template <typename T>
    struct is_pointable<std::shared_ptr<T>> { static constexpr bool value = true; };
    template <typename T>
    struct is_pointable<std::unique_ptr<T>> { static constexpr bool value = true; };

    template <typename T> struct is_stl_iterable { static constexpr bool value = false; };
    template <typename T, size_t N> struct is_stl_iterable<std::array<T, N>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_stl_iterable<std::vector<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_stl_iterable<std::deque<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_stl_iterable<std::forward_list<T, A>> { static constexpr bool value = true; };
    template <typename T, typename A> struct is_stl_iterable<std::list<T, A>> { static constexpr bool value = true; };
    template <typename K, typename C, typename A> struct is_stl_iterable<std::set<K, C, A>> { static constexpr bool value = true; };
    template <typename K, typename C, typename A> struct is_stl_iterable<std::multiset<K, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct is_stl_iterable<std::map<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A> struct is_stl_iterable<std::multimap<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename H, typename E, typename A> struct is_stl_iterable<std::unordered_set<K, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename H, typename E, typename A> struct is_stl_iterable<std::unordered_multiset<K, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct is_stl_iterable<std::unordered_map<K, T, H, E, A>>
    { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A> struct is_stl_iterable<std::unordered_multimap<K, T, H, E, A>>
    { static constexpr bool value = true; };

    template <typename T> struct is_adaptor { static constexpr bool value = false; };
    template <typename T, typename C> struct is_adaptor<std::stack<T, C>> { static constexpr bool value = true; };
    template <typename T, typename C> struct is_adaptor<std::queue<T, C>> { static constexpr bool value = true; };
    template <typename T, typename C, typename P> struct is_adaptor<std::priority_queue<T, C, P>> { static constexpr bool value = true; };

    template <typename T> struct contains_pointables { static constexpr bool value = false; };
    template <typename T, size_t N> struct contains_pointables<T[N]> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T, size_t N> struct contains_pointables<std::array<T, N>> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T, typename A> struct contains_pointables<std::vector<T, A>> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T, typename A> struct contains_pointables<std::deque<T, A>> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T, typename A> struct contains_pointables<std::forward_list<T, A>> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T, typename A> struct contains_pointables<std::list<T, A>> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T, typename C> struct contains_pointables<std::stack<T, C>> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T, typename C> struct contains_pointables<std::queue<T, C>> { static constexpr bool value = is_pointable<T>::value; };
    template <typename T, typename C, typename P> struct contains_pointables<std::priority_queue<T, C, P>> { static constexpr bool value = is_pointable<T>::value; };
    template <typename K, typename C, typename A> struct contains_pointables<std::set<K, C, A>> { static constexpr bool value = is_pointable<K>::value; };
    template <typename K, typename C, typename A> struct contains_pointables<std::multiset<K, C, A>> { static constexpr bool value = is_pointable<K>::value; };
    template <typename K, typename T, typename C, typename A> struct contains_pointables<std::map<K, T, C, A>>
    { static constexpr bool value = is_pointable<T>::value; };
    template <typename K, typename T, typename C, typename A> struct contains_pointables<std::multimap<K, T, C, A>>
    { static constexpr bool value = is_pointable<T>::value; };
    template <typename K, typename H, typename E, typename A> struct contains_pointables<std::unordered_set<K, H, E, A>>
    { static constexpr bool value = is_pointable<K>::value; };
    template <typename K, typename H, typename E, typename A> struct contains_pointables<std::unordered_multiset<K, H, E, A>>
    { static constexpr bool value = is_pointable<K>::value; };
    template <typename K, typename T, typename H, typename E, typename A> struct contains_pointables<std::unordered_map<K, T, H, E, A>>
    { static constexpr bool value = is_pointable<T>::value; };
    template <typename K, typename T, typename H, typename E, typename A> struct contains_pointables<std::unordered_multimap<K, T, H, E, A>>
    { static constexpr bool value = is_pointable<T>::value; };
    
    template <typename T> struct element_type { using type = void; };
    template <typename T, size_t N> struct element_type<T[N]> { using type = typename T; };
    template <typename T, size_t N> struct element_type<std::array<T, N>> { using type = typename T; };
    template <typename T, typename A> struct element_type<std::vector<T, A>> { using type = typename T; };
    template <typename T, typename A> struct element_type<std::deque<T, A>> { using type = typename T; };
    template <typename T, typename A> struct element_type<std::forward_list<T, A>> { using type = typename T; };
    template <typename T, typename A> struct element_type<std::list<T, A>> { using type = typename T; };
    template <typename T, typename C> struct element_type<std::stack<T, C>> { using type = typename T; };
    template <typename T, typename C> struct element_type<std::queue<T, C>> { using type = typename T; };
    template <typename T, typename C, typename P> struct element_type<std::priority_queue<T, C, P>> { using type = typename T; };
    template <typename K, typename C, typename A> struct element_type<std::set<K, C, A>> { using type = typename K; };
    template <typename K, typename C, typename A> struct element_type<std::multiset<K, C, A>> { using type = typename K; };
    template <typename K, typename T, typename C, typename A> struct element_type<std::map<K, T, C, A>> { using type = typename T; };
    template <typename K, typename T, typename C, typename A> struct element_type<std::multimap<K, T, C, A>> { using type = typename T; };
    template <typename K, typename H, typename E, typename A> struct element_type<std::unordered_set<K, H, E, A>> { using type = typename K; };
    template <typename K, typename H, typename E, typename A> struct element_type<std::unordered_multiset<K, H, E, A>> { using type = typename K; };
    template <typename K, typename T, typename H, typename E, typename A> struct element_type<std::unordered_map<K, T, H, E, A>> { using type = typename T; };
    template <typename K, typename T, typename H, typename E, typename A> struct element_type<std::unordered_multimap<K, T, H, E, A>> { using type = typename T; };

    template <typename T> struct key_type { using type = void; };
    template <typename K, typename C, typename A> struct key_type<std::set<K, C, A>> { using type = typename K; };
    template <typename K, typename C, typename A> struct key_type<std::multiset<K, C, A>> { using type = typename K; };
    template <typename K, typename T, typename C, typename A> struct key_type<std::map<K, T, C, A>> { using type = typename K; };
    template <typename K, typename T, typename C, typename A> struct key_type<std::multimap<K, T, C, A>> { using type = typename K; };
    template <typename K, typename H, typename E, typename A> struct key_type<std::unordered_set<K, H, E, A>> { using type = typename K; };
    template <typename K, typename H, typename E, typename A> struct key_type<std::unordered_multiset<K, H, E, A>> { using type = typename K; };
    template <typename K, typename T, typename H, typename E, typename A> struct key_type<std::unordered_map<K, T, H, E, A>> { using type = typename K; };
    template <typename K, typename T, typename H, typename E, typename A> struct key_type<std::unordered_multimap<K, T, H, E, A>> { using type = typename K; };

    template <typename T> struct contains_pairs { static constexpr bool value = false; };
    template <typename L, typename R, size_t N>
    struct contains_pairs<std::array<std::pair<L, R>, N>> { static constexpr bool value = true; };
    template <typename L, typename R, typename A>
    struct contains_pairs<std::vector<std::pair<L, R>, A>> { static constexpr bool value = true; };
    template <typename L, typename R, typename A>
    struct contains_pairs<std::deque<std::pair<L, R>, A>> { static constexpr bool value = true; };
    template <typename L, typename R, typename A>
    struct contains_pairs<std::forward_list<std::pair<L, R>, A>> { static constexpr bool value = true; };
    template <typename L, typename R, typename A>
    struct contains_pairs<std::list<std::pair<L, R>, A>> { static constexpr bool value = true; };
    template <typename L, typename R, typename C, typename A>
    struct contains_pairs<std::set<std::pair<L, R>, C, A>> { static constexpr bool value = true; };
    template <typename L, typename R, typename C, typename A>
    struct contains_pairs<std::multiset<std::pair<L, R>, C, A>> { static constexpr bool value = true; };
    template <typename L, typename R, typename H, typename E, typename A>
    struct contains_pairs<std::unordered_set<std::pair<L, R>, H, E, A>> { static constexpr bool value = true; };
    template <typename L, typename R, typename H, typename E, typename A>
    struct contains_pairs<std::unordered_multiset<std::pair<L, R>, H, E, A>> { static constexpr bool value = true; };

    // Maps are always formed using pairs
    template <typename K, typename T, typename H, typename E, typename A>
    struct contains_pairs<std::unordered_map<K, T, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A>
    struct contains_pairs<std::unordered_multimap<K, T, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A>
    struct contains_pairs<std::map<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A>
    struct contains_pairs<std::multimap<K, T, C, A>> { static constexpr bool value = true; };

    template <class Adaptor>
    typename Adaptor::container_type & get_underlying_container(Adaptor & adaptor) {
        struct AdaptorSubClass : Adaptor {
            static typename Adaptor::container_type & get(Adaptor & adaptor) {
                return adaptor.*(&AdaptorSubClass::c);
            }
        };
        return AdaptorSubClass::get(adaptor);
    }

    /// ConditionalCall uses the template parameter "isCallable" and partial template specialization
    /// to avoid compile-time generation of bad code (e.g. array access on non-array fields)
    template <bool isCallable, typename Function> class ConditionalCall;

    /// Calls function(0)
    template <typename Function> class ConditionalCall<true, Function> { public:
        /// Calls function(0), at least one parameter (0) is neccessary to prevent generation of cases that did not meet the condition
        constexpr static void call(Function function) { function(0); }
    };

    /// Does nothing
    template <typename Function> class ConditionalCall<false, Function> { public:
        /// Does nothing
        constexpr static void call(Function function) {}
    };

    template <bool templated, bool fieldIsReflected, bool fieldIsPointer, bool fieldIsArray, bool fieldIsStlIterable, bool fieldIsStlAdaptor,
        bool fieldContainsPointers, bool fieldContainsPairs, typename T>
    class Field;
    
    template <typename T>
    class Field<false, false, false, false, false, false, false, false, T> {
    public:
        size_t index;
        const char* name;
        const char* typeStr;
        size_t arraySize;
        bool isIterable;
        bool containsPairs;
        bool isReflected;
    };

    template <bool fieldIsReflected, bool fieldIsPointer, bool fieldIsArray, bool fieldIsStlIterable, bool fieldIsStlAdaptor,
        bool fieldContainsPointers, bool fieldContainsPairs, typename T>
    class Field<true, fieldIsReflected, fieldIsPointer, fieldIsArray, fieldIsStlIterable, fieldIsStlAdaptor, fieldContainsPointers, fieldContainsPairs, T> {
    public:
        size_t index;
        const char* name;
        const char* typeStr;
        size_t arraySize;
        bool isIterable;
        bool containsPairs;
        bool isReflected;
        
        using type = T;
        using element_type = typename element_type<T>::type;
        using key_type = typename key_type<T>::type;

        typename const Field<true, fieldIsReflected, fieldIsPointer, fieldIsArray, fieldIsStlIterable, fieldIsStlAdaptor,
            fieldContainsPointers, fieldContainsPairs, T> & as_const() { return *this; }


        /// Lambda guards: the call to function will only be generated by the compiler if the type fits the guard category
		template <typename Function> static constexpr void IfPointer(Function function) { ConditionalCall<fieldIsPointer, Function>::call(function); }

        template <typename Function> static constexpr void IfPrimitive(Function function)
            { ConditionalCall<!fieldIsArray && !fieldIsStlIterable && !fieldIsStlAdaptor && !fieldIsReflected && !fieldContainsPairs && !fieldIsPointer, Function>::call(function); }
        template <typename Function> static constexpr void IfPrimitivePointer(Function function)
            { ConditionalCall<!fieldIsArray && !fieldIsStlIterable && !fieldIsStlAdaptor && !fieldIsReflected && !fieldContainsPairs && fieldIsPointer, Function>::call(function); }
        
        template <typename Function> static constexpr void IfObject(Function function)
            { ConditionalCall<!fieldContainsPointers && !fieldIsStlIterable && !fieldIsStlAdaptor && fieldIsReflected && !fieldIsArray && !fieldIsPointer, Function>::call(function); }
        template <typename Function> static constexpr void IfObjectPointer(Function function)
            { ConditionalCall<!fieldContainsPointers && !fieldIsStlIterable && !fieldIsStlAdaptor && fieldIsReflected && !fieldIsArray && fieldIsPointer, Function>::call(function); }

        template <typename Function> static constexpr void IfPrimitiveArray(Function function)
            { ConditionalCall<fieldIsArray && !fieldIsReflected && !fieldContainsPairs && !fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfPrimitiveArrayPointer(Function function)
            { ConditionalCall<fieldIsArray && !fieldIsReflected && !fieldContainsPairs && fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterablePrimitives(Function function)
            { ConditionalCall<fieldIsStlIterable && !fieldIsReflected && !fieldContainsPairs && !fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterablePrimitivesPointer(Function function)
            { ConditionalCall<fieldIsStlIterable && !fieldIsReflected && !fieldContainsPairs && fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfPrimitiveAdaptor(Function function)
            { ConditionalCall<fieldIsStlAdaptor && !fieldIsReflected && !fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfPrimitiveAdaptorPointer(Function function)
            { ConditionalCall<fieldIsStlAdaptor && !fieldIsReflected && fieldIsPointer && !fieldContainsPointers, Function>::call(function); }

        template <typename Function> static constexpr void IfPrimitivePointerArray(Function function)
            { ConditionalCall<fieldIsArray && !fieldIsReflected && !fieldContainsPairs && !fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfPrimitivePointerArrayPointer(Function function)
            { ConditionalCall<fieldIsArray && !fieldIsReflected && !fieldContainsPairs && fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterablePrimitivePointers(Function function)
            { ConditionalCall<fieldIsStlIterable && !fieldIsReflected && !fieldContainsPairs && !fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterablePrimitivePointersPointer(Function function)
            { ConditionalCall<fieldIsStlIterable && !fieldIsReflected && !fieldContainsPairs && fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfPrimitivePointerAdaptor(Function function)
            { ConditionalCall<fieldIsStlAdaptor && !fieldIsReflected && !fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfPrimitivePointerAdaptorPointer(Function function)
            { ConditionalCall<fieldIsStlAdaptor && !fieldIsReflected && fieldIsPointer && fieldContainsPointers, Function>::call(function); }

        template <typename Function> static constexpr void IfObjectArray(Function function)
            { ConditionalCall<fieldIsArray && fieldIsReflected && !fieldContainsPairs && !fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfObjectArrayPointer(Function function)
            { ConditionalCall<fieldIsArray && fieldIsReflected && !fieldContainsPairs && fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterableObjects(Function function)
            { ConditionalCall<fieldIsStlIterable && fieldIsReflected && !fieldContainsPairs && !fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterableObjectsPointer(Function function)
            { ConditionalCall<fieldIsStlIterable && fieldIsReflected && !fieldContainsPairs && fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfObjectAdaptor(Function function)
            { ConditionalCall<fieldIsStlAdaptor && fieldIsReflected && !fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfObjectAdaptorPointer(Function function)
            { ConditionalCall<fieldIsStlAdaptor && fieldIsReflected && fieldIsPointer && !fieldContainsPointers, Function>::call(function); }

        template <typename Function> static constexpr void IfObjectPointerArray(Function function)
            { ConditionalCall<fieldIsArray && fieldIsReflected && !fieldContainsPairs && !fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfObjectPointerArrayPointer(Function function)
            { ConditionalCall<fieldIsArray && fieldIsReflected && !fieldContainsPairs && fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterableObjectPointers(Function function)
            { ConditionalCall<fieldIsStlIterable && fieldIsReflected && !fieldContainsPairs && !fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterableObjectPointersPointer(Function function)
            { ConditionalCall<fieldIsStlIterable && fieldIsReflected && !fieldContainsPairs && fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfObjectPointerAdaptor(Function function)
            { ConditionalCall<fieldIsStlAdaptor && fieldIsReflected && !fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfObjectPointerAdaptorPointer(Function function)
            { ConditionalCall<fieldIsStlAdaptor && fieldIsReflected && fieldIsPointer && fieldContainsPointers, Function>::call(function); }

        template <typename Function> static constexpr void IfIterablePrimitivePairs(Function function)
            { ConditionalCall<fieldIsStlIterable && !fieldIsReflected && fieldContainsPairs && !fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterablePrimitivePairsPointer(Function function)
            { ConditionalCall<fieldIsStlIterable && !fieldIsReflected && fieldContainsPairs && fieldIsPointer && !fieldContainsPointers, Function>::call(function); }

        template <typename Function> static constexpr void IfIterablePrimitivePointerPairs(Function function)
            { ConditionalCall<fieldIsStlIterable && !fieldIsReflected && fieldContainsPairs && !fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterablePrimitivePointerPairsPointer(Function function)
            { ConditionalCall<fieldIsStlIterable && !fieldIsReflected && fieldContainsPairs && fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        
        template <typename Function> static constexpr void IfIterableObjectPairs(Function function)
            { ConditionalCall<fieldIsStlIterable && fieldIsReflected && fieldContainsPairs && !fieldIsPointer && !fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterableObjectPairsPointer(Function function)
            { ConditionalCall<fieldIsStlIterable && fieldIsReflected && fieldContainsPairs && fieldIsPointer && !fieldContainsPointers, Function>::call(function); }

        template <typename Function> static constexpr void IfIterableObjectPointerPairs(Function function)
            { ConditionalCall<fieldIsStlIterable && fieldIsReflected && fieldContainsPairs && !fieldIsPointer && fieldContainsPointers, Function>::call(function); }
        template <typename Function> static constexpr void IfIterableObjectPointerPairsPointer(Function function)
            { ConditionalCall<fieldIsStlIterable && fieldIsReflected && fieldContainsPairs && fieldIsPointer && fieldContainsPointers, Function>::call(function); }


        /// Access accelerators: obtain access to field data, with lambda guards pre-applied
        /// and differentiation between pointers and non-pointers as well as arrays and STL-containers all taken care of
        /// 
        /// Stacks are special in that they're iterated bottom to top (so that if you inserted each into a new stack, it would have the same order)
        /// 
        /// All other STL containers are iterated naturally (e.g. vector begin to end, queue front to back, priority queue top to bottom)

		template <typename T, typename Function> static constexpr bool IfNull(T field, Function function) {
			bool isNull = false;
			IfPointer([&](auto) {
				isNull = field == nullptr;
				if ( isNull )
					function(field);
			});
			return isNull;
		}
        
        template <typename Primitive, typename Function> constexpr void ForPrimitive(Primitive primitive, Function function) {
            IfPrimitive([&](auto) { function(primitive); });
            IfPrimitivePointer([&](auto) { function(*primitive); });
        }
        template <typename Object, typename Function> constexpr void ForObject(Object object, Function function) {
            IfObject([&](auto) { function(object); });
            IfObjectPointer([&](auto) { function(*object); });
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitives(Iterable iterable, Function function) {
            IfPrimitiveArray([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, iterable[i]);
            });
            IfPrimitiveArrayPointer([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (*iterable)[i]);
            });
            IfIterablePrimitives([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element);
            });
            IfIterablePrimitivesPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForObjects(Iterable iterable, Function function) {
            IfObjectArray([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, iterable[i]);
            });
            IfObjectArrayPointer([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (*iterable)[i]);
            });
            IfIterableObjects([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element);
            });
            IfIterableObjectsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePairs(Iterable iterable, Function function) {
            IfIterablePrimitivePairs([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, element.second);
            });
            IfIterablePrimitivePairsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, element.second);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPairs(Iterable iterable, Function function) {
            IfIterableObjectPairs([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, element.second);
            });
            IfIterableObjectPairsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, element.second);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePointers(Iterable iterable, Function function) {
            IfPrimitivePointerArray([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, iterable[i]);
            });
            IfPrimitivePointerArrayPointer([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (*iterable)[i]);
            });
            IfIterablePrimitivePointers([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element);
            });
            IfIterablePrimitivePointersPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPointers(Iterable iterable, Function function) {
            IfObjectPointerArray([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, iterable[i]);
            });
            IfObjectPointerArrayPointer([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (*iterable)[i]);
            });
            IfIterableObjectPointers([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element);
            });
            IfIterableObjectPointersPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePointerPairs(Iterable iterable, Function function) {
            IfIterablePrimitivePointerPairs([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, element.second);
            });
            IfIterablePrimitivePointerPairsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, element.second);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPointerPairs(Iterable iterable, Function function) {
            IfIterableObjectPointerPairs([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
            IfIterableObjectPointerPairsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
        }

        template <typename Primitive, typename Function> constexpr void ForPrimitive(Primitive primitive, Function function) const {
            IfPrimitive([&](auto) { function((const type &)primitive); });
            IfPrimitivePointer([&](auto) { function((const type &)*primitive); });
        }
        template <typename Object, typename Function> constexpr void ForObject(Object object, Function function) const {
            IfObject([&](auto) { function((const type &)object); });
            IfObjectPointer([&](auto) { function((const type &)*object); });
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitives(Iterable iterable, Function function) const {
            IfPrimitiveArray([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)iterable[i]);
            });
            IfPrimitiveArrayPointer([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)(*iterable)[i]);
            });
            IfIterablePrimitives([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, (const element_type &)element);
            });
            IfIterablePrimitivesPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, (const element_type &)element);
            });
            IfPrimitiveAdaptor([&](auto) {
                size_t i=0;
                auto sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            });
            IfPrimitiveAdaptorPointer([&](auto) {
                size_t i=0;
                auto sequenceContainer = get_underlying_container(*iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForObjects(Iterable iterable, Function function) const {
            IfObjectArray([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)iterable[i]);
            });
            IfObjectArrayPointer([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)(*iterable)[i]);
            });
            IfIterableObjects([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, (const element_type &)element);
            });
            IfIterableObjectsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, (const element_type &)element);
            });
            IfObjectAdaptor([&](auto) {
                size_t i=0;
                auto sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            });
            IfObjectAdaptorPointer([&](auto) {
                size_t i=0;
                auto sequenceContainer = get_underlying_container(*iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePairs(Iterable iterable, Function function) const {
            IfIterablePrimitivePairs([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
            IfIterablePrimitivePairsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPairs(Iterable iterable, Function function) const {
            IfIterableObjectPairs([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
            IfIterableObjectPairsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePointers(Iterable iterable, Function function) const {
            IfPrimitivePointerArray([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)iterable[i]);
            });
            IfPrimitivePointerArrayPointer([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)(*iterable)[i]);
            });
            IfIterablePrimitivePointers([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, (const element_type &)element);
            });
            IfIterablePrimitivePointersPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, (const element_type &)element);
            });
            IfPrimitivePointerAdaptor([&](auto) {
                size_t i=0;
                auto sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            });
            IfPrimitivePointerAdaptorPointer([&](auto) {
                size_t i=0;
                auto sequenceContainer = get_underlying_container(*iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPointers(Iterable iterable, Function function) const {
            IfObjectPointerArray([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)iterable[i]);
            });
            IfObjectPointerArrayPointer([&](auto) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)(*iterable)[i]);
            });
            IfIterableObjectPointers([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, (const element_type &)element);
            });
            IfIterableObjectPointersPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, (const element_type &)element);
            });
            IfObjectPointerAdaptor([&](auto) {
                size_t i=0;
                auto sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            });
            IfObjectPointerAdaptorPointer([&](auto) {
                size_t i=0;
                auto sequenceContainer = get_underlying_container(*iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePointerPairs(Iterable iterable, Function function) const {
            IfIterablePrimitivePointerPairs([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
            IfIterablePrimitivePointerPairsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPointerPairs(Iterable iterable, Function function) const {
            IfIterableObjectPointerPairs([&](auto) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
            IfIterableObjectPointerPairsPointer([&](auto) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, (const element_type &)element.second);
            });
        }
    };
};

/// Contains the various type classes for declaring reflected fields and the definition for the REFLECT macro and non-generic supporting macros
namespace Reflect
{
    /// B "basic-type": must be used for any type which already has an acceptable representation when streamed
    template <typename T> struct B {
        static constexpr bool reflected = false;
        using type = typename T;
    };
    
    /// R "reflected-type": must be used for any object which in turn relies on reflection to be streamed
    template <typename T> struct R {
        static constexpr bool reflected = true;
        using type = typename T;
    };

#define GET_FIELD_NAME(x) RHS(x),
#define DESCRIBE_FIELD(x) struct RHS(x) { \
    static constexpr auto typeStr = ConstexprStr::substr<ConstexprStr::length_between(#x, '<', '>')>(#x+ConstexprStr::find(#x, '<')+1); \
    static constexpr auto nameStr = ConstexprStr::substr<ConstexprStr::length_after_last(#x, ' ')>(#x+ConstexprStr::find_last_of(#x, ' ')+1); \
    static constexpr RfS::Field<true, LHS(x)::reflected, RfS::is_pointable<LHS(x)::type>::value, std::is_array<LHS(x)::type>::value, \
        RfS::is_stl_iterable<LHS(x)::type>::value, RfS::is_adaptor<LHS(x)::type>::value, \
        RfS::contains_pointables<LHS(x)::type>::value, RfS::contains_pairs<LHS(x)::type>::value, LHS(x)::type> field = \
        { IndexOf::RHS(x), &nameStr.value[0], &typeStr.value[0], std::extent<LHS(x)::type>::value, \
        RfS::is_stl_iterable<LHS(x)::type>::value || std::is_array<LHS(x)::type>::value || RfS::is_adaptor<LHS(x)::type>::value, \
        RfS::contains_pairs<LHS(x)::type>::value, LHS(x)::reflected }; \
};
#define GET_FIELD(x) { IndexOf::RHS(x), &RHS(x)::nameStr.value[0], &RHS(x)::typeStr.value[0], std::extent<LHS(x)::type>::value, \
    RfS::is_stl_iterable<LHS(x)::type>::value || std::is_array<LHS(x)::type>::value || RfS::is_adaptor<LHS(x)::type>::value, \
    RfS::contains_pairs<LHS(x)::type>::value, LHS(x)::reflected },
#define USE_FIELD(x) function(RHS(x)::field, object.RHS(x));
#define USE_FIELD_AT(x) case IndexOf::RHS(x): function(RHS(x)::field, object.RHS(x)); break;

/// After the objectType there needs to be at least 1 and at most 123 fields, in the form "(B<type>) fieldName" or "(R<type>) fieldName"
/// e.g. REFLECT(myObj, (B<int>) myInt, (B<std::string>) myString)
#define REFLECT(objectType, ...) \
class Class { public: \
    static constexpr size_t totalFields = COUNT_ARGUMENTS(__VA_ARGS__); \
    enum_t(IndexOf, size_t, { FOR_EACH(GET_FIELD_NAME, __VA_ARGS__) }); \
    FOR_EACH(DESCRIBE_FIELD, __VA_ARGS__) \
    static constexpr RfS::Field<false, false, false, false, false, false, false, false, void> fields[totalFields] = { FOR_EACH(GET_FIELD, __VA_ARGS__) }; \
    template <typename Function> static void ForEachField(objectType & object, Function function) { FOR_EACH(USE_FIELD, __VA_ARGS__) } \
    template <typename Function> static void FieldAt(objectType & object, size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_AT, __VA_ARGS__) } } \
};

}

#endif