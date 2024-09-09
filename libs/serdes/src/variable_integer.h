#ifndef VARIABLE_INTEGER_H_
#define VARIABLE_INTEGER_H_

#include <array>
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

	const std::byte* bytes() const
	{
		return reinterpret_cast<const std::byte*>(m_bytes.data());
	}

private:
	std::array<uint8_t, 9u> m_bytes {};
	std::size_t             m_count {1u};
};

#endif  // VARIABLE_INTEGER_H_
