#include "ring_buffer/packet.h"
#include "ring_buffer/ring_buffer_rx.h"
#include "ring_buffer/ring_buffer_tx.h"

int main()
{
	uint8_t buffer[128] {};
	RingBufferRx<128u, 4u> rx {buffer};
	RingBufferTx<128u, 4u> tx {buffer};

	std::array<uint8_t, 5u> data {1, 2, 3, 4, 5};
	Packet packet {data, 1, 30};

	std::cout << "Tx Packet: \n";
	packet.print();
	tx.push(packet.bytes());

	const Packet rxPacket {rx.pop()};
	std::cout << "Rx Packet: \n";
	rxPacket.print();

	return 0;
}
