#ifndef SERIALISE_H_
#define SERIALISE_H_

#include "concepts.h"
#include "variable_integer.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <vector>

class Serialise
{
public:
	explicit constexpr Serialise(std::vector<std::byte>& data)
		: m_data {data}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		std::copy_n(reinterpret_cast<const std::byte*>(&value), sizeof(T), std::back_inserter(m_data));
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

	template <VarInt T>
	constexpr void operator()(T& value)
	{
		std::copy_n(std::begin(value.bytes()), value.size(), std::back_inserter(m_data));
	}

private:
	std::vector<std::byte>& m_data;
};

#endif  // SERIALISE_H_
