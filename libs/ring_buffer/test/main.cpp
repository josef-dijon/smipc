#include "ring_buffer.h"
#include "ring_buffer_rx.h"
#include "ring_buffer_tx.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <stack>
#include <string>
#include <thread>
#include <vector>

static void print_vector(const std::vector<uint8_t>& data)
{
	for (const auto& byte : data)
	{
		std::cout << static_cast<int>(byte) << ' ';
	}

	std::cout << '\n';
}

static std::vector<uint8_t> string_to_vector(const std::string& str)
{
	std::vector<uint8_t> vec(str.cbegin(), str.cend());
	return vec;
}

static std::string vector_to_string(const std::vector<uint8_t>& vec)
{
	return std::string(vec.cbegin(), vec.cend());
}

int main()
{
	constexpr std::size_t page_size {4096u};
	constexpr std::size_t size {page_size * 256u};
	// constexpr std::size_t size {256u};
	auto memory {std::make_unique<uint8_t[]>(size)};
	RingBufferRx<size, 4u> rx {memory.get()};
	RingBufferTx<size, 4u> tx {memory.get()};

	std::cout << "Size of ring buffer: " << sizeof(RingBuffer<size, 4>) << '\n';

	std::stack<std::string> stack {};
	std::mutex lock {};

	const auto rx_worker = [&]
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(80));

			if (rx.is_empty())
			{
				continue;
			}

			const auto str = vector_to_string(rx.pop());

			if (str.empty())
			{
				continue;
			}

			{
				std::string message {};
				message += "                                                   Rx: ";
				message += str;
				message += '\n';
				std::lock_guard lg {lock};
				stack.push(message);
			}

			if (str == "stop")
			{
				break;
			}
		}
	};

	const auto tx_worker = [&]
	{
		constexpr std::size_t count {1000u};
		for (int i = 0; i <= count; ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			const std::string msg = "This is a message! The number is : " + std::to_string(i);
			{
				std::string message {};
				message += "Tx: ";
				message += msg;
				message += '\n';
				std::lock_guard lg {lock};
				stack.push(message);
			}

			tx.push(string_to_vector(msg));

			if (i == count)
			{
				tx.push(string_to_vector("stop"));
			}
		}
	};

	const auto printer = [&]
	{
		uint32_t count {0};

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			if (count > 50)
			{
				break;
			}

			std::lock_guard lg {lock};

			if (stack.empty())
			{
				count++;
				continue;
			}
			else
			{
				count = 0;
				std::cout << stack.top();
				stack.pop();
			}
		}
	};

	std::cout << "                  TX                                              RX\n";
	std::thread rx_thread(rx_worker);
	std::thread tx_thread(tx_worker);
	std::thread print_thread(printer);

	rx_thread.join();
	tx_thread.join();
	print_thread.join();

	// tx.push(string_to_vector("0123456789abcdefghijklmnopqrstuvwxyz"));
	// tx.push(string_to_vector("0123456789abcdefghijklmnopqrstuvwxyz"));
	// rx.pop();
	// tx.push(string_to_vector("0123456789abcdefghijklmnopqrstuvwxyz"));
	// rx.pop();
	// tx.push(string_to_vector("0123456789abcdefghijklmnopqrstuvwxyz"));
	// rx.pop();
	// tx.push(string_to_vector("0123456789abcdefghijklmnopqrstuvwxyz"));
	// rx.pop();
	// tx.push(string_to_vector("0123456789abcdefghijklmnopqrstuvwxyz"));

	return 0;
}
