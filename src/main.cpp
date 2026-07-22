#include "tga.h"
#include <cmath>

// Draw a line of color c between two points using
// the linear interpolation varying on t
void draw_line(int ax, int ay, int bx, int by, TGAColor c, TGAImage &img) {
    for (float t = 0; t <= 1; t += 0.02) {
        int x = std::round(ax + (bx - ax)*t);
        int y = std::round(ay + (by - ay)*t);
        img.set(x, y, c);
    }
}

int main() {
    TGAImage image(64, 64);
    image.set(0, 0, red);
    image.set(50, 10, green);
    image.set(12, 48, blue);

    draw_line(0, 0, 50, 10, red, image);
    draw_line(50, 10, 12, 48, green, image);
    draw_line(12, 48, 0, 0, blue, image);

    image.write("out.tga");
    return 0;
}