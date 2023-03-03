#include <rarecpp/reflect.h>
#include <iostream>

class MyObj
{
public:
    int pub = 0;
protected:
    int prot = 0;
    inline static int otherProt = 0;
private:
    int priv = 0;
public:
    int otherPub = 0;

    REFLECT(MyObj, pub, prot, otherProt, priv, otherPub)
};

void printNonPrivateMods(size_t memberIndex, RareTs::AccessMod accessMod)
{
    if ( accessMod == RareTs::AccessMod::Public )
        std::cout << "member " << memberIndex << " is public" << std::endl;
    else if ( accessMod == RareTs::AccessMod::Protected )
        std::cout << "member " << memberIndex << " is protected" << std::endl;
}

void memberAccessProtectionModifier()
{
    constexpr RareTs::AccessMod accessModMemberZero = RareTs::access_modifier_v<MyObj, 0>;
    constexpr RareTs::AccessMod accessModMemberOne = RareTs::access_modifier_v<MyObj, 1>;
    constexpr RareTs::AccessMod accessModMemberTwo = RareTs::access_modifier_v<MyObj, 2>;
    constexpr RareTs::AccessMod accessModMemberThree = RareTs::access_modifier_v<MyObj, 3>;
    constexpr RareTs::AccessMod accessModMemberFour = RareTs::access_modifier_v<MyObj, 4>;
    printNonPrivateMods(0, accessModMemberZero);
    printNonPrivateMods(1, accessModMemberOne);
    printNonPrivateMods(2, accessModMemberTwo);
    printNonPrivateMods(3, accessModMemberThree);
    printNonPrivateMods(4, accessModMemberFour);
}
