#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

class ISharedMemoryFile {
public:
    ISharedMemoryFile(std::string_view name, std::size_t size);
    virtual ~ISharedMemoryFile() = default;

    const std::string& GetName() const;
    std::size_t GetSize() const;

    virtual std::vector<std::byte> Read() const = 0;
    virtual void Write(const std::vector<std::byte>& buffer) = 0;

protected:
    virtual void Open() = 0;
    virtual void Close() = 0;

private:
    std::string name_ {};
    std::size_t size_ {};
};
