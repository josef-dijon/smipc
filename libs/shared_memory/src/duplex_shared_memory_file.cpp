#include "duplex_shared_memory_file.h"

#include "shared_memory_file.h"

#include <stdexcept>
#include <string>

DuplexSharedMemoryFile::DuplexSharedMemoryFile(std::string_view rxName, std::string_view txName, std::size_t size)
{
	try
	{
		m_sharedMemoryFileRx = MakeSharedMemoryFile(rxName, size);
	}
	catch ([[maybe_unused]] const std::runtime_error& e)
	{
		// handle exception
	}

	try
	{
		m_sharedMemoryFileTx = MakeSharedMemoryFile(txName, size);
	}
	catch ([[maybe_unused]] const std::runtime_error& e)
	{
		// handle exception
	}
}

DuplexSharedMemoryFile::~DuplexSharedMemoryFile() = default;

auto DuplexSharedMemoryFile::read() -> std::span<const std::byte>
{
	return m_sharedMemoryFileRx->read();
}

void DuplexSharedMemoryFile::write(std::span<const std::byte> data)
{
	m_sharedMemoryFileTx->write(data);
}

auto MakeDuplexSharedMemoryFile(std::string_view name, std::size_t size, bool swapRxTx) -> std::unique_ptr<DuplexSharedMemoryFile>
{
	const std::string rxName = swapRxTx ? std::string {name} + ".tx" : std::string {name} + ".rx";
	const std::string txName = swapRxTx ? std::string {name} + ".rx" : std::string {name} + ".tx";

	return std::make_unique<DuplexSharedMemoryFile>(rxName, txName, size);
}
