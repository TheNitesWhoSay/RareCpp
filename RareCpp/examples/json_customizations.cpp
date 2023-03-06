#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <string>
#include <vector>

inline namespace json_customizations
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
        std::string name = "B.C.";
        std::vector<Point> points = {{0.0, 0.0}};

        REFLECT(Area, name, points)
    };
}

// Customizations must be in the global namespace
template <>
struct Json::Input::Customize<Point, RareTs::MemberType<Point>::latitude::type, RareTs::IndexOf<Point>::latitude>
{
    static bool as(std::istream & is, Context &, const Point &, double & value)
    {
        is >> Json::in(value);
        if ( value < -90 || value > 90 ) // Validating some input
            throw std::logic_error("Latitudes must be between -90 and 90");

        return true; // True indicates you did read the json field value off of is, false would indicate you want to fallback to the default json reader
    }
};

template <>
struct Json::Output::Customize<Area, RareTs::MemberType<Area>::name::type, RareTs::IndexOf<Area>::name>
{
    static bool as(Json::OutStreamType & os, Context &, const Area &, const std::string & value)
    {
        if ( value == "B.C." ) // Replacing some area name codes with the full area name
        {
            Json::Put::string(os, "British Columbia");
            return true; // True indicates you did put some valid json output
        }
        return false; // False indicates you want to use the default output
    }
};

inline namespace json_customizations
{
    void jsonCustomizations()
    {
        Area area {};
        std::cout << Json::out(area) << std::endl; // Out contains no whitespace

        std::cout << "Enter a new area: ";
        std::cin >> Json::in(area);

        std::cout << "Final area: " << Json::pretty(area) << std::endl; // Pretty is pretty-printed JSON
    }
}
