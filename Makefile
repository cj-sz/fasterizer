.PHONY: all run clean

all:
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug --log-level=ERROR
	cmake --build build

run: all
	./build/fasterizer

clean:
	rm -rf build out.tga
