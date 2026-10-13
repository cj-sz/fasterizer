#include "tga.h"
#include "model.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <vector>

mat<4> ModelView, Viewport, Perspective;

// top-left corner of the viewport is defined by (x, y) and the size is
// defined by (w, h)
void viewport(const int x, const int y, const int w, const int h) {
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
}

// perspective matrix for a canvas on the z-axis at a focal distance f
void perspective(const float f) {
    Perspective = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, -1/f, 1}}};
}

// model-view matrix for a given eye position, center of focus, and upward
// direction
void lookat(const vec3 eye, const vec3 center, const vec3 up) {
    // define the coordinate vectors from the center
    vec3 n = (eye - center).normalize();
    vec3 l = up.cross(n).normalize();
    vec3 m = n.cross(l).normalize();
    ModelView = mat<4>{{{l.x, l.y, l.z, 0}, {m.x, m.y, m.z, 0}, {n.x, n.y, n.z, 0}, {0,0,0,1}}}
        * mat<4>{{{1,0,0,-center.x}, {0, 1, 0, -center.y}, {0, 0, 1, -center.z}, {0, 0, 0, 1}}};
}

// draw a triangle given three sets of coordinates
// use a color if value passed in is true, otherwise use the rgb
// expects a.z/b.z/c.z to already be normalized into [0,255] by the caller
void draw_triangle(const vec4 clip[3], bool use_col, TGAColor col, TGAImage &framebuffer, TGAImage &zbuffer){
    // ndc describes the normalized device coordinates, i.e. the perspective
    // transformation (clip coordinates)
    // the clip coordinates here are computed in the main loop
    // and they are the result of Viewport * ModelView on each set of coordinates
    // corresponding to a single triangle
    vec4 ndc[3] = { clip[0]/clip[0].w, clip[1]/clip[1].w, clip[2]/clip[2].w };
    vec2 screen[3] = { (Viewport*ndc[0]).xy(), (Viewport*ndc[1]).xy(), (Viewport*ndc[2]).xy() };

    // build the triangle as a matrix
    mat<3> ABC = {{ {screen[0].x, screen[0].y, 1.}, {screen[1].x, screen[1].y, 1.}, {screen[2].x, screen[2].y, 1.}}};
    if (ABC.det()<1) return; // backface culling and triangles smaller than a pixel discarded under these circumstances

    // efficiently get the upper left and bottom right corners for the bounding box
    auto [bbminx, bbmaxx] = std::minmax({screen[0].x, screen[1].x, screen[2].x});
    auto [bbminy, bbmaxy] = std::minmax({screen[0].y, screen[1].y, screen[2].y});

    #pragma omp parallel for
    // iterate over the bounding box clipped by what will be shown
    for (int x = std::max<int>(bbminx, 0); x <= std::min<int>(bbmaxx, framebuffer.w - 1); x++) {
        for (int y = std::max<int>(bbminy, 0); y <= std::min<int>(bbmaxx, framebuffer.h - 1); y++) {
            // compute the barycentric coordinates for this x, y combination
            // with respect to the current triangle (what is computed in ABC)
            // -> is because inverse() returns an optional<> type
            // we will never have an error on the optional<> type here because
            // passing the determinant check earlier guarantees the matrix is
            // invertible
            vec3 bc = ABC.inverse()->T() * vec3 {static_cast<float>(x), static_cast<float>(y), 1.};
            
            // exit if this x,y is not inside the triangle
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            // formulate the z-buffer
            float z = bc.dot(vec3{ndc[0].z, ndc[1].z, ndc[2].z});

            if (z >= zbuffer.get(x, y)[0]) {
                zbuffer.set(x, y, {z, z, z, 255});
                if (use_col){
                    framebuffer.set(x, y, col);
                } else {
                    TGAColor color = TGAColor{l1 * 255, l2 * 255, l3 * 255, 255};
                    framebuffer.set(x, y, color);
                }
            }
        }
    }
}

// projects the x, y coordinates of a vec3 onto a width, height, and z-value for zbuffering
// normalizing to a range, for a comera a dist c away from the z=0 plane which
// is used as the canvas
vec3 project(vec3 v, int w, int h, float c, float zmin, float zmax) {
    // factor in the camera distance first 
    float xp = v.x / (1 - v.z / c);
    float yp = v.y / (1 - v.z / c);
    // top left is origin so y needs to be flipped
    float sx = ((xp + 1.) * w / 2.);
    float sy = ((1. - yp) * h / 2.);
    float sz = (v.z - zmin) / (zmax - zmin) * 255.;
    return vec3{sx, sy, sz};
}

int main() {
    // random seed 
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // diablo pose
    constexpr int w = 800;
    constexpr int h = 800;
    constexpr vec3 eyepos{-1, 0, 2};
    constexpr vec3 center{0,0,0};
    constexpr vec3 up{0,1,0};

    // initialize the static matrices based on the eye position, center, and up
    // direction
    lookat(eyepos, center, up);
    perspective((eyepos-center).normalize());
    viewport(w/16, h/16, w*7/8, h*7/8);

    TGAImage framebuffer(w, h);
    TGAImage zbuffer(w, h);
    Model m;
    m.load("obj/diablo3_pose.obj");

    // rotate every face vertex once and track z bounds
    std::vector<vec3> rotated(m.nfaces() * 3);
    float zmin = std::numeric_limits<float>::infinity();
    float zmax = -std::numeric_limits<float>::infinity();
    for (int i = 0; i < m.nfaces(); i++) {
        for (int j = 0; j < 3; j++) {
            vec3 rv = rot(m.vert(i, j));
            rotated[i * 3 + j] = rv;
            if (rv.z < zmin) zmin = rv.z;
            if (rv.z > zmax) zmax = rv.z;
        }
    }

    // project each cached rotated vertex using the zmin/zmax and draw lines
    for (int i = 0; i < m.nfaces(); i++) {
        vec3 a = project(rotated[i * 3 + 0], w, h, cdist, zmin, zmax);
        vec3 b = project(rotated[i * 3 + 1], w, h, cdist, zmin, zmax);
        vec3 c = project(rotated[i * 3 + 2], w, h, cdist, zmin, zmax);

        TGAColor rnd;
        for (int j = 0; j < 3; j++) rnd[j] = std::rand()%256;
        rnd[3] = 255;
        draw_triangle(a, b, c, false, rnd, framebuffer, zbuffer);
    }

    framebuffer.write("framebuf.tga");
    zbuffer.write("zbuf.tga");

    return 0;
}