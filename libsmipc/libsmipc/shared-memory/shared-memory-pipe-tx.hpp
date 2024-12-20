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

#ifndef SHARED_MEMORY_PIPE_TX_H_
#define SHARED_MEMORY_PIPE_TX_H_

#include <libsmipc/shared-memory/abstract-shared-memory.hpp>
#include <libsmipc/shared-memory/windows-shared-memory.hpp>
#include <libsmipc/ring-buffer/ring-buffer-tx.hpp>
#include <libsmipc/ring-buffer/packet.hpp>

#include <memory>
#include <string>
#include <cstdint>

template <std::size_t NSize>
class SharedMemoryPipeTx
{
public:
	SharedMemoryPipeTx(std::unique_ptr<ISharedMemory>&& sharedMemory)
		: m_sharedMemory {std::move(sharedMemory)}
		, m_ringBuffer {reinterpret_cast<uint8_t*>(m_sharedMemory->getView().data)}
	{}

	~SharedMemoryPipeTx()
	{
		m_sharedMemory->close();
	}

	void write(const Packet& packet) {
		m_ringBuffer.push(packet);
	}

	auto getSharedMemory() const -> const ISharedMemory*
	{
		return m_sharedMemory.get();
	}

	auto getRingBuffer() const -> const RingBufferTx<NSize>*
	{
		return &m_ringBuffer;
	}

private:
	std::unique_ptr<ISharedMemory> m_sharedMemory;
	RingBufferTx<NSize> m_ringBuffer;
};

#endif  // SHARED_MEMORY_PIPE_TX_H_
