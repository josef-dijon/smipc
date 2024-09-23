#ifndef ATOMIC_SPIN_LOCK_H_
#define ATOMIC_SPIN_LOCK_H_

#include <atomic>

class AtomicSpinLock
{
public:
	AtomicSpinLock() = default;
	~AtomicSpinLock() = default;
	AtomicSpinLock(const AtomicSpinLock&) = delete;
	AtomicSpinLock& operator=(const AtomicSpinLock&) = delete;
	AtomicSpinLock(AtomicSpinLock&&) = delete;
	AtomicSpinLock& operator=(AtomicSpinLock&&) = delete;

	void lock() noexcept;
	void unlock() noexcept;
	bool tryLock() noexcept;

private:
	std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
};

#endif  // ATOMIC_SPIN_LOCK_H_
