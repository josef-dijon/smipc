#ifndef VARIABLE_INTEGER_H_
#define VARIABLE_INTEGER_H_

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

static constexpr bool IsContinuationBitSet(std::byte b)
{
	return (b & std::byte(0x80)) != std::byte(0u);
}

class VariableInteger
{
public:
	using Bytes = std::array<std::byte, 9u>;

	constexpr VariableInteger(uint64_t value)
		: m_value {value}
	{
		if constexpr (std::endian::native == std::endian::big)
		{
			value = std::byteswap(value);
		}

		// Loop over the first 8 bytes
		std::for_each(std::begin(m_bytes), std::end(m_bytes) - 1u, [this, value](std::byte& b) mutable
			{
				// Take the bottom 7 bits of value then shift value down by 7 bits
				b = static_cast<std::byte>(value & 0x7F);
				value >>= 7u;

				// Set the continuation bit if there is data left in value
				b |= (std::byte(value != 0u) << 7u);
				m_count += static_cast<std::size_t>(value != 0u); });

		// The last byte is just the top byte of value since there is no continuation bit
		m_bytes.back() = static_cast<std::byte>(value >> 8u * 7u);
	}

	explicit constexpr VariableInteger(const std::byte* bytes)
	{
		// Loop over the first 8 bytes
		for (std::size_t i {0u}; i < 8u; ++i)
		{
			// Add the next 7 bits to value
			m_value |= static_cast<uint64_t>(bytes[i] & std::byte(0x7F)) << i * 7u;
			// Copy byte to the byte array
			m_bytes[i] = bytes[i];

			if (IsContinuationBitSet(bytes[i]))
			{
				// Increment count if the continuation bit is set
				++m_count;
			}
			else
			{
				break;
			}
		}

		constexpr std::size_t kLastByteIndex {8u};

		if (m_count == kLastByteIndex + 1u)
		{
			m_value |= static_cast<uint64_t>(bytes[kLastByteIndex]) << kLastByteIndex * 7u;
			m_bytes[kLastByteIndex] = bytes[kLastByteIndex];
		}

		if constexpr (std::endian::native == std::endian::big)
		{
			m_value = std::byteswap(m_value);
		}
	}

	constexpr std::size_t size() const
	{
		return m_count;
	}

	Bytes bytes() const
	{
		return m_bytes;
	}

	uint64_t value() const
	{
		return m_value;
	}

private:
	uint64_t    m_value {0u};
	Bytes       m_bytes {{{}, {}, {}, {}, {}, {}, {}, {}, {}}};
	std::size_t m_count {1u};
};

template <class T>
concept VarInt = std::is_same_v<T, VariableInteger>;

#endif  // VARIABLE_INTEGER_H_
