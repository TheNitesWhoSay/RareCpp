#include <rarecpp/reflect.h>
#include <iostream>

class MyObj
{
    int myInt = 3;
    char myChar = 'A';

public:
    REFLECT(MyObj, myInt, myChar)
};

void memberAccessByConstexprIndex()
{
    MyObj myObj{};
    using ZerothMember = RareTs::Member<MyObj, 0>;
    using FirstMember = RareTs::Member<MyObj, 1>;

    std::cout << "[" << ZerothMember::index << "] " << RareTs::toStr<typename ZerothMember::type>()
        << " " << ZerothMember::name << " = " << ZerothMember::value(myObj) << std::endl;

    std::cout << "[" << FirstMember::index << "] " << RareTs::toStr<typename FirstMember::type>()
        << " " << FirstMember::name << " = " << FirstMember::value(myObj) << std::endl;
}
