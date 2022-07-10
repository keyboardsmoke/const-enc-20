#include "doctest.h"
#include "ConstantString.h"

TEST_CASE("String")
{
    auto s1 = constant::String<const char, 14>("Hello, World!");
    auto s2 = constant::String<const wchar_t, 14>(L"Hello, World!");

    [&] () __declspec(noinline)
    {
        REQUIRE(strcmp(s1.Decode(), "Hello, World!") == 0);
        REQUIRE(wcscmp(s2.Decode(), L"Hello, World!") == 0);
    }();
}