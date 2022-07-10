#pragma once

#include <type_traits>

#include "ConstantCipher.h"
#include "ConstantRandom.h"
#include "ConstantLinearGenerator.h"

namespace constant
{
    template<typename KeyT, typename DataT, size_t KeyCount>
    struct KeyGenerator
    {
        constexpr static size_t KeyCount = KeyCount;

        using StorageT = std::remove_const_t<DataT>;

        using CipherT = Cipher<StorageT, KeyT, static_cast<Ciphers>(RandomGenerator<size_t>::Generate(0, static_cast<size_t>(Ciphers::Max) - 1))>;

        __forceinline consteval KeyGenerator() :
            m_keys{ InitializeKeyArray(std::make_index_sequence<KeyCount>()) } {}

        __forceinline consteval size_t GetKeyCount() const { return KeyCount; }

        template<size_t Index>
        __forceinline static consteval KeyT GenerateKey()
        {
            return RandomGenerator<KeyT>::Generate(std::numeric_limits<KeyT>::min(), std::numeric_limits<KeyT>::max(), LinearGenerator::Get<XorShift64<uint64_t, Index>>());
        }

        __forceinline KeyT GetKeyRuntime(size_t index)
        {
            return m_keys[index];
        }

        __forceinline KeyT GetEncodedValueRuntime(StorageT value) const
        {
            volatile KeyT ret = value;

            for (size_t i = 0; i < KeyCount; ++i)
            {
                ret = CipherT::EncodeRuntime(ret, m_keys[i]);
            }

            return ret;
        }

        __forceinline DataT GetDecodedValueRuntime(const StorageT value) const
        {
            KeyT ret = value;

            for (size_t i = KeyCount; i-- > 0;)
            {
                ret = CipherT::Decode(ret, m_keys[i]);
            }

            return ret;
        }

        template<size_t Index>
        struct KeyApplicatorConstant
        {
            __forceinline static consteval KeyT ApplySingleStage(KeyT value)
            {
                auto k = GenerateKey<Index>();
                auto e = CipherT::EncodeConstant(value, k);
                return e;
            }

            __forceinline static consteval KeyT GetValue(KeyT value)
            {
                using NextApplicatorT = KeyApplicatorConstant<Index + 1>;
                
                auto k = GenerateKey<Index>();
                auto e = CipherT::EncodeConstant(value, k);
                auto r = NextApplicatorT::GetValue(e);

                return r;
            }
        };

        template<>
        struct KeyApplicatorConstant<KeyCount>
        {
            __forceinline static consteval KeyT GetValue(KeyT value) { return value; }
        };

        __forceinline consteval KeyT GetEncodedValueConstant(StorageT value) const
        {
            volatile auto v = KeyApplicatorConstant<0U>::GetValue(value);

            return v;
        }

    private:
        template<std::size_t... Is>
        __forceinline consteval ConstantArray<KeyT, KeyCount> InitializeKeyArray(std::index_sequence<Is...>)
        {
            return ConstantArray<KeyT, KeyCount> { static_cast<KeyT>(GenerateKey<Is>())... };
        }

        ConstantArray<KeyT, KeyCount> m_keys;
    };
}