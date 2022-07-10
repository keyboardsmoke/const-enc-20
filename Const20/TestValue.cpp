#include "doctest.h"
#include "ConstantValue.h"

TEST_CASE("Value")
{
    constexpr constant::Value<uint64_t> a{ 0x1337ULL };

    REQUIRE(a.Decode() == 0x1337ULL);
}