#include "tga.h"

int main() {
    TGAImage image(64, 64);
    image.set(0, 0, red);
    image.set(50, 10, green);
    image.set(12, 48, blue);
    image.write("out.tga");
    return 0;
}