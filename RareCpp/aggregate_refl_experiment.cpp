#include <rarecpp/reflect.h>
#include <iostream>

namespace experimental
{
    struct QTest
    {
        int a = 0;
        const int some_fairly_long_complex_identifier = 1;
    };

    static_assert(2 == RareTs::Members<QTest>::total);
    static_assert(std::is_same_v<int, typename RareTs::Member<QTest, 0>::type>);
    static_assert(std::is_same_v<const int, typename RareTs::Member<QTest, 1>::type>);
    static_assert(0 == RareTs::Member<QTest, 0>::value(QTest{}));
    static_assert(1 == RareTs::Member<QTest, 1>::value(QTest{}));
    static_assert(std::string_view{"a"} == RareTs::Member<QTest, 0>::name);
    static_assert(std::string_view{"some_fairly_long_complex_identifier"} == RareTs::Member<QTest, 1>::name);

    void aggregates()
    {
        RareTs::Members<QTest>::forEach(QTest{}, [&](auto member, auto & value) {
            std::cout << member.name << ": " << value << std::endl;
        });
        std::string a = "a";
        RareTs::Members<QTest>::named("a", [&](auto member) {
            std::cout << "Found \"" << member.name << "\" at index " << member.index << "\n";
        });
        RareTs::Members<QTest>::named("some_fairly_long_complex_identifier", [&](auto member) {
            std::cout << "Found \"" << member.name << "\" at index " << member.index << "\n";
        });
    }
}
