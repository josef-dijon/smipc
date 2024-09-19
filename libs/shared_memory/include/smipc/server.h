#ifndef SERVER_H_
#define SERVER_H_

#include <memory>

class DuplexSharedMemoryFile;

class Server
{
public:
	Server();
	~Server();

	void run();

private:
	std::unique_ptr<DuplexSharedMemoryFile> m_sharedMemory {};
};

#endif  // SERVER_H_
