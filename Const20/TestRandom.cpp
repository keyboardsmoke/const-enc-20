#include "doctest.h"
#include "ConstantRandom.h"

#define GEN_RANDOM(x, min, max) (constant::RandomGenerator<x>::Generate((min), (max)));
#define GEN_RANDOM_MINMAX(x) GEN_RANDOM(x, std::numeric_limits<x>::min(), std::numeric_limits<x>::max())

TEST_CASE("Random")
{
    auto f = GEN_RANDOM(float, 1.0f, 6.0f);
    auto d = GEN_RANDOM(double, 1.0, 6.0);
    auto a1 = GEN_RANDOM_MINMAX(uint8_t);
    auto a2 = GEN_RANDOM_MINMAX(uint16_t);
    auto a3 = GEN_RANDOM_MINMAX(uint32_t);
    auto a4 = GEN_RANDOM_MINMAX(uint64_t);
    auto a5 = GEN_RANDOM(uint16_t, 10, 13);

    [&] () __declspec(noinline)
    {
        REQUIRE(f >= 1.0f);
        REQUIRE(f <= 6.0f);
        REQUIRE(d >= 1.0);
        REQUIRE(d <= 6.0);
        REQUIRE(a1 >= 0);
        REQUIRE(a1 <= std::numeric_limits<decltype(a1)>::max());
        REQUIRE(a2 >= 0);
        REQUIRE(a2 <= std::numeric_limits<decltype(a2)>::max());
        REQUIRE(a3 >= 0);
        REQUIRE(a3 <= std::numeric_limits<decltype(a3)>::max());
        REQUIRE(a4 >= 0);
        REQUIRE(a4 <= std::numeric_limits<decltype(a4)>::max());
        REQUIRE(a5 >= 0);
        REQUIRE(a5 <= std::numeric_limits<decltype(a5)>::max());
    }();
}

template<typename T, size_t S>
static void MatchRandom()
{
    auto a1 = constant::RandomGenerator<T>::Generate(std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), S);
    auto a2 = constant::RandomGenerator<T>::Generate(std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), S);

    [&] () __declspec(noinline)
    {
        REQUIRE(a1 == a2);
    }();
}

TEST_CASE("Random Consistency")
{
    MatchRandom<uint8_t, 2>();
    MatchRandom<uint16_t, 0xff32>();
    MatchRandom<uint32_t, 0x882a7020>();
    MatchRandom<uint64_t, 0xDDEEAADDBBEEFFFFULL>();
}