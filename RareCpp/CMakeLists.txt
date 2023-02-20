cmake_minimum_required(VERSION 3.15.0)

set(This RareCpp)

set(Headers
  examples/car.h
  examples/json_customization.h
)
set(Sources
  examples/annotated_supers.cpp
  examples/basic_reflection.cpp
  examples/functions.cpp
  examples/global_reflection.cpp
  examples/json_customization.cpp
  examples/json_dynamic.cpp
  examples/object_mapper.cpp
  examples/reflection_limits.cpp
  experimental/refactoring.cpp
  experimental/rest.cpp
  main.cpp
)

add_executable(${This} ${Sources} ${Headers})
