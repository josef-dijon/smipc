#include "ring_buffer/ring_buffer_rx.h"
#include "ring_buffer/ring_buffer_tx.h"

#include <array>
#include <atomic>
#include <cstdint>
#include <format>
#include <iostream>
#include <ostream>
#include <span>
#include <vector>

struct PacketHeader
{
	uint32_t checksum {};
	uint32_t size {};
	uint32_t packetCount {};
	uint32_t packetId {};
	uint32_t transferId {};

	std::atomic_bool lock {false};
	uint8_t padding[3] {0, 0, 0};
};

class Packet
{
public:
	Packet(uint32_t packetCount, uint32_t packetId, uint32_t transferId)
		: m_header {0u, 0u, packetCount, packetId, transferId, false}
	{}

	Packet(std::span<const uint8_t> bytes)
	{
		const auto header {reinterpret_cast<const PacketHeader*>(bytes.data())};
		m_header.checksum = header->checksum;
		m_header.size = header->size;
		m_header.packetCount = header->packetCount;
		m_header.packetId = header->packetId;
		m_header.transferId = header->transferId;
		m_data = bytes.subspan(sizeof(PacketHeader));
	}

	void setData(std::span<const uint8_t> data)
	{
		m_data = data;
		m_header.size = data.size();
		m_header.checksum = 0;
	}

	auto getData() -> std::span<const uint8_t>
	{
		return m_data;
	}

	auto getHeader() -> std::span<const uint8_t>
	{
		return std::span<const uint8_t> {reinterpret_cast<const uint8_t*>(&m_header), sizeof(PacketHeader)};
	}

	auto getPacket() -> std::vector<uint8_t>
	{
		std::vector<uint8_t> packet {};
		packet.reserve(sizeof(PacketHeader) + m_data.size());

		packet.insert(packet.end(), reinterpret_cast<const uint8_t*>(&m_header), reinterpret_cast<const uint8_t*>(&m_header) + sizeof(PacketHeader));
		packet.insert(packet.end(), m_data.begin(), m_data.end());

		return packet;
	}

	void printPacket() const
	{
		std::cout << std::format("  Size: {}\n  PacketId: {}\n  PacketCount: {}\n  TransferId: {}\n  Checksum: {}\n", m_header.size, m_header.packetId, m_header.packetCount, m_header.transferId, m_header.checksum);
		std::cout << "  Data: ";
		for (const auto& data : m_data)
		{
			std::cout << std::format("{:02X} ", data);
		}
		std::cout << std::endl;
	}

private:
	PacketHeader m_header {};
	std::span<const uint8_t> m_data {};
};

int main()
{
	uint8_t buffer[128] {};
	RingBufferRx<128u, 4u> rx {buffer};
	RingBufferTx<128u, 4u> tx {buffer};

	Packet packet {1, 1, 30};

	std::array<uint8_t, 5u> data {1, 2, 3, 4, 5};

	packet.setData(data);
	std::cout << "Tx Packet: \n";
	packet.printPacket();

	tx.push(packet.getPacket());

	const Packet rxPacket {rx.pop()};
	std::cout << "Rx Packet: \n";
	rxPacket.printPacket();

	return 0;
}
