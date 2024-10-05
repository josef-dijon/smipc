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

#ifndef DUPLEX_SHARED_MEMORY_FILE_H_
#define DUPLEX_SHARED_MEMORY_FILE_H_

#include <memory>
#include <span>
#include <string_view>

class SharedMemoryFile;

class DuplexSharedMemoryFile
{
public:
	DuplexSharedMemoryFile(std::string_view rxName, std::string_view txName, std::size_t size);
	~DuplexSharedMemoryFile();

	auto read() -> std::span<const std::byte>;
	void write(std::span<const std::byte> data);

private:
	std::unique_ptr<SharedMemoryFile> m_sharedMemoryFileRx {};
	std::unique_ptr<SharedMemoryFile> m_sharedMemoryFileTx {};
};

auto MakeDuplexSharedMemoryFile(std::string_view name, std::size_t size, bool is_client) -> std::unique_ptr<DuplexSharedMemoryFile>;

#endif  // DUPLEX_SHARED_MEMORY_FILE_H_
