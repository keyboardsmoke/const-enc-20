#include "doctest.h"
#include "ConstantHash.h"

#define HASH_STRING "hello"
#define HASH_STRING_LEN 5

TEST_CASE("Hash")
{
    auto a = constant::CRC32::Hash(HASH_STRING, HASH_STRING_LEN);
    auto b = constant::CRC32C::Hash(HASH_STRING, HASH_STRING_LEN);
    auto c = constant::FNV132::Hash(HASH_STRING, HASH_STRING_LEN);
    auto d = constant::FNV1A32::Hash(HASH_STRING, HASH_STRING_LEN);
    auto e = constant::FNV164::Hash(HASH_STRING, HASH_STRING_LEN);
    auto f = constant::FNV1A64::Hash(HASH_STRING, HASH_STRING_LEN);
    auto g = constant::JOAAT::Hash(HASH_STRING, HASH_STRING_LEN);

    [&] () __declspec(noinline)
    {
        REQUIRE(a == 0x3610a686);
        REQUIRE(b == 0x7d34210);
        REQUIRE(c == 0xb6fa7167);
        REQUIRE(d == 0x4f9f2cab);
        REQUIRE(e == 0x7b495389bdbdd4c7ULL);
        REQUIRE(f == 0xa430d84680aabd0bULL);
        REQUIRE(g == 0xc8fd181b);
    }();
}