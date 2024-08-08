#pragma once

#include <cstdint>
#include <string_view>
#include <memory>
#include <vector>

class ISharedMemoryFile;

class DuplexSharedMemoryFile {
public:
    DuplexSharedMemoryFile(std::string_view rx_name, std::string_view tx_name, std::size_t size);
    ~DuplexSharedMemoryFile();

    auto Read() -> std::vector<std::byte>;
    void Write(const std::vector<std::byte>& buffer);

private:
    std::unique_ptr<ISharedMemoryFile> sm_rx_ {};
    std::unique_ptr<ISharedMemoryFile> sm_tx_ {};
};

auto MakeDuplexSharedMemoryFile(std::string_view name, std::size_t size, bool is_client) -> std::unique_ptr<DuplexSharedMemoryFile>;
