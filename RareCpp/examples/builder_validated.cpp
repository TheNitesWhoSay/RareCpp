#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <exception>
#include <iostream>

inline namespace builder_validated
{
    NOTE(MyObj, RareTs::ValidatedBuilder)
    class MyObj
    {
        int a;
        int b;

    public:
        REFLECT_NOTED(MyObj, a, b)

        bool validate() // You may use either a boolean or a void method with exceptions to perform validation
        {
            return a >= 0 && b >= 0; // Returns boolean indicating whether the built structure was valid
        }
    };
    
    NOTE(MyOtherObj, RareTs::ValidatedBuilder)
    class MyOtherObj
    {
        int a;
        int b;

    public:
        REFLECT_NOTED(MyOtherObj, a, b)

        void validate()
        {
            if ( a < 0 || b < 0 ) // Throws an exception indicating what was invalid
                throw std::logic_error("a and b must both be non-negative!");
        }
    };

    void builderValidated()
    {
        try {
            RareBuilder<MyObj>().a(-1).b(1).build();
        } catch ( std::exception & e ) {
            std::cout << "boolean validator failed with message: " << e.what() << std::endl;
        }
        try {
            RareBuilder<MyOtherObj>().a(6).b(-6).build();
        } catch ( std::exception & e ) {
            std::cout << "void validator failed with message: " << e.what() << std::endl;
        }

        auto myObj = RareBuilder<MyObj>().a(1).b(2).build();
        std::cout << "Built MyObj: " << Json::pretty(myObj) << std::endl;

        auto myOtherObj = RareBuilder<MyOtherObj>().a(3).b(4).build();
        std::cout << "Built MyOtherObj: " << Json::pretty(myOtherObj) << std::endl;
    }
}
