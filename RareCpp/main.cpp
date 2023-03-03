#include "menu.h"

// Experimental
namespace experimental {
    void rest();
    void refactoring();
}

// Legacy examples
void basicReflectionExamples();
void globalReflectionExample();
void annotatedSupersExample();
void objectMapperExamples();
void dynamicJsonIoExample();
void jsonCustomizationExample();
void functionReflectionExample();

// Member examples
void memberDefineIterateModify();
void memberAccessByConstexprIndex();
void memberAccessByRuntimeIndex();
void memberAccessByName();
void memberAccessProtectionModifier();
void memberMetadataByIdentifier();
void memberPackFold();
void memberFilterConstexprIf();
void memberFilterPredicate();

// Note examples
void noteClassMembers();
void noteClass();
void noteAnonymous();
void noteFiltered();

// Super-class examples
void superDefineIterate();
void superAccessByConstexprIndex();
void superAccessByRuntimeIndex();
void superTraverseTree();

// Function examples
void functionDefineIterateCall();
void functionArgumentAndReturnTypes();
void functionBuildAndCall();

// Overload examples
void overloadDefineIterateCall();
void overloadAccessByConstexprIndex();
void overloadAccessByRuntimeIndex();
void overloadPackFold();
void overloadNonNotedArgumentSets();
void overloadCvrefQualifications();

// Misc examples
void miscTypeToString();
void miscRuntimeToConstexprIndex();
void miscWhiteboxTesting();
void miscMakeTuple();

// Adaptive structure examples
void adaptiveCloneStructure();
void adaptiveTypedefStructure();
void adaptiveImplementWhiteboxer();
void adaptiveImplementBuilder();
void adaptiveImplementChangeListener();
void adaptiveImplementMapper();

// Builder examples
void builderSimple();
void builderValidated();
void builderAggregated();

// Mapper examples
void mapperTrivialMapping();
void mapperCustomFieldMapping();
void mapperUsingStandardMappings();

// Json examples
void jsonBasic();
void jsonCustomizations();
void jsonUnknownFields();

// No-op
void none() {}

void openMenu()
{
    Menu("Select a category")
        .item(
            Menu("Experimental", "Select an experiment: ")
            .item("Rest experiment", &experimental::rest)
            .item("Refactoring experiment", &experimental::refactoring)
        ).item(
            Menu("Legacy examples", "Select an example: ")
            .item("Basic reflection", &basicReflectionExamples)
            .item("Global reflection", &globalReflectionExample)
            .item("Annotated supers", &annotatedSupersExample)
            .item("Object mapper", &objectMapperExamples)
            .item("Dynamic JSON I/O", &dynamicJsonIoExample)
            .item("Function reflection", &functionReflectionExample)
            .item("Json customization", &jsonCustomizationExample)
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
            .item("Get argument & return types", &functionArgumentAndReturnTypes)
            .item("Iteratively build arguments & call function", &functionBuildAndCall)
        ).item(
            Menu{"Overload examples", "Select an example: "}
            .item("Define, iterate & call overloads", &overloadDefineIterateCall)
            .item("Access known overload by constexpr index", &overloadAccessByConstexprIndex)
            .item("Access known overload by runtime index", &overloadAccessByRuntimeIndex)
            .item("Pack known overloads", &overloadPackFold)
            .item("Access unknown overload (non-noted argument set)", &overloadNonNotedArgumentSets)
            .item("Working with cv-ref qualified overloads", &overloadCvrefQualifications)
        ).item(
            Menu{"Misc examples", "Select an example: "}
            .item("Type to string", &miscTypeToString)
            .item("Runtime to constexpr index", &miscRuntimeToConstexprIndex)
            .item("White-box testing", &miscWhiteboxTesting)
            .item("Making tuples", &miscMakeTuple)
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
    auto specificCall = &none;

    specificCall();
    openMenu();
    return 0;
}
