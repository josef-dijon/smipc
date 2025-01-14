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

#include <libsmipc/ring-buffer/packet.hpp>
#include <libsmipc/shared-memory/shared-memory-factory.hpp>
#include <libsmipc/shared-memory/rx-shared-memory-pipe.hpp>
#include <libsmipc/shared-memory/tx-shared-memory-pipe.hpp>

#include <cstdint>
#include <memory>
#include <string>

class SharedMemoryPipe
{
public:
	SharedMemoryPipe(std::unique_ptr<ISharedMemory>&& rxSharedMemory, std::unique_ptr<ISharedMemory>&& txSharedMemory)
		: m_rxSharedMemoryPipe {std::move(rxSharedMemory)}
		, m_txSharedMemoryPipe {std::move(txSharedMemory)}
	{}

	~SharedMemoryPipe() = default;

	auto getRxPipe() const -> const RxSharedMemoryPipe&
	{
		return m_rxSharedMemoryPipe;
	}

	auto getTxPipe() const -> const TxSharedMemoryPipe&
	{
		return m_txSharedMemoryPipe;
	}

	auto read() -> Packet
	{
		return m_rxSharedMemoryPipe.read();
	}

	void write(Packet&& packet)
	{
		m_txSharedMemoryPipe.write(std::move(packet));
	}

	void write(const Packet& packet)
	{
		m_txSharedMemoryPipe.write(packet);
	}

private:
	RxSharedMemoryPipe m_rxSharedMemoryPipe;
	TxSharedMemoryPipe m_txSharedMemoryPipe;
};

inline std::unique_ptr<SharedMemoryPipe> CreateSharedMemoryPipe(const std::string& name, std::size_t size)
{
	auto rxSharedMemory = MakeUniqueSharedMemory();
	auto txSharedMemory = MakeUniqueSharedMemory();
	rxSharedMemory->create("/smipc." + name + ".rx", size);
	txSharedMemory->create("/smipc." + name + ".tx", size);

	return std::make_unique<SharedMemoryPipe>(std::move(rxSharedMemory), std::move(txSharedMemory));
}

inline std::unique_ptr<SharedMemoryPipe> OpenSharedMemoryPipe(const std::string& name)
{
	auto rxSharedMemory = MakeUniqueSharedMemory();
	auto txSharedMemory = MakeUniqueSharedMemory();
	rxSharedMemory->open("/smipc." + name + ".tx");
	txSharedMemory->open("/smipc." + name + ".rx");

	return std::make_unique<SharedMemoryPipe>(std::move(rxSharedMemory), std::move(txSharedMemory));
}

#endif  // SHARED_MEMORY_PIPE_H_
