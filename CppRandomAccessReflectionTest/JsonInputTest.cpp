#include <gtest/gtest.h>
#include <regex>
#include "JsonInputTest.h"
using namespace Reflect;

TEST(JsonInputCustomizersTest, CustomizeFullySpecialized)
{
    CustomizeFullySpecialized customizeFullySpecialized = { 1, 2, 'a' };
    bool isSpecialized = Json::Input::Customize<CustomizeFullySpecialized, int, CustomizeFullySpecialized::Class::IndexOf::firstField, Annotate<>, CustomizeFullySpecialized::Class::firstField_::Field>
        ::As(std::cin, Json::context, customizeFullySpecialized, customizeFullySpecialized.firstField);
    EXPECT_TRUE(isSpecialized);
}
