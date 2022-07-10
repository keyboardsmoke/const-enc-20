#pragma once

#include <cstdint>
#include <type_traits>

namespace constant
{
    template<typename T, size_t Index>
    struct XorShift32
    {
        static_assert(std::is_same_v<T, uint32_t>, "uint32_t is the only supported type for XorShift32...");

        static consteval T Get()
        {
            T x = 0x64fcc2ffUL;

            for (size_t i = 0; i < Index; ++i)
            {
                x ^= x << 13;
                x ^= x >> 17;
                x ^= x << 5;
            }

            return x;
        }
    };

    template<typename T, size_t Index>
    struct XorShift64
    {
        static_assert(std::is_same_v<T, uint64_t>, "uint64_t is the only supported type for XorShift64...");
        
        static consteval T Get()
        {
            T x = 0x9e3779b97f4a7c16ULL;

            for (size_t i = 0; i < Index; ++i)
            {
                x ^= x << 13;
                x ^= x >> 7;
                x ^= x << 17;
            }

            return x;
        }
    };

/*
    struct xorshift128_state {
        uint32_t x[4];
    };

    uint32_t xorshift128(struct xorshift128_state* state)
    {
        uint32_t t = state->x[3];

        uint32_t s = state->x[0];
        state->x[3] = state->x[2];
        state->x[2] = state->x[1];
        state->x[1] = s;

        t ^= t << 11;
        t ^= t >> 8;
        return state->x[0] = t ^ s ^ (s >> 19);
    }
*/

    struct LinearGenerator
    {
        template<typename EngineT>
        static consteval auto Get()
        {
            return EngineT::Get();
        }
    };
}