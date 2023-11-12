cmake_minimum_required(VERSION 3.15.0)

set(This RareCppTest)

set(Headers
  json_input_test.h
  json_test.h
)
set(Sources
  builder_test.cpp
  extended_type_support_test.cpp
  member_test.cpp
  generic_macro_test.cpp
  inherit_test.cpp
  json_input_test.cpp
  json_input_test_buffered.cpp
  json_input_test_unbuffered.cpp
  json_test.cpp
  json_test_run_buffered.cpp
  json_test_run_unbuffered.cpp
  object_mapper_test.cpp
  reflection_test.cpp
  reflect_test.cpp
  string_buffer_test.cpp
  test_main.cpp
  tuples_test.cpp
  whitebox_test.cpp
)

add_executable(${This} ${Sources} ${Headers})

target_link_libraries(${This} PUBLIC
  gtest_main
)

add_test(
  NAME ${This}
  COMMAND {$This}
)
