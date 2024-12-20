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

#ifndef SHARED_MEMORY_VIEW_H_
#define SHARED_MEMORY_VIEW_H_

#include <atomic>
#include <cstdint>
#include <bitset>

enum class Signal : uint32_t
{
	Close,
};

struct SharedMemoryView
{
	std::atomic_flag* lock {nullptr};
	uint32_t* refCount {0u};
	std::bitset<32u>* signals {0u};
	uint32_t* dataSize {nullptr};
	std::byte* data {nullptr};
};

constexpr std::size_t kSharedMemoryViewLockOffset {offsetof(SharedMemoryView, lock)};
constexpr std::size_t kSharedMemoryViewRefCountOffset {offsetof(SharedMemoryView, refCount)};
constexpr std::size_t kSharedMemoryViewSignalsOffset {offsetof(SharedMemoryView, signals)};
constexpr std::size_t kSharedMemoryViewDataSizeOffset {offsetof(SharedMemoryView, dataSize)};
constexpr std::size_t kSharedMemoryViewDataOffset {offsetof(SharedMemoryView, data)};

#endif  // SHARED_MEMORY_VIEW_H_
