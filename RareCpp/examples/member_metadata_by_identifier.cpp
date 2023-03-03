#include <rarecpp/reflect.h>
#include <iostream>

class MyObj
{
    int myInt = 3;
    long myLong = 4;
    char myChar = 'A';

public:
    REFLECT(MyObj, myInt, myChar)
};

void memberMetadataByIdentifier()
{
    std::cout << "myInt was the " << RareTs::IndexOf<MyObj>::myInt << "th member reflected" << std::endl;
    std::cout << "myChar was the " << RareTs::IndexOf<MyObj>::myChar << "st member reflected" << std::endl;
    
    using MyIntMember = typename RareTs::MemberType<MyObj>::myInt;
    using MyCharMember = typename RareTs::MemberType<MyObj>::myChar;

    MyObj myObj{};
    std::cout << MyIntMember::name << " had a default value of " << MyIntMember::value(myObj) << std::endl;
    std::cout << MyCharMember::name << " had a default value of " << MyCharMember::value(myObj) << std::endl;
}
