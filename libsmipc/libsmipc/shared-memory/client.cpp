#include "libsmipc/shared-memory/client.hpp"

#include <libsmipc/shared-memory/duplex-shared-memory-file.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

Client::Client()
	: m_sharedMemory {MakeDuplexSharedMemoryFile("Global//test", 1024, true)}
{}

Client::~Client() = default;

void Client::run()
{
	while (true)
	{
		std::string str;
		std::getline(std::cin, str);

		if (str.empty())
		{
			continue;
		}

		std::vector<std::byte> data {};
		data.resize(str.size());
		std::copy(str.begin(), str.end(), reinterpret_cast<char*>(data.data()));
		m_sharedMemory->write(data);

		while (true)
		{
			const auto resp_data = m_sharedMemory->read();

			if (! resp_data.empty())
			{
				std::string_view resp {reinterpret_cast<const char*>(resp_data.data()), resp_data.size()};
				std::cout << "Received: " << resp << "\n";
				break;
			}

			Sleep(100);
		}

		if (str == "exit" || str == "quit" || str == "q")
		{
			break;
		}
	}
}
