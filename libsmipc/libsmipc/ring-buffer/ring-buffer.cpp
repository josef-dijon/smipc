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

#include <libsmipc/ring-buffer/ring-buffer.hpp>

#include <stdexcept>

RingBuffer::RingBuffer(uint8_t* memory, std::size_t size)
	: m_memory {memory, size}
	, header {reinterpret_cast<RingBufferHeader*>(m_memory.data())}
	, data {m_memory.begin() + sizeof(RingBufferHeader), size - sizeof(RingBufferHeader)}
{
	if (reinterpret_cast<uintptr_t>(memory) % kAlignment != 0u)
	{
		throw std::invalid_argument("Memory must be 4 byte aligned");
	}

	if (size % kAlignment != 0u)
	{
		throw std::invalid_argument("Buffer size must be a multiple of 4");
	}

	if (size <= sizeof(RingBufferHeader))
	{
		throw std::invalid_argument("Buffer size is too small");
	}

	if (size > 2048u * 1024u * 1024u)
	{
		throw std::invalid_argument("Buffer size is too large, must be less than 2GB");
	}

	header->freeSpace = static_cast<uint32_t>(data.size());
}

[[nodiscard]]
auto RingBuffer::getMemoryBlockSize() const noexcept -> uint32_t
{
	return static_cast<uint32_t>(m_memory.size());
}
