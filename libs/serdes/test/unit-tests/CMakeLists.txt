cmake_minimum_required(VERSION 3.27)

project("serdes-unit-tests")

find_package(GTest CONFIG REQUIRED)

# Add source to this project's executable.
add_executable(serdes-unit-tests unit_test.cpp)

target_link_libraries(serdes-unit-tests PRIVATE serdes)
target_link_libraries(serdes-unit-tests PRIVATE GTest::gtest)

set_property(TARGET serdes-unit-tests PROPERTY CXX_STANDARD 23)

add_test(UnitTests serdes-unit-tests)
