#include "smipc/client.h"

#include "duplex_shared_memory_file.h"

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

Client::Client()
	: sm_ {MakeDuplexSharedMemoryFile("Global//test", 1024, true)}
{}

Client::~Client() = default;

void Client::Run()
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
		sm_->Write(data);

		while (true)
		{
			const std::vector<std::byte> resp_data = sm_->Read();

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
