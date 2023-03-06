#include "car.h"
#include <rarecpp/reflect.h>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
using RareTs::Reflect;

void basicReflectionExamples()
{
    std::vector<std::shared_ptr<CupHolder>> cupHolders;
    std::map<std::string, std::string> occupantId;
    std::map<std::string, std::shared_ptr<CupHolder>> occupantCupHolderUsage;
    Wheel frontLeft(Wheel::Rim::Regular, 14, 32.2f), frontRight(Wheel::Rim::Regular, 14, 31.9f), backLeft(Wheel::Rim::Spinner, 15, 33.0f), backRight(Wheel::Rim::Spinner, 15, 30.9f);
    std::string driver = "Fred";
    std::string passenger = "Bob";
    std::vector<std::vector<std::vector<std::string>>> testNest = {
        {{ "a", "b", "c" }, { "d", "e" }},
        {{ "f", "g", "h", "i" }, {"j"}}
    };
    std::map<std::string, std::vector<std::vector<std::string>>> testMapNest = {
        { "one", {{ "a", "b", "c" }, { "d", "e" }} },
        { "two", {{ "f", "g" }, { "h" }}}
    };
    occupantId.insert(std::pair<std::string, std::string>(driver, "B252-123-839-244"));
    occupantId.insert(std::pair<std::string, std::string>(passenger, "B252-612-321-245"));
    cupHolders.push_back(std::make_shared<CupHolder>(3, 3, true));
    cupHolders.push_back(std::make_shared<CupHolder>(2, 2, false));
    occupantCupHolderUsage.insert(std::pair<std::string, std::shared_ptr<CupHolder>>(driver, cupHolders[0]));
    occupantCupHolderUsage.insert(std::pair<std::string, std::shared_ptr<CupHolder>>(passenger, cupHolders[1]));
    FuelTank fuelTank(15.0f, 14.6f, 1.0f, 7.5f);
    Car car(frontLeft, frontRight, backLeft, backRight, driver, passenger, testNest, testMapNest, occupantId, occupantCupHolderUsage, cupHolders, fuelTank, 22.5f);
    
    /*for ( size_t i=0; i<Reflect<Wheel>::Members::total; i++ )
        std::cout << Wheel::Class::Members[i].name << std::endl;*/

    for ( size_t i=0; i<Reflect<Wheel>::Members::total; i++ )
    {
        Reflect<Wheel>::Members::at(i, frontLeft, [&](auto & member, auto & value) {
            std::cout << member.name << ": " << value << std::endl;
        });
    }

    Reflect<Wheel>::Members::forEach(backRight, [&](auto & member, auto & value) {
        std::cout << member.name << ": " << value << std::endl;
    });

    Reflect<Car>::Members::forEach(car, [&](auto & /*member*/, auto & /*value*/) {
        //std::cout << member.name << ": " << value << std::endl; // This will cause a compiler error as there's no ostream operator overload for the FuelTank type!
    });

    Reflect<Car>::Members::forEach(car, [&](auto & member, auto & value) {
        using Type = std::remove_reference_t<decltype(value)>;
        if constexpr ( !RareTs::is_iterable_v<Type> && !RareTs::is_reflected_v<Type> )
            std::cout << "(carPrimitive) " << member.name << ": " << value << std::endl;
        else if constexpr ( !RareTs::is_iterable_v<Type> && RareTs::is_reflected_v<Type> )
            std::cout << "(carObject) " << member.name << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl;
        else if constexpr ( RareTs::is_iterable_v<Type> && RareTs::is_reflected_v<RareTs::element_type_t<Type>> )
            std::cout << "(carObjectIterable) " << member.name << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl;
    });

    Reflect<FuelTank>::Members::forEach(fuelTank, [&](auto & member, auto & value) {
        std::cout << member.name << ": " << value << std::endl; // This will print out a pointer, not the array values!
    });

    Reflect<FuelTank>::Members::forEach(fuelTank, [&](auto & member, auto & value) {
        using Type = std::remove_reference_t<decltype(value)>;
        if constexpr ( RareTs::is_static_array_v<Type> )
            std::cout << member.name << ": " << value[0] << std::endl;
        else
            std::cout << member.name << ": " << value << std::endl;
    });

    Reflect<FuelTank>::Members::forEach(fuelTank, [&](auto & member, auto & value) {
        using Type = std::remove_reference_t<decltype(value)>;
        if constexpr ( !RareTs::is_iterable_v<Type> && !RareTs::is_reflected_v<Type> )
            std::cout << "(fuelTankPrimitive) " << member.name << ": " << value << std::endl;
        else if constexpr ( RareTs::is_static_array_v<Type> && !RareTs::is_reflected_v<RareTs::element_type_t<Type>> )
            std::cout << "(fuelTankPrimitiveArray) " << member.name << ": " << value << std::endl;
    });
}
