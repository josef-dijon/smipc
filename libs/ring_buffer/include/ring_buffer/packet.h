#ifndef PACKET_H_
#define PACKET_H_

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <span>

struct PacketHeader
{
	uint32_t checksum {};
	uint32_t size {};
	uint32_t packetCount {};
	uint32_t packetId {};
	uint32_t transferId {};

	std::atomic_bool lock {false};
};

class Packet
{
public:
	Packet(uint32_t packetCount, uint32_t packetId, uint32_t transferId)
		: m_header {false, 0, packetCount, packetId, transferId, 0}
	{}

	void setData(std::span<const std::byte> data)
	{
		m_data = data;
		m_header.size = data.size();
		m_header.checksum = 0;
	}

	auto getData() -> std::span<const std::byte>
	{
		return m_data;
	}

	auto getHeader() -> std::span<const std::byte>
	{
		return std::span<const std::byte> {reinterpret_cast<const std::byte*>(&m_header), sizeof(PacketHeader)};
	}

private:
	PacketHeader m_header {};
	std::span<const std::byte> m_data {};
};

#endif  // PACKET_H_
