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

#include "libsmipc/shared-memory/duplex-shared-memory-file.hpp"

#include <libsmipc/shared-memory/shared-memory-file.hpp>

#include <stdexcept>
#include <string>

DuplexSharedMemoryFile::DuplexSharedMemoryFile(std::string_view rxName, std::string_view txName, std::size_t size)
{
	try
	{
		m_sharedMemoryFileRx = MakeSharedMemoryFile(rxName, size);
	}
	catch ([[maybe_unused]]
		   const std::runtime_error& e)
	{
		// handle exception
	}

	try
	{
		m_sharedMemoryFileTx = MakeSharedMemoryFile(txName, size);
	}
	catch ([[maybe_unused]]
		   const std::runtime_error& e)
	{
		// handle exception
	}
}

DuplexSharedMemoryFile::~DuplexSharedMemoryFile() = default;

auto DuplexSharedMemoryFile::read() -> std::span<const std::byte>
{
	return m_sharedMemoryFileRx->read();
}

void DuplexSharedMemoryFile::write(std::span<const std::byte> data)
{
	m_sharedMemoryFileTx->write(data);
}

auto MakeDuplexSharedMemoryFile(std::string_view name, std::size_t size, bool swapRxTx) -> std::unique_ptr<DuplexSharedMemoryFile>
{
	const std::string rxName = swapRxTx ? std::string {name} + ".tx" : std::string {name} + ".rx";
	const std::string txName = swapRxTx ? std::string {name} + ".rx" : std::string {name} + ".tx";

	return std::make_unique<DuplexSharedMemoryFile>(rxName, txName, size);
}
