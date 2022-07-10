#pragma once

#include <type_traits>
#include <cstdint>

#include "ConstantArray.h"

namespace constant
{
    struct CRC32
    {
		static consteval auto Hash(const char* p, size_t size)
		{
			auto crc = ~std::uint_fast32_t{ 0 } &std::uint_fast32_t{ 0xFFFFFFFFuL };

			while (size--)
				crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
			
			return crc ^ ~0U;
		}

	private:
		template<uint32_t Index>
		static consteval uint32_t ComputeTableIndex()
		{
			auto Idx = Index;
			auto checksum = Idx++;

			for (auto i = 0; i < 8; ++i)
				checksum = (checksum >> 1) ^ ((checksum & 0x1u) ? reversed_polynomial : 0);

			return checksum;
		}

		template<size_t... Indices>
		static consteval auto GenerateTable(std::index_sequence<Indices...>)
		{
			return ConstantArray<uint32_t, 256> { (ComputeTableIndex<Indices>())... };
		}

		// make exclusive-or pattern from polynomial (0xedb88320L)
		constexpr static uint32_t reversed_polynomial{ 0xedb88320L };
		constexpr static ConstantArray<uint32_t, 256> crc32_tab = { CRC32::GenerateTable(std::make_index_sequence<256>()) };
    };

	struct CRC32C
	{
		static consteval auto Hash(const char* p, size_t size)
		{
			auto crc = ~std::uint_fast32_t{ 0 } &std::uint_fast32_t{ 0xFFFFFFFFuL };

			while (size--)
				crc = crc32_tab[(crc ^ *p++) & 0xff] ^ (crc >> 8);

			return crc;
		}

	private:
		template<uint32_t Index>
		static consteval uint32_t ComputeTableIndex()
		{
			auto Idx = Index;
			auto checksum = Idx++;

			for (auto i = 0; i < 8; ++i)
				checksum = (checksum >> 1) ^ ((checksum & 0x1u) ? reversed_polynomial : 0);

			return checksum;
		}

		template<size_t... Indices>
		static consteval auto GenerateTable(std::index_sequence<Indices...>)
		{
			return ConstantArray<uint32_t, 256> { (ComputeTableIndex<Indices>())... };
		}

		constexpr static uint32_t reversed_polynomial{ 0x1EDC6F41L };
		constexpr static ConstantArray<uint32_t, 256> crc32_tab = { CRC32C::GenerateTable(std::make_index_sequence<256>()) };
	};

	struct FNV1A32
	{
		static consteval auto Hash(const char* p, size_t size)
		{
			uint32_t value = offset_basis;

			for (size_t i = 0; i < size; ++i)
			{
				value = ((value ^ uint32_t(p[i])) * prime);
			}

			return value;
		}

	private:
		static constexpr uint32_t offset_basis = 0x811c9dc5UL;
		static constexpr uint32_t prime = 0x01000193UL;
	};

	struct FNV132
	{
		static consteval auto Hash(const char* p, size_t size)
		{
			uint32_t value = offset_basis;

			for (size_t i = 0; i < size; ++i)
			{
				value = ((value * prime) ^ uint32_t(p[i]));
			}

			return value;
		}

	private:
		static constexpr uint32_t offset_basis = 0x811c9dc5UL;
		static constexpr uint32_t prime = 0x01000193UL;
	};

	struct FNV1A64
	{
		static consteval auto Hash(const char* p, size_t size)
		{
			uint64_t value = offset_basis;

			for (size_t i = 0; i < size; ++i)
			{
				value = ((value ^ uint64_t(p[i])) * prime);
			}

			return value;
		}

	private:
		static constexpr uint64_t offset_basis = 0xcbf29ce484222325ULL;
		static constexpr uint64_t prime = 0x00000100000001B3ULL;
	};

	struct FNV164
	{
		static consteval auto Hash(const char* p, size_t size)
		{
			uint64_t value = offset_basis;

			for (size_t i = 0; i < size; ++i)
			{
				value = ((value * prime) ^ uint32_t(p[i]));
			}

			return value;
		}

	private:
		static constexpr uint64_t offset_basis = 0xcbf29ce484222325ULL;
		static constexpr uint64_t prime = 0x00000100000001B3ULL;
	};

	struct JOAAT
	{
		static consteval auto Hash(const char* p, size_t size)
		{
			uint32_t value = 0UL;

			for (size_t i = 0; i < size; ++i)
			{
				value += static_cast<uint32_t>(p[i]);
				value += (value << 10);
				value ^= (value >> 6);
			}

			value += (value << 3);
			value ^= (value >> 11);
			value += (value << 15);

			return value;
		}
	};
}