#!/bin/bash

if [[ $1 == "--debug" ]]
then
	cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug
else
	cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
fi

cmake --build build
ctest --test-dir build -V
