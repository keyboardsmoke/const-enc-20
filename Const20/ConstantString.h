#pragma once

#include <type_traits>

#include "ConstantArray.h"
#include "ConstantKeyGenerator.h"
#include "ConstantValue.h"

namespace constant
{
    // This is needed so we can use T, and T*, yeah it's annoying...
    template<typename T>
    struct StringConstructor
    {
        using CharT = T;
        using PtrT = T*;

        template<PtrT Value>
        static consteval auto Construct()
        {
            return String<T, Value>();
        }
    };

    template<typename CharT, size_t Length>
    struct String
    {
        using PtrT = CharT*;

        static_assert(Length != 0, "Length cannot be zero.");
        static_assert(std::is_const_v<CharT>, "String char type must be const.");

        static constexpr size_t DataSize = Length * sizeof(CharT);

        static constexpr bool IsBlock8 = (DataSize == 1);
        static constexpr bool IsBlock16 = ((DataSize % 2) == 0);
        static constexpr bool IsBlock32 = ((DataSize % 4) == 0);
        static constexpr bool IsBlock64 = ((DataSize % 8) == 0);

        // Select the granularity to encrypt the string at
        // It can use uint8, uint16, uint32, uint64
        using BlockT =
            std::conditional_t<IsBlock8 == true, uint8_t,
            std::conditional_t<IsBlock64 == true, uint64_t,
            std::conditional_t<IsBlock32 == true, uint32_t,
            std::conditional_t<IsBlock16 == true, uint16_t, void>>>>;

        static_assert(!std::is_same_v<BlockT, void>, "Invalid input size.");

        static constexpr size_t BlockSize = sizeof(BlockT);

        using ValueT = Value<BlockT, Length, 1, 2>;

        consteval String(CharT S[Length]) : m_data{ Initialize(std::make_index_sequence<Length>(), S) } {}

        __forceinline PtrT Decode()
        {
            using RetT = std::remove_const_t<CharT>;

            RetT* p = reinterpret_cast<RetT*>(m_data.data());

            for (size_t i = 0; i < Length; ++i)
            {
                // Stomp on the original data...
                p[i] = static_cast<RetT>(m_data[i].Decode());
            }

            return p;
        }

    private:
        template<std::size_t... Is>
        __forceinline consteval auto Initialize(std::index_sequence<Is...>, PtrT S)
        {
            return ConstantArray<ValueT, Length> { ValueT(S[Is])... };
        }

        ConstantArray<ValueT, Length> m_data;
    };
}