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

#include <libsmipc/ring-buffer/tx-ring-buffer.hpp>

#include <algorithm>
#include <mutex>
#include <stdexcept>

TxRingBuffer::TxRingBuffer(uint8_t* memory, std::size_t size)
	: RingBuffer{memory, size}
{}

[[nodiscard]]
auto TxRingBuffer::isFull() const noexcept -> bool
{
	std::lock_guard lock(m_lock);
	return header->freeSpace <= 0u;
}

void TxRingBuffer::push(const Packet& packet)
{
	// If the data size is 0, there is nothing to do
	if (packet.data.size() == 0)
	{
		return;
	}

	const uint32_t headerSize {static_cast<uint32_t>(sizeof(PacketHeader))};
	const uint32_t dataSize {static_cast<uint32_t>(packet.data.size())};
	const uint32_t alignedDataSize {AlignedSize(dataSize)};
	const uint32_t packetSize {headerSize + alignedDataSize};

	std::lock_guard lock(m_lock);

	// Read the buffer header data
	uint32_t tmpFront {header->front};
	uint32_t tmpNext {header->next};
	uint32_t tmpFreeSpace {header->freeSpace};
	uint32_t tmpMessageCount {header->messageCount};

	// Check if there is space for the header and the unaligned data, throw if not
	if (packetSize > tmpFreeSpace)
	{
		throw std::overflow_error("Buffer overflow");
	}

	const uint32_t headerStart {tmpNext};
	const bool headerWrap {(tmpFront <= tmpNext) && (headerStart + headerSize > data.size())};
	tmpNext = (tmpNext + headerSize) % data.size();
	tmpFreeSpace -= headerSize;

	const uint32_t dataStart {tmpNext};
	const bool dataWrap {(tmpFront <= tmpNext) && (dataStart + dataSize > data.size())};
	tmpNext = (tmpNext + alignedDataSize) % data.size();
	tmpFreeSpace -= alignedDataSize;

	++tmpMessageCount;

	// Copy back the buffer header data and unlock the buffer
	// Unlocking the buffer is safe because the message is locked
	// This means readers can read other messages but not this one
	header->front = tmpFront;
	header->next = tmpNext;
	header->freeSpace = tmpFreeSpace;
	header->messageCount = tmpMessageCount;

	if (headerWrap)
	{
		const std::size_t part1Size = data.size() - headerStart;
		std::copy_n(reinterpret_cast<const uint8_t*>(&packet.header), part1Size, std::begin(data) + headerStart);
		std::copy_n(reinterpret_cast<const uint8_t*>(&packet.header) + part1Size, headerSize - part1Size, std::begin(data));
	}
	else
	{
		std::copy_n(reinterpret_cast<const uint8_t*>(&packet.header), headerSize, std::begin(data) + headerStart);
	}

	if (dataWrap)
	{
		const std::size_t part1Size = data.size() - dataStart;
		std::copy_n(std::begin(packet.data), part1Size, std::begin(data) + dataStart);
		std::copy_n(std::begin(packet.data) + part1Size, dataSize - part1Size, std::begin(data));
	}
	else
	{
		std::copy(std::begin(packet.data), std::end(packet.data), std::begin(data) + dataStart);
	}
}