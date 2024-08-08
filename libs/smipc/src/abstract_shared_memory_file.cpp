#include "abstract_shared_memory_file.h"

ISharedMemoryFile::ISharedMemoryFile(std::string_view name, std::size_t size)
    : name_{name}
    , size_{size} {}

const std::string& ISharedMemoryFile::GetName() const {
    return name_;
}

std::size_t ISharedMemoryFile::GetSize() const {
    return size_;
}