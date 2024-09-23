cmake_minimum_required(VERSION 3.27)

project("ring-buffer-benchmarks")

find_package(benchmark CONFIG REQUIRED)

add_executable(ring-buffer-benchmarks "benchmark.cpp")

target_link_libraries(ring-buffer-benchmarks PRIVATE ring-buffer)
target_link_libraries(ring-buffer-benchmarks PRIVATE benchmark::benchmark)

set_property(TARGET ring-buffer-benchmarks PROPERTY CXX_STANDARD 23)

add_test(NAME ring-buffer-benchmarks
         COMMAND ring-buffer-benchmarks --benchmark_out=results.json
                 --benchmark_out_format=json --benchmark_min_warmup_time=0.5)
