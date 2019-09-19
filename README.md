# CppRandomAccessReflection
Creating a simpler, more intuitive means of C++ reflection




Within the class you're looking to reflect you place the REFLECT macro.

```
class FuelTank {
public:
    float capacity;
    float currentLevel;
    float tickMarks[2];

    REFLECT(FuelTank, B(float, capacity), B(float, currentLevel), B(float[2], tickMarks))
};
```

The REFLECT macro takes the class name, then one or more B or R types...

- B: Basic-type - a type which already has an acceptable representation when streamed
- R: Reflected-type - an object or object array which is also reflected

## Field
Information provided to REFLECT is used to generate meta-data about your classes fields in the form of "Field" objects stored in a sub-class of your object titled "Class", Field objects come in two flavors but both share the following members:

1. fieldIndex
2. fieldName
3. fieldType (as string)
4. arraySize (0 for fields that are not arrays)
5. isArray
6. isFieldTypeReflected

## Class
As stated, Class is a sub-class of the class you're trying to reflect; Class has the following static data members:

1. totalFields
2. fields[totalFields] // Simple flavor, doesn't include the type or Lambda guards

Class also provides two static methods to retrieve information about fields
```
ForEachField(object, [&](auto field, auto value) {
	// Your code here
});
FieldAt(object, size_t fieldIndex, [&](auto field, auto value) {
	// Your code here
});
```

"value" is a reference to the actual field in the object, which you can read or change. "field" is the enhanced flavor of Field, meaning it includes the type (accessible using decltype(field)::type), as well as the "Lambda Guards", which can be used to conviently use fields without hitting compiler errors.
```
field.IfPrimitive([&](auto) {
	// Your code here
});
field.IfObject([&](auto) {
	// Your code here
});
field.IfPrimitiveArray([&](auto) {
	// Your code here
});
field.IfObjectArray([&](auto) {
	// Your code here
});
```


## Usage

Following the setup of the REFLECT macro, you can easily loop over the fields of a reflected class...

```
FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
	std::cout << field.fieldName << ": " << value << std::endl;
});
```


Array access can be a little troublesome, e.g.:

```
FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
	if ( field.isArray ) {
		std::cout << field.fieldName << ": " << value[0] << std::endl;
	}
});
```

Will cause a compiler error, as the compiler will attempt to generate array access of FuelTank::capacity and FuelTank::currentLevel in addition to the appropriate array access code for tickMarks.

Placing your array access code inside the appropriate guard method will circumvent the issue, e.g.
```
FuelTank::Class::ForEachField(fuelTank, [&](auto field, auto value) {
    field.IfPrimitive([&](auto) { std::cout << "(fuelTankPrimitive) " << field.fieldName << ": " << value << std::endl; });
    field.IfPrimitiveArray([&](auto) { std::cout << "(fuelTankPrimitiveArray) " << field.fieldName << ": " << value[0] << std::endl; });
});
```


Similarly attempting to access sub-object meta-data can run you into issues, e.g.:

```
template <typename T>
void TraverseFields(T & t)
{
	using Class = typename T::Class;
	Class::ForEachField(obj, [&](auto field, auto value) {
		TraverseFields<decltype(field)::type>(value);
	});
}
```

Will cause errors similar to the array access issue, you can guard against these just the same.

```
template <typename T>
void TraverseFields(T & t)
{
	using Class = typename T::Class;
	Class::ForEachField(obj, [&](auto field, auto value) {
		field.IfObject([&](auto) { TraverseFields<decltype(field)::type>(value); });
		field.IfObjectArray([&](auto) {
			for ( size_t i=0; i<field.arraySize; i++ ) {
				auto reflectedObject = value[i];
				TraverseFields<decltype(reflectedObject)>(reflectedObject);
			});
		}
	});
}
```

See [Json.h](https://github.com/jjf28/CppRandomAccessReflection/blob/master/CppRandomAccessReflection/Json.h) for a full traversal-and-print of all fields.

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

The REFLECT macro takes in the name of the class you're adding reflection to, followed by a list of triplets (fieldType, fieldName, isFieldReflected) - usually as B(fieldType, fieldName), which implicitly adds the false, or R(fieldType, fieldName), which implicity adds the true; using the for each macros we build the class "Class" one piece at a time...

1. totalFields gets set to the count of arguments, not including the class name, divided by 3.
2. an enum "IndexOf" is generated using each field name, because enums start at 0 and count up, IndexOf::fieldName provides the index of a given field in a manner statically available at compile time (this especially helps us build switches later).
3. a "Field" object is defined for each fieldName/fieldType named f_(fieldName), using the triplets passed in, together with C++ type support https://en.cppreference.com/w/cpp/types , these Field objects are the enhanced flavor
4. a "Field" array is generated, similar to the field object defined in the third step, but the simple flavor
5. The ForEachField method is generated, calling the given function with the enhanced flavor of the Field and a reference to the field
6. The FieldAt method is generated, calling the given function with the enhanced flavor of the Field and a reference to the field at the given fieldIndex
