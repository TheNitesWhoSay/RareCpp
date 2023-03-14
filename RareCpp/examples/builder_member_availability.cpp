#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <iostream>

inline namespace builder_member_availability
{
    /** Different members may be available to a builder depending on certain conditions:
    * 
    *                              (default) - public only
    *         member note BuilderIgnore used - all unignored
    * bool or void validate() method present - all
    *             class-level note Buildable - all
    *         class-level note BuilderIgnore - none
    */

    /** Private and protected variables are only default-available to a builder if...
    *       - the class is validatable
    *       - the class signals intention to be used with builders
    *                      
    *   This helps prevent unintentional violation of class invariants when using builders ubiquitously
    */
    class Default
    {
        int priv = 0;

    protected:
        int prot = 0;

    public:
        int pub = 0;

        REFLECT(Default, priv, prot, pub)
    };

    class MemberBuilderIgnore
    {
        int priv = 0; // Use of BuilderIgnore on "pub" signals intent to use builders with the class, so private/protected variables are available by default

    protected:
        int prot = 0;

    public:
        NOTE(pub, RareTs::BuilderIgnore)
        int pub = 0; // This member was ignored, so it's not available to builders

        int otherPub = 0;

        REFLECT(MemberBuilderIgnore, priv, prot, pub, otherPub)
    };

    class BoolValidatable
    {
        int a = 0;
        int b = 0;

    public:
        REFLECT(BoolValidatable, a, b)

        bool validate() { return a >= 0 && b >= 0; } // bool or void validate method (which ".build()" will call) makes private/protected varaiables available
    };

    NOTE(ClassBuildable, RareTs::Buildable) // Use of class-level annotation "Buildable" signals intent to use builders with the class, 
    class ClassBuildable
    {
        int a = 0;
        int b = 0;

        REFLECT_NOTED(ClassBuildable, a, b)
    };

    NOTE(ClassBuilderIgnore, RareTs::BuilderIgnore) // Use of class-level annotation "BuilderIgnore" prevents use of builders on this class
    class ClassBuilderIgnore
    {
        int a = 0;
        int b = 0;

        REFLECT_NOTED(ClassBuilderIgnore, a, b)
    };

    void builderMemberAvailability()
    {
        //RareBuilder<Default>().priv(1).build(); // Compiler error, privs not available by default
        //RareBuilder<Default>().prot(1).build(); // Compiler error, prots not available by default
        auto defaulted = RareBuilder<Default>().pub(1).build();
    
        //RareBuilder<MemberBuilderIgnore>().pub(1).build(); // Compiler error, ignored member
        auto membered = RareBuilder<MemberBuilderIgnore>().priv(1).prot(2).otherPub(3).build();
    
        auto validated = RareBuilder<BoolValidatable>().a(1).b(2).build();
    
        auto buildable = RareBuilder<ClassBuildable>().a(1).b(2).build();

        //RareBuilder<ClassBuilderIgnore>().build(); // Compiler error, ignored class

        std::cout << "Default: " << Json::pretty(defaulted) << std::endl;
        std::cout << "MemberBuilderIgnore: " << Json::pretty(membered) << std::endl;
        std::cout << "BoolValidatable: " << Json::pretty(validated) << std::endl;
        std::cout << "ClassBuildable: " << Json::pretty(buildable) << std::endl;
    }
}
