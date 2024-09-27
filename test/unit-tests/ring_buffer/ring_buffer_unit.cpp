#include "gtest/gtest.h"
#include "ring_buffer/ring_buffer_rx.h"
#include "ring_buffer/ring_buffer_tx.h"

#include <cstddef>
#include <cstdint>
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

TEST(ring_buffer, basic_rx_tx_wrap_1)
{
	constexpr std::size_t bufferSize {128u};
	constexpr std::size_t align {4u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);
	RingBufferRx<bufferSize, align> rx(buffer);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push({0u});
	tx.push({1u});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		const std::vector<uint8_t> txMessage = {
			static_cast<uint8_t>(i * 1 + 0u)};
		tx.push(txMessage);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxMessage = rx.pop();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const std::vector<uint8_t> expectedRxMessage = {
			static_cast<uint8_t>((i - 2u) * 1 + 0u)};

		EXPECT_EQ(rxMessage.size(), expectedRxMessage.size());
		EXPECT_EQ(rxMessage, expectedRxMessage);
	}

	const auto rxMessage = rx.pop();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxMessage.size(), 1u);
	EXPECT_EQ(rxMessage, std::vector<uint8_t>({254u}));

	const auto rxLastMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastMessage.size(), 1u);
	EXPECT_EQ(rxLastMessage, std::vector<uint8_t>({255u}));
}

TEST(ring_buffer, basic_rx_tx_wrap_3)
{
	constexpr std::size_t bufferSize {128u};
	constexpr std::size_t align {4u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);
	RingBufferRx<bufferSize, align> rx(buffer);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push({0u, 1u, 2u});
	tx.push({3u, 4u, 5u});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		const std::vector<uint8_t> txMessage = {
			static_cast<uint8_t>(i * 3 + 0u),
			static_cast<uint8_t>(i * 3 + 1u),
			static_cast<uint8_t>(i * 3 + 2u)};
		tx.push(txMessage);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxMessage = rx.pop();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const std::vector<uint8_t> expectedRxMessage = {
			static_cast<uint8_t>((i - 2u) * 3 + 0u),
			static_cast<uint8_t>((i - 2u) * 3 + 1u),
			static_cast<uint8_t>((i - 2u) * 3 + 2u)};

		EXPECT_EQ(rxMessage.size(), expectedRxMessage.size());
		EXPECT_EQ(rxMessage, expectedRxMessage);
	}

	const auto rxMessage = rx.pop();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxMessage.size(), 3u);
	EXPECT_EQ(rxMessage, std::vector<uint8_t>({250u, 251u, 252u}));

	const auto rxLastMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastMessage.size(), 3u);
	EXPECT_EQ(rxLastMessage, std::vector<uint8_t>({253u, 254u, 255u}));
}

TEST(ring_buffer, basic_rx_tx_wrap_4)
{
	constexpr std::size_t bufferSize {128u};
	constexpr std::size_t align {4u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);
	RingBufferRx<bufferSize, align> rx(buffer);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push({0u, 1u, 2u, 3u});
	tx.push({4u, 5u, 6u, 7u});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		const std::vector<uint8_t> txMessage = {
			static_cast<uint8_t>(i * 4 + 0u),
			static_cast<uint8_t>(i * 4 + 1u),
			static_cast<uint8_t>(i * 4 + 2u),
			static_cast<uint8_t>(i * 4 + 3u)};
		tx.push(txMessage);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxMessage = rx.pop();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const std::vector<uint8_t> expectedRxMessage = {
			static_cast<uint8_t>((i - 2u) * 4 + 0u),
			static_cast<uint8_t>((i - 2u) * 4 + 1u),
			static_cast<uint8_t>((i - 2u) * 4 + 2u),
			static_cast<uint8_t>((i - 2u) * 4 + 3u)};

		EXPECT_EQ(rxMessage.size(), expectedRxMessage.size());
		EXPECT_EQ(rxMessage, expectedRxMessage);
	}

	const auto rxMessage = rx.pop();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxMessage.size(), 4u);
	EXPECT_EQ(rxMessage, std::vector<uint8_t>({248u, 249u, 250u, 251u}));

	const auto rxLastMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastMessage.size(), 4u);
	EXPECT_EQ(rxLastMessage, std::vector<uint8_t>({252u, 253u, 254u, 255u}));
}

TEST(ring_buffer, basic_rx_tx_wrap_5)
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

	for (std::size_t i = 2u; i < 4096u; ++i)
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
	EXPECT_EQ(rxMessage, std::vector<uint8_t>({246u, 247u, 248u, 249u, 250u}));

	const auto rxLastMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastMessage.size(), 5u);
	EXPECT_EQ(rxLastMessage, std::vector<uint8_t>({251u, 252u, 253u, 254u, 255u}));
}

TEST(ring_buffer, basic_rx_tx_wrap_13)
{
	constexpr std::size_t bufferSize {128u};
	constexpr std::size_t align {4u};

	uint8_t buffer[bufferSize];
	RingBufferTx<bufferSize, align> tx(buffer);
	RingBufferRx<bufferSize, align> rx(buffer);

	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_TRUE(rx.isEmpty());

	tx.push({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u});
	tx.push({13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u});
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 2u);
	EXPECT_FALSE(rx.isEmpty());

	for (std::size_t i = 2u; i < 4096u; ++i)
	{
		const std::vector<uint8_t> txMessage = {
			static_cast<uint8_t>(i * 13 + 0u),
			static_cast<uint8_t>(i * 13 + 1u),
			static_cast<uint8_t>(i * 13 + 2u),
			static_cast<uint8_t>(i * 13 + 3u),
			static_cast<uint8_t>(i * 13 + 4u),
			static_cast<uint8_t>(i * 13 + 5u),
			static_cast<uint8_t>(i * 13 + 6u),
			static_cast<uint8_t>(i * 13 + 7u),
			static_cast<uint8_t>(i * 13 + 8u),
			static_cast<uint8_t>(i * 13 + 9u),
			static_cast<uint8_t>(i * 13 + 10u),
			static_cast<uint8_t>(i * 13 + 11u),
			static_cast<uint8_t>(i * 13 + 12u)};
		tx.push(txMessage);

		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 3u);
		EXPECT_FALSE(rx.isEmpty());

		const auto rxMessage = rx.pop();
		EXPECT_FALSE(rx.isEmpty());
		EXPECT_FALSE(tx.isFull());
		EXPECT_EQ(rx.getMessageCount(), 2u);

		const std::vector<uint8_t> expectedRxMessage = {
			static_cast<uint8_t>((i - 2u) * 13 + 0u),
			static_cast<uint8_t>((i - 2u) * 13 + 1u),
			static_cast<uint8_t>((i - 2u) * 13 + 2u),
			static_cast<uint8_t>((i - 2u) * 13 + 3u),
			static_cast<uint8_t>((i - 2u) * 13 + 4u),
			static_cast<uint8_t>((i - 2u) * 13 + 5u),
			static_cast<uint8_t>((i - 2u) * 13 + 6u),
			static_cast<uint8_t>((i - 2u) * 13 + 7u),
			static_cast<uint8_t>((i - 2u) * 13 + 8u),
			static_cast<uint8_t>((i - 2u) * 13 + 9u),
			static_cast<uint8_t>((i - 2u) * 13 + 10u),
			static_cast<uint8_t>((i - 2u) * 13 + 11u),
			static_cast<uint8_t>((i - 2u) * 13 + 12u)};

		EXPECT_EQ(rxMessage.size(), expectedRxMessage.size());
		EXPECT_EQ(rxMessage, expectedRxMessage);
	}

	const auto rxMessage = rx.pop();
	EXPECT_FALSE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 1u);
	EXPECT_EQ(rxMessage.size(), 13u);
	EXPECT_EQ(rxMessage, std::vector<uint8_t>({230u, 231u, 232u, 233u, 234u, 235u, 236u, 237u, 238u, 239u, 240u, 241u, 242u}));

	const auto rxLastMessage = rx.pop();
	EXPECT_TRUE(rx.isEmpty());
	EXPECT_FALSE(tx.isFull());
	EXPECT_EQ(rx.getMessageCount(), 0u);
	EXPECT_EQ(rxLastMessage.size(), 13u);
	EXPECT_EQ(rxLastMessage, std::vector<uint8_t>({243u, 244u, 245u, 246u, 247u, 248u, 249u, 250u, 251u, 252u, 253u, 254u, 255u}));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
