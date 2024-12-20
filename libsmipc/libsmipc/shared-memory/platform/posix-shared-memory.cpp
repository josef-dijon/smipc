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

#include <libsmipc/shared-memory/platform/posix-shared-memory.hpp>

#include <format>
#include <stdexcept>

static std::byte* tmpBuffer = nullptr;

void PosixSharedMemory::create(const std::string& name, std::size_t size)
{
	tmpBuffer = new std::byte[size]();
	if (m_handle != 0)
	{
		throw std::runtime_error("Shared memory already created.");
	}

	m_name = name;
	m_size = size;

	// 1. Create shared memory mapping using the name as the id
	m_handle = 1;

	// if (reinterpret_cast<HANDLE>(m_handle) == nullptr)
	// {
	// 	throw std::runtime_error(std::format("Failed to create file mapping object. Error code: {}", GetLastError()));
	// }

	// 2. Create a file mapping of the shared memory
	m_buffer = tmpBuffer;

	// if (m_buffer == nullptr)
	// {
	// 	const auto errorCode = GetLastError();
	// 	CloseHandle(reinterpret_cast<HANDLE>(m_handle));
	// 	throw std::runtime_error(std::format("Failed to map view of file. Error code: {}", errorCode));
	// }

	// Configure and initialise the shared memory view
	m_view.lock = reinterpret_cast<std::atomic_flag*>(m_buffer + kSharedMemoryViewLockOffset);
	m_view.lock->clear(std::memory_order_release);

	while (m_view.lock->test_and_set(std::memory_order_acquire));

	m_view.signals = reinterpret_cast<std::bitset<32u>*>(m_buffer + kSharedMemoryViewSignalsOffset);
	*m_view.signals = 0u;
	m_view.refCount = reinterpret_cast<uint32_t*>(m_buffer + kSharedMemoryViewRefCountOffset);
	*m_view.refCount = 1u;
	m_view.dataSize = reinterpret_cast<uint32_t*>(m_buffer + kSharedMemoryViewDataSizeOffset);
	*m_view.dataSize = m_size - kSharedMemoryViewDataOffset;
	m_view.data = m_buffer + kSharedMemoryViewDataOffset;

	m_view.lock->clear(std::memory_order_release);
}

void PosixSharedMemory::open(const std::string& name)
{
	if (m_handle != 0)
	{
		throw std::runtime_error("Shared memory already opened.");
	}

	m_name = name;

	// 1. Open shared memory mapping using the name as the id
	m_handle = 1;

	// if (reinterpret_cast<HANDLE>(m_handle) == nullptr)
	// {
	// 	throw std::runtime_error(std::format("Failed to open file mapping object. Error code: {}", GetLastError()));
	// }

	// 2. Create a file mapping of the shared memory
	m_buffer = tmpBuffer;
	
	// if (m_buffer == nullptr)
	// {
	// 	const auto errorCode = GetLastError();
	// 	CloseHandle(reinterpret_cast<HANDLE>(m_handle));
	// 	throw std::runtime_error(std::format("Failed to map view of file. Error code: {}", errorCode));
	// }

	// Configure the shared memory view
	m_view.lock = reinterpret_cast<std::atomic_flag*>(m_buffer + kSharedMemoryViewLockOffset);

	while (m_view.lock->test_and_set(std::memory_order_acquire));

	m_view.signals = reinterpret_cast<std::bitset<32u>*>(m_buffer + kSharedMemoryViewSignalsOffset);
	m_view.refCount = reinterpret_cast<uint32_t*>(m_buffer + kSharedMemoryViewRefCountOffset);
	(*m_view.refCount)++;
	m_view.dataSize = reinterpret_cast<uint32_t*>(m_buffer + kSharedMemoryViewDataSizeOffset);
	m_size = *m_view.dataSize + kSharedMemoryViewDataOffset;
	m_view.data = m_buffer + kSharedMemoryViewDataOffset;

	m_view.lock->clear(std::memory_order_release);
}

void PosixSharedMemory::close()
{
	while(m_view.lock->test_and_set(std::memory_order_acquire));

	if (*m_view.refCount > 0)
	{
		--(*m_view.refCount);
	}

	m_view.lock->clear(std::memory_order_release);

	if (m_buffer)
	{
		//UnmapViewOfFile(m_buffer);
		delete[] tmpBuffer;
		m_buffer = nullptr;
		m_view = {};
	}

	if (m_handle)
	{
		// CloseHandle(reinterpret_cast<HANDLE>(m_handle));
		m_handle = 0u;
	}
}

void PosixSharedMemory::closeAll()
{
	// First close any existing views
	if (*m_view.refCount > 1)
	{
		while (m_view.lock->test_and_set(std::memory_order_acquire));
		m_view.signals->set(static_cast<uint32_t>(Signal::Close));
		m_view.lock->clear(std::memory_order_release);

		while (*m_view.refCount > 1)
		{
			// sleep 1
		}
	}

	// Now close the shared memory
	close();
}

auto PosixSharedMemory::getName() const -> std::string_view
{
	return m_name;
}

auto PosixSharedMemory::getSize() const -> std::size_t
{
	return m_size;
}

auto PosixSharedMemory::getView() -> SharedMemoryView
{
	return m_view;
}

auto PosixSharedMemory::getView() const -> const SharedMemoryView
{
	return m_view;
}
