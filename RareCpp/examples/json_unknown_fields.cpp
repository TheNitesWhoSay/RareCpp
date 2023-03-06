#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>

inline namespace json_unknown_fields
{
    struct MyObj {
        int myInt = 0;
        std::string myString;
        Json::FieldCluster fieldCluster;

        REFLECT(MyObj, myInt, myString, fieldCluster)
    };
    
    void jsonUnknownFields()
    {
        MyObj myObj{};
        std::cout << Json::out(myObj) << std::endl;
        std::cout << "Enter a new version of this object including additional fields: " << std::endl;
        std::cin >> Json::in(myObj);

        std::cout << Json::pretty(myObj) << std::endl;
        if ( myObj.fieldCluster.object().size() > 0 )
            std::cout << "I like your new field: " << myObj.fieldCluster.object().begin()->first << std::endl;

        Json::Generic::Object anyObj{};
        std::cout << std::endl << "Enter any object: ";
        std::cin >> Json::in(anyObj);
        std::cout << Json::pretty(anyObj);
    }
}
