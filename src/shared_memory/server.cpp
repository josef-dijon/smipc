#include "server.h"

#include "duplex_shared_memory_file.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

Server::Server()
	: m_sharedMemory {MakeDuplexSharedMemoryFile("Global//test", 1024, false)}
{}

Server::~Server() = default;

void Server::run()
{
	while (true)
	{
		const std::vector<std::byte> data = m_sharedMemory->read();

		if (data.empty())
		{
			Sleep(100);
			continue;
		}

		std::string_view str {reinterpret_cast<const char*>(data.data()), data.size()};

		const std::string_view resp {"ack"};
		std::vector<std::byte> resp_data {};
		resp_data.resize(resp.size());
		std::copy(resp.begin(), resp.end(), reinterpret_cast<char*>(resp_data.data()));
		m_sharedMemory->write(resp_data);

		std::cout << "Received: " << str << "\n";

		if (str == "exit" || str == "quit" || str == "q")
		{
			break;
		}
	}
}
