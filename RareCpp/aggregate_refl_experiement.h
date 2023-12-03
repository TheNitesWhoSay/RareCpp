#ifndef AGGREGATE_REFL_EXPERIMENT
#define AGGREGATE_REFL_EXPERIMENT
#include <cstddef>
#include <source_location>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace detail
{
    struct any { template <class T> constexpr operator T(); };

    template <typename T> requires std::is_aggregate_v<T>
    struct member_counter
    { // Credit for algorithm: https://github.com/Tsche/repr/blob/master/include/librepr/reflection/detail/arity.h (MIT License)
        template <typename ... Ts>
        static consteval auto len(auto ... args) {
            if constexpr ( requires { T{args..., {Ts{}..., any{}}}; } )
                return len<Ts..., any>(args...);
            else
                return sizeof...(Ts);
        }

        static consteval auto count_simple(auto ... args) {
            if constexpr ( requires { T{args..., any{}}; } )
                return count_simple(args..., any{});
            else
                return sizeof...(args);
        }

        template <typename ... Ts>
        static consteval auto count_agg(auto ... args) {
            if constexpr ( requires { T{args..., {any{}, any{}}, Ts{}..., any{}}; } )
                return count_agg<Ts..., any>(args...);
            else
                return sizeof...(args) + sizeof...(Ts) + 1;
        }

        static consteval auto count(size_t skip = 0, auto ... args) {
            if constexpr ( requires { T{args..., {any{}, any{}}}; } ) {
                if constexpr ( count_agg(args...) != count_simple(args...) )
                    skip += len(args...) - 1;
                
                return count(skip, args..., any{});
            }
            else if constexpr ( requires { T{args..., any{}}; } )
                return count(skip, args..., any{});
            else
                return sizeof...(args) - skip;
        }
    };

    template <typename ... Ts>
    struct unref_tuple_elements {
        using type = std::tuple<std::remove_reference_t<Ts>...>;
    };
    template <template <typename...> class Tup, typename ... Ts>
    struct unref_tuple_elements<Tup<Ts...>> : unref_tuple_elements<Ts...> {};

    template <typename T> using unref_tuple_elements_t = typename unref_tuple_elements<T>::type;

    #ifdef __clang__
    #pragma clang diagnostic ignored "-Wundefined-var-template"
    template <class T> struct val_wrapper { T v; };
    template <class T> constexpr auto make_val_wrapper(T v) { return val_wrapper<T>{v}; }
    #endif

    template <class T>
    extern const T fake_obj;

    template <class T, auto MemberAddr> constexpr auto name() {
        #if defined(__GNUC__) && !defined(__clang__)
        auto str = std::string_view{std::source_location::current().function_name()};
        str.remove_prefix(str.find_last_of("::")+1);
        str.remove_suffix(str.size()-str.find_first_of(")]"));
        #elif defined(_MSC_VER) && !defined(__clang__)
        auto str = std::string_view{__FUNCSIG__};
        str.remove_prefix(str.find_last_of("-")+1);
        str.remove_prefix(str.find_first_of(">")+1);
        str.remove_suffix(str.size()-str.find_first_of(">"));
        #else // Assume __clang__/clang-like formatting
        auto str = std::string_view{std::source_location::current().function_name()};
        str.remove_prefix(str.find_last_of(".")+1);
        str.remove_suffix(str.size()-str.find_first_of("}]"));
        #endif
        return str;
    }
}

template <typename T> requires std::is_aggregate_v<T>
inline constexpr size_t member_count = detail::member_counter<T>::count();

template <typename T>
requires (std::is_aggregate_v<std::remove_cvref_t<T>> && !std::is_array_v<std::remove_cvref_t<T>>)
constexpr auto members_of(T && o) {
    constexpr auto count = member_count<std::remove_cvref_t<T>>;
    if constexpr ( count == 0 ) {
        return std::tie();
    } else if constexpr ( count == 1 ) {
        auto & [a] = o;
        return std::tie(a);
    } else if constexpr ( count == 2 ) {
        auto & [a, b] = o;
        return std::tie(a, b);
    } else if constexpr ( count == 3 ) {
        auto [a, b, c] = o;
        return std::tie(a, b, c);
    } else if constexpr ( count == 4 ) {
        auto & [a, b, c, d] = o;
        return std::tie(a, b, c, d);
    } else if constexpr ( count == 5 ) {
        auto & [a, b, c, d, e] = o;
        return std::tie(a, b, c, d, e);
    } else {
        static_assert(std::is_void_v<T>, "TODO: More members unimplemented");
    }
}

template <class T> using member_types = detail::unref_tuple_elements_t<decltype(members_of(std::declval<T>()))>;

template <size_t I, class T> using member_type = std::tuple_element_t<I, member_types<T>>;

template <size_t I, class T> constexpr decltype(auto) member_ref(T && t) {
    return std::get<I>(members_of(std::forward<T>(t)));
}

template <size_t I, class T> inline constexpr auto member_name = detail::name<T,
#ifdef __clang__
    detail::make_val_wrapper(std::addressof(std::get<I>(members_of(detail::fake_obj<T>)))) >();
#else
    std::addressof(std::get<I>(members_of(detail::fake_obj<T>))) >();
#endif



namespace experimental_agg
{

struct Empty {};                                            static_assert(member_count<Empty> == 0);
struct One { int a; };                                      static_assert(member_count<One> == 1);
struct Two { int a; int b; };                               static_assert(member_count<Two> == 2);
struct Three { int a; int b; int c; };                      static_assert(member_count<Three> == 3);
struct OneRay { int a[2]; };                                static_assert(member_count<OneRay> == 1);
struct TwoRay { int a[2]; int b[2]; };                      static_assert(member_count<TwoRay> == 2);
struct ThreeRay { int a[2]; int b[2]; int c[2]; };          static_assert(member_count<ThreeRay> == 3);
struct OneObj { Empty a; };                                 static_assert(member_count<OneObj> == 1);
struct TwoObj { Empty a; Empty b; };                        static_assert(member_count<TwoObj> == 2);
struct ThreeObj { Empty a; Empty b; Empty c; };             static_assert(member_count<ThreeObj> == 3);
struct OneObjRay { Empty a[2]; };                           static_assert(member_count<OneObjRay> == 1);
struct TwoObjRay { Empty a[2]; Empty b[2]; };               static_assert(member_count<TwoObjRay> == 2);
struct ThreeObjRay { Empty a[2]; Empty b[2]; Empty c[2]; }; static_assert(member_count<ThreeObjRay> == 3);

struct HT1 { int a[12]; int b; int c; int d; }; static_assert(member_count<HT1> == 4);
struct HT2 { int a; int b; int c; int d[12]; }; static_assert(member_count<HT2> == 4);
struct HT3 { Empty a[12]; int b; int c; int d; }; static_assert(member_count<HT3> == 4);
struct HT4 { int a[12]; int b; int c; Empty d[12]; }; static_assert(member_count<HT4> == 4);
struct HT5 { Empty a[12]; int b[12]; Empty c; int d; Empty e[12]; }; static_assert(member_count<HT5> == 5);

struct Bar {
    int* a;
    static inline int* b;
    void foo() {}
    void (Bar::* bar)() = &Bar::foo;
};

static_assert(member_count<Bar> == 2);

struct QTest
{
    int a = 0;
    const int some_fairly_long_complex_identifier = 1;
};

static_assert(2 == member_count<QTest>);
static_assert(std::is_same_v<std::tuple<int &, const int &>, decltype(members_of(QTest{}))>);
static_assert(std::is_same_v<std::tuple<int, const int>, member_types<QTest>>);
static_assert(std::is_same_v<int, member_type<0, QTest>>);
static_assert(std::is_same_v<const int, member_type<1, QTest>>);
static_assert(0 == member_ref<0>(QTest{}));
static_assert(1 == member_ref<1>(QTest{}));
static_assert(std::string_view{"a"} == member_name<0, QTest>);
static_assert(std::string_view{"some_fairly_long_complex_identifier"} == member_name<1, QTest>);

}

#endif