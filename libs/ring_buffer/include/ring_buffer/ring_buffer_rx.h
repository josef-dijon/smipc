#ifndef RING_BUFFER_RX_H_
#define RING_BUFFER_RX_H_

#include "dekkar_lock.h"
#include "ring_buffer.h"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <vector>

template <std::size_t S, std::size_t A>
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
		return m_ringBuffer->freeSpace == m_ringBuffer->buffer.size();
	}

	[[nodiscard]]
	auto getMessageCount() const noexcept -> std::size_t
	{
		std::lock_guard lock(m_lock);
		return m_ringBuffer->messageCount;
	}

	[[nodiscard]]
	auto pop() noexcept -> std::vector<uint8_t>
	{
		constexpr std::size_t headerSize {RingBuffer<S, A>::AlignedSize(sizeof(typename RingBuffer<S, A>::MessageHeader))};

		if (isEmpty())
		{
			return {};
		}

		std::lock_guard lock(m_lock);
		auto* messageHeader {reinterpret_cast<typename RingBuffer<S, A>::MessageHeader*>(m_ringBuffer->buffer.data() + m_ringBuffer->front)};
		std::unique_lock messageLock(messageHeader->lock);
		const std::size_t size {messageHeader->size};
		const std::size_t alignedSize {RingBuffer<S, A>::AlignedSize(size)};
		std::size_t tmpFront = m_ringBuffer->front;
		std::size_t tmpFreeSpace = m_ringBuffer->freeSpace;

		std::vector<uint8_t> data {};
		data.reserve(size);

		// Rewrite as header + data
		if (tmpFront + headerSize < m_ringBuffer->buffer.size())
		{
			tmpFreeSpace += headerSize;
			tmpFront += headerSize;
		}
		else
		{
			tmpFreeSpace += m_ringBuffer->buffer.size() - tmpFront + headerSize;
			tmpFront = 0u;
		}

		if (tmpFront + size <= m_ringBuffer->buffer.size())
		{
			std::copy_n(m_ringBuffer->buffer.cbegin() + tmpFront, size, std::back_inserter(data));

			if constexpr (IsDebugBuild())
			{
				std::fill_n(m_ringBuffer->buffer.begin() + tmpFront, size, 0u);
			}

			if (tmpFront + size == m_ringBuffer->buffer.size())
			{
				tmpFront = 0u;
				tmpFreeSpace += size;
			}
			else
			{
				tmpFront += alignedSize;
				tmpFreeSpace += alignedSize;
			}
		}
		else
		{
			const std::size_t part1Size = m_ringBuffer->buffer.size() - tmpFront;
			std::copy_n(m_ringBuffer->buffer.cbegin() + tmpFront, part1Size, std::back_inserter(data));
			std::copy_n(m_ringBuffer->buffer.cbegin(), size - part1Size, std::back_inserter(data));

			if constexpr (IsDebugBuild())
			{
				std::fill_n(m_ringBuffer->buffer.begin() + tmpFront, part1Size, 0u);
				std::fill_n(m_ringBuffer->buffer.begin(), size - part1Size, 0u);
			}

			tmpFreeSpace += part1Size;
			tmpFront = RingBuffer<S, A>::AlignedSize(size - part1Size);
			tmpFreeSpace += RingBuffer<S, A>::AlignedSize(size - part1Size);
		}

		m_ringBuffer->front = static_cast<uint32_t>(tmpFront);
		m_ringBuffer->freeSpace = static_cast<uint32_t>(tmpFreeSpace);
		messageLock.unlock();

		if (m_ringBuffer->freeSpace == m_ringBuffer->buffer.size())
		{
			m_ringBuffer->front = 0u;
			m_ringBuffer->next = 0u;
		}

		--m_ringBuffer->messageCount;

		if constexpr (IsDebugBuild())
		{
			messageHeader->size = 0;
		}

		return data;
	}

private:
	RingBuffer<S, A>* m_ringBuffer;
	mutable DekkarLock m_lock {m_ringBuffer->rxWaiting, m_ringBuffer->txWaiting, m_ringBuffer->turn};
};

#endif  // RING_BUFFER_RX_H_
