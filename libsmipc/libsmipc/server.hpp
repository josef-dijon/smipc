#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <libsmipc/client-channel.hpp>
#include <>

#include <memory>
#include <string>
#include <unordered_map>

class Server
{
public:
	Server();
	~Server();

private:
	std::string m_name;
	std::unique_ptr<SharedMemoryPipe<4096u>> m_connectionBrokerPipe;
	std::unordered_map<std::string, std::unique_ptr<Channel>> m_clientChannels;
};

#endif  // SERVER_HPP_