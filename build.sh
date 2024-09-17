#!/bin/bash

if [[ $1 == "--full" ]]
then
	cmake -B build -S . -G Ninja
fi

cmake --build build
ctest --test-dir build -V
