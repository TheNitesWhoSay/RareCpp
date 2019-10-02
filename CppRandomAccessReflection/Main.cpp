#include <iostream>
#include "Json.h"
#include <tuple>
#include <typeinfo>
#include <algorithm>
#include <memory>

using namespace Reflect;
using u8 = uint8_t;

std::string myStr("myStr");

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

    REFLECT(FuelTank, (B<float>) capacity, (B<float>) currentLevel, (B<float[2]>) tickMarks)
};

class Wheel {
public:
    enum_t(Rim, short, {
        Regular = 0,
        Spinner = 1
    })

    Wheel() : rim(Rim::Regular), size(0), pressure(0.0f) {}
    Wheel(Rim rim, int size, float pressure) : rim(rim), size(size), pressure(pressure) {}

    Rim rim;
    int size;
    float pressure;

    REFLECT(Wheel, (B<Rim>) rim, (B<int>) size, (B<float>) pressure)
};

class CupHolder {
public:
    CupHolder() : width(0), height(0), occupied(false) {}
    CupHolder(int width, int height, bool occupied) : width(width), height(height), occupied(occupied) {}

    int width;
    int height;
    bool occupied;

    REFLECT(CupHolder, (B<int>) width, (B<int>) height, (B<bool>) occupied)
};
using CupHolderPtr = std::shared_ptr<CupHolder>;

class Car {
public:
    Car() : milesPerGallon(0.0f) {}
    Car(Wheel frontLeft, Wheel frontRight, Wheel backLeft, Wheel backRight, const std::string &driver, const std::string &passenger,
        const std::map<std::string, std::string> & occupantId, std::map<std::string, CupHolderPtr> occupantCupHolderUsage,
        const std::vector<CupHolderPtr> & cupHolders, const FuelTank &fuelTank, float milesPerGallon)
        : occupantId(occupantId), occupantCupHolderUsage(occupantCupHolderUsage), cupHolders(cupHolders), fuelTank(fuelTank), milesPerGallon(milesPerGallon)
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
    std::map<std::string, std::string> occupantId;
    std::map<std::string, CupHolderPtr> occupantCupHolderUsage;
    std::vector<CupHolderPtr> cupHolders;
    FuelTank fuelTank;
    float milesPerGallon;
    
    using OccupantIdType = std::map<std::string, std::string>;
    using OccupantCupHolderUsageType = std::map<std::string, CupHolderPtr>;
    REFLECT(Car, (R<Wheel[4]>) wheels, (B<std::vector<std::string>>) occupants, (B<OccupantIdType>) occupantId,
        (R<OccupantCupHolderUsageType>) occupantCupHolderUsage,
        (R<std::vector<CupHolderPtr>>) cupHolders,
        (R<FuelTank>) fuelTank, (B<float>) milesPerGallon)

};

class MassiveObject {
public:
    u8 f001; u8 f002; u8 f003; u8 f004; u8 f005; u8 f006; u8 f007; u8 f008; u8 f009; u8 f010; u8 f011; u8 f012; u8 f013; u8 f014; u8 f015; u8 f016;
    u8 f017; u8 f018; u8 f019; u8 f020; u8 f021; u8 f022; u8 f023; u8 f024; u8 f025; u8 f026; u8 f027; u8 f028; u8 f029; u8 f030; u8 f031; u8 f032;
    u8 f033; u8 f034; u8 f035; u8 f036; u8 f037; u8 f038; u8 f039; u8 f040; u8 f041; u8 f042; u8 f043; u8 f044; u8 f045; u8 f046; u8 f047; u8 f048;
    u8 f049; u8 f050; u8 f051; u8 f052; u8 f053; u8 f054; u8 f055; u8 f056; u8 f057; u8 f058; u8 f059; u8 f060; u8 f061; u8 f062; u8 f063; u8 f064;
    u8 f065; u8 f066; u8 f067; u8 f068; u8 f069; u8 f070; u8 f071; u8 f072; u8 f073; u8 f074; u8 f075; u8 f076; u8 f077; u8 f078; u8 f079; u8 f080;
    u8 f081; u8 f082; u8 f083; u8 f084; u8 f085; u8 f086; u8 f087; u8 f088; u8 f089; u8 f090; u8 f091; u8 f092; u8 f093; u8 f094; u8 f095; u8 f096;
    u8 f097; u8 f098; u8 f099; u8 f100; u8 f101; u8 f102; u8 f103; u8 f104; u8 f105; u8 f106; u8 f107; u8 f108; u8 f109; u8 f110; u8 f111; u8 f112;
    u8 f113; u8 f114; u8 f115; u8 f116; u8 f117; u8 f118; u8 f119; u8 f120; u8 f121; u8 f122; u8 f123; u8 f124;

    REFLECT(MassiveObject,
        (B<u8>) f001, (B<u8>) f002, (B<u8>) f003, (B<u8>) f004, (B<u8>) f005, (B<u8>) f006, (B<u8>) f007, (B<u8>) f008,
        (B<u8>) f009, (B<u8>) f010, (B<u8>) f011, (B<u8>) f012, (B<u8>) f013, (B<u8>) f014, (B<u8>) f015, (B<u8>) f016,
        (B<u8>) f017, (B<u8>) f018, (B<u8>) f019, (B<u8>) f020, (B<u8>) f021, (B<u8>) f022, (B<u8>) f023, (B<u8>) f024,
        (B<u8>) f025, (B<u8>) f026, (B<u8>) f027, (B<u8>) f028, (B<u8>) f029, (B<u8>) f030, (B<u8>) f031, (B<u8>) f032,
        (B<u8>) f033, (B<u8>) f034, (B<u8>) f035, (B<u8>) f036, (B<u8>) f037, (B<u8>) f038, (B<u8>) f039, (B<u8>) f040,
        (B<u8>) f041, (B<u8>) f042, (B<u8>) f043, (B<u8>) f044, (B<u8>) f045, (B<u8>) f046, (B<u8>) f047, (B<u8>) f048,
        (B<u8>) f049, (B<u8>) f050, (B<u8>) f051, (B<u8>) f052, (B<u8>) f053, (B<u8>) f054, (B<u8>) f055, (B<u8>) f056,
        (B<u8>) f057, (B<u8>) f058, (B<u8>) f059, (B<u8>) f060, (B<u8>) f061, (B<u8>) f062, (B<u8>) f063, (B<u8>) f064,
        (B<u8>) f065, (B<u8>) f066, (B<u8>) f067, (B<u8>) f068, (B<u8>) f069, (B<u8>) f070, (B<u8>) f071, (B<u8>) f072,
        (B<u8>) f073, (B<u8>) f074, (B<u8>) f075, (B<u8>) f076, (B<u8>) f077, (B<u8>) f078, (B<u8>) f079, (B<u8>) f080,
        (B<u8>) f081, (B<u8>) f082, (B<u8>) f083, (B<u8>) f084, (B<u8>) f085, (B<u8>) f086, (B<u8>) f087, (B<u8>) f088,
        (B<u8>) f089, (B<u8>) f090, (B<u8>) f091, (B<u8>) f092, (B<u8>) f093, (B<u8>) f094, (B<u8>) f095, (B<u8>) f096,
        (B<u8>) f097, (B<u8>) f098, (B<u8>) f099, (B<u8>) f100, (B<u8>) f101, (B<u8>) f102, (B<u8>) f103, (B<u8>) f104,
        (B<u8>) f105, (B<u8>) f106, (B<u8>) f107, (B<u8>) f108, (B<u8>) f109, (B<u8>) f110, (B<u8>) f111, (B<u8>) f112,
        (B<u8>) f113, (B<u8>) f114, (B<u8>) f115, (B<u8>) f116, (B<u8>) f117, (B<u8>) f118, (B<u8>) f119, (B<u8>) f120,
        (B<u8>) f121, (B<u8>) f122, (B<u8>) f123
        //,(B<u8>) f124 // This will cause an error "C1009 - compiler limit: macros nested too deeply"
    )
};

int main()
{
    for ( size_t i=1; i<=MassiveObject::Class::totalFields; i++ )
    {
        std::cout << MassiveObject::Class::fields[i-1].name;
        if ( i % 16 == 0 || i == MassiveObject::Class::totalFields )
            std::cout << std::endl;
        else
            std::cout << ", ";
    }
    
    std::vector<CupHolderPtr> cupHolders;
    std::map<std::string, std::string> occupantId;
    std::map<std::string, CupHolderPtr> occupantCupHolderUsage;
    Wheel frontLeft(Wheel::Rim::Regular, 14, 32.2f), frontRight(Wheel::Rim::Regular, 14, 31.9f), backLeft(Wheel::Rim::Spinner, 15, 33.0f), backRight(Wheel::Rim::Spinner, 15, 30.9f);
    std::string driver = "Fred";
    std::string passenger = "Bob";
    occupantId.insert(std::pair<std::string, std::string>(driver, "B252-123-839-244"));
    occupantId.insert(std::pair<std::string, std::string>(passenger, "B252-612-321-245"));
    cupHolders.push_back(CupHolderPtr(new CupHolder(3, 3, true)));
    cupHolders.push_back(CupHolderPtr(new CupHolder(2, 2, false)));
    occupantCupHolderUsage.insert(std::pair<std::string, CupHolderPtr>(driver, cupHolders[0]));
    occupantCupHolderUsage.insert(std::pair<std::string, CupHolderPtr>(passenger, cupHolders[1]));
    FuelTank fuelTank(15.0f, 14.6f, 1.0f, 7.5f);
    Car car(frontLeft, frontRight, backLeft, backRight, driver, passenger, occupantId, occupantCupHolderUsage, cupHolders, fuelTank, 22.5f);
    
    for ( size_t i=0; i<Wheel::Class::totalFields; i++ )
        std::cout << Wheel::Class::fields[i].name << std::endl;

    for ( size_t i=0; i<Wheel::Class::totalFields; i++ )
    {
        Wheel::Class::FieldAt(frontLeft, i, [&](auto field, auto value) {
            std::cout << field.name << ": " << value << std::endl;
        });
    }

    Wheel::Class::ForEachField(backRight, [&](auto field, auto value) {
        std::cout << field.name << ": " << value << std::endl;
    });

    Car::Class::ForEachField(car, [&](auto field, auto value) {
        //std::cout << field.name << ": " << value << std::endl; // This will cause a compiler error as there's no ostream operator overload for the FuelTank type!
    });

    Car::Class::ForEachField(car, [&](auto field, auto value) {
        field.IfPrimitive([&](auto) { std::cout << "(carPrimitive) " << field.name << ": " << value << std::endl; });
        field.IfObject([&](auto) { std::cout << "(carObject) " << field.name << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl; });
        field.IfObjectArray([&](auto) { std::cout << "(carObjectArray) " << field.name << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl; });
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        std::cout << field.name << ": " << value << std::endl; // This will print out a pointer, not the array values!
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        if ( field.isIterable ) {
            //std::cout << field.name << ": " << value[0] << std::endl; // This will cause a compiler error as not every field in the FuelTank class is an array!
        }
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        field.IfPrimitive([&](auto) { std::cout << "(fuelTankPrimitive) " << field.name << ": " << value << std::endl; });
        field.IfPrimitiveArray([&](auto) { std::cout << "(fuelTankPrimitiveArray) " << field.name << ": " << value << std::endl; });
    });

    std::cout << Json::out(car) << std::endl;

    std::cout << std::endl << "Press any key to exit." << std::endl;
    std::cin.clear();
    std::cin.get();

    return 0;
}
