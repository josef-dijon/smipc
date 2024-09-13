cmake_minimum_required(VERSION 3.27)

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")

project("shared_memory_experiments")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

# add_subdirectory("libs/smipc") add_subdirectory("libs/client_test")
# add_subdirectory("libs/server_test") add_subdirectory("libs/ring_buffer")
add_subdirectory("libs/serdes")
