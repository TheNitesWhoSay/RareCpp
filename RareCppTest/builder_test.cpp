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
};

TEST(BuilderTest, SimpleBuilder)
{
    auto simpleObj = RareBuilder<SimpleObj>().a(1).b(2).build();
    bool isSame = std::is_same_v<SimpleObj, decltype(simpleObj)>;
    EXPECT_TRUE(isSame);

    auto ref = RareTs::whitebox(simpleObj);
    EXPECT_EQ(1, ref.a);
    EXPECT_EQ(2, ref.b);
}

TEST(BuilderTest, ReverseBuilder)
{
    auto simpleObj = RareBuilder<SimpleObj>().b(2).a(1).build();
    bool isSame = std::is_same_v<SimpleObj, decltype(simpleObj)>;
    EXPECT_TRUE(isSame);

    auto ref = RareTs::whitebox(simpleObj);
    EXPECT_EQ(1, ref.a);
    EXPECT_EQ(2, ref.b);
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
};

TEST(BuilderTest, AggregateBuilder)
{
    auto myObj = RareBuilder<MyObj>().a(1337).agg({2, 3.3f, "4"}).build();

    auto ref = RareTs::whitebox(myObj);
    EXPECT_EQ(1337, ref.a);
    EXPECT_EQ(2, ref.agg.a);
    EXPECT_EQ(3.3f, ref.agg.b);
    EXPECT_STREQ("4", ref.agg.c.c_str());
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
};

TEST(BuilderTest, BoolValidators)
{
    bool threwException = false;
    try {
        RareBuilder<AlwaysInvalid>().a(0).b(1).build();
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);
    
    threwException = false;
    try {
        auto alwaysValid = RareBuilder<AlwaysValid>().a(0).b(1).build();
        auto ref = RareTs::whitebox(alwaysValid);
        EXPECT_EQ(0, ref.a);
        EXPECT_EQ(1, ref.b);
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_FALSE(threwException);

    threwException = false;
    try {
        RareBuilder<NonNegativeValidator>().a(-1).b(1).build();
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        RareBuilder<NonNegativeValidator>().a(1).b(-1).build();
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        RareBuilder<NonNegativeValidator>().a(-1).b(-1).build();
    } catch ( std::logic_error & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        auto nonNeg = RareBuilder<NonNegativeValidator>().a(1).b(1).build();
        auto ref = RareTs::whitebox(nonNeg);
        EXPECT_EQ(1, ref.a);
        EXPECT_EQ(1, ref.b);
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
};

TEST(BuilderTest, ExceptValidators)
{
    bool threwException = false;
    try {
        RareBuilder<AlwaysInvalidExcept>().a(0).b(1).build();
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);
    
    threwException = false;
    try {
        auto alwaysValid = RareBuilder<AlwaysValidExcept>().a(0).b(1).build();
        auto ref = RareTs::whitebox(alwaysValid);
        EXPECT_EQ(0, ref.a);
        EXPECT_EQ(1, ref.b);
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_FALSE(threwException);

    threwException = false;
    try {
        RareBuilder<NonNegativeValidatorExcept>().a(-1).b(1).build();
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        RareBuilder<NonNegativeValidatorExcept>().a(1).b(-1).build();
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        RareBuilder<NonNegativeValidatorExcept>().a(-1).b(-1).build();
    } catch ( std::exception & ) {
        threwException = true;
    }
    EXPECT_TRUE(threwException);

    threwException = false;
    try {
        auto nonNeg = RareBuilder<NonNegativeValidatorExcept>().a(1).b(1).build();
        auto ref = RareTs::whitebox(nonNeg);
        EXPECT_EQ(1, ref.a);
        EXPECT_EQ(1, ref.b);
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
    auto exclusions = RareBuilder<Exclusions>()/*.c(1)*/.a(1).b(2).build(); // Fields like c, d, e, f, etc. are not available for use in builders
    EXPECT_EQ(1, exclusions.a);
    EXPECT_EQ(2, exclusions.b);
}
