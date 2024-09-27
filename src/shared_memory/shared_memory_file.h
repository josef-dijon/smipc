#ifndef SHARED_MEMORY_FILE_H_
#define SHARED_MEMORY_FILE_H_

#include "atomic_memory_view.h"

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

class SharedMemoryFile
{
public:
	SharedMemoryFile(std::string_view name, std::size_t size);
	~SharedMemoryFile();

	auto getName() const -> const std::string&;
	auto getSize() const -> std::size_t;

	auto read() const -> std::vector<std::byte>;
	void write(std::span<const std::byte> data);

private:
	void openSharedMemoryFile();
	void closeSharedMemoryFile();

	std::string m_name {};
	std::size_t m_size {};

	std::uintptr_t m_handle {};
	std::byte* m_buffer {};
	AtomicMemoryView m_view {};
};

auto MakeSharedMemoryFile(std::string_view name, std::size_t size) -> std::unique_ptr<SharedMemoryFile>;

#endif  // SHARED_MEMORY_FILE_H_
