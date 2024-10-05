/* MIT License
 * 
 * Copyright (c) 2024 Josef de Joanelli (josef@pixelrift.io)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef VARIABLE_INTEGER_H_
#define VARIABLE_INTEGER_H_

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>

static constexpr bool IsContinuationBitSet(std::byte b)
{
	return (b & std::byte(0x80)) != std::byte(0u);
}

static constexpr uint64_t ByteSwap(uint64_t value)
{
	const auto bytes = std::bit_cast<std::array<std::byte, 8u>>(value);
	std::array<std::byte, 8u> swappedBytes {};

	swappedBytes[0u] = bytes[7u];
	swappedBytes[1u] = bytes[6u];
	swappedBytes[2u] = bytes[5u];
	swappedBytes[3u] = bytes[4u];
	swappedBytes[4u] = bytes[3u];
	swappedBytes[5u] = bytes[2u];
	swappedBytes[6u] = bytes[1u];
	swappedBytes[7u] = bytes[0u];

	return std::bit_cast<uint64_t>(swappedBytes);
}

class VariableInteger
{
public:
	constexpr VariableInteger(uint64_t value)
		: m_value {value}
	{
		if constexpr (std::endian::native == std::endian::big)
		{
			value = ByteSwap(value);
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
			m_value = ByteSwap(m_value);
		}
	}

	constexpr std::size_t size() const
	{
		return m_count;
	}

	std::span<const std::byte> bytes() const
	{
		return {m_bytes.data(), m_count};
	}

	uint64_t value() const
	{
		return m_value;
	}

private:
	uint64_t m_value {0u};
	std::array<std::byte, 9u> m_bytes {
		{{}, {}, {}, {}, {}, {}, {}, {}, {}}
    };
	std::size_t m_count {1u};
};

template <class T>
concept VarInt = std::is_same_v<T, VariableInteger>;

#endif  // VARIABLE_INTEGER_H_
