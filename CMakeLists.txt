cmake_minimum_required(VERSION 3.21.0)

set(This Root)

if ( WIN32 AND NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" )
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if ( PROJECT_IS_TOP_LEVEL )
  set(RARECPP_BUILD_EXAMPLES TRUE)
  set(RARECPP_BUILD_TESTS TRUE)
else()
  add_library(${PROJECT_NAME} INTERFACE)
  target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

if ( RARECPP_BUILD_EXAMPLES OR RARECPP_BUILD_TESTS )
  include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()


if ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" )
  add_compile_options(
    -pedantic-errors -Wall -Wextra -Wconversion
  )
  # The following definitions were required for MinGW, they may not be appropriate on every system
  add_definitions(-DEOVERFLOW=132 -D_GLIBCXX_HAVE_EOVERFLOW=1)
elseif ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
  add_compile_options(
    -pedantic-errors -Wall -Wextra -Wconversion
  )
endif ()

if ( MSVC )
  add_compile_options(/permissive-)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif ( MSVC )

enable_testing()

if ( RARECPP_BUILD_EXAMPLES )
  add_subdirectory(RareCpp)
endif()

if ( RARECPP_BUILD_TESTS )
  add_subdirectory(GoogleTestLib/googletest)
  add_subdirectory(RareCppTest)
endif()

# Default Commands:
# mkdir build
# cd build
# cmake ../
# cmake --build .

# Replace third line to force gcc/g++
# cmake -G "MSYS Makefiles" -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ../

# Replace third line to force clang
# cmake -G "MinGW Makefiles" -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ ../
