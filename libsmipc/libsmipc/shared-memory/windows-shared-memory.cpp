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

#include <libsmipc/shared-memory/windows-shared-memory.hpp>

#include <algorithm>
#include <format>
#include <iterator>
#include <stdexcept>
#include <windows.h>

void WindowsSharedMemory::create(const std::string& name, std::size_t size)
{
	if (exists())
	{
		open(name);
		return;
	}

	m_name = name;
	m_size = size;

	const uint32_t low_size {static_cast<uint32_t>(m_size & 0xFFFFFFFF)};
	const uint32_t high_size {static_cast<std::size_t>((m_size >> 32) & 0xFFFFFFFF)};
	m_handle = reinterpret_cast<std::uintptr_t>(CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, high_size, low_size, m_name.c_str()));

	if (reinterpret_cast<HANDLE>(m_handle) == nullptr)
	{
		throw std::runtime_error(std::format("Failed to create file mapping object. Error code: {}", GetLastError()));
	}

	m_buffer = static_cast<std::byte*>(MapViewOfFile(reinterpret_cast<HANDLE>(m_handle), FILE_MAP_ALL_ACCESS, 0, 0, m_size));

	if (m_buffer == nullptr)
	{
		const auto errorCode = GetLastError();
		CloseHandle(reinterpret_cast<HANDLE>(m_handle));
		throw std::runtime_error(std::format("Failed to map view of file. Error code: {}", errorCode));
	}

	m_view.lock = reinterpret_cast<std::atomic_flag*>(m_buffer);
	m_view.dataSize = *reinterpret_cast<std::size_t*>(m_buffer + sizeof(std::atomic_flag));
	m_view.data = m_buffer + sizeof(std::atomic_flag) + sizeof(std::size_t);
}

void WindowsSharedMemory::open(const std::string& name)
{
	if (! exists())
	{
		create(name, m_size);
		return;
	}

	m_handle = reinterpret_cast<std::uintptr_t>(OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name.data()));

	if (reinterpret_cast<HANDLE>(m_handle) == nullptr)
	{
		throw std::runtime_error(std::format("Failed to create file mapping object. Error code: {}", GetLastError()));
	}

	m_buffer = static_cast<std::byte*>(MapViewOfFile(reinterpret_cast<HANDLE>(m_handle), FILE_MAP_ALL_ACCESS, 0, 0, m_size));

	if (m_buffer == nullptr)
	{
		const auto errorCode = GetLastError();
		CloseHandle(reinterpret_cast<HANDLE>(m_handle));
		throw std::runtime_error(std::format("Failed to map view of file. Error code: {}", errorCode));
	}

	const auto dataSize = *reinterpret_cast<std::size_t*>(m_buffer + sizeof(std::atomic_flag));
	m_size = dataSize + sizeof(std::atomic_flag) + sizeof(std::size_t);

	m_view.lock = reinterpret_cast<std::atomic_flag*>(m_buffer);
	m_view.dataSize = dataSize;
	m_view.data = m_buffer + sizeof(std::atomic_flag) + sizeof(std::size_t);
}

void WindowsSharedMemory::close()
{
	UnmapViewOfFile(m_buffer);
	CloseHandle(reinterpret_cast<HANDLE>(m_handle));
	m_handle = 0u;
}

auto WindowsSharedMemory::getName() const -> std::string_view
{
	return m_name;
}

auto WindowsSharedMemory::getSize() const -> std::size_t
{
	return m_size;
}

auto WindowsSharedMemory::getView() -> AtomicMemoryView
{
	return m_view;
}

auto WindowsSharedMemory::exists() const -> bool
{
	return m_handle != 0;
}
