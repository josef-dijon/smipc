#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

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

static constexpr std::size_t kAlignment {4u};

static constexpr uint32_t AlignedSize(uint32_t size)
{
	return size + ((kAlignment - (size % kAlignment)) % kAlignment);
}

template <uint32_t TSize>
struct RingBuffer
{
	struct RingBufferHeader
	{
		std::atomic_bool rxWaiting {false};
		std::atomic_bool txWaiting {false};
		std::atomic_bool turn {false};
		uint8_t padding_ {0u};

		uint32_t front {0u};
		uint32_t next {0u};
		uint32_t freeSpace {static_cast<uint32_t>(kBufferSize)};
		uint32_t messageCount {0u};
	};

	static constexpr uint32_t kBufferSize {TSize - sizeof(RingBufferHeader)};

	static_assert(TSize > kBufferSize, "RingBuffer TSize template parameter is too small");
	static_assert(TSize <= 2048u * 1024u * 1024u, "RingBuffer TSize template parameter is too large, must be less than 2GB");
	static_assert((TSize & (TSize - 1u)) == 0u, "RingBuffer TSIze template parameter is not power of 2");
	static_assert(TSize % kAlignment == 0u, "RingBuffer TSize template parameter is not 4 byte aligned");

	using RingBufferData = std::array<uint8_t, kBufferSize>;

	RingBufferHeader header {};
	RingBufferData data {};
};

#endif  // RING_BUFFER_H_
