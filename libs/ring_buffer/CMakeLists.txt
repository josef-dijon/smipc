cmake_minimum_required(VERSION 3.28)

project("ring-buffer")

add_library(ring-buffer STATIC "src/atomic_spin_lock.cpp" "src/dekkar_lock.cpp")

target_include_directories(ring-buffer PUBLIC "include")
target_include_directories(ring-buffer PUBLIC "src")

set_property(TARGET ring-buffer PROPERTY CXX_STANDARD 23)

add_subdirectory("test")
