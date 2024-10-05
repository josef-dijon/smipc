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
