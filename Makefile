.PHONY: all run png clean

all:
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug --log-level=ERROR
	cmake --build build

run: all
	./build/fasterizer
	$(MAKE) png

png:
	convert framebuf.tga framebuf.png
	convert zbuf.tga zbuf.png

clean:
	rm -rf build out.tga framebuf.tga zbuf.tga framebuf.png zbuf.png
