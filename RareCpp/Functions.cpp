/*
    ! This code is experimental for future functionality and not yet supported !
*/

#include "../RareCppLib/Reflect.h"
#include "../RareCppLib/Json.h"
#include <iostream>
using ExtendedTypeSupport::TypeToStr;
using Reflection::Filter;

namespace FunctionDeduction
{
    template <typename F, typename ... Ts> constexpr F returnType(F(*&&)(Ts...));

    template <typename T, typename F, typename ... Ts> constexpr F returnType(F(T::*&&)(Ts...));
    
    template <size_t I, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>> argument(F(*&&)(Ts...));

    template <size_t I, typename T, typename F, typename ... Ts> constexpr std::tuple_element_t<I, std::tuple<Ts...>> argument(F(T::*&&)(Ts...));

    template <typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(*&&)(Ts...));

    template <typename T, typename F, typename ... Ts> constexpr std::tuple<Ts...> arguments(F(T::*&&)(Ts...));

    template <typename F, typename ... Ts> constexpr void memberOf(F(*&&)(Ts...));

    template <typename T, typename F, typename ... Ts> constexpr T memberOf(F(T::*&&)(Ts...));
};

template <size_t I, typename F> using argument_t = decltype(FunctionDeduction::argument<I>(std::declval<std::remove_reference_t<F>>()));

template <typename F> using arguments_t = decltype(FunctionDeduction::arguments(std::declval<std::remove_reference_t<F>>()));

template <typename F> using return_type_t = decltype(FunctionDeduction::returnType(std::declval<std::remove_reference_t<F>>()));

template <typename F> inline constexpr size_t argument_count_v = std::tuple_size_v<arguments_t<F>>;

template <typename F> using member_of_t = decltype(FunctionDeduction::memberOf(std::declval<std::remove_reference_t<F>>()));

template <typename MetaFunction, typename ... Ts>
struct invoker
{
    template <typename T, size_t I> struct Argument {
        using Function = MetaFunction;
        using type = T;
        static constexpr size_t Index = I;
    };

    template <typename Function, typename ArgBuilder, size_t... I>
    static constexpr auto invoke(Function && function, ArgBuilder && argBuilder, std::index_sequence<I...>) {
        return function(argBuilder(Argument<Ts, I>{})...);
    }

    template <typename Function, typename ArgBuilder, typename T, size_t... I>
    static constexpr auto invoke(T && t, Function && function, ArgBuilder && argBuilder, std::index_sequence<I...>) {
        return (t.*function)(argBuilder(Argument<Ts, I>{})...);
    }
};

template <typename MetaFunction, typename ... Ts> struct invoker<MetaFunction, std::tuple<Ts...>> : invoker<MetaFunction, Ts...> {};
template <typename MetaFunction, typename ... Ts> struct invoker<MetaFunction, const std::tuple<Ts...>> : invoker<MetaFunction, Ts...> {};

template <typename F>
struct Function
{
    using Pointer = F;
    Pointer pointer;
    constexpr Function(F f) : pointer(f) {}

    using Arguments = arguments_t<F>;
    static constexpr size_t ArgumentCount = std::tuple_size_v<Arguments>;
    template <size_t I> using Argument = std::tuple_element_t<I, Arguments>;
    static constexpr bool IsMemberFunction = std::is_member_function_pointer_v<F>;
    using Return = return_type_t<F>;
    using MemberOf = member_of_t<F>;

    template <typename ArgBuilder> inline auto invoke(ArgBuilder && argBuilder) const {
        return invoker<Function<F>, Arguments>::template invoke(
                pointer,
                std::forward<ArgBuilder>(argBuilder),
                std::make_index_sequence<ArgumentCount>{}
            );
    };

    template <typename T, typename ArgBuilder>
    inline auto invoke(T && t, ArgBuilder && argBuilder) const {
        if constexpr ( IsMemberFunction )
        {
            if constexpr ( ExtendedTypeSupport::is_tuple<T>::value )
            {
                constexpr size_t index = ExtendedTypeSupport::index_of<MemberOf, T>::value;
                if constexpr ( index < std::tuple_size_v<std::remove_cv_t<std::remove_reference_t<T>>> )
                {
                    return invoker<Function<F>, Arguments>::template invoke(
                            std::get<index>(std::forward<T>(t)),
                            pointer,
                            std::forward<ArgBuilder>(argBuilder),
                            std::make_index_sequence<ArgumentCount>{}
                        );
                }
                else
                    return nullptr;
            }
            else
            {
                return invoker<Function<F>, Arguments>::template invoke(
                        std::forward<T>(t),
                        pointer,
                        std::forward<ArgBuilder>(argBuilder),
                        std::make_index_sequence<ArgumentCount>{}
                    );
            }
        }
        else
        {
            return invoker<Function<F>, Arguments>::template invoke(
                    pointer,
                    std::forward<ArgBuilder>(argBuilder),
                    std::make_index_sequence<ArgumentCount>{}
                );
        }
    };

    //friend std::ostream & operator<<(std::ostream & os, const Function<F> & info);
};

template <typename T, typename ... Ts> struct clean_tuple {};
template <> struct clean_tuple<std::tuple<void>> { using type = std::tuple<>; };
template <typename T> struct clean_tuple<std::tuple<T>> { using type = std::tuple<T>; };
template <typename T, typename ... Ts> struct clean_tuple<std::tuple<T, Ts...>> {
	using type = decltype(std::tuple_cat(std::declval<typename clean_tuple<std::tuple<T>>::type>(), 
			std::declval<typename clean_tuple<std::tuple<Ts...>>::type>()));
};

template <typename T, typename R, typename ... Args>
struct overload_identifier
{
    template <size_t i, typename = std::remove_reference_t<T>> struct guard;
    template <typename D> struct guard<0, D> {
        static constexpr auto of(R(*p)(Args...)) { return p; }
        static constexpr auto of(R(*p)(Args...) noexcept) { return p; }
        static constexpr auto of(R(D::*p)(Args...)) { return p; };
        static constexpr auto of(R(D::*p)(Args...) noexcept) { return p; };
    };
    template <typename D> struct guard<1, D> {
        static constexpr auto of(R(D::*p)(Args...) const) { return p; };
        static constexpr auto of(R(D::*p)(Args...) const noexcept) { return p; };
    };
    template <typename D> struct guard<2, D> {
        static constexpr auto of(R(D::*p)(Args...) volatile) { return p; };
        static constexpr auto of(R(D::*p)(Args...) volatile noexcept) { return p; };
    };
    template <typename D> struct guard<3, D> {
        static constexpr auto of(R(D::*p)(Args...) const volatile) { return p; };
        static constexpr auto of(R(D::*p)(Args...) const volatile noexcept) { return p; };
    };
    template <typename D> struct guard<4, D> {
        static constexpr auto of(R(D::*p)(Args...) &) { return p; };
        static constexpr auto of(R(D::*p)(Args...) & noexcept) { return p; };
    };
    template <typename D> struct guard<5, D> {
        static constexpr auto of(R(D::*p)(Args...) const &) { return p; };
        static constexpr auto of(R(D::*p)(Args...) const & noexcept) { return p; };
    };
    template <typename D> struct guard<6, D> {
        static constexpr auto of(R(D::*p)(Args...) volatile &) { return p; };
        static constexpr auto of(R(D::*p)(Args...) volatile & noexcept) { return p; };
    };
    template <typename D> struct guard<7, D> {
        static constexpr auto of(R(D::*p)(Args...) const volatile &) { return p; }; 
        static constexpr auto of(R(D::*p)(Args...) const volatile & noexcept) { return p; }; 
    };
    template <typename D> struct guard<8, D> {
        static constexpr auto of(R(D::*p)(Args...) &&) { return p; };
        static constexpr auto of(R(D::*p)(Args...) && noexcept) { return p; };
    };
    template <typename D> struct guard<9, D> {
        static constexpr auto of(R(D::*p)(Args...) const &&) { return p; };
        static constexpr auto of(R(D::*p)(Args...) const && noexcept) { return p; };
    };
    template <typename D> struct guard<10, D> {
        static constexpr auto of(R(D::*p)(Args...) volatile &&) { return p; };
        static constexpr auto of(R(D::*p)(Args...) volatile && noexcept) { return p; };
    };
    template <typename D> struct guard<11, D> {
        static constexpr auto of(R(D::*p)(Args...) const volatile &&) { return p; };
        static constexpr auto of(R(D::*p)(Args...) const volatile && noexcept) { return p; };
    };
    
    using id = guard<(std::is_rvalue_reference_v<T> ? 8 : (std::is_lvalue_reference_v<T> ? 4 : 0))
        + (std::is_volatile_v<std::remove_reference_t<T>> ? 2 : 0) + std::is_const_v<std::remove_reference_t<T>>>;
};

template <class Id, class T, bool Exists> struct overloaded_pointer {
    using type = void;
    static constexpr auto pointer = nullptr;
};
template <class Id, class T> struct overloaded_pointer<Id, T, true> {
    using type = typename Id::template pointer_type<T>;
    static constexpr auto pointer = Id::template pointer<T>;
};

template <typename Info, typename Id, typename ... Ts> struct active_overloads {
    using class_types = typename clean_tuple<std::tuple< std::conditional_t<ExtendedTypeSupport::OpExists<void, Id::template pointer_type, Ts>::value, Ts, void>... >>::type;
    static constexpr auto overload_pointers = std::tuple{ Info::template overload_pointer<Ts>... };
};
template <typename Info, typename Id, typename ... Ts> struct active_overloads<Info, Id, std::tuple<Ts...>> : active_overloads<Info, Id, Ts...> {};

template <typename F, typename ... Args> struct is_non_member_function {
    template <typename R> static constexpr std::true_type id(R(*&&)(Args...));
    template <typename T> static constexpr std::false_type id(T && t);
    static constexpr bool value = decltype(id(std::declval<std::remove_reference_t<F>>()))::value;
};
template <typename ... Args> struct is_non_member_function<void, Args...> { static constexpr bool value = false; };
template <typename ... Args> inline constexpr bool is_non_member_function_v = is_non_member_function<Args...>::value;

template <typename ClassType, typename Id, const char* FunctionName, typename ... Args>
struct OverloadInfo
{
    using This = OverloadInfo<ClassType, Id, FunctionName, Args...>;
    using Class = ClassType;
    using Arguments = std::tuple<Args...>;
    static constexpr std::string_view name = FunctionName;

    template <typename T> using overload_pointer_t =
        typename overloaded_pointer<Id, T, ExtendedTypeSupport::OpExists<void, Id::template pointer_type, T>::value>::type;

    template <typename T> static constexpr auto overload_pointer =
        overloaded_pointer<Id, T,ExtendedTypeSupport::OpExists<void, Id::template pointer_type, T>::value>::pointer;

    static constexpr bool is_static = is_non_member_function_v<overload_pointer_t<Class>, Args...>;

    using overload_class_types = typename active_overloads<This, Id, Class, const Class, volatile Class, const volatile Class, Class &, const Class &,
        volatile Class &, const volatile Class &, Class &&, const Class &&, volatile Class &&, const volatile Class &&>::class_types;

    static constexpr auto overload_pointers = active_overloads<This, Id, overload_class_types>::overload_pointers;

    using overload_pointers_t = std::remove_const_t<decltype(overload_pointers)>;

    static constexpr size_t total = std::tuple_size_v<overload_pointers_t>;
};

// part of overload struct below perhaps? maybe unnecessary?
//template <class ... Ts> static constexpr auto static_caller(Ts && ... ts) { return  }

#define ID_OVERLOADS(x) template <typename U_> struct FieldDescr<IndexOf::x, U_> { \
    template <class ... Args_> struct overload { \
        template <class T> using return_type = decltype(std::declval<T>().x(std::declval<Args_>()...)); \
        template <class T> using pointer_type = decltype(overload_identifier<T,return_type<T>,Args_...>::id::of(&std::remove_reference_t<T>::x)); \
        template <class T> static constexpr auto pointer = static_cast<pointer_type<T>>(&std::remove_reference_t<T>::x); \
    }; \
    template <typename T_> static constexpr ExtendedTypeSupport::TypePair<decltype(T_::x), decltype(&T_::x)> id(int); \
    template <typename T_> static constexpr ExtendedTypeSupport::TypePair<decltype(T_::x), std::nullptr_t> id(unsigned int); \
    template <typename T_> static constexpr ExtendedTypeSupport::TypePair<decltype(&T_::x), decltype(&T_::x)> id(...); \
    template <typename T_> using IdOp = decltype(id<T_>(0)); \
    static constexpr bool is_overloaded = !ExtendedTypeSupport::OpExists<void, IdOp, ClassType>::value; \
    template <typename T_, bool IsOverloaded> struct Id { using type = decltype(id<T_>(0)); }; \
    template <typename T_> struct Id<T_, true> { using type = ExtendedTypeSupport::TypePair<void, std::nullptr_t>; }; \
    using Type = typename Id<ProxyType, is_overloaded>::type::Left; \
    using Pointer = typename Id<ProxyType, is_overloaded>::type::Right; \
    template <typename T_, bool NotPointable> struct GetPointer { static constexpr auto value = &T_::x; }; \
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
        static constexpr Pointer p { GetPointer<ProxyType, is_overloaded || std::is_reference_v<Type>>::value }; }; \
    using Field = Reflection::Fields::Field<Type, FieldBase, FieldDescr<IndexOf::x, U_>, Pointer, IndexOf::x, decltype(idNote<ProxyType>(0))>; \
    static constexpr Field field {}; \
    STATIC_VALUE_HELPER(x) \
}; using x##_ = FieldDescr<IndexOf::x>;

#define REFLECT_FUNCTIONS(objectType, ...) struct Class { \
    using ProxyType = objectType; \
    using ClassType = objectType; \
    enum_t(IndexOf, size_t, { __VA_ARGS__ }); \
    static constexpr size_t Total = COUNT_ARGUMENTS(__VA_ARGS__); \
    template <size_t FieldIndex, typename U_ = void> struct FieldDescr; \
    FOR_EACH(ID_OVERLOADS, __VA_ARGS__) \
};

template <typename T, size_t I, typename ... Ts>
using overload_info = OverloadInfo<T, typename T::Class::template FieldDescr<I>::template overload<Ts...>, T::Class::template FieldDescr<I>::field.name, Ts...>;

template <typename ClassType, typename Id, const char* functionName, typename ... Args>
std::ostream & operator<<(std::ostream & os, const OverloadInfo<ClassType, Id, functionName, Args...> & overloadInfo) {
    using Info = std::remove_cv_t<std::remove_reference_t<decltype(overloadInfo)>>;
    using argument_types = typename Info::Arguments;
    constexpr size_t numOverloads = Info::total;
    constexpr size_t numArguments = std::tuple_size_v<argument_types>;

    if constexpr ( numOverloads > 0 )
    {
        std::cout << overloadInfo.name << "(";
        ExtendedTypeSupport::ForIntegralConstants<numArguments>([&](auto I) {
            using Arg = std::tuple_element_t<decltype(I)::value, argument_types>;
            if constexpr ( decltype(I)::value > 0 )
                std::cout << ", ";
            std::cout << TypeToStr<Arg>();
        });
        std::cout << ")" << std::endl;

        using overloads = decltype(Info::overload_pointers);
        ExtendedTypeSupport::ForIntegralConstants<numOverloads>([&](auto I) {
            using overload = std::tuple_element_t<decltype(I)::value, overloads>;
            std::cout << "\t" << TypeToStr<overload>() << std::endl;
        });
    }
    return os;
}

struct bar
{
    int foo(char) { return 1; }
    int foo(char) const noexcept { return 2; }
    int foo(char) volatile { return 3; }
    int foo(char) const volatile { return 4; }

    int foo(char, int) { return 5; }

    int refFoo(char) & { return 5; }
    int refFoo(char) const & { return 6; }
    int refFoo(char) volatile & { return 7; }
    bool refFoo(char) const volatile & { return false; }
    int refFoo(char) && { return 9; }
    int refFoo(char) const && { return 10; }
    int refFoo(char) volatile && { return 11; }
    int refFoo(char) const volatile && noexcept { return 12; }

    int refFoo(char, int) & { return 1337; }

    static int staticFoo(char) { return 13; }
    static int staticFoo(char, int) { return 13; }

    int a;
    static int b;
    int & c;
    static int & d;
    int e() { return 1337; }
    static int f() { return 13; }

    REFLECT_FUNCTIONS(bar, foo, refFoo, staticFoo, a, b, c, d, e, f, nonexists)
};

/*template <typename F>
std::ostream & operator<<(std::ostream & os, const Function<F> & info) {
    using Function = std::remove_const_t<std::remove_reference_t<decltype(info)>>;
    os << "Function: " << TypeToStr<Function>() << std::endl
        << "\tMemberOf: " << (Function::IsMemberFunction ? TypeToStr<Function::MemberOf>() : "(none)") << std::endl
        << "\tReturnType: " << TypeToStr<Function::Return>() << std::endl;
    if constexpr ( Function::ArgumentCount > 0 )
    {
        os << "\tArguments:";
        ExtendedTypeSupport::ForIntegralConstants<Function::ArgumentCount>([&](auto A) {
            constexpr size_t argIndex = decltype(A)::value;
            using ArgType = typename Function::template Argument<argIndex>;
            os << std::endl << "\t\t [" << argIndex << "]: " << TypeToStr<ArgType>();
        });
    }
    else
        os << "\tArguments: (none)";
        
    return os;
}*/

namespace Http
{
    enum class Method
    {
        GET,
        POST,
        PUT,
        PATCH,
        DELETE
    };
}

std::ostream & operator<<(std::ostream & os, Http::Method method)
{
    switch ( method ) {
    case Http::Method::GET: os << "GET"; break;
    case Http::Method::POST: os << "POST"; break;
    case Http::Method::PUT: os << "PUT"; break;
    case Http::Method::PATCH: os << "PATCH"; break;
    case Http::Method::DELETE: os << "DELETE"; break;
    }
    return os;
}

namespace Rest
{
    struct Uri
    {
        std::string_view value;
    };

    struct Endpoint
    {
        Http::Method method;
        std::string uri;
        size_t controllerIndex;
        size_t fieldIndex;
    };

    template <typename ... RestControllers>
    struct Engine
    {
        static constexpr size_t TotalControllers = sizeof...(RestControllers);
        std::tuple<RestControllers...> restControllers;
        std::vector<Rest::Endpoint> endpoints;

        Engine(RestControllers ... restControllers) : restControllers(std::forward_as_tuple(restControllers...))
        {
            ExtendedTypeSupport::ForIntegralConstants<TotalControllers>([&](auto I) {

                using RestController = std::tuple_element_t<decltype(I)::value, std::tuple<RestControllers...>>;

                std::string controllerUri;
                if constexpr ( Reflect<RestController>::Notes::template Has<Rest::Uri> )
                    controllerUri = std::string(Reflect<RestController>::Notes::template Get<Rest::Uri>().value);
                else
                    controllerUri = "/" + Json::simplifyTypeStr(TypeToStr<RestController>());

                Reflect<RestController>::Fields::template ForEach<Filter::IsFunction>([&](auto & field) {
                    using Field = std::remove_reference_t<decltype(field)>;
                    Http::Method httpMethod = Http::Method::GET;
                    if constexpr ( Field::template HasAnnotation<Http::Method> )
                        httpMethod = Field::template getAnnotation<Http::Method>();

                    std::string endpointUri;
                    if constexpr ( Field::template HasAnnotation<Rest::Uri> )
                    {
                        const auto & uri = Field::template getAnnotation<Rest::Uri>().value;
                        endpointUri = controllerUri + (controllerUri.back() == '/' || uri.front() == '/' ? "" : "/") + std::string(uri);
                    }
                    else
                        endpointUri = controllerUri + (controllerUri.back() == '/' ? "" : "/") + Field::name;

                    endpoints.push_back(Rest::Endpoint{httpMethod, endpointUri, decltype(I)::value, Field::Index});
                    const auto & added = endpoints.back();
                    std::cout << "Registered method: " << added.method << " \"" << added.uri << "\" controller: " << added.controllerIndex << " field: " << added.fieldIndex << std::endl;
                });
            });
        }

        void handleRestEvent(Http::Method method, const std::string & uri, const std::string & payload = "")
        {
            for ( Rest::Endpoint endpoint : endpoints )
            {
                if ( method == endpoint.method && uri.substr(0, 10).compare(endpoint.uri.substr(0, 10)) == 0 )
                {
                    ExtendedTypeSupport::ForIntegralConstant<TotalControllers>(endpoint.controllerIndex, [&](auto I) {
                        
                        using RestController = std::tuple_element_t<decltype(I)::value, std::tuple<RestControllers...>>;
                        auto & restController = std::get<decltype(I)::value>(restControllers);

                        Reflect<RestController>::Fields::At(endpoint.fieldIndex, [&](auto & field) {
                            using Field = std::remove_reference_t<decltype(field)>;
                            using Function = Function<typename Field::Pointer>;
                            Function function = Function(field.p);
                            constexpr size_t totalArguments = Function::ArgumentCount;
                            auto argBuilder = [&](auto arg) {
                                using Type = typename decltype(arg)::type;
                                constexpr size_t Index = decltype(arg)::Index;
                                std::string argInput = Index == 0 ? "1" : "a";
                                //std::cout << "Reading input argument: " << argInput << std::endl;
                                Type type;
                                std::stringstream(argInput) >> Json::in(type);
                                return type;
                            };
                            if constexpr ( Field::IsStatic )
                                function.invoke(argBuilder);
                            else
                                function.invoke(restController, argBuilder);
                        });
                    });
                }
            }
        }
    };
}

NOTE(TestController, Rest::Uri{"/my/path"})
struct TestController
{
    NOTE(sayHi,
        Http::Method::GET,
        Rest::Uri{"/hello/{a}/{b}"})
    void sayHi(int a, char b)
    {
        std::cout << "hi " << a << ", " << b << std::endl;
    }

    NOTE(postit,
        Http::Method::POST)
    static int postit(std::string stuff)
    {
        std::cout << "postingit" << stuff << std::endl;
        return 5;
    }

    REFLECT_NOTED(TestController, sayHi, postit)
};

NOTE(AnotherTestController, Rest::Uri{"/my/other/path"})
struct AnotherTestController
{
    NOTE(add,
        Http::Method::GET,
        Rest::Uri{"/add/{l}/{r}"})
    static int add(int l, int r)
    {
        return l+r;
    }

    NOTE(str,
        Http::Method::PATCH)
    static std::string str(std::string stuff)
    {
        return stuff + " modifications";
    }

    REFLECT_NOTED(AnotherTestController, add, str)
};

Rest::Engine restEngine(TestController{}, AnotherTestController{});

int foo(int arg) { return 0; }

/*template <typename ... Ts>
struct FunctionType {};

template <typename ... Ts>
inline constexpr FunctionType<Ts...> Function{};

template <typename ... Ts>
inline constexpr FunctionType<Ts...> overload{

};

template <typename ... Ts>
inline constexpr FunctionType<Ts...> const_overload{};


template <typename T = void>
struct Arg
{
    using type = T;
    std::string_view name;
};

/// Extractor_Blank (when you append "EO_B" with an expression of the form "(LHS) RHS", the "(LHS)" gets blanked out and you're left with just RHS
#define EO_B(...)

/// Extractor_First
#define EO_F(x, y) x

/// Extractor_GetFirst
#define EO_G(x) ML_C(EO_F, x)

/// Extractor_WrapAndAppendComma
#define EO_W(x) (x),

// If x takes the form "(LHS) RHS", then this macro returns LHS
#define LHS(x) EO_G((ML_E EO_W x))

// If x takes the form "(LHS) RHS", then this macro returns RHS
#define RHS(x) EO_B x

#define STRINGIFY(x) #x

#define DESCRIBE_FUNCTION(x) struct RHS(x)##_ { \
    static constexpr auto pointer = &RHS(x); \
    static constexpr auto name = RHS(x); \
    static constexpr auto args = "" LHS(x); \
};

#define REFLECT_FUNCTIONS(...) FOR_EACH_POSITIVE(DESCRIBE_FUNCTION, __VA_ARGS__)*/

void functionExperiment()
{
    ExtendedTypeSupport::ForIntegralConstants<bar::Class::Total>([&](auto I) { // I = fieldIndex
        std::cout << bar::Class::FieldDescr<decltype(I)::value>::field.name 
            << " " << (bar::Class::FieldDescr<decltype(I)::value>::is_overloaded ? "[ovl]" : "[non]")
            << " [" << TypeToStr<typename bar::Class::FieldDescr<decltype(I)::value>::Type>() << "]"
            << " [" << TypeToStr<typename bar::Class::FieldDescr<decltype(I)::value>::Pointer>() << "]"
            << std::endl;
        std::cout << overload_info<bar, 0, char>{} << std::endl;
        std::cout << overload_info<bar, 0, char, int>{} << std::endl;
    });

    restEngine.handleRestEvent(Http::Method::GET, "/my/path/hello/1/a");
}
