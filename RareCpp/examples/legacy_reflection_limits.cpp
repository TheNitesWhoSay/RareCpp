#include <rarecpp/reflect.h>
#include <iostream>

class MassiveObject {
public:
    int f001; int f002; int f003; int f004; int f005; int f006; int f007; int f008; int f009; int f010; int f011; int f012; int f013; int f014; int f015;
    int f016; int f017; int f018; int f019; int f020; int f021; int f022; int f023; int f024; int f025; int f026; int f027; int f028; int f029; int f030;
    int f031; int f032; int f033; int f034; int f035; int f036; int f037; int f038; int f039; int f040; int f041; int f042; int f043; int f044; int f045;
    int f046; int f047; int f048; int f049; int f050; int f051; int f052; int f053; int f054; int f055; int f056; int f057; int f058; int f059; int f060;
    int f061; int f062; int f063; int f064; int f065; int f066; int f067; int f068; int f069; int f070; int f071; int f072; int f073; int f074; int f075;
    int f076; int f077; int f078; int f079; int f080; int f081; int f082; int f083; int f084; int f085; int f086; int f087; int f088; int f089; int f090;
    int f091; int f092; int f093; int f094; int f095; int f096; int f097; int f098; int f099; int f100; int f101; int f102; int f103; int f104; int f105;
    int f106; int f107; int f108; int f109; int f110; int f111; int f112; int f113; int f114; int f115; int f116; int f117; int f118; int f119; int f120;
    int f121; int f122; int f123; int f124; int f125; int f126;

    REFLECT(MassiveObject,
        f001, f002, f003, f004, f005, f006, f007, f008,
        f009, f010, f011, f012, f013, f014, f015, f016,
        f017, f018, f019, f020, f021, f022, f023, f024,
        f025, f026, f027, f028, f029, f030, f031, f032,
        f033, f034, f035, f036, f037, f038, f039, f040,
        f041, f042, f043, f044, f045, f046, f047, f048,
        f049, f050, f051, f052, f053, f054, f055, f056,
        f057, f058, f059, f060, f061, f062, f063, f064,
        f065, f066, f067, f068, f069, f070, f071, f072,
        f073, f074, f075, f076, f077, f078, f079, f080,
        f081, f082, f083, f084, f085, f086, f087, f088,
        f089, f090, f091, f092, f093, f094, f095, f096,
        f097, f098, f099, f100, f101, f102, f103, f104,
        f105, f106, f107, f108, f109, f110, f111, f112,
        f113, f114, f115, f116, f117, f118, f119, f120,
        f121, f122, f123, f124, f125 //, 126 // This will exceed the max capacity of macro loops and cause compilation errors
    )
};

void reflectionLimitsExample()
{
    RareTs::Members<MassiveObject>::forEach([](auto & member) {
        if constexpr ( member.index % 16 == 0 && member.index > 0 )
            std::cout << std::endl << member.name << ", ";
        else if constexpr ( member.index+1 < RareTs::Members<MassiveObject>::total )
            std::cout << member.name << ", ";
        else
            std::cout << member.name << std::endl;
    });
}
