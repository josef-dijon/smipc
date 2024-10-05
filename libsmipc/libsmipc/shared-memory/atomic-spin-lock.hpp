#ifndef ATOMIC_SPIN_LOCK_H_
#define ATOMIC_SPIN_LOCK_H_

#include <atomic>

struct AtomicSpinLock
{
public:
	AtomicSpinLock(std::atomic_flag* flag)
		: m_flag {flag}
	{
		while (m_flag->test_and_set(std::memory_order_acquire))
		{
			;
		}
	}

	~AtomicSpinLock()
	{
		m_flag->clear(std::memory_order_release);
	}

private:
	std::atomic_flag* m_flag;
};

#endif  // ATOMIC_SPIN_LOCK_H_
