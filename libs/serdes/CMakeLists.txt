# CMakeList.txt : CMake project for shared_memory_experiments, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.27)

project ("serdes")

# Add source to this project's executable.
add_executable(serdes src/main.cpp)

target_include_directories(serdes PRIVATE include)

set_property(TARGET serdes PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
