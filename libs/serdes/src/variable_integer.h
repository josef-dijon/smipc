#ifndef VARIABLE_INTEGER_H_
#define VARIABLE_INTEGER_H_

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>

static constexpr bool IsTopBitSet(std::byte b)
{
	return (b & std::byte(0x80)) != std::byte(0u);
}

class VariableInteger
{
public:
	template <std::unsigned_integral T>
	explicit constexpr VariableInteger(T value)
		: m_count {9u + 1u}
	{
		for_each(std::begin(m_bytes), std::end(m_bytes), [value, i = 0](std::byte& b) mutable
			{ b = static_cast<std::byte>(static_cast<uint64_t>(value) >> i * 7u) & std::byte(0x7F); });

		for_each(std::crbegin(m_bytes), std::crend(m_bytes), [this](std::byte b)
			{ m_count -= std::size_t(! IsTopBitSet(b)); });
	}

	explicit constexpr VariableInteger(const std::byte* bytes)
	{
		for (std::size_t i {0u}; i < m_bytes.size(); ++i)
		{
			// TODO: This needs fixing, we need to shift by 7
			m_bytes[i] = bytes[i];
			++m_count;

			if (! IsTopBitSet(bytes[i]))
			{
				break;
			}
		}
	}

	constexpr std::size_t size() const
	{
		return m_count;
	}

	const std::byte* bytes() const
	{
		return reinterpret_cast<const std::byte*>(m_bytes.data());
	}

private:
	std::array<std::byte, 9u> m_bytes {{}};
	std::size_t               m_count {0u};
};

#endif  // VARIABLE_INTEGER_H_
