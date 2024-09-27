cmake_minimum_required(VERSION 3.28)

project("server-test")

add_executable(server-test "main.cpp")

target_link_libraries(server-test PRIVATE smipc)

set_property(TARGET server-test PROPERTY CXX_STANDARD 23)
