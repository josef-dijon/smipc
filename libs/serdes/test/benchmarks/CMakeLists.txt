cmake_minimum_required(VERSION 3.27)

project("serdes-benchmarks")

find_package(benchmark CONFIG REQUIRED)

# Add source to this project's executable.
add_executable(serdes-benchmarks benchmark.cpp)

target_link_libraries(serdes-benchmarks PRIVATE serdes)
target_link_libraries(serdes-benchmarks PRIVATE benchmark::benchmark)

set_property(TARGET serdes-benchmarks PROPERTY CXX_STANDARD 23)

add_test(NAME serdes-benchmarks
         COMMAND serdes-benchmarks --benchmark_out=results.json
                 --benchmark_out_format=json --benchmark_min_warmup_time=0.5)
