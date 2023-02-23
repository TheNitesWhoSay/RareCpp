#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <cstddef>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace Http
{
    enum class Method : size_t
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
        size_t memberIndex;
        size_t overloadIndex;
    };

    template <size_t controllerIndex, typename Controller, typename Member, bool overloaded = false, size_t overloadIndex = 0> struct EndpointDescr;

    template <
        size_t controllerIndex,
        typename Controller,
        typename Member,
        size_t overloadIndex>
    struct EndpointDescr <controllerIndex, Controller, Member, false, overloadIndex> {
        static constexpr bool Overloaded = false;
        static constexpr size_t ControllerIndex = controllerIndex;
        static constexpr size_t MemberIndex = Member::index;
        static constexpr size_t OverloadIndex = overloadIndex;

        static constexpr Http::Method Method = [](){
            if constexpr ( Member::template hasNote<Http::Method>() )
                return Member::template getNote<Http::Method>();
            else
                return Http::Method::GET;
        }();
        static constexpr std::string_view Uri = [](){
            if constexpr ( Member::template hasNote<Rest::Uri>() )
                return Member::template getNote<Rest::Uri>().value;
            else
                return Member::name;
        }();
    };

    template <
        size_t controllerIndex,
        typename Controller,
        typename Member,
        size_t overloadIndex>
    struct EndpointDescr <controllerIndex, Controller, Member, true, overloadIndex> {
        static constexpr bool Overloaded = true;
        static constexpr size_t ControllerIndex = controllerIndex;
        static constexpr size_t MemberIndex = Member::index;
        static constexpr size_t OverloadIndex = overloadIndex;

        static constexpr auto overload = typename Member::Overloads::template Overload<overloadIndex>{};
        using Overload = RareTs::remove_cvref_t<decltype(overload)>;

        static constexpr Http::Method Method = [](){
            if constexpr ( Overload::template hasNote<Http::Method>() )
                return overload.template getNote<Http::Method>();
            else if constexpr ( Member::template hasNote<Http::Method> )
                return Member::template getNote<Http::Method>();
            else
                return Http::Method::GET;
        }();
        static constexpr std::string_view Uri = [](){
            if constexpr ( Overload::template hasNote<Rest::Uri>() )
                return overload.template getNote<Rest::Uri>().value;
            else if constexpr ( Member::template hasNote<Rest::Uri>() )
                return Member::template getNote<Rest::Uri>().value;
            else
                return Member::name;
        }();
    };
    
    template <typename Member, typename = RareTs::enable_if_member_t<Member>>
    struct IsRestEndpoint : std::bool_constant<(Member::isFunction || Member::Overloads::totalArgumentSets > 0)> {};

    template <typename ... RestControllers>
    struct Engine
    {
        static constexpr size_t TotalControllers = sizeof...(RestControllers);

        using RestControllerTuple = std::tuple<RestControllers...>;

        template <size_t C> static constexpr auto controllerEndpointMembers() {
            return RareTs::Reflect<std::tuple_element_t<C, RestControllerTuple>>::Members::template pack<IsRestEndpoint>([](auto ... member) {
                return std::tuple { member... };
            });
        }

        template <size_t C, typename Cntl, typename F, size_t ... Is> // combines overload endpoints // here Is is the note index
        static constexpr auto overloadEndpoint(std::index_sequence<Is...>) {
            return std::tuple { EndpointDescr<C, Cntl, F, true, Is>{}... };
        }

        template <size_t C, typename Cntl, typename F, typename ... Ts> static constexpr auto overloadEndpoint(const std::tuple<Ts...> &) {
            return overloadEndpoint<C, Cntl, F>(typename RareTs::type_mask<RareTs::is_overload_note, Ts...>::indexes{});
        }

        template <size_t C, typename Cntl, typename F, std::enable_if_t<F::isOverloaded>* = nullptr>
        static constexpr auto endpoint() { return overloadEndpoint<C, Cntl, F>(F::notes); }

        template <size_t C, typename Cntl, typename F, std::enable_if_t<F::isFunction>* = nullptr>
        static constexpr auto endpoint() { return std::tuple { EndpointDescr<C, Cntl, F>{} }; }

        template <size_t C, typename ... F> static constexpr auto controllerEndpoints(const std::tuple<F...> &) {
            return std::tuple_cat(endpoint<C, std::tuple_element_t<C, RestControllerTuple>, F>()...); // combines member endpoints
        }

        template <size_t ... ControllerIndex> static constexpr auto controllerEndpoints(std::index_sequence<ControllerIndex...>) {
            return std::tuple_cat(controllerEndpoints<ControllerIndex>(controllerEndpointMembers<ControllerIndex>())...); // combines controller endpoints
        }

        // While not currently in use, ControllerEndpoints represents an option for calculating all the endpoints at compile time
        // Which could then be stuck in a hyper-constexpr-optimized URL router
        using ControllerEndpoints = decltype(controllerEndpoints(std::make_index_sequence<TotalControllers>()));

        static constexpr size_t TotalEndpoints = std::tuple_size_v<ControllerEndpoints>;

        static void Debug()
        {
            std::cout << "TotalControllers: " << TotalControllers << std::endl;
            std::cout << "Endpoints [" << TotalEndpoints << "]: " << std::endl;

            RareTs::forIndexes<TotalEndpoints>([](auto I) {
                using Endpoint = std::tuple_element_t<decltype(I)::value, ControllerEndpoints>;
                std::cout << "  " << RareTs::toStr<Endpoint>() << std::endl;
                std::cout << "    " << Endpoint::Method << " " << Endpoint::Uri << std::endl;
            });
        }

        RestControllerTuple restControllers;
        std::vector<Rest::Endpoint> endpoints;

        Engine(RestControllers ... restControllers) : restControllers(std::forward_as_tuple(restControllers...))
        {
            RareTs::forIndexes<TotalControllers>([&](auto I) {

                constexpr auto i = decltype(I)::value;

                using RestControllerT = std::tuple_element_t<decltype(I)::value, std::tuple<RestControllers...>>;

                std::string controllerUri;
                if constexpr ( RareTs::Reflect<RestControllerT>::Notes::template hasNote<Rest::Uri>() )
                    controllerUri = std::string(RareTs::Reflect<RestControllerT>::Notes::template getNote<Rest::Uri>().value);
                else
                    controllerUri = "/" + Json::simplifyTypeStr(RareTs::toStr<RestControllerT>());

                RareTs::Reflect<RestControllerT>::Members::template forEach<IsRestEndpoint>([&](auto & member) {
                    using Member = std::remove_reference_t<decltype(member)>;
                    if constexpr ( Member::isFunction )
                    {
                        Http::Method httpMethod = Http::Method::GET;
                        if constexpr ( Member::template hasNote<Http::Method>() )
                            httpMethod = Member::template getNote<Http::Method>();

                        std::string endpointUri;
                        if constexpr ( Member::template hasNote<Rest::Uri>() )
                        {
                            const auto & uri = Member::template getNote<Rest::Uri>().value;
                            endpointUri = controllerUri + (controllerUri.back() == '/' || uri.front() == '/' ? "" : "/") + std::string(uri);
                        }
                        else
                            endpointUri = controllerUri + (controllerUri.back() == '/' ? "" : "/") + Member::name;

                        endpoints.push_back(Rest::Endpoint{httpMethod, endpointUri, i, Member::index, 0});
                        //const auto & added = endpoints.back();
                        //std::cout << "Registered method: " << added.method << " \"" << added.uri << "\" controller: " << added.controllerIndex << " member: " << added.memberIndex << std::endl;
                    }
                    else if constexpr ( RareTs::is_overloaded_v<RestControllerT, Member::index> )
                    {
                        size_t overloadIndex = 0;
                        Member::Overloads::forEach([&](auto overload) {
                            using Ovl = decltype(overload);
                            
                            Http::Method httpMethod = Http::Method::GET;
                            if constexpr ( Ovl::template hasNote<Http::Method>() )
                                httpMethod = overload.template getNote<Http::Method>();

                            std::string endpointUri;
                            if constexpr ( Ovl::template hasNote<Rest::Uri>() )
                            {
                                const auto & uri = overload.template getNote<Rest::Uri>().value;
                                endpointUri = controllerUri + (controllerUri.back() == '/' || uri.front() == '/' ? "" : "/") + std::string(uri);
                            }
                            else
                                endpointUri = controllerUri + (controllerUri.back() == '/' ? "" : "/") + Member::name;

                            endpoints.push_back(Rest::Endpoint{httpMethod, endpointUri, i, Member::index, overloadIndex});
                            //const auto & added = endpoints.back();
                            //std::cout << "Registered method: " << added.method << " \"" << added.uri << "\" controller: " << added.controllerIndex << " member: " << added.memberIndex << " ovl: " << overloadIndex << std::endl;
                            ++overloadIndex;
                        });
                    }
                });
            });
        }

        void handleRestEvent(Http::Method method, const std::string & uri, const std::string & /*payload*/ = "")
        {
            for ( Rest::Endpoint endpoint : endpoints )
            {
                if ( method == endpoint.method && uri.substr(0, 12).compare(endpoint.uri.substr(0, 12)) == 0 )
                {
                    RareTs::forIndex<TotalControllers>(endpoint.controllerIndex, [&](auto I) {
                        
                        using RestControllerT = std::tuple_element_t<decltype(I)::value, std::tuple<RestControllers...>>;
                        auto & restController = std::get<decltype(I)::value>(restControllers);
                        auto argBuilder = [&](auto arg) {
                            using Type = typename decltype(arg)::type;
                            constexpr size_t index = decltype(arg)::index;
                            std::string argInput = index == 0 ? "1" : "\"a\"";
                            //std::cout << "Reading input argument: " << argInput << std::endl;
                            Type type;
                            std::stringstream(argInput) >> Json::in(type);
                            //std::cout << "Read: " << RareTs::toStr<Type>() << ": " << type << std::endl;
                            return type;
                        };

                        RareTs::Reflect<RestControllerT>::Members::at(endpoint.memberIndex, [&](auto & member) {
                            using Member = std::remove_reference_t<decltype(member)>;
                            if constexpr ( Member::isFunction ) 
                            {
                                using Function = RareTs::Function<typename Member::pointer_type>;
                                Function function = Function(member.pointer);
                                if constexpr ( Member::isStatic )
                                    function.invoke(argBuilder);
                                else
                                    function.invoke(restController, argBuilder);
                            }
                            else if constexpr ( RareTs::is_overloaded_v<RestControllerT, Member::index> )
                            {
                                Member::Overloads::at(endpoint.overloadIndex, [&](auto overload) {
                                    auto function = RareTs::Function(overload.pointer);
                                    if constexpr ( Member::isStatic )
                                        function.invoke(argBuilder);
                                    else
                                        function.invoke(restController, argBuilder);
                                });
                            }
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
        RareTs::Overload<int>             (Http::Method::GET, Rest::Uri{"/hello/{a}"}),
        RareTs::Overload<int, std::string>(Http::Method::GET, Rest::Uri{"/hello/{a}/{b}"}))
    void sayHi(int a)
    {
        std::cout << "hi " << a << std::endl;
    }
    void sayHi(int a, std::string b)
    {
        std::cout << "altHi " << a << ", " << b << std::endl;
    }
    void sayHi(float a) {
        std::cout << "hi " << a << std::endl;
    }
    
    NOTE(sayHiSolo, Http::Method::GET, Rest::Uri{"/hi/{a}/{b}"})
    void sayHiSolo(int a, std::string b)
    {
        std::cout << "hiSolo " << a << ", " << b << std::endl;
    }

    NOTE(postit, Http::Method::POST)
    static int postit(std::string stuff)
    {
        std::cout << "postingit" << stuff << std::endl;
        return 5;
    }

    REFLECT_NOTED(TestController, sayHi, sayHiSolo, postit)
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

void restExperiment()
{
    restEngine.handleRestEvent(Http::Method::GET, "/my/path/hello/1/a");
    restEngine.handleRestEvent(Http::Method::GET, "/my/path/altHello/1");

    decltype(restEngine)::Debug();
}
