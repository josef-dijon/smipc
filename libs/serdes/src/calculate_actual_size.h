#ifndef CALCULATE_ACTUAL_SIZE_H_
#define CALCULATE_ACTUAL_SIZE_H_

#include "concepts.h"
#include "variable_integer.h"

#include <cstdint>

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

	template <Enum T>
	constexpr void operator()(T& value)
	{
		m_size += sizeof(std::underlying_type_t<T>);
	}

	template <Vector T>
	constexpr void operator()(T& value)
	{
		const VariableInteger varInt {value.size()};
		m_size += varInt.size();

		for (auto& elem : value)
		{
			operator()(elem);
		}
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		const VariableInteger varInt {value.length()};
		m_size += varInt.size();

		m_size += sizeof(typename T::value_type) * value.length();
	}

private:
	std::size_t& m_size;
};

#endif  // CALCULATE_ACTUAL_SIZE_H_
