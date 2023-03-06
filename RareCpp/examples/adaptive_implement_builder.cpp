#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>

inline namespace adaptive_implement_builder
{
    inline namespace AdaptiveBuilder
    {
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
                    return remaining(typename RareTs::remove_index<I, Is...>::type{}); // Remove member index from set of indexes after you've used it
                                                                                       // Return a builder ("sub_builder") for remaining members
                }
            };
        };

        template <typename T, size_t ... Is>
        class sub_builder : public RareTs::Class::adapt_member<member_builder<T, sub_builder<T, Is...>, Is...>::template type, T, Is>...
        {
            T & t;

        public:
            constexpr T build() { return t; }
            constexpr sub_builder(T & t) : RareTs::Class::adapt_member<member_builder<T, sub_builder<T, Is...>, Is...>::template type, T, Is> {{t}}..., t(t) {}
        };

        template <typename T, size_t ... Is>
        class head_builder : public sub_builder<T, Is...>
        {
            T t{};

        public:
            constexpr head_builder() : sub_builder<T, Is...>(t), t({}) {}
        };

        template <typename T, size_t ... Is> constexpr auto builder(std::index_sequence<Is...>) {
            return head_builder<T, Is...>{};
        }
    
        template <typename T> constexpr auto builder() {
            // Create a builder using member indexes, you could apply a member filter here (e.g. use non-static data members only)
            return RareTs::template Members<T>::template pack([&](auto & ... member) {
                return builder<T>(std::index_sequence<RareTs::remove_cvref_t<decltype(member)>::index...>{});
            });
        }
    }

    struct MyObj
    {
        int a;
        std::string b;
        float c;

        REFLECT(MyObj, a, b, c)
    };

    void adaptiveImplementBuilder()
    {
        // Builder for reflected members, strongly typed, removes members from list of those available to set after they're used
        auto myObj = builder<MyObj>().a(1).c(2.2f).b("asdf").build();
        std::cout << Json::pretty(myObj) << std::endl;

    }
}
