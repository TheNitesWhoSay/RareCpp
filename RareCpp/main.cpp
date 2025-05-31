#include "examples/declare_examples.h"
#include "menu.h"

// Experimental
namespace experimental {
    void rest();
    void dataHistory();
}

// No-op
void none() {}

void openMenu()
{
    Menu("Select a category")
        .item(
            Menu("Experimental", "Select an experiment: ")
            .item("Rest experiment", &experimental::rest)
            .item("Data history experiment", &experimental::dataHistory)
        ).item(
            Menu("Legacy examples", "Select an example: ")
            .item("Basic reflection", &basicReflectionExamples)
            .item("Global reflection", &globalReflectionExample)
            .item("Annotated supers", &annotatedSupersExample)
            .item("Object mapper", &objectMapperExamples)
            .item("Dynamic JSON I/O", &dynamicJsonIoExample)
            .item("Function reflection", &functionReflectionExample)
            .item("Json customization", &jsonCustomizationExample)
            .item("Reflection limits example", &reflectionLimitsExample)
        ).item(
            Menu{"Member Examples", "Select an example: "}
            .item("Define, iterate & modify class members", &memberDefineIterateModify)
            .item("Access class members by constexpr index", &memberAccessByConstexprIndex)
            .item("Access class members by runtime index", &memberAccessByRuntimeIndex)
            .item("Access class members by name", &memberAccessByName)
            .item("Get member access protection modifier", &memberAccessProtectionModifier)
            .item("Get member metadata using identifier", &memberMetadataByIdentifier)
            .item("Pack & fold class members", &memberPackFold)
            .item("Filter members using constexpr ifs", &memberFilterConstexprIf)
            .item("Filter members using RareCpp filtering", &memberFilterPredicate)
            .item("Reflect members outside of class", &memberReflectOutsideOfClass)
            .item("Reflect private members outside of class", &unownedPrivateReflect)
            .item("Reflect noted private members outside of class", &unownedPrivateReflectNoted)
        ).item(
            Menu{"Aggregate Examples", "Select an example: "}
            .item("Auto-reflect aggregates", &autoReflectAggregate)
        ).item(
            Menu{"Note Examples", "Select an example: "}
            .item("Member annotations", &noteClassMembers)
            .item("Class-level annotations", &noteClass)
            .item("Anonymous annotations", &noteAnonymous)
            .item("Filtered notes has/get/forEach", &noteFiltered)
        ).item(
            Menu{"Super-class examples", "Select an example: "}
            .item("Define & iterate super-classes", &superDefineIterate)
            .item("Access super-class by constexpr index", &superAccessByConstexprIndex)
            .item("Access super-class by runtime index", &superAccessByRuntimeIndex)
            .item("Traversing inheritance trees", &superTraverseTree)
        ).item(
            Menu{"Function examples", "Select an example: "}
            .item("Define, iterate & call functions", &functionDefineIterateCall)
            .item("Iteratively build arguments & call function", &functionBuildAndCall)
        ).item(
            Menu{"Overload examples", "Select an example: "}
            .item("Define, iterate & call overloads", &overloadDefineIterateCall)
            .item("Access known overload by constexpr index", &overloadAccessByConstexprIndex)
            .item("Access known overload by runtime index", &overloadAccessByRuntimeIndex)
            .item("Pack known overloads", &overloadPackExpand)
            .item("Access unknown overload (non-noted argument set)", &overloadNonNotedArgumentSets)
            .item("Working with cv-ref qualified overloads", &overloadCvrefQualifications)
        ).item(
            Menu{"Misc examples", "Select an example: "}
            .item("Type to string", &miscTypeToString)
            .item("Runtime to constexpr index", &miscRuntimeToConstexprIndex)
            .item("White-box testing", &miscWhiteboxTesting)
            .item("Making tuples", &miscMakeTuple)
            .item("Tuplifying", &miscTuplify)
            .item("Direct tuplification", &miscDirectTuplification)
        ).item(
            Menu{"Adaptive structure examples", "Select an example: "}
            .item("Clone reflected structure", &adaptiveCloneStructure)
            .item("Typedefs per reflected structure member", &adaptiveTypedefStructure)
            .item("Implementing whiteboxer/reference structure", &adaptiveImplementWhiteboxer)
            .item("Implementing builders", &adaptiveImplementBuilder)
            .item("Implementing change-listeners", &adaptiveImplementChangeListener)
            .item("Implementing object-mappers", &adaptiveImplementMapper)
        ).item(
            Menu{"Builder examples", "Select an example: "}
            .item("Simple builder", &builderSimple)
            .item("Validated builders", &builderValidated)
            .item("Aggregated builders", &builderAggregated)
            .item("Member availability", &builderMemberAvailability)
        ).item(
            Menu{"Mapper examples", "Select an example: "}
            .item("Trivial mapping", &mapperTrivialMapping)
            .item("Custom-field mapping", &mapperCustomFieldMapping)
            .item("Using standard mappings", &mapperUsingStandardMappings)
        ).item(
            Menu{"Json examples", "Select an example: "}
            .item("Basic JSON I/O", &jsonBasic)
            .item("JSON customization", &jsonCustomizations)
            .item("Unknown fields", &jsonUnknownFields)
        ).open();
}

// The main executable/RareCpp project is only for example/experimental purposes only and should not be built into other projects
int main()
{
    // Set to whatever function you want called on startup (reset to &none when finished)
    auto specificCall = &experimental::dataHistory;

    specificCall();
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //openMenu();
    return 0;
}
