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

#ifndef RX_RING_BUFFER_HPP_
#define RX_RING_BUFFER_HPP_

#include <libsmipc/ring-buffer/dekkar-lock.hpp>
#include <libsmipc/ring-buffer/packet.hpp>
#include <libsmipc/ring-buffer/ring-buffer.hpp>

#include <cstdint>

class RxRingBuffer: private RingBuffer
{
public:
	RxRingBuffer(uint8_t* memory, std::size_t size);
	~RxRingBuffer() = default;

	[[nodiscard]]
	auto isEmpty() const noexcept -> bool;

	[[nodiscard]]
	auto getMessageCount() const noexcept -> uint32_t;

	[[nodiscard]]
	auto pull() -> Packet;

private:
	mutable DekkarLock m_lock {header->rxWaiting, header->txWaiting, header->turn};
};

#endif  // RX_RING_BUFFER_HPP_
