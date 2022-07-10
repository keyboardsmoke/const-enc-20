#pragma once

namespace constant
{
    enum class Ciphers
    {
        Xor,
        SubAdd,

        Max
    };

    template<typename DataT, typename KeyT, Ciphers CipherType>
    struct Cipher;

    template<typename DataT, typename KeyT>
    struct Cipher<DataT, KeyT, Ciphers::Xor>
    {
        __forceinline static consteval KeyT EncodeConstant(const DataT value, const KeyT key)
        {
            return key ^ value;
        }

        __forceinline static KeyT EncodeRuntime(const DataT value, const KeyT key)
        {
            return key ^ value;
        }

        __forceinline static DataT Decode(const KeyT value, const KeyT key)
        {
            return key ^ value;
        }
    };

    template<typename DataT, typename KeyT>
    struct Cipher<DataT, KeyT, Ciphers::SubAdd>
    {
        __forceinline static consteval KeyT EncodeConstant(const DataT value, const KeyT key)
        {
            return value - key;
        }

        __forceinline static KeyT EncodeRuntime(const DataT value, const KeyT key)
        {
            return value - key;
        }

        __forceinline static DataT Decode(const KeyT value, const KeyT key)
        {
            return value + key;
        }
    };

    //
}