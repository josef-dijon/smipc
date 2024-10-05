#include <libsmipc/ring-buffer/ring-buffer-rx.hpp>
#include <libsmipc/ring-buffer/ring-buffer-tx.hpp>

#include <benchmark/benchmark.h>

static void BM_DeserialiseComplex(benchmark::State& state)
{
	for (auto _ : state)
	{
		;
	}
}

BENCHMARK(BM_DeserialiseComplex);

BENCHMARK_MAIN();
