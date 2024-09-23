#ifndef RING_BUFFER_TX_H_
#define RING_BUFFER_TX_H_

#include "dekkar_lock.h"
#include "ring_buffer.h"

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <vector>

template <std::size_t S, std::size_t A>
class RingBufferTx
{
public:
	RingBufferTx(uint8_t* memory)
		: m_ringBuffer {new(memory) RingBuffer<S, A>}
	{}

	[[nodiscard]]
	auto isFull() const noexcept -> bool
	{
		std::lock_guard lock(m_lock);
		return m_ringBuffer->freeSpace <= 0u;
	}

	void push(const std::vector<uint8_t>& data)
	{
		push(data.data(), data.size());
	}

	void push(const uint8_t* data, uint32_t size)
	{
		// If the data size is 0, there is nothing to do
		if (size == 0)
		{
			return;
		}

		// If the data pointer is nullptr, throw
		if (! data)
		{
			throw std::invalid_argument("Data is nullptr");
		}

		constexpr uint32_t ringBufferSize {RingBuffer<S, A>::BufferSize()};
		constexpr uint32_t headerSize {RingBuffer<S, A>::AlignedSize(sizeof(typename RingBuffer<S, A>::MessageHeader))};
		const uint32_t dataSize {size};
		const uint32_t alignedDataSize {RingBuffer<S, A>::AlignedSize(size)};

		// Read the buffer header data
		uint32_t front {m_ringBuffer->header.front};
		uint32_t next {m_ringBuffer->header.next};
		uint32_t freeSpace {m_ringBuffer->header.freeSpace};
		uint32_t messageCount {m_ringBuffer->header.messageCount};

		// Header is guaranteed to fit at the next pointer position in the buffer, so only the data will ever need to wrap around

		// Lock the buffer so we can read the header and update it
		std::unique_lock lock(m_lock);

		// Check if there is space for the header and the unaligned data, throw if not
		if (headerSize + dataSize > freeSpace)
		{
			throw std::overflow_error("Buffer overflow");
		}

		// At this point, we know the header and data will fit in the buffer, so we can calculate the start position
		const uint32_t headerStart {next};

		// Calculate the new next pointer and free space
		next += headerSize;
		freeSpace -= headerSize;

		// Check if the data will wrap around the buffer
		const bool wrapAround {(front <= next) && (next + dataSize > ringBufferSize)};

		// Calculate the data start position
		// Since the header is aligned, the data will always be aligned too
		const uint32_t dataStart {next};

		// Calculate the new next pointer and free space
		// The next pointer needs to be aligned for the next message
		next = RingBuffer<S, A>::AlignedSize((next + dataSize) % ringBufferSize);
		freeSpace -= alignedDataSize;

		// We can't let the next header split, so if it will wrap around, we need to adjust the next pointer
		if (ringBufferSize - next < headerSize)
		{
			// If there won't be room for the next header, we will consider the buffer full
			if (freeSpace < headerSize)
			{
				freeSpace = 0u;
				next = front;
			}
			else
			{
				freeSpace -= ringBufferSize - next;
				next = 0u;
			}
		}

		// Increment the message count
		++messageCount;

		// Copy the header to the buffer and lock the data
		auto* messageHeader = new (m_ringBuffer->data.data() + headerStart) typename RingBuffer<S, A>::MessageHeader {dataSize};
		std::lock_guard messageLock(messageHeader->lock);

		// Copy back the buffer header data and unlock the buffer
		// Unlocking the buffer is safe because the message is locked
		// This means readers can read other messages but not this one
		m_ringBuffer->header.front = front;
		m_ringBuffer->header.next = next;
		m_ringBuffer->header.freeSpace = freeSpace;
		m_ringBuffer->header.messageCount = messageCount;
		lock.unlock();

		// Copy the data to the buffer, wrapping around if necessary
		// The data lock will be unlocked when we reach the end of scope
		if (wrapAround)
		{
			const std::size_t part1Size = m_ringBuffer->data.size() - dataStart;
			std::copy(data, data + part1Size, std::begin(m_ringBuffer->data) + dataStart);
			std::copy(data + part1Size, data + dataSize, std::begin(m_ringBuffer->data));
		}
		else
		{
			std::copy(data, data + dataSize, std::begin(m_ringBuffer->data) + dataStart);
		}
	}

private:
	RingBuffer<S, A>* m_ringBuffer;
	mutable DekkarLock m_lock {m_ringBuffer->txWaiting, m_ringBuffer->rxWaiting, m_ringBuffer->turn};
};

#endif  // RING_BUFFER_TX_H_
