#include <iostream>
#include "../CppRandomAccessReflectionLib/Json.h"
#include <tuple>
#include <typeinfo>
#include <algorithm>
#include <memory>
using namespace Reflect;
using u8 = uint8_t;

ENABLE_JSON_INPUT;

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

    REFLECT(() FuelTank, () capacity, () currentLevel, () tickMarks)
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

    REFLECT(() Wheel, () rim, () size, () pressure)
};

class CupHolder {
public:
    CupHolder() : width(0), height(0), occupied(false) {}
    CupHolder(int width, int height, bool occupied) : width(width), height(height), occupied(occupied) {}

    int width;
    int height;
    bool occupied;

    REFLECT(() CupHolder, () width, () height, () occupied)
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
    
    REFLECT(() Car,
        (Reflected) wheels,
        (Json::String) occupants,
        (Json::String) testNest,
        (Json::String) testMapNest,
        (Json::String) occupantId,
        (Reflected) occupantCupHolderUsage,
        (Reflected) cupHolders,
        (Reflected) fuelTank,
        () milesPerGallon
    )
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
        () f001, () f002, () f003, () f004, () f005, () f006, () f007, () f008,
        () f009, () f010, () f011, () f012, () f013, () f014, () f015, () f016,
        () f017, () f018, () f019, () f020, () f021, () f022, () f023, () f024,
        () f025, () f026, () f027, () f028, () f029, () f030, () f031, () f032,
        () f033, () f034, () f035, () f036, () f037, () f038, () f039, () f040,
        () f041, () f042, () f043, () f044, () f045, () f046, () f047, () f048,
        () f049, () f050, () f051, () f052, () f053, () f054, () f055, () f056,
        () f057, () f058, () f059, () f060, () f061, () f062, () f063, () f064,
        () f065, () f066, () f067, () f068, () f069, () f070, () f071, () f072,
        () f073, () f074, () f075, () f076, () f077, () f078, () f079, () f080,
        () f081, () f082, () f083, () f084, () f085, () f086, () f087, () f088,
        () f089, () f090, () f091, () f092, () f093, () f094, () f095, () f096,
        () f097, () f098, () f099, () f100, () f101, () f102, () f103, () f104,
        () f105, () f106, () f107, () f108, () f109, () f110, () f111, () f112,
        () f113, () f114, () f115, () f116, () f117, () f118, () f119, () f120,
        () f121, () f122, () f123
        //,() f124 // This will cause an error "C1009 - compiler limit: macros nested too deeply"
    )
};

class Super
{
public:
    int val;

    REFLECT(() Super, () val)
};

class OtherSuper
{
public:
    int otherVal;
    
    REFLECT(() OtherSuper, () otherVal)
};

class SubTest : public Super, public OtherSuper
{
public:
    int subVal;
    
    using Parents = Inherit<Super, OtherSuper>;
    REFLECT((Parents) SubTest, () otherVal)
};

Car outputExamples()
{
    for ( size_t i=1; i<=MassiveObject::Class::TotalFields; i++ )
    {
        std::cout << MassiveObject::Class::Fields[i-1].name;
        if ( i % 16 == 0 || i == MassiveObject::Class::TotalFields )
            std::cout << std::endl;
        else
            std::cout << ", ";
    }

    SubTest sub;
    sub.val = 1;
    sub.otherVal = 2;
    sub.subVal = 3;
    
    SubTest::Supers::ForEach(sub, [&](auto index, auto & superObj) {
        using Super = typename std::remove_reference<decltype(superObj)>::type;
        std::cout << decltype(index)::Index << ": " << ExtendedTypeSupport::TypeToStr<Super>() << " {" << std::endl;
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
    
    for ( size_t i=0; i<Wheel::Class::TotalFields; i++ )
        std::cout << Wheel::Class::Fields[i].name << std::endl;

    for ( size_t i=0; i<Wheel::Class::TotalFields; i++ )
    {
        Wheel::Class::FieldAt(frontLeft, i, [&](auto & field, auto & value) {
            std::cout << field.name << ": " << value << std::endl;
        });
    }

    Wheel::Class::ForEachField(backRight, [&](auto & field, auto & value) {
        std::cout << field.name << ": " << value << std::endl;
    });

    Car::Class::ForEachField(car, [&](auto & field, auto & value) {
        //std::cout << field.name << ": " << value << std::endl; // This will cause a compiler error as there's no ostream operator overload for the FuelTank type!
    });

    Car::Class::ForEachField(car, [&](auto & field, auto & value) {
        using Field = std::remove_reference<decltype(field)>::type;
        using Type = std::remove_reference<decltype(value)>::type;
        if constexpr ( !ExtendedTypeSupport::is_iterable<Type>::value && !Field::template HasAnnotation<Reflected> )
            std::cout << "(carPrimitive) " << field.name << ": " << value << std::endl;
        else if constexpr ( !ExtendedTypeSupport::is_iterable<Type>::value && !Field::template HasAnnotation<Reflected> )
            std::cout << "(carObject) " << field.name << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl;
        else if constexpr ( !ExtendedTypeSupport::is_iterable<Type>::value && !Field::template HasAnnotation<Reflected> )
            std::cout << "(carObjectArray) " << field.name << ": " << "[Skipped, this would be a good place for recursion!]" << std::endl;
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto & field, auto & value) {
        //std::cout << field.name << ": " << value << std::endl; // This will print out a pointer, not the array values!
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto & field, auto & value) {
        using Type = std::remove_reference<decltype(value)>::type;
        if constexpr ( ExtendedTypeSupport::is_static_array<Type>::value )
            std::cout << field.name << ": " << value[0] << std::endl;
        else
            std::cout << field.name << ": " << value << std::endl;
    });

    FuelTank::Class::ForEachField(fuelTank, [&](auto & field, auto & value) {
        using Field = std::remove_reference<decltype(field)>::type;
        using Type = std::remove_reference<decltype(value)>::type;
        if constexpr ( !ExtendedTypeSupport::is_iterable<Type>::value && !Field::template HasAnnotation<Reflected> )
            std::cout << "(fuelTankPrimitive) " << field.name << ": " << value << std::endl;
        else if constexpr ( ExtendedTypeSupport::is_static_array<decltype(value)>::value && !Field::template HasAnnotation<Reflected> )
            std::cout << "(fuelTankPrimitiveArray) " << field.name << ": " << value << std::endl;
    });

    return car;
}

class SuperA {
public:
    SuperA() : superVal(0) {}

    int superVal;

    REFLECT(() SuperA, () superVal)
};

class OtherSuperA {
public:
    REFLECT_EMPTY(() OtherSuperA)
};

class SubA {
public:
    SubA() : subVal(0) {}

    int subVal;

    REFLECT(() SubA, () subVal)
};

class A : public SuperA, public OtherSuperA {
public:
    enum_t(TestEnum, u8, {
        first,
        second
    });
    static const std::unordered_map<std::string, TestEnum> TestEnumCache;

    A() : testEnum(TestEnum::first), first(0), second(0), ptr(nullptr), sub(), boolean(false), str("") { ray[0] = 0; ray[1] = 0; }

    TestEnum testEnum;
    int first;
    int second;
    int* ptr;
    SubA sub;
    bool boolean;
    std::string str;
    std::map<std::string, std::string> map;
    std::vector<std::vector<int>> vecVec;
    int ray[2];

    using Parents = Inherit<SuperA, OtherSuperA>;
    REFLECT((Parents) A, (Json::Enum) testEnum, (Reflected) sub, () first, () second,
        () ptr, () boolean, (Json::String) str, (Json::String) map, () vecVec, () ray)
};

const std::unordered_map<std::string, A::TestEnum> A::TestEnumCache = {
    { "first", A::TestEnum::first },
    { "second", A::TestEnum::second }
};

bool Json::EnumString<A, A::TestEnum, A::Class::IndexOf::testEnum>::From(const std::string input, const A & object, A::TestEnum & value)
{
    auto found = A::TestEnumCache.find(input);
    if ( found != A::TestEnumCache.end() )
    {
        value = found->second;
        return true;
    }
    else
        return false;
}

std::string Json::EnumString<A, A::TestEnum, A::Class::IndexOf::testEnum>::To(const A & object, const A::TestEnum & value)
{
    switch ( value )
    {
        case A::TestEnum::first: return "firstAnnotation";
        case A::TestEnum::second: return "secondAnnotation";
    }
    return "";
}

std::ostream & operator<<(std::ostream & os, const A::TestEnum & testEnum)
{
    switch ( testEnum )
    {
        case A::TestEnum::first: os << "firstStream"; break;
        case A::TestEnum::second: os << "secondStream"; break;
    }
    return os;
}

std::istream & operator>>(std::istream & is, A::TestEnum & testEnum)
{
    std::string input;
    is >> input;
    if ( is.good() )
    {
        auto found = A::TestEnumCache.find(input);
        if ( found != A::TestEnumCache.end() )
            testEnum = found->second;
    }
    return is;
}

int main()
{
    Car car = outputExamples();
    std::cout << std::endl << Json::out(car) << std::endl << std::endl;

    A a;
    do {
        bool successfulRead = false;
        try {
            std::cin >> Json::in(a);
            successfulRead = true;
        } catch ( Json::Exception & e ) {
            std::cout << std::endl << "Exception: " << e.what() << std::endl;
        }
        //Json::putClassFieldCache(std::cout);
        //std::cout << "..." << std::endl;
        std::cout << "Read in: " << Json::pretty(a) << std::endl;
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
