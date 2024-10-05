#ifndef RING_BUFFER_TX_H_
#define RING_BUFFER_TX_H_

#include <libsmipc/ring-buffer/dekkar-lock.hpp>
#include <libsmipc/ring-buffer/packet.hpp>
#include <libsmipc/ring-buffer/ring-buffer.hpp>

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <vector>

template <std::size_t TSize>
class RingBufferTx
{
public:
	RingBufferTx(uint8_t* memory)
		: m_ringBuffer {new(memory) RingBuffer<TSize>}
	{}

	[[nodiscard]]
	auto isFull() const noexcept -> bool
	{
		std::lock_guard lock(m_lock);
		return m_ringBuffer->header.freeSpace <= 0u;
	}

	void push(const Packet& packet)
	{
		// If the data size is 0, there is nothing to do
		if (packet.data.size() == 0)
		{
			return;
		}

		constexpr uint32_t ringBufferSize {RingBuffer<TSize>::kBufferSize};
		const uint32_t headerSize {static_cast<uint32_t>(sizeof(PacketHeader))};
		const uint32_t dataSize {static_cast<uint32_t>(packet.data.size())};
		const uint32_t alignedDataSize {AlignedSize(dataSize)};
		const uint32_t packetSize {headerSize + alignedDataSize};

		std::lock_guard lock(m_lock);

		// Read the buffer header data
		uint32_t front {m_ringBuffer->header.front};
		uint32_t next {m_ringBuffer->header.next};
		uint32_t freeSpace {m_ringBuffer->header.freeSpace};
		uint32_t messageCount {m_ringBuffer->header.messageCount};

		// Check if there is space for the header and the unaligned data, throw if not
		if (packetSize > freeSpace)
		{
			throw std::overflow_error("Buffer overflow");
		}

		const uint32_t headerStart {next};
		const bool headerWrap {(front <= next) && (headerStart + headerSize > ringBufferSize)};
		next = (next + headerSize) % ringBufferSize;
		freeSpace -= headerSize;

		const uint32_t dataStart {next};
		const bool dataWrap {(front <= next) && (dataStart + dataSize > ringBufferSize)};
		next = (next + alignedDataSize) % ringBufferSize;
		freeSpace -= alignedDataSize;

		++messageCount;

		// Copy back the buffer header data and unlock the buffer
		// Unlocking the buffer is safe because the message is locked
		// This means readers can read other messages but not this one
		m_ringBuffer->header.front = front;
		m_ringBuffer->header.next = next;
		m_ringBuffer->header.freeSpace = freeSpace;
		m_ringBuffer->header.messageCount = messageCount;

		if (headerWrap)
		{
			const std::size_t part1Size = ringBufferSize - headerStart;
			std::copy_n(reinterpret_cast<const uint8_t*>(&packet.header), part1Size, std::begin(m_ringBuffer->data) + headerStart);
			std::copy_n(reinterpret_cast<const uint8_t*>(&packet.header) + part1Size, headerSize - part1Size, std::begin(m_ringBuffer->data));
		}
		else
		{
			std::copy_n(reinterpret_cast<const uint8_t*>(&packet.header), headerSize, std::begin(m_ringBuffer->data) + headerStart);
		}

		if (dataWrap)
		{
			const std::size_t part1Size = ringBufferSize - dataStart;
			std::copy_n(std::begin(packet.data), part1Size, std::begin(m_ringBuffer->data) + dataStart);
			std::copy_n(std::begin(packet.data) + part1Size, dataSize - part1Size, std::begin(m_ringBuffer->data));
		}
		else
		{
			std::copy(std::begin(packet.data), std::end(packet.data), std::begin(m_ringBuffer->data) + dataStart);
		}
	}

private:
	RingBuffer<TSize>* m_ringBuffer;
	mutable DekkarLock m_lock {m_ringBuffer->header.txWaiting, m_ringBuffer->header.rxWaiting, m_ringBuffer->header.turn};
};

#endif  // RING_BUFFER_TX_H_
