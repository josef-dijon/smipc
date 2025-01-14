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

#include <algorithm>
#include <iterator>
#include <mutex>

RxRingBuffer::RxRingBuffer(uint8_t* memory, std::size_t size)
	: RingBuffer {memory, size}
{}

[[nodiscard]]
auto RxRingBuffer::isEmpty() const noexcept -> bool
{
	std::lock_guard lock(m_lock);
	return header->freeSpace == data.size();
}

[[nodiscard]]
auto RxRingBuffer::getMessageCount() const noexcept -> uint32_t
{
	std::lock_guard lock(m_lock);
	return header->messageCount;
}

[[nodiscard]]
auto RxRingBuffer::pull() -> Packet
{
	if (isEmpty())
	{
		throw std::runtime_error("No packets in buffer");
	}

	constexpr uint32_t headerSize {AlignedSize(sizeof(PacketHeader))};

	std::lock_guard lock(m_lock);

	uint32_t tmpFront {header->front};
	uint32_t tmpNext {header->next};
	uint32_t tmpFreeSpace {header->freeSpace};
	uint32_t tmpMessageCount {header->messageCount};

	const uint32_t headerStart {tmpFront};
	const bool headerWrap {tmpFront + headerSize > data.size()};
	tmpFront = (tmpFront + headerSize) % data.size();
	tmpFreeSpace += headerSize;

	Packet packet;

	if (headerWrap)
	{
		const uint32_t part1Size = data.size() - headerStart;
		std::copy_n(std::cbegin(data) + headerStart, part1Size, reinterpret_cast<uint8_t*>(&packet.header));
		std::copy_n(std::cbegin(data), headerSize - part1Size, reinterpret_cast<uint8_t*>(&packet.header) + part1Size);

		if constexpr (IsDebugBuild())
		{
			std::fill_n(std::begin(data) + headerStart, part1Size, 0u);
			std::fill_n(std::begin(data), headerSize - part1Size, 0u);
		}
	}
	else
	{
		std::copy_n(std::cbegin(data) + headerStart, headerSize, reinterpret_cast<uint8_t*>(&packet.header));

		if constexpr (IsDebugBuild())
		{
			std::fill_n(std::begin(data) + headerStart, headerSize, 0u);
		}
	}

	const uint32_t dataSize {packet.header.size};
	const uint32_t alignedDataSize {AlignedSize(dataSize)};
	const uint32_t dataStart {tmpFront};
	const bool dataWrap {tmpFront + dataSize > data.size()};
	tmpFront = (tmpFront + alignedDataSize) % data.size();
	tmpFreeSpace += alignedDataSize;

	packet.data.reserve(dataSize);

	if (dataWrap)
	{
		const uint32_t part1Size = data.size() - dataStart;
		std::copy_n(std::cbegin(data) + dataStart, part1Size, std::back_inserter(packet.data));
		std::copy_n(std::cbegin(data), dataSize - part1Size, std::back_inserter(packet.data));

		if constexpr (IsDebugBuild())
		{
			std::fill_n(std::begin(data) + dataStart, part1Size, 0u);
			std::fill_n(std::begin(data), dataSize - part1Size, 0u);
		}
	}
	else
	{
		std::copy_n(std::cbegin(data) + dataStart, dataSize, std::back_inserter(packet.data));

		if constexpr (IsDebugBuild())
		{
			std::fill_n(std::begin(data) + dataStart, dataSize, 0u);
		}
	}

	if constexpr (IsDebugBuild())
	{
		packet.header.size = 0;
	}

	if (tmpFreeSpace == data.size())
	{
		tmpFront = 0u;
		tmpNext = 0u;
	}

	header->front = tmpFront;
	header->freeSpace = tmpFreeSpace;
	header->next = tmpNext;
	header->messageCount = --tmpMessageCount;

	return packet;
}
