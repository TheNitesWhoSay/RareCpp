#include <rarecpp/reflect.h>
#include <iostream>
#include <tuple>
#include <string>
#include <sstream>

inline namespace function_build_and_call
{
    struct MyController
    {
        std::string foo(int a, int b, float c) {
            return std::string("I got ") + std::to_string(a) + "," + std::to_string(b) + "," + std::to_string(c);
        }

        int bar(int a, int b) {
            return a+b;
        }

        REFLECT(MyController, foo, bar)
    };

    template <typename T>
    void invokeControllerMethod(T & controller, std::string_view name, std::string_view input)
    {
        std::istringstream ss(std::string(input).c_str());
        auto argBuilder = [&](auto argInfo) {
            using type = typename decltype(argInfo)::type;
            //constexpr size_t index = decltype(argInfo)::index;
            type arg {};
            ss >> arg;
            return arg;
        };

        RareTs::Members<T>::named(name, [&](auto member) {
            auto func = RareTs::Function(member.pointer);
            auto result = func.invoke(controller, argBuilder);
            std::cout << "result of call to " << name << ": " << result << std::endl;
        });
    }

    void functionBuildAndCall()
    {
        MyController myController {};
        invokeControllerMethod(myController, "foo", "0 1 3.4");
        invokeControllerMethod(myController, "bar", "2 3");
    }
}
