#include <rarecpp/reflect.h>
#include <iostream>

inline namespace member_access_by_name
{
    struct MyObj
    {
        int myInt = 0;
        long myLong = 0;
        char myChar = '0';

        REFLECT(MyObj, myInt, myLong, myChar)
    };

    void printObj(const MyObj & myObj) {
        std::cout << "{ " << "myInt:" << myObj.myInt << ", myLong:" << myObj.myLong << ", myChar:" << myObj.myChar << " }" << std::endl;
    }

    template <typename T>
    constexpr auto parseMember(T & obj, std::string_view memberInput)
    {
        size_t colon = memberInput.find_first_of(":");
        std::string_view name = memberInput.substr(0, colon);
        std::string_view newValue = memberInput.substr(colon+1, memberInput.size());
        RareTs::Values<T>::named(name, obj, [&](auto & value) {
            value = std::remove_reference_t<decltype(value)>(int(newValue[0]-'0')*10+int(newValue[1]-'0'));
        });
    }

    constexpr auto parseInput(std::string_view input)
    {
        MyObj myObj {};
        size_t commaIndex = input.find_first_of(",");
        parseMember(myObj, input.substr(0, commaIndex));
        parseMember(myObj, input.substr(commaIndex+1, input.size()));
        return myObj;
    }

    void memberAccessByName()
    {
        constexpr std::string_view input = "myInt:77,myChar:99";
        constexpr auto myConstexprObj = parseInput(input); // Makes use of access by constexpr member name from "input"
        std::cout << "constexpr input \"" << input << "\" parsed to constexpr object: ";
        printObj(myConstexprObj);

        std::string memberName {};
        std::cout << "Enter a member name: ";
        std::cin >> memberName;
    
        RareTs::Values<MyObj>::named(memberName, myConstexprObj, [&](auto & value) { // Makes use of runtime-provided member name
            std::cout << "Member " << memberName << " had value: " << value << std::endl;
        });
    }
}
