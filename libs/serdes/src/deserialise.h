#ifndef DESERIALISE_H_
#define DESERIALISE_H_

#include "concepts.h"
#include "variable_integer.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <vector>

class Deserialise
{
public:
	explicit constexpr Deserialise(const std::vector<std::byte>& data)
		: m_data {data}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		std::copy_n(std::begin(m_data) + m_pointer, sizeof(T), reinterpret_cast<std::byte*>(&value));
		m_pointer += sizeof(T);
	}

	template <Enum T>
	constexpr void operator()(T& value)
	{
		std::underlying_type_t<T> fundamentalValue {};
		operator()(fundamentalValue);
		value = static_cast<T>(fundamentalValue);
	}

	template <Vector T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {value.size()};
		operator()(size);
		std::for_each(std::begin(value), std::end(value), [this](auto& elem)
			{ operator()(elem); });
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {value.length()};
		operator()(size);
		std::for_each(std::begin(value), std::end(value), [this](auto& elem)
			{ operator()(elem); });
	}

	template <std::same_as<VariableInteger> T>
	constexpr void operator()(T& value)
	{
		value = VariableInteger(m_data.data() + m_pointer);
	}

private:
	const std::vector<std::byte>& m_data;
	std::size_t                   m_pointer {0u};
};

#endif  // DESERIALISE_H_
