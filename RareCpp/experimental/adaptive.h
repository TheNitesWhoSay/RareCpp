// Experimental features!
#ifndef ADAPTIVE_H
#define ADAPTIVE_H
#include <rarecpp/reflect.h>
#include <functional>
#include <iostream>
#include <list>
#include <stdexcept>

namespace Adaptive
{
    inline namespace adaptive_instance_baseline
    {
        template <typename T, size_t I = std::numeric_limits<size_t>::max()>
        struct partition_member : RareTs::Class::adapt_member<partition_member<T>::template type, T, I> {};

        template <typename T>
        struct partition_member<T, std::numeric_limits<size_t>::max()> {
            template <size_t I> using type = RareTs::Class::member_type<T, I>;
        };

        template <typename T, size_t ... Is>
        struct partition_members : RareTs::Class::adapt_member<partition_member<T>::template type, T, Is> ... {};

        template <typename T, size_t ... Is>
        constexpr partition_members<T, Is...> cloneMembers(std::index_sequence<Is...>);

        template <typename T> using clone_members = decltype(cloneMembers<T>(std::make_index_sequence<RareTs::Class::member_count<T>>()));
    }

    inline namespace adaptive_type_baseline
    {
        template <typename T, size_t I = std::numeric_limits<size_t>::max()>
        struct partition_member_type : RareTs::Class::adapt_member_type<partition_member_type<T>::template type, T, I> {};

        template <typename T>
        struct partition_member_type<T, std::numeric_limits<size_t>::max()> {
            template <size_t I> using type = RareTs::Class::member_type<T, I>;
        };

        template <typename T, size_t ... Is>
        struct partition_member_types : RareTs::Class::adapt_member_type<partition_member_type<T>::template type, T, Is> ... {};

        template <typename T, size_t ... Is>
        constexpr partition_member_types<T, Is...> cloneMemberTypes(std::index_sequence<Is...>);

        template <typename T> using clone_member_types = decltype(cloneMemberTypes<T>(std::make_index_sequence<RareTs::Class::member_count<T>>()));
    }

    inline namespace adaptive_builder // A version of this is now available in RareTs
    {
        namespace detail {
            template <typename T, bool Validate, size_t ... Is> class sub_builder;

            template <typename T, bool Validate, typename Builder, size_t ... Is> struct member_builder {
                template <size_t I> class type {
                    using Member = typename RareTs::Reflect<T>::Members::template Member<I>;
                    template <size_t ... Js> constexpr auto remaining(std::index_sequence<Js...>) { return sub_builder<T, Validate, Js...>(t); }
                    T & t;

                public:
                    constexpr type(T & t) : t(t) {}
                    constexpr auto operator() (const typename Member::type & value) {
                        Member::value(t) = value;
                        return remaining(typename RareTs::remove_index<I, Is...>::type{});
                    }
                };
            };

            template <typename T, bool Validate, size_t ... Is>
            class sub_builder : public RareTs::Class::adapt_member<member_builder<T, Validate, sub_builder<T, Is...>, Is...>::template type, T, Is>...
            {
                T & t;
                template <typename U> using ValidateOp = decltype(static_cast<bool>(std::declval<U>().validate()));

            public:
                template <typename U> T validate(U & t)
                {
                    if constexpr ( std::is_same_v<bool, decltype(t.validate())> ) // Boolean validate method
                    {
                        if ( !t.validate() ) // Validate should return false if the build was not accepted
                            throw std::logic_error("Builder validation failed!");
                    }
                    else // Non-boolean validate method
                        t.validate(); // Validate should throw an exception if the build was not accepted
                }
                constexpr T build()
                {
                    if constexpr ( Validate && RareTs::op_exists_v<ValidateOp, T> ) // t.validate() exists
                        validate();
            
                    return t;
                }
                constexpr sub_builder(T & t)
                    : RareTs::Class::adapt_member<member_builder<T, Validate, sub_builder<T, Is...>, Is...>::template type, T, Is> {{t}}..., t(t) {}
            };

            template <typename T, bool Validate, size_t ... Is>
            class head_builder : public sub_builder<T, Validate, Is...>
            {
                T t{};

            public:
                constexpr head_builder() : sub_builder<T, Validate, Is...>(t), t({}) {}
            };

            template <typename T, bool Validate, size_t ... Is> constexpr auto builder(std::index_sequence<Is...>) { return detail::head_builder<T, Validate, Is...>{}; }
        }
    
        template <typename T, bool Validate = false> constexpr auto builder() {
            return detail::builder<T, Validate>(std::make_index_sequence<RareTs::Class::template member_count<T>>());
        }
    }
    
    inline namespace adaptive_reference_object
    {
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

        template <typename T, size_t ... Is> constexpr auto reference_object(T & t, std::index_sequence<Is...>) {
            return reference_members_adapter<T, Is...>(t);
        }

        template <typename T> constexpr auto reference_object(T & t) {
            return reference_object<T>(t, std::make_index_sequence<RareTs::Class::template member_count<T>>());
        }
    }

    inline namespace adaptive_change_listener
    {
        template <typename T> struct member_change_listener {
            template <size_t I> class type {
                using Member = typename RareTs::Reflect<T>::Members::template Member<I>;
                using MemberType = typename Member::type;
                using Listener = std::function<void(const T &, const Member &, const MemberType & oldValue, const MemberType & newValue)>;
                static constexpr auto member = Member{};

                T & t;
                std::list<Listener> listeners {};

            public:
                class UniqueListener {
                    std::list<Listener> & in;
                    typename std::list<Listener>::iterator it;
                public:
                    UniqueListener(std::list<Listener> & in, typename std::list<Listener>::iterator it) : in(in), it(it) {}
                    UniqueListener(UniqueListener && other) noexcept : in (other.in), it(std::move(other.it)) { other.it = in.end(); }
                    ~UniqueListener() { if ( it != in.end() ) in.erase(it); }
                };

                auto & operator=(MemberType newValue) {
                    auto & value = Member::value(t);
                    std::swap(value, newValue);
                    for ( const auto & listener : listeners )
                        listener(t, member, newValue, value);

                    return *this;
                }

                auto make_unique_listener(Listener && listener) {
                    listeners.push_front(listener);
                    return UniqueListener(listeners, listeners.begin());
                }

                type(T & t, typename member_change_listener<T>::template type<I>* & self) : t(t) { self = this; }
            };
        };

        template <typename T, size_t ... Is>
        class change_listener_adapter : public RareTs::Class::adapt_member<member_change_listener<T>::template type, T, Is>...
        {
            std::tuple<typename member_change_listener<T>::template type<Is>*...> members;
            template <size_t I> using inc = RareTs::Class::adapt_member<member_change_listener<T>::template type, T, I>;
            template <size_t I> using incT = typename member_change_listener<T>::template type<I>;

        public:
            template <typename F> inline auto make_unique_listener(F && f) {
                return std::tuple { std::move(std::get<Is>(members)->make_unique_listener(std::forward<F>(f)))... };
            }

            change_listener_adapter(T & t, decltype(members) f = {})
                : RareTs::Class::adapt_member<member_change_listener<T>::template type, T, Is>{{t, std::get<Is>(f)}}... { members.swap(f); }
        };

        template <typename T, size_t ... Is> inline auto change_listener(T & t, std::index_sequence<Is...>) {
            return change_listener_adapter<T, Is...>(t);
        }

        template <typename T> inline auto change_listener(T & t) {
            return change_listener(t, std::make_index_sequence<RareTs::Class::member_count<T>>());
        }
    }

    inline namespace adaptive_object_mapping // A version of this is now in use in RareMapper
    {
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

                template <size_t ... Is> static constexpr auto rightAgg(std::index_sequence<Is...>) -> decltype(rightAggregate( // filter out unmappable members
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
        constexpr auto mapping(std::index_sequence<Is...>) { return typename left_aggregate<L, R>::template type<true, Is...>{}; }

        template <typename L, typename R>
        constexpr auto mapping() { return mapping<L, R>(std::make_index_sequence<RareTs::Class::member_count<L>>()); }
    }

    inline namespace adaptive_member_selector
    {
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

        template <typename T> using all_members = decltype(aggregateMembers<T>(std::make_index_sequence<RareTs::Class::member_count<T>>()));
    }
}

struct L
{
    int l1;
    int l2;
    int l3;
    
    REFLECT(L, l1, l2, l3)
};

struct R
{
    int r1;
    int r2;
    
    REFLECT(R, r1, r2)

    NOTE(ObjectMappings, RareMapper::createMapping<L, R>()
        .l1->r1()
        .l3->r2()
        .bidirectional()
    )
};

struct MyObj
{
private:
    int myInt = 0;
    std::string myString{};

    REFLECT(MyObj, myInt, myString)
};

inline std::ostream & operator<<(std::ostream & os, const MyObj & myObj) {
    auto get = Adaptive::reference_object(myObj);
    os << "MyObj: {" << get.myInt << ", " << get.myString << "}" << std::endl << std::endl;
    return os;
}

inline void adaptiveStructExperiment()
{
    L l {};
    R r {1, 2};
    RareMapper::map(l, r);
    std::cout << "{" << r.r1 << ", " << r.r2 << "} --> {" << l.l1 << ", " << l.l2 << ", " << l.l3 << "}" << std::endl;

    MyObj myObj = Adaptive::builder<MyObj>().myInt(42).myString("l33t").build(); // adapted types overload operator()
    std::cout << myObj;
    
    auto ref = Adaptive::reference_object(myObj); // adapted types are references to the members
    ref.myInt = 444;
    ref.myString = "asdf";

    auto myTappedObj = Adaptive::change_listener(myObj); // adapted types are given operator= and listen(...)
    auto listener = myTappedObj.make_unique_listener([&](auto & t, auto & member, auto & oldValue, auto & newValue) {
        std::cout << "I heard you changed " << RareTs::toStr<RareTs::remove_cvref_t<decltype(t)>>() << "." << member.name
            << " from " << oldValue << " to " << newValue << std::endl;
    });
    myTappedObj.myString = "poiuy";
    myTappedObj.myInt = 1;
    myTappedObj.myString = "qwerty";
    std::cout << myObj;
}

#endif