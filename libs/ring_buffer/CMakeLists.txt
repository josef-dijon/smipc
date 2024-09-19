cmake_minimum_required(VERSION 3.28)

project("ring-buffer")

add_executable(ring-buffer "test/main.cpp")

target_include_directories(ring-buffer PRIVATE "include/ring_buffer")

set_property(TARGET ring-buffer PROPERTY CXX_STANDARD 23)
