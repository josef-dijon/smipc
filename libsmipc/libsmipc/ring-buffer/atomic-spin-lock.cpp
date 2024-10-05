#include "libsmipc/ring-buffer/atomic-spin-lock.hpp"

void AtomicSpinLock::lock() noexcept
{
	while (! tryLock())
	{
		;
	}
}

void AtomicSpinLock::unlock() noexcept
{
	std::atomic_thread_fence(std::memory_order_release);
	m_lock.store(false, std::memory_order_relaxed);
}

bool AtomicSpinLock::tryLock() noexcept
{
	bool x {m_lock.exchange(true, std::memory_order_relaxed)};
	std::atomic_thread_fence(std::memory_order_acquire);
	return x;
}
