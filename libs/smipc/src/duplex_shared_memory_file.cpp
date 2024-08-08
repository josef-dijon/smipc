#include "duplex_shared_memory_file.h"

#include "windows_shared_memory_file.h"

#include <string>
#include <stdexcept>

DuplexSharedMemoryFile::DuplexSharedMemoryFile(std::string_view rx_name, std::string_view tx_name, std::size_t size) {
    try {
        sm_rx_ = MakeSharedMemoryFile(rx_name, size);
    } catch ([[maybe_unused]] const std::runtime_error& e) {
        // handle exception
    }

    try {
        sm_tx_ = MakeSharedMemoryFile(tx_name, size);
    } catch ([[maybe_unused]] const std::runtime_error& e) {
        // handle exception
    }
}

DuplexSharedMemoryFile::~DuplexSharedMemoryFile() = default;

auto DuplexSharedMemoryFile::Read() -> std::vector<std::byte> {
    return sm_rx_->Read();
}

void DuplexSharedMemoryFile::Write(const std::vector<std::byte>& buffer) {
    sm_tx_->Write(buffer);
}

auto MakeDuplexSharedMemoryFile(std::string_view name, std::size_t size, bool is_client) -> std::unique_ptr<DuplexSharedMemoryFile> {
    std::string rx_name{name};
    std::string tx_name{name};

    if (is_client) {
        rx_name += ".rx";
        tx_name += ".tx";
    } else {
        rx_name += ".tx";
        tx_name += ".rx";
    }

    return std::make_unique<DuplexSharedMemoryFile>(rx_name, tx_name, size);
}
