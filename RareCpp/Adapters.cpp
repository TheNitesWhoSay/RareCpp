/*
    ! This code is experimental for future functionality and not yet supported !
*/

#include "../RareCppLib/Reflect.h"
#include <iostream>

template <size_t FieldIndex, typename Builder, typename T>
class BuilderAdapter
{
    using Field = typename T::Class::template FieldDescr<FieldIndex>::Field;
    using FieldType = typename Field::Type;
    Builder & b;
    T & t;

public:
    BuilderAdapter(Builder & builder, T & t) : b(builder), t(t) {}

    /** Sets the field to the value given */
    Builder & operator() (FieldType && value) { Field::template callback<false>(t, [&](auto & field){ field = value; }); return b; }
};

template <typename T>
class Builder : public T::Class::template Adapter<BuilderAdapter, Builder<T>, T>
{
    T t{};

public:
    using type = T;

    T build() { return t; };

    template <size_t ... Is> Builder(std::index_sequence<Is...>)
        : T::Class::template Adapter<BuilderAdapter, Builder<T>, T> { BuilderAdapter<Is, Builder<T>, T>{*this, t}... } {}
    Builder() : Builder(std::make_index_sequence<2>{}) {}
};

struct MyObj
{
    int myInt;
    std::string myString;

    REFLECT(MyObj, myInt, myString)
};

void adapterExperiment()
{
    auto myObj = Builder<MyObj>().myInt(42).myString("l33t").build();
    Builder<MyObj>().myInt(12).build();

    std::cout
        << myObj.myInt << std::endl
        << myObj.myString << std::endl;
}
