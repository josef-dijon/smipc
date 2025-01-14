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

#include <atomic>
#include <cstdint>
#include <span>
#include <stdexcept>

constexpr bool IsDebugBuild()
{
#ifdef NDEBUG
	return false;
#else
	return true;
#endif
}

static constexpr std::size_t kAlignment {4u};

static constexpr uint32_t AlignedSize(uint32_t size)
{
	return size + ((kAlignment - (size % kAlignment)) % kAlignment);
}

class RingBuffer
{
public:
	struct RingBufferHeader
	{
		std::atomic_bool rxWaiting {false};
		std::atomic_bool txWaiting {false};
		std::atomic_bool turn {false};
		uint8_t padding_ {};

		uint32_t front {};
		uint32_t next {};
		uint32_t freeSpace {};
		uint32_t messageCount {};
	};

	RingBuffer(uint8_t* memory, std::size_t size);
	~RingBuffer() = default;

	[[nodiscard]]
	auto getMemoryBlockSize() const noexcept -> uint32_t;

private:
	std::span<uint8_t> m_memory {};

protected:
	RingBufferHeader* header {};
	std::span<uint8_t> data {};
};

#endif  // RING_BUFFER_H_
