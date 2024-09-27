#ifndef PACKET_H_
#define PACKET_H_

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <format>
#include <iostream>
#include <span>
#include <vector>

static uint32_t MakeTransferId()
{
	static std::atomic<uint32_t> transferId {1u};
	return transferId++;
}

class PacketHeader
{
public:
	PacketHeader(uint32_t size_, uint32_t packetId_, uint32_t packetCount_)
		: size {size_}
		, packetId {packetId_}
		, packetCount {packetCount_}
	{}

	PacketHeader(std::span<const uint8_t> bytes)
	{
		std::copy_n(bytes.begin(), sizeof(PacketHeader), reinterpret_cast<uint8_t*>(this));
	}

	uint32_t checksum {};
	uint32_t size {};
	uint32_t packetId {};
	uint32_t packetCount {};
	const uint32_t transferId {MakeTransferId()};
	std::atomic_bool lock {false};

private:
	uint8_t _m_padding[4 - sizeof(decltype(lock))] {};
};

class Packet
{
public:
	Packet(std::span<const uint8_t> data, uint32_t packetId, uint32_t packetCount)
		: m_header {static_cast<uint32_t>(data.size()), packetId, packetCount}
		, m_data {data}
	{}

	Packet(std::span<const uint8_t> bytes)
		: m_header {bytes.subspan(0u, sizeof(PacketHeader))}
		, m_data {bytes.subspan(sizeof(PacketHeader))}
	{}

	void print() const
	{
		std::cout << std::format("  Size: {}\n  PacketId: {}\n  PacketCount: {}\n  TransferId: {}\n  Checksum: {}\n", m_header.size, m_header.packetId, m_header.packetCount, m_header.transferId, m_header.checksum);
		std::cout << "  Data: ";
		for (const auto& data : m_data)
		{
			std::cout << std::format("{:02X} ", data);
		}
		std::cout << std::endl;
	}

	auto bytes() const -> std::vector<uint8_t>
	{
		std::vector<uint8_t> bytes;
		bytes.reserve(sizeof(PacketHeader) + m_data.size());
		std::copy_n(reinterpret_cast<const uint8_t*>(&m_header), sizeof(PacketHeader), std::back_inserter(bytes));
		std::copy(m_data.begin(), m_data.end(), std::back_inserter(bytes));
		return bytes;
	}

private:
	PacketHeader m_header;
	std::span<const uint8_t> m_data;
};

#endif  // PACKET_H_
