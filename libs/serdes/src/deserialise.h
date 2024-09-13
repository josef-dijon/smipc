#ifndef DESERIALISE_H_
#define DESERIALISE_H_

#include "concepts.h"
#include "variable_integer.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <vector>

class Deserialise
{
public:
	explicit constexpr Deserialise(std::vector<std::byte>& data)
		: m_data {data}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		std::copy_n(std::begin(m_data) + m_dataPointer, sizeof(T), std::reinterpret_cast<std::byte*>(&value));
		m_dataPointer += sizeof(T);
	}

	template <Enum T>
	constexpr void operator()(T& value)
	{
		const auto fundamentalValue {static_cast<std::underlying_type_t<T>>(value)};
		operator()(fundamentalValue);
	}

	template <Vector T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {value.size()};
		operator()(size);
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {value.length()};
		operator()(size);
	}

	template <std::same_as<VariableInteger> T>
	constexpr void operator()(T& value)
	{
		std::copy_n(std::begin(m_data) + m_dataPointer, sizeof(T), value.bytes());
		m_dataPointer += sizeof(T);
	}

private:
	std::vector<std::byte>& m_data;
	std::size_t m_dataPointer {};
};

#endif  // DESERIALISE_H_
