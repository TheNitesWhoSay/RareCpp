cmake_minimum_required(VERSION 3.10)

set(This CppRandomAccessReflectionTest)

set(Headers
  JsonInputTest.h
  JsonTest.h
)
set(Sources
  ConstexprStrTest.cpp
  ExtendedTypeSupportTest.cpp
  FieldTest.cpp
  GenericMacroTest.cpp
  JsonDefinition.cpp
  JsonInputTest.cpp
  JsonInputTestBuffered.cpp
  JsonInputTestUnbuffered.cpp
  JsonTest.cpp
  JsonTestRunBuffered.cpp
  JsonTestRunUnbuffered.cpp
  ReflectTest.cpp
  StringBufferTest.cpp
  TestMain.cpp
)

add_executable(${This} ${Sources} ${Headers})

target_link_libraries(${This} PUBLIC
  gtest_main
)

add_test(
  NAME ${This}
  COMMAND {$This}
)
