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

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <format>
#include <stdexcept>

void PosixSharedMemory::create(const std::string& name, std::size_t size)
{
	if (m_handle != 0)
	{
		throw std::runtime_error("Shared memory already created.");
	}

	m_name = name;
	m_size = size;

	// 1. Create shared memory mapping using the name as the id
	m_handle = shm_open(m_name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if (m_handle == -1)
	{
		throw std::runtime_error(std::format("Failed to create file mapping object. Errno: {}", errno));
	}

	ftruncate(m_handle, m_size);

	// 2. Create a file mapping of the shared memory
	auto buffer = mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_handle, 0);

	if (reinterpret_cast<intptr_t>(buffer) == -1)
	{
		shm_unlink(m_name.c_str());
		m_handle = 0u;
		throw std::runtime_error(std::format("Failed to map view of file. Errno: {}", errno));
	}

	m_buffer = reinterpret_cast<std::byte*>(buffer);

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
	m_handle = shm_open(m_name.c_str(), O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

	if (m_handle == -1)
	{
		throw std::runtime_error(std::format("Failed to open file mapping object. Errno: {}", errno));
	}

	// Initially mmap just enough to read the header and get the size
	auto tmp_buffer = mmap(nullptr, kSharedMemoryViewDataOffset, PROT_READ | PROT_WRITE, MAP_SHARED, m_handle, 0);

	if (reinterpret_cast<intptr_t>(tmp_buffer) == -1)
	{
		shm_unlink(m_name.c_str());
		m_handle = 0u;
		throw std::runtime_error(std::format("Failed to map view of file. Errno: {}", errno));
	}

	m_size = *reinterpret_cast<uint32_t*>(reinterpret_cast<std::byte*>(tmp_buffer) + kSharedMemoryViewDataSizeOffset) + kSharedMemoryViewDataOffset;

	if (munmap(tmp_buffer, m_size) == -1)
	{
		throw std::runtime_error(std::format("Failed to unmap view of file. Errno: {}", errno));
	}

	// 2. Create a file mapping of the shared memory
	auto buffer = mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_handle, 0);

	if (reinterpret_cast<intptr_t>(buffer) == -1)
	{
		shm_unlink(m_name.c_str());
		m_handle = 0u;
		throw std::runtime_error(std::format("Failed to map view of file. Errno: {}", errno));
	}

	m_buffer = reinterpret_cast<std::byte*>(buffer);

	// Configure the shared memory view
	m_view.lock = reinterpret_cast<std::atomic_flag*>(m_buffer + kSharedMemoryViewLockOffset);

	while (m_view.lock->test_and_set(std::memory_order_acquire));

	m_view.signals = reinterpret_cast<std::bitset<32u>*>(m_buffer + kSharedMemoryViewSignalsOffset);
	m_view.refCount = reinterpret_cast<uint32_t*>(m_buffer + kSharedMemoryViewRefCountOffset);
	(*m_view.refCount)++;
	m_view.dataSize = reinterpret_cast<uint32_t*>(m_buffer + kSharedMemoryViewDataSizeOffset);
	m_view.data = m_buffer + kSharedMemoryViewDataOffset;

	m_view.lock->clear(std::memory_order_release);
}

void PosixSharedMemory::close()
{
	while (m_view.lock->test_and_set(std::memory_order_acquire));

	if (*m_view.refCount > 0)
	{
		--(*m_view.refCount);
	}

	m_view.lock->clear(std::memory_order_release);

	if (m_buffer)
	{
		if (munmap(m_buffer, m_size) == -1)
		{
			throw std::runtime_error(std::format("Failed to unmap view of file. Errno: {}", errno));
		}

		m_buffer = nullptr;
		m_view = {};
	}

	if (m_handle > 0)
	{
		if (shm_unlink(m_name.c_str()))
		{
			// I wouldn't expect the file to be missing here, will need to investigate further
			if (errno != ENOENT)
			{
				throw std::runtime_error(std::format("Failed to unlink shared memory file. Errno: {}", errno));
			}
		}

		m_handle = 0u;
	}
}

void PosixSharedMemory::closeAll()
{
	while (m_view.lock->test_and_set(std::memory_order_acquire));
	m_view.signals->set(static_cast<uint32_t>(Signal::Close));
	m_view.lock->clear(std::memory_order_release);

	// First close any existing views
	while (*m_view.refCount > 1)
	{
		usleep(1000);
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
