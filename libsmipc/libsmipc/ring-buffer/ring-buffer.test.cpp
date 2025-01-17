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

#include <libsmipc/ring-buffer/rx-ring-buffer.hpp>
#include <libsmipc/ring-buffer/tx-ring-buffer.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

TEST(ring_buffer, basic_rx_tx)
{
	constexpr std::size_t bufferSize {256u};

	uint8_t buffer[bufferSize] {};
	TxRingBuffer tx(buffer, bufferSize);
	RxRingBuffer rx(buffer, bufferSize);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	Packet txPacket {std::vector<uint8_t>({1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u})};
	tx.push(txPacket);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_FALSE(rx.isEmpty());

	const auto rxPacket = rx.pull();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);

	EXPECT_EQ(rxPacket.data.size(), txPacket.data.size());
	EXPECT_EQ(rxPacket.data, txPacket.data);
}

TEST(ring_buffer, basic_tx_throw_on_overflow)
{
	constexpr std::size_t bufferSize {128u};

	uint8_t buffer[bufferSize] {};
	TxRingBuffer tx(buffer, bufferSize);

	Packet packet {std::vector<uint8_t>(129u)};

	EXPECT_THROW(tx.push(packet), std::overflow_error);
}

TEST(ring_buffer, basic_rx_tx_1)
{
	constexpr std::size_t bufferSize {128u};

	uint8_t buffer[bufferSize] {};
	TxRingBuffer tx(buffer, bufferSize);
	RxRingBuffer rx(buffer, bufferSize);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push(Packet {std::vector<uint8_t>({0u})});
	tx.push(Packet {std::vector<uint8_t>({1u})});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		Packet txPacket {std::vector<uint8_t>({static_cast<uint8_t>(i * 1 + 0u)})};
		tx.push(txPacket);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxPacket = rx.pull();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		Packet expectedRxPacket {std::vector<uint8_t>({static_cast<uint8_t>((i - 2u) * 1 + 0u)})};

		EXPECT_EQ(rxPacket.data.size(), expectedRxPacket.data.size());
		EXPECT_EQ(rxPacket.data, expectedRxPacket.data);
	}

	const auto rxPacket = rx.pull();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxPacket.data.size(), 1u);
	EXPECT_EQ(rxPacket.data, std::vector<uint8_t>({254u}));

	const auto rxLastPacket = rx.pull();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastPacket.data.size(), 1u);
	EXPECT_EQ(rxLastPacket.data, std::vector<uint8_t>({255u}));
}

TEST(ring_buffer, basic_rx_tx_3)
{
	constexpr std::size_t bufferSize {128u};

	uint8_t buffer[bufferSize] {};
	TxRingBuffer tx(buffer, bufferSize);
	RxRingBuffer rx(buffer, bufferSize);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push(Packet {std::vector<uint8_t>({0u, 1u, 2u})});
	tx.push(Packet {std::vector<uint8_t>({3u, 4u, 5u})});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		Packet txPacket {std::vector<uint8_t>({static_cast<uint8_t>(i * 3 + 0u), static_cast<uint8_t>(i * 3 + 1u), static_cast<uint8_t>(i * 3 + 2u)})};
		tx.push(txPacket);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxPacket = rx.pull();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const Packet expectedRxPacket {std::vector<uint8_t>({static_cast<uint8_t>((i - 2u) * 3 + 0u), static_cast<uint8_t>((i - 2u) * 3 + 1u), static_cast<uint8_t>((i - 2u) * 3 + 2u)})};

		EXPECT_EQ(rxPacket.data.size(), expectedRxPacket.data.size());
		EXPECT_EQ(rxPacket.data, expectedRxPacket.data);
	}

	const auto rxPacket = rx.pull();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxPacket.data.size(), 3u);
	EXPECT_EQ(rxPacket.data, std::vector<uint8_t>({250u, 251u, 252u}));

	const auto rxLastPacket = rx.pull();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastPacket.data.size(), 3u);
	EXPECT_EQ(rxLastPacket.data, std::vector<uint8_t>({253u, 254u, 255u}));
}

TEST(ring_buffer, basic_rx_tx_4)
{
	constexpr std::size_t bufferSize {128u};

	uint8_t buffer[bufferSize] {};
	TxRingBuffer tx(buffer, bufferSize);
	RxRingBuffer rx(buffer, bufferSize);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push(Packet {std::vector<uint8_t>({0u, 1u, 2u, 3u})});
	tx.push(Packet {std::vector<uint8_t>({4u, 5u, 6u, 7u})});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		Packet txPacket {std::vector<uint8_t>({static_cast<uint8_t>(i * 4u + 0u), static_cast<uint8_t>(i * 4u + 1u), static_cast<uint8_t>(i * 4u + 2u), static_cast<uint8_t>(i * 4u + 3u)})};
		tx.push(txPacket);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxPacket = rx.pull();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const Packet expectedRxPacket {std::vector<uint8_t>({static_cast<uint8_t>((i - 2u) * 4u + 0u), static_cast<uint8_t>((i - 2u) * 4u + 1u), static_cast<uint8_t>((i - 2u) * 4u + 2u), static_cast<uint8_t>((i - 2u) * 4u + 3u)})};

		EXPECT_EQ(rxPacket.data.size(), expectedRxPacket.data.size());
		EXPECT_EQ(rxPacket.data, expectedRxPacket.data);
	}

	const auto rxPacket = rx.pull();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxPacket.data.size(), 4u);
	EXPECT_EQ(rxPacket.data, std::vector<uint8_t>({248u, 249u, 250u, 251u}));

	const auto rxLastPacket = rx.pull();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastPacket.data.size(), 4u);
	EXPECT_EQ(rxLastPacket.data, std::vector<uint8_t>({252u, 253u, 254u, 255u}));
}

TEST(ring_buffer, basic_rx_tx_5)
{
	constexpr std::size_t bufferSize {128u};

	uint8_t buffer[bufferSize] {};
	TxRingBuffer tx(buffer, bufferSize);
	RxRingBuffer rx(buffer, bufferSize);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push(Packet {std::vector<uint8_t>({0u, 1u, 2u, 3u, 4u})});
	tx.push(Packet {std::vector<uint8_t>({5u, 6u, 7u, 8u, 9u})});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		Packet txPacket {std::vector<uint8_t>({static_cast<uint8_t>(i * 5u + 0u), static_cast<uint8_t>(i * 5u + 1u), static_cast<uint8_t>(i * 5u + 2u), static_cast<uint8_t>(i * 5u + 3u), static_cast<uint8_t>(i * 5u + 4u)})};
		tx.push(txPacket);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxPacket = rx.pull();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const Packet expectedRxPacket {std::vector<uint8_t>({static_cast<uint8_t>((i - 2u) * 5u + 0u), static_cast<uint8_t>((i - 2u) * 5u + 1u), static_cast<uint8_t>((i - 2u) * 5u + 2u), static_cast<uint8_t>((i - 2u) * 5u + 3u), static_cast<uint8_t>((i - 2u) * 5u + 4u)})};

		EXPECT_EQ(rxPacket.data.size(), expectedRxPacket.data.size());
		EXPECT_EQ(rxPacket.data, expectedRxPacket.data);
	}

	const auto rxPacket = rx.pull();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxPacket.data.size(), 5u);
	EXPECT_EQ(rxPacket.data, std::vector<uint8_t>({246u, 247u, 248u, 249u, 250u}));

	const auto rxLastPacket = rx.pull();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastPacket.data.size(), 5u);
	EXPECT_EQ(rxLastPacket.data, std::vector<uint8_t>({251u, 252u, 253u, 254u, 255u}));
}

TEST(ring_buffer, basic_rx_tx_13)
{
	constexpr std::size_t bufferSize {256u};

	uint8_t buffer[bufferSize] {};
	TxRingBuffer tx(buffer, bufferSize);
	RxRingBuffer rx(buffer, bufferSize);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push(Packet {std::vector<uint8_t>({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u})});
	tx.push(Packet {std::vector<uint8_t>({13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u})});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		Packet txPacket {std::vector<uint8_t>({static_cast<uint8_t>(i * 13u + 0u), static_cast<uint8_t>(i * 13u + 1u), static_cast<uint8_t>(i * 13u + 2u), static_cast<uint8_t>(i * 13u + 3u), static_cast<uint8_t>(i * 13u + 4u), static_cast<uint8_t>(i * 13u + 5u), static_cast<uint8_t>(i * 13u + 6u), static_cast<uint8_t>(i * 13u + 7u), static_cast<uint8_t>(i * 13u + 8u), static_cast<uint8_t>(i * 13u + 9u), static_cast<uint8_t>(i * 13u + 10u), static_cast<uint8_t>(i * 13u + 11u), static_cast<uint8_t>(i * 13u + 12u)})};
		tx.push(txPacket);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxPacket = rx.pull();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const Packet expectedRxPacket {std::vector<uint8_t>({static_cast<uint8_t>((i - 2u) * 13u + 0u), static_cast<uint8_t>((i - 2u) * 13u + 1u), static_cast<uint8_t>((i - 2u) * 13u + 2u), static_cast<uint8_t>((i - 2u) * 13u + 3u), static_cast<uint8_t>((i - 2u) * 13u + 4u), static_cast<uint8_t>((i - 2u) * 13u + 5u), static_cast<uint8_t>((i - 2u) * 13u + 6u), static_cast<uint8_t>((i - 2u) * 13u + 7u), static_cast<uint8_t>((i - 2u) * 13u + 8u), static_cast<uint8_t>((i - 2u) * 13u + 9u), static_cast<uint8_t>((i - 2u) * 13u + 10u), static_cast<uint8_t>((i - 2u) * 13u + 11u), static_cast<uint8_t>((i - 2u) * 13u + 12u)})};

		EXPECT_EQ(rxPacket.data.size(), expectedRxPacket.data.size());
		EXPECT_EQ(rxPacket.data, expectedRxPacket.data);
	}

	const auto rxPacket = rx.pull();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxPacket.data.size(), 13u);
	EXPECT_EQ(rxPacket.data, std::vector<uint8_t>({230u, 231u, 232u, 233u, 234u, 235u, 236u, 237u, 238u, 239u, 240u, 241u, 242u}));

	const auto rxLastPacket = rx.pull();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastPacket.data.size(), 13u);
	EXPECT_EQ(rxLastPacket.data, std::vector<uint8_t>({243u, 244u, 245u, 246u, 247u, 248u, 249u, 250u, 251u, 252u, 253u, 254u, 255u}));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
