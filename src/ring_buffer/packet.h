#ifndef PACKET_H_
#define PACKET_H_

#include "atomic_spin_lock.h"
#include "fmt/core.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <iostream>
#include <vector>

static uint32_t MakeTransferId()
{
	static std::atomic<uint32_t> transferId {1u};
	return transferId++;
}

struct PacketHeader
{
	uint32_t checksum {};
	uint32_t size {};
	uint32_t packetId {};
	uint32_t packetCount {};
	uint32_t transferId {MakeTransferId()};
};

struct Packet
{
	void print() const
	{
		std::cout << fmt::format("  Size: {}\n  PacketId: {}\n  PacketCount: {}\n  TransferId: {}\n  Checksum: {}\n", header.size, header.packetId, header.packetCount, header.transferId, header.checksum);
		std::cout << "  Data: ";
		for (const auto& byte : data)
		{
			std::cout << fmt::format("{:02X} ", byte);
		}
		std::cout << std::endl;
	}

	/*
	auto getBytes() const -> std::vector<uint8_t>
	{
		std::vector<uint8_t> bytes;
		bytes.reserve(sizeof(PacketHeader) + data.size());
		std::copy_n(reinterpret_cast<const uint8_t*>(&header), sizeof(PacketHeader), std::back_inserter(bytes));
		std::copy(data.begin(), data.end(), std::back_inserter(bytes));
		return bytes;
	}
	*/

	PacketHeader header {};
	std::vector<uint8_t> data {};
};

#endif  // PACKET_H_
