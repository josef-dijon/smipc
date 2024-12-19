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

#include <libsmipc/ring-buffer/ring-buffer-rx.hpp>
#include <libsmipc/ring-buffer/ring-buffer-tx.hpp>

#include <benchmark/benchmark.h>

static void BM_push_pop_1(benchmark::State& state)
{
	constexpr std::size_t kSize = 1024;
	uint8_t buffer[kSize] {};

	RingBufferRx<kSize> rx {buffer};
	RingBufferTx<kSize> tx {buffer};

	Packet packet {
		std::vector<uint8_t> {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
    };

	for (auto _ : state)
	{
		tx.push(packet);
		auto p1 = rx.pull();

		benchmark::DoNotOptimize(p1);
	}
}

static void BM_push_pop_4(benchmark::State& state)
{
	constexpr std::size_t kSize = 1024;
	uint8_t buffer[kSize] {};

	RingBufferRx<kSize> rx {buffer};
	RingBufferTx<kSize> tx {buffer};

	Packet packet {
		std::vector<uint8_t> {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
    };

	for (auto _ : state)
	{
		tx.push(packet);
		tx.push(packet);
		tx.push(packet);
		tx.push(packet);

		auto p1 = rx.pull();
		auto p2 = rx.pull();
		auto p3 = rx.pull();
		auto p4 = rx.pull();

		benchmark::DoNotOptimize(p1);
		benchmark::DoNotOptimize(p2);
		benchmark::DoNotOptimize(p3);
		benchmark::DoNotOptimize(p4);
	}
}

BENCHMARK(BM_push_pop_1);
BENCHMARK(BM_push_pop_4);

BENCHMARK_MAIN();
