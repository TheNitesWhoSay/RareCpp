#include <iostream>
#include "../CppRandomAccessReflectionLib/Json.h"
#include <tuple>
#include <typeinfo>
#include <algorithm>
#include <memory>

ENABLE_JSON_INPUT;

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
        testStack.push(0);
        testStack.push(1);
    }

    float capacity;
    float currentLevel;
    float tickMarks[2];
    std::stack<int> testStack;

    REFLECT(() FuelTank, (B) capacity, (B) currentLevel, (B) tickMarks, (B) testStack)
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

    REFLECT(() Wheel, (B) rim, (B) size, (B) pressure)
};

class CupHolder {
public:
    CupHolder() : width(0), height(0), occupied(false) {}
    CupHolder(int width, int height, bool occupied) : width(width), height(height), occupied(occupied) {}

    int width;
    int height;
    bool occupied;

    REFLECT(() CupHolder, (B) width, (B) height, (B) occupied)
};
using CupHolderPtr = std::shared_ptr<CupHolder>;

class Car {
public:
    Car() : milesPerGallon(0.0f) {}
    Car(Wheel frontLeft, Wheel frontRight, Wheel backLeft, Wheel backRight, const std::string &driver, const std::string &passenger,
        const std::vector<std::vector<std::vector<std::string>>> & testNest, const std::map<std::string, std::vector<std::vector<std::string>>> & testMapNest,
        const std::map<std::string, std::string> & occupantId, std::map<std::string, CupHolderPtr> occupantCupHolderUsage,
        const std::vector<CupHolderPtr> & cupHolders, const FuelTank &fuelTank, float milesPerGallon)
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
    std::map<std::string, CupHolderPtr> occupantCupHolderUsage;
    std::vector<CupHolderPtr> cupHolders;
    FuelTank fuelTank;
    float milesPerGallon;
    
    using OccupantIdType = std::map<std::string, std::string>;
    using OccupantCupHolderUsageType = std::map<std::string, CupHolderPtr>;
    REFLECT(() Car, (R) wheels, (S) occupants, (S) testNest, (S) testMapNest, (S) occupantId, (R) occupantCupHolderUsage, (R) cupHolders, (R) fuelTank, (B) milesPerGallon)

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

    REFLECT(() MassiveObject,
        (B) f001, (B) f002, (B) f003, (B) f004, (B) f005, (B) f006, (B) f007, (B) f008,
        (B) f009, (B) f010, (B) f011, (B) f012, (B) f013, (B) f014, (B) f015, (B) f016,
        (B) f017, (B) f018, (B) f019, (B) f020, (B) f021, (B) f022, (B) f023, (B) f024,
        (B) f025, (B) f026, (B) f027, (B) f028, (B) f029, (B) f030, (B) f031, (B) f032,
        (B) f033, (B) f034, (B) f035, (B) f036, (B) f037, (B) f038, (B) f039, (B) f040,
        (B) f041, (B) f042, (B) f043, (B) f044, (B) f045, (B) f046, (B) f047, (B) f048,
        (B) f049, (B) f050, (B) f051, (B) f052, (B) f053, (B) f054, (B) f055, (B) f056,
        (B) f057, (B) f058, (B) f059, (B) f060, (B) f061, (B) f062, (B) f063, (B) f064,
        (B) f065, (B) f066, (B) f067, (B) f068, (B) f069, (B) f070, (B) f071, (B) f072,
        (B) f073, (B) f074, (B) f075, (B) f076, (B) f077, (B) f078, (B) f079, (B) f080,
        (B) f081, (B) f082, (B) f083, (B) f084, (B) f085, (B) f086, (B) f087, (B) f088,
        (B) f089, (B) f090, (B) f091, (B) f092, (B) f093, (B) f094, (B) f095, (B) f096,
        (B) f097, (B) f098, (B) f099, (B) f100, (B) f101, (B) f102, (B) f103, (B) f104,
        (B) f105, (B) f106, (B) f107, (B) f108, (B) f109, (B) f110, (B) f111, (B) f112,
        (B) f113, (B) f114, (B) f115, (B) f116, (B) f117, (B) f118, (B) f119, (B) f120,
        (B) f121, (B) f122, (B) f123
        //,(B) f124 // This will cause an error "C1009 - compiler limit: macros nested too deeply"
    )
};

class Super
{
public:
    int val;

    REFLECT(() Super, (B) val)
};

class OtherSuper
{
public:
    int otherVal;
    
    REFLECT(() OtherSuper, (B) otherVal)
};

class SubTest : public Super, public OtherSuper
{
public:
    int subVal;
    
    using Inherit = I<Super, OtherSuper>;
    REFLECT((Inherit) SubTest, (B) otherVal)
};

void outputExamples()
{
    for ( size_t i=1; i<=MassiveObject::Class::totalFields; i++ )
    {
        std::cout << MassiveObject::Class::fields[i-1].name;
        if ( i % 16 == 0 || i == MassiveObject::Class::totalFields )
            std::cout << std::endl;
        else
            std::cout << ", ";
    }

    SubTest sub;
    sub.val = 1;
    sub.otherVal = 2;
    sub.subVal = 3;
    
    SubTest::Supers::ForEach(sub, [&](size_t index, auto & superObj) {
        using Super = typename std::remove_reference<decltype(superObj)>::type;
        std::cout << index << ": " << TypeToStr<Super>() << " {" << std::endl;
        Super::Class::ForEachField(superObj, [&](auto & field, auto & value) {
            std::cout << "  " << field.name << ": " << value << std::endl;
        });
        std::cout << "}" << std::endl;
    });

    std::vector<CupHolderPtr> cupHolders;
    std::map<std::string, std::string> occupantId;
    std::map<std::string, CupHolderPtr> occupantCupHolderUsage;
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
    cupHolders.push_back(CupHolderPtr(new CupHolder(3, 3, true)));
    cupHolders.push_back(CupHolderPtr(new CupHolder(2, 2, false)));
    occupantCupHolderUsage.insert(std::pair<std::string, CupHolderPtr>(driver, cupHolders[0]));
    occupantCupHolderUsage.insert(std::pair<std::string, CupHolderPtr>(passenger, cupHolders[1]));
    FuelTank fuelTank(15.0f, 14.6f, 1.0f, 7.5f);
    Car car(frontLeft, frontRight, backLeft, backRight, driver, passenger, testNest, testMapNest, occupantId, occupantCupHolderUsage, cupHolders, fuelTank, 22.5f);
    
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
        if constexpr ( field.IsPrimitive )
            std::cout << "(carPrimitive) " << field.name << ": " << value << std::endl;
        else if constexpr ( field.IsObject )
            std::cout << "(carObject) " << field.name << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl;
        else if constexpr ( field.IsObjectArray )
            std::cout << "(carObjectArray) " << field.name << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl;
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        //std::cout << field.name << ": " << value << std::endl; // This will print out a pointer, not the array values!
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        if ( field.isIterable ) {
            //std::cout << field.name << ": " << value[0] << std::endl; // This will cause a compiler error as not every field in the FuelTank class is an array!
        }
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
        if constexpr ( field.IsPrimitive )
            std::cout << "(fuelTankPrimitive) " << field.name << ": " << value << std::endl;
        else if constexpr ( field.IsPrimitiveArray )
            std::cout << "(fuelTankPrimitiveArray) " << field.name << ": " << value << std::endl;
    });

    std::cout << Json::out(car) << std::endl;
}

class SuperA {
public:
    int superVal;

    REFLECT(() SuperA, (B) superVal)
};

class SubA {
public:
    SubA() : subVal(0) {}

    int subVal;

    REFLECT(() SubA, (B) subVal)
};

class A {
public:
    A() : first(0), second(0), ptr(nullptr), sub(), boolean(false), str("") {}

    int first;
    int second;
    int* ptr;
    SubA sub;
    bool boolean;
    std::string str;
    std::map<std::string, std::string> map;
    int ray[2];

    REFLECT((SuperA) A, (R) sub, (B) first, (B) second, (B) ptr, (B) boolean, (S) str, (S) map, (B) ray)
};

std::istream & operator >>(std::istream & is, A & a) {
    is >> a.first;
    is >> a.second;
    return is;
}

int main()
{
    outputExamples();

    A a;
    a.ptr = nullptr;
    do {
        bool successfulRead = false;
        try {
            std::cin >> std::ws;
            std::cin >> Json::in(a);
            successfulRead = true;
        } catch ( Json::Exception & e ) {
            std::cout << std::endl << "Exception: " << e.what() << std::endl;
        }
        //Json::putClassFieldCache(std::cout);
        //std::cout << "..." << std::endl;
        std::cout << "Read in: " << Json::out(a) << std::endl;
        std::cout << "..." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while ( true );

    std::cout << std::endl << "Press enter to exit." << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}
