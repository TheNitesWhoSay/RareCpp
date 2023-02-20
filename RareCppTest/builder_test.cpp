#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <exception>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

class SimpleObj
{
    int a;
    int b;

public:
    REFLECT(SimpleObj, a, b)
    
    inline int getA() { return a; }
    inline int getB() { return b; }
};

TEST(BuilderTest, SimpleBuilder)
{
    auto simpleObj = RareTs::builder<SimpleObj>().a(1).b(2).build();
    bool isSame = std::is_same_v<SimpleObj, decltype(simpleObj)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, simpleObj.getA());
    EXPECT_EQ(2, simpleObj.getB());
}

TEST(BuilderTest, ReverseBuilder)
{
    auto simpleObj = RareTs::builder<SimpleObj>().b(2).a(1).build();
    bool isSame = std::is_same_v<SimpleObj, decltype(simpleObj)>;
    EXPECT_TRUE(isSame);
    EXPECT_EQ(1, simpleObj.getA());
    EXPECT_EQ(2, simpleObj.getB());
}

struct Aggregate
{
    int a;
    float b;
    std::string c;
};

class MyObj
{
    int a;
    Aggregate agg;

public:
    REFLECT(MyObj, a, agg)

    inline auto getA() { return a; }
    inline auto getAgg() { return agg; }
};

TEST(BuilderTest, AggregateBuilder)
{
    auto myObj = RareTs::builder<MyObj>().a(1337).agg({2, 3.3f, "4"}).build();
    
    EXPECT_EQ(1337, myObj.getA());
    EXPECT_EQ(2, myObj.getAgg().a);
    EXPECT_EQ(3.3f, myObj.getAgg().b);
    EXPECT_STREQ("4", myObj.getAgg().c.c_str());
}

NOTE(AlwaysInvalid, RareTs::ValidatedBuilder)
class AlwaysInvalid
{
    int a;
    int b;
    
public:
    REFLECT_NOTED(AlwaysInvalid, a, b)

    bool validate()
    {
        return !(a < std::numeric_limits<int>::max() && b < std::numeric_limits<int>::max()); // always false in these tests
    }
};

NOTE(AlwaysValid, RareTs::ValidatedBuilder)
class AlwaysValid
{
    int a;
    int b;
    
public:
    REFLECT_NOTED(AlwaysValid, a, b)

    bool validate()
    {
        return true;
    }
    inline int getA() { return a; }
    inline int getB() { return b; }
};

NOTE(NonNegativeValidator, RareTs::ValidatedBuilder)
class NonNegativeValidator
{
    int a;
    int b;

public:
    REFLECT_NOTED(NonNegativeValidator, a, b)

    bool validate()
    {
        return a >= 0 && b >= 0;
    }
    inline int getA() { return a; }
    inline int getB() { return b; }
};

TEST(BuilderTest, BoolValidators)
{
    bool threwException = false;
    try {
        RareTs::builder<AlwaysInvalid>().a(0).b(1).build();
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);
    
    threwException = false;
    try {
        auto alwaysValid = RareTs::builder<AlwaysValid>().a(0).b(1).build();
        EXPECT_EQ(0, alwaysValid.getA());
        EXPECT_EQ(1, alwaysValid.getB());
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_FALSE(threwException);

    threwException = false;
    try {
        RareTs::builder<NonNegativeValidator>().a(-1).b(1).build();
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        RareTs::builder<NonNegativeValidator>().a(1).b(-1).build();
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        RareTs::builder<NonNegativeValidator>().a(-1).b(-1).build();
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        auto nonNeg = RareTs::builder<NonNegativeValidator>().a(1).b(1).build();
        EXPECT_EQ(1, nonNeg.getA());
        EXPECT_EQ(1, nonNeg.getB());
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_FALSE(threwException);
}

NOTE(AlwaysInvalidExcept, RareTs::ValidatedBuilder)
class AlwaysInvalidExcept
{
    int a;
    int b;

public:
    REFLECT_NOTED(AlwaysInvalidExcept, a, b)

    void validate()
    {
        if ( a < std::numeric_limits<int>::max() && b < std::numeric_limits<int>::max() ) // always true in these tests
            throw std::exception();
    }
};

NOTE(AlwaysValidExcept, RareTs::ValidatedBuilder)
class AlwaysValidExcept
{
    int a;
    int b;
    
public:
    REFLECT_NOTED(AlwaysValidExcept, a, b)

    void validate()
    {

    }
    inline int getA() { return a; }
    inline int getB() { return b; }
};

NOTE(NonNegativeValidatorExcept, RareTs::ValidatedBuilder)
class NonNegativeValidatorExcept
{
    int a;
    int b;
    
public:
    REFLECT_NOTED(NonNegativeValidatorExcept, a, b)

    void validate()
    {
        if ( a < 0 || b < 0 )
            throw std::exception();
    }
    inline int getA() { return a; }
    inline int getB() { return b; }
};

TEST(BuilderTest, ExceptValidators)
{
    bool threwException = false;
    try {
        RareTs::builder<AlwaysInvalidExcept>().a(0).b(1).build();
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);
    
    threwException = false;
    try {
        auto alwaysValid = RareTs::builder<AlwaysValidExcept>().a(0).b(1).build();
        EXPECT_EQ(0, alwaysValid.getA());
        EXPECT_EQ(1, alwaysValid.getB());
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_FALSE(threwException);

    threwException = false;
    try {
        RareTs::builder<NonNegativeValidatorExcept>().a(-1).b(1).build();
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        RareTs::builder<NonNegativeValidatorExcept>().a(1).b(-1).build();
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        RareTs::builder<NonNegativeValidatorExcept>().a(-1).b(-1).build();
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        auto nonNeg = RareTs::builder<NonNegativeValidatorExcept>().a(1).b(1).build();
        EXPECT_EQ(1, nonNeg.getA());
        EXPECT_EQ(1, nonNeg.getB());
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_FALSE(threwException);
}

struct Exclusions
{
    int a;

    int b;

    inline static int c = 0; // Not an instance member

    NOTE(d, RareTs::BuilderIgnore)
    int d = 0; // Explicitly ignored using note

    inline void e() {} // Not a data member

    inline static void f() {} // Neither an instance nor a data member

    REFLECT(Exclusions, a, b, c, d, e, f)
};

TEST(BuilderTest, TestExclusions)
{
    auto exclusions = RareTs::builder<Exclusions>()/*.c(1)*/.a(1).b(2).build(); // Fields like c, d, e, f, etc. are not available for use in builders
    EXPECT_EQ(1, exclusions.a);
    EXPECT_EQ(2, exclusions.b);
}
