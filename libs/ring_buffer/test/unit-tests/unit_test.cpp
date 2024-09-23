#include "ring_buffer/ring_buffer_rx.h"
#include "ring_buffer/ring_buffer_tx.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

TEST(ring_buffer, basic_rx_tx_align_4)
{
	constexpr std::size_t bufferSize {256u};
	constexpr std::size_t align {4u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);
	RingBufferRx<bufferSize, align> rx(buffer);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	const std::vector<uint8_t> txMessage = {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};
	tx.push(txMessage);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_FALSE(rx.isEmpty());

	const auto rxMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);

	EXPECT_EQ(rxMessage.size(), txMessage.size());
	EXPECT_EQ(rxMessage, txMessage);
}

TEST(ring_buffer, basic_rx_tx_align_8)
{
	constexpr std::size_t bufferSize {256u};
	constexpr std::size_t align {8u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);
	RingBufferRx<bufferSize, align> rx(buffer);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	const std::vector<uint8_t> txMessage = {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};
	tx.push(txMessage);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_FALSE(rx.isEmpty());

	const auto rxMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);

	EXPECT_EQ(rxMessage.size(), txMessage.size());
	EXPECT_EQ(rxMessage, txMessage);
}

TEST(ring_buffer, basic_rx_tx_align_16)
{
	constexpr std::size_t bufferSize {256u};
	constexpr std::size_t align {16u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);
	RingBufferRx<bufferSize, align> rx(buffer);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	const std::vector<uint8_t> txMessage = {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};
	tx.push(txMessage);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_FALSE(rx.isEmpty());

	const auto rxMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);

	EXPECT_EQ(rxMessage.size(), txMessage.size());
	EXPECT_EQ(rxMessage, txMessage);
}

TEST(ring_buffer, basic_tx_throw_on_nullptr)
{
	constexpr std::size_t bufferSize {128u};
	constexpr std::size_t align {4u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);

	EXPECT_THROW(tx.push(nullptr, 1u), std::invalid_argument);
}

TEST(ring_buffer, basic_tx_throw_on_overflow)
{
	constexpr std::size_t bufferSize {128u};
	constexpr std::size_t align {4u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);

	EXPECT_THROW(tx.push(std::vector<uint8_t>(129u)), std::overflow_error);
}

TEST(ring_buffer, basic_rx_tx_wrap)
{
	constexpr std::size_t bufferSize {128u};
	constexpr std::size_t align {4u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);
	RingBufferRx<bufferSize, align> rx(buffer);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push({0u, 1u, 2u, 3u, 4u});
	tx.push({5u, 6u, 7u, 8u, 9u});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 1024u; ++i)
	{
		const std::vector<uint8_t> txMessage = {
			static_cast<uint8_t>(i * 5 + 0u),
			static_cast<uint8_t>(i * 5 + 1u),
			static_cast<uint8_t>(i * 5 + 2u),
			static_cast<uint8_t>(i * 5 + 3u),
			static_cast<uint8_t>(i * 5 + 4u)};
		tx.push(txMessage);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxMessage = rx.pop();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const std::vector<uint8_t> expectedRxMessage = {
			static_cast<uint8_t>((i - 2u) * 5 + 0u),
			static_cast<uint8_t>((i - 2u) * 5 + 1u),
			static_cast<uint8_t>((i - 2u) * 5 + 2u),
			static_cast<uint8_t>((i - 2u) * 5 + 3u),
			static_cast<uint8_t>((i - 2u) * 5 + 4u)};

		EXPECT_EQ(rxMessage.size(), expectedRxMessage.size());
		EXPECT_EQ(rxMessage, expectedRxMessage);
	}

	const auto rxMessage = rx.pop();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxMessage.size(), 5u);
	EXPECT_EQ(rxMessage, std::vector<uint8_t>({0u, 0u, 0u, 0u, 0u}));

	const auto rxLastMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastMessage.size(), 5u);
	EXPECT_EQ(rxLastMessage, std::vector<uint8_t>({0u, 0u, 0u, 0u, 0u}));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
