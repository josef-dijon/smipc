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

#include <libsmipc/shared-memory/shared-memory-pipe.hpp>

#include <gtest/gtest.h>

#include <iostream>

TEST(shared_memory_pipe, host_creation)
{
	constexpr std::size_t kSharedMemorySize {256u};
	constexpr std::size_t bufferSize {kSharedMemorySize - kSharedMemoryViewDataOffset};
	const auto hostPipe = CreateSharedMemoryPipe("test-pipe", kSharedMemorySize);

	const auto& rxPipe = hostPipe->getRxPipe();
	const auto& txPipe = hostPipe->getTxPipe();

	EXPECT_STREQ(rxPipe.getSharedMemory()->getName().data(), "/smipc.test-pipe.rx");
	EXPECT_EQ(rxPipe.getSharedMemory()->getSize(), kSharedMemorySize);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().refCount, nullptr);
	EXPECT_EQ(*rxPipe.getSharedMemory()->getView().refCount, 1);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().signals, nullptr);
	EXPECT_EQ(rxPipe.getSharedMemory()->getView().signals->to_ulong(), 0);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().dataSize, nullptr);
	EXPECT_EQ(*rxPipe.getSharedMemory()->getView().dataSize, bufferSize);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().lock, nullptr);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().data, nullptr);

	EXPECT_STREQ(txPipe.getSharedMemory()->getName().data(), "/smipc.test-pipe.tx");
	EXPECT_EQ(txPipe.getSharedMemory()->getSize(), kSharedMemorySize);
	EXPECT_NE(txPipe.getSharedMemory()->getView().refCount, nullptr);
	EXPECT_EQ(*txPipe.getSharedMemory()->getView().refCount, 1);
	EXPECT_NE(txPipe.getSharedMemory()->getView().signals, nullptr);
	EXPECT_EQ(txPipe.getSharedMemory()->getView().signals->to_ulong(), 0);
	EXPECT_NE(txPipe.getSharedMemory()->getView().dataSize, nullptr);
	EXPECT_EQ(*txPipe.getSharedMemory()->getView().dataSize, bufferSize);
	EXPECT_NE(txPipe.getSharedMemory()->getView().lock, nullptr);
	EXPECT_NE(txPipe.getSharedMemory()->getView().data, nullptr);
}

TEST(shared_memory_pipe, client_creation)
{
	constexpr std::size_t kSharedMemorySize {256u};
	constexpr std::size_t bufferSize {kSharedMemorySize - kSharedMemoryViewDataOffset};
	const auto hostPipe = CreateSharedMemoryPipe("test-pipe", kSharedMemorySize);
	const auto clientPipe = OpenSharedMemoryPipe("test-pipe");

	const auto& rxPipe = clientPipe->getRxPipe();
	const auto& txPipe = clientPipe->getTxPipe();

	EXPECT_STREQ(rxPipe.getSharedMemory()->getName().data(), "/smipc.test-pipe.tx");
	EXPECT_EQ(rxPipe.getSharedMemory()->getSize(), kSharedMemorySize);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().refCount, nullptr);
	EXPECT_EQ(*rxPipe.getSharedMemory()->getView().refCount, 2);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().signals, nullptr);
	EXPECT_EQ(rxPipe.getSharedMemory()->getView().signals->to_ulong(), 0);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().dataSize, nullptr);
	EXPECT_EQ(*rxPipe.getSharedMemory()->getView().dataSize, bufferSize);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().lock, nullptr);
	EXPECT_NE(rxPipe.getSharedMemory()->getView().data, nullptr);

	EXPECT_STREQ(txPipe.getSharedMemory()->getName().data(), "/smipc.test-pipe.rx");
	EXPECT_EQ(txPipe.getSharedMemory()->getSize(), kSharedMemorySize);
	EXPECT_NE(txPipe.getSharedMemory()->getView().refCount, nullptr);
	EXPECT_EQ(*txPipe.getSharedMemory()->getView().refCount, 2);
	EXPECT_NE(txPipe.getSharedMemory()->getView().signals, nullptr);
	EXPECT_EQ(txPipe.getSharedMemory()->getView().signals->to_ulong(), 0);
	EXPECT_NE(txPipe.getSharedMemory()->getView().dataSize, nullptr);
	EXPECT_EQ(*txPipe.getSharedMemory()->getView().dataSize, bufferSize);
	EXPECT_NE(txPipe.getSharedMemory()->getView().lock, nullptr);
	EXPECT_NE(txPipe.getSharedMemory()->getView().data, nullptr);
}

TEST(shared_memory_pipe, basic_rx_tx)
{
	constexpr std::size_t kSharedMemorySize {256u};
	const auto hostPipe = CreateSharedMemoryPipe("test-pipe", kSharedMemorySize);
	const auto clientPipe = OpenSharedMemoryPipe("test-pipe");

	{
		const auto& rxPipe = hostPipe->getRxPipe();
		const auto& txPipe = hostPipe->getTxPipe();

		EXPECT_EQ(*rxPipe.getSharedMemory()->getView().refCount, 2);
		EXPECT_EQ(*txPipe.getSharedMemory()->getView().refCount, 2);
	}

	{
		const auto& rxPipe = clientPipe->getRxPipe();
		const auto& txPipe = clientPipe->getTxPipe();

		EXPECT_EQ(*rxPipe.getSharedMemory()->getView().refCount, 2);
		EXPECT_EQ(*txPipe.getSharedMemory()->getView().refCount, 2);
	}

	Packet packet {std::vector<uint8_t>({1u, 2u, 3u, 4u, 5u})};

	hostPipe->write(packet);
	EXPECT_EQ(clientPipe->getRxPipe().getRingBuffer().getMessageCount(), 1u);
	const auto rxPacket = clientPipe->read();
	EXPECT_EQ(rxPacket.data.size(), packet.data.size());
	EXPECT_EQ(rxPacket.data, packet.data);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
