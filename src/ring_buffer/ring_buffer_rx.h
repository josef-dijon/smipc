#ifndef RING_BUFFER_RX_H_
#define RING_BUFFER_RX_H_

#include "dekkar_lock.h"
#include "ring_buffer.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <vector>

template <uint32_t S, uint32_t A>
class RingBufferRx
{
public:
	RingBufferRx(uint8_t* memory)
		: m_ringBuffer {new(memory) RingBuffer<S, A>}
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
	auto pop() noexcept -> std::vector<uint8_t>
	{
		if (isEmpty())
		{
			return {};
		}

		constexpr uint32_t headerSize {RingBuffer<S, A>::AlignedSize(sizeof(MessageHeader))};
		constexpr uint32_t ringBufferSize {RingBuffer<S, A>::BufferSize()};

		std::lock_guard lock(m_lock);
		auto* messageHeader {reinterpret_cast<MessageHeader*>(m_ringBuffer->data.data() + m_ringBuffer->header.front)};
		std::unique_lock messageLock {messageHeader->lock};

		const uint32_t dataSize {messageHeader->size};
		const uint32_t alignedDataSize {RingBuffer<S, A>::AlignedSize(dataSize)};

		uint32_t front = m_ringBuffer->header.front;
		uint32_t next = m_ringBuffer->header.next;
		uint32_t freeSpace = m_ringBuffer->header.freeSpace;
		uint32_t messageCount = m_ringBuffer->header.messageCount;

		std::vector<uint8_t> data {};
		data.reserve(dataSize);

		// Message header is guaranteed to not be split across the ring buffer
		freeSpace += headerSize;
		front = (front + headerSize) % ringBufferSize;

		const bool wrapAround {front + dataSize > ringBufferSize};

		if (wrapAround)
		{
			const uint32_t part1Size = ringBufferSize - front;
			std::copy_n(std::cbegin(m_ringBuffer->data) + front, part1Size, std::back_inserter(data));
			std::copy_n(std::cbegin(m_ringBuffer->data), dataSize - part1Size, std::back_inserter(data));

			if constexpr (IsDebugBuild())
			{
				std::fill_n(std::begin(m_ringBuffer->data) + front, part1Size, 0u);
				std::fill_n(std::begin(m_ringBuffer->data), dataSize - part1Size, 0u);
			}

			freeSpace += part1Size;
			front = (RingBuffer<S, A>::AlignedSize(dataSize - part1Size)) % ringBufferSize;
			freeSpace += RingBuffer<S, A>::AlignedSize(dataSize - part1Size);
		}
		else
		{
			std::copy_n(std::cbegin(m_ringBuffer->data) + front, dataSize, std::back_inserter(data));

			if constexpr (IsDebugBuild())
			{
				std::fill_n(std::begin(m_ringBuffer->data) + front, dataSize, 0u);
			}

			front = (front + alignedDataSize) % ringBufferSize;
			freeSpace += alignedDataSize;
		}

		if constexpr (IsDebugBuild())
		{
			messageHeader->size = 0;
		}

		messageLock.unlock();

		if (front + headerSize > ringBufferSize)
		{
			freeSpace += ringBufferSize - front;
			front = 0u;
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

		return data;
	}

private:
	RingBuffer<S, A>* m_ringBuffer;
	mutable DekkarLock m_lock {m_ringBuffer->header.rxWaiting, m_ringBuffer->header.txWaiting, m_ringBuffer->header.turn};
};

#endif  // RING_BUFFER_RX_H_
