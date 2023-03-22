#ifndef CAR_H
#define CAR_H
#include <rarecpp/reflect.h>
#include <map>
#include <memory>
#include <ostream>
#include <vector>

class FuelTank {
public:
    FuelTank() : capacity(0.0f), currentLevel(0.0f) { tickMarks[0] = 0.0f; tickMarks[1] = 0.0f; }
    FuelTank(float capacity, float currentLevel, float tickMarkOne, float tickMarkTwo) : capacity(capacity), currentLevel(currentLevel)
    {
        tickMarks[0] = tickMarkOne;
        tickMarks[1] = tickMarkTwo;
    }

    float capacity;
    float currentLevel;
    float tickMarks[2];

    REFLECT(FuelTank, capacity, currentLevel, tickMarks)
};

class Wheel {
public:
    enum class Rim { Regular, Spinner };

    Wheel() : rim(Rim::Regular), size(0), pressure(0.0f) {}
    Wheel(Rim rim, int size, float pressure) : rim(rim), size(size), pressure(pressure) {}

    Rim rim;
    int size;
    float pressure;

    REFLECT(Wheel, rim, size, pressure)
};

class CupHolder {
public:
    CupHolder() : width(0), height(0), occupied(false) {}
    CupHolder(int width, int height, bool occupied) : width(width), height(height), occupied(occupied) {}

    int width;
    int height;
    bool occupied;

    REFLECT(CupHolder, width, height, occupied)
};

class Car {
public:
    Car() : milesPerGallon(0.0f) {}
    Car(Wheel frontLeft, Wheel frontRight, Wheel backLeft, Wheel backRight, const std::string &driver, const std::string &passenger,
        const std::vector<std::vector<std::vector<std::string>>> & testNest, const std::map<std::string, std::vector<std::vector<std::string>>> & testMapNest,
        const std::map<std::string, std::string> & occupantId, std::map<std::string, std::shared_ptr<CupHolder>> occupantCupHolderUsage,
        const std::vector<std::shared_ptr<CupHolder>> & cupHolders, const FuelTank &fuelTank, float milesPerGallon)
        : testNest(testNest), testMapNest(testMapNest), occupantId(occupantId), occupantCupHolderUsage(occupantCupHolderUsage), cupHolders(cupHolders),
        fuelTank(fuelTank), milesPerGallon(milesPerGallon)
    {
        wheels[0] = frontLeft;
        wheels[1] = frontRight;
        wheels[2] = backLeft;
        wheels[3] = backRight;
        occupants.push_back(driver);
        occupants.push_back(passenger);
    }

    Wheel wheels[4];
    std::vector<std::string> occupants;
    std::vector<std::vector<std::vector<std::string>>> testNest;
    std::map<std::string, std::vector<std::vector<std::string>>> testMapNest;
    std::map<std::string, std::string> occupantId;
    std::map<std::string, std::shared_ptr<CupHolder>> occupantCupHolderUsage;
    std::vector<std::shared_ptr<CupHolder>> cupHolders;
    FuelTank fuelTank;
    float milesPerGallon;
    
    REFLECT(Car,
        wheels,
        occupants,
        testNest,
        testMapNest,
        occupantId,
        occupantCupHolderUsage,
        cupHolders,
        fuelTank,
        milesPerGallon
    )
};

inline std::ostream & operator<<(std::ostream & os, const Wheel::Rim & rim)
{
    os << (short)rim;
    return os;
}

#endif