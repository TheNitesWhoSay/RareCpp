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
    struct TypeToStr {
        static constexpr auto Get() {
            return ConstexprStr::substr<ConstexprStr::length_between(__FUNCTION__, '<', '>')>(__FUNCTION__ + ConstexprStr::find(__FUNCTION__, '<') + 1);
        }
    };
    
    template <typename T>
    struct is_bool { static constexpr bool value = std::is_same<bool, T>::value; };

    template <typename T>
    struct is_pointable { static constexpr bool value = std::is_pointer<T>::value; };
    template <typename T>
    struct is_pointable<std::shared_ptr<T>> { static constexpr bool value = true; };
    template <typename T>
    struct is_pointable<std::unique_ptr<T>> { static constexpr bool value = true; };

    template <typename T>
    struct remove_pointer { using type = typename std::remove_pointer<T>::type; };
    template <typename T>
    struct remove_pointer<std::shared_ptr<T>> { using type = typename T; };
    template <typename T>
    struct remove_pointer<std::unique_ptr<T>> { using type = typename T; };

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
    template <typename T, typename C, typename P> struct contains_pointables<std::priority_queue<T, C, P>>
    { static constexpr bool value = is_pointable<T>::value; };
    template <typename K, typename C, typename A> struct contains_pointables<std::set<K, C, A>>
    { static constexpr bool value = is_pointable<K>::value; };
    template <typename K, typename C, typename A> struct contains_pointables<std::multiset<K, C, A>>
    { static constexpr bool value = is_pointable<K>::value; };
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
    
    template <typename L, typename R, size_t N> struct contains_pointables<std::array<std::pair<L, R>, N>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename A> struct contains_pointables<std::vector<std::pair<L, R>, A>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename A> struct contains_pointables<std::deque<std::pair<L, R>, A>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename A> struct contains_pointables<std::forward_list<std::pair<L, R>, A>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename A> struct contains_pointables<std::list<std::pair<L, R>, A>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename C> struct contains_pointables<std::stack<std::pair<L, R>, C>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename C> struct contains_pointables<std::queue<std::pair<L, R>, C>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename C, typename P> struct contains_pointables<std::priority_queue<std::pair<L, R>, C, P>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename C, typename A> struct contains_pointables<std::set<std::pair<L, R>, C, A>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename C, typename A> struct contains_pointables<std::multiset<std::pair<L, R>, C, A>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename H, typename E, typename A> struct contains_pointables<std::unordered_set<std::pair<L, R>, H, E, A>>
    { static constexpr bool value = is_pointable<R>::value; };
    template <typename L, typename R, typename H, typename E, typename A> struct contains_pointables<std::unordered_multiset<std::pair<L, R>, H, E, A>>
    { static constexpr bool value = is_pointable<R>::value; };

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
    template <typename K, typename T, typename H, typename E, typename A> struct element_type<std::unordered_map<K, T, H, E, A>>
    { using type = typename T; };
    template <typename K, typename T, typename H, typename E, typename A> struct element_type<std::unordered_multimap<K, T, H, E, A>>
    { using type = typename T; };
    
    template <typename L, typename R, size_t N> struct element_type<std::array<std::pair<L, R>, N>>
    { using type = typename R; };
    template <typename L, typename R, typename A> struct element_type<std::vector<std::pair<L, R>, A>>
    { using type = typename R; };
    template <typename L, typename R, typename A> struct element_type<std::deque<std::pair<L, R>, A>>
    { using type = typename R; };
    template <typename L, typename R, typename A> struct element_type<std::forward_list<std::pair<L, R>, A>>
    { using type = typename R; };
    template <typename L, typename R, typename A> struct element_type<std::list<std::pair<L, R>, A>>
    { using type = typename R; };
    template <typename L, typename R, typename C> struct element_type<std::stack<std::pair<L, R>, C>>
    { using type = typename R; };
    template <typename L, typename R, typename C> struct element_type<std::queue<std::pair<L, R>, C>>
    { using type = typename R; };
    template <typename L, typename R, typename C, typename P> struct element_type<std::priority_queue<std::pair<L, R>, C, P>>
    { using type = typename R; };
    template <typename L, typename R, typename C, typename A> struct element_type<std::set<std::pair<L, R>, C, A>>
    { using type = typename R; };
    template <typename L, typename R, typename C, typename A> struct element_type<std::multiset<std::pair<L, R>, C, A>>
    { using type = typename R; };
    template <typename L, typename R, typename H, typename E, typename A> struct element_type<std::unordered_set<std::pair<L, R>, H, E, A>>
    { using type = typename R; };
    template <typename L, typename R, typename H, typename E, typename A> struct element_type<std::unordered_multiset<std::pair<L, R>, H, E, A>>
    { using type = typename R; };

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
    struct contains_pairs<std::pair<L, R>[N]> { static constexpr bool value = true; };
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
    template <typename K, typename T, typename H, typename E, typename A>
    struct contains_pairs<std::unordered_map<K, T, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename H, typename E, typename A>
    struct contains_pairs<std::unordered_multimap<K, T, H, E, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A>
    struct contains_pairs<std::map<K, T, C, A>> { static constexpr bool value = true; };
    template <typename K, typename T, typename C, typename A>
    struct contains_pairs<std::multimap<K, T, C, A>> { static constexpr bool value = true; };
    
    template <typename Type, typename TypeIfVoid>
    struct if_void { using type = Type; };
    template <typename TypeIfVoid>
    struct if_void<void, TypeIfVoid> { using type = TypeIfVoid; };

    template <class Adaptor>
    const typename Adaptor::container_type & get_underlying_container(const Adaptor & adaptor) {
        struct AdaptorSubClass : Adaptor {
            static const typename Adaptor::container_type & get(const Adaptor & adaptor) {
                return adaptor.*(&AdaptorSubClass::c);
            }
        };
        return AdaptorSubClass::get(adaptor);
    }
    
    template <typename T = void, bool fieldIsReflected = false, bool fieldIsString = false>
    class Field;
    
    template <>
    class Field<void, false, false> {
    public:
        size_t index;
        const char* name;
        const char* typeStr;
        size_t arraySize;
        bool isIterable;
        bool containsPairs;
        bool isReflected;
        bool isString;
    };

    template <typename T, bool fieldIsReflected, bool fieldIsString>
    class Field {
    public:
        size_t index;
        const char* name;
        const char* typeStr;
        size_t arraySize;
        bool isIterable;
        bool containsPairs;
        bool isReflected;
        bool isString;
        
        static constexpr bool IsBool = is_bool<T>::value;
        static constexpr bool IsString = fieldIsString;
        static constexpr bool IsPointer = is_pointable<T>::value;
        static constexpr bool IsArray = std::is_array<typename remove_pointer<T>::type>::value;
        static constexpr bool IsStlIterable = is_stl_iterable<typename remove_pointer<T>::type>::value;
        static constexpr bool IsStlAdaptor = is_adaptor<typename remove_pointer<T>::type>::value;
        static constexpr bool ContainsPointers = contains_pointables<typename remove_pointer<T>::type>::value;
        static constexpr bool ContainsPairs = contains_pairs<typename remove_pointer<T>::type>::value;


        /// Aggregations

        static constexpr bool IsPrimitive = !IsArray && !IsStlIterable && !IsStlAdaptor && !fieldIsReflected && !ContainsPairs && !IsPointer;
        static constexpr bool IsPrimitivePointer = !IsArray && !IsStlIterable && !IsStlAdaptor && !fieldIsReflected && !ContainsPairs && IsPointer;

        static constexpr bool IsObject = !ContainsPointers && !IsStlIterable && !IsStlAdaptor && fieldIsReflected && !IsArray && !IsPointer;
        static constexpr bool IsObjectPointer = !ContainsPointers && !IsStlIterable && !IsStlAdaptor && fieldIsReflected && !IsArray && IsPointer;
        
        static constexpr bool IsPrimitiveArray = IsArray && !fieldIsReflected && !ContainsPairs && !IsPointer && !ContainsPointers;
        static constexpr bool IsPrimitiveArrayPointer = IsArray && !fieldIsReflected && !ContainsPairs && IsPointer && !ContainsPointers;
        static constexpr bool IsIterablePrimitives = IsStlIterable && !fieldIsReflected && !ContainsPairs && !IsPointer && !ContainsPointers;
        static constexpr bool IsIterablePrimitivesPointer = IsStlIterable && !fieldIsReflected && !ContainsPairs && IsPointer && !ContainsPointers;
        static constexpr bool IsPrimitiveAdaptor = IsStlAdaptor && !fieldIsReflected && !IsPointer && !ContainsPointers;
        static constexpr bool IsPrimitiveAdaptorPointer = IsStlAdaptor && !fieldIsReflected && IsPointer && !ContainsPointers;
        
        static constexpr bool IsPrimitivePointerArray = IsArray && !fieldIsReflected && !ContainsPairs && !IsPointer && ContainsPointers;
        static constexpr bool IsPrimitivePointerArrayPointer = IsArray && !fieldIsReflected && !ContainsPairs && IsPointer && ContainsPointers;
        static constexpr bool IsIterablePrimitivePointers = IsStlIterable && !fieldIsReflected && !ContainsPairs && !IsPointer && ContainsPointers;
        static constexpr bool IsIterablePrimitivePointersPointer = IsStlIterable && !fieldIsReflected && !ContainsPairs && IsPointer && ContainsPointers;
        static constexpr bool IsPrimitivePointerAdaptor = IsStlAdaptor && !fieldIsReflected && !IsPointer && ContainsPointers;
        static constexpr bool IsPrimitivePointerAdaptorPointer = IsStlAdaptor && !fieldIsReflected && IsPointer && ContainsPointers;
        
        static constexpr bool IsObjectArray = IsArray && fieldIsReflected && !ContainsPairs && !IsPointer && !ContainsPointers;
        static constexpr bool IsObjectArrayPointer = IsArray && fieldIsReflected && !ContainsPairs && IsPointer && !ContainsPointers;
        static constexpr bool IsIterableObjects = IsStlIterable && fieldIsReflected && !ContainsPairs && !IsPointer && !ContainsPointers;
        static constexpr bool IsIterableObjectsPointer = IsStlIterable && fieldIsReflected && !ContainsPairs && IsPointer && !ContainsPointers;
        static constexpr bool IsObjectAdaptor = IsStlAdaptor && fieldIsReflected && !IsPointer && !ContainsPointers;
        static constexpr bool IsObjectAdaptorPointer = IsStlAdaptor && fieldIsReflected && IsPointer && !ContainsPointers;
        
        static constexpr bool IsObjectPointerArray = IsArray && fieldIsReflected && !ContainsPairs && !IsPointer && ContainsPointers;
        static constexpr bool IsObjectPointerArrayPointer = IsArray && fieldIsReflected && !ContainsPairs && IsPointer && ContainsPointers;
        static constexpr bool IsIterableObjectPointers = IsStlIterable && fieldIsReflected && !ContainsPairs && !IsPointer && ContainsPointers;
        static constexpr bool IsIterableObjectPointersPointer = IsStlIterable && fieldIsReflected && !ContainsPairs && IsPointer && ContainsPointers;
        static constexpr bool IsObjectPointerAdaptor = IsStlAdaptor && fieldIsReflected && !IsPointer && ContainsPointers;
        static constexpr bool IsObjectPointerAdaptorPointer = IsStlAdaptor && fieldIsReflected && IsPointer && ContainsPointers;
        
        static constexpr bool IsIterablePrimitivePairs = IsStlIterable && !fieldIsReflected && ContainsPairs && !IsPointer && !ContainsPointers;
        static constexpr bool IsIterablePrimitivePairsPointer = IsStlIterable && !fieldIsReflected && ContainsPairs && IsPointer && !ContainsPointers;
        
        static constexpr bool IsIterablePrimitivePointerPairs = IsStlIterable && !fieldIsReflected && ContainsPairs && !IsPointer && ContainsPointers;
        static constexpr bool IsIterablePrimitivePointerPairsPointer = IsStlIterable && !fieldIsReflected && ContainsPairs && IsPointer && ContainsPointers;
        
        static constexpr bool IsIterableObjectPairs = IsStlIterable && fieldIsReflected && ContainsPairs && !IsPointer && !ContainsPointers;
        static constexpr bool IsIterableObjectPairsPointer = IsStlIterable && fieldIsReflected && ContainsPairs && IsPointer && !ContainsPointers;
        
        static constexpr bool IsIterableObjectPointerPairs = IsStlIterable && fieldIsReflected && ContainsPairs && !IsPointer && ContainsPointers;
        static constexpr bool IsIterableObjectPointerPairsPointer = IsStlIterable && fieldIsReflected && ContainsPairs && IsPointer && ContainsPointers;
        

        using type = T;

        /// Key type is the map key type or "first" in contained pairs, void if field is neither a map nor contains pairs
        using key_type = typename key_type<typename remove_pointer<T>::type>::type;

        /// Element type is the type of the field (if singular) or the type contained in this field (second if pair), void if not iterable
        using element_type = typename element_type<typename remove_pointer<T>::type>::type;

        /// Sub type is the type you should pass if recursively reading this field (if singular) or the type contained in this field (second if pair)
        using sub_type = typename if_void<typename remove_pointer<element_type>::type, typename remove_pointer<T>::type>::type;


        /// Access accelerators: obtain access to field data, with constexpr-if guards pre-applied
        /// and differentiation between pointers and non-pointers as well as arrays and STL-containers all taken care of
        /// 
        /// Stacks are special in that they're iterated bottom to top (so that if you inserted each into a new stack, it would have the same order)
        /// 
        /// All other STL containers are iterated naturally (e.g. vector begin to end, queue front to back, priority queue top to bottom)

		template <typename T> static constexpr bool IsNull(T & field) {
            if constexpr ( IsPointer )
                return field == nullptr;
            else
                return false;
		}
        
        template <typename Primitive, typename Function> constexpr void ForPrimitive(Primitive & primitive, Function function) const {
            if constexpr ( IsPrimitive )
                function(primitive);
            else if constexpr ( IsPrimitivePointer )
                function(*primitive);
        }
        template <typename Object, typename Function> constexpr void ForObject(Object & object, Function function) const {
            if constexpr ( IsObject )
                function(object);
            else if constexpr ( IsObjectPointer )
                function(*object);
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitives(Iterable & iterable, Function function) const {
            if constexpr ( IsPrimitiveArray ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, iterable[i]);
            }
            else if constexpr ( IsPrimitiveArrayPointer ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (*iterable)[i]);
            }
            else if constexpr ( IsIterablePrimitives ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element);
            }
            else if constexpr ( IsIterablePrimitivesPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivesConst(Iterable & iterable, Function function) const {
            if constexpr ( IsPrimitiveArray ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)iterable[i]);
            }
            else if constexpr ( IsPrimitiveArrayPointer ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)(*iterable)[i]);
            }
            else if constexpr ( IsIterablePrimitives ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, (const element_type &)element);
            }
            else if constexpr ( IsIterablePrimitivesPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, (const element_type &)element);
            }
            else if constexpr ( IsPrimitiveAdaptor ) {
                size_t i=0;
                const auto & sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            }
            else if constexpr ( IsPrimitiveAdaptorPointer ) {
                size_t i=0;
                const auto & sequenceContainer = get_underlying_container(*iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForObjects(Iterable & iterable, Function function) const {
            if constexpr ( IsObjectArray ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, iterable[i]);
            }
            else if constexpr ( IsObjectArrayPointer ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (*iterable)[i]);
            }
            else if constexpr ( IsIterableObjects ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element);
            }
            else if constexpr ( IsIterableObjectsPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForObjectsConst(Iterable & iterable, Function function) const {
            if constexpr ( IsObjectArray ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)iterable[i]);
            }
            else if constexpr ( IsObjectArrayPointer ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)(*iterable)[i]);
            }
            else if constexpr ( IsIterableObjects ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, (const element_type &)element);
            }
            else if constexpr ( IsIterableObjectsPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, (const element_type &)element);
            }
            else if constexpr ( IsObjectAdaptor ) {
                size_t i=0;
                const auto & sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            }
            else if constexpr ( IsObjectAdaptorPointer ) {
                size_t i=0;
                const auto & sequenceContainer = get_underlying_container(*iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePairs(Iterable & iterable, Function function) const {
            if constexpr ( IsIterablePrimitivePairs ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, element.second);
            }
            else if constexpr ( IsIterablePrimitivePairsPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, element.second);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPairs(Iterable & iterable, Function function) const {
            if constexpr ( IsIterableObjectPairs ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, element.second);
            }
            else if constexpr ( IsIterableObjectPairsPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, element.second);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePointers(Iterable & iterable, Function function) const {
            if constexpr ( IsPrimitivePointerArray ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, iterable[i]);
            }
            else if constexpr ( IsPrimitivePointerArrayPointer ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (*iterable)[i]);
            }
            else if constexpr ( IsIterablePrimitivePointers ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element);
            }
            else if constexpr ( IsIterablePrimitivePointersPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePointersConst(Iterable & iterable, Function function) const {
            if constexpr ( IsPrimitivePointerArray ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)iterable[i]);
            }
            else if constexpr ( IsPrimitivePointerArrayPointer ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)(*iterable)[i]);
            }
            else if constexpr ( IsIterablePrimitivePointers ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, (const element_type &)element);
            }
            else if constexpr ( IsIterablePrimitivePointersPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, (const element_type &)element);
            }
            else if constexpr ( IsPrimitivePointerAdaptor ) {
                size_t i=0;
                const auto & sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            }
            else if constexpr ( IsPrimitivePointerAdaptorPointer ) {
                size_t i=0;
                const auto & sequenceContainer = get_underlying_container(*iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPointers(Iterable & iterable, Function function) const {
            if constexpr ( IsObjectPointerArray ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, iterable[i]);
            }
            else if constexpr ( IsObjectPointerArrayPointer ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (*iterable)[i]);
            }
            else if constexpr ( IsIterableObjectPointers ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element);
            }
            else if constexpr ( IsIterableObjectPointersPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPointersConst(Iterable & iterable, Function function) const {
            if constexpr ( IsObjectPointerArray ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)iterable[i]);
            }
            else if constexpr ( IsObjectPointerArrayPointer ) {
                for ( size_t i=0; i<arraySize; i++ )
                    function(i, (const element_type &)(*iterable)[i]);
            }
            else if constexpr ( IsIterableObjectPointers ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, (const element_type &)element);
            }
            else if constexpr ( IsIterableObjectPointersPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, (const element_type &)element);
            }
            else if constexpr ( IsObjectPointerAdaptor ) {
                size_t i=0;
                const auto & sequenceContainer = get_underlying_container(iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            }
            else if constexpr ( IsObjectPointerAdaptorPointer ) {
                size_t i=0;
                const auto & sequenceContainer = get_underlying_container(*iterable);
                for ( auto it = sequenceContainer.begin(); it != sequenceContainer.end(); ++it )
                    function(i++, (const element_type &)*it);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForPrimitivePointerPairs(Iterable & iterable, Function function) const {
            if constexpr ( IsIterablePrimitivePointerPairs ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, element.second);
            }
            else if constexpr ( IsIterablePrimitivePointerPairsPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, element.second);
            }
        }
        template <typename Iterable, typename Function> constexpr void ForObjectPointerPairs(Iterable & iterable, Function function) const {
            if constexpr ( IsIterableObjectPointerPairs ) {
                size_t i=0;
                for ( auto & element : iterable )
                    function(i++, element.first, (const element_type &)element.second);
            }
            else if constexpr ( IsIterableObjectPointerPairsPointer ) {
                size_t i=0;
                for ( auto & element : *iterable )
                    function(i++, element.first, (const element_type &)element.second);
            }
        }

    };
};

/// Contains the various type classes for declaring reflected fields and the definition for the REFLECT macro and non-generic supporting macros
namespace Reflect
{
    template <typename T>
    std::string TypeToStr() {
        return std::string(RfS::TypeToStr<T>::Get().value);
    }

    /// B "basic-type": must be used for any type which already has an acceptable representation when streamed
    struct B {
        static constexpr bool reflected = false;
        static constexpr bool isString = false;
    };

    struct S {
        static constexpr bool reflected = false;
        static constexpr bool isString = true;
    };
    
    /// R "reflected-type": must be used for any object which in turn relies on reflection to be streamed
    struct R {
        static constexpr bool reflected = true;
        static constexpr bool isString = false;
    };

    // I "inherit-from": used to denote a set of classes whose properties are being inherited by another reflected class
    template <typename ... Ts>
    class I;

    template <>
    class I<> { public:
        
        static constexpr size_t totalSupers = 0;
        
        template <typename Function, typename SubClass>
        static void ForEach(SubClass & object, Function function) {}

        template <typename Function, typename SubClass>
        static void At(SubClass & object, size_t superIndex, Function function) {}
    };

    template <>
    class I<I<>> { public:
        
        static constexpr size_t totalSupers = 0;

        template <typename Function, typename SubClass>
        static void ForEach(SubClass & object, Function function) {}

        template <typename Function, typename SubClass>
        static void At(SubClass & object, size_t superIndex, Function function) {}
    };

    template <typename T>
    class I<T> { public:
        
        static constexpr size_t totalSupers = 1;

        template <typename Function, typename SubClass>
        static void ForEach(SubClass & object, Function function) {
            function(0, (T &)object);
        }

        template <typename Function, typename SubClass>
        static void At(SubClass & object, size_t superIndex, Function function) {
            if ( superIndex == 0 )
                function((T &)object);
        }
    };

    template <typename T>
    class I<I<T>> { public:
        
        static constexpr size_t totalSupers = 1;

        template <typename Function, typename SubClass>
        static void ForEach(SubClass & object, Function function) {
            function(0, (T &)object);
        }

        template <typename Function, typename SubClass>
        static void At(SubClass & object, size_t superIndex, Function function) {
            if ( superIndex == 0 )
                function((T &)object);
        }
    };

    template <typename ... Ts>
    class I<I<Ts ...>> { public:
        
        static constexpr size_t totalSupers = sizeof...(Ts);

        template <size_t SuperIndex, typename Function, typename SubClass>
        static void ForEachRecursion(SubClass &, Function function) {
            // Base case for recursion
        }

        template <size_t SuperIndex, typename Function, typename SubClass, typename CurrentSuperClassType, typename... NextSuperClassTypes>
        static void ForEachRecursion(SubClass & object, Function function) {
            size_t superIndex = SuperIndex;
            function(superIndex, (CurrentSuperClassType &)object);
            ForEachRecursion<SuperIndex+1, Function, SubClass, NextSuperClassTypes...>(object, function);
        }

        template <typename Function, typename SubClass>
        static void ForEach(SubClass & object, Function function) {
            ForEachRecursion<0, Function, SubClass, Ts ...>(object, function);
        }
        
        template <size_t SuperIndex, typename Function, typename SubClass>
        static void AtRecursion(SubClass & object, size_t superIndex, Function function) {
            // Base case for recursion
        }
        
        template <size_t SuperIndex, typename Function, typename SubClass, typename CurrentSuperClassType, typename... NextSuperClassTypes>
        static void AtRecursion(SubClass & object, size_t superIndex, Function function) {
            if ( SuperIndex == superIndex )
                function((CurrentSuperClassType &)object);

            AtRecursion<SuperIndex+1, Function, SubClass, NextSuperClassTypes...>(object, superIndex, function);
        }

        template <typename Function, typename SubClass>
        static void At(SubClass & object, size_t superIndex, Function function) {
            AtRecursion<0, Function, SubClass, Ts ...>(object, superIndex, function);
        }
    };


#define ALIAS_TYPE(x) using RHS(x) = decltype(RHS(x));
#define GET_FIELD_NAME(x) RHS(x),
#define DESCRIBE_FIELD(x) struct RHS(x)_ { \
    static constexpr auto nameStr = ConstexprStr::substr<ConstexprStr::length_after_last(#x, ' ')>(#x+ConstexprStr::find_last_of(#x, ' ')+1); \
    static constexpr auto typeStr = RfS::TypeToStr<RHS(x)>::Get(); \
    static constexpr RfS::Field<Class::RHS(x), LHS(x)::reflected, LHS(x)::isString> field = \
        { IndexOf::RHS(x), &nameStr.value[0], &typeStr.value[0], std::extent<RfS::remove_pointer<RHS(x)>::type>::value, \
        RfS::is_stl_iterable<RfS::remove_pointer<RHS(x)>::type>::value || std::is_array<RfS::remove_pointer<RHS(x)>::type>::value || \
        RfS::is_adaptor<RfS::remove_pointer<RHS(x)>::type>::value, RfS::contains_pairs<RfS::remove_pointer<RHS(x)>::type>::value, \
        LHS(x)::reflected, LHS(x)::isString }; \
};
#define GET_FIELD(x) { Class::RHS(x)_::field.index, Class::RHS(x)_::field.name, Class::RHS(x)_::field.typeStr, Class::RHS(x)_::field.arraySize, \
    Class::RHS(x)_::field.isIterable, Class::RHS(x)_::field.containsPairs, Class::RHS(x)_::field.isReflected, Class::RHS(x)_::field.isString },
#define USE_FIELD(x) function(RHS(x)_::field, object.RHS(x));
#define USE_FIELD_AT(x) case IndexOf::RHS(x): function(RHS(x)_::field, object.RHS(x)); break;


#pragma warning(disable: 4003) // Not enough arguments warning generated despite macros working perfectly

/// After the objectType there needs to be at least 1 and at most 123 fields, in the form "(B) fieldName" or "(R) fieldName"
/// e.g. REFLECT(myObj, (B) myInt, (B) myString)
#define REFLECT(objectType, ...) \
class Class { public: \
    static constexpr size_t totalFields = COUNT_ARGUMENTS(__VA_ARGS__); \
    enum_t(IndexOf, size_t, { FOR_EACH(GET_FIELD_NAME, __VA_ARGS__) }); \
    FOR_EACH(ALIAS_TYPE, __VA_ARGS__) \
    FOR_EACH(DESCRIBE_FIELD, __VA_ARGS__) \
    static constexpr RfS::Field<> fields[totalFields] = { FOR_EACH(GET_FIELD, __VA_ARGS__) }; \
    template <typename Function> static void ForEachField(RHS(objectType) & object, Function function) { FOR_EACH(USE_FIELD, __VA_ARGS__) } \
    template <typename Function> static void ForEachField(const RHS(objectType) & object, Function function) { FOR_EACH(USE_FIELD, __VA_ARGS__) } \
    template <typename Function> static void FieldAt(RHS(objectType) & object, size_t fieldIndex, Function function) { \
        switch ( fieldIndex ) { FOR_EACH(USE_FIELD_AT, __VA_ARGS__) } } \
}; \
using Supers = I<LHS(objectType)>;

}

#endif