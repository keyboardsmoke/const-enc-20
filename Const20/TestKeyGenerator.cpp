#include "doctest.h"
#include "ConstantArray.h"
#include "ConstantKeyGenerator.h"

static void test1()
{
    using GenT = constant::KeyGenerator<uint32_t, uint16_t, 10>;

    auto Gen = GenT{};

    auto a = Gen.GetEncodedValueConstant(0x1337);
    auto b = Gen.GetEncodedValueRuntime(0x1337);

    auto a2 = Gen.GetDecodedValueRuntime(a);
    auto b2 = Gen.GetDecodedValueRuntime(b);

    [&] () __declspec(noinline)
    {
        REQUIRE(Gen.GenerateKey<0>() == Gen.GetKeyRuntime(0));
        REQUIRE(Gen.GenerateKey<1>() == Gen.GetKeyRuntime(1));
        REQUIRE(Gen.GenerateKey<2>() == Gen.GetKeyRuntime(2));
        REQUIRE(Gen.GenerateKey<3>() == Gen.GetKeyRuntime(3));
        REQUIRE(Gen.GenerateKey<4>() == Gen.GetKeyRuntime(4));
        REQUIRE(Gen.GenerateKey<5>() == Gen.GetKeyRuntime(5));
        REQUIRE(Gen.GenerateKey<6>() == Gen.GetKeyRuntime(6));
        REQUIRE(Gen.GenerateKey<7>() == Gen.GetKeyRuntime(7));
        REQUIRE(Gen.GenerateKey<8>() == Gen.GetKeyRuntime(8));
        REQUIRE(Gen.GenerateKey<9>() == Gen.GetKeyRuntime(9));

        REQUIRE(a == b);
        REQUIRE(a2 == b2);
    }();
}

static void test2()
{
    using GenT = constant::KeyGenerator<uint16_t, uint32_t, 2>;

    auto Gen = GenT{};

    auto a = Gen.GetEncodedValueConstant(0x1337);
    auto b = Gen.GetEncodedValueRuntime(0x1337);

    auto a2 = Gen.GetDecodedValueRuntime(a);
    auto b2 = Gen.GetDecodedValueRuntime(b);

    [&] () __declspec(noinline)
    {
        REQUIRE(Gen.GenerateKey<0>() == Gen.GetKeyRuntime(0));
        REQUIRE(Gen.GenerateKey<1>() == Gen.GetKeyRuntime(1));
        REQUIRE(a == b);
        REQUIRE(a2 == b2);
    }();
}

TEST_CASE("Key Generator")
{
    test1();
    test2();
}