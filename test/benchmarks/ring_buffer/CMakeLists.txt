cmake_minimum_required(VERSION 3.27)

project("ring-buffer-benchmark")

find_package(benchmark CONFIG REQUIRED)

add_executable(ring-buffer-benchmark "ring_buffer_bench.cpp")

target_link_libraries(ring-buffer-benchmark PRIVATE smipc)
target_link_libraries(ring-buffer-benchmark PRIVATE benchmark::benchmark)

set_property(TARGET ring-buffer-benchmark PROPERTY CXX_STANDARD 23)

add_test(NAME ring-buffer-benchmark
         COMMAND ring-buffer-benchmark --benchmark_out=results.json
                 --benchmark_out_format=json --benchmark_min_warmup_time=0.5)
