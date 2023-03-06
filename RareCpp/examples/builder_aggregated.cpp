#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>
#include <string>
#include <vector>

inline namespace builder_aggregated
{
    struct Point
    {
        float x;
        float y;

        REFLECT(Point, x, y)
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

    void builderAggregated()
    {
        auto area = RareBuilder<Area>()
            .id(92)
            .descriptor(RareBuilder<Descriptor>().name("some area").address("12345 main street").build()) // Can use a nested builder
            .points({{1.1f, 2.2f}, {1.9f, 3.2f}, {1.3f, -2.5f}, {-4.2f, -1.2f}}) // You can pass ~anything you could assign to std::vector<Point>
            .build();

        std::cout << Json::pretty(area) << std::endl;
    }
}
