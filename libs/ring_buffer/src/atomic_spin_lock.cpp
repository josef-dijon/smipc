#include "atomic_spin_lock.h"

void AtomicSpinLock::lock() noexcept
{
	while (! tryLock())
	{
		;
	}
}

void AtomicSpinLock::unlock() noexcept
{
	m_lock.clear(std::memory_order_release);
}

bool AtomicSpinLock::tryLock() noexcept
{
	return m_lock.test_and_set(std::memory_order_acquire);
}
