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
	auto is_full() const noexcept -> bool
	{
		std::lock_guard lock(m_lock);
		return m_ringBuffer->freeSpace <= 0u;
	}

	void push(const std::vector<uint8_t>& data)
	{
		push(data.data(), data.size());
	}

	void push(const uint8_t* data, std::size_t size)
	{
		if (size == 0)
		{
			return;
		}

		if (! data)
		{
			throw std::invalid_argument("Data is nullptr");
		}

		constexpr std::size_t headerSize {RingBuffer<S, A>::AlignedSize(sizeof(typename RingBuffer<S, A>::MessageHeader))};
		const std::size_t alignedSize {RingBuffer<S, A>::AlignedSize(size)};

		std::unique_lock lock(m_lock);

		const std::size_t tmpFirst = m_ringBuffer->front;
		std::size_t tmpNext = m_ringBuffer->next;
		std::size_t tmpFreeSpace = m_ringBuffer->freeSpace;

		if (headerSize + size > m_ringBuffer->freeSpace)
		{
			throw std::overflow_error("Buffer overflow");
		}

		if (tmpFirst <= tmpNext)
		{
			if (tmpNext + headerSize > m_ringBuffer->buffer.size())
			{
				tmpFreeSpace -= tmpNext + headerSize - m_ringBuffer->buffer.size();
				tmpNext = 0u;
			}
			else if (tmpNext + headerSize == m_ringBuffer->buffer.size())
			{
				tmpNext = 0u;
			}
		}

		if (headerSize > tmpFreeSpace)
		{
			throw std::overflow_error("Buffer overflow");
		}

		const std::size_t headerStart {tmpNext};

		tmpNext += headerSize;
		tmpFreeSpace -= headerSize;

		if (alignedSize > tmpFreeSpace)
		{
			throw std::overflow_error("Buffer overflow");
		}

		auto* messageHeader {new (m_ringBuffer->buffer.data() + headerStart) typename RingBuffer<S, A>::MessageHeader {size}};
		std::lock_guard messageLock(messageHeader->lock);

		if (tmpNext + alignedSize < m_ringBuffer->buffer.size())
		{
			m_ringBuffer->next = static_cast<uint32_t>(tmpNext + alignedSize);
		}
		else if (tmpNext + size > m_ringBuffer->buffer.size())
		{
			m_ringBuffer->next = static_cast<uint32_t>(tmpNext + alignedSize - m_ringBuffer->buffer.size());
		}
		else
		{
			m_ringBuffer->next = 0u;
		}

		m_ringBuffer->freeSpace = static_cast<uint32_t>(tmpFreeSpace - alignedSize);

		lock.unlock();

		if (tmpNext + size <= m_ringBuffer->buffer.size())
		{
			std::copy(data, data + size, m_ringBuffer->buffer.begin() + tmpNext);
		}
		else
		{
			const std::size_t part1Size = m_ringBuffer->buffer.size() - tmpNext;
			std::copy(data, data + part1Size, m_ringBuffer->buffer.begin() + tmpNext);
			std::copy(data + part1Size, data + size, m_ringBuffer->buffer.begin());
		}

		++m_ringBuffer->messageCount;
	}

private:
	RingBuffer<S, A>* m_ringBuffer;
	mutable DekkarLock m_lock {m_ringBuffer->txWaiting, m_ringBuffer->rxWaiting, m_ringBuffer->turn};
};

#endif  // RING_BUFFER_TX_H_
