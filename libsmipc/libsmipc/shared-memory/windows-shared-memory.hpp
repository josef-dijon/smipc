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

#ifndef WINDOWS_SHARED_MEMORY_H_
#define WINDOWS_SHARED_MEMORY_H_

#include <libsmipc/shared-memory/abstract-shared-memory.hpp>
#include <libsmipc/shared-memory/atomic-memory-view.hpp>

#include <cstdint>
#include <string>

class WindowsSharedMemory: public ISharedMemory
{
public:
	void create(const std::string& name, std::size_t size) final;
	void open(const std::string& name) final;
	void close() final;
	auto getName() const -> std::string_view final;
	auto getSize() const -> std::size_t final;
	auto getView() -> AtomicMemoryView final;
	auto getView() const -> const AtomicMemoryView final;

private:
	std::size_t m_size {};
	std::string m_name {};
	std::uintptr_t m_handle {};
	std::byte* m_buffer {nullptr};
	AtomicMemoryView m_view {};
};

#endif  // WINDOWS_SHARED_MEMORY_H_