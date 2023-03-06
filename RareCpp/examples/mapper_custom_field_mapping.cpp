#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <string>

inline namespace mapper_custom_field_mapping
{
    struct MyDao
    {
        long object_id;
        std::string object_name;
        std::string object_description;
        std::string last_modified_by;

        REFLECT(MyDao, object_id, object_name, object_description, last_modified_by)
    };

    struct MyDto
    {
        std::string name;
        std::string description;

        REFLECT(MyDto, name, description)

        NOTE(ObjectMappings, RareMapper::createMapping<MyDao, MyDto>()
            .object_name->name()
            .object_description->description()
            .bidirectional())
    };

    void mapperCustomFieldMapping()
    {
        MyDao myDao { 19L, "some name", "some description", "admin" };
        MyDto myDto = RareMapper::map<MyDto>(myDao);
        std::cout << "Mapped from: " << Json::pretty(myDao) << std::endl;
        std::cout << "To: " << Json::pretty(myDto) << std::endl;
        std::cout << "Using the bidirectional mapping" << std::endl;

        myDto.description = "updated description";
        RareMapper::map(myDao, myDto);
        std::cout << "Mapped from: " << Json::pretty(myDto) << std::endl;
        std::cout << "To: " << Json::pretty(myDao) << std::endl;
        std::cout << "Using the bidirectional mapping" << std::endl;
    }
}
