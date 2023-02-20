#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <cstddef>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>

using RareTs::Super;
using RareTs::Reflect;
using RareTs::Filter;

struct SerialVersionId { size_t i; };
template <typename Class, typename T>
bool contains_member_of_type()
{
    return Reflect<Class>::Members::pack([](auto && ...ts) {
        return (std::is_same_v<T,typename std::decay_t<decltype(ts)>::type> || ...);
    });
}

template <typename T>
auto tie(T & t)
{
    return Reflect<T>::Values::pack(t, [](auto && ...ts) {
        return std::forward_as_tuple(ts...);
    });
}

struct SuperOne {};
struct SuperTwo {};

NOTE(TestObj, SerialVersionId{1},
    Super<SuperOne>( Json::Name{"One"} ),
    Super<SuperTwo>( Json::Ignore ))
struct TestObj : SuperOne, SuperTwo
{
    int a = 0;
    constexpr static int b = 2;
    static int c;
    std::string d = "four";
    static std::string e;
    static int* f;

    REFLECT_NOTED(TestObj, a, b, c, d, e, f)
};

int TestObj::c = 3;
std::string TestObj::e = "five";
int* TestObj::f = nullptr;

void refactoringExperiment()
{
    std::cout << "total: " << Reflect<TestObj>::Members::total << std::endl;
    std::cout << "a: " << Reflect<TestObj>::Members::indexOf("a") << std::endl;
    std::cout << "b: " << Reflect<TestObj>::Members::indexOf("b") << std::endl;
    std::cout << "c: " << Reflect<TestObj>::Members::indexOf("c") << std::endl;
    std::cout << "d: " << Reflect<TestObj>::Members::indexOf("d") << std::endl;

    TestObj mixerTest {};
    Reflect<TestObj>::Members::forEach<Filter::IsInstanceData>([&](auto & member) {
        using Member = std::remove_reference_t<decltype(member)>;
        std::cout << RareTs::toStr<typename Member::type>() << std::endl;
    });

    constexpr auto member = Reflect<TestObj>::Members::member<2>;
    std::cout << 2 << ": " << member.name << std::endl;
    size_t runtimeIndex = Reflect<TestObj>::Members::indexOf("c");
    Reflect<TestObj>::Members::named(std::string("c"), [&](auto & member) {
        std::cout << runtimeIndex << ": " << member.name << std::endl;
    });
    Reflect<TestObj>::Members::pack([&](auto & ... ts) {
        (std::cout << ... << std::remove_reference_t<decltype(ts)>::name) << std::endl;
    });

    TestObj t {};

    Reflect<TestObj>::Members::forEach<Filter::IsInstanceData>(t, [&](auto & member, auto & value) {
        std::cout << "[instance] " << member.name << ": " << value << std::endl;
    });

    Reflect<TestObj>::Members::forEach<std::is_same, std::string>(t, [&](auto & member, auto & value) {
        std::cout << "[str] " << member.name << ": " << value << std::endl;
    });

    int checksum = 0;
    Reflect<TestObj>::Values::forEach<std::is_integral>(t, [&](auto & value) {
        checksum += value;
    });
    std::cout << "Checksum: " << checksum << std::endl;

    Reflect<TestObj>::Values::forEach<std::is_pointer>([&](auto & value) {
        value = &TestObj::c;
    });
    std::cout << "f* = " << (t.f == &TestObj::c ? "&TestObj::c" : "nullptr") << std::endl;
    

    
    Reflect<TestObj>::Members::forEach(t, [&](auto & member, const auto & value) {
            std::cout << member.name << ": " << value << std::endl;
    });
    Reflect<TestObj>::Members::forEach(t, [&](auto & member, auto & value) {
        using Member = std::remove_reference_t<decltype(member)>;
        if constexpr ( Member::index == 0 || Member::index == 2 ) {
            value++;
        }
    });
    Reflect<TestObj>::Members::forEach(t, [&](auto & member, const auto & value) {
            std::cout << member.name << ": " << value << std::endl;
    });
    Reflect<TestObj>::Members::forEach([&](auto & member, const auto & value) {
        std::cout << member.name << ": static: " << value << std::endl;
    });

    std::cout << "Total Supers: " << Reflect<TestObj>::Supers::total << std::endl;
    Reflect<TestObj>::Supers::at(0, [&](auto superInfo) {
        std::cout << RareTs::toStr<typename decltype(superInfo)::type>() << std::endl;
    });
    Reflect<TestObj>::Supers::at(1, [&](auto superInfo) {
        std::cout << RareTs::toStr<typename decltype(superInfo)::type>() << std::endl;
    });
    Reflect<TestObj>::Supers::forEach([&](auto superInfo) {
        std::cout << RareTs::toStr<typename decltype(superInfo)::type>() << std::endl;
    });

    std::cout << "contains_member_of_type: " << contains_member_of_type<TestObj, std::string>() << std::endl;

    Reflect<TestObj>::Members::pack([&](auto && ...ts) {
        (std::cout << ... << RareTs::toStr<typename std::remove_reference_t<decltype(ts)>::type>()) << std::endl;
    });
    Reflect<TestObj>::Values::pack([&](auto & ...ts) {
        (std::cout << ... << RareTs::toStr<std::decay_t<decltype(ts)>>()) << std::endl;
    });

    std::cout << "isStatic: " << (Reflect<TestObj>::Members::Member<0>::isStatic ? "true" : "false") << std::endl;
    Reflect<TestObj>::Values::pack([&](auto && ... ts) {
        (std::cout << ... << RareTs::toStr<std::decay_t<decltype(ts)>>()) << std::endl;
    });

    using TypeTup = decltype(tie(t));
    std::cout << RareTs::toStr<TypeTup>() << std::endl;

    //for ( const auto & m : Reflect<TestObj>::Experimental::Members{t} ) {
    //    std::cout << m.typeStr << " " << m.name << " {" << m << "}" << std::endl;
    //}

    std::cout << Reflect<TestObj>::Members::filteredCount<Filter::IsFunction>() << std::endl;

    Reflect<TestObj>::Members::forEach(t, [](auto & member, auto & value) {
        using Member = std::remove_reference_t<decltype(member)>;
        if constexpr ( Member::index == 0 || Member::index == 2 ) {
            value++;
        }
    });

    Reflect<TestObj>::Members::forEach(t, [](auto & member, auto & value) {
        std::cout << member.name << ": " << value << std::endl;
    });
    
    RareTs::forIndex<4>(0, [&](auto && I) {
        std::cout << "IntegralConst: " << std::decay_t<decltype(I)>::value << std::endl;
    });
    Reflect<TestObj>::Members::at(0, t, [&](auto & member, auto & value) {
        std::cout << "Zero: " << member.name << ": " << value << std::endl;
    });


    for ( size_t i=0; i<Reflect<TestObj>::Members::total; i++ ) {
        Reflect<TestObj>::Members::at(i, t, [&](auto & member, auto & value) {
            std::cout << i << ": " << member.name << ": " << value << std::endl;
        });
    }
    std::cout << t.a << std::endl;

    RareTs::forIndex<126>(112, [](auto i){
        std::cout << i.value << std::endl;
    });
    Reflect<TestObj>::Notes::forEachNote([](auto & annotation) {
        std::cout << RareTs::toStr<decltype(annotation)>() << std::endl;
    });
    Reflect<TestObj>::Values::at(1, t, [&](auto & value) {
        std::cout << "ValueAt: " << value << std::endl;
    });
    std::cout << Reflect<TestObj>::Members::template filteredCount<Filter::IsInstanceData>();
    
    std::cout << "typeStr: " << Reflect<TestObj>::Members::member<0>.typeStr << std::endl;
    std::cout << "name: " << Reflect<TestObj>::Members::member<0>.name << std::endl;
    //Reflect<TestObj>::Experimental::Members{t}[0]([](auto & value){ std::cout << value << std::endl; });
    Reflect<TestObj>::Values::at(0, t, [](auto & value) { std::cout << value << std::endl; });

    RareTs::forIndexes<Reflect<TestObj>::Members::total>([&](auto I) {
        std::cout << Reflect<TestObj>::Members::Member<I>::isStatic << std::endl;
    });
}
