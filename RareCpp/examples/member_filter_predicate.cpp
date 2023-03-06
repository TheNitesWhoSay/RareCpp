#include <rarecpp/reflect.h>
#include <iostream>

inline namespace member_filter_predicate
{
    class MyFilterableObj
    {
        int myInt = 0;
        float myArray[3];
        inline static int myStaticArray[3] { 2, 1, 0 };
        int & myRef;

    public:
        MyFilterableObj(int & referrable) : myInt(0), myArray{2.2f, 1.1f, 0.0f}, myRef(referrable) {}

        REFLECT(MyFilterableObj, myInt, myArray, myRef)
    };

    // This is a type-predicate, when you use type-predicates they only operate on Member::type, they can't access other details of Member
    template <typename T> struct is_integral_or_integral_ref { static constexpr bool value = std::is_integral_v<RareTs::remove_cvref_t<T>>; };

    // This is a member-predicate (the enable_if_member_t part is mandatory for member predicates), they can operator on the full details of Member
    template <typename Member, typename = RareTs::enable_if_member_t<Member>>
    struct IsNonStaticArray : std::bool_constant<!Member::isStatic && std::is_array_v<typename Member::type>> {};

    void memberFilterPredicate()
    {
        int referrable = 1;
        MyFilterableObj myObj(referrable);

        RareTs::Members<MyFilterableObj>::forEach<std::is_integral>(myObj, [&](auto member, auto & value) { // Only matches int, will not match array or reference
            std::cout << "is_integral: " << member.name << " = " << value << std::endl;
        });

        RareTs::Members<MyFilterableObj>::forEach<is_integral_or_integral_ref>(myObj, [&](auto member, auto & value) { // Matches both ints
            std::cout << "is_integral_or_integral_ref: " << member.name << " = " << value << std::endl;
        });

        RareTs::Members<MyFilterableObj>::forEach<IsNonStaticArray>(myObj, [&](auto member, auto & value) { // Matches the non-static array only
            std::cout << "IsNonStaticArray: " << member.name << " = " << value[0] << ", " << value[1] << ", " << value[2] << std::endl;
        });

        // If you access members using runtime information/index and perform an operation only valid on one of the members (e.g. array access), you must filter!
        RareTs::Values<MyFilterableObj>::at(1, [&](auto & /*value*/) {
            //std::cout << value[0] << std::endl; // This is a compile-time error since code for all members is generated
        });
    
        RareTs::Values<MyFilterableObj>::at<IsNonStaticArray>(1, myObj, [&](auto & value) {
            std::cout << "predicate filtered: " << value[0] << std::endl; // This is fine since you've filtered to your particular types using IsNonStaticArray
        });
        RareTs::Members<MyFilterableObj>::at(1, myObj, [&](auto member, auto & value) { // This is more work, but ultimately a little more flexible at times
            using Member = decltype(member);
            if constexpr ( std::is_array_v<typename Member::type> )
                std::cout << "if constexpr filtered: " << value[0] << std::endl; // This is fine since you've filtered to your particular types using if constexpr
        });
    }
}
