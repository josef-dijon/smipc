#ifndef DEKKAR_LOCK_H_
#define DEKKAR_LOCK_H_

#include <atomic>

class DekkarLock {
public:
    DekkarLock(std::atomic_bool& flag1, std::atomic_bool& flag2)
        : flag1_{flag1}, flag2_{flag2} {}
    DekkarLock() = delete;
    ~DekkarLock() = default;
    DekkarLock(const DekkarLock&) = delete;
    DekkarLock& operator=(const DekkarLock&) = delete;
    DekkarLock(DekkarLock&&) = delete;
    DekkarLock& operator=(DekkarLock&&) = delete;

    void lock() noexcept {
        while (!try_lock()) {;}
    }

    void unlock() noexcept {
        flag1_.store(false);
    }

    bool try_lock() noexcept {
        flag1_.store(true);
        return ! flag2_.load();
    }

private:
    std::atomic_bool& flag1_;
    std::atomic_bool& flag2_;
};

#endif  // DEKKAR_LOCK_H_