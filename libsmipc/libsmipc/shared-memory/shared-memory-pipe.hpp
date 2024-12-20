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

#ifndef SHARED_MEMORY_PIPE_H_
#define SHARED_MEMORY_PIPE_H_

#include <libsmipc/shared-memory/shared-memory-factory.hpp>
#include <libsmipc/shared-memory/shared-memory-pipe-rx.hpp>
#include <libsmipc/shared-memory/shared-memory-pipe-tx.hpp>
#include <libsmipc/ring-buffer/packet.hpp>

#include <memory>
#include <string>
#include <cstdint>


template <std::size_t NSize>
class SharedMemoryPipe
{
public:
	SharedMemoryPipe(std::unique_ptr<ISharedMemory>&& rxSharedMemory, std::unique_ptr<ISharedMemory>&& txSharedMemory)
		: m_sharedMemoryPipeRx {std::move(rxSharedMemory)}
		, m_sharedMemoryPipeTx {std::move(txSharedMemory)}
	{}
	~SharedMemoryPipe() = default;

	auto getRxPipe() const -> const SharedMemoryPipeRx<NSize>&
	{
		return m_sharedMemoryPipeRx;
	}

	auto getTxPipe() const -> const SharedMemoryPipeTx<NSize>&
	{
		return m_sharedMemoryPipeTx;
	}

	auto read() -> Packet
	{
		return m_sharedMemoryPipeRx.read();
	}
	void write(Packet&& packet)
	{
		m_sharedMemoryPipeTx.write(std::move(packet));
	}
	void write(const Packet& packet)
	{
		m_sharedMemoryPipeTx.write(packet);
	}

private:
	SharedMemoryPipeRx<NSize> m_sharedMemoryPipeRx;
	SharedMemoryPipeTx<NSize> m_sharedMemoryPipeTx;
};

template <std::size_t NSize>
inline std::unique_ptr<SharedMemoryPipe<NSize>> CreateSharedMemoryPipe(const std::string& name)
{
	auto rxSharedMemory = MakeUniqueSharedMemory();
	auto txSharedMemory = MakeUniqueSharedMemory();
	rxSharedMemory->create("smipc://" + name + ".rx", NSize);
	txSharedMemory->create("smipc://" + name + ".tx", NSize);

	return std::make_unique<SharedMemoryPipe<NSize>>(std::move(rxSharedMemory), std::move(txSharedMemory));
}

template <std::size_t NSize>
inline std::unique_ptr<SharedMemoryPipe<NSize>> OpenSharedMemoryPipe(const std::string& name)
{
	auto rxSharedMemory = MakeUniqueSharedMemory();
	auto txSharedMemory = MakeUniqueSharedMemory();
	rxSharedMemory->open("smipc://" + name + ".tx");
	txSharedMemory->open("smipc://" + name + ".rx");

	return std::make_unique<SharedMemoryPipe<NSize>>(std::move(rxSharedMemory), std::move(txSharedMemory));
}

#endif  // SHARED_MEMORY_PIPE_H_
