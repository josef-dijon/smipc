#include "ring_buffer/ring_buffer_rx.h"
#include "ring_buffer/ring_buffer_tx.h"

#include <vector>

int main()
{
	uint8_t buffer[128] {};
	RingBufferRx<128u> rx {buffer};
	RingBufferTx<128u> tx {buffer};

	Packet packet {
		{},
		std::vector<uint8_t> {1u, 2u, 3u, 4u, 5u}
    };

	std::cout << "Tx Packet: \n";
	packet.print();
	tx.push(packet);

	const Packet rxPacket {rx.pop()};
	std::cout << "Rx Packet: \n";
	rxPacket.print();

	return 0;
}
