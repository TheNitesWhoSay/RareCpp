# Rare JSON

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
- Ignore any unknown fields in an object or automatically read them into a Json::FieldCluster if present

There are a few annotations you can use to alter the JSON representation...
- Json::Ignore will cause a field to be ignored by json I/O, on input
- Json::String will result in the value given by the ostream operator being quoted and escaped as a JSON string
- Json::Unstring will demote an std::string type (which by default behaves like Json::String) so that it's put/read without quotes
- Json::EnumInt can be used to force use of the integer value for an enum field that may otherwise have iostream overloads


## Usage

To use simply copy the Reflect.h and Json.h files into your project, then put the line ```ENABLE_JSON;``` near the top of one of your cpp files where Json.h is included; then REFLECT objects as needed and enjoy the awsome power of JSON!

```
struct MyObject
{
    int myInt;
    std::string myString;
    std::vector<int> myIntCollection;

    REFLECT(() MyObject, () myInt, () myString, () myIntCollection)
};

int main()
{
    MyObject myObject = {};
    std::cout << "Enter MyObject:" << std::endl;
    std::cin >> Json::in(myObject);
    std::cout << std::endl << std::endl << "You entered:" << std::endl;
    std::cout << Json::pretty(myObject);
}
```

```
Enter MyObject:
{ "myInt": 1337, "myString": "stringy", "myIntCollection": [2,4,6] }

You entered:
{
  "myInt": 1337,
  "myString": "stringy",
  "myIntCollection": [ 2, 4, 6 ]
}
```



## FieldClusters & Runtime JSON

While the primary focus of this library is dealing with objects and JSON structures known at compile time, it's not unheard of to have JSON structures that you'll only come to know at runtime, it's perhaps even more common to have a structure that you know at compile time but which includes many fields that you don't need to use except to receive them, perhaps audit them, and pass them along. This is where this libraries JSON generics can come in handy.

A Json::FieldCluster is for the latter case, where you know and use some fields in your program, but others you won't need to reference explicitly. To maximize performance and avoid misuse of field clusters, they may not be the first, nor may they be the only field in an object.

```
struct SomeStructure
{
    int usefulField;
    Json::FieldCluster fieldCluster;

    REFLECT(() RegularFields, () usefulField, () fieldCluster)
};
```

Now if you read in something like...
```
{
  "usefulField": 222,
  "seedValue": "0x12A39B",
  "sequence": 5
}
```

The "seedValue" and "sequence" values would be automatically stored in the fieldCluster and will be streamed as if you had explicitly declared and reflected those fields, and can also, if for some reason necessary, be traversed programatically.

You can also take in an entirely unknown object using Json::Object, e.g.
```
Json::Object obj;
std::cin >> Json::in(obj);
```

All Generic types extend from Json::Value, FieldCluster extends Json::Object but has special I/O behavior and its own type. The following are the generic types...
- Json::Value (pure virtual class, check the real type with the type() method)

- Json::Bool (type: Json::Value::Type::Boolean)
- Json::Number (type: Json::Value::Type::Number)
- Json::String (type: Json::Value::Type::String)
- Json::Object (type: Json::Value::Type::Object)

- Json::NullArray (type: Json::Value::Type::NullArray)
- Json::BoolArray (type: Json::Value::Type::BoolArray)
- Json::NumberArray (type: Json::Value::Type::NumberArray
- Json::StringArray (type: Json::Value::Type::StringArray)
- Json::ObjectArray (type: Json::Value::Type::ObjectArray)
- Json::MixedArray (type: Json::Value::Type::MixedArray)

- Json::FieldCluster (type: Json::Value::Type::FieldCluster)

The null, bool, string, and object arrays are optimized for the type of contents they contain, whereas the elements in MixedArray are all shared pointers to Json::Values. Any type that contains nested arrays are automatically a MixedArray.

Json::Value has virtual methods for getting the contents of any given type, e.g. for a Json::Bool call the boolean() method to get the stored value, if it's not the correct type for that method a TypeMismatch exception is thrown, check the type of a Json::Value in advance to avoid that.


## Customizers

In addition to the annotations, you can further customize behavior for specific types, or specific fields within a type. This is accomplished using partial template specialization on one of the following structs...
- Json::Output::Customize<Object, Value, ... [optional parameters]>
- Json::Output::CustomizeType<Value, ... [optional parameters]>
- Json::Input::Customize<Object, Value, ... [optional parameters]>
- Json::Input::CustomizeType<Value, ... [optional parameters]>

You can use the optional parmeters to further specialize if say, you only wanted the customizer to only apply to a specific field index. The optional parameters can also help should you need to know additional details - such as any annotations applied to the field, whether pretty print is active, the current indentation level, etc. Customizers should usually be written in header (".h", or ".hpp") files, as you will be using template arguments which will be very limited if in a ".cpp" file.

If you had a ```class A``` with a field ```uint16_t customInt;``` you might write a customizer as follows...

```template <>
struct Json::Output::Customize<A, uint16_t, A::Class::IndexOf::customInt>
{
    static bool As(Json::OutStreamType & os, Context & context, const A & object, const uint16_t & value)
    {
        if ( value > 9000 )
            Json::Put::String(os, "Over 9000!");

        return value > 9000;
    }
};
```

This particular customizer will only run for the customInt field as the FieldIndex was supplied (via A::Class::IndexOf::customInt), if the integer was over 9000 then it will put the string "Over 9000!" in the json for this field, else it will return false - returning false triggers the default behavior for the field (which will put the number field into the json), returning true says you do not wish to continue on to the default behavior for a given type.

To help with customization you can add an arbitrary "Context" object to the operation, a context object is any struct or class that has Json::Context in its inheritance heirarchy, via this object you can supply additional information that you might need, such as an object that knows what string value certain integers should take on, or whether or not to convert everything to lowercase or uppercase, and so on.

```
struct EnhancedContext : public Json::Context
{
    virtual ~EnhancedContext() {}
    EnhancedContext(int enhanced) : enhanced(enhanced) {}
    static std::shared_ptr<EnhancedContext> Make(int enhanced) {
        return std::shared_ptr<EnhancedContext>(new EnhancedContext(enhanced));
    }

    int enhanced;
};

template <>
struct Json::Output::Customize<A, A::TestEnum>
{
    static bool As(Json::OutStreamType & os, Context & context, const A & object, const A::TestEnum & value)
    {
        try {
            EnhancedContext & enhanced = dynamic_cast<EnhancedContext &>(context);
            switch ( value )
            {
                case A::TestEnum::first:
                    Json::Put::String(os, "firstCustom" + std::to_string(enhanced.enhanced));
                    return true;
                case A::TestEnum::second:
                    Json::Put::String(os, "secondCustom" + std::to_string(enhanced.enhanced));
                    return true;
            }
            return true;
        } catch ( std::bad_cast & ) {
            return false;
        }
    }
};


std::cout << Json::pretty(a, EnhancedContext::Make(1337)) << std::endl;
```


## Enums

There are three main ways you can handle enumerations...
1. Have an enumeration with the Json::EnumInt annotation used, this will force use of the integer value regardless of ostream/istream overloads
2. Have an enumeration with ostream/istream operator overloads and the Json::String annotation used, this will create a one-to-one relationship with a string value for output, and a one-to-one or one-to-many relationship with a string value for input
3. Have an enumeration with the Json::Enum annotation used, and add a customizer for your enumeration, this allows you to customize the string representation based on the state of the object and which specific field the enum belongs to - this is especially useful when you have multiple enum strings that share the same value (perhaps because the meaning of the enum value changes based on some type field, or because one string makes more grammatical sense than another in different contexts.

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

template <>
struct Json::Input::Customize<A, A::TestEnum, A::Class::IndexOf::testEnum>
{
    static bool As(std::istream & is, Context & context, const A & object, A::TestEnum & value)
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
}

template <>
struct Json::Output::Customize<A, A::TestEnum, A::Class::IndexOf::testEnum>
{
    static bool As(Json::OutStreamType & os, Context & context, const A & object, const A::TestEnum & value)
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
}
```


## StringBuffer Performance Enhancements

Depending on your system and compiler, using StringBuffers instead of std::stringstream can result in a massive performance boost - with MSVC runtime is consistently halved or more. To use, StringBuffer.h needs to be included in your project along with Json.h, then you need to have USE_BUFFERED_STREAMS within your pre-defined macros; any output customizers will need to ensure they're using Json::OutStreamType rather than std::ostream, no other changes should be required.
