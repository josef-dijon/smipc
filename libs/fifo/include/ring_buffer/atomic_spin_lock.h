#ifndef ATOMIC_SPIN_LOCK_H_
#define ATOMIC_SPIN_LOCK_H_

#include <atomic>

class AtomicSpinLock {
public:
    AtomicSpinLock() = default;
    ~AtomicSpinLock() = default;
    AtomicSpinLock(const AtomicSpinLock&) = delete;
    AtomicSpinLock& operator=(const AtomicSpinLock&) = delete;
    AtomicSpinLock(AtomicSpinLock&&) = delete;
    AtomicSpinLock& operator=(AtomicSpinLock&&) = delete;

    void lock() noexcept {
        while (!try_lock()) {;}
    }

    void unlock() noexcept {
        lock_.clear(std::memory_order_release);
    }

    bool try_lock() noexcept {
        return lock_.test_and_set(std::memory_order_acquire);
    }
private:
    std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
};

#endif  // ATOMIC_SPIN_LOCK_H_