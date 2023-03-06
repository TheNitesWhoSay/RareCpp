#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>

inline namespace adaptive_implement_mapper
{
    template <size_t L, size_t R> struct IndexPair {
        static constexpr size_t Left = L;
        static constexpr size_t Right = R;
    };

    template <typename Left, typename Right, typename ... Ts> struct member_mapping : RareTs::type_id<std::tuple<Ts...>> {};
        
    template <typename L, typename R, typename ... Ts /* IndexPairs for previous mappings */> struct left_aggregate;

    template <typename L, typename R, typename T /* integral_constant<size_t> for L-member */, typename ... Ts /* IndexPairs for previous mappings */>
    struct right_item
    {
        template <size_t I> struct type
        {
            static constexpr size_t rightCount = RareTs::Class::member_count<R>;
            template <typename> struct filter : std::bool_constant<sizeof...(Ts)+1 < rightCount> {}; // all valid unless all R-members have been mapped

            template <size_t ... Is> static constexpr auto leftAggregate(std::index_sequence<Is...>) -> // returns type of left-aggregate mapping object
                typename left_aggregate<L, R, Ts..., IndexPair<T::value, I>>::template type<Is...>;

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
        template <size_t ... Is> struct type : RareTs::Class::adapt_member<left_item<L, R, Ts...>::template type, L, Is>... {
            static constexpr auto build() { return member_mapping<L, R, Ts...>{}; }
        };
    };

    template <typename L, typename R, size_t ... Is>
    constexpr auto createMapping(std::index_sequence<Is...>) { return typename left_aggregate<L, R>::template type<Is...>{}; }
    
    template <typename L, typename R> constexpr auto createMapping() {
        return createMapping<L, R>(
            std::make_index_sequence<RareTs::Class::member_count<L>>());
    }

    struct From {
        int one = 1;
        int five = 5;
        int nine = 9;

        REFLECT(From, one, five, nine)
    };

    struct To {
        int zerothTo = 0;
        int firstTo = 0;

        REFLECT(To, zerothTo, firstTo)
    };

    void adaptiveImplementMapper()
    {
        constexpr auto mapping = createMapping<From, To>()
            .one->zerothTo()
            .nine->firstTo()
            .build();

        From from {};
        To to {};

        using Mapping = typename decltype(mapping)::type;
        constexpr size_t totalFieldsMapped = std::tuple_size_v<Mapping>;
        std::cout << RareTs::toStr<Mapping>() << std::endl << std::endl;

        std::cout << "From: " << Json::pretty(from) << std::endl << std::endl;
        RareTs::forIndexes<totalFieldsMapped>([&](auto i) {
            constexpr size_t index = decltype(i)::value;
            using MemberMapping = std::tuple_element_t<index, Mapping>;
            constexpr size_t leftMemberIndex = MemberMapping::Left;
            constexpr size_t rightMemberIndex = MemberMapping::Right;
            
            using LeftMember = RareTs::Member<From, leftMemberIndex>;
            using RightMember = RareTs::Member<To, rightMemberIndex>;

            std::cout << "to." << LeftMember::name << " = from." << RightMember::name << std::endl;
            RightMember::value(to) = LeftMember::value(from);
        });
        std::cout << std::endl << "To: " << Json::pretty(to) << std::endl;
    }
}
