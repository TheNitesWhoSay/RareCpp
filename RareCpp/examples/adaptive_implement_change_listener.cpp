#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <functional>
#include <iostream>
#include <list>

inline namespace adaptive_implement_change_listener
{
    inline namespace AdaptiveChangeListener
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
        class ChangeListenerAdapter : public RareTs::Class::adapt_member<member_change_listener<T>::template type, T, Is>...
        {
            std::tuple<typename member_change_listener<T>::template type<Is>*...> members;

        public:
            template <typename F> inline auto make_unique_listener(F && f) {
                return std::tuple { std::move(std::get<Is>(members)->make_unique_listener(std::forward<F>(f)))... };
            }

            ChangeListenerAdapter(T & t, decltype(members) f = {})
                : RareTs::Class::adapt_member<member_change_listener<T>::template type, T, Is>{{t, std::get<Is>(f)}}... { members.swap(f); }
        };

        template <typename T, size_t ... Is> inline auto changeListener(T & t, std::index_sequence<Is...>) {
            return ChangeListenerAdapter<T, Is...>(t);
        }

        template <typename T> inline auto changeListener(T & t) {
            return changeListener(t, std::make_index_sequence<RareTs::Class::member_count<T>>());
        }
    }

    struct MyObj
    {
        int myInt = 0;
        std::string myString = "init";

        REFLECT(MyObj, myInt, myString)
    };

    void adaptiveImplementChangeListener()
    {
        MyObj myObj{};
        std::cout << Json::pretty(myObj) << std::endl << std::endl;

        auto myTappedObj = changeListener(myObj); // adapted types are given operator= and listen(...)
        auto listener = myTappedObj.make_unique_listener([&](auto & t, auto & member, auto & oldValue, auto & newValue) {
            std::cout << "I heard you changed " << RareTs::toStr<RareTs::remove_cvref_t<decltype(t)>>() << "." << member.name
                << " from " << oldValue << " to " << newValue << std::endl;
        });

        myTappedObj.myString = "poiuy";
        myTappedObj.myInt = 1;
        myTappedObj.myString = "qwerty";
        
        std::cout << std::endl << Json::pretty(myObj) << std::endl;
    }
}
