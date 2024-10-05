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

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <array>
#include <atomic>
#include <cstdint>

static constexpr bool IsDebugBuild()
{
#ifndef DEBUG
	return true;
#else
	return false;
#endif
}

static constexpr std::size_t kAlignment {4u};

static constexpr uint32_t AlignedSize(uint32_t size)
{
	return size + ((kAlignment - (size % kAlignment)) % kAlignment);
}

template <uint32_t TSize>
struct RingBuffer
{
	struct RingBufferHeader
	{
		std::atomic_bool rxWaiting {false};
		std::atomic_bool txWaiting {false};
		std::atomic_bool turn {false};
		uint8_t padding_ {0u};

		uint32_t front {0u};
		uint32_t next {0u};
		uint32_t freeSpace {static_cast<uint32_t>(kBufferSize)};
		uint32_t messageCount {0u};
	};

	static constexpr uint32_t kBufferSize {TSize - sizeof(RingBufferHeader)};

	static_assert(TSize > kBufferSize, "RingBuffer TSize template parameter is too small");
	static_assert(TSize <= 2048u * 1024u * 1024u, "RingBuffer TSize template parameter is too large, must be less than 2GB");
	static_assert((TSize & (TSize - 1u)) == 0u, "RingBuffer TSIze template parameter is not power of 2");
	static_assert(TSize % kAlignment == 0u, "RingBuffer TSize template parameter is not 4 byte aligned");

	using RingBufferData = std::array<uint8_t, kBufferSize>;

	RingBufferHeader header {};
	RingBufferData data {};
};

#endif  // RING_BUFFER_H_
