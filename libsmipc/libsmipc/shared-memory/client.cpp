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
