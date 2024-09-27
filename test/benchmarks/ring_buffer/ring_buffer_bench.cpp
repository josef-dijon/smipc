#include "benchmark/benchmark.h"
#include "ring_buffer/ring_buffer_rx.h"
#include "ring_buffer/ring_buffer_tx.h"

static void BM_DeserialiseComplex(benchmark::State& state)
{
	for (auto _ : state)
	{
		;
	}
}

BENCHMARK(BM_DeserialiseComplex);

BENCHMARK_MAIN();
