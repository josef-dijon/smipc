/* MIT License
 * 
 * Copyright (c) 2024 Josef de Joanelli (josef@pixelrift.io)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "libsmipc/ring-buffer/dekkar-lock.hpp"

DekkarLock::DekkarLock(std::atomic_bool& flag1, std::atomic_bool& flag2, std::atomic_bool& turn) noexcept
	: m_flag1 {flag1}
	, m_flag2 {flag2}
	, m_turn {turn}
{}

void DekkarLock::lock() noexcept
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

void DekkarLock::unlock() noexcept
{
	m_turn.store(true);
	m_flag1.store(false);
}

bool DekkarLock::tryLock() noexcept
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
