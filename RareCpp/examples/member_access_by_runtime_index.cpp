#include <rarecpp/reflect.h>
#include <exception>
#include <iostream>
#include <stdexcept>

class MyObj
{
    int myInt = 3;
    char myChar = 'A';

public:
    REFLECT(MyObj, myInt, myChar)
};

void memberAccessByRuntimeIndex()
{
    std::cout << "Enter the index of the member you wish to get: ";
    size_t memberIndex = 0;
    std::cin >> memberIndex;
    
    if ( !std::cin.good() || memberIndex > RareTs::Members<MyObj>::total ) {
        throw std::logic_error("Invalid index!");
    }

    MyObj myObj{};
    RareTs::Members<MyObj>::at(memberIndex, [&](auto & member) { // Not supplying an instance of MyObj
        std::cout << "[" << member.index << "] " << member.name << " = " << member.value(myObj) << std::endl;
    });
    RareTs::Members<MyObj>::at(memberIndex, myObj, [&](auto & member, auto & value) { // Supplying an instance of MyObj
        std::cout << "[" << member.index << "] " << member.name << " = " << value << std::endl;
    });
}
