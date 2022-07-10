#pragma once

#include <cstdint>
#include <type_traits>
#include <random>

#include "ConstantTime.h"
#include "ConstantMersenne.h"
#include "ConstantDistribution.h"

namespace constant
{
    template<typename T>
    struct RandomGenerator
    {
        using const_engine32 = dist::const_mersenne_twister_engine<unsigned int, 32, 624, 397, 31, 0x9908b0df, 11, 0xffffffff, 7, 0x9d2c5680, 15, 0xefc60000, 18, 1812433253>;
        using const_engine64 = dist::const_mersenne_twister_engine<unsigned long long, 64, 312, 156, 31, 0xb5026f5aa96619e9ULL, 29, 0x5555555555555555ULL, 17, 0x71d67fffeda60000ULL, 37, 0xfff7eee000000000ULL, 43, 6364136223846793005ULL>;

        using DistT =
            std::conditional_t<std::is_floating_point_v<T>, const_uniform_real_distribution<T>,
            std::conditional_t<std::is_integral_v<T>, const_uniform_int_distribution<T>, void>>;

        using EngineT = std::conditional_t<sizeof(T) == sizeof(uint64_t), const_engine64, const_engine32>;
        using SeedT = std::conditional_t<sizeof(T) == sizeof(uint64_t), uint64_t, uint32_t>;

        static_assert(!std::is_void_v<DistT>, "Invalid distribution");

        static consteval T Generate(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max(), uint64_t mtseed = 0ULL)
        {
            auto seed = static_cast<uint64_t>((mtseed == 0ULL) ? time::_detail::GetSystemTimePoint().time_since_epoch().count() : mtseed);
            EngineT engine(static_cast<SeedT>(seed));
            DistT dist(min, max);
            return dist(engine);
        }
    };
}