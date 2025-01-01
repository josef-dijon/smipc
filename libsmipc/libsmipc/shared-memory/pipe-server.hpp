#ifndef PIPE_SERVER_HPP_
#define PIPE_SERVER_HPP_

#include <memory>
#include <unordered_map>

class PipeConnection;

class PipeServer
{
public:
	PipeServer();
	~PipeServer();

private:
	std::unordered_map<std::string, std::unique_ptr<PipeConnection>> m_connections;
};

#endif  // PIPE_SERVER_HPP_
