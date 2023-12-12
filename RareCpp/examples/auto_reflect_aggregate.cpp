#include <rarecpp/reflect.h>
#include <rarecpp/json.h>
#include <iostream>
#include <string>

inline namespace auto_reflect_aggregate
{
    struct SubObj
    {
        int number = 0;
        std::string str {"asdf"};
    };

    struct MyObj
    {
        SubObj subObj {};
        int ray[5] {5, 6, 7, 8, 9};
        std::string str {"qwerty"};
    };

    // As of C++20, member names (in addition to member types & values) can be reflected on all major compilers without any form of registration
    // This cannot reflect classes using inheritance, private/protected members, virtualization, more than 121 members, or large C-arrays
    // In addition, while not preventing auto-reflection of aggregates, static data members and functions are not auto-reflected

    void autoReflectAggregate()
    {
#ifdef RARE_NO_CPP_20
        std::cout << "C++20 and up required to reflect aggregates" << std::endl;
#else
        std::cout << Json::pretty(MyObj{}) << std::endl;
#endif
    }
}
