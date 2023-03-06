#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct UnownedEncapsulator
{
public:
    UnownedEncapsulator(int a) : a(a) {}
    int getA() const { return a; }
    void setA(int newA) { this->a = newA; }

private:
    int a;
};

struct ProxiedObjectMapping
{
    int a = 0;
};

struct NoteMapping
{
    int b = 0;
    int c = 0;

    REFLECT(NoteMapping, b, c)
};

struct SpecializationMapping
{
    int a = 0;
};

struct OwnedObject1
{
    int a = 0;
    int c = 0;
    REFLECT(OwnedObject1, a, c)

    void map_to(UnownedEncapsulator & o) const { o.setA(this->a); }
    void map_from(const UnownedEncapsulator & o) { this->a = o.getA(); }

    NOTE(ObjectMappings, RareMapper::createMapping<OwnedObject1, NoteMapping>().a->b().c->c().bidirectional())
};

template <>
void RareMapper::map(SpecializationMapping & to, const OwnedObject1 & from)
{
    to.a = from.a;
}

struct OwnedObject2
{
    int a = 0;
    REFLECT(OwnedObject2, a)
};

template <> struct RareTs::Proxy<ProxiedObjectMapping> : public ProxiedObjectMapping
{
    REFLECT(RareTs::Proxy<ProxiedObjectMapping>, a)
};

struct UnownedObjDao
{
    int a;
    int b;
    int c;
    std::map<int, int> d;
    std::vector<OwnedObject2> e;

    UnownedObjDao() : a(0), b(0), c(0), d({}), e({}) {}
};

template <> struct RareTs::Proxy<UnownedObjDao> : public UnownedObjDao
{
    REFLECT(RareTs::Proxy<UnownedObjDao>, a, b, c, d, e)
};

struct OwnedObjectModel
{
    int a;
    int b;
    std::map<int, int> c;
    std::vector<OwnedObject1> d;

    REFLECT(OwnedObjectModel, a, b, c, d)

    NOTE(ObjectMappings, RareMapper::createMapping<OwnedObjectModel, UnownedObjDao>().a->a().b->b().c->d().d->e().bidirectional())
};

struct F1 {
    std::string a;
    static constexpr bool isF1 = true;
    REFLECT(F1, isF1, a)
};

struct F2 { std::string a; REFLECT(F2, a) };
SET_DEFAULT_OBJECT_MAPPING(F2, F1)

NOTE(F3, RareMapper::MappedBy<F1>)
struct F3 { std::string a; REFLECT_NOTED(F3, a) };

struct F4 { std::string a; REFLECT(F4, a) };

struct F5 { std::string a; REFLECT(F5, a) };

struct F6 { std::string a; REFLECT(F6, a) };

struct MappedByTest
{
    F1 f1; // Serialization-friendly
    
    F2 f2; // Mapped to F1 by RareMapper tags

    F3 f3; // Mapped to F1 by class-level annotation

    NOTE(f4, RareMapper::MappedBy<F1>)
    F4 f4; // Mapped to F1 by member-level annotation

    F5 f5; // No fixed mapping to F1, will use op-level annotation

    F6 f6; // No mapping

    REFLECT(MappedByTest, f1, f2, f3, f4, f5, f6)
};

void objectMapperExamples()
{
    OwnedObjectModel objModel { 1, 2, {{3, 4}, {5, 6}}, {{7}} };
    UnownedObjDao objDao {};

    RareMapper::map(objDao, objModel);
    std::cout << "objDao: { " << objDao.a << ", " << objDao.b << ", " << objDao.c << ", " << objDao.e[0].a << " }" << std::endl;

    for ( auto & pair : objDao.d )
        std::cout << "objDao.d[]: { " << pair.first << ", " << pair.second << " }" << std::endl;

    OwnedObject1 ownedObject { 1337, 9001 };

    // When you can't add add REFLECT to an object, you can proxy reflect public (or protected) members to the same effect
    ProxiedObjectMapping unownedObject1 = RareMapper::map<ProxiedObjectMapping>(ownedObject);
    std::cout << "unownedObject1: { " << unownedObject1.a << " }" << std::endl;

    // Or use NOTE(ObjectMappings, RareMapper::createMapping<L, R> ... )
    NoteMapping unownedObject2 = RareMapper::map<NoteMapping>(ownedObject);
    std::cout << "unownedObject2: { " << unownedObject2.b << ", " << unownedObject2.c << " }" << std::endl;

    // Or use specializations of RareMapper::map...
    SpecializationMapping unownedObject3 = RareMapper::map<SpecializationMapping>(ownedObject);
    std::cout << "unownedObject3: { " << unownedObject3.a << " }" << std::endl;

    // Or define map_to/map_from methods within a class/struct you do control
    UnownedEncapsulator unownedEncapsulator(5);
    RareMapper::map<UnownedEncapsulator>(unownedEncapsulator, ownedObject);
    std::cout << "unownedEncapsulator: { " << unownedEncapsulator.getA() << " }" << std::endl;
    
    MappedByTest mappedByTest = { {"f1.a"}, {"f2.a"}, {"f3.a"}, {"f4.a"}, {"f5.a"}, {"f6.a"} };
    std::cout << Json::out<Json::Statics::Included>(mappedByTest) << std::endl;
    std::cout << Json::out<Json::Statics::Included, Json::OpNotes<RareMapper::UseMapping<F5, F1>>>(mappedByTest) << std::endl;
}
