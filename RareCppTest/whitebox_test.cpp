#include <rarecpp/reflect.h>
#include <gtest/gtest.h>

class SimpleObj
{
    int a;
    int b;

public:
    REFLECT(SimpleObj, a, b)

    SimpleObj(int a, int b) : a(a), b(b) {}
    
    inline int getA() { return a; }
    inline int getB() { return b; }
};

TEST(WhiteboxTest, SimpleReferenceObj)
{
    auto simpleObj = SimpleObj{3, 4};
    auto ref = RareTs::whitebox(simpleObj);
    EXPECT_EQ(ref.a, simpleObj.getA());
    EXPECT_EQ(ref.b, simpleObj.getB());

    ref.a = 5;
    ref.b = 6;
    EXPECT_EQ(5, simpleObj.getA());
    EXPECT_EQ(6, simpleObj.getB());
}

struct Involved
{
    int a;

    int b;

    inline static int c = 0;

    inline void d() {}

    inline static void e() {}

    NOTE(f)
    inline void f(int) {}
    inline void f(float) {}

public:
    REFLECT(Involved, a, b, c, d, e, f)
};

TEST(WhiteboxTest, TestExclusions)
{
    auto involved = Involved{3, 4};
    auto ref = RareTs::whitebox(involved);
    EXPECT_EQ(ref.a, involved.a);
    EXPECT_EQ(ref.b, involved.b);
    EXPECT_EQ(ref.c, involved.c);
    ref.a = 5;
    ref.b = 6;
    ref.c = 7;
    EXPECT_EQ(ref.a, involved.a);
    EXPECT_EQ(ref.b, involved.b);
    EXPECT_EQ(ref.c, involved.c);
    involved.a = 8;
    involved.b = 9;
    involved.c = 10;
    EXPECT_EQ(ref.a, involved.a);
    EXPECT_EQ(ref.b, involved.b);
    EXPECT_EQ(ref.c, involved.c);
    //referenceObj.d; // Not referencable members/causes compiler errors
    //referenceObj.e;
    //referenceObj.f;
}
