#ifndef VARIABLE_INTEGER_H_
#define VARIABLE_INTEGER_H_

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

static constexpr bool IsTopBitSet(std::byte b)
{
	return (b & std::byte(0x80)) != std::byte(0u);
}

class VariableInteger
{
public:
	constexpr VariableInteger(uint64_t value)
		: m_value {value}
		, m_count {9u + 1u}
	{
		for_each(std::begin(m_bytes), std::end(m_bytes), [this, i = 0](std::byte& b) mutable
			{ b = static_cast<std::byte>(m_value >> i * 7u) & std::byte(0x7F); });

		for_each(std::crbegin(m_bytes), std::crend(m_bytes), [this](std::byte b)
			{ m_count -= std::size_t(! IsTopBitSet(b)); });
	}

	explicit constexpr VariableInteger(const std::byte* bytes)
	{
		for (std::size_t i = 0u; i < m_bytes.size(); ++i)
		{
			m_value |= static_cast<uint64_t>(bytes[i] & std::byte(0x7F)) << i * 7u;
			m_bytes[i] = bytes[i];

			if (! IsTopBitSet(bytes[i]))
			{
				break;
			}

			++m_count;
		}
	}

	constexpr std::size_t size() const
	{
		return m_count;
	}

	const std::byte* data() const
	{
		return reinterpret_cast<const std::byte*>(m_bytes.data());
	}

	std::byte* data()
	{
		return reinterpret_cast<std::byte*>(m_bytes.data());
	}

	uint64_t value() const
	{
		return m_value;
	}

private:
	uint64_t                  m_value {0u};
	std::array<std::byte, 9u> m_bytes {{{}, {}, {}, {}, {}, {}, {}, {}, {}}};
	std::size_t               m_count {1u};
};

template <class T>
concept VarInt = std::is_same_v<T, VariableInteger>;

#endif  // VARIABLE_INTEGER_H_
