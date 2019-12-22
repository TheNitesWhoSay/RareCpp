cmake_minimum_required(VERSION 3.10)

set(This Root)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

if ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" )
add_compile_options(
  -Wno-error
)
endif ()

if ( MSVC )
add_compile_options(/permissive-)
endif ( MSVC )

enable_testing()

add_subdirectory(CppRandomAccessReflection)
add_subdirectory(GoogleTestLib/googletest)
add_subdirectory(CppRandomAccessReflectionTest)

# Default Commands:
# mkdir build
# cd build
# cmake ../
# cmake --build .

# Replace third line to force gcc/g++
# cmake -G "MSYS Makefiles" -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ../
