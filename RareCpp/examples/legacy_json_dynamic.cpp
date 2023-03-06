#include <rarecpp/json.h>
#include <iostream>
#include <limits>

void dynamicJsonIoExample()
{
    Json::Object obj;
    try {
        std::cout << std::endl << std::endl << "Enter Any JSON:" << std::endl;
        std::cin >> Json::in(obj);
    } catch ( Json::Exception & e ) {
        std::cout << std::endl << "Exception: " << e.what() << std::endl;
    }
    std::cout << "Read in: " << Json::pretty(obj) << std::endl;
    std::cout << "..." << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}