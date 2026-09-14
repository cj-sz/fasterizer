#include "tga.h"
#include "model.h"
#include <cmath>

// Draw a line of color c between two points using
// the linear interpolation varying on t
void draw_line(int ax, int ay, int bx, int by, TGAColor c, TGAImage &img) {
    // transpose the image when the lines are steep, so that we iterate
    // across the y-axis instead of the x
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    // swap the order if ax > bx
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    for (float x = ax; x < bx; x++) {
        float t = (x-ax) / static_cast<float>(bx-ax);
        int y = std::round(ay + (by - ay)*t);
        if (steep) {
            img.set(y, x, c);
        } else {
            img.set(x, y, c);
        }
    }
}

// projects the x, y coordinates of a vec3 onto a width and height
std::pair<int, int> project(vec3 v, int w, int h) {
    // top left is origin so y needs to be flipped
    int sx = static_cast<int>((v.x + 1.) * w / 2.);
    int sy = static_cast<int>((1. - v.y) * h / 2.);
    return {sx, sy};
}

int main() {
    int w = 800;
    int h = 800;
    TGAImage image(w, h);
    Model m;
    m.load("obj/diablo3_pose.obj");

    // convert x and y values of vec3s to 800,800 mapping from each face
    // and draw lines between them 
    for (int i = 0; i < m.nfaces(); i++) {
        vec3 v0 = m.vert(i, 0);
        vec3 v1 = m.vert(i, 1);
        vec3 v2 = m.vert(i, 2);

        auto [x0, y0] = project(v0, w, h);
        auto [x1, y1] = project(v1, w, h);
        auto [x2, y2] = project(v2, w, h);

        draw_line(x0, y0, x1, y1, red, image);
        draw_line(x1, y1, x2, y2, red, image);
        draw_line(x2, y2, x0, y0, red, image);
    }

    image.write("out.tga");
    return 0;
}