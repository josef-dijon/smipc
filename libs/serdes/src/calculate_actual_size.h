#ifndef CALCULATE_ACTUAL_SIZE_H_
#define CALCULATE_ACTUAL_SIZE_H_

#include "concepts.h"
#include "variable_integer.h"

#include <cstdint>
#include <type_traits>

class CalculateActualSize
{
public:
	explicit constexpr CalculateActualSize(std::size_t& size)
		: m_size {size}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		m_size += sizeof(T);
	}

	template <VarInt T>
	constexpr void operator()(T& value)
	{
		m_size += value.size();
	}

	template <Enum T>
	constexpr void operator()(T& value)
	{
		m_size += sizeof(std::underlying_type_t<T>);
	}

	template <Vector T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {value.size()};
		m_size += size.size();

		for_each(std::begin(value), std::end(value), [this](auto& elem)
			{ operator()(elem); });
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {value.length()};
		m_size += size.size();
		m_size += sizeof(typename T::value_type) * value.length();
	}

private:
	std::size_t& m_size;
};

#endif  // CALCULATE_ACTUAL_SIZE_H_
