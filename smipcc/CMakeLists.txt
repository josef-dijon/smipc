# MIT License
#
# Copyright (c) 2024 Josef de Joanelli (josef@pixelrift.io)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.28)

project("smipc")

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(smipcc "${CMAKE_CURRENT_SOURCE_DIR}/smipcc/main.cpp")

target_link_libraries(smipcc PUBLIC smipc)
target_link_libraries(smipcc PUBLIC fmt::fmt)
target_link_libraries(smipcc PUBLIC nlohmann_json::nlohmann_json)

target_include_directories(smipcc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_property(TARGET smipcc PROPERTY CXX_STANDARD 23)
