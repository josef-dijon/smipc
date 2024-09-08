#ifndef CALCULATE_MINIMUM_SIZE_H_
#define CALCULATE_MINIMUM_SIZE_H_

#include "concepts.h"
#include "variable_integer.h"

#include <cstdint>

class CalculateMinimumSize
{
public:
	explicit constexpr CalculateMinimumSize(std::size_t& size)
		: m_size {size}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		m_size += sizeof(T);
	}

	template <Enum T>
	constexpr void operator()(T& value)
	{
		m_size += sizeof(std::underlying_type_t<T>);
	}

	template <Vector T>
	constexpr void operator()(T& value)
	{
		constexpr VariableInteger varInt {0u};
		m_size += varInt.size();
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		constexpr VariableInteger varInt {0u};
		m_size += varInt.size();
	}

private:
	std::size_t& m_size;
};

#endif  // CALCULATE_MINIMUM_SIZE_H_
