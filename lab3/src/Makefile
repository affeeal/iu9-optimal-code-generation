.PHONY: format clean generate build

generate:
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build:
	cmake --build build

clean:
	rm -rf build .cache

format:
	clang-format -i --style=Google *.h *.cc
