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

float signed_triangle_area(vec2 a, vec2 b, vec2 c) {
    // calculate signed area based on the determinant
    return .5*((b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y));
}

// obtain the barycenter gammas for a point p within a triangle
std::array<float,3> bary_gammas(float sabc, vec2 a, vec2 b, vec2 c, vec2 p) {
    // computing the gammas from the barycentric coordinate formula
    float l1 = signed_triangle_area(p, b, c) / sabc;
    float l2 = signed_triangle_area(a, p, c) / sabc;
    float l3 = signed_triangle_area(a, b, p) / sabc;
    return std::array<float,3> {l1, l2, l3};
}

// draw a triangle given three sets of coordinates
void draw_triangle(vec2 a, vec2 b, vec2 c, TGAColor col, TGAImage &img){
    // first find the bounding box for the triangle
    int bbminx = std::min(std::min(a.x, b.x), c.x);
    int bbminy = std::min(std::min(a.y, b.y), c.y);
    int bbmaxx = std::max(std::max(a.x, b.x), c.x);
    int bbmaxy = std::max(std::max(a.y, b.y), c.y);
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            // get the signed triangle area, skip if it is 0
            float sabc = signed_triangle_area(a, b, c);
            if (sabc == 0) continue;
            // compute the barycentric coefficients for all points in the bounding
            // box and draw with the provided color if they are within
            vec2 p = vec2{x, y};
            std::array<float,3> gammas = bary_gammas(sabc, a, b, c, p);
            if (gammas[0] < 0 || gammas[1] < 0 || gammas[2] < 0) continue;
            img.set(x, y, col);
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
    // diablo pose
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

        vec2 a = vec2{x0, y0};
        vec2 b = vec2{x1, y1};
        vec2 c = vec2{x2, y2};

        TGAColor rnd;
        for (int c = 0; c < 3; c++) rnd[c] = std::rand()%255;
        rnd[3] = 255;
        draw_triangle(a, b, c, rnd, image);
    }

    image.write("out.tga");

    return 0;
}