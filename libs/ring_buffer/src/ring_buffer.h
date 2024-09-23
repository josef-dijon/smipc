#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "atomic_spin_lock.h"

#include <array>
#include <atomic>
#include <cstdint>

static constexpr bool IsDebugBuild()
{
#ifndef DEBUG
	return true;
#else
	return false;
#endif
}

struct MessageHeader
{
	MessageHeader(std::size_t size_)
		: size {static_cast<uint32_t>(size_)}
	{}
	AtomicSpinLock lock {};
	uint32_t size {0u};
};

template <std::size_t S, std::size_t A>
struct RingBuffer
{
	static constexpr std::size_t AlignedSize(std::size_t size)
	{
		return size + ((A - (size % A)) % A);
	}

	static constexpr std::size_t BufferSize()
	{
		return AlignedSize(S) - 4 * sizeof(uint32_t) - 3 * sizeof(std::atomic_flag) - sizeof(uint8_t);
	}

	static_assert(S > BufferSize(), "RingBuffer S template parameter is too small");
	static_assert(S <= 2048u * 1024u * 1024u, "RingBuffer S template parameter is too large, must be less than 2GB");
	static_assert(A >= 4, "RingBuffer A template parameter is too small, must be >= 4");
	static_assert((A & (A - 1)) == 0, "RingBuffer A template parameter is not power of 2");
	static_assert((S & (S - 1)) == 0, "RingBuffer S template parameter is not power of 2");

	struct RingBufferHeader
	{
		std::atomic_bool rxWaiting {false};
		std::atomic_bool txWaiting {false};
		std::atomic_bool turn {false};
		uint8_t padding_ {0u};

		uint32_t front {0u};
		uint32_t next {0u};
		uint32_t freeSpace {static_cast<uint32_t>(BufferSize())};
		uint32_t messageCount {0u};
	};

	using RingBufferData = std::array<uint8_t, BufferSize()>;

	RingBufferHeader header {};
	RingBufferData data {};
};

#endif  // RING_BUFFER_H_
