#ifndef DEKKAR_LOCK_H_
#define DEKKAR_LOCK_H_

#include <atomic>

class DekkarLock
{
public:
	DekkarLock(std::atomic_bool& flag1, std::atomic_bool& flag2, std::atomic_bool& turn) noexcept
		: m_flag1 {flag1}
		, m_flag2 {flag2}
		, m_turn {turn}
	{}

	DekkarLock()                             = delete;
	~DekkarLock()                            = default;
	DekkarLock(const DekkarLock&)            = delete;
	DekkarLock& operator=(const DekkarLock&) = delete;
	DekkarLock(DekkarLock&&)                 = delete;
	DekkarLock& operator=(DekkarLock&&)      = delete;

	void lock() noexcept
	{
		m_flag1.store(true);

		while (m_flag2.load())
		{
			if (m_flag1.load())
			{
				m_flag1.store(false);
				while (m_turn.load())
				{
					;
				}
				m_flag1.store(true);
			}
		}
	}

	void unlock() noexcept
	{
		m_turn.store(true);
		m_flag1.store(false);
	}

	bool try_lock() noexcept
	{
		m_flag1.store(true);

		if (m_flag2.load())
		{
			m_flag1.store(false);
			return false;
		}

		if (! m_flag1.load())
		{
			m_flag1.store(false);
			return false;
		}

		m_flag1.store(false);

		if (m_turn.load())
		{
			return false;
		}

		m_flag1.store(true);
		return true;
	}

private:
	std::atomic_bool& m_flag1;
	std::atomic_bool& m_flag2;
	std::atomic_bool& m_turn;
};

#endif  // DEKKAR_LOCK_H_
