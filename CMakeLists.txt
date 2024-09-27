cmake_minimum_required(VERSION 3.27)

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")

project("smipc")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

enable_testing()

add_subdirectory("src")
add_subdirectory("test/test-app")
# add_subdirectory("test/client-test") add_subdirectory("test/server-test")
add_subdirectory("test/unit-tests")
add_subdirectory("test/benchmarks")
