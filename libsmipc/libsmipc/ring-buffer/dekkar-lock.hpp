#ifndef DEKKAR_LOCK_H_
#define DEKKAR_LOCK_H_

#include <atomic>

class DekkarLock
{
public:
	DekkarLock(std::atomic_bool& flag1, std::atomic_bool& flag2, std::atomic_bool& turn) noexcept;
	DekkarLock() = delete;
	~DekkarLock() = default;
	DekkarLock(const DekkarLock&) = delete;
	DekkarLock& operator=(const DekkarLock&) = delete;
	DekkarLock(DekkarLock&&) = delete;
	DekkarLock& operator=(DekkarLock&&) = delete;

	void lock() noexcept;
	void unlock() noexcept;
	bool tryLock() noexcept;

private:
	std::atomic_bool& m_flag1;
	std::atomic_bool& m_flag2;
	std::atomic_bool& m_turn;
};

#endif  // DEKKAR_LOCK_H_
