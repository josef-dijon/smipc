#include "windows_shared_memory_file.h"

#include <windows.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <iostream>

WindowsSharedMemoryFile::WindowsSharedMemoryFile(std::string_view name, std::size_t size) 
    : ISharedMemoryFile(name, size) {
    Open();
}

WindowsSharedMemoryFile::~WindowsSharedMemoryFile() {
    Close();
}

auto WindowsSharedMemoryFile::Read() const -> std::vector<std::byte> {
    //std::cout << "Reading from shared memory\n";
    AtomicSpinLock lock{view_.lock};

    if (*view_.message_size == 0) {
        return {};
    }

    std::vector<std::byte> data{};
    data.reserve(*view_.message_size);
    std::copy_n(std::begin(view_.data), *view_.message_size, std::back_inserter(data));
    *view_.message_size = 0;
    return data;
}

void WindowsSharedMemoryFile::Write(const std::vector<std::byte>& data) {
    //std::cout << "Writing to shared memory\n";
    AtomicSpinLock lock{view_.lock};
    *view_.message_size = data.size();
    std::copy(data.begin(), data.end(), std::begin(view_.data));
}

void WindowsSharedMemoryFile::Open() {
    std::cout << "Opening shared memory file: " << GetName() << "\n";
    const std::string& name{GetName()};
    const uint32_t low_size{static_cast<uint32_t>(GetSize() & 0xFFFFFFFF)};
    const uint32_t high_size{static_cast<std::size_t>((GetSize() >> 32) & 0xFFFFFFFF)};
    handle_ = reinterpret_cast<std::uintptr_t>(CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, high_size, low_size, name.c_str()));

    if (reinterpret_cast<HANDLE>(handle_) == nullptr) {
        throw std::runtime_error("Failed to create file mapping object");
    }

    buffer_ = static_cast<std::byte*>(MapViewOfFile(reinterpret_cast<HANDLE>(handle_), FILE_MAP_ALL_ACCESS, 0, 0, GetSize()));

    if (buffer_ == nullptr) {
        CloseHandle(reinterpret_cast<HANDLE>(handle_));
        throw std::runtime_error("Failed to map view of file");
    }

    view_.lock = reinterpret_cast<std::atomic_flag*>(buffer_);
    view_.message_size = reinterpret_cast<std::size_t*>(buffer_ + sizeof(std::atomic_flag));
    view_.data = {buffer_ + sizeof(std::atomic_flag) + sizeof(std::size_t), GetSize() - sizeof(std::atomic_flag) - sizeof(std::size_t)};
}

void WindowsSharedMemoryFile::Close() {
    std::cout << "Closing shared memory file: " << GetName() << "\n";
    UnmapViewOfFile(buffer_);
    CloseHandle(reinterpret_cast<HANDLE>(handle_));
    handle_ = 0;
}
