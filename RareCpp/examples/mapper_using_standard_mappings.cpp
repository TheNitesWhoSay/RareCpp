#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <string>

inline namespace mapper_using_standard_mappings
{
    struct MyConstructible
    {
        std::string description;
    };

    struct MyAssignable
    {
        std::string description;
    };

    struct MyDto
    {
        std::string description;

        MyDto() {}
        MyDto(const MyConstructible &) : description("from constructor") {} // RareMapper will use converting constructors if present
        
        MyDto & operator=(const MyAssignable &) { // RareMapper will use assignment operators if present
            this->description = "from assignment";
            return *this;
        }
        
        operator MyAssignable() const { return MyAssignable{"from conversion"}; } // RareMapper will use conversion operators if present
    };
    // Highly recommend using such C++ standard mapping methods to define any non-trivial mappings even if you use RareMapper::map to map between types

    void mapperUsingStandardMappings()
    {
        MyDto constructed = RareMapper::map<MyDto>(MyConstructible{});
        std::cout << constructed.description << std::endl;
        MyDto assigned = RareMapper::map<MyDto>(MyAssignable{});
        std::cout << assigned.description << std::endl;
        
        MyAssignable assignable = RareMapper::map<MyAssignable>(assigned);
        std::cout << assignable.description << std::endl;
    }
}
