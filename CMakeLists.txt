cmake_minimum_required(VERSION 3.27)

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")

project("smipc")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

enable_testing()

add_subdirectory("libs/shared_memory")
# add_subdirectory("libs/client_test") add_subdirectory("libs/server_test")
add_subdirectory("libs/ring_buffer")
add_subdirectory("libs/serdes")
