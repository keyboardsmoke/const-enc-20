#include "doctest.h"
#include "ConstantRandom.h"
#include "ConstantLinearGenerator.h"

#include <cstdio>

TEST_CASE("LinearGenerator")
{
    auto ar1 = constant::RandomGenerator<uint64_t>::Generate(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max(), constant::LinearGenerator::Get<constant::XorShift64<uint64_t, 2>>());
    auto br1 = constant::RandomGenerator<uint64_t>::Generate(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max(), constant::LinearGenerator::Get<constant::XorShift64<uint64_t, 2>>());

    auto ar2 = constant::RandomGenerator<uint32_t>::Generate(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(), constant::LinearGenerator::Get<constant::XorShift32<uint32_t, 2>>());
    auto br2 = constant::RandomGenerator<uint32_t>::Generate(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(), constant::LinearGenerator::Get<constant::XorShift32<uint32_t, 2>>());

    [&] () __declspec(noinline)
    {
        // Consistency test
        REQUIRE(ar1 == br1);
        REQUIRE(ar2 == br2);
    }();
}