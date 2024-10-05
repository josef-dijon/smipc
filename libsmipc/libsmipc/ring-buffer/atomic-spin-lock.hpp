#ifndef ATOMIC_SPIN_LOCK_H_
#define ATOMIC_SPIN_LOCK_H_

#include <atomic>

class AtomicSpinLock
{
public:
	void lock() noexcept;
	void unlock() noexcept;
	bool tryLock() noexcept;

private:
	std::atomic_bool m_lock;
};

#endif  // ATOMIC_SPIN_LOCK_H_
