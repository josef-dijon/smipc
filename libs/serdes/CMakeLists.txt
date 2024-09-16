# CMakeList.txt : CMake project for shared_memory_experiments, include source
# and define project specific logic here.
#
cmake_minimum_required(VERSION 3.27)

project("serdes")

# Add source to this project's executable.
add_library(serdes INTERFACE)

target_include_directories(serdes INTERFACE include)
target_include_directories(serdes INTERFACE src)

set_property(TARGET serdes PROPERTY CXX_STANDARD 23)

add_subdirectory(test)
