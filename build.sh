#!/bin/bash

if [[ $1 == "--debug" ]]
then
	cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug
else
	cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
fi

if [[ $? -ne 0 ]]
then
	exit 1
fi

cmake --build build

if [[ $? -ne 0 ]]
then
	exit 1
fi

ctest --test-dir build -V

if [[ $? -ne 0 ]]
then
	exit 1
fi

jq --argjson results "$(cat build/libs/serdes/test/benchmarks/results.json )" '."benchmark-runs" += [$results]' test/serdes_benchmark_history.json > build/tmp_serdes_benchmark_history.json

if [[ $? -ne 0 ]]
then
	exit 1
fi

mv build/tmp_serdes_benchmark_history.json test/serdes_benchmark_history.json
