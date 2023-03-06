#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <string>
#include <vector>

inline namespace json_basic
{
    struct Point
    {
        NOTE(latitude, Json::Name{"lat"})
        double latitude;

        NOTE(longitude, Json::Name{"long"})
        double longitude;

        REFLECT(Point, latitude, longitude)
    };

    struct Area
    {
        std::string name;
        std::vector<Point> points = {{0.0, 0.0}};

        REFLECT(Area, name, points)
    };

    void jsonBasic()
    {
        Area area {};
        std::cout << Json::out(area) << std::endl; // Out contains no whitespace

        std::cout << "Enter a new area: ";
        std::cin >> Json::in(area);

        std::cout << "Final area: " << Json::pretty(area) << std::endl; // Pretty is pretty-printed JSON
    }
}
