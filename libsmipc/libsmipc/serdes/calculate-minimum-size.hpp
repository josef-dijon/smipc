#ifndef CALCULATE_MINIMUM_SIZE_H_
#define CALCULATE_MINIMUM_SIZE_H_

#include <libsmipc/serdes/concepts.hpp>
#include <libsmipc/serdes/variable-integer.hpp>

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
		VariableInteger size {std::size_t {0u}};
		operator()(size);
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {std::size_t {0u}};
		operator()(size);
	}

private:
	std::size_t& m_size;
};

#endif  // CALCULATE_MINIMUM_SIZE_H_
