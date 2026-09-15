#include "tga.h"
#include "model.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

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
// use a color if value passed in is true, otherwise use the rgb
void draw_triangle(vec2 a, vec2 b, vec2 c, bool use_col, TGAColor col, TGAImage &img){
    // first find the bounding box for the triangle
    int bbminx = std::min(std::min(a.x, b.x), c.x);
    int bbminy = std::min(std::min(a.y, b.y), c.y);
    int bbmaxx = std::max(std::max(a.x, b.x), c.x);
    int bbmaxy = std::max(std::max(a.y, b.y), c.y);
    // compute signed triangle area and ensure nonzero
    float sabc = signed_triangle_area(a, b, c);
    if (sabc < 1) return;
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            // compute the barycentric coefficients for all points in the bounding
            // box and draw with the provided color if they are within
            vec2 p = vec2{x, y};
            std::array<float,3> gammas = bary_gammas(sabc, a, b, c, p);
            if (gammas[0] < 0 || gammas[1] < 0 || gammas[2] < 0) continue;
            if (use_col){
                img.set(x, y, col);
            } else {
                TGAColor color = TGAColor{gammas[0] * 255, gammas[1] * 255, gammas[2] * 255, 255};
                img.set(x, y, color);
            }

        }
    }
}

// projects the x, y coordinates of a vec3 onto a width and height
vec2 project(vec3 v, int w, int h) {
    // top left is origin so y needs to be flipped
    float sx = ((v.x + 1.) * w / 2.);
    float sy = ((1. - v.y) * h / 2.);
    return vec2{sx, sy};
}

int main() {
    // random seed 
    std::srand(static_cast<unsigned>(std::time(nullptr)));

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

        vec2 a = project(v0, w, h);
        vec2 b = project(v1, w, h);
        vec2 c = project(v2, w, h);

        TGAColor rnd;
        for (int j = 0; j < 3; j++) rnd[j] = std::rand()%256;
        rnd[3] = 255;
        draw_triangle(a, b, c, false, rnd, image);
    }

    image.write("out.tga");

    return 0;
}