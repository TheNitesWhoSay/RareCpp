# RareCpp [![MIT License](https://img.shields.io/badge/license-MIT-blueviolet)](https://github.com/TheNitesWhoSay/RareCpp/blob/master/LICENSE) [![Header Only](https://img.shields.io/static/v1?label=format&message=Header%20Only&color=blueviolet)](https://github.com/TheNitesWhoSay/RareCpp/wiki/1.1.-Overview:-Quick-Start#setupinclusion) [![C++17](https://img.shields.io/static/v1?label=c%2B%2B&message=%3E%3D%2017&color=informational)](https://github.com/TheNitesWhoSay/RareCpp/wiki/1.1.-Overview:-Quick-Start#setupinclusion) [![GCC 10.1](https://img.shields.io/static/v1?label=gcc&message=%3E%3D%2010.1&color=informational)](https://github.com/TheNitesWhoSay/RareCpp/wiki/1.1.-Overview:-Quick-Start#compiler-compatibility) [![Clang 9.0.0](https://img.shields.io/static/v1?label=clang&message=%3E%3D%209.0.0&color=informational)](https://github.com/TheNitesWhoSay/RareCpp/wiki/1.1.-Overview:-Quick-Start#compiler-compatibility) [![MSVC 19.26](https://img.shields.io/static/v1?label=msvc&message=%3E%3D%2019.26&color=informational)](https://github.com/TheNitesWhoSay/RareCpp/wiki/1.1.-Overview:-Quick-Start#compiler-compatibility) [![579 Tests Passed](https://img.shields.io/badge/passed%20tests-579-success)](https://github.com/TheNitesWhoSay/RareCpp/tree/master/RareCppTest)

***C++** **R**andom **A**ccess **R**eflection & **E**xtensions: adding a simple, intuitive means of reflection to C++*

***

[Documentation Home](https://github.com/TheNitesWhoSay/RareCpp/wiki)

[Quick-Start Guide](https://github.com/TheNitesWhoSay/RareCpp/wiki/1.1.-Overview:-Quick-Start)

| Component | Summary | Examples |
| --------- | ------- | -------- |
| [RareTs](https://github.com/TheNitesWhoSay/RareCpp/wiki/1.1.-Overview:-Quick-Start) | Reflection & type support | <a target="_blank" href="https://godbolt.org/z/vdPdx1E7d">![link descr][run1]</a><a target="_blank" href="https://godbolt.org/z/ErTvW8oaT">![link descr][run2]</a><a target="_blank" href="https://godbolt.org/z/n1oqrEafq">![link descr][run3]</a><a target="_blank" href="https://github.com/TheNitesWhoSay/RareCpp/wiki/1.2.-Overview:-How-To...">![link descr][more]</a> |
| [RareBuilder](https://github.com/TheNitesWhoSay/RareCpp/wiki/4.0.-RareBuilder) | Adaptive object builder | <a target="_blank" href="https://godbolt.org/z/rWKx7P583">![link descr][run1]</a><a target="_blank" href="https://godbolt.org/z/hhcG1x4M1">![link descr][run2]</a><a target="_blank" href="https://godbolt.org/z/Wscbb4Tfn">![link descr][run3]</a><a target="_blank" href="https://github.com/TheNitesWhoSay/RareCpp/wiki/1.2.-Overview:-How-To...">![link descr][more]</a> |
| [RareMapper](https://github.com/TheNitesWhoSay/RareCpp/wiki/5.0.-RareMapper) | Dynamic type mapping | <a target="_blank" href="https://godbolt.org/z/rv5P9Wo6s">![link descr][run1]</a><a target="_blank" href="https://godbolt.org/z/jqPYadPWG">![link descr][run2]</a><a target="_blank" href="https://godbolt.org/z/7bKK1vbTE">![link descr][run3]</a><a target="_blank" href="https://github.com/TheNitesWhoSay/RareCpp/wiki/1.2.-Overview:-How-To...">![link descr][more]</a> |
| [RareJson](https://github.com/TheNitesWhoSay/RareCpp/wiki/6.0.-RareJson) | Reflection-based JSON I/O | <a target="_blank" href="https://godbolt.org/z/PjPsxshf5">![link descr][run1]</a><a target="_blank" href="https://godbolt.org/z/WP5jr4hM7">![link descr][run2]</a><a target="_blank" href="https://godbolt.org/z/EcdbcshdW">![link descr][run3]</a><a target="_blank" href="https://github.com/TheNitesWhoSay/RareCpp/wiki/1.2.-Overview:-How-To...">![link descr][more]</a> |

***

# Examples

Simple Example: <a target="_blank" href="https://godbolt.org/z/WzqGh8Gje">![Run][run]</a>
```cpp
#include <rarecpp/reflect.h>
#include <iostream>

struct MyObj
{
    int a = 42;
    float b = 1337.f;

    REFLECT(MyObj, a, b)
};

int main()
{
    MyObj myObj{};
    RareTs::Members<MyObj>::forEach(myObj, [&](auto member, auto & value) {
        std::cout << member.name << ": " << value << std::endl;
    });
}
```
Output:
```
a: 42
b: 1337
```

JSON & Builders Example: <a target="_blank" href="https://godbolt.org/z/1zn9bYa9o">![Run][run]</a>
```cpp
struct Point
{
    float x;
    float y;

    NOTE(hash, Json::Ignore) // Control behavior with member or class-level annotations!
    size_t hash = 0;

    REFLECT(Point, x, y, hash)
};

struct Descriptor
{
    std::string name;
    std::string address;

    REFLECT(Descriptor, name, address)
};

struct Area
{
    int id;
    Descriptor descriptor;
    std::vector<Point> points;

    REFLECT(Area, id, descriptor, points)
};

int main()
{
    auto area = RareBuilder<Area>()
        .id(92)
        .descriptor(RareBuilder<Descriptor>().name("some area").address("12345 main street").build())
        .points({{1.1f, 2.2f}, {1.9f, 3.2f}, {1.3f, -2.5f}, {-4.2f, -1.2f}})
        .build();

    std::cout << Json::pretty(area) << std::endl;
}
```
Output:
```
{
  "id": 92,
  "descriptor": {
    "name": "some area",
    "address": "12345 main street"
  },
  "points": [
    {"x": 1.1,"y": 2.2},
    {"x": 1.9,"y": 3.2},
    {"x": 1.3,"y": -2.5},
    {"x": -4.2,"y": -1.2}
  ]
}
```

[More Examples / How to...](https://github.com/TheNitesWhoSay/RareCpp/wiki/1.2.-Overview:-How-To...">)

# Features
### Reflect
* REFLECT classes and up to 125 class members
* REFLECT from inside the class definition
* REFLECT from outside the class definition*
* REFLECT static & non-static members
* REFLECT data members
* REFLECT reference members
* REFLECT functions
* REFLECT overloaded-functions
* REFLECT super-classes
* REFLECT templated-classes
* REFLECT templated-members**

_\* Reflection from outside the class definition is limited to public and protected members_

_\*\* Any members based on the class-level template parameters and any functions for which template parameters are inferred from parameters are reflectable, reflecting other templated members is not always possible._

### Annotate
* NOTE classes, members, super-classes and overloads to tag them with additional compile-time information
* NOTE using any data available in constexpr, user-defined or library-specific
* Common interface to check for note-of-type presence and get note values on particular members/classes/supers/overloads
* Common interface to filter and iterate notes on particular members/classes/supers/overloads

### Member Information
* Iterate members
* Access members by constexpr index
* Access members by runtime index
* Access members by name string
* Access members by identifier
* Get member static value, instance value, type, pointer type, name, index, staticness, access-protection modifier, offset, and overloads

### Access supers
* In lambda loops
* By constexpr index
* By runtime index

### Extensions
* Adaptive Structures (dynamically create structures consisting of one some combination/subset of one or more classes members)
* RareBuilder - Type-safe, adaptive-struct-based builder implementation
* RareMapper - Reflection-based object mapper
* RareJson - JSON I/O for reflected structures

# Limitations
### C++ Version
* C++17 and up required

### Supported Compilers
RareCpp minimizes use of compiler-specific features wherever possible, other compilers may work without modification and may or may not be possible to add to supported compilers upon request.

Use of compiler versions lower than preferred may disable or cause bugs with certain features.

* GCC Minimum: 10.1 (10.3.0 and up preferred)
* Clang Minimum: 9.0.0 (15.0.0 and up preferred)
* MSVC Minimum: 19.26 (MSVC 19.29/Visual Studios 2019 version 16.11.2 and up preferred)

### Known Compiler Issues
* Unable to use the REFLECT macro in non-public sections in GCC versions before 12, recommended to use public sections for the REFLECT macro until GCC 12 has wider support & adoption
* MSVC 1932 (Visual Studio 2022 version 17.2.2) and up is required to accurately detect the access-protection level of protected members

### Known General Limitations
* Cannot currently reflect type alias-members
* Cannot currently reflect nested classes
* Cannot currently detect the access-protection level of overloads (they will default to private)
* Cannot currently reflect all template members (cannot reflect templated data members not-based on class-level template parameters nor templated functions with parameters that can't be deduced from function parameters)

[Documentation Home](https://github.com/TheNitesWhoSay/RareCpp/wiki)

# Video Preview

[RareCppPreview.webm](https://user-images.githubusercontent.com/8083653/227604198-1d0921a4-4d50-4968-823e-1b986d3aef44.webm)

[run]: https://img.shields.io/static/v1?style=social&label=Run&message=%20&logo=data:image/svg+xml;base64,PHN2ZyB2ZXJzaW9uPSIxLjEiIGlkPSJMYXllcl8xIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB4PSIwcHgiIHk9IjBweCIgdmlld0JveD0iMCAwIDEwMDAgMTAwMCIgc3R5bGU9ImVuYWJsZS1iYWNrZ3JvdW5kOm5ldyAwIDAgMTAwMCAxMDAwOyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+PHN0eWxlIHR5cGU9InRleHQvY3NzIj4uc3Qwe2ZpbGw6IzY3QzUyQTt9LnN0MXtmaWxsOiMzQzNDM0Y7fTwvc3R5bGU+PGcgaWQ9IkNFIj48cGF0aCBpZD0iWE1MSURfM18iIGNsYXNzPSJzdDAiIGQ9Ik03NTYuNCw2NTguMWMtMy4yLTUuNy0zLjItMTIuNywwLTE4LjVjMy44LTYuOSw3LjMtMTMuOCwxMC42LTIxLjFjMi4zLTUuMi0xLjQtMTEuMS03LjItMTEuMUg2OTdjLTYuNCwwLTEyLjUsMy4yLTE2LDguNmMtOC4xLDEyLjMtMTcuNSwyNC4zLTI4LjMsMzVjLTQwLjMsNDAuMy05NCw2Mi41LTE1MSw2Mi41Yy01NywwLTExMC43LTIyLjItMTUxLTYyLjVjLTQwLjMtNDAuMy02Mi41LTk0LTYyLjUtMTUxczIyLjItMTEwLjcsNjIuNS0xNTFjNDAuMy00MC4zLDk0LTYyLjUsMTUxLTYyLjVjNTcsMCwxMTAuNywyMi4yLDE1MSw2Mi41YzEwLjcsMTAuNywyMC4yLDIyLjcsMjguMywzNWMzLjUsNS40LDkuNSw4LjYsMTYsOC42aDYyLjhjNS43LDAsOS41LTUuOSw3LjItMTEuMWMtMy4yLTcuMi02LjgtMTQuMS0xMC42LTIxLjFjLTMuMi01LjctMy4xLTEyLjcsMC0xOC41bDIxLjMtMzguNmM0LjEtNy41LDIuOC0xNi44LTMuMi0yMi44bC01My4yLTUzLjJjLTYtNi0xNS40LTcuNC0yMi44LTMuMmwtMzguNywyMS40Yy01LjgsMy4yLTEyLjcsMy4xLTE4LjUtMC4xYy0xOC40LTEwLjEtMzcuOS0xOC4xLTU4LjEtMjMuOWMtNi4zLTEuOC0xMS4zLTYuNy0xMy4xLTEzTDU1Ny44LDE2NmMtMi40LTguMi05LjktMTMuOS0xOC40LTEzLjloLTc1LjNjLTguNSwwLTE2LjEsNS42LTE4LjQsMTMuOWwtMTIuMiw0Mi4zYy0xLjgsNi4zLTYuOCwxMS4yLTEzLjEsMTNjLTIwLjIsNS45LTM5LjcsMTMuOS01OC4xLDI0Yy01LjgsMy4yLTEyLjcsMy4yLTE4LjUsMC4xTDMwNSwyMjQuMWMtNy41LTQuMS0xNi44LTIuOC0yMi44LDMuMkwyMjksMjgwLjVjLTYsNi03LjQsMTUuNC0zLjIsMjIuOGwyMS40LDM4LjdjMy4yLDUuOCwzLjEsMTIuNy0wLjEsMTguNWMtMTAuMSwxOC40LTE4LjEsMzcuOS0yMy45LDU4LjFjLTEuOCw2LjMtNi43LDExLjMtMTMsMTMuMWwtNDIuMywxMi4yYy04LjIsMi40LTEzLjksOS45LTEzLjksMTguNHY3NS4zYzAsOC41LDUuNiwxNi4xLDEzLjksMTguNGw0Mi4zLDEyLjJjNi4zLDEuOCwxMS4yLDYuOCwxMywxMy4xYzUuOSwyMC4yLDEzLjksMzkuNywyNCw1OC4xYzMuMiw1LjgsMy4yLDEyLjcsMC4xLDE4LjVsLTIxLjQsMzguN2MtNC4xLDcuNS0yLjgsMTYuOCwzLjIsMjIuOGw1My4yLDUzLjJjNiw2LDE1LjQsNy40LDIyLjgsMy4ybDM4LjctMjEuNGM1LjgtMy4yLDEyLjctMy4xLDE4LjUsMC4xYzE4LjQsMTAuMSwzNy45LDE4LjEsNTguMSwyMy45YzYuMywxLjgsMTEuMyw2LjcsMTMuMSwxM2wxMi4yLDQyLjNjMi40LDguMiw5LjksMTMuOSwxOC40LDEzLjloNzUuM2M4LjUsMCwxNi4xLTUuNiwxOC40LTEzLjlsMTIuMi00Mi4zYzEuOC02LjMsNi44LTExLjIsMTMuMS0xM2MyMC4yLTUuOSwzOS43LTEzLjksNTguMS0yNGM1LjgtMy4yLDEyLjctMy4yLDE4LjUtMC4xbDM4LjcsMjEuNGM3LjUsNC4xLDE2LjgsMi44LDIyLjgtMy4ybDUzLjItNTMuMmM2LTYsNy40LTE1LjQsMy4yLTIyLjhMNzU2LjQsNjU4LjF6Ii8+PGcgaWQ9IlhNTElEXzJfIj48cmVjdCBpZD0iWE1MSURfMTVfIiB4PSIzNzMuNyIgeT0iMzkxLjYiIGNsYXNzPSJzdDEiIHdpZHRoPSIyNTguNCIgaGVpZ2h0PSI0OCIvPjwvZz48ZyBpZD0iWE1MSURfNF8iPjxyZWN0IGlkPSJYTUxJRF8xNF8iIHg9IjM3My43IiB5PSI0NzYuOSIgY2xhc3M9InN0MSIgd2lkdGg9IjIxMy4yIiBoZWlnaHQ9IjQ4Ii8+PC9nPjxnIGlkPSJYTUxJRF81XyI+PHJlY3QgaWQ9IlhNTElEXzEzXyIgeD0iMzczLjciIHk9IjU2Mi4xIiBjbGFzcz0ic3QxIiB3aWR0aD0iMjU4LjQiIGhlaWdodD0iNDgiLz48L2c+PC9nPjwvc3ZnPg==
[run1]: https://img.shields.io/static/v1?style=social&label=1&message=%20&logo=data:image/svg+xml;base64,PHN2ZyB2ZXJzaW9uPSIxLjEiIGlkPSJMYXllcl8xIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB4PSIwcHgiIHk9IjBweCIgdmlld0JveD0iMCAwIDEwMDAgMTAwMCIgc3R5bGU9ImVuYWJsZS1iYWNrZ3JvdW5kOm5ldyAwIDAgMTAwMCAxMDAwOyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+PHN0eWxlIHR5cGU9InRleHQvY3NzIj4uc3Qwe2ZpbGw6IzY3QzUyQTt9LnN0MXtmaWxsOiMzQzNDM0Y7fTwvc3R5bGU+PGcgaWQ9IkNFIj48cGF0aCBpZD0iWE1MSURfM18iIGNsYXNzPSJzdDAiIGQ9Ik03NTYuNCw2NTguMWMtMy4yLTUuNy0zLjItMTIuNywwLTE4LjVjMy44LTYuOSw3LjMtMTMuOCwxMC42LTIxLjFjMi4zLTUuMi0xLjQtMTEuMS03LjItMTEuMUg2OTdjLTYuNCwwLTEyLjUsMy4yLTE2LDguNmMtOC4xLDEyLjMtMTcuNSwyNC4zLTI4LjMsMzVjLTQwLjMsNDAuMy05NCw2Mi41LTE1MSw2Mi41Yy01NywwLTExMC43LTIyLjItMTUxLTYyLjVjLTQwLjMtNDAuMy02Mi41LTk0LTYyLjUtMTUxczIyLjItMTEwLjcsNjIuNS0xNTFjNDAuMy00MC4zLDk0LTYyLjUsMTUxLTYyLjVjNTcsMCwxMTAuNywyMi4yLDE1MSw2Mi41YzEwLjcsMTAuNywyMC4yLDIyLjcsMjguMywzNWMzLjUsNS40LDkuNSw4LjYsMTYsOC42aDYyLjhjNS43LDAsOS41LTUuOSw3LjItMTEuMWMtMy4yLTcuMi02LjgtMTQuMS0xMC42LTIxLjFjLTMuMi01LjctMy4xLTEyLjcsMC0xOC41bDIxLjMtMzguNmM0LjEtNy41LDIuOC0xNi44LTMuMi0yMi44bC01My4yLTUzLjJjLTYtNi0xNS40LTcuNC0yMi44LTMuMmwtMzguNywyMS40Yy01LjgsMy4yLTEyLjcsMy4xLTE4LjUtMC4xYy0xOC40LTEwLjEtMzcuOS0xOC4xLTU4LjEtMjMuOWMtNi4zLTEuOC0xMS4zLTYuNy0xMy4xLTEzTDU1Ny44LDE2NmMtMi40LTguMi05LjktMTMuOS0xOC40LTEzLjloLTc1LjNjLTguNSwwLTE2LjEsNS42LTE4LjQsMTMuOWwtMTIuMiw0Mi4zYy0xLjgsNi4zLTYuOCwxMS4yLTEzLjEsMTNjLTIwLjIsNS45LTM5LjcsMTMuOS01OC4xLDI0Yy01LjgsMy4yLTEyLjcsMy4yLTE4LjUsMC4xTDMwNSwyMjQuMWMtNy41LTQuMS0xNi44LTIuOC0yMi44LDMuMkwyMjksMjgwLjVjLTYsNi03LjQsMTUuNC0zLjIsMjIuOGwyMS40LDM4LjdjMy4yLDUuOCwzLjEsMTIuNy0wLjEsMTguNWMtMTAuMSwxOC40LTE4LjEsMzcuOS0yMy45LDU4LjFjLTEuOCw2LjMtNi43LDExLjMtMTMsMTMuMWwtNDIuMywxMi4yYy04LjIsMi40LTEzLjksOS45LTEzLjksMTguNHY3NS4zYzAsOC41LDUuNiwxNi4xLDEzLjksMTguNGw0Mi4zLDEyLjJjNi4zLDEuOCwxMS4yLDYuOCwxMywxMy4xYzUuOSwyMC4yLDEzLjksMzkuNywyNCw1OC4xYzMuMiw1LjgsMy4yLDEyLjcsMC4xLDE4LjVsLTIxLjQsMzguN2MtNC4xLDcuNS0yLjgsMTYuOCwzLjIsMjIuOGw1My4yLDUzLjJjNiw2LDE1LjQsNy40LDIyLjgsMy4ybDM4LjctMjEuNGM1LjgtMy4yLDEyLjctMy4xLDE4LjUsMC4xYzE4LjQsMTAuMSwzNy45LDE4LjEsNTguMSwyMy45YzYuMywxLjgsMTEuMyw2LjcsMTMuMSwxM2wxMi4yLDQyLjNjMi40LDguMiw5LjksMTMuOSwxOC40LDEzLjloNzUuM2M4LjUsMCwxNi4xLTUuNiwxOC40LTEzLjlsMTIuMi00Mi4zYzEuOC02LjMsNi44LTExLjIsMTMuMS0xM2MyMC4yLTUuOSwzOS43LTEzLjksNTguMS0yNGM1LjgtMy4yLDEyLjctMy4yLDE4LjUtMC4xbDM4LjcsMjEuNGM3LjUsNC4xLDE2LjgsMi44LDIyLjgtMy4ybDUzLjItNTMuMmM2LTYsNy40LTE1LjQsMy4yLTIyLjhMNzU2LjQsNjU4LjF6Ii8+PGcgaWQ9IlhNTElEXzJfIj48cmVjdCBpZD0iWE1MSURfMTVfIiB4PSIzNzMuNyIgeT0iMzkxLjYiIGNsYXNzPSJzdDEiIHdpZHRoPSIyNTguNCIgaGVpZ2h0PSI0OCIvPjwvZz48ZyBpZD0iWE1MSURfNF8iPjxyZWN0IGlkPSJYTUxJRF8xNF8iIHg9IjM3My43IiB5PSI0NzYuOSIgY2xhc3M9InN0MSIgd2lkdGg9IjIxMy4yIiBoZWlnaHQ9IjQ4Ii8+PC9nPjxnIGlkPSJYTUxJRF81XyI+PHJlY3QgaWQ9IlhNTElEXzEzXyIgeD0iMzczLjciIHk9IjU2Mi4xIiBjbGFzcz0ic3QxIiB3aWR0aD0iMjU4LjQiIGhlaWdodD0iNDgiLz48L2c+PC9nPjwvc3ZnPg==
[run2]: https://img.shields.io/static/v1?style=social&label=2&message=%20&logo=data:image/svg+xml;base64,PHN2ZyB2ZXJzaW9uPSIxLjEiIGlkPSJMYXllcl8xIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB4PSIwcHgiIHk9IjBweCIgdmlld0JveD0iMCAwIDEwMDAgMTAwMCIgc3R5bGU9ImVuYWJsZS1iYWNrZ3JvdW5kOm5ldyAwIDAgMTAwMCAxMDAwOyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+PHN0eWxlIHR5cGU9InRleHQvY3NzIj4uc3Qwe2ZpbGw6IzY3QzUyQTt9LnN0MXtmaWxsOiMzQzNDM0Y7fTwvc3R5bGU+PGcgaWQ9IkNFIj48cGF0aCBpZD0iWE1MSURfM18iIGNsYXNzPSJzdDAiIGQ9Ik03NTYuNCw2NTguMWMtMy4yLTUuNy0zLjItMTIuNywwLTE4LjVjMy44LTYuOSw3LjMtMTMuOCwxMC42LTIxLjFjMi4zLTUuMi0xLjQtMTEuMS03LjItMTEuMUg2OTdjLTYuNCwwLTEyLjUsMy4yLTE2LDguNmMtOC4xLDEyLjMtMTcuNSwyNC4zLTI4LjMsMzVjLTQwLjMsNDAuMy05NCw2Mi41LTE1MSw2Mi41Yy01NywwLTExMC43LTIyLjItMTUxLTYyLjVjLTQwLjMtNDAuMy02Mi41LTk0LTYyLjUtMTUxczIyLjItMTEwLjcsNjIuNS0xNTFjNDAuMy00MC4zLDk0LTYyLjUsMTUxLTYyLjVjNTcsMCwxMTAuNywyMi4yLDE1MSw2Mi41YzEwLjcsMTAuNywyMC4yLDIyLjcsMjguMywzNWMzLjUsNS40LDkuNSw4LjYsMTYsOC42aDYyLjhjNS43LDAsOS41LTUuOSw3LjItMTEuMWMtMy4yLTcuMi02LjgtMTQuMS0xMC42LTIxLjFjLTMuMi01LjctMy4xLTEyLjcsMC0xOC41bDIxLjMtMzguNmM0LjEtNy41LDIuOC0xNi44LTMuMi0yMi44bC01My4yLTUzLjJjLTYtNi0xNS40LTcuNC0yMi44LTMuMmwtMzguNywyMS40Yy01LjgsMy4yLTEyLjcsMy4xLTE4LjUtMC4xYy0xOC40LTEwLjEtMzcuOS0xOC4xLTU4LjEtMjMuOWMtNi4zLTEuOC0xMS4zLTYuNy0xMy4xLTEzTDU1Ny44LDE2NmMtMi40LTguMi05LjktMTMuOS0xOC40LTEzLjloLTc1LjNjLTguNSwwLTE2LjEsNS42LTE4LjQsMTMuOWwtMTIuMiw0Mi4zYy0xLjgsNi4zLTYuOCwxMS4yLTEzLjEsMTNjLTIwLjIsNS45LTM5LjcsMTMuOS01OC4xLDI0Yy01LjgsMy4yLTEyLjcsMy4yLTE4LjUsMC4xTDMwNSwyMjQuMWMtNy41LTQuMS0xNi44LTIuOC0yMi44LDMuMkwyMjksMjgwLjVjLTYsNi03LjQsMTUuNC0zLjIsMjIuOGwyMS40LDM4LjdjMy4yLDUuOCwzLjEsMTIuNy0wLjEsMTguNWMtMTAuMSwxOC40LTE4LjEsMzcuOS0yMy45LDU4LjFjLTEuOCw2LjMtNi43LDExLjMtMTMsMTMuMWwtNDIuMywxMi4yYy04LjIsMi40LTEzLjksOS45LTEzLjksMTguNHY3NS4zYzAsOC41LDUuNiwxNi4xLDEzLjksMTguNGw0Mi4zLDEyLjJjNi4zLDEuOCwxMS4yLDYuOCwxMywxMy4xYzUuOSwyMC4yLDEzLjksMzkuNywyNCw1OC4xYzMuMiw1LjgsMy4yLDEyLjcsMC4xLDE4LjVsLTIxLjQsMzguN2MtNC4xLDcuNS0yLjgsMTYuOCwzLjIsMjIuOGw1My4yLDUzLjJjNiw2LDE1LjQsNy40LDIyLjgsMy4ybDM4LjctMjEuNGM1LjgtMy4yLDEyLjctMy4xLDE4LjUsMC4xYzE4LjQsMTAuMSwzNy45LDE4LjEsNTguMSwyMy45YzYuMywxLjgsMTEuMyw2LjcsMTMuMSwxM2wxMi4yLDQyLjNjMi40LDguMiw5LjksMTMuOSwxOC40LDEzLjloNzUuM2M4LjUsMCwxNi4xLTUuNiwxOC40LTEzLjlsMTIuMi00Mi4zYzEuOC02LjMsNi44LTExLjIsMTMuMS0xM2MyMC4yLTUuOSwzOS43LTEzLjksNTguMS0yNGM1LjgtMy4yLDEyLjctMy4yLDE4LjUtMC4xbDM4LjcsMjEuNGM3LjUsNC4xLDE2LjgsMi44LDIyLjgtMy4ybDUzLjItNTMuMmM2LTYsNy40LTE1LjQsMy4yLTIyLjhMNzU2LjQsNjU4LjF6Ii8+PGcgaWQ9IlhNTElEXzJfIj48cmVjdCBpZD0iWE1MSURfMTVfIiB4PSIzNzMuNyIgeT0iMzkxLjYiIGNsYXNzPSJzdDEiIHdpZHRoPSIyNTguNCIgaGVpZ2h0PSI0OCIvPjwvZz48ZyBpZD0iWE1MSURfNF8iPjxyZWN0IGlkPSJYTUxJRF8xNF8iIHg9IjM3My43IiB5PSI0NzYuOSIgY2xhc3M9InN0MSIgd2lkdGg9IjIxMy4yIiBoZWlnaHQ9IjQ4Ii8+PC9nPjxnIGlkPSJYTUxJRF81XyI+PHJlY3QgaWQ9IlhNTElEXzEzXyIgeD0iMzczLjciIHk9IjU2Mi4xIiBjbGFzcz0ic3QxIiB3aWR0aD0iMjU4LjQiIGhlaWdodD0iNDgiLz48L2c+PC9nPjwvc3ZnPg==
[run3]: https://img.shields.io/static/v1?style=social&label=3&message=%20&logo=data:image/svg+xml;base64,PHN2ZyB2ZXJzaW9uPSIxLjEiIGlkPSJMYXllcl8xIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB4PSIwcHgiIHk9IjBweCIgdmlld0JveD0iMCAwIDEwMDAgMTAwMCIgc3R5bGU9ImVuYWJsZS1iYWNrZ3JvdW5kOm5ldyAwIDAgMTAwMCAxMDAwOyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+PHN0eWxlIHR5cGU9InRleHQvY3NzIj4uc3Qwe2ZpbGw6IzY3QzUyQTt9LnN0MXtmaWxsOiMzQzNDM0Y7fTwvc3R5bGU+PGcgaWQ9IkNFIj48cGF0aCBpZD0iWE1MSURfM18iIGNsYXNzPSJzdDAiIGQ9Ik03NTYuNCw2NTguMWMtMy4yLTUuNy0zLjItMTIuNywwLTE4LjVjMy44LTYuOSw3LjMtMTMuOCwxMC42LTIxLjFjMi4zLTUuMi0xLjQtMTEuMS03LjItMTEuMUg2OTdjLTYuNCwwLTEyLjUsMy4yLTE2LDguNmMtOC4xLDEyLjMtMTcuNSwyNC4zLTI4LjMsMzVjLTQwLjMsNDAuMy05NCw2Mi41LTE1MSw2Mi41Yy01NywwLTExMC43LTIyLjItMTUxLTYyLjVjLTQwLjMtNDAuMy02Mi41LTk0LTYyLjUtMTUxczIyLjItMTEwLjcsNjIuNS0xNTFjNDAuMy00MC4zLDk0LTYyLjUsMTUxLTYyLjVjNTcsMCwxMTAuNywyMi4yLDE1MSw2Mi41YzEwLjcsMTAuNywyMC4yLDIyLjcsMjguMywzNWMzLjUsNS40LDkuNSw4LjYsMTYsOC42aDYyLjhjNS43LDAsOS41LTUuOSw3LjItMTEuMWMtMy4yLTcuMi02LjgtMTQuMS0xMC42LTIxLjFjLTMuMi01LjctMy4xLTEyLjcsMC0xOC41bDIxLjMtMzguNmM0LjEtNy41LDIuOC0xNi44LTMuMi0yMi44bC01My4yLTUzLjJjLTYtNi0xNS40LTcuNC0yMi44LTMuMmwtMzguNywyMS40Yy01LjgsMy4yLTEyLjcsMy4xLTE4LjUtMC4xYy0xOC40LTEwLjEtMzcuOS0xOC4xLTU4LjEtMjMuOWMtNi4zLTEuOC0xMS4zLTYuNy0xMy4xLTEzTDU1Ny44LDE2NmMtMi40LTguMi05LjktMTMuOS0xOC40LTEzLjloLTc1LjNjLTguNSwwLTE2LjEsNS42LTE4LjQsMTMuOWwtMTIuMiw0Mi4zYy0xLjgsNi4zLTYuOCwxMS4yLTEzLjEsMTNjLTIwLjIsNS45LTM5LjcsMTMuOS01OC4xLDI0Yy01LjgsMy4yLTEyLjcsMy4yLTE4LjUsMC4xTDMwNSwyMjQuMWMtNy41LTQuMS0xNi44LTIuOC0yMi44LDMuMkwyMjksMjgwLjVjLTYsNi03LjQsMTUuNC0zLjIsMjIuOGwyMS40LDM4LjdjMy4yLDUuOCwzLjEsMTIuNy0wLjEsMTguNWMtMTAuMSwxOC40LTE4LjEsMzcuOS0yMy45LDU4LjFjLTEuOCw2LjMtNi43LDExLjMtMTMsMTMuMWwtNDIuMywxMi4yYy04LjIsMi40LTEzLjksOS45LTEzLjksMTguNHY3NS4zYzAsOC41LDUuNiwxNi4xLDEzLjksMTguNGw0Mi4zLDEyLjJjNi4zLDEuOCwxMS4yLDYuOCwxMywxMy4xYzUuOSwyMC4yLDEzLjksMzkuNywyNCw1OC4xYzMuMiw1LjgsMy4yLDEyLjcsMC4xLDE4LjVsLTIxLjQsMzguN2MtNC4xLDcuNS0yLjgsMTYuOCwzLjIsMjIuOGw1My4yLDUzLjJjNiw2LDE1LjQsNy40LDIyLjgsMy4ybDM4LjctMjEuNGM1LjgtMy4yLDEyLjctMy4xLDE4LjUsMC4xYzE4LjQsMTAuMSwzNy45LDE4LjEsNTguMSwyMy45YzYuMywxLjgsMTEuMyw2LjcsMTMuMSwxM2wxMi4yLDQyLjNjMi40LDguMiw5LjksMTMuOSwxOC40LDEzLjloNzUuM2M4LjUsMCwxNi4xLTUuNiwxOC40LTEzLjlsMTIuMi00Mi4zYzEuOC02LjMsNi44LTExLjIsMTMuMS0xM2MyMC4yLTUuOSwzOS43LTEzLjksNTguMS0yNGM1LjgtMy4yLDEyLjctMy4yLDE4LjUtMC4xbDM4LjcsMjEuNGM3LjUsNC4xLDE2LjgsMi44LDIyLjgtMy4ybDUzLjItNTMuMmM2LTYsNy40LTE1LjQsMy4yLTIyLjhMNzU2LjQsNjU4LjF6Ii8+PGcgaWQ9IlhNTElEXzJfIj48cmVjdCBpZD0iWE1MSURfMTVfIiB4PSIzNzMuNyIgeT0iMzkxLjYiIGNsYXNzPSJzdDEiIHdpZHRoPSIyNTguNCIgaGVpZ2h0PSI0OCIvPjwvZz48ZyBpZD0iWE1MSURfNF8iPjxyZWN0IGlkPSJYTUxJRF8xNF8iIHg9IjM3My43IiB5PSI0NzYuOSIgY2xhc3M9InN0MSIgd2lkdGg9IjIxMy4yIiBoZWlnaHQ9IjQ4Ii8+PC9nPjxnIGlkPSJYTUxJRF81XyI+PHJlY3QgaWQ9IlhNTElEXzEzXyIgeD0iMzczLjciIHk9IjU2Mi4xIiBjbGFzcz0ic3QxIiB3aWR0aD0iMjU4LjQiIGhlaWdodD0iNDgiLz48L2c+PC9nPjwvc3ZnPg==
[more]: https://img.shields.io/static/v1?style=social&label=More&message=%20
[docs]: https://img.shields.io/static/v1?style=social&label=Docs&message=%20
