# RareCpp
***C++** **R**andom **A**ccess **R**eflection & **E**xtensions: adding a simple, intuitive means of reflection to C++ and leveraging it to create powerful extensions.*

To include reflection in your project you simply need to copy the [Reflect.h](https://github.com/TheNitesWhoSay/RareCpp/blob/master/RareCppLib/Reflect.h) file into your project and (optionally) use the Reflect namespace; then within the class(es) you're looking to reflect you place the REFLECT macro.

```C++
#include "Reflect.h"
using namespace Reflect;

class FuelTank {
public:
    float capacity;
    float currentLevel;
    float tickMarks[2];

    REFLECT(FuelTank, capacity, currentLevel, tickMarks)
};
```

The REFLECT macro takes the class type, then between 1 and 125 fields.



With reflection you can use and write very powerful code to automatically handle complex tasks, for instance, conversion to and from JSON
```C++
FuelTank fuelTank;
std::cin >> Json::in(fuelTank);
std::cout << Json::pretty(fuelTank) << std::endl;
```
```JSON
{"capacity":15.0,"tickMarks":[1.0,7.5]}
{
  "capacity": 15,
  "currentLevel": 0,
  "tickMarks": [ 1, 7.5 ]
}
```


## Usage

Following the setup of the REFLECT macro, you can easily loop over the fields of a reflected class...
```C++
for ( size_t i=0; i<Wheel::Class::TotalFields; i++ )
{
    Wheel::Class::FieldAt(frontLeft, i, [&](auto & field, auto & value) {
        std::cout << field.name << ": " << value << std::endl;
    });
}
```

Though whenever you're trying to access every field in an object, the ForEachField method is preferrable:
```C++
Wheel::Class::ForEachField(backRight, [&](auto & field, auto & value) {
    std::cout << field.name << ": " << value << std::endl;
});
```

More complex structures involving pointers, iterable values, any maybe even fields that are in turn reflected require more complex code to handle, but all of these can effectively be traversed using this reflection library.

If you are iterating more complex structures you're likely to run into problems where the compiler generates code for one field that is inapplicable for another field.

```C++
class FuelTank {
public:
    float capacity;
    float currentLevel;
    float tickMarks[2];

    REFLECT(FuelTank, capacity, currentLevel, tickMarks)
};

FuelTank::Class::ForEachField(fuelTank, [&](auto & field, auto & value) {
    std::cout << field.name << ": " << value[0] << std::endl;
});
```

The above will cause a compiler error as not every field in FuelTank is an array, meaning the array access "value[0]" will not be valid code on those fields. Using constexpr ifs as well as code from the ExtendedTypeSupport namespace (if not from the standard library type support methods https://en.cppreference.com/w/cpp/types ) included in Reflect.h you can circumvent such problems.
```C++
FuelTank::Class::ForEachField(fuelTank, [&](auto & field, auto & value) {
    using Type = typename std::remove_reference<decltype(value)>::type;
    if constexpr ( ExtendedTypeSupport::is_static_array<Type>::value )
        std::cout << field.name << ": " << value[0] << std::endl;
    else
        std::cout << field.name << ": " << value << std::endl;
});
```

ExtendedTypeSupport defines many useful interfaces for these purposes...
- pair_lhs\<T\>::type // Gets the type of the left-hand value in an std::pair
- pair_rhs\<T\>::type // Gets the type of the right-hand value in an std::pair
- element_type\<T\>::type // Gets the type of the element contained in some iterable (be it a static_array, or STL container)
- is_pointable\<T\>::value // Checks whether a type is a regular or smart pointer
- remove_pointer\<T\>::type // Gets the type pointed to by a regular or smart pointer type
- static_array_size\<T\>::value // Gets the size of a static array, which may be a basic C++ array or the STL std::array type
- is_static_array\<T\>::value // Checks whether a type is a static array
- is_iterable\<T\>::value // Checks whether a type can be iterated, meaning it's a static array or other STL container
- is_map\<T\>::value // Checks whether a type is a map (std::map, std::unordered_map, std::multimap, std::unordered_multimap)
- is_stl_iterable\<T\>::value // Checks whether a type can be iterated with begin()/end()
- is_adaptor\<T\>::value // Checks whether a type is an STL adaptor (std::stack, std::queue, std::priority_queue)
- is_forward_list\<T\>::value // Checks whether a type is a forward list
- is_pair\<T\>::value // Checks whether a type is a pair
- is_bool\<T\>::value // Checks whether a type is a bool
- is_string\<T\>::value // Checks whether a type is a std::string
- has_push_back\<T\>::value // Checks whether a type is an STL container with a push_back method
- has_push\<T\>::value // Checks whether a type is an STL container with a push method
- has_insert\<T\>::value // Checks whether a type is an STL container with an insert method
- has_clear\<T\>::value // Checks whether a type is an STL container with a clear method

Extended type support also provides several other pieces of functionality including... 
- The Append, Clear, and IsEmpty methods that work on all stl containers
- The has_type interface to check whether a type is included in a list of types
- A get_underlying_container method to retrieve a const version of the underlying container for an STL adaptor
- An interface "get\<T\>::from(tuple)" to get the first instance of type T from a tuple
- An interface "for_each\<T\>::in(tuple)" to iterate each tuple element of type T
- An interface "for_each_in(tuple)" to iterate all tuple elements
- A TypeToStr method to retrieve a string representation of a type


See [Json.h](https://github.com/TheNitesWhoSay/RareCpp/blob/master/RareCppLib/Json.h) for where all of this gets put together to traverse fairly complex objects - in the case of JSON serialization and deserialization is split into three main methods: get[Object]/put[Object], getIterable/putIterable, and getValue/putValue.


## Field
Information provided to REFLECT is used to generate meta-data about your classes fields in the form of "Field" objects stored in a nested-class of your object titled "Class", Field objects come in two flavors but both share two members:

1. name
2. typeStr

The enhanced flavor of Field provides several additional members:

3. Type (the actual type of the field)
4. Index (the index of the field, first field is zero, second is one, and so on)
5. IsStatic (boolean indicating whether the field is static)
6. IsFunction (boolean indicating whether the field is a member or static function)
7. HasOffset (boolean indicating whether the field has an offset)
8. getOffset() (returns the offset of the field as from offsetof if the field has an offset, 0 otherwise)
9. p (a pointer to the field, nullptr if the field is a reference)
10. Pointer (the type of a pointer to the field)
11. annotations (the value of the annotations attached to the field)
12. Annotations (the type of the annotations attached to the field)
13. HasAnnotation (template boolean indicating whether the field has a given annotation)
14. getAnnotation<T> (template function getting the first instance of a given annotation)
15. forEach<T> (lambda you can use to iterate all instances of a given annotation)
16. forEachAnnotation (lambda you can use to iterate all annotations)
	


## Class
As stated, Class is a nested-class of the class you're trying to reflect; Class has the following static data members:

1. TotalFields
2. TotalStaticFields
3. Fields[TotalFields] // Simple flavor, doesn't include members like Type, or annotation details
4. annotations

Class also provides four static methods to retrieve information about fields
```C++
ForEachField(object, [&](auto & field, auto & value) {
	// Your code here
});
FieldAt(object, fieldIndex, [&](auto & field, auto & value) {
	// Your code here
});

ForEachField([&](auto & field) {
	// Your code here
});
FieldAt(fieldIndex, [&](auto & field) {
	// Your code here
});
```

"value" is a reference to the actual field in the object, which you can read or change. "field" is the enhanced flavor of Field, meaning it includes such members as Type, Index, and HasAnnotation. It's often useful to grab the type of the field and value parameters so you can more cleanly access the static members
```C++
using Field = std::remove_reference<decltype(field)>::type;
using Type = std::remove_reference<decltype(value)>::type;
```


## Annotations


Annotations are a way of giving additional information about a field that cannot be determined only using the field name and type information, for instance whether a field should be ignored or treated as a string or renamed when serializing. Annotations should be placed above the item being annotated using the NOTE macro - which takes the name of the item being annotated followed by one or more comma-separated annotations.

```C++
struct Point
{
    NOTE(latitude, Json::Name{"lat"})
    double latitude;

    NOTE(longitude, Json::Name{"long"})
    double longitude;

    REFLECT(Point, latitude, longitude)
};
```

A downstream library, such is the JSON library included in this project, or whatever code you may be writing, can define its own annotations, an annotation is quite simply anything that can be stored at compile time (basic types, custom structures with basic types, string views, etc).

At present the only annotation specific to the RareCpp reflection library is the "Super" annotation, which enables reflection of super-classes (explained more in the following section); the "Super" annotation is a class-level annotation; when using class-level annotations you have to switch the REFLECT macro to REFLECT_NOTED (or REFLECT_EMPTY if there are no reflected fields in that class), this is opposed to field-level annotations which do not need any changes.

## Super Classes

If you have super classes you can capture them in a class-level annotation like so...

```C++
NOTE(A, Super<SuperA>)
class A : public SuperA {
public:
    ...
    REFLECT_NOTED(A, ...)
};
```

You can list multiple super classes, you can also add annotations specific to the subclass-to-superclass relationship e.g. ...

```C++
NOTE(A,
    Super<SuperA>,
    Super<Another>(Json::Ignore),
    Super<OtherSuper>)
class A : public SuperA, public Another, public OtherSuper {
public:
    ...
    REFLECT_NOTED(A, ...)
};
```


## How It Works

REFLECT is nothing more than a macro that defines the class Class, it does so using a concept called macro loops, macro loops start with static macro iterations...
```C++
#define FOR_EACH_1(f,a,...) f(a)
#define FOR_EACH_2(f,a,...) f(a) EXPAND(FOR_EACH_1(f,__VA_ARGS__))
#define FOR_EACH_3(f,a,...) f(a) EXPAND(FOR_EACH_2(f,__VA_ARGS__))
```
(could be longer)

f(a) simply calls the function-like macro "f", with the argument "a".
If you called FOR_EACH_3(f, a, b, c), it would be expanded to "f(a) f(b) f(c)"

With a count of the arguments we can concatenate FOR_EACH_ and the count to invoke the correct static macro iteration.

To count we lay out all of the arguments, appended with a list of all numbers from ArgMax to 0
```
a, b, c, 3, 2, 1, 0
         ^
```
And then select the argument at ArgMax, in this case 3, if we passed two arguments the layout would have been...
```
a, b, 3, 2, 1, 0
         ^
```
If we passed one argument...
```
a, 3, 2, 1, 0
         ^
```
With the count, and call to the appropriate static macro iteration, plus a little extra macro expansion and concatenation to avoid bugs with Visual Studios, we have all the magic needed to perform a FOR_EACH loop on a set of macro arguments.

The REFLECT macro takes in the name of the class you're adding reflection to, followed by a list of fields; using the for each macro we build the class "Class" one piece at a time...

1. ClassType is set to the first argument from the reflect macro
2. An enum "IndexOf" is generated using each field name, because enums start at 0 and count up, IndexOf::fieldName provides the index of a given field in a manner statically available at compile time (this especially helps us build switches later)
3. An empty annotation "NoNote" is set to an empty tuple (this will help stage field annotations later and the class anotations in the next step)
4. An "annotations" field is set to the class level annotation (if using REFLECT_NOTED), else it's set to an empty-brace initialized NoNote
5. A description of each field is generated in "Class::fieldName_", wherein the type of the field and the type of a pointer to the field is deduced (with the help of decltype and SFINAE to default cases where either type may not be available) and where possible the pointer is obtained, the field name and type strings are extracted, any field annotations are identified and a reference to them is obtained, and all of this is packaged into a "Field" object
6. TotalFields gets set to the count of arguments, not including the class name
7. TotalStaticFields gets set to the count of static field arguments
8. A "Field" array is generated, similar to the field object defined the fifth step, but the simple flavor
9. The ForEachField methods are generated, calling the given function with the enhanced flavor of the Field and a reference to the field (if calling the instance or const instance ForEachField overloads)
10. The FieldAt methods are generated, calling the given function with the enhanced flavor of the Field and a reference to the field (if calling the instance or const instance FieldAt overloads) at the given fieldIndex

Another sub-class called "Supers" is added after this using the Class::ClassType and Class::Annotations which has similar static and instance ForEach and At methods to loop through any super-classes, as well as a "TotalSupers" member.

See [Reflect.h](https://github.com/TheNitesWhoSay/RareCpp/blob/master/RareCppLib/Reflect.h) for the full implementation of macro loops and the REFLECT macro.


## Compatibility

Works with Visual Studios, Clang, and gcc/g++ , you may need to manually set your compiler to be using at least C++17.
