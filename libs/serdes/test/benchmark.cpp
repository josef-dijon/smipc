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
};

SERIALISABLE(MyStruct)
{
	MEMBER(a);
	MEMBER(b);
	MEMBER(c);
	MEMBER(d);
	MEMBER(e);
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

BENCHMARK(BM_GetMinimumSize);
BENCHMARK(BM_GetActualSize);
BENCHMARK(BM_CheckDynamicism);

BENCHMARK_MAIN();
