#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <exception>
#include <iostream>

inline namespace builder_validated
{
    class MyObj
    {
        int a;
        int b;

    public:
        REFLECT(MyObj, a, b)

        // Having a "bool validate()" method signals that private data should be buidlable unless RareTs::BuilderIgnore'd
        bool validate() // You may use either a boolean, or a void method with exceptions, to perform validation
        {
            return a >= 0 && b >= 0; // Returns boolean indicating whether the built structure was valid
        }
    };
    
    class MyOtherObj
    {
        int a;
        int b;

        NOTE(c, RareTs::BuilderIgnore)
        int c = 0;

    public:
        REFLECT(MyOtherObj, a, b, c)

        void validate() // Having a "void validate()" method signals that private data should be buidlable unless RareTs::BuilderIgnore'd
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

        //RareBuilder<MyOtherObj>().c(1).build(); // "c" was annotated with RareTs::BuilderIgnore so this is not a buildable member/is a compile error
    }
}
