cmake_minimum_required(VERSION 3.27)

project("ring-buffer-unit-tests")

find_package(GTest CONFIG REQUIRED)

add_executable(ring-buffer-unit-tests "ring_buffer_unit.cpp")

target_link_libraries(ring-buffer-unit-tests PRIVATE smipc)
target_link_libraries(ring-buffer-unit-tests PRIVATE GTest::gtest)

set_property(TARGET ring-buffer-unit-tests PROPERTY CXX_STANDARD 23)

add_test(NAME ring-buffer-unit-tests COMMAND ring-buffer-unit-tests
                                             --gtest_color=1)
