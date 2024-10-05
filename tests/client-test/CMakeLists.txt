cmake_minimum_required(VERSION 3.28)

project("client-test")

add_executable(client-test "main.cpp")

target_link_libraries(client-test PRIVATE smipc)

set_property(TARGET client-test PROPERTY CXX_STANDARD 23)
