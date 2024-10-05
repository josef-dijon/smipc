/* MIT License
 * 
 * Copyright (c) 2024 Josef de Joanelli (josef@pixelrift.io)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "libsmipc/shared-memory/server.hpp"

#include <libsmipc/shared-memory/duplex_shared_memory_file.hpp>

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
