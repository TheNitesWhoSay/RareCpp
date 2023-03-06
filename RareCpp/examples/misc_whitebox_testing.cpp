#include <rarecpp/reflect.h>
#include <iostream>

inline namespace misc_whitebox_testing
{
    class Blackbox
    {
    private:
        int a = 3;
        int b = 4;
        int c = 5;

    public:
        REFLECT(Blackbox, a, b, c)

        int checksum() { return a+b+c; }
    };

    void miscWhiteboxTesting()
    {
        Blackbox blackbox {};
        std::cout << blackbox.checksum() << std::endl;

        auto whitebox = RareTs::whitebox(blackbox); // whitebox is a object with public members that are reference to the given objects members
        std::cout << "a: " << whitebox.a << std::endl;
        std::cout << "b: " << whitebox.b << std::endl;
        std::cout << "c: " << whitebox.c << std::endl;

        // You can use it to edit the members of blackbox
        whitebox.a++;
        whitebox.b = 5;
        whitebox.c++;

        std::cout << blackbox.checksum() << std::endl;
        // Highly recommeneded that you only use this for testing purposes, working around access protection is generally a bad idea
        // General caution is recommended when any reflection that you're not accidentally working around protection and invalidating object invariants
    }
}
