#include "experimental/adaptive.h"
#include <iostream>
#include <limits>

void functionExperiment();
void basicReflectionExamples();
void globalReflectionExample();
void annotatedSupersExample();
void objectMapperExamples();
void dynamicJsonIoExample();
void refactoringExperiment();
void jsonCustomizationExample();
void restExperiment();

int main()
{
    restExperiment();
    adaptiveStructExperiment();
    functionExperiment();
    basicReflectionExamples();
    globalReflectionExample();
    annotatedSupersExample();
    objectMapperExamples();
    refactoringExperiment();
    dynamicJsonIoExample();
    jsonCustomizationExample();

    std::cout << std::endl << "Press enter to exit." << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}
