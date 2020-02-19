cmake_minimum_required(VERSION 3.10)

set(This Root)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" )
add_compile_options(
  -Wno-error
)
# The following definitions were required for MinGW, they may not be appropriate on every system
add_definitions(-DEOVERFLOW=132 -D_GLIBCXX_HAVE_EOVERFLOW=1)
endif ()

if ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
add_compile_options(
  -Wno-switch
)
endif ()

if ( MSVC )
add_compile_options(/permissive-)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
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

# Replace third line to force clang
# cmake -G "MinGW Makefiles" -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ ../
