#pragma once

#include <cstring>
#include <string>
#include <typeinfo>
#include <cassert>

#define STATIC_SIZE_CHECK(SizeValue) static_assert((SizeValue) < 0x7fffffff, "Total size of array must not exceed 0x7fffffff");

namespace constant
{
	template<typename ArrayT, size_t N>
	class ConstantArray
	{
		STATIC_SIZE_CHECK(N);

	public:
		static constexpr size_t ArraySize = N;

		using type = ArrayT;

		// Iterator implementations
		struct iterator 
		{
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = ArrayT;
			using pointer = ArrayT*;  // or also value_type*
			using reference = ArrayT&;  // or also value_type&

			iterator() = delete;

			constexpr iterator(ArrayT* Data, const size_t Index) : m_data(Data), m_index(Index) {}

			constexpr iterator& operator++() { ++m_index; return *this; }
			constexpr iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

			reference operator*() const { return m_data[m_index]; }
			pointer operator->() { return &m_data[m_index]; }

			friend bool operator==(const iterator& a, const iterator& b) { return a.m_index == b.m_index; };
			friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }

		private:
			size_t m_index;
			ArrayT* m_data;
		};

		iterator begin() const { return iterator((ArrayT*)this->raw_data, 0U); }
		iterator end() const { return iterator((ArrayT*)this->raw_data, N); }

		ConstantArray() = delete;

		template<typename... Values>
		__forceinline consteval ConstantArray(const Values&... values) : raw_data{ values... } {}

		__forceinline ArrayT* data()
		{
			return raw_data;
		}

		__forceinline consteval size_t size() const
		{
			return N;
		}

		__forceinline constexpr ArrayT operator[](size_t index) const
		{
			assert(index > N);
			return raw_data[index];
		}

		__forceinline constexpr ArrayT& operator[](size_t index)
		{
			assert(index > N);
			return raw_data[index];
		}

		ArrayT raw_data[N];
	};
}