cmake_minimum_required(VERSION 3.27)

project("serdes-benchmarks")

find_package(benchmark CONFIG REQUIRED)

add_executable(serdes-benchmark "serdes_bench.cpp")

target_link_libraries(serdes-benchmark PRIVATE smipc)
target_link_libraries(serdes-benchmark PRIVATE benchmark::benchmark)

set_property(TARGET serdes-benchmark PROPERTY CXX_STANDARD 23)

add_test(NAME serdes-benchmark
         COMMAND serdes-benchmark --benchmark_out=results.json
                 --benchmark_out_format=json --benchmark_min_warmup_time=0.5)
