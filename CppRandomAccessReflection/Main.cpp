#include <iostream>
#include "Json.h"

class FuelTank {
public:
    FuelTank() {}
    FuelTank(float capacity, float currentLevel, float tickMarkOne, float tickMarkTwo) : capacity(capacity), currentLevel(currentLevel)
    {
        tickMarks[0] = tickMarkOne;
        tickMarks[1] = tickMarkTwo;
    }

    float capacity;
    float currentLevel;
    float tickMarks[2];

    REFLECT(FuelTank, B(float, capacity), B(float, currentLevel), B(float[2], tickMarks))
};

class Wheel {
public:
    enum Rim : unsigned int {
        Regular = 0,
        Spinner = 1
    };

    Wheel() {}
    Wheel(Rim rim, int size, float pressure) : rim(rim), size(size), pressure(pressure) {}

    Rim rim;
    int size;
    float pressure;

    REFLECT(Wheel, B(Rim, rim), B(int, size), B(float, pressure))
};

class Car {
public:
    Car() {}
    Car(Wheel frontLeft, Wheel frontRight, Wheel backLeft, Wheel backRight, const std::string &driver, const std::string &passenger, const FuelTank &fuelTank, float milesPerGallon)
        : fuelTank(fuelTank), milesPerGallon(milesPerGallon)
    {
        wheels[0] = frontLeft;
        wheels[1] = frontRight;
        wheels[2] = backLeft;
        wheels[3] = backRight;
        occupants[0] = driver;
        occupants[1] = passenger;
    }

    Wheel wheels[4];
    std::string occupants[2];
    FuelTank fuelTank;
    float milesPerGallon;
    
    REFLECT(Car, R(Wheel[4], wheels), B(std::string[2], occupants), R(FuelTank, fuelTank), B(float, milesPerGallon))
};

int main()
{
    std::string driver = "Fred";
    std::string passenger = "Bob";
    FuelTank fuelTank(15.0f, 14.6f, 1.0f, 7.5f);
    Wheel frontLeft(Wheel::Rim::Regular, 14, 32.2f), frontRight(Wheel::Rim::Regular, 14, 31.9f), backLeft(Wheel::Rim::Spinner, 15, 33.0f), backRight(Wheel::Rim::Spinner, 15, 30.9f);
    Car car(frontLeft, frontRight, backLeft, backRight, driver, passenger, fuelTank, 22.5f);
    
    for ( size_t i=0; i<Wheel::Class::totalFields; i++ )
        std::cout << Wheel::Class::fields[i].fieldName << std::endl;

    for ( size_t i=0; i<Wheel::Class::totalFields; i++ )
    {
        Wheel::Class::FieldAt(frontLeft, i, [&](auto field, auto value) {
            std::cout << field.fieldName << ": " << value << std::endl;
        });
    }

    Wheel::Class::ForEachField(backRight, [&](auto field, auto value) {
        std::cout << field.fieldName << ": " << value << std::endl;
    });

    Car::Class::ForEachField(car, [&](auto field, auto value) {
        //std::cout << field.fieldName << ": " << value << std::endl; // This will cause a compiler error as there's no ostream operator overload for the FuelTank type!
    });

    Car::Class::ForEachField(car, [&](auto field, auto value) {
        field.IfPrimitive([&](auto) { std::cout << "(carPrimitive) " << field.fieldName << ": " << value << std::endl; });
        field.IfObject([&](auto) { std::cout << "(carObject) " << field.fieldName << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl; });
        field.IfObjectArray([&](auto) { std::cout << "(carObjectArray) " << field.fieldName << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl; });
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        std::cout << field.fieldName << ": " << value << std::endl; // This will print out a pointer, not the array values!
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        if ( field.isArray ) {
            //std::cout << field.fieldName << ": " << value[0] << std::endl; // This will cause a compiler error as not every field in the FuelTank class is an array!
        }
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        field.IfPrimitive([&](auto) { std::cout << "(fuelTankPrimitive) " << field.fieldName << ": " << value << std::endl; });
        field.IfPrimitiveArray([&](auto) { std::cout << "(fuelTankPrimitiveArray) " << field.fieldName << ": " << value << std::endl; });
    });

    std::cout << Json::out(car) << std::endl;

    std::cout << std::endl << "Press any key to exit." << std::endl;
    std::cin.clear();
    std::cin.get();

    return 0;
}
