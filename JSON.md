# JSON Automation Library

A library for reading and writing complex objects with minimal effort.

Any object to which the REFLECT macro has already applied can be written and read from streams as such:
```C++
std::cin >> Json::in(myObj);
std::cout << Json::out(myObj) << std::endl;
std::cout << Json::pretty(myObj) << std::endl;
```

This JSON library will automatically...
- Read or put each field and accompanying value
- Infer the appropriate JSON representation for various types
- Read or put any array, STL iterable, or nested STL iterable as a JSON array
- Read or put any map or STL iterable containg pairs, or nested maps/STL iterables as a JSON object with keys being the field names
- Read or put any nested reflected objects as nested JSON objects

There are a few annotations you can use to alter the JSON representation...
- Json::String will result in the value given by the ostream operator being quoted and escaped as a JSON string
- Json::Unstring will demote an std::string type (which by default behaves like Json::String) so that it's put/read without quotes
- Json::EnumInt can be used to force use of the integer value for an enum field that may otherwise have iostream overloads

## Enums

There are three main ways you can handle enumerations...
1. Have an enumeration with the Json::EnumInt annotation used, this will force use of the integer value regardless of ostream/istream overloads
2. Have an enumeration with ostream/istream operator overloads and the Json::String annotation used, this will create a one-to-one relationship with a string value for output, and a one-to-one or one-to-many relationship with a string value for input
3. Have an enumeration with the Json::Enum annotation used, and overload the EnumString To/From methods for your field, this allows you to customize the string representation based on the state of the object and which specific field the enum belongs to - this is especially useful when you have multiple enum strings that share the same value (perhaps because the meaning of the enum value changes based on some type field, or because one string makes more grammatical sense than another in different contexts.

(1.)
```C++
class A {
public:
    enum class TestEnum : u8 {
        first,
        second
    };

    A() : testEnum(TestEnum::first) { }

    TestEnum testEnum;

    REFLECT(() A, (Json::EnumInt) testEnum)
};
```

(2.)
```C++
class A {
public:
    enum class TestEnum : u8 {
        first,
        second
    };

    A() : testEnum(TestEnum::first) { }

    TestEnum testEnum;

    REFLECT(() A, (Json::String) testEnum)
        
    static const std::unordered_map<std::string, TestEnum> TestEnumCache;
};

const std::unordered_map<std::string, A::TestEnum> A::TestEnumCache = {
    { "first", A::TestEnum::first },
    { "second", A::TestEnum::second }
};

std::ostream & operator<<(std::ostream & os, const A::TestEnum & testEnum)
{
    switch ( testEnum )
    {
        case A::TestEnum::first: os << "first"; break;
        case A::TestEnum::second: os << "second"; break;
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
```

(3.)
```C++
class A {
public:
    enum class TestEnum : u8 {
        first,
        second
    };

    A() : alternate(false), testEnum(TestEnum::first) { }

    bool alternate;
    TestEnum testEnum;

    REFLECT(() A, () alternate, (Json::Enum) testEnum)
        
    static const std::unordered_map<std::string, TestEnum> TestEnumCache;
    static const std::unordered_map<std::string, TestEnum> AltTestEnumCache;
};

const std::unordered_map<std::string, A::TestEnum> A::TestEnumCache = {
    { "first", A::TestEnum::first },
    { "second", A::TestEnum::second }
};

const std::unordered_map<std::string, A::TestEnum> A::AltTestEnumCache = {
    { "one", A::TestEnum::first },
    { "two", A::TestEnum::second }
};

bool Json::EnumString<A, A::TestEnum, A::Class::IndexOf::testEnum>::From(
    const std::string input, const A & object, A::TestEnum & value)
{
    if ( object.alternate )
    {
        auto found = A::AltTestEnumCache.find(input);
        if ( found != A::AltTestEnumCache.end() )
        {
            value = found->second;
            return true;
        }
    }
    else
    {
        auto found = A::TestEnumCache.find(input);
        if ( found != A::TestEnumCache.end() )
        {
            value = found->second;
            return true;
        }
    }
    return false;
}

std::string Json::EnumString<A, A::TestEnum, A::Class::IndexOf::testEnum>::To(
    const A & object, const A::TestEnum & value)
{
    if ( object.alternate )
    {
        switch ( value )
        {
            case A::TestEnum::first: return "one";
            case A::TestEnum::second: return "two";
        }
    }
    else
    {
        switch ( value )
        {
            case A::TestEnum::first: return "first";
            case A::TestEnum::second: return "second";
        }
    }
    return "";
}
```
