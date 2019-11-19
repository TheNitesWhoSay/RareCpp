# CppRandomAccessReflection
Creating a simpler, more intuitive means of C++ reflection.




Within the class you're looking to reflect you place the REFLECT macro.

```
class FuelTank {
public:
    float capacity;
    float currentLevel;
    float tickMarks[2];

    REFLECT(() FuelTank, () capacity, () currentLevel, () tickMarks)
};
```

The REFLECT macro takes the class parameter, then between 1 and 123 fields.

- The class parameter is of the form "(SuperClasses) ClassName", SuperClasses are optional
- The fields are all of the form "(Annotations) FieldName", Annotations are optional


## Usage

Following the setup of the REFLECT macro, you can easily loop over the fields of a reflected class...
```
for ( size_t i=0; i<Wheel::Class::TotalFields; i++ )
{
    Wheel::Class::FieldAt(frontLeft, i, [&](auto & field, auto & value) {
        std::cout << field.name << ": " << value << std::endl;
    });
}
```

Though whenever you're trying to access every field in an object, the ForEachField method is preferrable:
```
Wheel::Class::ForEachField(backRight, [&](auto & field, auto & value) {
    std::cout << field.name << ": " << value << std::endl;
});
```

More complex structures involving pointers, iterable values, any maybe even fields that are in turn reflected require more complex code to handle, but all of these can effectively be traversed using this reflection library.

If you are iterating more complex structures you're likely to run into problems where the compiler generates code for one field that is inapplicable for another field.

```
class FuelTank {
public:
    float capacity;
    float currentLevel;
    float tickMarks[2];

    REFLECT(() FuelTank, () capacity, () currentLevel, () tickMarks)
};

FuelTank::Class::ForEachField(fuelTank, [&](auto & field, auto & value) {
    if ( field.isIterable ) {
        std::cout << field.name << ": " << value[0] << std::endl;
    }
});
```

The above will cause a compiler error as not every field in FuelTank is an array, meaning the array access "value[0]" will not be valid code on those fields. Using constexpr ifs as well as code from the ExtendedTypeSupport namespace (if not from the standard library type support methods https://en.cppreference.com/w/cpp/types ) included in Reflect.h you can circumvent such problems.
```
FuelTank::Class::ForEachField(fuelTank, [&](auto & field, auto & value) {
    using Type = std::remove_reference<decltype(value)>::type;
    if constexpr ( ExtendedTypeSupport::is_static_array<Type>::value )
        std::cout << field.name << ": " << value[0] << std::endl;
    else
        std::cout << field.name << ": " << value << std::endl;
});
```

ExtendedTypeSupport defines many useful interfaces for these purposes...
- pair_rhs<T>::type // Gets the type of the right-hand value in an std::pair
- element_type<T>::type // Gets the type of the element contained in some iterable (be it a static_array, or STL container)
- remove_pointer<T>::type // Gets the type pointed to by a regular or smart pointer type
- is_pointable<T>::value // Checks whether a type is a regular or smart pointer
- static_array_size<T>::value // Gets the size of a static array, which may be a basic C++ array or the STL std::array type
- is_static_array<T>::value // Checks whether a type is a static array
- is_iterable<T>::value // Checks whether a type can be iterated, meaning it's a static array or other STL container
- is_stl_iterable<T>::value // Checks whether a type can be iterated with begin()/end()
- is_adaptor<T>::value // Checks whether a type is an STL adaptor (std::stack, std::queue, std::priority_queue)
- is_forward_list<T>::value // Checks whether a type is a forward list
- is_pair<T>::value // Checks whether a type is a pair
- is_bool<T>::value // Checks whether a type is a bool
- has_push_back<T>::value // Checks whether a type is an STL container with a push_back method
- has_push<T>::value // Checks whether a type is an STL container with a push method
- has_insert<T>::value // Checks whether a type is an STL container with an insert method
- has_clear<T>::value // Checks whether a type is an STL container with a clear method

Extended type support also provides the HasType method to check whether a type is included in a list of types, a TypeToStr method to retrieve a string representation of a type, and the get_underlying_container method to retrieve a const version of the underlying container for an STL adaptor.

See [Json.h](https://github.com/jjf28/CppRandomAccessReflection/blob/master/CppRandomAccessReflection/Json.h) for where all of this gets put together to traverse fairly complex objects.


## Field
Information provided to REFLECT is used to generate meta-data about your classes fields in the form of "Field" objects stored in a sub-class of your object titled "Class", Field objects come in two flavors but both share the following members:

1. name
2. typeStr
3. arraySize (0 for fields that are not static arrays)
4. isIterable (either a static array or STL container)
5. isReflected

The enhanced flavor of Field provides two additional static members:
1. Type (the actual type of the field)
2. Index (the index of the field, first field is zero, second is one, and so on)

The enhanced flavor also provides the HasAnnotation method which can be used to check what fields were annotated with. 


## Class
As stated, Class is a sub-class of the class you're trying to reflect; Class has the following static data members:

1. TotalFields
2. Fields[TotalFields] // Simple flavor, doesn't include the Type, Index, or HasAnnotation members

Class also provides two static methods to retrieve information about fields
```
ForEachField(object, [&](auto & field, auto & value) {
	// Your code here
});
FieldAt(object, size_t fieldIndex, [&](auto & field, auto & value) {
	// Your code here
});
```

"value" is a reference to the actual field in the object, which you can read or change. "field" is the enhanced flavor of Field, meaning it includes the Type, Index, and HasAnnotation members, it's often useful to grab the type of the field and value parameters so you can more cleanly access the static members
```
using Field = std::remove_reference<decltype(field)>::type;
using Type = std::remove_reference<decltype(value)>::type;
```


## Super Classes

If you only have one SuperClass, you can simply put the name of the SuperClass in for the SuperClasses parameter like so...

```
class A : public SuperA {
public:
    REFLECT((SuperA) A, ...)
};
```

If you have multiple SuperClasses then you need to first group them as template arguments for "Inherit" (the using statement is required because it's difficult if not impossible to accomodate a comma within a single argument of a macro), you can do this with a using directive...

```
class SubTest : public Super, public OtherSuper
{
public:
    int subVal;
    
    using Parents = Inherit<Super, OtherSuper>;
    REFLECT((Parents) SubTest, () otherVal)
};
```

## Annotations

Annotations are a way of giving additional information about a field that cannot be determined only using the field name and type information, for instance, whether the object stored in a field is also reflected, or whether the field should be treated as a string when streaming to JSON.

At present the only annotation specific to the RandomAccessReflection library is the "Reflected" annotation, which simply tells any code examining the field that the Class in that field is in turn, reflected using the REFLECT macro.

```
class A : public SuperA {
public:
    int first;
    int second;
    SubA sub;

    REFLECT((SuperA) A, () first, () second, (Reflected) sub, ...)
}
```

A downstream library, such is the JSON library included in this project, or whatever code you may be writing, can define its own annotations, an annotation is quite simply a unique type - that is a type that isn't already used as an annotation for a different purpose (in the future I may find ways to have annotations including string values).

```
/// The "Reflected" annotation denotes whether a given field is a type that is also reflected
struct Reflected {};
```

You can then check whether a field has a given annotation using code such as:
```
bool isReflected = Field::template HasAnnotation<Reflected>;
```



## How It Works

REFLECT is nothing more than a macro that defines the class Class, it does so using a concept called macro loops, macro loops start with static macro iterations...
```
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

The REFLECT macro takes in the name of the class you're adding reflection to, followed by a list of fields, using the LHS macro to extract superclasses from the first parenthesis, and any annotations in subsequent sets of parenthesis, and the RHS macro to extract the class name and fieldNames; using the for each macro we build the class "Class" one piece at a time...

1. TotalFields gets set to the count of arguments, not including the class name
2. an enum "IndexOf" is generated using each field name, because enums start at 0 and count up, IndexOf::fieldName provides the index of a given field in a manner statically available at compile time (this especially helps us build switches later)
3. An alias for each type is defined (via "using") named the same as the fieldname
4. a subclass is defined for each field named fieldName_, in this subclass a typeStr and nameStr are constructed, and a "Field" object is defined, using the typeStr, and name, these Field objects are the enhanced flavor
5. a "Field" array is generated, similar to the field object defined in the third step, but the simple flavor
6. The ForEachField method is generated, calling the given function with the enhanced flavor of the Field and a reference to the field
7. The FieldAt method is generated, calling the given function with the enhanced flavor of the Field and a reference to the field at the given fieldIndex

See [Reflect.h](https://github.com/jjf28/CppRandomAccessReflection/blob/master/CppRandomAccessReflection/Reflect.h) for the full implementation of macro loops and the REFLECT macro.
