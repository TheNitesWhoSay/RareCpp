#include <rarecpp/reflect.h>
#include <iostream>

inline namespace member_filter_constexpr_if
{
    class MyObj
    {
        int myInt = 0;
        float myArray[3];
        int & myRef;

    public:
        MyObj(int & referrable) : myInt(0), myArray{0.0f, 1.1f, 2.2f}, myRef(referrable) {}

        REFLECT(MyObj, myInt, myArray, myRef)
    };

    void memberFilterConstexprIf()
    {
        int referrable = 1;
        MyObj myObj(referrable);

        RareTs::Members<MyObj>::forEach(myObj, [&](auto member, auto & value) { // Member has no non-static data members, so it's a bit better to pass it by value
            using Member = decltype(member); // If you passed member by reference, you'd need:  using Member = std::remove_reference_t<decltype(member)>;

            using MemberType = typename Member::type; // Good, this is the type something was declared as in the structure
            using ValueType = decltype(value); // Uusually bad, this decays, this aquires the reference/constness (or lack-thereof) of the lambda "value" param
            std::cout << "[" << member.index << "]" << " had a declared type of " << RareTs::toStr<MemberType>() << std::endl
                << "    and was passed to the function as: " << RareTs::toStr<ValueType>() << std::endl;

            if constexpr ( std::is_array_v<MemberType> )
            {
                auto length = std::extent_v<MemberType>;
                std::cout << "    value: ";
                for ( size_t i=0; i<length; ++i )
                    std::cout << value[i] << ", ";
                std::cout << std::endl;
            }
            else
            {
                std::cout << "    value: " << value << std::endl;
            }
        });
    }
}
