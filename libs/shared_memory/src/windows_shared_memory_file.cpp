#include "atomic_spin_lock.h"
#include "shared_memory_file.h"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <windows.h>

SharedMemoryFile::SharedMemoryFile(std::string_view name, std::size_t size)
{
	openSharedMemoryFile();
}

SharedMemoryFile::~SharedMemoryFile()
{
	closeSharedMemoryFile();
}

auto SharedMemoryFile::getName() const -> const std::string&
{
	return m_name;
}

auto SharedMemoryFile::getSize() const -> std::size_t
{
	return m_size;
}

auto SharedMemoryFile::read() const -> std::vector<std::byte>
{
	AtomicSpinLock lock {m_view.lock};

	if (*m_view.messageSize == 0)
	{
		return {};
	}

	std::vector<std::byte> data {};
	data.reserve(*m_view.messageSize);
	std::copy_n(std::begin(m_view.data), *m_view.messageSize, std::back_inserter(data));
	*m_view.messageSize = 0;
	return data;
}

void SharedMemoryFile::write(std::span<const std::byte> data)
{
	AtomicSpinLock lock {m_view.lock};
	*m_view.messageSize = data.size();
	std::copy(data.begin(), data.end(), std::begin(m_view.data));
}

void SharedMemoryFile::openSharedMemoryFile()
{
	const std::string& name {getName()};
	const uint32_t low_size {static_cast<uint32_t>(getSize() & 0xFFFFFFFF)};
	const uint32_t high_size {static_cast<std::size_t>((getSize() >> 32) & 0xFFFFFFFF)};
	m_handle = reinterpret_cast<std::uintptr_t>(CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, high_size, low_size, name.c_str()));

	if (reinterpret_cast<HANDLE>(m_handle) == nullptr)
	{
		throw std::runtime_error("Failed to create file mapping object");
	}

	m_buffer = static_cast<std::byte*>(MapViewOfFile(reinterpret_cast<HANDLE>(m_handle), FILE_MAP_ALL_ACCESS, 0, 0, getSize()));

	if (m_buffer == nullptr)
	{
		CloseHandle(reinterpret_cast<HANDLE>(m_handle));
		throw std::runtime_error("Failed to map view of file");
	}

	m_view.lock = reinterpret_cast<std::atomic_flag*>(m_buffer);
	m_view.messageSize = reinterpret_cast<std::size_t*>(m_buffer + sizeof(std::atomic_flag));
	m_view.data = {m_buffer + sizeof(std::atomic_flag) + sizeof(std::size_t), getSize() - sizeof(std::atomic_flag) - sizeof(std::size_t)};
}

void SharedMemoryFile::closeSharedMemoryFile()
{
	UnmapViewOfFile(m_buffer);
	CloseHandle(reinterpret_cast<HANDLE>(m_handle));
	m_handle = 0u;
}
