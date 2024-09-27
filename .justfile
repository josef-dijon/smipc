default: build

clean:
	@echo "Cleaning..."
	rm -rf build
	rm -rf vcpkg_installed
	rm -f vcpkg-manifest-install.log

cmake:
	@echo "Configuring cmake..."
	cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake-debug:
	@echo "Configuring cmake in debug mode..."
	cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug

build: cmake
	@echo "Building..."
	cmake --build build

build-debug: cmake-debug
	@echo "Building in debug mode..."
	cmake --build build

test: build
	@echo "Testing..."
	ctest --test-dir build -V

benchmark: test
	@echo "Benchmarking..."
	jq --argjson results "$(cat build/libs/serdes/test/benchmarks/results.json )" '."benchmark-runs" += [$results]' test/serdes_benchmark_history.json > build/tmp_serdes_benchmark_history.json
	mv build/tmp_serdes_benchmark_history.json test/serdes_benchmark_history.json

format:
	@echo "Formatting..."
	find libs -type f -name '*.h' -o -name '*.cpp' | xargs clang-format -i --verbose
