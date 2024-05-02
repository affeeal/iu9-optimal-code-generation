.PHONY: clean generate build

generate:
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DGRAMMAR="parser.y"

build:
	cmake --build build

clean:
	rm -rf build .cache
