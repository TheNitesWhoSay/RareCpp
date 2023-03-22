#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <string>

inline namespace mapper_trivial_mapping
{
    struct MyDao
    {
        long pk;
        std::string name;
        std::string description;
        std::string lastModifiedBy;

        REFLECT(MyDao, pk, name, description, lastModifiedBy)
    };

    struct MyDto
    {
        std::string name;
        std::string description;

        REFLECT(MyDto, name, description)
    };

    void mapperTrivialMapping()
    {
        MyDao myDao { 19L, "some name", "some description", "admin" };
        MyDto myDto = RareMapper::map<MyDto>(myDao);
        std::cout << "Mapped from: " << Json::pretty(myDao) << std::endl;
        std::cout << "To: " << Json::pretty(myDto) << std::endl;
        std::cout << "Using matching reflected field names (since no more-specific mapping approach was provided)" << std::endl;
    }
}
