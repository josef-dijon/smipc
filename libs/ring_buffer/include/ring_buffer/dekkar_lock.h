#ifndef DEKKAR_LOCK_H_
#define DEKKAR_LOCK_H_

#include <atomic>

class DekkarLock {
public:
    template <bool Swap>
    DekkarLock(std::atomic_bool& flag1, std::atomic_bool& flag2, std::atomic_bool& turn) noexcept
        : flag1_{flag1}, flag2_{flag2}, turn_{turn}, swap_{false};

    DekkarLock() = delete;
    ~DekkarLock() = default;
    DekkarLock(const DekkarLock&) = delete;
    DekkarLock& operator=(const DekkarLock&) = delete;
    DekkarLock(DekkarLock&&) = delete;
    DekkarLock& operator=(DekkarLock&&) = delete;

    void lock() noexcept {
        flag1_.store(true);

        while (flag2_.load()) {
            if (flag1_.load()) {
                flag1_.store(false);
                while (turn_.load()) {;}
                flag1_.store(true);
            }
        }
    }

    void unlock() noexcept {
        turn_.store(true);
        flag1_.store(false);
    }

    bool try_lock() noexcept {
        flag1_.store(true);
        
        if (flag2_.load()) {
            flag1_.store(false);
            return false;
        }
        
        if (!flag1_.load()) {
            flag1_.store(false);
            return false;
        }
        
        flag1_.store(false);

        if (turn_.load()) {
            return false;
        }
        
        flag1_.store(true);
        return true;
    }

private:
    std::atomic_bool& flag1_;
    std::atomic_bool& flag2_;
    std::atomic_bool& turn_;
    bool swap_ {false};
};
    
template <>
DekkarLock::DekkarLock<false>(std::atomic_bool& flag1, std::atomic_bool& flag2, std::atomic_bool& turn) noexcept
    : flag1_{flag1}, flag2_{flag2}, turn_{turn}, swap_{false} {}

template <>
DekkarLock::DekkarLock<true>(std::atomic_bool& flag1, std::atomic_bool& flag2, std::atomic_bool& turn) noexcept
    : flag1_{flag2}, flag2_{flag1}, turn_{turn}, swap_{true} {}
    

#endif  // DEKKAR_LOCK_H_