#include "serdes/serdes.h"

#include <benchmark/benchmark.h>
#include <cstdint>
#include <string>
#include <vector>

enum class MyEnum : uint16_t
{
};

struct MyStruct
{
	uint32_t             a {};               // 4, 4
	float                b {};               // 4, 4
	MyEnum               c {};               // 2, 2
	std::vector<uint8_t> d {1, 2, 3, 4};     // 4, 12
	std::string          e {"hello world"};  // 4, 19
											 // 18, 41
											 //
	std::vector<uint16_t>    f {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::vector<std::string> g {"fdsafda", "fdsafdsa", "rewqrew", "fdsjaklhfudsaihjkjhfdksajhkfd", "fjdskafhds7ahjkfjdsajk"};
};

SERIALISABLE(MyStruct)
{
	MEMBER(a);
	MEMBER(b);
	MEMBER(c);
	MEMBER(d);
	MEMBER(e);
	MEMBER(f);
	MEMBER(g);
}

static void BM_GetMinimumSize(benchmark::State& state)
{
	const SerialisableMyStruct x {};

	for (auto _ : state)
	{
		std::size_t size = x.getMinimumSize();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_GetActualSize(benchmark::State& state)
{
	const SerialisableMyStruct x {};

	for (auto _ : state)
	{
		std::size_t size = x.getActualSize();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_CheckDynamicism(benchmark::State& state)
{
	const SerialisableMyStruct x {};

	for (auto _ : state)
	{
		bool size = x.isDynamic();
		benchmark::DoNotOptimize(size);
	}
}

static void BM_Serialise(benchmark::State& state)
{
	const SerialisableMyStruct x {};

	for (auto _ : state)
	{
		auto data {x.serialise()};
		benchmark::DoNotOptimize(data);
	}
}

static void BM_SerialiseInto(benchmark::State& state)
{
	const SerialisableMyStruct x {};
	std::vector<std::byte>     data(x.getActualSize());

	for (auto _ : state)
	{
		x.serialiseInto(data);
		benchmark::DoNotOptimize(data);
	}
}

static void BM_Deserialise(benchmark::State& state)
{
	const SerialisableMyStruct x {};
	const auto                 data {x.serialise()};

	for (auto _ : state)
	{
		SerialisableMyStruct s {};
		s.deserialise(data);
		benchmark::DoNotOptimize(s);
	}
}

BENCHMARK(BM_GetMinimumSize);
BENCHMARK(BM_GetActualSize);
BENCHMARK(BM_CheckDynamicism);
BENCHMARK(BM_Serialise);
BENCHMARK(BM_SerialiseInto);
BENCHMARK(BM_Deserialise);

BENCHMARK_MAIN();
