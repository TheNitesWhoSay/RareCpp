#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
using RareTs::Reflect;
using RareTs::Super;

class Superish
{
public:
    int val;

    REFLECT(Superish, val)
};

class OtherSuper
{
public:
    int otherVal;
    
    REFLECT(OtherSuper, otherVal)
};

NOTE(SubTest, Super<Superish>(Json::Name{"asdfjk"}, Json::Name{"anotherName"}, Json::Ignore), Super<OtherSuper>)
class SubTest : public Superish, public OtherSuper
{
public:
    int subVal;
    
    REFLECT_NOTED(SubTest, subVal)
};

void annotatedSupersExample()
{
    SubTest sub;
    sub.val = 1;
    sub.otherVal = 2;
    sub.subVal = 3;
    std::cout << Json::pretty(sub) << std::endl;

    std::map<std::string, int> myMap { {"myMap", 1}, {"a", 2} };
    std::tuple<std::string, int> myTuple { "myTuple", 3 };
    std::vector<std::string> myVector { "myVector", "b", "c", "d" };
    std::cout << Json::pretty(myMap) << std::endl;
    std::cout << Json::pretty(myTuple) << std::endl;
    std::cout << Json::pretty(myVector) << std::endl;
    
    Reflect<SubTest>::Supers::forEach(sub, [&](auto superInfo, auto & superObj) {
        using SuperInfo = decltype(superInfo);
        auto & annotations = superInfo.notes;
        using Annotations = RareTs::remove_cvref_t<decltype(annotations)>;
        std::cout << RareTs::toStr<Annotations>() << std::endl;
        std::cout << SuperInfo::template hasNote<Json::IgnoreType>() << std::endl;
        
        superInfo.template forEachNote<Json::Name>([&](auto & jsonName) {
            std::cout << "jsonName: " <<  jsonName.value << std::endl;
        });

        superInfo.forEachNote([&](auto & annotation) {
            using Annotation = decltype(annotation);
            std::cout << "annotation: " << RareTs::toStr<Annotation>() << std::endl;
        });

        if constexpr ( SuperInfo::template hasNote<Json::Name>() )
            std::cout << superInfo.template getNote<Json::Name>().value << std::endl;

        using Super = typename SuperInfo::type;
        std::cout << decltype(superInfo)::index << ": " << RareTs::toStr<Super>() << " {" << std::endl;
        Reflect<Super>::Members::forEach(superObj, [&](auto & member, auto & value) {
            std::cout << "  " << member.name << ": " << value << std::endl;
        });
        std::cout << "}" << std::endl;
    });
}