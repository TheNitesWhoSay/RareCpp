#include <rarecpp/reflect.h>
#include <rarecpp/json.h>
#include <iostream>

struct RegularClass
{
    int a = 0;
    int b = 0;

    REFLECT(RegularClass, a, b)
};

struct OtherClass
{
    int eyy = 0;
    int c = 0;
    int bee = 0;

    REFLECT(OtherClass, eyy, c, bee)

    // Rarely you might use an "anonymous" note, which does NOT refer to an identifier used in the REFLECT macro
    // These can be useful when a reflection extension needs to refer to the REFLECT metadata of the class in which it resides, as is the case with RareMapper
    NOTE(ObjectMappings, RareMapper::createMapping<RegularClass, OtherClass>()
        .a->eyy()
        .b->bee()
        .bidirectional())
};

void noteAnonymous()
{
    RegularClass reg {3, 4};
    std::cout << Json::pretty(reg) << std::endl;

    OtherClass other = RareMapper::map<OtherClass>(reg); // Finds & uses the anonymous note "OtherClass::ObjectMappings" to figure out which fields to assign
    std::cout << Json::pretty(other) << std::endl;
}
