# MIT License
#
# Copyright (c) 2024 Josef de Joanelli (josef@pixelrift.io)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.28)

find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)

add_library(
  smipc STATIC
  "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/ring-buffer/dekkar-lock.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/ring-buffer/atomic-spin-lock.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/shared-memory/duplex-shared-memory-file.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/$<$<PLATFORM_ID:Windows>:shared-memory/windows-shared-memory-file.cpp>"
  "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/$<$<PLATFORM_ID:Linux>:shared-memory/posix-shared-memory-file.cpp>"
  # "${CMAKE_CURRENT_SOURCE_DIR}/shared_memory/client.cpp"
  # "${CMAKE_CURRENT_SOURCE_DIR}/shared_memory/server.cpp"
)

target_link_libraries(smipc PUBLIC fmt::fmt)

target_include_directories(smipc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_property(TARGET smipc PROPERTY CXX_STANDARD 23)

add_executable(serdes-unit-test
               "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/serdes/serdes.test.cpp")
target_link_libraries(serdes-unit-test PRIVATE smipc)
target_link_libraries(serdes-unit-test PRIVATE GTest::gtest)
set_property(TARGET serdes-unit-test PROPERTY CXX_STANDARD 23)
add_test(NAME serdes-unit-test COMMAND serdes-unit-test --gtest_color=1)

add_executable(
  ring-buffer-unit-test
  "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/ring-buffer/ring-buffer.test.cpp")
target_link_libraries(ring-buffer-unit-test PRIVATE smipc)
target_link_libraries(ring-buffer-unit-test PRIVATE GTest::gtest)
set_property(TARGET ring-buffer-unit-test PROPERTY CXX_STANDARD 23)
add_test(NAME ring-buffer-unit-test COMMAND ring-buffer-unit-test
                                            --gtest_color=1)

add_executable(
  serdes-benchmark
  "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/serdes/serdes.benchmark.cpp")

target_link_libraries(serdes-benchmark PRIVATE smipc)
target_link_libraries(serdes-benchmark PRIVATE benchmark::benchmark)

set_property(TARGET serdes-benchmark PROPERTY CXX_STANDARD 23)

add_test(NAME serdes-benchmark
         COMMAND serdes-benchmark --benchmark_out=results.json
                 --benchmark_out_format=json --benchmark_min_warmup_time=0.5)

add_executable(
  ring-buffer-benchmark
  "${CMAKE_CURRENT_SOURCE_DIR}/libsmipc/ring-buffer/ring-buffer.benchmark.cpp")

target_link_libraries(ring-buffer-benchmark PRIVATE smipc)
target_link_libraries(ring-buffer-benchmark PRIVATE benchmark::benchmark)

set_property(TARGET ring-buffer-benchmark PROPERTY CXX_STANDARD 23)

add_test(NAME ring-buffer-benchmark
         COMMAND ring-buffer-benchmark --benchmark_out=results.json
                 --benchmark_out_format=json --benchmark_min_warmup_time=0.5)
