#include <rarecpp/reflect.h>
#include <iostream>

class MyObj
{
    int myInt = 3;
    char myChar = 'A';

public:
    REFLECT(MyObj, myInt, myChar)
};

void memberDefineIterateModify()
{
    MyObj myObj{};

    RareTs::Members<MyObj>::forEach(myObj, [&](auto & member, auto & value) { // Supplying an instance of MyObj, incrementing and printing the values
        std::cout << member.name << " = " << ++value << std::endl;
    });

    RareTs::Members<MyObj>::forEach([&](auto & member) { // Same thing, but providing the instance of MyObj to member to get the value
        std::cout << member.name << " = " << ++member.value(myObj) << std::endl;
    });

    RareTs::Values<MyObj>::forEach(myObj, [&](auto & value) { // Similar, but only accessing the value of the member/no metadata like member name is available
        std::cout << ++value << std::endl;
    });

    const MyObj & constRef = myObj;
    RareTs::Values<MyObj>::forEach(constRef, [&](auto & value) { // Note: const correct, value is const if the member declaration or object instance is const
        static_assert(std::is_const_v<std::remove_reference_t<decltype(value)>>, "const check");
    });
}
