cmake_minimum_required(VERSION 3.27)

project("serdes-benchmarks")

find_package(benchmark CONFIG REQUIRED)

# Add source to this project's executable.
add_executable(serdes-benchmarks benchmark.cpp)

target_link_libraries(serdes-benchmarks PRIVATE serdes)
target_link_libraries(serdes-benchmarks PRIVATE benchmark::benchmark)

set_property(TARGET serdes-benchmarks PROPERTY CXX_STANDARD 20)

add_test(Benchmarks serdes-benchmarks)
