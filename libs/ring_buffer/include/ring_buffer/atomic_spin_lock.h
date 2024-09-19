#ifndef ATOMIC_SPIN_LOCK_H_
#define ATOMIC_SPIN_LOCK_H_

#include <atomic>

class AtomicSpinLock
{
public:
	AtomicSpinLock()                                 = default;
	~AtomicSpinLock()                                = default;
	AtomicSpinLock(const AtomicSpinLock&)            = delete;
	AtomicSpinLock& operator=(const AtomicSpinLock&) = delete;
	AtomicSpinLock(AtomicSpinLock&&)                 = delete;
	AtomicSpinLock& operator=(AtomicSpinLock&&)      = delete;

	void lock() noexcept
	{
		while (! try_lock())
		{
			;
		}
	}

	void unlock() noexcept
	{
		m_lock.clear(std::memory_order_release);
	}

	bool try_lock() noexcept
	{
		return m_lock.test_and_set(std::memory_order_acquire);
	}

private:
	std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
};

#endif  // ATOMIC_SPIN_LOCK_H_
