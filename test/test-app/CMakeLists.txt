cmake_minimum_required(VERSION 3.28)

project("test-app")

add_executable(test-app "main.cpp")

target_link_libraries(test-app PRIVATE smipc)

set_property(TARGET test-app PROPERTY CXX_STANDARD 23)
