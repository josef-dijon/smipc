#pragma once

#include "abstract_shared_memory_file.h"

#include <cstdint>
#include <string_view>
#include <memory>
#include <vector>
#include <atomic>
#include <span>

struct AtomicMemoryView {
    std::atomic_flag* lock;
    std::span<std::byte> data;
    std::size_t* message_size;
};

struct AtomicSpinLock {
public:
    AtomicSpinLock(std::atomic_flag* flag) : lock_{flag} {
        while (lock_->test_and_set(std::memory_order_acquire)) {;}
    }

    ~AtomicSpinLock() {
        lock_->clear(std::memory_order_release);
    }
private:
    std::atomic_flag* lock_;
};

class WindowsSharedMemoryFile : public ISharedMemoryFile {
public:
    WindowsSharedMemoryFile(std::string_view name, std::size_t size);
    ~WindowsSharedMemoryFile();

    auto Read() const -> std::vector<std::byte> final;
    void Write(const std::vector<std::byte>& data) final;

private:
    void Open() final;
    void Close() final;

    std::uintptr_t handle_ {};
    std::byte* buffer_ {};
    AtomicMemoryView view_ {};
};

inline std::unique_ptr<ISharedMemoryFile> MakeSharedMemoryFile(std::string_view name, std::size_t size) {
    return std::make_unique<WindowsSharedMemoryFile>(name, size);
}
