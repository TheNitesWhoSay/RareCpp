// MIT License, Copyright (c) 2019-2023 Justin F https://github.com/TheNitesWhoSay/RareCpp/blob/master/LICENSE
#ifndef REFLECT_H
#define REFLECT_H
#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#ifdef _MSC_VER
#define MSVC_UNUSED_FALSE_POSITIVE _Pragma("warning(suppress: 4100)")
#else
#define MSVC_UNUSED_FALSE_POSITIVE 
#endif

// RareCpp Type Support - general type support and reflection capabilities
namespace RareTs
{
    // Generic Macros: Contains everything neccessary to loop over varadic macro arguments
    namespace {

// ArgMax: 125 (derived from the C spec requiring at least 127 arguments and the RARE_COUNT_ARGUMENTS macro requiring ArgMax+2 arguments)

// MacroLoop_Expand
#define ML_E(x) x

// MacroLoop_Concatenate
#define ML_C(x,y) x##y

// MacroLoop_ForEach[1, ..., ArgMax]
#define ML_0(...)
#define ML_1(f,a) f(a)
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

// MacroLoop_ArgumentCounts [ArgMax ... 0]
#define ML_G() 125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,\
95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,\
47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

// MacroLoop_Select_Argument_At_Argument_Max [a0, ..., a(ArgMax-1), argAtArgMax]
#define ML_M(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,\
d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,g0,g1,g2,g3,g4,g5,g6,g7,g8,g9,h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,\
i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,j0,j1,j2,j3,j4,j5,j6,j7,j8,argAtArgMax,...) argAtArgMax

// MacroLoop_Expand_Select_Argument_At_Argument_Max (necessary due to bugs in VS macro handling)
#define ML_S(...) ML_E(ML_M(__VA_ARGS__))

// MacroLoop_ForEach_N
#define ML_N(N,f,...) ML_E(ML_C(ML_,N)(f,__VA_ARGS__))

// Selects the count of varadic arguments (can count 0 arguments, but cannot count parenthesized arguments)
#define RARE_COUNT_ARGUMENTS(...) ML_S(ML##__VA_ARGS__##_G(),ML_G())

// Call "f" for each argument
#define RARE_FOR_EACH(f,...) ML_N(RARE_COUNT_ARGUMENTS(__VA_ARGS__),f,__VA_ARGS__)

    }

    // Non-reflection specific type-support
    inline namespace ExtendedTypeSupport
    {
        template <class T, T> using enable_if_constexpr = void;

        template <typename T, typename ... Ts> using first_of_t = T; // Like void_t but with a type
        
        template <size_t I, typename ... Ts> using as_index = std::integral_constant<size_t, I>; // Like void_t but with an index

        template <typename T> struct remove_cvref { using type = std::remove_cv_t<std::remove_reference_t<T>>; }; // If in C++20, prefer std::remove_cvref_t
        template <typename T> using remove_cvref_t = typename remove_cvref<T>::type;
        
        template <typename T> struct type_id { using type = T; };
        template <typename T> struct type_tag {};

        template <typename T, typename TypeIfVoid> struct replace_void { using type = T; };
        template <typename TypeIfVoid> struct replace_void<void, TypeIfVoid> { using type = TypeIfVoid; };
        template <typename T, typename TypeIfVoid> using replace_void_t = typename replace_void<T, TypeIfVoid>::type;

        template <typename T> struct promote_char { using type = T; };
        template <typename T> struct promote_char<const T> { using type = std::add_const_t<typename promote_char<T>::type>; };
        template <> struct promote_char<char> { using type = int; };
        template <> struct promote_char<signed char> { using type = int; };
        template <> struct promote_char<unsigned char> { using type = int; };
        template <typename T> using promote_char_t = typename promote_char<T>::type;

        template <typename T> struct pair_lhs { using type = void; };
        template <typename L, typename R> struct pair_lhs<std::pair<L, R>> { using type = L; };
        template <typename T> using pair_lhs_t = typename pair_lhs<T>::type;

        template <typename T> struct remove_pair_const { using type = T; };
        template <typename ... Ts> struct remove_pair_const<std::pair<Ts...>> { using type = std::pair<std::remove_const_t<Ts>...>; };
        template <typename T> using remove_pair_const_t = typename remove_pair_const<T>::type;

        template <typename ... Ts> struct element_type { using type = void; };
        template <typename T, size_t N> struct element_type<T[N]> { using type = T; };
        template <typename T, size_t N> struct element_type<const T[N]> { using type = T; };
        template <typename T> struct element_type<first_of_t<T, typename T::value_type>> { using type = remove_pair_const_t<typename T::value_type>; };
        template <typename ... Ts> using element_type_t = typename element_type<Ts...>::type;

        template <typename T> struct remove_pointer {
            template <typename U, class = std::void_t<decltype(*std::declval<std::remove_cv_t<U>>()), decltype(std::declval<std::remove_cv_t<U>>() == nullptr),
                decltype(std::declval<std::remove_cv_t<U>>().use_count())>>
            static constexpr RareTs::type_id<typename U::element_type> detail(int, int); // shared_ptr
            template <typename U, class = std::void_t<decltype(*std::declval<std::remove_cv_t<U>>().lock()),
                decltype(std::declval<std::remove_cv_t<U>>().lock() == nullptr), decltype(std::declval<std::remove_cv_t<U>>().use_count())>>
            static constexpr RareTs::type_id<typename U::element_type> detail(int, unsigned); // weak_ptr
            template <typename U, class = std::void_t<decltype(*std::declval<std::remove_cv_t<U>>()), decltype(std::declval<std::remove_cv_t<U>>() == nullptr),
                typename U::pointer>>
            static constexpr RareTs::type_id<typename U::element_type> detail(unsigned, unsigned); // unique_ptr
            template <typename U> static constexpr RareTs::type_id<U> detail(unsigned, ...);

            using type = typename decltype(detail<T>(0, 0))::type;
        };
        template <typename T> struct remove_pointer<T*> { using type = T; };
        template <typename T> struct remove_pointer<T* const> { using type = T; };
        template <typename T> struct remove_pointer<T* volatile> { using type = T; };
        template <typename T> struct remove_pointer<T* const volatile> { using type = T; };
        template <typename T> using remove_pointer_t = typename remove_pointer<T>::type;

        template <typename T> struct is_pointable : std::bool_constant<!std::is_same_v<T, remove_pointer_t<T>>> {};
        template <typename T> inline constexpr bool is_pointable_v = is_pointable<T>::value;

        template <typename T> struct is_non_null { static constexpr bool value = !std::is_null_pointer_v<T>; };
        template <typename T> inline constexpr bool is_non_null_v = is_non_null<T>::value;
        
        template <typename T> struct static_array_size { static constexpr size_t value = 0; };
        template <typename T, size_t N> struct static_array_size<T[N]> { static constexpr size_t value = N; };
        template <typename T, size_t N> struct static_array_size<const T[N]> { static constexpr size_t value = N; };
        template <typename T, size_t N> struct static_array_size<std::array<T, N>> { static constexpr size_t value = N; };
        template <typename T, size_t N> struct static_array_size<const std::array<T, N>> { static constexpr size_t value = N; };
        template <typename T> inline constexpr size_t static_array_size_v = static_array_size<T>::value;
        
        template <typename T> struct is_static_array { static constexpr bool value = false; };
        template <typename T, size_t N> struct is_static_array<T[N]> { static constexpr bool value = true; };
        template <typename T, size_t N> struct is_static_array<const T[N]> { static constexpr bool value = true; };
        template <typename T, size_t N> struct is_static_array<std::array<T, N>> { static constexpr bool value = true; };
        template <typename T, size_t N> struct is_static_array<const std::array<T, N>> { static constexpr bool value = true; };
        template <typename T> inline constexpr bool is_static_array_v = is_static_array<T>::value;

        template <typename T> struct is_iterable { static constexpr bool value = !std::is_same_v<void, element_type_t<std::remove_cv_t<T>>>; };
        template <typename T> inline constexpr bool is_iterable_v = is_iterable<T>::value;

        template <typename ... Ts> struct is_map : std::false_type {};
        template <typename T> struct is_map<first_of_t<T, typename T::mapped_type>> : std::true_type {};
        template <typename T> inline constexpr bool is_map_v = is_map<T>::value;
    
        template <typename ... Ts> struct is_adaptor { static constexpr bool value = false; };
        template <typename T> struct is_adaptor<first_of_t<T, typename T::container_type>> : std::true_type {};
        template <typename T> inline constexpr bool is_adaptor_v = is_adaptor<T>::value;

        template <typename T> struct is_pair { static constexpr bool value = false; };
        template <typename L, typename R> struct is_pair<std::pair<L, R>> { static constexpr bool value = true; };
        template <typename L, typename R> struct is_pair<const std::pair<L, R>> { static constexpr bool value = true; };
        template <typename T> inline constexpr bool is_pair_v = is_pair<T>::value;
        
        template <typename T> struct is_tuple { static constexpr bool value = false; };
        template <typename T> struct is_tuple<const T> { static constexpr bool value = is_tuple<T>::value; };
        template <typename ...Ts> struct is_tuple<std::tuple<Ts...>> { static constexpr bool value = true; };
        template <typename ...Ts> struct is_tuple<std::tuple<Ts...> &> { static constexpr bool value = true; };
        template <typename T> inline constexpr bool is_tuple_v = is_tuple<T>::value;
        
        template <typename T, template<typename ...> class Of> struct is_specialization : std::false_type {};
        template <typename T, template<typename ...> class Of> struct is_specialization<const T, Of> : is_specialization<T, Of> {};
        template <template<typename ...> class Of, typename ...Ts> struct is_specialization<Of<Ts...>, Of> : std::true_type {};
        template <typename T, template<typename ...> class Of> inline constexpr bool is_specialization_v = is_specialization<T, Of>::value;
    
        namespace detail {
            template <typename L, typename R> using AssignmentOp = decltype(std::declval<L>() = std::declval<const R &>());
            template <typename L, typename R> using StaticCastAssignmentOp = decltype(
                std::declval<L>() = static_cast<std::remove_reference_t<L>>(std::declval<const R &>()));
            template <typename T> using BeginOp = decltype(std::declval<T>().begin());
            template <typename T> using EndOp = decltype(std::declval<T>().end());
            template <typename T, typename E> using PushBackOp = decltype(std::declval<T>().push_back(std::declval<E>()));
            template <typename T, typename E> using PushOp = decltype(std::declval<T>().push(std::declval<E>()));
            template <typename T, typename E> using InsertOp = decltype(std::declval<T>().insert(std::declval<E>()));
            template <typename T> using ClearOp = decltype(std::declval<T>().clear());
            template <typename T, typename E> using InsertAfterBeforeBeginOp = decltype(
                std::declval<T>().insert_after(std::declval<T>().before_begin(), std::declval<E>()));
        }

        template <class AlwaysVoid, template<class...> class Op, class... Args> struct op_exists { static constexpr bool value = false; };
        template <template <class...> class Op, class... Args> struct op_exists<std::void_t<Op<Args...>>, Op, Args...> { static constexpr bool value = true; };
        template <template <class...> class Op, class... Args> inline constexpr bool op_exists_v = op_exists<void, Op, Args...>::value;

        template <typename L, typename R> struct is_assignable : op_exists<void, detail::AssignmentOp, L, R> {};
        template <typename L, typename R> inline constexpr bool is_assignable_v = is_assignable<L, R>::value;

        template <typename L, typename R> struct is_static_cast_assignable : op_exists<void, detail::StaticCastAssignmentOp, L, R> {};
        template <typename L, typename R> inline constexpr bool is_static_cast_assignable_v = is_static_cast_assignable<L, R>::value;
        
        template <typename T> struct has_begin_end { static constexpr bool value = op_exists_v<detail::BeginOp, T> && op_exists_v<detail::EndOp, T>; };
        template <typename T> inline constexpr bool has_begin_end_v = has_begin_end<T>::value;
        
        template <typename T, typename E> struct has_push_back { static constexpr bool value = op_exists_v<detail::PushBackOp, T, E>; };
        template <typename T, typename E> inline constexpr bool has_push_back_v = has_push_back<T, E>::value;
        
        template <typename T, typename E> struct has_push { static constexpr bool value = op_exists_v<detail::PushOp, T, E>; };
        template <typename T, typename E> inline constexpr bool has_push_v = has_push<T, E>::value;
        
        template <typename T, typename E> struct has_insert { static constexpr bool value = op_exists_v<detail::InsertOp, T, E>; };
        template <typename T, typename E> inline constexpr bool has_insert_v = has_insert<T, E>::value;
        
        template <typename T, typename E> struct has_insert_after { static constexpr bool value = op_exists_v<detail::InsertAfterBeforeBeginOp, T, E>; };
        template <typename T, typename E> inline constexpr bool has_insert_after_v = has_insert_after<T, E>::value;
        
        template <typename T> struct has_clear { static constexpr bool value = op_exists_v<detail::ClearOp, T>; };
        template <typename T> inline constexpr bool has_clear_v = has_clear<T>::value;
    
        template <typename Container, typename Element>
        constexpr void append(Container & container, Element && element)
        {
            if constexpr ( std::is_const_v<Container> )
                return;
            else if constexpr ( has_push_back_v<Container, Element> )
                container.push_back(std::forward<Element>(element));
            else if constexpr ( has_push_v<Container, Element> )
                container.push(std::forward<Element>(element));
            else if constexpr ( has_insert_v<Container, Element> )
                container.insert(std::forward<Element>(element));
            else if constexpr ( has_insert_after_v<Container, Element> )
            {
                auto last = container.before_begin();
                for ( auto curr = last; curr != container.end(); last = curr++);
                container.insert_after(last, std::forward<Element>(element));
            }
        }

        template <typename Iterable>
        constexpr bool isEmpty(const Iterable & iterable)
        {
            if constexpr ( std::is_array_v<Iterable> )
                return std::extent_v<Iterable> == 0;
            else
                return iterable.empty();
        }

        template <typename Iterable>
        constexpr void clear(Iterable & iterable)
        {
            if constexpr ( std::is_const_v<Iterable> )
                return;
            else if constexpr ( has_clear_v<Iterable> )
                iterable.clear();
            else if constexpr ( is_adaptor_v<Iterable> )
            {
                while ( !iterable.empty() )
                    iterable.pop();
            }
        }

        struct Unspecialized {}; // Primary templates (and not specializations), should inherit from Unspecialized
        template <typename T> struct is_specialized { static constexpr bool value = !std::is_base_of_v<Unspecialized, T>; };
        template <typename T> inline constexpr bool is_specialized_v = is_specialized<T>::value;

        template <typename F, size_t ... Is>
        constexpr void forIndex(size_t i, std::index_sequence<Is...>, F f) {
            (void)((i == Is && (f(std::integral_constant<size_t, Is>{}), true)) || ...);
        }

        template <size_t Max, typename F>
        constexpr void forIndex(size_t i, F && f) {
            if constexpr ( Max > 0 )
                forIndex(i, std::make_index_sequence<Max>{}, std::forward<F>(f));
        }

        template <typename F, size_t ...Is>
        constexpr void forIndexes(std::index_sequence<Is...>, F f) {
            (void)((f(std::integral_constant<size_t, Is>{}), true) && ...);
        }

        // For integral constants 0 to Max-1
        template <size_t Max, typename F>
        constexpr void forIndexes(F && f) {
            if constexpr ( Max > 0 )
                forIndexes(std::make_index_sequence<Max>{}, std::forward<F>(f));
        }

        template <typename F, size_t ...Is>
        constexpr void packIndexes(std::index_sequence<Is...>, F f) {
            f(std::integral_constant<size_t, Is>{}...);
        }

        // For integral constants 0 to Max-1
        template <size_t Max, typename F>
        constexpr void packIndexes(F && f) {
            if constexpr ( Max > 0 )
                packIndexes(std::make_index_sequence<Max>(), std::forward<F>(f));
        }

        template <typename T, typename ... Ts> struct index_of {
            static constexpr size_t value = [](){
                size_t index = sizeof...(Ts);
                RareTs::forIndexes<sizeof...(Ts)>([&](auto I) {
                    if constexpr ( std::is_same_v<RareTs::remove_cvref_t<T>,
                            RareTs::remove_cvref_t<std::tuple_element_t<decltype(I)::value, std::tuple<Ts...>>>> )
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
        template <typename T, typename ... Ts> inline constexpr size_t index_of_v = index_of<T, Ts...>::value;

        template <size_t ... Is>
        struct Masked
        {
            template <typename T, typename Tuple>
            static constexpr size_t indexOf = [](){
                size_t result = std::tuple_size_v<Tuple>;
                (void)((std::is_same_v<T, std::tuple_element_t<Is, Tuple>> ? (result = Is, true) : false) || ...);
                return result;
            }();

            template <template <typename...> class Of, typename Tuple>
            static constexpr size_t indexOfSpecialization = [](){
                size_t result = std::tuple_size_v<Tuple>;
                (void)((RareTs::is_specialization_v<std::tuple_element_t<Is, Tuple>, Of> ? (result = Is, true) : false) || ...);
                return result;
            }();

            template <typename T, typename Tuple>
            static constexpr const auto & get(const Tuple & tuple) {
                constexpr size_t index = indexOf<T, Tuple>;
                static_assert(index < std::tuple_size_v<Tuple>, "The given type \"T\" does not appear in the tuple at any of the given indexes \"Is...\"");
                return std::get<index>(tuple);
            }

            template <template <typename...> class Of, typename Tuple>
            static constexpr const auto & get(const Tuple & tuple) {
                constexpr size_t index = indexOfSpecialization<Of, Tuple>;
                static_assert(index < std::tuple_size_v<Tuple>,
                    "The given specialization \"Of\" does not appear in the tuple at any of the given indexes \"Is...\"");
                return std::get<index>(tuple);
            }

            template <typename T, typename Tuple, typename Function>
            static constexpr void forEach(const Tuple & tuple, Function function) {
                RareTs::forIndexes(std::index_sequence<Is...>{}, [&](auto I){
                    if constexpr ( std::is_same_v<T, std::tuple_element_t<decltype(I)::value, Tuple>> )
                        function(std::get<decltype(I)::value>(tuple));
                });
            }

            template <template <typename...> class Of, typename Tuple, typename Function>
            static constexpr void forEach(const Tuple & tuple, Function function) {
                RareTs::forIndexes(std::index_sequence<Is...>{}, [&](auto I){
                    if constexpr ( RareTs::is_specialization_v<std::tuple_element_t<decltype(I)::value, Tuple>, Of> )
                        function(std::get<decltype(I)::value>(tuple));
                });
            }
        };

        template <typename ... Ts> struct type_list {
            template <typename T> struct has : std::bool_constant<(std::is_same_v<Ts, T> || ...)> {};
            template <typename T> static constexpr bool has_v = has<T>::value;
            template <template <typename ...> class Of> struct has_specialization : std::bool_constant<(is_specialization_v<Ts, Of> || ...)> {};
            template <template <typename ...> class Of> static constexpr bool has_specialization_v = has_specialization<Of>::value;
            template <template <typename ...> class Of> class get_specialization {
                template <typename ... Us> struct get { using type = void; };
                template <typename U, typename ... Us> struct get<U, Us...> : std::conditional<is_specialization_v<U, Of>, U, typename get<Us...>::type> {};
            public:
                using type = typename get<Ts...>::type;
            };
            template <template <typename ...> class Of> using get_specialization_t = typename get_specialization<Of>::type;
        };
        template <typename ...Ts> struct type_list<const std::tuple<Ts...>> : type_list<Ts...> {};
        template <typename ...Ts> struct type_list<std::tuple<Ts...>> : type_list<Ts...> {};

        template <typename T>
        constexpr auto toStr()
        {
            #if defined(__GNUC__) || defined(__clang__)
            std::string_view view = __PRETTY_FUNCTION__;
            view.remove_prefix(view.find_first_of("=")+1);
            view.remove_prefix(view.find_first_not_of(" "));
            view.remove_suffix(view.size()-view.find_last_of("]"));
            #elif defined(_MSC_VER)
            std::string_view view = __FUNCSIG__;
            view.remove_prefix(view.find_first_of("<")+1);
            view.remove_suffix(view.size()-view.find_last_of(">"));
            #else
            std::string_view view = "unknown";
            #endif
            return view;
        }

        template <typename T>
        struct TypeName
        {
            static constexpr size_t length = toStr<T>().size();

            constexpr TypeName() : value() {
                auto view = toStr<T>();
                for ( size_t i=0; i<length; i++ )
                    value[i] = view[i];

                value[length] = '\0';
            }
            char value[length+1];

            constexpr operator const char*() const { return &value[0]; };
        };

        template <class Adaptor>
        inline const typename Adaptor::container_type & baseContainer(const Adaptor & adaptor) {
            struct AdaptorSubClass : Adaptor {
                static const typename Adaptor::container_type & get(const Adaptor & adaptor) {
                    return adaptor.*(&AdaptorSubClass::c);
                }
            };
            return AdaptorSubClass::get(adaptor);
        }

	    constexpr size_t fnv1aHash(const std::string_view & str) noexcept { // Returns a fnv1a hash of the given string
		    constexpr size_t seed = static_cast<size_t>([](){
                if constexpr ( sizeof(size_t) >= 8 ) return 14695981039346656037ULL;
                else return 2166136261U;
            }());
		    constexpr size_t factor = static_cast<size_t>([](){
                if constexpr ( sizeof(size_t) >= 8 ) return 1099511628211ULL;
                else return 16777619U;
            }());
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
            struct StringIndex { std::string_view str {}; size_t index = 0; }; // The string and the index with which it was passed in to the map
            struct HashMap {
                Bucket stringBucket[totalBuckets] {};
                StringIndex stringIndex[total];

                static constexpr void swap(size_t & l, size_t & r, StringIndex & l2, StringIndex & r2) noexcept { // sort's swap helper
                    size_t t = std::move(l); l = std::move(r); r = std::move(t); StringIndex t2 = std::move(l2); l2 = std::move(r2); r2 = std::move(t2);
                }

                constexpr void sort(size_t (&hashes)[total], int l = 0, int r = int(total)-1) noexcept { // sort hashes, keep indexes parallel
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
                size_t dupedHashes[total] { (fnv1aHash(std::string_view(s)) & mask)... };
            
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
            static constexpr size_t dupedHashes[total] { (fnv1aHash(std::string_view(s)) & mask)... };

        public:
            static constexpr size_t indexOf(const std::string_view & str) noexcept {
                const Bucket & bucket = hashMap.stringBucket[fnv1aHash(str) & mask];
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
            static constexpr size_t indexOf(const std::string_view &) noexcept { return std::numeric_limits<size_t>::max(); }
        };

        template <template <typename...> class P, typename ... Ts> class type_mask { // Values for which P<Ts> is false are removed
            struct calc {
                size_t total = 0;
                size_t typeIndex[sizeof...(Ts)]{};
                constexpr calc() noexcept {
                    size_t skipped = 0; // typeIndex[total] set to total+skipped and total incremented if predicate passes, skipped incremented otherwise
                    (((P<Ts>::value && ((typeIndex[total] = total+skipped) < 0 || ++total)) || ++skipped) && ...);
                }
            };
            static constexpr calc c{}; // Contains "total" types that matched the predicate and index of matching types in the type list
            template <size_t ... Is> static constexpr std::index_sequence<(c.typeIndex[Is])...> filterSeq(std::index_sequence<Is...>);
        public:
            using indexes = decltype(filterSeq(std::make_index_sequence<c.total>())); // index_sequence of indexes in Ts that match the predicate
        };
        template <template <typename> class P> struct type_mask<P> { using indexes = std::index_sequence<>; };

        template <template <typename> class P, typename ... Ts> struct tuple_type_mask;
        template <template <typename> class P, typename ... Ts> struct tuple_type_mask<P, std::tuple<Ts...>> : type_mask<P, Ts...> {};

        template <typename T, size_t ... Is> constexpr auto referenceTuple(const T & t, std::index_sequence<Is...>) {
            return std::tie(std::get<Is>(t)...);
        }

        template <template <typename> class P, typename T> constexpr auto referenceTuple(const T & t) {
            return referenceTuple(t, typename tuple_type_mask<P, T>::indexes{});
        }

        template <size_t Remove, size_t ... Is> class remove_index { // Removes "Remove" from "Is", only valid if count with value "Remove" in Is == 1
            struct calc {
                size_t total = 0;
                size_t index[sizeof...(Is)]{};
                constexpr calc() noexcept {
                    (((Remove != Is && ((index[total] = Is) < 0 || ++total)) || true) && ...);
                }
            };
            static constexpr calc c{};
            template <size_t ... Js> static constexpr std::index_sequence<c.index[Js]...> filterSeq(std::index_sequence<Js...>);
        public:
            using type = decltype(filterSeq(std::make_index_sequence<sizeof...(Is)-1>()));
        };
    }

    // Contains support for working with reflected members, the definition for the REFLECT macro and non-generic supporting macros
    inline namespace Reflection
    {
        inline namespace Annotations
        {
            #define NOTE(target, ...) static inline constexpr auto target##_note { std::tuple { __VA_ARGS__ } };

            using NoNote = std::tuple<>;

            inline constexpr NoNote noNote {};

            template <class T, typename> struct sub_notes { static constexpr auto & value = T::notes; };
            template <class T, size_t I> // T has "T::notes", notes with sub-notes have a non-static member "notes"
            struct sub_notes<T, std::integral_constant<size_t, I>> { static constexpr auto & value = std::get<I>(T::notes).notes; };

            template <typename Root, typename SubIndex = void, size_t ... Is> struct AnnotationsType
            {
                static constexpr auto & notes = sub_notes<Root, SubIndex>::value; // Annotation values
                using Notes = RareTs::remove_cvref_t<decltype(notes)>; // Annotation types

                template <typename Annotation>
                static constexpr bool hasNote() { return (std::is_same_v<Annotation, std::tuple_element_t<Is, Notes>> || ...); }

                template <template <typename ...> class Of>
                static constexpr bool hasNote() { return (RareTs::is_specialization_v<std::tuple_element_t<Is, Notes>, Of> || ...); }

                template <typename Annotation>
                static constexpr auto & getNote() { return RareTs::Masked<Is...>::template get<Annotation, Notes>(notes); }

                template <template <typename ...> class Of>
                static constexpr auto & getNote() { return RareTs::Masked<Is...>::template get<Of>(notes); }

                template <typename Function>
                static constexpr void forEachNote(Function function) { (function(std::get<Is>(notes)), ...); }
            
                // function(annotation) [filtered to type 'Annotation']
                template <typename Annotation, typename Function> static constexpr void forEachNote(Function && function) {
                    RareTs::Masked<Is...>::template forEach<Annotation>(notes, std::forward<Function>(function));
                }

                // function(annotation) [filtered to annotations specializing 'Of']
                template <template <typename ...> class Of, typename Function> static constexpr void forEachNote(Function && function) {
                    RareTs::Masked<Is...>::template forEach<Of>(notes, std::forward<Function>(function));
                }
            };
            template <typename Root, typename SubIndex> struct AnnotationsType<Root, SubIndex>
            {
                static constexpr auto & notes = RareTs::noNote; // Annotation values
                using Notes = RareTs::NoNote; // Annotation types
                template <typename Annotation> static constexpr bool hasNote() { return false; }
                template <template <typename ...> class Of> static constexpr bool hasNote() { return false; }
                template <typename Annotation> static constexpr auto & getNote() = delete;
                template <template <typename ...> class Of> static constexpr auto & getNote() = delete;
                template <typename Function> static constexpr void forEachNote(Function) {}
                template <typename Annotation, typename Function> static constexpr void forEachNote(Function &&) {}
                template <template <typename ...> class Of, typename Function> static constexpr void forEachNote(Function &&) {}
            };

            namespace detail {
                template <typename Root, size_t ... Is>
                constexpr AnnotationsType<Root, void, Is...> annotationsType(std::index_sequence<Is...>);

                template <typename Root, size_t SubIndex, size_t ... Is>
                constexpr AnnotationsType<Root, std::integral_constant<size_t, SubIndex>, Is...> subAnnotationsType(std::index_sequence<Is...>);
            }
        }
        
        enum class AccessMod {
            Public,
            Protected,
            Private
        };
        
        template <typename T> struct Proxy;
    
        template <typename T, typename = void> struct is_proxied : std::false_type {};
        template <typename T> struct is_proxied<T, std::void_t<decltype(Proxy<T>::Class::I_::N_)>> : std::true_type {};
        template <typename T> inline constexpr bool is_proxied_v = is_proxied<T>::value;

        #ifdef __clang__
        template <typename T> constexpr void classType(T);

        namespace detail {
            template <typename T, size_t MemberIndex>
            struct is_private_member : T
            {
                template <class AlwaysVoid, template<class...> class Op, class... Args> struct op_exists { static constexpr bool value = false; };
                template <template<class...> class Op, class... Args>
                struct op_exists<std::void_t<Op<Args...>>, Op, Args...> { static constexpr bool value = true; };

                template <typename U = typename decltype(classType(RareTs::type_tag<RareTs::Proxy<RareTs::remove_cvref_t<T>>>{}))::template A_<MemberIndex, T>>
                static constexpr bool value = !op_exists<void, U::template t, T>::value && !op_exists<void, U::template p, T>::value;
            };
        }
        #endif

        struct Class
        {
            // These 5 structures are used as the defaults/fallbacks inside the REFLECT/REFLECT_NOTED macros for the various member component templates
            struct NullptrType { using type = std::nullptr_t; };
            struct EmptyComponent {
                using type = void;
                static constexpr std::nullptr_t p = nullptr;
                template <typename T> static constexpr auto & s() { return p; }
                template <typename T> static constexpr auto & i(T &&) { return p; }
            };
            struct NonPointable { static constexpr std::nullptr_t p = nullptr; };
            struct NonNoted { static constexpr std::tuple notes{}; };
            struct NoOffset { static constexpr size_t o() { return std::numeric_limits<size_t>::max(); }; };

            template <typename T, typename R, typename ... Args>
            struct OverloadIdentifier
            {
                template <size_t i, typename = std::remove_reference_t<T>> struct Quals;
                template <typename D> struct Quals<0, D> {
                    static constexpr auto of(R(*p)(Args...)) { return p; }
                    static constexpr auto of(R(*p)(Args...) noexcept) { return p; }
                    static constexpr auto of(R(D::*p)(Args...)) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) noexcept) { return p; };
                };
                template <typename D> struct Quals<1, D> {
                    static constexpr auto of(R(D::*p)(Args...) const) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) const noexcept) { return p; };
                };
                template <typename D> struct Quals<2, D> {
                    static constexpr auto of(R(D::*p)(Args...) volatile) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) volatile noexcept) { return p; };
                };
                template <typename D> struct Quals<3, D> {
                    static constexpr auto of(R(D::*p)(Args...) const volatile) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) const volatile noexcept) { return p; };
                };
                template <typename D> struct Quals<4, D> {
                    static constexpr auto of(R(D::*p)(Args...) &) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) & noexcept) { return p; };
                };
                template <typename D> struct Quals<5, D> {
                    static constexpr auto of(R(D::*p)(Args...) const &) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) const & noexcept) { return p; };
                };
                template <typename D> struct Quals<6, D> {
                    static constexpr auto of(R(D::*p)(Args...) volatile &) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) volatile & noexcept) { return p; };
                };
                template <typename D> struct Quals<7, D> {
                    static constexpr auto of(R(D::*p)(Args...) const volatile &) { return p; }; 
                    static constexpr auto of(R(D::*p)(Args...) const volatile & noexcept) { return p; }; 
                };
                template <typename D> struct Quals<8, D> {
                    static constexpr auto of(R(D::*p)(Args...) &&) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) && noexcept) { return p; };
                };
                template <typename D> struct Quals<9, D> {
                    static constexpr auto of(R(D::*p)(Args...) const &&) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) const && noexcept) { return p; };
                };
                template <typename D> struct Quals<10, D> {
                    static constexpr auto of(R(D::*p)(Args...) volatile &&) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) volatile && noexcept) { return p; };
                };
                template <typename D> struct Quals<11, D> {
                    static constexpr auto of(R(D::*p)(Args...) const volatile &&) { return p; };
                    static constexpr auto of(R(D::*p)(Args...) const volatile && noexcept) { return p; };
                };
    
                using Id = Quals<(std::is_rvalue_reference_v<T> ? 8 : (std::is_lvalue_reference_v<T> ? 4 : 0))
                    + (std::is_volatile_v<std::remove_reference_t<T>> ? 2 : 0) + std::is_const_v<std::remove_reference_t<T>>>;
            };

            template <typename T> struct unproxy { using type = T; };
            template <typename T> struct unproxy<Proxy<T>> { using type = T; };
            template <typename T> using unproxy_t = typename unproxy<T>::type;

            template <typename P, typename T> struct forward_proxy { using type = P &; };
            template <typename P, typename T> struct forward_proxy<P, const T> { using type = const P &; };
            template <typename P, typename T> struct forward_proxy<P, volatile T> { using type = volatile P &; };
            template <typename P, typename T> struct forward_proxy<P, const volatile T> { using type = const volatile P &; };
            template <typename P, typename T> struct forward_proxy<P, T &> { using type = P &; };
            template <typename P, typename T> struct forward_proxy<P, const T &> { using type = const P &; };
            template <typename P, typename T> struct forward_proxy<P, volatile T &> { using type = volatile P &; };
            template <typename P, typename T> struct forward_proxy<P, const volatile T &> { using type = const volatile P &; };
            template <typename P, typename T> struct forward_proxy<P, T &&> { using type = P &&; };
            template <typename P, typename T> struct forward_proxy<P, const T &&> { using type = const P &&; };
            template <typename P, typename T> struct forward_proxy<P, volatile T &&> { using type = volatile P &&; };
            template <typename P, typename T> struct forward_proxy<P, const volatile T &&> { using type = const volatile P &&; };
            template <typename P, typename T> using forward_proxy_t = typename forward_proxy<P, T>::type;

            template <typename T, typename=typename T::type, typename=typename T::Notes, typename=decltype(T::index)>
            static constexpr std::true_type typeIsMember(int);
            template <typename T> static constexpr std::false_type typeIsMember(unsigned int);

            template <template <typename...> class Filter, typename ...Ts> static constexpr Filter<Ts...> filterResult(int);
            template <template <typename...> class Filter, typename T, typename ...Ts> static constexpr std::false_type filterResult(unsigned);

            #if !defined(__clang__)
            template <class T, class=void> struct clazz { using type = void; };
            template <class T> struct clazz<T, std::void_t<typename T::Class>> { using type = typename T::Class; };
            template <class T> struct clazz<T, std::void_t<typename Proxy<T>::Class>> { using type = typename Proxy<T>::Class; };
            template <typename T> using class_t = typename clazz<T>::type;
            #else
            template <typename T> using class_t = decltype(classType(RareTs::type_tag<RareTs::Proxy<RareTs::remove_cvref_t<T>>>{}));
            #endif

            template <typename T> struct is_reflected { static constexpr bool value = !std::is_void_v<class_t<T>>; };

            template <typename T> static constexpr size_t member_count = class_t<T>::I_::N_;

            template <typename ReflectedObject, size_t MemberIndex> struct validate
            {
                #if defined(__GNUC__) && !defined(__clang__)
                template <class U> static constexpr std::enable_if_t<!std::is_void_v<typename U::type>, int> member(int) { return 0; }
                template <class> [[deprecated(
                    "Invalid member present in REFLECT member list. Suppress for overloads by adding a NOTE for the member or defining RARE_NO_VALIDATE.")]]
                static constexpr int member(unsigned) { return 0; }
                #else
                template <class Field, class PointerType, class Notes> static constexpr std::enable_if_t<
                    !std::is_base_of_v<EmptyComponent, Field> || !std::is_base_of_v<NullptrType, PointerType> || !std::is_base_of_v<NonNoted, Notes>, int>
                member(int) { return 0; }

                template <class, class, class> [[deprecated(
                    "Invalid member present in REFLECT member list. Suppress for overloads by adding a NOTE for the member or defining RARE_NO_VALIDATE.")]]
                static constexpr int member(unsigned) { return 0; }
                #endif
            };

            template <typename T, size_t I> using member_type = typename class_t<T>::template F_<I>::type;
            template <typename T, size_t I> using member_pointer_type = typename class_t<T>::template Q_<I>::type;
            template <typename T, size_t I> static constexpr auto member_name = class_t<T>::template N_<I>::n;
            template <typename T, size_t I> static constexpr std::string_view member_type_name { RareTs::toStr<typename class_t<T>::template F_<I>::type>() };
            template <typename T, size_t I> static constexpr auto member_pointer = class_t<T>::template P_<I>::p;
            template <typename T, size_t I> static constexpr size_t member_offset = class_t<T>::template O_<I>::o();
            template <typename T, size_t I> static constexpr auto & member_note = class_t<T>::template E_<I>::notes;
            template <typename T, size_t I> using member_note_wrapper = typename class_t<T>::template E_<I>;
            template <typename T, size_t I> using member_note_type = RareTs::remove_cvref_t<decltype(class_t<T>::template E_<I>::notes)>;
            template <typename T, size_t I> static constexpr size_t member_note_count = std::tuple_size_v<member_note_type<T, I>>;
            template <typename T, size_t I> static constexpr auto & memberValue() {
                using U = class_t<RareTs::remove_cvref_t<T>>;
                return U::template F_<I>::template s<typename U::B_>();
            }
            template <size_t I, typename T> static constexpr auto & memberValue(T && t) {
                using U = class_t<RareTs::remove_cvref_t<T>>;
                return U::template F_<I>::template i(static_cast<forward_proxy_t<typename U::B_, T>>(t));
            }

            template <typename T, size_t ... Is> using member_types = std::tuple<typename class_t<T>::template F_<Is>::type...>;
            template <typename T, size_t ... Is> using member_pointer_types = std::tuple<typename class_t<T>::template Q_<Is>::type...>;
            template <typename T, size_t ... Is> static constexpr auto member_names { class_t<T>::template N_<Is>::n... };
            template <typename T, size_t ... Is>
            static constexpr std::string_view member_type_names[] { RareTs::toStr<typename class_t<T>::template F_<Is>::type>()... };
            template <typename T, size_t ... Is> static constexpr auto member_pointers = std::tuple { class_t<T>::template P_<Is>::p... };
            template <typename T, size_t ... Is> static constexpr size_t member_offsets[] = { class_t<T>::template O_<Is>::o()... };
            template <typename T, size_t ... Is> static constexpr auto member_notes = std::forward_as_tuple( class_t<T>::template E_<Is>::notes... );
            template <typename T, size_t ... Is> using member_note_types = std::tuple<decltype(class_t<T>::template E_<Is>::notes)...>;
            template <typename T, size_t ... Is> static constexpr auto memberValues() {
                using U = class_t<RareTs::remove_cvref_t<T>>;
                return std::forward_as_tuple( U::template F_<Is>::template s<typename U::B_>()... );
            }
            template <size_t ... Is, typename T> static constexpr auto memberValues(T && t) {
                using U = class_t<RareTs::remove_cvref_t<T>>;
                return std::forward_as_tuple( U::template F_<Is>::i(static_cast<forward_proxy_t<typename U::B_, T>>(t))... );
            }

            template <typename T, size_t I> static constexpr bool is_function =
                std::is_function_v<member_type<T, I>> || std::is_member_function_pointer_v<member_pointer_type<T, I>>;

            template <typename T, size_t I> static constexpr bool is_static =
                !std::is_member_pointer_v<member_pointer_type<T, I>> && !std::is_same_v<std::nullptr_t, member_pointer_type<T, I>>;

            template <typename T> static constexpr size_t class_note_count = std::tuple_size_v<RareTs::remove_cvref_t<decltype(class_t<T>::notes)>>;

            template <typename T, size_t I> static constexpr auto & class_note = std::get<I>(class_t<T>::notes);
            template <typename T, size_t I> using class_note_type = std::tuple_element_t<I, RareTs::remove_cvref_t<decltype(class_t<T>::notes)>>;

            template <typename T> static constexpr auto & class_notes = class_t<T>::notes;
            template <typename T> using class_notes_type = RareTs::remove_cvref_t<decltype(class_t<T>::notes)>;

            template <typename T> using index_of = typename class_t<T>::I_;

            template <template <size_t> class C, typename T, size_t I> using adapt_member = typename class_t<T>::template M_<I, C>;
            template <template <size_t> class C, typename T, size_t I> using adapt_member_type = typename class_t<T>::template D_<I, C>;

            template <size_t I, typename T> static constexpr auto & cleanMemberValue(T && t) {
                using U = RareTs::remove_cvref_t<T>;
                if constexpr ( !is_function<U, I> )
                    return memberValue<I>(t);
                else
                    return std::add_lvalue_reference_t<member_pointer_type<U, I>>(class_t<U>::template P_<I>::p);
            }

            template <typename T, size_t I> static constexpr auto & cleanMemberValue() {
                if constexpr ( is_static<T, I> && !is_function<T, I> )
                    return class_t<T>::template F_<I>::template s<T>();
                else
                    return std::add_lvalue_reference_t<member_pointer_type<T, I>>(class_t<T>::template P_<I>::p);
            }

            template <typename T, typename F, size_t ... Is> static constexpr decltype(auto) packMemberValues(F && f, std::index_sequence<Is...>) {
                return f(cleanMemberValue<T, Is>()...);
            }
            template <typename T, typename F> static constexpr auto packMemberValues(F && f) {
                return packMemberValues<T>(std::forward<F>(f), std::make_index_sequence<member_count<T>>());
            }

            template <typename T, typename F, size_t ... Is> static constexpr decltype(auto) packMemberValues(T && t, F && f, std::index_sequence<Is...>) {
                return f(cleanMemberValue<Is>(t)...);
            }
            template <typename T, typename F> static constexpr decltype(auto) packMemberValues(T && t, F && f) {
                using U = RareTs::remove_cvref_t<T>;
                return packMemberValues(std::forward<T>(t), std::forward<F>(f), std::make_index_sequence<member_count<U>>());
            }

            template <typename ... Args>
            class overload
            {
                template <typename Q, size_t I, typename = void> struct ptr : RareTs::type_id<std::nullptr_t> {
                    static constexpr std::nullptr_t pointer = nullptr;
                };
                template <typename Q, size_t I> struct ptr<Q, I,
                    std::void_t<typename class_t<RareTs::remove_cvref_t<Q>>::template L_<I, Args...>::template W_<Q>>>
                    : RareTs::type_id<typename class_t<RareTs::remove_cvref_t<Q>>::template L_<I, Args...>::template W_<Q>>
                {
                    static constexpr auto pointer = class_t<RareTs::remove_cvref_t<Q>>::template L_<I, Args...>::template p<Q>;
                };

            public:
                template <typename Q, size_t I>
                using return_type = typename class_t<RareTs::remove_cvref_t<Q>>::template L_<I, Args...>::template R_<Q>;

                template <typename Q, size_t I>
                using pointer_type = typename ptr<Q, I>::type;

                template <typename Q, size_t I>
                static constexpr auto pointer = ptr<Q, I>::pointer;

                template <typename Q, size_t ... Is>
                using return_types = std::tuple<typename class_t<RareTs::remove_cvref_t<Q>>::template L_<Is, Args...>::template R_<Q>...>;

                template <typename Q, size_t ... Is>
                using pointer_types = std::tuple<typename class_t<RareTs::remove_cvref_t<Q>>::template L_<Is, Args...>::template W_<Q>...>;

                template <typename Q, size_t ... Is>
                static constexpr auto pointers = std::tuple { class_t<RareTs::remove_cvref_t<Q>>::template L<Is, Args...>::template p<Q>... };
            };

            template <typename T, typename = void> struct class_notes_type_defaulted { using type = RareTs::NoNote; };
            template <typename T> struct class_notes_type_defaulted<T, std::enable_if_t<is_reflected<T>::value>> {
                using type = RareTs::remove_cvref_t<decltype(class_t<T>::notes)>;
            };
            template <typename T> using class_notes_t = typename class_notes_type_defaulted<T>::type;

            template <typename T, size_t MemberIndex> using MemberAnnotationsType = decltype(
                RareTs::Reflection::Annotations::detail::template annotationsType<typename RareTs::Class::template member_note_wrapper<T, MemberIndex>>(
                    std::make_index_sequence<RareTs::Class::template member_note_count<T, MemberIndex>>()));

            #ifdef __clang__
            // Overloads (and references on MSVC < 1932 / Visual Studios <= 2019) are unsupported/always private
            template <typename T, size_t MemberIndex, typename U = typename Class::class_t<T>::template A_<MemberIndex>>
            struct access_modifier
            {
                static constexpr AccessMod value = [](){
                    if constexpr ( Class::MemberAnnotationsType<T, MemberIndex>::template hasNote<AccessMod>() )
                        return Class::MemberAnnotationsType<T, MemberIndex>::template getNote<AccessMod>();
                    else if constexpr ( RareTs::op_exists<void, U::template t, T>::value || RareTs::op_exists<void, U::template p, T>::value )
                        return AccessMod::Public;
                    else if constexpr ( detail::is_private_member<T, MemberIndex>::template value<> ) // this actually detects not protected/defaults to true
                        return AccessMod::Private;
                    else
                        return AccessMod::Protected;
                }();
            };

            #else
            template <typename T, size_t I, typename ... Ts>
            static constexpr size_t accessible() {
                return decltype(id(typename RareTs::Class::class_t<T>::template A_<I, T, Ts...>{}, 0, 0))::value;
            }

            template <typename T, size_t MemberIndex> // Overloads (and references on MSVC < 1932 / Visual Studios <= 2019) are unsupported/always private
            struct access_modifier
            {
                static constexpr AccessMod value = [](){
                    if constexpr ( MemberAnnotationsType<T, MemberIndex>::template hasNote<AccessMod>() )
                        return MemberAnnotationsType<T, MemberIndex>::template getNote<AccessMod>();
                    else if constexpr ( accessible<T, MemberIndex>() != 2 )
                        return AccessMod::Public;
                    else if constexpr ( accessible<T, MemberIndex, T>() != 2 )
                        return AccessMod::Protected;
                    else
                        return AccessMod::Private;
                }();
            };
            #endif
        };

        template <typename T> using is_reflected = typename Class::is_reflected<T>;
        template <typename T> inline constexpr bool is_reflected_v = is_reflected<T>::value;

        template <typename T> struct is_member { static constexpr bool value = decltype(RareTs::Class::typeIsMember<T>(0))::value; };
        template <typename T> inline constexpr bool is_member_v = is_member<T>::value;
        
        template <typename T> using enable_if_member_t = std::enable_if_t<is_member_v<T>>;

        /** filter_result uses Filter<Member, Ts...> if it results in a valid expression, otherwise it uses Filter<Member::type, Ts...>
            This allows the filter to be based on member metadata (e.g. whether a variable is static), or operate purely on the member type */
        template <template <typename...> class Filter, typename Member, typename ...Ts> struct passes_filter : std::bool_constant<
            decltype(RareTs::Class::filterResult<Filter, Member, Ts...>(0))::value ||
            decltype(RareTs::Class::filterResult<Filter, typename Member::type, Ts...>(0))::value> {};

        template <template <typename...> class Filter, typename T, typename ...Ts>
        inline constexpr bool passes_filter_v = passes_filter<Filter, T, Ts...>::value;

        struct Filter {
            template <typename Member, typename = enable_if_member_t<Member>> struct None : std::true_type {};
            template <typename Member, typename = enable_if_member_t<Member>>
            struct IsData : std::bool_constant<Member::isData> {}; // Data is defined as non-functions (raw data, pointers, references)
            template <typename Member, typename = enable_if_member_t<Member>> struct IsFunction : std::bool_constant<Member::isFunction> {};
            template <typename Member, typename = enable_if_member_t<Member>> struct IsInstanceMember : std::bool_constant<!Member::isStatic> {};
            template <typename Member, typename = enable_if_member_t<Member>> struct IsStaticMember: std::bool_constant<Member::isStatic> {};
            template <typename Member, typename = enable_if_member_t<Member>>
            struct IsInstanceData : std::bool_constant<!Member::isStatic && Member::isData> {};
            template <typename Member, typename = enable_if_member_t<Member>>
            struct IsInstanceFunction : std::bool_constant<!Member::isStatic && Member::isFunction> {};
            template <typename Member, typename = enable_if_member_t<Member>>
            struct IsStaticData : std::bool_constant<Member::isStatic && Member::isData> {};
            template <typename Member, typename = enable_if_member_t<Member>>
            struct IsStaticFunction : std::bool_constant<Member::isStatic && Member::isFunction> {};
		
            template <template <typename...> class Of> struct Is {
                template <typename Member, typename = enable_if_member_t<Member>> struct Specialization : std::bool_constant<
                    RareTs::is_specialization_v<typename Member::type, Of>> {};
            };
        };

        inline namespace Inheritance
        {
            template <typename T, typename ...Ts> struct NotedSuper {
                using type = T;
                std::tuple<Ts...> notes;
                using Notes = decltype(notes);

                constexpr NotedSuper(const Ts & ... args) : notes { args... } {}
            };

            template <typename T> struct SuperClass {
                using type = T;
                static constexpr NoNote notes{};
                using Notes = NoNote;

                template <typename ... Ts> constexpr NotedSuper<T, Ts...> operator() (const Ts & ... args) const { return NotedSuper<T, Ts...>(args...); }
            };

            template <typename T> inline constexpr SuperClass<T> Super{};
        
            template <typename T> struct is_super_note { static constexpr bool value = false; };
            template <typename T> struct is_super_note<const T> { static constexpr bool value = is_super_note<T>::value; };
            template <typename T> struct is_super_note<SuperClass<T>> { static constexpr bool value = true; };
            template <typename T, typename ...Ts> struct is_super_note<NotedSuper<T, Ts...>> { static constexpr bool value = true; };
            template <typename T> inline constexpr bool is_super_note_v = is_super_note<T>::value;

            namespace detail {
                template <typename T> struct SuperNotes { static constexpr auto notes = RareTs::referenceTuple<is_super_note>(RareTs::Class::class_notes<T>); };
                template <typename T> inline constexpr size_t superNoteCount = std::tuple_size_v<std::remove_const_t<decltype(SuperNotes<T>::notes)>>;
            }

            template <size_t SuperIndex, typename SuperType, typename Notes> struct SuperInfo : Notes
            {
                using type = SuperType;
                static constexpr size_t index = SuperIndex;
            };
            
            template <typename SubClass, size_t ... Is> class Inherit
            {
                using Notez = detail::SuperNotes<SubClass>;
                using Notes = RareTs::remove_cvref_t<decltype(Notez::notes)>;

            public:
                template <size_t I> using SuperType = typename RareTs::remove_cvref_t<std::tuple_element_t<I, Notes>>::type;
                template <size_t I> using SuperInfo = Inheritance::SuperInfo<I, SuperType<I>, decltype(
                    RareTs::Reflection::Annotations::detail::subAnnotationsType<Notez, I>(std::make_index_sequence<std::tuple_size_v<
                        typename std::remove_reference_t<std::tuple_element_t<I, Notes>>::Notes>>()))>;
                
                static constexpr size_t total = sizeof...(Is);
                    
                template <typename Function>
                static constexpr void forEach(Function function) {
                    (function(SuperInfo<Is>{}), ...);
                }

                template <typename Function, typename U, typename = std::enable_if_t<std::is_same_v<SubClass,std::decay_t<U>>>>
                static constexpr void forEach(U && object, Function function) {
                    if constexpr ( std::is_const_v<std::remove_reference_t<U>> ) {
                        (function(SuperInfo<Is>{}, (const SuperType<Is> &)object), ...);
                    } else {
                        (function(SuperInfo<Is>{}, (SuperType<Is> &)object), ...);
                    }
                }

                template <typename Function, typename U, typename = std::enable_if_t<std::is_same_v<SubClass,std::decay_t<U>>>>
                static constexpr void forEachSuper(U && object, Function function) {
                    if constexpr ( std::is_const_v<std::remove_reference_t<U>> ) {
                        (function((const SuperType<Is> &)object), ...);
                    } else {
                        (function((SuperType<Is> &)object), ...);
                    }
                }

                template <typename Function>
                static constexpr void at(size_t superIndex, Function function) {
                    RareTs::forIndex<total>(superIndex, [&](auto I) {
                        function(SuperInfo<decltype(I)::value>{});
                    });
                }

                template <typename Function, typename U, typename = std::enable_if_t<std::is_same_v<SubClass,std::decay_t<U>>>> MSVC_UNUSED_FALSE_POSITIVE
                static constexpr void at(size_t superIndex, U && object, Function function) {
                    RareTs::forIndex<total>(superIndex, [&](auto I) {
                        constexpr size_t i = decltype(I)::value;
                        if constexpr ( std::is_const_v<std::remove_reference_t<U>> )
                            function(SuperInfo<i>{}, (const SuperType<i> &)object);
                        else
                            function(SuperInfo<i>{}, (SuperType<i> &)object);
                    });
                }

                template <typename Function, typename U, typename = std::enable_if_t<std::is_same_v<SubClass,std::decay_t<U>>>>
                static constexpr void superAt(size_t superIndex, U && object, Function function) {
                    RareTs::forIndex<total>(superIndex, [&](auto I) {
                        if constexpr ( std::is_const_v<std::remove_reference_t<U>> )
                            function((const SuperType<decltype(I)::value> &)object);
                        else
                            function((SuperType<decltype(I)::value> &)object);
                    });
                }
            };
            template <typename SubClass> class Inherit<SubClass>
            {
            public:
                template <size_t I> using SuperType = void;
                template <size_t I> using SuperInfo = void;
                static constexpr size_t total = 0;
                template <typename Function> static constexpr void forEach(Function) {}
                template <typename Function, typename U> static constexpr void forEach(U &&, Function) {}
                template <typename Function, typename U> static constexpr void forEachSuper(U &&, Function) {}
                template <typename Function> static constexpr void at(size_t, Function) {}
                template <typename Function, typename U> static constexpr void at(size_t, U &&, Function) {}
                template <typename Function, typename U> static constexpr void superAt(size_t, U &&, Function) {}
            };

            namespace detail {
                template <typename SubClass, size_t ... Is> constexpr Inherit<SubClass, Is...> inherit(std::index_sequence<Is...>);
            }
        }

        inline namespace Functions
        {
            template <typename T, size_t I> struct is_overloaded {
                static constexpr bool value = std::is_void_v<RareTs::Class::template member_type<T, I>> &&
                    std::is_null_pointer_v<RareTs::Class::template member_pointer_type<T, I>>;
            };
            template <typename T, size_t I> inline constexpr bool is_overloaded_v = is_overloaded<T, I>::value;

            template <typename ArgTypes, typename ... Ts> struct NotedOverload {
                using ArgumentTypes = ArgTypes;
                using Notes = std::tuple<Ts...>;
                Notes notes;
            };

            template <typename ... Ts> struct OverloadType {
                using ArgumentTypes = std::tuple<Ts...>;
                using Notes = NoNote;
                static constexpr NoNote notes{};

                template <typename ... Us> constexpr NotedOverload<std::tuple<Ts...>, Us...> operator() (const Us & ... args) const {
                    return NotedOverload<std::tuple<Ts...>, Us...>{std::tuple<Us...>(args...)};
                }
            };

            template <typename ... Ts> inline constexpr OverloadType<Ts...> Overload{};

            template <typename T> struct is_overload_note : std::bool_constant<
                RareTs::is_specialization_v<T, OverloadType> || RareTs::is_specialization_v<T, NotedOverload>> {};

            namespace detail
            {
                template <typename Member> struct OverloadNotes { static constexpr auto notes = RareTs::referenceTuple<is_overload_note>(Member::notes); };

                template <typename Member> inline constexpr size_t overloadNoteCount =
                    std::tuple_size_v<std::remove_const_t<decltype(OverloadNotes<Member>::notes)>>;

                template <typename F, typename ... Ts> constexpr F returnType(F(*&&)(Ts...));
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...));
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) const);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) volatile);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) const volatile);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) &);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) const &);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) volatile &);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) const volatile &);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) &&);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) const &&);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) volatile &&);
                template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...) const volatile &&);
                template <size_t I, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>> argument(F(*&&)(Ts...));
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...));
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) const);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) volatile);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) const volatile);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) &);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) const &);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) volatile &);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) const volatile &);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) &&);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) const &&);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) volatile &&);
                template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>>
                    argument(F(T::*&&)(Ts...) const volatile &&);
                template <typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(*&&)(Ts...));
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...));
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) const);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) volatile);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) const volatile);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) &);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) const &);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) volatile &);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) const volatile &);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) &&);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) const &&);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) volatile &&);
                template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...) const volatile &&);
                template <typename F, typename ... Ts> constexpr void memberOf(F(*&&)(Ts...));
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...));
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) const);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) volatile);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) const volatile);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) &);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) const &);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) volatile &);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) const volatile &);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) &&);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) const &&);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) volatile &&);
                template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...) const volatile &&);
            }

            template <size_t I, typename F> using argument_t = decltype(detail::argument<I>(std::declval<std::remove_reference_t<F>>()));
            template <typename F> using arguments_t = decltype(detail::arguments(std::declval<std::remove_reference_t<F>>()));
            template <typename F> using return_type_t = decltype(detail::returnType(std::declval<std::remove_reference_t<F>>()));
            template <typename F> inline constexpr size_t argument_count_v = std::tuple_size_v<arguments_t<F>>;
            template <typename F> using member_of_t = decltype(detail::memberOf(std::declval<std::remove_reference_t<F>>())); // non-static members only

            template <typename ... Ts> struct ArgumentBuilder
            {
                template <typename T, size_t I> struct Argument : RareTs::type_id<T> { static constexpr size_t index = I; };

                template <size_t I, typename Function, typename ArgBuilder, typename ... Us> MSVC_UNUSED_FALSE_POSITIVE
                static constexpr decltype(auto) invoke(Function && function, ArgBuilder && argBuilder, Us && ... args) {
                    if constexpr ( I < sizeof...(Ts) )
                    {
                        return invoke<I+1>(std::forward<Function>(function), std::forward<ArgBuilder>(argBuilder),
                            std::forward<Us>(args)..., argBuilder(Argument<std::tuple_element_t<I, std::tuple<Ts...>>, I>{}));
                    }
                    else
                        return function(std::forward<Us>(args)...);
                }

                template <size_t I, typename Function, typename ArgBuilder, typename T, typename ... Us> MSVC_UNUSED_FALSE_POSITIVE
                static constexpr decltype(auto) invokeInstance(T && t, Function && function, ArgBuilder && argBuilder, Us && ... args) {
                    if constexpr ( I < sizeof...(Ts) )
                    {
                        return invokeInstance<I+1>(std::forward<T>(t), std::forward<Function>(function), std::forward<ArgBuilder>(argBuilder),
                            std::forward<Us>(args)..., argBuilder(Argument<std::tuple_element_t<I, std::tuple<Ts...>>, I>{}));
                    }
                    else
                        return (t.*function)(std::forward<Us>(args)...);
                }
            };
            template <typename ... Ts> struct ArgumentBuilder<std::tuple<Ts...>> : ArgumentBuilder<Ts...> {};
            template <typename ... Ts> struct ArgumentBuilder<const std::tuple<Ts...>> : ArgumentBuilder<Ts...> {};

            template <typename F, typename = std::enable_if_t<!std::is_void_v<F> && !std::is_null_pointer_v<F>>>
            struct Function
            {
                using pointer_type = F;
                pointer_type pointer;
                constexpr Function(F f) : pointer(f) {}

                using Arguments = arguments_t<F>;
                static constexpr size_t ArgumentCount = std::tuple_size_v<Arguments>;
                template <size_t I> using Argument = std::tuple_element_t<I, Arguments>;
                static constexpr bool IsMemberFunction = std::is_member_function_pointer_v<F>;
                using Return = return_type_t<F>;
                using MemberOf = member_of_t<F>; // Set for non-static member functions, for static and free functions this is void (not deducible from "F")

                template <typename ArgBuilder> constexpr auto invoke(ArgBuilder && argBuilder) const
                {
                    return ArgumentBuilder<Arguments>::template invoke<0>(
                        pointer, std::forward<ArgBuilder>(argBuilder));
                }

                template <typename T, typename ArgBuilder> constexpr auto invoke(T && t, ArgBuilder && argBuilder) const
                {
                    if constexpr ( IsMemberFunction )
                    {
                        if constexpr ( RareTs::is_tuple_v<T> )
                        {
                            constexpr size_t index = RareTs::index_of_v<MemberOf, T>;
                            if constexpr ( index < std::tuple_size_v<RareTs::remove_cvref_t<T>> )
                            {
                                return ArgumentBuilder<Arguments>::template invokeInstance(std::get<index>(std::forward<T>(t)),
                                    pointer, std::forward<ArgBuilder>(argBuilder));
                            }
                            else
                                return nullptr;
                        }
                        else
                        {
                            return ArgumentBuilder<Arguments>::template invokeInstance<0>(
                                std::forward<T>(t), pointer, std::forward<ArgBuilder>(argBuilder));
                        }
                    }
                    else
                        return ArgumentBuilder<Arguments>::template invoke<0>(
                            pointer, std::forward<ArgBuilder>(argBuilder));
                }
            };

            namespace detail {
                template <typename ... Ts> struct ResolveQualified // ResolveQualified models compiler overload resolution for a given class qualification
                {
                    using InputQualifications = std::tuple<Ts...>;

                    template <size_t MemberIndex, typename ... Args>
                    struct Overload
                    {
                        template <typename Q> struct qualification_exists {
                        #ifdef __INTELLISENSE__
                            static constexpr bool value = RareTs::op_exists_v< // This doesn't work with how clang does friends, but helps with intellisense
                                RareTs::Class::class_t<RareTs::remove_cvref_t<Q>>::template L_<MemberIndex, Args...>::template W_, Q>;
                        #else
                            static constexpr bool value = !std::is_null_pointer_v<
                                typename RareTs::Class::template overload<Args...>::template pointer_type<Q, MemberIndex>>;
                        #endif
                        };
                        using QualificationMask = typename RareTs::type_mask<qualification_exists, Ts...>::indexes;
                        template <typename F, size_t ... Is> static constexpr decltype(auto) pack(F f, std::index_sequence<Is...>) {
                            using ovl = typename RareTs::Class::template overload<Args...>;
                            return f(ovl::template pointer<std::tuple_element_t<Is, InputQualifications>, MemberIndex>...);
                        }
                        template <typename F> static constexpr decltype(auto) pack(F && f) {
                            return pack(std::forward<F>(f), QualificationMask{});
                        }
                        static constexpr auto pointers = pack([](auto ... ptr) { // All overloads that could be called for object of qualified type T with Args
                            return std::tuple { ptr... };
                        }, QualificationMask{});
                        static constexpr auto pointer = [](){
                            if constexpr ( std::tuple_size_v<decltype(pointers)> > 0 )
                                return std::get<0>(pointers);
                            else
                                return nullptr;
                        }();
                        using pointer_type = RareTs::remove_cvref_t<decltype(pointer)>;
                    };
                };
                template <typename T> struct ResolveQualified<T> : ResolveQualified<
                    T, const T, volatile T, const volatile T, T &, const T &, volatile T &, const volatile T &> {};
                template <typename T> struct ResolveQualified<const T> : ResolveQualified<const T, const volatile T, const T &, const volatile T &> {};
                template <typename T> struct ResolveQualified<volatile T> : ResolveQualified<volatile T, const volatile T, volatile T &, const volatile T &>{};
                template <typename T> struct ResolveQualified<const volatile T> : ResolveQualified<const volatile T, const volatile T &> {};
                template <typename T> struct ResolveQualified<T &> : ResolveQualified<
                    T, const T, volatile T, const volatile T, T &, const T &, volatile T &, const volatile T &> {};
                template <typename T> struct ResolveQualified<const T &> : ResolveQualified<const T, const volatile T, const T &, const volatile T &> {};
                template <typename T> struct ResolveQualified<volatile T &> : ResolveQualified<
                    volatile T, const volatile T, volatile T &, const volatile T &> {};
                template <typename T> struct ResolveQualified<const volatile T &> : ResolveQualified<const volatile T, const volatile T &> {};
                template <typename T> struct ResolveQualified<T &&> : ResolveQualified<
                    T, const T, volatile T, const volatile T, T &&, const T &&, volatile T &&, const volatile T &&, const T &> {};
                template <typename T> struct ResolveQualified<const T &&> : ResolveQualified<
                    const T, const volatile T, const T &&, const volatile T &&, const T &> {};
                template <typename T> struct ResolveQualified<volatile T &&> : ResolveQualified<
                    volatile T, const volatile T, volatile T &&, const volatile T &&> {};
                template <typename T> struct ResolveQualified<const volatile T &&> : ResolveQualified<const volatile T, const volatile T &&> {};
            }
            
            template <typename F, typename ... Args> struct is_instance_method {
                template <typename R> static constexpr std::false_type id(R(*&&)(Args...));
                #ifdef __clang__
                template <typename R> static constexpr std::false_type id(R(*&&)(Args...) noexcept);
                #endif
                template <typename T> static constexpr std::true_type id(T && t);
                static constexpr bool value = decltype(id(std::declval<std::remove_reference_t<F>>()))::value;
            };
            template <typename ... Args> struct is_instance_method<void, Args...> { static constexpr bool value = false; };
            template <typename ... Args> inline constexpr bool is_instance_method_v = is_instance_method<Args...>::value;

            template <typename Class, size_t MemberIndex, typename ... Args>
            struct OverloadInfo
            {
                using class_type = Class;
                using argument_types = std::tuple<Args...>;

                static constexpr auto pointers = Functions::detail::ResolveQualified<Class, const Class, volatile Class, const volatile Class, Class &,
                    const Class &, volatile Class &, const volatile Class &, Class &&, const Class &&, volatile Class &&, const volatile Class &&>
                    ::template Overload<MemberIndex, Args...>::pointers;

                using pointer_types = std::remove_const_t<decltype(pointers)>;

                // Returns the pointer type for the overload with the fewest qualifications if Q=void, or the exact same qualifications as Q otherwise
                template <typename Q = void> using pointer_type = std::conditional_t<std::is_void_v<Q>, std::tuple_element_t<0, pointer_types>,
                    typename RareTs::Class::template overload<Args...>::template pointer_type<Q, MemberIndex>>;
                
                // Returns the overload pointer type which would be resolved by the compiler if calling the method on qualified-object-type Q
                template <typename Q> using resolve_pointer_type =
                    typename Functions::detail::ResolveQualified<Q>::template Overload<MemberIndex, Args...>::pointer_type;

                static constexpr size_t memberIndex = MemberIndex;
                static constexpr bool isStatic = !RareTs::is_instance_method_v<pointer_type<Class>, Args...>;

                // Returns the overload pointer which would be resolved by the compiler if calling the method on qualified-object-type Q
                template <typename Q> static constexpr auto resolvePointer =
                    Functions::detail::ResolveQualified<Q>::template Overload<MemberIndex, Args...>::pointer;

                static constexpr size_t totalQualifications = std::tuple_size_v<pointer_types>; // The total unique qualifications for this overload with args

            private:
                template <class Q, class = void>
                struct ptr { static constexpr auto value = RareTs::Class::template overload<Args...>::template pointer<Q, MemberIndex>; };
                template <class Void> struct ptr<void, Void> { static constexpr auto value = std::get<0>(pointers); };

            public:
                // Returns the pointer for the overload with the fewest qualifications if Q=void, or the exact same qualifications as Q otherwise
                template <typename Q = void> static constexpr auto pointer = ptr<Q>::value;
            };

            template <typename T, size_t MemberIndex, typename MemberNotes, size_t ... Is>
            class KnownOverloads
            {
                using Notes = detail::OverloadNotes<MemberNotes>;
                template <size_t I> using OvlNote = std::remove_reference_t<std::tuple_element_t<I, RareTs::remove_cvref_t<decltype(Notes::notes)>>>;

                template <typename Q, typename ... Args> using ResolveQual = typename detail::ResolveQualified<Q>::template Overload<MemberIndex, Args...>;

                template <typename ... Args> using AllQualifications = typename detail::ResolveQualified<T, const T, volatile T, const volatile T, T &,
                    const T &, volatile T &, const volatile T &, T &&, const T &&, volatile T &&, const volatile T &&>
                    ::template Overload<MemberIndex, Args...>;
            
                template <typename Q, typename ... Args> struct ptr {
                    using pointer_type = typename RareTs::Class::template overload<Args...>::template pointer_type<Q, MemberIndex>;
                    static constexpr auto pointer = RareTs::Class::template overload<Args...>::template pointer<Q, MemberIndex>;
                };
                template <typename ... Args> struct ptr<void, Args...> : AllQualifications<Args...> {};

                template <typename Q, size_t Ovl> struct Qualified
                { // Note that intellisense (vs2019) is VERY touchy about this section of code, changes must be checked carefully
                    using Notez = decltype(RareTs::Reflection::Annotations::detail::template subAnnotationsType<Notes, Ovl>(
                        std::make_index_sequence<std::tuple_size_v<typename OvlNote<Ovl>::Notes>>()));

                    template <typename Ptr, typename ... Args> struct Overload : Notez, Function<typename Ptr::pointer_type> {
                        static constexpr size_t index = Ovl;
                        constexpr Overload() : Function<typename Ptr::pointer_type>(Ptr::pointer) {}
                    };
                    template <typename ... Args> static constexpr Overload<ptr<Q, Args...>, Args...> ovl(RareTs::type_tag<std::tuple<Args...>>);

                    template <typename ... Args> struct Resolution : Notez, Function<typename ResolveQual<Q, Args...>::pointer_type> {
                        static constexpr size_t index = Ovl;
                        constexpr Resolution() : Function<typename ResolveQual<Q, Args...>::pointer_type>(ResolveQual<Q, Args...>::pointer) {}
                    };
                    template <typename ... Args> static constexpr Resolution<Args...> res(RareTs::type_tag<std::tuple<Args...>>);
                };
            
                template <typename Q, typename ... Args> static constexpr auto resolvePtr(RareTs::type_tag<std::tuple<Args...>>)
                    -> typename detail::ResolveQualified<Q>::template Overload<MemberIndex, Args...>::pointer_type;

                template <typename Q, size_t I> using ResolvePtr = decltype(resolvePtr<Q>(RareTs::type_tag<typename OvlNote<I>::ArgumentTypes>{}));

            public:
                using class_type = T;

                static constexpr size_t memberIndex = MemberIndex;

                static constexpr size_t totalArgumentSets = sizeof...(Is); // The total number of NOTE'd, unique argument sets

                // Returns the overload for argument set I which has the fewest qualifications if Q=void, or the exact same qualifications as Q otherwise
                template <size_t I, typename Q = void> using Overload = decltype(Qualified<Q, I>::ovl(RareTs::type_tag<typename OvlNote<I>::ArgumentTypes>{}));

                // Returns the overload for argument set I which would be resolved by the compiler if calling the method on qualified-object-type Q
                template <size_t I, typename Q> using ResolveOverload = decltype(Qualified<Q, I>::res(RareTs::type_tag<typename OvlNote<I>::ArgumentTypes>{}));

                template <typename Q, typename F> static constexpr void forEach(Q &&, F function) { // Overloads resolved on qualified class "Q"
                    RareTs::packIndexes(typename type_mask<RareTs::is_non_null, ResolvePtr<Q, Is>...>::indexes{}, [&](auto ... I) {
                        (function(ResolveOverload<decltype(I)::value, Q>{}), ...);
                    });
                }

                template <typename Q, typename F> static constexpr void forEach(F function) { // Overloads resolved on qualified class "Q"
                    RareTs::packIndexes(typename type_mask<RareTs::is_non_null, ResolvePtr<Q, Is>...>::indexes{}, [&](auto ... I) {
                        (function(ResolveOverload<decltype(I)::value, Q>{}), ...);
                    });
                }

                template <typename F> static constexpr void forEach(F function) { // For each argument sets least-qualified overload
                    (function(Overload<Is>{}), ...);
                }

                template <typename Q, typename F> static constexpr void at(Q &&, size_t index, F function) { // Overloads resolved on qualified class "Q"
                    RareTs::forIndex<totalArgumentSets>(index, [&](auto I) {
                        (function(ResolveOverload<decltype(I)::value, Q>{}));
                    });
                }

                template <typename Q, typename F> static constexpr void at(size_t index, F function) { // Overloads resolved on qualified class "Q"
                    RareTs::forIndex<totalArgumentSets>(index, [&](auto I) {
                        (function(ResolveOverload<decltype(I)::value, Q>{}));
                    });

                }
            
                template <typename F> static constexpr void at(size_t index, F function) { // For nth argument sets least-qualified overload
                    RareTs::forIndex<totalArgumentSets>(index, [&](auto I) {
                        (function(Overload<decltype(I)::value>{}));
                    });
                }

                template <typename Q, typename F> static constexpr void pack(Q &&, F function) { // Overloads resolved on qualified class "U"
                    RareTs::packIndexes(typename type_mask<RareTs::is_non_null, ResolvePtr<Q, Is>...>::indexes{}, [&](auto ... I) {
                        function(ResolveOverload<decltype(I)::value, Q>{}...);
                    });
                }

                template <typename Q, typename F> static constexpr void pack(F function) { // Overloads resolved on qualified class "Q"
                    RareTs::packIndexes(typename type_mask<RareTs::is_non_null, ResolvePtr<Q, Is>...>::indexes{}, [&](auto ... I) {
                        function(ResolveOverload<decltype(I)::value, Q>{}...);
                    });
                }

                template <typename F> static constexpr void pack(F function) { // Pack each argument sets least-qualified overload
                    function(Overload<Is>{}...);
                }
            };
            template <typename T, size_t MemberIndex, typename MemberNotes>
            class KnownOverloads<T, MemberIndex, MemberNotes> {
            public:
                using class_type = T;
                static constexpr size_t memberIndex = MemberIndex;
                static constexpr size_t totalArgumentSets = 0; // The total number of NOTE'd, unique argument sets
                // Returns the overload for argument set I which has the fewest qualifications if Qualification=void, or the exact same Qualifications if not
                template <size_t I, typename Qualification = void> using Overload = void;
                // Returns the overload for argument set I which would be resolved by the compiler if calling the method on qualified-object-type U
                template <size_t I, typename U> using ResolveOverload = void;
                template <typename U, typename F> static constexpr void forEach(U &&, F) {} // Overloads resolved on qualified class "U"
                template <typename U, typename F> static constexpr void forEach(F) {} // Overloads resolved on qualified class "U"
                template <typename F> static constexpr void forEach(F) {} // For each argument sets least-qualified overload
                template <typename U, typename F> static constexpr void at(U &&, size_t, F) {} // Overloads resolved on qualified class "U"
                template <typename U, typename F> static constexpr void at(size_t, F) {} // Overloads resolved on qualified class "U"
                template <typename F> static constexpr void at(size_t, F) {} // For nth argument sets least-qualified overload
                template <typename U, typename F> static constexpr void pack(U &&, F) {} // Overloads resolved on qualified class "U"
                template <typename U, typename F> static constexpr void pack(F) {} // Overloads resolved on qualified class "U"
                template <typename F> static constexpr void pack(F) {} // Pack each argument sets least-qualified overload
            };

            namespace detail {
                template <typename T, size_t MemberIndex, typename MemberNotes, size_t ... Is>
                constexpr KnownOverloads<T, MemberIndex, MemberNotes, Is...> knownOverloads(std::index_sequence<Is...>);
            }
        }

        template <typename T, size_t MemberIndex, typename Notez = typename RareTs::Class::MemberAnnotationsType<T, MemberIndex>>
        class Member : public Notez
        {
            using M = typename RareTs::Class::template member_type<T, MemberIndex>; // Inlining this causes access protection errors on lower gcc versions
            static constexpr TypeName<M> typeStrStorage {};
            using P = RareTs::Class::member_pointer_type<T, MemberIndex>;
        public:
            using object_type = T;
            using type = RareTs::replace_void_t<M, P>;
            using pointer_type = std::conditional_t<std::is_reference_v<type>, std::nullptr_t, P>;

            static constexpr const char* typeStr = typeStrStorage;
            static constexpr const char* name = RareTs::Class::member_name<T, MemberIndex>;
        
            static constexpr size_t index = MemberIndex;
            static constexpr bool isStatic = !std::is_member_pointer_v<P> && !std::is_same_v<std::nullptr_t, P>;
            static constexpr bool isFunction = std::is_function_v<type> || std::is_member_function_pointer_v<P>;
            static constexpr bool isOverloaded = RareTs::is_overloaded_v<T, MemberIndex>;
            static constexpr bool isData = !isFunction && !isOverloaded;
            static constexpr bool hasOffset = std::is_member_object_pointer_v<P>;
            static constexpr size_t getOffset() { return RareTs::Class::member_offset<T, MemberIndex>; } // If none, returns std::numeric_limits<size_t>::max()
            static constexpr auto pointer = RareTs::Class::member_pointer<T, MemberIndex>;

            static constexpr auto & value() noexcept {
                if constexpr ( isStatic && isData )
                    return RareTs::Class::memberValue<T, MemberIndex>();
                else
                    return pointer;
            }
            
            template <typename U> static constexpr auto & value(U && t) noexcept {
                if constexpr ( isData )
                    return RareTs::Class::memberValue<MemberIndex>(std::forward<U>(t));
                else
                    return pointer;
            }

            using Overloads = decltype(RareTs::Functions::detail::knownOverloads<T, MemberIndex,
                typename RareTs::Class::template member_note_wrapper<T, MemberIndex>>(
                    std::make_index_sequence<RareTs::Functions::detail::overloadNoteCount<Notez>>()));
        };

        namespace AggregatedMembers {
            template <typename T, size_t I = std::numeric_limits<size_t>::max()>
            struct aggregate_member : RareTs::Class::adapt_member_type<aggregate_member<T>::template type, T, I> {};

            template <typename T>
            struct aggregate_member<T, std::numeric_limits<size_t>::max()> {
                template <size_t I> using type = RareTs::Member<T, I>;
            };

            template <typename T, size_t ... Is>
            struct aggregate_members : RareTs::Class::adapt_member_type<aggregate_member<T>::template type, T, Is> ... {};

            template <typename T, size_t ... Is>
            constexpr aggregate_members<T, Is...> aggregateMembers(std::index_sequence<Is...>);
        }

        template <typename T, size_t MemberIndex> // Overloads (and references on MSVC < 1932 / Visual Studios <= 2019) are unsupported/always private
        inline constexpr AccessMod access_modifier_v = RareTs::Class::template access_modifier<T, MemberIndex>::value;

        namespace { // REFLECT macros

#ifndef __clang__
#ifdef _MSC_VER
#if _MSC_VER < 1932
#define RARE_DISABLE_REF_ACCESS
#endif
#endif
#endif

#ifdef __clang__
#define RARE_CLASS_FRIEND(x) friend RareTs::Class;\
friend constexpr inline Class classType(RareTs::type_tag<RareTs::Proxy<RareTs::Class::unproxy_t<x>>>) { return {}; }
#else
#define RARE_CLASS_FRIEND(x) friend RareTs::Class;
#endif

#ifdef __clang__
#define RARE_ACCESS_HEADER template <size_t, class=void> struct A_;
#define RARE_ACCESS_MEMBER(x) template <class U_> struct A_<I_::x, U_> { \
    template <class T_> using t = decltype(T_::x); \
    template <class T_> using p = decltype(&T_::x); };
#else
#define RARE_ACCESS_HEADER template <size_t, class, class...> struct A_;
#ifdef RARE_DISABLE_REF_ACCESS
#define RARE_ACCESS_MEMBER(x) \
template <class U_, class ... Ts_> struct A_<I_::x, U_, Ts_...> : Ts_... { \
    template <class T_> friend constexpr auto id(A_<I_::x, T_, Ts_...>, int, int) -> RareTs::as_index<0, decltype(&T_::x)>; \
    template <class T_> friend constexpr auto id(A_<I_::x, T_, Ts_...> a, int, unsigned) -> RareTs::as_index<1, decltype(&decltype(a)::x)>; \
    template <class T_> friend constexpr auto id(A_<I_::x, T_, Ts_...>, ...) -> RareTs::as_index<2>; };
#else
#define RARE_ACCESS_MEMBER(x) \
template <class U_, class ... Ts_> struct A_<I_::x, U_, Ts_...> : Ts_... { \
    template <class T_> friend constexpr auto id(A_<I_::x, T_, Ts_...>, int, int) -> RareTs::as_index<0, decltype(T_::x)>; \
    template <class T_> friend constexpr auto id(A_<I_::x, T_, Ts_...>, int, unsigned) -> RareTs::as_index<0, decltype(&T_::x)>; \
    template <class T_> friend constexpr auto id(A_<I_::x, T_, Ts_...> a, unsigned, unsigned) -> RareTs::as_index<1, decltype(decltype(a)::x)>; \
    template <class T_> friend constexpr auto id(A_<I_::x, T_, Ts_...> a, unsigned, ...) -> RareTs::as_index<1, decltype(&decltype(a)::x)>; \
    template <class T_> friend constexpr auto id(A_<I_::x, T_, Ts_...>, ...) -> RareTs::as_index<2>; };
#endif
#endif

#if defined(RARE_NO_VALIDATE)
#define RARE_MEMBER_VALIDATOR(x)
#elif defined(__GNUC__) && !defined(__clang__)
#define RARE_MEMBER_VALIDATOR(x) template <typename T_> static constexpr RareTs::type_id<decltype(T_::x)> x##_V_(int, int); \
template <typename T_> static constexpr RareTs::type_id<decltype(&T_::x)> x##_V_(int, unsigned); \
template <typename T_> static constexpr RareTs::type_id<decltype(T_::x##_note)> x##_V_(unsigned, unsigned); \
template <typename T_> static constexpr RareTs::type_id<void> x##_V_(...); \
static constexpr auto x##_validate = RareTs::Class::validate<C_, I_::x>::template member<decltype(x##_V_<B_>(0, 0))>(0);
#else
#define RARE_MEMBER_VALIDATOR(x) static constexpr auto x##_validate = RareTs::Class::validate<C_, I_::x>::template member<F_<I_::x>, Q_<I_::x>, E_<I_::x>>(0);
#endif

#define RARE_RESTATE_COMMA(x) x,
#define RARE_MEMBER(x) \
template <class T_> struct F_<I_::x, T_, std::void_t<decltype(T_::x)>> : RareTs::type_id<decltype(T_::x)> { \
    template <class U_> static constexpr auto & s() { return U_::x; } \
    template <class U_> static constexpr auto & i(U_ && t) { return t.x; } \
}; \
template <class T_> struct Q_<I_::x, T_, std::void_t<decltype(&T_::x)>> : RareTs::type_id<decltype(&T_::x)> {}; \
template <class T_> struct N_<I_::x, T_> { static constexpr char n[]=#x; }; \
template <class T_> struct P_<I_::x, T_, RareTs::enable_if_constexpr<decltype(&T_::x), &T_::x>> { static constexpr auto p = &T_::x; }; \
template <class T_> struct E_<I_::x, T_, std::void_t<decltype(T_::x##_note)>> { static constexpr auto & notes = T_::x##_note; }; \
template <class T_> struct O_<I_::x, T_, std::enable_if_t<std::is_member_object_pointer_v<decltype(&T_::x)>>> { \
    static constexpr size_t o() { return offsetof(T_,x); }; \
}; \
template <template <size_t> class T_> struct M_<I_::x, T_> { T_<I_::x> x; }; \
template <template <size_t> class T_> struct D_<I_::x, T_> { using x = T_<I_::x>; }; \
template <class ... A_> struct L_<I_::x, A_...> { \
    template <class T_> using R_ = decltype(std::declval<T_>().x(std::declval<A_>()...)); \
    template <class T_> using W_ = decltype(RareTs::Class::OverloadIdentifier<T_,R_<T_>,A_...>::Id::of(&std::remove_reference_t<T_>::x)); \
    template <class T_> static constexpr auto p = static_cast<W_<T_>>(&std::remove_reference_t<T_>::x); \
}; \
RARE_MEMBER_VALIDATOR(x) \
RARE_ACCESS_MEMBER(x)

/*
    I_: "Index Of" (field index enum) - always required

    F_: "Field" - usually required, cannot support reference fields without
        type = decltype(T::x)
        s() returns T::x
        i() return t.x

    Q_: "Pointer Type" - usually required, cannot support member functions without
        type = decltype(&T::x)

    N_: "Name" - usually required, will not have field names otherwise
        n = name

    P_: "Pointer Value" - usually required, cannot support functions otherwise
        p = pointer value

    E_: "Notes" - usually required, no note support otherwise
        notes = notes

    O_: "Offset" - rarely required, only needed if using offsets
        o() returns offsetof(T, x)

    M_: "Member Adapter" - required exclusively for member-based adapters
        TT<FieldIndex> x;

    D_: "Member Type Adapter" - required exclusively for type-based adapters
        x = TT<x>

    L_: "Overload" - required exclusively for reflecting overloads
        R_ = return type
        W_ = overload id
        p = pointer
*/

// After the objectType can be 0 to 125 member names, e.g. REFLECT(MyObj, myInt, myString, myOtherObj)
#define REFLECT(objectType, ...) \
struct Class { \
    using B_ = objectType; \
    using C_ = RareTs::Class::unproxy_t<B_>; \
    struct I_ { enum { RARE_FOR_EACH(RARE_RESTATE_COMMA, __VA_ARGS__) N_ }; }; \
    static constexpr auto & notes = RareTs::noNote; \
    template <size_t, class T_ = B_, class = void> struct F_ : RareTs::Class::EmptyComponent {}; \
    template <size_t, class T_ = B_, class = void> struct Q_ : RareTs::Class::NullptrType {}; \
    template <size_t, class T_ = B_, class = void> struct P_ : RareTs::Class::NonPointable {}; \
    template <size_t, class T_ = B_, class = void> struct E_ : RareTs::Class::NonNoted {}; \
    template <size_t, class T_ = B_, class = void> struct O_ : RareTs::Class::NoOffset {}; \
    template <size_t, template <size_t> class> struct M_; \
    template <size_t, template <size_t> class> struct D_; \
    template <size_t, class T_ = B_> struct N_; \
    template <size_t, class...> struct L_; \
    RARE_ACCESS_HEADER \
    RARE_FOR_EACH(RARE_MEMBER, __VA_ARGS__) \
}; \
RARE_CLASS_FRIEND(objectType)

// REFLECT_NOTED is exactly the same as REFLECT except this signals that objectType itself is annotated
#define REFLECT_NOTED(objectType, ...) \
struct Class { \
    using B_ = objectType; \
    using C_ = RareTs::Class::unproxy_t<B_>; \
    struct I_ { enum { RARE_FOR_EACH(RARE_RESTATE_COMMA, __VA_ARGS__) N_ }; }; \
    static constexpr auto & notes = objectType##_note; \
    template <size_t, class T_ = B_, class = void> struct F_ : RareTs::Class::EmptyComponent {}; \
    template <size_t, class T_ = B_, class = void> struct Q_ : RareTs::Class::NullptrType {}; \
    template <size_t, class T_ = B_, class = void> struct P_ : RareTs::Class::NonPointable {}; \
    template <size_t, class T_ = B_, class = void> struct E_ : RareTs::Class::NonNoted {}; \
    template <size_t, class T_ = B_, class = void> struct O_ : RareTs::Class::NoOffset {}; \
    template <size_t, template <size_t> class> struct M_; \
    template <size_t, template <size_t> class> struct D_; \
    template <size_t, class T_ = B_> struct N_; \
    template <size_t, class...> struct L_; \
    RARE_ACCESS_HEADER \
    RARE_FOR_EACH(RARE_MEMBER, __VA_ARGS__) \
}; \
RARE_CLASS_FRIEND(objectType)

        }

        template <typename T> using IndexOf = typename RareTs::Class::index_of<T>;

        template <typename T> using MemberType = decltype(AggregatedMembers::aggregateMembers<T>(std::make_index_sequence<RareTs::Class::member_count<T>>()));
        
        template <typename T> struct Members // Access to member information
        {
            static constexpr size_t total = RareTs::IndexOf<T>::N_; // The total number of reflected members
                
            // Member<I> - member type by index
            template <size_t MemberIndex> using Member = RareTs::Member<T, MemberIndex>;
                
            // member<I> - member instance by index
            template <size_t MemberIndex> static constexpr Member<MemberIndex> member = Member<MemberIndex>{};

        protected:
            template <size_t ... I> static constexpr auto memberIndexMap(std::index_sequence<I...>) {
				if constexpr ( sizeof...(I) > 0 )
					return RareTs::StringIndexMap<(RareTs::Class::member_name<T, I>)...>();
				else
					return RareTs::StringIndexMap<>();
            }

            using MemberNames = decltype(memberIndexMap(std::make_index_sequence<total>()));

            template <size_t I = 0, typename F, typename = std::void_t<decltype(std::declval<F>()(member<I>))>>
            static constexpr std::true_type onlyUsesMember(F && f);

            template <size_t I = 0> static constexpr std::false_type onlyUsesMember(...);

            template <typename F, size_t ... Is> static constexpr auto packMembers(F && f, std::index_sequence<Is...>) {
                return f(member<Is>...);
            }
                
        public:
            // Gets the index of a member using the member name
            template <class U=T> static constexpr size_t indexOf(std::string_view memberName) {
                return MemberNames::indexOf(memberName);
            }

            // Forwards all members to function as a parameter pack
            template <typename Function> static constexpr auto pack(Function && function) {
                return packMembers(std::forward<Function>(function), std::make_index_sequence<total>());
            }

            // Forwards all members matching filter to function as a parameter pack
            template <template <typename ...> class Filter, typename Function> static constexpr auto pack(Function && function) {
                return pack([&function](auto ... member) {
                    return packMembers(std::forward<Function>(function), typename RareTs::type_mask<Filter, decltype(member)...>::indexes{});
                });
            }

            // The total number of reflected members after the given filter is applied
            template <template <typename ...> class Filter, typename ...FilterArgs>
            static constexpr size_t filteredCount() {
                if constexpr ( total == 0 )
                    return 0;
                else
                {
                    size_t count = 0;
                    RareTs::forIndexes<total>([&](auto I) {
                        if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                            ++count;
                    });
                    return count;
                }
            }
            
            // function(member) or...
            // function(member, value) [statics only]
            template <typename Function> static constexpr void forEach(Function && function) {
                RareTs::forIndexes<total>([&](auto I) {
                    if constexpr ( decltype(onlyUsesMember(std::forward<Function>(function)))::value )
                        function(member<decltype(I)::value>);
                    else if constexpr ( Member<decltype(I)::value>::isStatic )
                        function(member<decltype(I)::value>, Member<decltype(I)::value>::value());
                });
            }

            // function(member) [filtered] or...
            // function(member, value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function> static constexpr void forEach(Function && function) {
                RareTs::forIndexes<total>([&](auto I) {
                    if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                    {
                        if constexpr ( decltype(onlyUsesMember(std::forward<Function>(function)))::value )
                            function(member<decltype(I)::value>);
                        else if constexpr ( Member<decltype(I)::value>::isStatic )
                            function(member<decltype(I)::value>, Member<decltype(I)::value>::value());
                    }
                });
            }

            // t, function(member, value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void forEach(U && t, Function && function) {
                
                RareTs::forIndexes<total>([&](auto I) {
                    if constexpr ( decltype(onlyUsesMember(std::forward<Function>(function)))::value )
                        function(member<decltype(I)::value>);
                    else
                        function(member<decltype(I)::value>, Member<decltype(I)::value>::value(t));
                });
            }

            // t, function(member, value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U,
                typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void forEach(U && t, Function && function) {
                RareTs::forIndexes<total>([&](auto I) {
                    if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                    {
                        if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
                            function(member<decltype(I)::value>);
                        else
                            function(member<decltype(I)::value>, Member<decltype(I)::value>::value(t));
                    }
                });
            }

            // function(member) or...
            // function(member, value) [statics only]
            template <typename Function> static constexpr void at(size_t memberIndex, Function && function) {
				RareTs::forIndex<total>(memberIndex, [&](auto I) {
					if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
						function(member<decltype(I)::value>);
                    else if constexpr ( Member<decltype(I)::value>::isStatic )
                        function(member<decltype(I)::value>, Member<decltype(I)::value>::value());
				});
            }
            
            // function(member) [filtered] or...
            // function(member, value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function>
            static constexpr void at(size_t memberIndex, Function && function) {
				RareTs::forIndex<total>(memberIndex, [&](auto I) {
					if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                    {
					    if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
							function(member<decltype(I)::value>);
                        else if constexpr ( Member<decltype(I)::value>::isStatic )
                            function(member<decltype(I)::value>, Member<decltype(I)::value>::value());
                    }
				});
            }

            // t, function(member, value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void at(size_t memberIndex, U && t, Function && function) {
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
					if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
						function(member<decltype(I)::value>);
                    else
                        function(member<decltype(I)::value>, Member<decltype(I)::value>::value(t));
                });
            }

            // t, function(member, value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U,
                typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void at(size_t memberIndex, U && t, Function && function) {
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                    {
					    if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
							function(member<decltype(I)::value>);
                        else
                            function(member<decltype(I)::value>, Member<decltype(I)::value>::value(t));
                    }
                });
            }
            
            // memberName, function(member) or...
            // memberName, function(member, value) [statics only]
            template <typename Function> static constexpr void named(std::string_view memberName, Function && function) {
			    size_t memberIndex = indexOf<>(memberName);
			    RareTs::forIndex<total>(memberIndex, [&](auto I) {
				    if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
					    function(member<decltype(I)::value>);
				    else if constexpr ( Member<decltype(I)::value>::isStatic )
                        function(member<decltype(I)::value>, Member<decltype(I)::value>::value());
			    });
            }
            
            // memberName, function(member) [filtered] or...
            // memberName, function(member, value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function>
            static constexpr void named(std::string_view memberName, Function && function) {
			    size_t memberIndex = indexOf<>(memberName);
			    RareTs::forIndex<total>(memberIndex, [&](auto I) {
				    if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
				    {
					    if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
						    function(member<decltype(I)::value>);
					    else if constexpr ( Member<decltype(I)::value>::isStatic )
                            function(member<decltype(I)::value>, Member<decltype(I)::value>::value());
				    }
			    });
            }

            // t, memberName, function(member, value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void named(std::string_view memberName, U && t, Function && function) {
                size_t memberIndex = indexOf<>(memberName);
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
					if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
						function(member<decltype(I)::value>);
                    else
                        function(member<decltype(I)::value>, Member<decltype(I)::value>::value(t));
                });
            }

            // t, memberName, function(member, value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U,
                typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void named(std::string_view memberName, U && t, Function && function) {
                size_t memberIndex = indexOf<>(memberName);
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                    {
					    if constexpr ( decltype(onlyUsesMember<decltype(I)::value>(std::forward<Function>(function)))::value )
							function(member<decltype(I)::value>);
                        else
                            function(member<decltype(I)::value>, Member<decltype(I)::value>::value(t));
                    }
                });
            }
        };

        template <typename T> class Values // Access to member-value information
        {
            template <size_t MemberIndex> using Member = RareTs::Member<T, MemberIndex>;

        public:
            static constexpr size_t total = RareTs::IndexOf<T>::N_; // The total number of reflected members

            // Forwards the static value of all members (member pointers if instance member) to function as a parameter pack
            template <typename Function> static constexpr auto pack(Function && function) { return RareTs::Class::packMemberValues<T>(function); }
            
            // Forwards the value of all members to function as a parameter pack
            template <typename Function, class U> static constexpr auto pack(U && t, Function && function) {
                return RareTs::Class::packMemberValues(std::forward<U>(t), std::forward<Function>(function));
            }

            // function(value) [statics only]
            template <typename Function> static constexpr void forEach(Function && function) {
                RareTs::forIndexes<total>([&](auto I) {
                    if constexpr ( Member<decltype(I)::value>::isStatic )
                        function(Member<decltype(I)::value>::value());
                });
            }

            // function(value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function> static constexpr void forEach(Function && function) {
                RareTs::forIndexes<total>([&](auto I) {
                    if constexpr ( Member<decltype(I)::value>::isStatic && passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                        function(Member<decltype(I)::value>::value());
                });
            }

            // t, function(value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void forEach(U && t, Function && function) {
                RareTs::forIndexes<total>([&](auto I) {
                    function(Member<decltype(I)::value>::value(std::forward<U>(t)));
                });
            }

            // t, function(value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U,
                typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void forEach(U && t, Function && function) {
                RareTs::forIndexes<total>([&](auto I) {
                    if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                        function(Member<decltype(I)::value>::value(std::forward<U>(t)));
                });
            }
            
            // function(value) [statics only]
            template <typename Function> static constexpr void at(size_t memberIndex, Function && function) {
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    if constexpr ( Member<decltype(I)::value>::isStatic )
                        function(Member<decltype(I)::value>::value());
                });
            }
            
            // function(value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function>
            static constexpr void at(size_t memberIndex, Function && function) {
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    if constexpr ( Member<decltype(I)::value>::isStatic && passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                        function(Member<decltype(I)::value>::value());
                });
            }

            // t, function(value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void at(size_t memberIndex, U && t, Function && function) {
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    function(Member<decltype(I)::value>::value(std::forward<U>(t)));
                });
            }
            
            // t, function(value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U,
                typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void at(size_t memberIndex, U && t, Function && function) {
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                        function(Member<decltype(I)::value>::value(std::forward<U>(t)));
                });
            }

            // memberName, function(value) [statics only]
            template <typename Function> static constexpr void named(std::string_view memberName, Function && function) {
                size_t memberIndex = Members<T>::template indexOf<>(memberName);
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    if constexpr ( Member<decltype(I)::value>::isStatic )
                        function(Member<decltype(I)::value>::value());
                });
            }

            // memberName, function(value) [filtered, statics only]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function>
            static constexpr void named(std::string_view memberName, Function && function) {
                size_t memberIndex = Members<T>::template indexOf<>(memberName);
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    if constexpr ( Member<decltype(I)::value>::isStatic && passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                        function(Member<decltype(I)::value>::value());
                });
            }
            
            // t, memberName, function(value)
            template <typename Function, class U, typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void named(std::string_view memberName, U && t, Function && function) {
                size_t memberIndex = Members<T>::template indexOf<>(memberName);
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    function(Member<decltype(I)::value>::value(std::forward<U>(t)));
                });
            }

            // t, memberName, function(value) [filtered]
            template <template <typename ...> class Filter, typename ...FilterArgs, typename Function, class U,
                typename = std::enable_if_t<std::is_same_v<T,std::decay_t<U>>>>
            static constexpr void named(std::string_view memberName, U && t, Function && function) {
                size_t memberIndex = Members<T>::template indexOf<>(memberName);
                RareTs::forIndex<total>(memberIndex, [&](auto I) {
                    if constexpr ( passes_filter_v<Filter, Member<decltype(I)::value>, FilterArgs...> )
                        function(Member<decltype(I)::value>::value(std::forward<U>(t)));
                });
            }
        };

        template <typename T> using Notes = decltype(RareTs::Annotations::detail::annotationsType<RareTs::Class::class_t<T>>(
            std::make_index_sequence<RareTs::Class::class_note_count<T>>()));

        template <typename T> using Supers = decltype(RareTs::Reflection::Inheritance::detail::inherit<T>(
            std::make_index_sequence<RareTs::Reflection::Inheritance::detail::superNoteCount<T>>()));

        template <typename T>
        struct Reflect
        {
            using type = T;
            using IndexOf = RareTs::IndexOf<T>;
            using MemberType = RareTs::MemberType<T>;
            using Members = RareTs::Members<T>;
            template <size_t MemberIndex> using Member = RareTs::Member<T, MemberIndex>;
            using Values = RareTs::Values<T>;
            using Notes = RareTs::Notes<T>; // Specifically the class-level annotations of the reflected type, not member-level annotations
            using Supers = RareTs::Supers<T>; // Super-class info for type T
        };

        inline namespace AdaptiveStructures
        {
            struct BuildableType {};
            inline constexpr BuildableType Buildable {};

            struct BuilderIgnoreType {};
            inline constexpr BuilderIgnoreType BuilderIgnore {};
            
            template <typename T, typename = void> struct is_validatable { static constexpr bool value = false; };
            template <typename T> struct is_validatable<T, std::enable_if_t<std::is_same_v<bool, decltype(std::declval<T>().validate())>>> : std::true_type {};
            template <typename T> struct is_validatable<T, std::enable_if_t<std::is_same_v<void, decltype(std::declval<T>().validate())>>> : std::true_type {};
            template <typename T> inline constexpr bool is_validatable_v = is_validatable<T>::value;

            namespace detail {
                template <typename T, size_t ... Is> class sub_builder;

                template <typename T, typename Builder, size_t ... Is> struct member_builder {
                    template <size_t I> class type {
                        using Member = typename RareTs::Reflect<T>::Members::template Member<I>;
                        template <size_t ... Js> constexpr auto remaining(std::index_sequence<Js...>) { return sub_builder<T, Js...>(t); }
                        T & t;

                    public:
                        constexpr type(T & t) : t(t) {}
                        constexpr auto operator() (const typename Member::type & value) {
                            Member::value(t) = value;
                            return remaining(typename RareTs::remove_index<I, Is...>::type{});
                        }
                    };
                };

                template <typename T, size_t ... Is>
                class sub_builder : public RareTs::Class::adapt_member<member_builder<T, sub_builder<T, Is...>, Is...>::template type, T, Is>...
                {
                    T & t;

                public:
                    constexpr T build()
                    {
                        if constexpr ( is_validatable_v<T> )
                        {
                            if constexpr ( std::is_same_v<bool, decltype(std::declval<T>().validate())> ) // Has bool validate method
                            {
                                if ( !t.validate() ) // validate should return false if the build was not accepted
                                    throw std::logic_error("Builder validation failed!");
                            }
                            else if constexpr ( std::is_same_v<void, decltype(std::declval<T>().validate())> ) // Has a void validate method
                                t.validate(); // validate should throw an exception if the build was not accepted
                        }

                        return t;
                    }
                    constexpr sub_builder(T & t)
                        : RareTs::Class::adapt_member<member_builder<T, sub_builder<T, Is...>, Is...>::template type, T, Is> {{t}}..., t(t) {}
                };

                template <typename T, size_t ... Is>
                class head_builder : public sub_builder<T, Is...>
                {
                    T t{};

                public:
                    constexpr head_builder() : sub_builder<T, Is...>(t), t({}) {}
                };

                template <typename T> struct is_public_only_builder {
                    static constexpr bool value =
                        !RareTs::Notes<T>::template hasNote<BuildableType>() &&
                        !RareTs::Notes<T>::template hasNote<BuilderIgnoreType>() &&
                        !RareTs::is_validatable_v<T> &&
                        !RareTs::Members<T>::pack([](auto ... member) {
                            return (member.template hasNote<BuilderIgnoreType>() || ...);
                        });
                };

                template <typename T, typename = enable_if_member_t<T>> struct is_builder_member {
                    using object_type = typename T::object_type;
                    static constexpr bool value = T::isData && !T::isStatic && !T::template hasNote<BuilderIgnoreType>() &&
                        (!is_public_only_builder<object_type>::value || access_modifier_v<object_type, T::index> == AccessMod::Public);
                };

                template <typename T, size_t ... Is> constexpr auto builder(std::index_sequence<Is...>) {
                    return detail::head_builder<T, Is...>{};
                }

                template <typename T, size_t I> struct type_wrapper { using type = typename RareTs::Class::template member_type<T, I>; }; // Fixes gcc access bugs

                template <typename T, size_t I = std::numeric_limits<size_t>::max()>
                struct reference_member : RareTs::Class::adapt_member<reference_member<T>::template type, T, I> {};

                template <typename T>
                struct reference_member<T, std::numeric_limits<size_t>::max()> {
                    template <size_t I> using type = std::add_lvalue_reference_t<typename type_wrapper<T, I>::type>;
                };

                template <typename T, size_t ... Is>
                struct reference_members_adapter : RareTs::Class::adapt_member<reference_member<T>::template type, T, Is>... // Adapter extends the Ith members type
                {
                    constexpr reference_members_adapter(T & t)
                        : RareTs::Class::adapt_member<reference_member<T>::template type, T, Is> {{ RareTs::Class::memberValue<Is>(t) }}... {}
                };

                template <typename T, size_t ... Is> constexpr auto whitebox(T & t, std::index_sequence<Is...>) {
                    return detail::reference_members_adapter<T, Is...>(t);
                }
            }
    
            template <typename T> constexpr auto builder() {
                static_assert(!RareTs::Notes<T>::template hasNote<BuilderIgnoreType>(), "Cannot build type annotated with BuilderIgnore");
                return RareTs::template Members<T>::template pack<detail::is_builder_member>([&](auto & ... member) {
                    return detail::builder<T>(std::index_sequence<RareTs::remove_cvref_t<decltype(member)>::index...>{});
                });
            }

            template <typename T> constexpr auto whitebox(T & t) {
                return RareTs::template Members<T>::template pack<RareTs::Filter::IsData>([&](auto & ... member) {
                    return detail::whitebox(t, std::index_sequence<RareTs::remove_cvref_t<decltype(member)>::index...>{});
                });
            }
        }
    }
}

template <typename T> struct RareBuilder : decltype(RareTs::AdaptiveStructures::builder<T>()) {};

namespace RareMapper
{
    template <typename To, typename From> constexpr void map(To & to, const From & from);

    namespace detail
    {
        template <typename L, typename R> using MapToOp = decltype(std::declval<L>().map_to(std::declval<R &>()));
        template <typename L, typename R> using MapFromOp = decltype(std::declval<L>().map_from(std::declval<const R &>()));
        template <typename T> using ObjectMappingsOp = decltype(T::ObjectMappings_note);

        template <typename L, typename R> inline constexpr bool hasMapTo = RareTs::op_exists_v<MapToOp, L, R>;
        template <typename L, typename R> inline constexpr bool hasMapFrom = RareTs::op_exists_v<MapFromOp, L, R>;
        template <typename T> inline constexpr bool hasObjectMappings = RareTs::op_exists_v<ObjectMappingsOp, T>;

        template <typename T, bool noNote> struct Note { using type = std::tuple<>; };
        template <typename T> struct Note<T, true> { using type = decltype(T::ObjectMappings_note); };
        
        template <size_t L, size_t R> struct IndexPair {
            static constexpr size_t Left = L;
            static constexpr size_t Right = R;
        };

        template <bool Bidirectional, typename Left, typename Right, typename ... Ts> struct member_mapping : RareTs::type_id<std::tuple<Ts...>> {
            template <typename To, typename From> static constexpr bool is_for = (std::is_same_v<From, Left> && std::is_same_v<To, Right>) ||
                (Bidirectional && std::is_same_v<From, Right> && std::is_same_v<To, Left>);
            template <typename To, typename From> static constexpr bool is_forward = std::is_same_v<From, Left> && std::is_same_v<To, Right>;
        };
        
        template <typename L, typename R, typename ... Ts /* IndexPairs for previous mappings */> struct left_aggregate;

        template <typename L, typename R, typename T /* integral_constant<size_t> for L-member */, typename ... Ts /* IndexPairs for previous mappings */>
        struct right_item
        {
            template <size_t I> struct type
            {
                static constexpr size_t rightCount = RareTs::Class::member_count<R>;
                template <typename> struct filter : std::bool_constant<sizeof...(Ts)+1 < rightCount> {}; // all valid unless all R-members have been mapped

                template <size_t ... Is> struct has_repeating_index : std::false_type {}; // repeating L-members precludes bidirectional mappings
                template <size_t Index, size_t ... Is> struct has_repeating_index<Index, Is...> {
                    static constexpr bool value = ((Index == Is) || ...) || has_repeating_index<Is...>::value;
                };

                template <size_t ... Is> static constexpr auto leftAggregate(std::index_sequence<Is...>) -> // returns type of left-aggregate mapping object
                    typename left_aggregate<L, R, Ts..., IndexPair<T::value, I>>::template type<!has_repeating_index<T::value, Ts::Left...>::value, Is...>;

                template <size_t ... Is> static constexpr auto leftAgg(std::index_sequence<Is...>) -> decltype(leftAggregate( // filter out unmappable members
                    typename RareTs::template type_mask<filter, std::integral_constant<size_t, Is>...>::indexes{}));

                constexpr auto operator()() const { return decltype(leftAgg(std::make_index_sequence<RareTs::Class::member_count<L>>())){}; }
            };
        };

        template <size_t I, typename L, typename R, typename T, typename ...Ts> struct right_aggregate // Adapted members are right types
        {
            template <size_t ... Is> struct type : RareTs::Class::adapt_member<right_item<L, R, T, Ts...>::template type, R, Is>... {};
        };

        template <typename L, typename R, typename ... Ts /* IndexPairs for previous mappings */> struct left_item
        {
            template <size_t I> struct type
            {
                template <size_t, typename> struct has_right_index : std::false_type {}; // true if has right-index "I" in IndexPair<J, I>
                template <size_t Ri, size_t Li> struct has_right_index<Ri, IndexPair<Li, Ri>> : std::true_type {};

                template <typename T> struct filter : std::bool_constant<(!has_right_index<T::value, Ts>::value && ...)> {}; // R-member not already mapped

                template <size_t ... Is> static constexpr auto rightAggregate(std::index_sequence<Is...>) // returns type of right-aggregate mapping object
                    -> typename right_aggregate<I, L, R, std::integral_constant<size_t, I>, Ts...>::template type<Is...>;

                template <size_t ... Is> static constexpr auto rightAgg(std::index_sequence<Is...>) -> decltype(rightAggregate( // filter unmappable members
                    typename RareTs::template type_mask<filter, std::integral_constant<size_t, Is>...>::indexes{}));

                static constexpr decltype(rightAgg(std::make_index_sequence<RareTs::Class::member_count<R>>())) right {};

                constexpr auto operator->() { return &right; }
            };
        };

        template <typename L, typename R, typename ... Ts> struct left_aggregate // Adapted members are left types
        {
            template <bool bi, size_t ... Is> struct type : RareTs::Class::adapt_member<left_item<L, R, Ts...>::template type, L, Is>... {
                static constexpr auto bidirectional() { return member_mapping<true, L, R, Ts...>{}; } // Mapping is valid for L->R as well as R->L
                static constexpr auto unidirectional() { return member_mapping<false, L, R, Ts...>{}; } // Mapping is valid for L->R
            };
            template <size_t ... Is> struct type<false, Is...> : RareTs::Class::adapt_member<left_item<L, R, Ts...>::template type, L, Is>... {
                static constexpr auto unidirectional() { return member_mapping<false, L, R, Ts...>{}; } // Mapping is valid for L->R
            };
        };

        template <typename L, typename R, size_t ... Is>
        constexpr auto createMapping(std::index_sequence<Is...>) { return typename left_aggregate<L, R>::template type<true, Is...>{}; }

        template <typename L, typename R> using AllMappings = decltype(
            std::tuple_cat(std::declval<typename Note<L, hasObjectMappings<L>>::type>(), std::declval<typename Note<R, hasObjectMappings<R>>::type>()));

        template <typename L, typename R, typename ... Ts> struct mapping { using type = member_mapping<false, void, void>; };
        template <typename L, typename R, typename T, typename ... Ts> struct mapping<L, R, T, Ts...>
            : std::conditional<(T::template is_for<L, R>), T, typename mapping<L, R, Ts...>::type> {};
        template <typename L, typename R, typename ... Ts> struct mapping<L, R, std::tuple<Ts...>> : mapping<L, R, Ts...>::type {};
        
        template <typename L, typename R> using mapping_t = typename mapping<L, R, AllMappings<L, R>>::type;

        template <typename L, typename R> inline constexpr bool hasObjectMemberMapping =
            !std::is_same_v<mapping_t<L, R>, std::tuple<>>;
        
        template <typename To, typename From, typename ... Ts> struct NoteMapper
        {
            static constexpr void map(To & to, const From & from)
            {
                if constexpr ( mapping<To, From, AllMappings<To, From>>::template is_forward<To, From> ) { // Mapping is read in the forward direction
                    (RareMapper::map(RareTs::Member<To, Ts::Right>::value(to), RareTs::Member<From, Ts::Left>::value(from)), ...);
                } else { // This is a bi-directional mapping being read in reverse, mapping creation having been done in the from "L->R"/"From->To"
                    (RareMapper::map(RareTs::Member<To, Ts::Left>::value(to), RareTs::Member<From, Ts::Right>::value(from)), ...);
                }
            }
        };
        template <typename To, typename From> struct NoteMapper<To, From, void> { static constexpr void map(To &, const From &) {} };
        template <typename To, typename From, typename ... Ts> struct NoteMapper<To, From, std::tuple<Ts...>> : NoteMapper<To, From, Ts...> {};
        template <typename To, typename From> struct NoteMapper<To, From>
            : NoteMapper<To, From, mapping_t<To, From>> {};

        template <typename ... Ts> struct is_shared_pointable : std::false_type {}; // element_type, weak_type, use_count(), dereferencable, nullptr-comparable
        template <typename T> struct is_shared_pointable<RareTs::first_of_t<T, typename T::element_type, typename T::weak_type,
            decltype(std::declval<std::remove_cv_t<T>>().use_count()),
            decltype(*std::declval<std::remove_cv_t<T>>()), decltype(std::declval<std::remove_cv_t<T>>() == nullptr)>> : std::true_type {};
        template <typename T> inline constexpr bool is_shared_pointable_v = is_shared_pointable<T>::value;

        template <typename ... Ts> struct is_unique_pointable : std::false_type {}; // pointer, element_type, release(), dereferencable, nullptr-comparable
        template <typename T> struct is_unique_pointable<RareTs::first_of_t<T, typename T::pointer, typename T::element_type,
            decltype(std::declval<std::remove_cv_t<T>>().release()),
            decltype(*std::declval<std::remove_cv_t<T>>()), decltype(std::declval<std::remove_cv_t<T>>() == nullptr)>> : std::true_type {};
        template <typename T> inline constexpr bool is_unique_pointable_v = is_unique_pointable<T>::value;

        // Helper method for RareMapper::mapDefault(To &, From &); do not specialize this method
        template <size_t Index, typename ...To, typename ...From> MSVC_UNUSED_FALSE_POSITIVE
        constexpr void mapTuple(std::tuple<To...> & to, const std::tuple<From...> & from)
        {
            if constexpr ( Index < sizeof...(To) && Index < sizeof...(From) )
            {
                RareMapper::map(std::get<Index>(to), std::get<Index>(from));
                RareMapper::detail::mapTuple<Index+1>(to, from);
            }
        }
    }
    
    template <typename L, typename R> constexpr auto createMapping() {
        return detail::createMapping<L, R>(
            std::make_index_sequence<RareTs::Class::member_count<L>>());
    }

    // Default mapping implementation, safe to call from RareMapper::map specializations, assignment & conversion operators, and map_to/map_from methods
    // Do not specialize this method
    template <typename To, typename From> constexpr void mapDefault(To &, const From &);
    
    // Default mapping helper, safe to call from RareMapper::map specializations, assignment & conversion operators, and map_to/map_from methods
    // Do not specialize this method
    template<typename To, typename From> constexpr To mapDefault(const From & from)
    {
        To to;
        RareMapper::mapDefault(to, from);
        return to;
    }

    // If any mapping exists from "from" to "to", "to" is assigned mapped values from "from", if no mapping exists, "to" is unchanged and nothing is thrown
    // A mapping may exist if...
    // - Both "To" and "From" are reflected objects and have members with identical names and compatible types
    // - Both "To" and "From" are compatible types ("to = from" or "to = static_cast<To>(from)" is valid)
    // - Both "To" and "From" are compatible pairs, tuples, array, or STL containers
    // - An assignment operator, converting constructor, or conversion operator exists
    // - A map_to or map_from member method exists in "to" or "from"
    // - An ObjectMappings note exists in the "To" or "From" class e.g. NOTE(ObjectMappings, RareMapper::createMapping<To, From>() ... )
    // - This method was specialized
    // 
    // To avoid infinite recursion, call RareMapper::mapDefault instead of RareMapper::map when you want default mappings in...
    // RareMapper::map specializations, assignment operators, conversion operators, converting constructors, map_to, or map_from methods with the same types
    template <typename To, typename From> constexpr void map(To & to, const From & from)
    {
        if constexpr ( std::is_const_v<To> )
            return;
        else if constexpr ( detail::hasMapFrom<To, From> )
            to.map_from(from);
        else if constexpr ( detail::hasMapTo<From, To> )
            from.map_to(to);
        else if constexpr ( detail::hasObjectMemberMapping<To, From> )
            detail::NoteMapper<To, From>::map(to, from);
        else if constexpr ( RareTs::is_assignable_v<decltype(to), decltype(from)> )
            to = from;
        else if constexpr ( RareTs::is_static_cast_assignable_v<decltype(to), decltype(from)> )
            to = static_cast<To>(from);
        else
            RareMapper::mapDefault(to, from);
    }

    // Helper method for RareMapper::map(To &, From &); do not specialize this method
    template <typename To, typename From> constexpr To map(const From & from)
    {
        To to;
        RareMapper::map(to, from);
        return to;
    }

    template <typename To, typename From> constexpr void mapDefault(To & to, const From & from)
    {
        if constexpr ( std::is_const_v<To> )
            return;
        else if constexpr ( RareTs::is_pointable_v<To> )
        {
            using ToDereferenced = RareTs::remove_pointer_t<To>;
            if ( to == nullptr )
            {
                if constexpr ( RareTs::is_pointable_v<From> )
                {
                    if ( from == nullptr )
                        return;
                    else if constexpr ( detail::is_shared_pointable_v<To> )
                    {
                        if constexpr ( std::is_same_v<From, To> )
                            to = from; // Share shared pointer
                        else
                        {
                            to = RareTs::remove_cvref_t<To>(new ToDereferenced); // Equivalent of std::make_shared given a default-initialized ToDereferenced
                            RareMapper::map(*to, *from);
                        }
                    }
                    else if constexpr ( detail::is_unique_pointable_v<To> )
                    {
                        to = RareTs::remove_cvref_t<To>(new ToDereferenced); // Equivalent of std::make_unique given a default-initialized ToDereferenced
                        RareMapper::map(*to, *from);
                    }
                }
                else if constexpr ( detail::is_shared_pointable_v<To> )
                {
                    to = RareTs::remove_cvref_t<To>(new ToDereferenced); // Equivalent of std::make_shared given a default-initialized ToDereferenced
                    RareMapper::map(*to, from);
                }
                else if constexpr ( detail::is_unique_pointable_v<To> )
                {
                    to = RareTs::remove_cvref_t<To>(new ToDereferenced); // Equivalent of std::make_unique given a default-initialized ToDereferenced
                    RareMapper::map(*to, from);
                }
            }
            else // to != nullptr
            {
                if constexpr ( RareTs::is_pointable_v<From> )
                {
                    if ( from == nullptr )
                        to = nullptr;
                    else
                        RareMapper::map(*to, *from);
                }
                else
                    RareMapper::map(*to, from);
            }
        }
        else if constexpr ( RareTs::is_pointable_v<From> )
        {
            if ( from != nullptr )
                RareMapper::map(to, *from);
        }
        else if constexpr ( RareTs::is_pair_v<To> && RareTs::is_pair_v<From> )
        {
            RareMapper::map(to.first, from.first);
            RareMapper::map(to.second, from.second);
        }
        else if constexpr ( RareTs::is_tuple_v<To> && RareTs::is_tuple_v<From> )
        {
            RareMapper::detail::mapTuple<0>(to, from);
        }
        else if constexpr ( RareTs::is_iterable_v<To> && RareTs::is_iterable_v<From> )
        {
            using ToElementType = RareTs::element_type_t<std::remove_cv_t<To>>;
            if constexpr ( (RareTs::has_begin_end_v<To> || RareTs::is_adaptor_v<To>) && (RareTs::has_begin_end_v<From> || RareTs::is_adaptor_v<From>) )
            {
                RareTs::clear(to);
                for ( auto & fromElement : from )
                {
                    ToElementType toElement;
                    RareMapper::map(toElement, fromElement);
                    RareTs::append(to, toElement);
                }
            }
            else if constexpr ( RareTs::is_static_array_v<From> && RareTs::static_array_size<From>::value > 0 )
            {
                if constexpr ( RareTs::is_static_array_v<To> && RareTs::static_array_size<To>::value > 0 )
                {
                    constexpr size_t toSize = RareTs::static_array_size_v<To>;
                    constexpr size_t fromSize = RareTs::static_array_size_v<From>;
                    constexpr size_t limit = toSize < fromSize ? toSize : fromSize;
                    for ( size_t i=0; i<limit; i++ )
                        RareMapper::map(to[i], from[i]);
                }
                else
                {
                    RareTs::clear(to);
                    for ( size_t i=0; i<RareTs::static_array_size_v<From>; i++ )
                    {
                        ToElementType toElement;
                        RareMapper::map(toElement, from[i]);
                        RareTs::append(to, toElement);
                    }
                }
            }
            else if constexpr ( RareTs::is_static_array_v<To> && RareTs::static_array_size<To>::value > 0 )
            {
                size_t i=0;
                for ( auto & element : from )
                {
                    RareMapper::map(to[i], element);
                    if ( ++i == RareTs::static_array_size_v<To> )
                        break;
                }
            }
        }
        else if constexpr ( RareTs::is_reflected_v<To> && RareTs::is_reflected_v<From> )
        {
            RareTs::Reflect<To>::Members::forEach(to, [&](auto toMember, auto & toValue) {
                RareTs::Reflect<From>::Members::forEach(from, [&](auto fromMember, auto & fromValue) {
                    if constexpr ( std::string_view(toMember.name) == std::string_view(fromMember.name) )
                        RareMapper::map(toValue, fromValue);
                });
            });
        }
    }

    template <typename MappedBy, typename Type = void> struct MappedByType { using Object = Type; using DefaultMapping = MappedBy; };
    template <typename MappedBy> struct MappedByType<MappedBy, void> { using DefaultMapping = MappedBy; };

    // Member or class-level annotation saying a member or class should be mapped to "T" for activities like serialization
    template <typename T> inline constexpr MappedByType<T, void> MappedBy{};

    // Operation annotation saying type "T" should be mapped to type "MappedBy" for activities like serialization
    template <typename T, typename MappedBy> using UseMapping = MappedByType<MappedBy, T>;

    namespace detail {
        template <typename T> inline constexpr bool isMappedByNotes = RareTs::type_list<T>::template has_specialization_v<MappedByType>;

        template <typename T, typename Enable = void> struct GetMappingFromNotes { using type = void; };
        template <typename T> struct GetMappingFromNotes<T, std::enable_if_t<isMappedByNotes<T>>>
        { using type = typename RareTs::type_list<T>::template get_specialization_t<MappedByType>::DefaultMapping; };

        template <typename T, typename Enable = void> struct GetMappingByClassNote { using type = void; };
        template <typename T> struct GetMappingByClassNote<T, std::enable_if_t<isMappedByNotes<RareTs::Class::class_notes_t<T>>>>
        { using type = typename GetMappingFromNotes<RareTs::Class::class_notes_type<T>>::type; };

        template <typename T, typename Note, typename Enable = void> struct IsUseMappingNote { static constexpr bool value = false; };
        template <typename T, typename Note> struct IsUseMappingNote<T, Note, std::enable_if_t<RareTs::is_specialization_v<Note, MappedByType>>>
        { static constexpr bool value = std::is_same_v<T, typename Note::Object>; };

        template <typename T, typename ...> struct GetOpNoteMapping { using type = void; };
        template <typename T, typename ... Ts> struct GetOpNoteMapping<T, std::enable_if_t<(IsUseMappingNote<T, Ts>::value || ...)>, std::tuple<Ts...>>
        { using type = typename RareTs::type_list<std::tuple<Ts...>>::template get_specialization_t<MappedByType>::DefaultMapping; };
    }

    template <typename T> struct SetTags { using DefaultMapping = void; }; // e.g. struct RareMapper::SetTags<Foo> : IsMappedBy<Bar> {};
    template <typename T> using IsMappedBy = MappedByType<T>; // Tags a type "T" to be mapped to for activities like serialization

    // Sets default type "mappedBy" which this object "object" should be mapped to for activities like serialization
    #define SET_DEFAULT_OBJECT_MAPPING(object, mappedBy) template <> struct RareMapper::SetTags<object> : IsMappedBy<mappedBy> {};
    
    // Gets the type which "T" should be mapped to for activities like serialization (or void if no default exists)
    template <typename T, typename MemberNotes = void, typename OpNotes = void>
    struct default_mapping { using type = RareTs::replace_void_t<typename detail::GetOpNoteMapping<T, void, std::remove_const_t<OpNotes>>::type,
        RareTs::replace_void_t<typename detail::GetMappingFromNotes<MemberNotes>::type,
        RareTs::replace_void_t<typename SetTags<T>::DefaultMapping, typename detail::GetMappingByClassNote<T>::type>>>; };

    // Gets the type which "T" should be mapped to for activities like serialization (or void if no default exists)
    template <typename T, typename MemberNotes = void, typename OpNotes = void>
    using default_mapping_t = typename default_mapping<T, MemberNotes, OpNotes>::type;

    // Checks whether the type "T" has a type to use for default mappings for activies like serialization
    template <typename T, typename MemberNotes = void, typename OpNotes = void>
    struct has_default_mapping { static constexpr bool value = !std::is_void_v<default_mapping_t<T, MemberNotes, OpNotes>>; };

    // Checks whether the type "T" has a type to use for default mappings for activies like serialization
    template <typename T, typename MemberNotes = void, typename OpNotes = void>
    inline constexpr bool has_default_mapping_v = has_default_mapping<T, MemberNotes, OpNotes>::value;
}

#endif