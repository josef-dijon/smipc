#ifndef RING_BUFFER_RX_H_
#define RING_BUFFER_RX_H_

#include "dekkar_lock.h"
#include "packet.h"
#include "ring_buffer.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <vector>

template <uint32_t TSize>
class RingBufferRx
{
public:
	RingBufferRx(uint8_t* memory)
		: m_ringBuffer {new(memory) RingBuffer<TSize>}
	{}

	[[nodiscard]]
	auto isEmpty() const noexcept -> bool
	{
		std::lock_guard lock(m_lock);
		return m_ringBuffer->header.freeSpace == m_ringBuffer->data.size();
	}

	[[nodiscard]]
	auto getMessageCount() const noexcept -> uint32_t
	{
		std::lock_guard lock(m_lock);
		return m_ringBuffer->header.messageCount;
	}

	[[nodiscard]]
	auto pop() -> Packet
	{
		if (isEmpty())
		{
			throw std::runtime_error("No packets in buffer");
		}

		constexpr uint32_t headerSize {AlignedSize(sizeof(PacketHeader))};
		constexpr uint32_t ringBufferSize {RingBuffer<TSize>::kBufferSize};

		std::lock_guard lock(m_lock);

		uint32_t front {m_ringBuffer->header.front};
		uint32_t next {m_ringBuffer->header.next};
		uint32_t freeSpace {m_ringBuffer->header.freeSpace};
		uint32_t messageCount {m_ringBuffer->header.messageCount};

		const uint32_t headerStart {front};
		const bool headerWrap {front + headerSize > ringBufferSize};
		front = (front + headerSize) % ringBufferSize;
		freeSpace += headerSize;

		Packet packet;

		if (headerWrap)
		{
			const uint32_t part1Size = ringBufferSize - headerStart;
			std::copy_n(std::cbegin(m_ringBuffer->data) + headerStart, part1Size, reinterpret_cast<uint8_t*>(&packet.header));
			std::copy_n(std::cbegin(m_ringBuffer->data), headerSize - part1Size, reinterpret_cast<uint8_t*>(&packet.header) + part1Size);

			if constexpr (IsDebugBuild())
			{
				std::fill_n(std::begin(m_ringBuffer->data) + headerStart, part1Size, 0u);
				std::fill_n(std::begin(m_ringBuffer->data), headerSize - part1Size, 0u);
			}
		}
		else
		{
			std::copy_n(std::cbegin(m_ringBuffer->data) + headerStart, headerSize, reinterpret_cast<uint8_t*>(&packet.header));

			if constexpr (IsDebugBuild())
			{
				std::fill_n(std::begin(m_ringBuffer->data) + headerStart, headerSize, 0u);
			}
		}

		const uint32_t dataSize {packet.header.size};
		const uint32_t alignedDataSize {AlignedSize(dataSize)};
		const uint32_t dataStart {front};
		const bool dataWrap {front + dataSize > ringBufferSize};
		front = (front + alignedDataSize) % ringBufferSize;
		freeSpace += alignedDataSize;

		packet.data.reserve(dataSize);

		if (dataWrap)
		{
			const uint32_t part1Size = ringBufferSize - dataStart;
			std::copy_n(std::cbegin(m_ringBuffer->data) + dataStart, part1Size, std::back_inserter(packet.data));
			std::copy_n(std::cbegin(m_ringBuffer->data), dataSize - part1Size, std::back_inserter(packet.data));

			if constexpr (IsDebugBuild())
			{
				std::fill_n(std::begin(m_ringBuffer->data) + dataStart, part1Size, 0u);
				std::fill_n(std::begin(m_ringBuffer->data), dataSize - part1Size, 0u);
			}
		}
		else
		{
			std::copy_n(std::cbegin(m_ringBuffer->data) + dataStart, dataSize, std::back_inserter(packet.data));

			if constexpr (IsDebugBuild())
			{
				std::fill_n(std::begin(m_ringBuffer->data) + dataStart, dataSize, 0u);
			}
		}

		if constexpr (IsDebugBuild())
		{
			packet.header.size = 0;
		}

		if (freeSpace == ringBufferSize)
		{
			front = 0u;
			next = 0u;
		}

		m_ringBuffer->header.front = front;
		m_ringBuffer->header.freeSpace = freeSpace;
		m_ringBuffer->header.next = next;
		m_ringBuffer->header.messageCount = --messageCount;

		return packet;
	}

private:
	RingBuffer<TSize>* m_ringBuffer;
	mutable DekkarLock m_lock {m_ringBuffer->header.rxWaiting, m_ringBuffer->header.txWaiting, m_ringBuffer->header.turn};
};

#endif  // RING_BUFFER_RX_H_
