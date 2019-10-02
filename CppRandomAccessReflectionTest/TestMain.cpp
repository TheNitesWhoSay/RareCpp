#include <gtest/gtest.h>

#ifdef _WIN32
#ifdef UNICODE
#define ENTRY_POINT
int wmain(int argc, wchar_t* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    return result;
}
#endif
#endif

#ifndef ENTRY_POINT
int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    return result;
}
#endif
