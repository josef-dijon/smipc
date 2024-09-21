#ifndef DESERIALISE_H_
#define DESERIALISE_H_

#include "concepts.h"
#include "variable_integer.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <span>
#include <type_traits>

class Deserialise
{
public:
	explicit constexpr Deserialise(std::span<const std::byte> data) noexcept
		: m_data {data}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		readBytes({reinterpret_cast<std::byte*>(&value), sizeof(T)});
	}

	template <VarInt T>
	constexpr void operator()(T& value)
	{
		value = VariableInteger(m_data.data() + m_pointer);
		m_pointer += value.size();
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

private:
	constexpr void readBytes(std::span<std::byte> data)
	{
		std::copy_n(std::begin(m_data) + m_pointer, data.size(), std::begin(data));
		m_pointer += data.size();
	}

	std::span<const std::byte> m_data;
	std::size_t m_pointer {0u};
};

#endif  // DESERIALISE_H_
