#ifndef DUPLEX_SHARED_MEMORY_FILE_H_
#define DUPLEX_SHARED_MEMORY_FILE_H_

#include <memory>
#include <span>
#include <string_view>

class SharedMemoryFile;

class DuplexSharedMemoryFile
{
public:
	DuplexSharedMemoryFile(std::string_view rxName, std::string_view txName, std::size_t size);
	~DuplexSharedMemoryFile();

	auto read() -> std::span<const std::byte>;
	void write(std::span<const std::byte> data);

private:
	std::unique_ptr<SharedMemoryFile> m_sharedMemoryFileRx {};
	std::unique_ptr<SharedMemoryFile> m_sharedMemoryFileTx {};
};

auto MakeDuplexSharedMemoryFile(std::string_view name, std::size_t size, bool is_client) -> std::unique_ptr<DuplexSharedMemoryFile>;

#endif  // DUPLEX_SHARED_MEMORY_FILE_H_
