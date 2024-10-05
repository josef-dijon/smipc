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

#ifndef SHARED_MEMORY_FILE_H_
#define SHARED_MEMORY_FILE_H_

#include <libsmipc/shared-memory/atomic-memory-view.hpp>

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

class SharedMemoryFile
{
public:
	SharedMemoryFile(std::string_view name, std::size_t size);
	~SharedMemoryFile();

	auto getName() const -> const std::string&;
	auto getSize() const -> std::size_t;

	auto read() const -> std::vector<std::byte>;
	void write(std::span<const std::byte> data);

private:
	void openSharedMemoryFile();
	void closeSharedMemoryFile();

	std::string m_name {};
	std::size_t m_size {};

	std::uintptr_t m_handle {};
	std::byte* m_buffer {};
	AtomicMemoryView m_view {};
};

auto MakeSharedMemoryFile(std::string_view name, std::size_t size) -> std::unique_ptr<SharedMemoryFile>;

#endif  // SHARED_MEMORY_FILE_H_
