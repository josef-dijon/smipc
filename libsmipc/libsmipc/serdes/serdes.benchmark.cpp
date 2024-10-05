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

#include <libsmipc/serdes/serdes.hpp>

#include <benchmark/benchmark.h>

#include <cstdint>
#include <string>
#include <vector>

enum class MyEnum : uint16_t
{
	Apple,
	Banana,
	Cherry,
	Durian,
	Elderberry,
};

struct MyStructSimple
{
	uint32_t a {42};
	float b {3.14159f};
	double c {2.71828};
	MyEnum d {MyEnum::Banana};
};

struct MyStructComplex
{
	uint32_t a {42};
	float b {3.14159f};
	MyEnum c {MyEnum::Cherry};
	std::vector<uint8_t> d {1, 2, 3, 4};
	std::string e {"hello world"};
	std::vector<uint16_t> f {0, 0, 0, 0, 5, 0, 0, 0, 0, 9, 0, 0, 0};
	std::vector<std::string> g {"fdsafda", "fdsafdsa", "rewqrew", "fdsjaklhfudsaihjkjhfdksajhkfd", "fjdskafhds7ahjkfjdsajk"};
};

SERIALISABLE(MyStructSimple)
{
	MEMBER(a);
	MEMBER(b);
	MEMBER(c);
	MEMBER(d);
};

SERIALISABLE(MyStructComplex)
{
	MEMBER(a);
	MEMBER(b);
	MEMBER(c);
	MEMBER(d);
	MEMBER(e);
	MEMBER(f);
	MEMBER(g);
}

static void BM_GetMinimumSizeSimple(benchmark::State& state)
{
	const SerialisableMyStructSimple x {};

	for (auto _ : state)
	{
		std::size_t size = x.getMinimumSize();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_GetActualSizeSimple(benchmark::State& state)
{
	const SerialisableMyStructSimple x {};

	for (auto _ : state)
	{
		std::size_t size = x.getSize();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_CheckDynamicismSimple(benchmark::State& state)
{
	const SerialisableMyStructSimple x {};

	for (auto _ : state)
	{
		bool size = x.isDynamic();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_SerialiseSimple(benchmark::State& state)
{
	const SerialisableMyStructSimple x {};

	for (auto _ : state)
	{
		auto data {x.serialise()};
		benchmark::DoNotOptimize(data);
	}
}

static void BM_SerialiseIntoSimple(benchmark::State& state)
{
	const SerialisableMyStructSimple x {};
	std::vector<std::byte> data(x.getSize());

	for (auto _ : state)
	{
		x.serialiseInto(data);
		benchmark::DoNotOptimize(data);
	}
}

static void BM_DeserialiseSimple(benchmark::State& state)
{
	const SerialisableMyStructSimple x {};
	const auto data {x.serialise()};

	for (auto _ : state)
	{
		SerialisableMyStructSimple s {};
		s.deserialise(data);
		benchmark::DoNotOptimize(s);
	}
}

static void BM_GetMinimumSizeComplex(benchmark::State& state)
{
	const SerialisableMyStructComplex x {};

	for (auto _ : state)
	{
		std::size_t size = x.getMinimumSize();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_GetActualSizeComplex(benchmark::State& state)
{
	const SerialisableMyStructComplex x {};

	for (auto _ : state)
	{
		std::size_t size = x.getSize();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_CheckDynamicismComplex(benchmark::State& state)
{
	const SerialisableMyStructComplex x {};

	for (auto _ : state)
	{
		bool size = x.isDynamic();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_SerialiseComplex(benchmark::State& state)
{
	const SerialisableMyStructComplex x {};

	for (auto _ : state)
	{
		auto data {x.serialise()};
		benchmark::DoNotOptimize(data);
	}
}

static void BM_SerialiseIntoComplex(benchmark::State& state)
{
	const SerialisableMyStructComplex x {};
	std::vector<std::byte> data(x.getSize());

	for (auto _ : state)
	{
		x.serialiseInto(data);
		benchmark::DoNotOptimize(data);
	}
}

static void BM_DeserialiseComplex(benchmark::State& state)
{
	const SerialisableMyStructComplex x {};
	const auto data {x.serialise()};

	for (auto _ : state)
	{
		SerialisableMyStructComplex s {};
		s.deserialise(data);
		benchmark::DoNotOptimize(s);
	}
}

BENCHMARK(BM_GetMinimumSizeSimple);
BENCHMARK(BM_GetActualSizeSimple);
BENCHMARK(BM_CheckDynamicismSimple);
BENCHMARK(BM_SerialiseSimple);
BENCHMARK(BM_SerialiseIntoSimple);
BENCHMARK(BM_DeserialiseSimple);
BENCHMARK(BM_GetMinimumSizeComplex);
BENCHMARK(BM_GetActualSizeComplex);
BENCHMARK(BM_CheckDynamicismComplex);
BENCHMARK(BM_SerialiseComplex);
BENCHMARK(BM_SerialiseIntoComplex);
BENCHMARK(BM_DeserialiseComplex);

BENCHMARK_MAIN();
