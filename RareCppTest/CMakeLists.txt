cmake_minimum_required(VERSION 3.10)

set(This RareCppTest)

set(Headers
  JsonInputTest.h
  JsonTest.h
)
set(Sources
  ExtendedTypeSupportTest.cpp
  FieldTest.cpp
  GenericMacroTest.cpp
  InheritTest.cpp
  JsonInputTest.cpp
  JsonInputTestBuffered.cpp
  JsonInputTestUnbuffered.cpp
  JsonTest.cpp
  JsonTestRunBuffered.cpp
  JsonTestRunUnbuffered.cpp
  ObjectMapperTest.cpp
  ReflectionTest.cpp
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
