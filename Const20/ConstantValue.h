#pragma once

#include "ConstantCipher.h"
#include "ConstantArray.h"
#include "ConstantKeyGenerator.h"

namespace constant
{
    template<typename T, size_t Index = 0, size_t MinKey = 3, size_t MaxKey = 6>
    struct Value
    {
        using KeyT = T;

        static constexpr uint64_t mtseed = time::_detail::GetSystemTimePoint().time_since_epoch().count() * constant::LinearGenerator::Get<constant::XorShift64<uint64_t, Index>>();

        static constexpr size_t KeyCount = RandomGenerator<size_t>::Generate(MinKey, MaxKey, mtseed);

        Value() = delete;

        consteval Value(const T value) :
            Generator{}, EncodedValue{ Initialize(value) } {}

        T Decode() const
        {
            return Generator.GetDecodedValueRuntime(EncodedValue);
        }

        KeyGenerator<KeyT, T, KeyCount> Generator;
        KeyT EncodedValue;

    private:
        consteval KeyT Initialize(const T value)
        {
            return Generator.GetEncodedValueConstant(value);
        }
    };
}