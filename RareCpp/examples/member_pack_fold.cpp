#include <rarecpp/reflect.h>
#include <iostream>

class MyObj
{
    bool a = true;
    bool b = true;
    bool c = false;

public:
    REFLECT(MyObj, a, b, c)
};

void memberPackFold()
{
    MyObj myObj{};
    RareTs::Values<MyObj>::pack(myObj, [&](auto & ... value) {
        bool andResult = true && (... && value);
        bool orResult = (... || value);
        (std::cout << ... << (value ? " true &&" : " false &&")) << " = " << (andResult ? "true" : "false") << std::endl;
        (std::cout << ... << (value ? " true ||" : " false ||")) << " = " << (orResult ? "true" : "false") << std::endl;
    });
}
