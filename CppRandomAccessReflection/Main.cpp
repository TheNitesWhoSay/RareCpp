#include <iostream>
#include "Json.h"

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

    Wheel wheels[4];
    std::string occupants[2];
    float milesPerGallon;

    Car() {}
    Car(Wheel frontLeft, Wheel frontRight, Wheel backLeft, Wheel backRight, std::string driver, std::string passenger, float milesPerGallon) : milesPerGallon(milesPerGallon) {
        wheels[0] = frontLeft;
        wheels[1] = frontRight;
        wheels[2] = backLeft;
        wheels[3] = backRight;
        occupants[0] = driver;
        occupants[1] = passenger;
    }

    REFLECT(Car, R(Wheel[4], wheels), B(std::string[2], occupants), B(float, milesPerGallon))
};

int main()
{
    std::string driver = "Fred";
    std::string passenger = "Bob";
    Wheel frontLeft(Wheel::Rim::Regular, 14, 32.2f), frontRight(Wheel::Rim::Regular, 14, 31.9f), backLeft(Wheel::Rim::Spinner, 15, 33.0f), backRight(Wheel::Rim::Spinner, 15, 30.9f);
    Car car(frontLeft, frontRight, backLeft, backRight, driver, passenger, 22.5f);
    
    std::cout << Json::out(car) << std::endl;

    std::cout << std::endl << "Press any key to exit." << std::endl;
    std::cin.clear();
    std::cin.get();

    return 0;
}
