#ifndef ATOMIC_MEMORY_VIEW_H_
#define ATOMIC_MEMORY_VIEW_H_

#include <atomic>
#include <span>

struct AtomicMemoryView
{
	std::atomic_flag*    lock;
	std::span<std::byte> data;
	std::size_t*         messageSize;
};

#endif  // ATOMIC_MEMORY_VIEW_H_
