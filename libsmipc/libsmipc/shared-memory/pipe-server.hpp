#ifndef PIPE_SERVER_HPP_
#define PIPE_SERVER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

class PipeConnection;

class PipeServer
{
public:
	PipeServer();
	~PipeServer();

private:
	std::string m_name;

	std::unique_ptr<SharedMemoryPipe<4096u>> m_serverPipe;
	std::unordered_map<std::string, std::unique_ptr<PipeConnection>> m_clientConnections;
};

#endif  // PIPE_SERVER_HPP_