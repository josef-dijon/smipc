#ifndef VARIABLE_INTEGER_H_
#define VARIABLE_INTEGER_H_

#include <concepts>
#include <cstdint>

class VariableInteger
{
public:
	template <std::unsigned_integral T>
	constexpr VariableInteger(T value)
		: m_count {sizeof(T)}
	{}

	constexpr std::size_t size() const
	{
		return m_count;
	}

private:
	std::size_t m_count;
};

#endif  // VARIABLE_INTEGER_H_
