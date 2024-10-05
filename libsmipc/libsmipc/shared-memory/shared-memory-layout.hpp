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

#ifndef SHARED_MEMORY_LAYOUT_H_
#define SHARED_MEMORY_LAYOUT_H_

#include <atomic>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

class AtomicSpinLock
{
public:
	AtomicSpinLock(std::atomic_flag& lock)
		: lock_ {lock}
	{
		while (lock_.test_and_set(std::memory_order_acquire))
		{
			;
		}
	}

	~AtomicSpinLock()
	{
		release();
	}

	void release()
	{
		lock_.clear(std::memory_order_release);
	}

private:
	std::atomic_flag& lock_;
};

struct MessageHeader
{
	std::atomic_flag lock {};
	uint32_t message_size {};
};

struct SharedMemoryHeader
{
	std::atomic_flag lock_;
	uint32_t first_message_offset_;
	uint32_t next_free_offset_;
	uint32_t message_count_;
	uint32_t free_space_;
};

template <std::size_t N>
class SharedMemory
{
public:
	void Lock()
	{
		while (header_.lock_.test_and_set(std::memory_order_acquire))
		{
			;
		}
	}

	void Unlock()
	{
		header_.lock_.clear(std::memory_order_release);
	}

protected:
	SharedMemoryHeader header_ {};
	std::array<std::byte, N - sizeof(SharedMemoryHeader)> data_ {};
};

template <std::size_t N>
class SharedMemoryReader: private SharedMemory<N>
{
public:
	std::vector<std::byte> Read()
	{
		std::vector<std::byte> data {};
		return data;
	}
};

template <std::size_t N>
class SharedMemoryWriter: private SharedMemory<N>
{
public:
	void Write(std::span<const std::byte> data)
	{
		const std::size_t message_size {sizeof(MessageHeader) + data.size()};
		const std::size_t padding_size {4u - message_size % 4u};
		const std::size_t aligned_message_size {message_size + padding_size};

		if (data.empty())
		{
			return;
		}

		Lock();

		if (aligned_message_size > header_.free_space_)
		{
			Unlock();
			throw std::runtime_error("Data size exceeds buffer size");
		}

		header_.next_free_offset_ += aligned_message_size;
		++header_.message_count_;
		header_.free_space_ -= aligned_message_size;

		Unlock();

		if (message_size > GetFreeSpaceBack())
		{
			const std::size_t back_size {message_buffer_.size() - next_free_offset_};
			const std::size_t front_size {data_size - back_size};
			const std::span<const std::byte> back_src_data {data.data(), back_size};
			const std::span<const std::byte> front_src_data {data.data() + back_size, front_size};
			std::span<std::byte> back_dst_data {message_buffer_.data() + next_free_offset_, back_size};
			std::span<std::byte> front_dst_data {message_buffer_.data(), front_size};
			std::copy(std::cbegin(back_src_data), std::cend(back_src_data), std::begin(back_dst_data));
			std::copy(std::cbegin(front_src_data), std::cend(front_src_data), std::begin(front_dst_data));
		}
		else
		{
			const std::size_t size {data.size()};
			const std::span<const std::byte> src_data {data.data(), size};
			std::span<std::byte> dst_data {message_buffer_.data() + next_free_offset_, size};
			std::copy(std::cbegin(src_data), std::cend(src_data), std::begin(dst_data));
		}
	}
};

template <std::size_t N>
class SharedMemoryLayout
{
public:
	std::size_t GetFirstMessageOffset() const
	{
		return 0;
	}

	std::size_t GetNextMessageOffset() const
	{
		return 0;
	}

	void WriteMessage(std::span<const std::byte> data)
	{
		const std::size_t data_size {sizeof(MessageHeader) + data.size()};

		if (data.empty())
		{
			return;
		}

		if (data_size > free_space_)
		{
			throw std::runtime_error("Data size exceeds buffer size");
		}

		AtomicSpinLock buffer_lock {lock_};

		// If the header doesn't fit in the remaining space, wrap around. Don't split the header.
		if (sizeof(MessageHeader) > message_buffer_.size() - next_free_offset_)
		{
			free_space_ -= message_buffer_.size() - next_free_offset_;
			next_free_offset_ = 0;
		}

		std::fill_n(message_buffer_.begin() + next_free_offset_, sizeof(MessageHeader), std::byte {0});
		MessageHeader* header = reinterpret_cast<MessageHeader*>(message_buffer_.data() + next_free_offset_);
		header->message_size = data_size;
		AtomicSpinLock message_lock header->lock;

		next_free_offset_ += data_size;
		++message_count;
		free_space_ -= data_size;

		buffer_lock.release();

		if (data.size() > message_buffer_.size() - next_free_offset_)
		{
			const std::size_t back_size {message_buffer_.size() - next_free_offset_};
			const std::size_t front_size {data_size - back_size};
			const std::span<const std::byte> back_src_data {data.data(), back_size};
			const std::span<const std::byte> front_src_data {data.data() + back_size, front_size};
			std::span<std::byte> back_dst_data {message_buffer_.data() + next_free_offset_, back_size};
			std::span<std::byte> front_dst_data {message_buffer_.data(), front_size};
			std::copy(std::cbegin(back_src_data), std::cend(back_src_data), std::begin(back_dst_data));
			std::copy(std::cbegin(front_src_data), std::cend(front_src_data), std::begin(front_dst_data));
		}
		else if (data.size() <= message_buffer_.size() - next_free_offset_)
		{
			const std::size_t size {data.size()};
			const std::span<const std::byte> src_data {data.data(), size};
			std::span<std::byte> dst_data {message_buffer_.data() + next_free_offset_, size};
			std::copy(std::cbegin(src_data), std::cend(src_data), std::begin(dst_data));
		}
	}

	std::optional<std::vector<std::byte>> TryReadMessage()
	{
		std::vector<std::byte> data {};

		AtomicSpinLock buffer_lock {lock_};

		if (message_count_ == 0)
		{
			return {};
		}
	}

private:
	std::atomic_flag lock_;
	uint32_t first_message_offset_;
	uint32_t next_free_offset_;
	uint32_t message_count_;
	uint32_t free_space_;

	std::array<std::byte, GetArraySize(N)> message_buffer_;
};

#endif  // SHARED_MEMORY_LAYOUT_H_
