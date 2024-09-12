# CMakeList.txt : CMake project for shared_memory_experiments, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.27)

project ("serdes-test")

find_package(GTest CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)

# Add source to this project's executable.
add_executable(unit-test unit_test.cpp)
add_executable(benchmark-test benchmark.cpp)

target_include_directories(unit-test PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(unit-test PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_include_directories(benchmark-test PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(benchmark-test PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(unit-test PRIVATE GTest::gtest)
target_link_libraries(benchmark-test PRIVATE benchmark::benchmark)

set_property(TARGET unit-test PROPERTY CXX_STANDARD 20)
set_property(TARGET benchmark-test PROPERTY CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_test(UnitTests unit-test)
add_test(Benchmarks benchmark-test)

# TODO: Add tests and install targets if needed.
