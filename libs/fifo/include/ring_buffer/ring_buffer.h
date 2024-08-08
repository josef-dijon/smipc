#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "atomic_spin_lock.h"
#include "dekkar_lock.h"

#include <array>
#include <atomic>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <stdexcept>
#include <vector>

static constexpr bool IsDebugBuild() {
    #ifdef _DEBUG
        return true;
    #else
        return false;
    #endif
}

template <std::size_t S, std::size_t A>
struct RingBuffer {
    static constexpr std::size_t AlignedSize(std::size_t size) {
        return size + ((A - (size % A)) % A);
    }

    static constexpr std::size_t BufferSize() {
        return AlignedSize(S) - 4 * sizeof(uint32_t) - 2 * sizeof(std::atomic_flag);
    }

    static_assert(S > BufferSize(), "RingBuffer S template parameter is too small");
    static_assert(S <= 2048u * 1024u * 1024u, "RingBuffer S template parameter is too large, must be less than 2GB");
    static_assert(A >= 4, "RingBuffer A template parameter is too small, must be >= 4");
    static_assert((A & (A - 1)) == 0, "RingBuffer A template parameter is not power of 2");
    static_assert((S & (S - 1)) == 0, "RingBuffer S template parameter is not power of 2");

    struct MessageHeader {
        MessageHeader(std::size_t size_): size{static_cast<uint32_t>(size_)} {}
        AtomicSpinLock lock {};
        uint32_t size {0u};
    };

    std::atomic_bool rx_waiting {false};
    std::atomic_bool tx_waiting {false};

    uint32_t front {0u};
    uint32_t next {0u};
    uint32_t free_space {static_cast<uint32_t>(BufferSize())};
    uint32_t message_count {0u};

    std::array<uint8_t, BufferSize()> buffer {};
};

template <std::size_t S, std::size_t A>
class RingBufferRx {
public:
    RingBufferRx(uint8_t* memory)
        : ring_buffer_{new (memory) RingBuffer<S, A>}
    {}

    [[nodiscard]]
    auto is_empty() const noexcept -> bool {
        std::lock_guard lock(lock_);
        return ring_buffer_->free_space == ring_buffer_->buffer.size();
    }

    [[nodiscard]]
    auto get_message_count() const noexcept -> std::size_t {
        std::lock_guard lock(lock_);
        return ring_buffer_->message_count;
    }

    [[nodiscard]]
    auto pop() noexcept -> std::vector<uint8_t> {
        constexpr std::size_t header_size {RingBuffer<S, A>::AlignedSize(sizeof(RingBuffer<S, A>::MessageHeader))};

        if (is_empty()) {
            return {};
        }

        std::lock_guard lock(lock_);
        auto* message_header {reinterpret_cast<RingBuffer<S, A>::MessageHeader*>(ring_buffer_->buffer.data() + ring_buffer_->front)};
        std::unique_lock message_lock(message_header->lock);
        const std::size_t size {message_header->size};
        const std::size_t aligned_size {RingBuffer<S, A>::AlignedSize(size)};
        std::size_t tmp_front = ring_buffer_->front;
        std::size_t tmp_free_space = ring_buffer_->free_space;

        std::vector<uint8_t> data {};
        data.reserve(size);

        if (tmp_front + header_size < ring_buffer_->buffer.size()) {
            tmp_free_space += header_size;
            tmp_front += header_size;
        } else {
            tmp_free_space += ring_buffer_->buffer.size() - tmp_front + header_size;
            tmp_front = 0u;
        }

        if (tmp_front + size <= ring_buffer_->buffer.size()) {
            std::copy_n(ring_buffer_->buffer.cbegin() + tmp_front, size, std::back_inserter(data));

            if constexpr (IsDebugBuild()) {
                std::fill_n(ring_buffer_->buffer.begin() + tmp_front, size, 0u);
            }

            if (tmp_front + size == ring_buffer_->buffer.size()) {
                tmp_front = 0u;
                tmp_free_space += size;
            } else {
                tmp_front += aligned_size;
                tmp_free_space += aligned_size;
            }
        } else {
            const std::size_t part1_size = ring_buffer_->buffer.size() - tmp_front;
            std::copy_n(ring_buffer_->buffer.cbegin() + tmp_front, part1_size, std::back_inserter(data));
            std::copy_n(ring_buffer_->buffer.cbegin(), size - part1_size, std::back_inserter(data));

            if constexpr (IsDebugBuild()) {
                std::fill_n(ring_buffer_->buffer.begin() + tmp_front, part1_size, 0u);
                std::fill_n(ring_buffer_->buffer.begin(), size - part1_size, 0u);
            }

            tmp_free_space += part1_size;
            tmp_front = RingBuffer<S, A>::AlignedSize(size - part1_size);
            tmp_free_space += RingBuffer<S, A>::AlignedSize(size - part1_size);
        }

        ring_buffer_->front = static_cast<uint32_t>(tmp_front);
        ring_buffer_->free_space = static_cast<uint32_t>(tmp_free_space);
        message_lock.unlock();

        if (ring_buffer_->free_space == ring_buffer_->buffer.size()) {
            ring_buffer_->front = 0u;
            ring_buffer_->next = 0u;
        }

        --ring_buffer_->message_count;

        if constexpr (IsDebugBuild()) {
            message_header->size = 0;
        }

        return data;
    }

private: 
    RingBuffer<S, A>* ring_buffer_;
    mutable DekkarLock lock_ {ring_buffer_->rx_waiting, ring_buffer_->tx_waiting};
};

template <std::size_t S, std::size_t A>
class RingBufferTx {
public:
    RingBufferTx(uint8_t* memory)
        : ring_buffer_{new (memory) RingBuffer<S, A>}
    {}

    [[nodiscard]]
    auto is_full() const noexcept -> bool {
        std::lock_guard lock(lock_);
        return ring_buffer_->free_space <= 0u;
    }

    void push(const std::vector<uint8_t>& data) {
        push(data.data(), data.size());
    }

    void push(const uint8_t* data, std::size_t size) {
        if (size == 0) {
            return;
        }

        if (!data) {
            throw std::invalid_argument("Data is nullptr");
        }

        constexpr std::size_t header_size {RingBuffer<S, A>::AlignedSize(sizeof(RingBuffer<S, A>::MessageHeader))};
        const std::size_t aligned_size {RingBuffer<S, A>::AlignedSize(size)};

        std::unique_lock lock(lock_);

        const std::size_t tmp_first = ring_buffer_->front;
        std::size_t tmp_next = ring_buffer_->next;
        std::size_t tmp_free_space = ring_buffer_->free_space;

        if (header_size + size > ring_buffer_->free_space) {
            throw std::overflow_error("Buffer overflow");
        }

        if (tmp_first <= tmp_next) {
            if (tmp_next + header_size > ring_buffer_->buffer.size()) {
                tmp_free_space -= tmp_next + header_size - ring_buffer_->buffer.size();
                tmp_next = 0u;
            } else if (tmp_next + header_size == ring_buffer_->buffer.size()) {
                tmp_next = 0u;
            }
        }

        if (header_size > tmp_free_space) {
            throw std::overflow_error("Buffer overflow");
        }

        const std::size_t header_start{tmp_next};
        const std::size_t header_end{header_start + header_size};

        tmp_next += header_size;
        tmp_free_space -= header_size;

        if (aligned_size > tmp_free_space) {
            throw std::overflow_error("Buffer overflow");
        }

        auto* message_header {new (ring_buffer_->buffer.data() + header_start) RingBuffer<S, A>::MessageHeader{size}};
        std::lock_guard message_lock(message_header->lock);

        if (tmp_next + aligned_size < ring_buffer_->buffer.size()) {
            ring_buffer_->next = static_cast<uint32_t>(tmp_next + aligned_size);
        } else if (tmp_next + size > ring_buffer_->buffer.size()) {
            ring_buffer_->next = static_cast<uint32_t>(tmp_next + aligned_size - ring_buffer_->buffer.size());
        } else {
            ring_buffer_->next = 0u;
        }

        ring_buffer_->free_space = static_cast<uint32_t>(tmp_free_space - aligned_size);

        lock.unlock();

        if (tmp_next + size <= ring_buffer_->buffer.size()) {
            std::copy(data, data + size, ring_buffer_->buffer.begin() + tmp_next);
        } else {
            const std::size_t part1_size = ring_buffer_->buffer.size() - tmp_next;
            std::copy(data, data + part1_size, ring_buffer_->buffer.begin() + tmp_next);
            std::copy(data + part1_size, data + size, ring_buffer_->buffer.begin());
        }

        ++ring_buffer_->message_count;
    }

private: 
    RingBuffer<S, A>* ring_buffer_;
    mutable DekkarLock lock_ {ring_buffer_->tx_waiting, ring_buffer_->rx_waiting};
};

#endif  // RING_BUFFER_H_