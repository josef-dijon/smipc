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
#include <libsmipc/ring-buffer/tx-ring-buffer.hpp>

#include <vector>

int main()
{
	constexpr std::size_t kSize = 128;
	uint8_t buffer[kSize] {};
	TxRingBuffer tx(buffer, kSize);
	RxRingBuffer rx(buffer, kSize);

	Packet packet {
		std::vector<uint8_t> {1u, 2u, 3u, 4u, 5u}
    };

	std::cout << "Tx Packet: \n";
	packet.print();
	tx.push(packet);

	const Packet rxPacket {rx.pull()};
	std::cout << "Rx Packet: \n";
	rxPacket.print();

	return 0;
}
