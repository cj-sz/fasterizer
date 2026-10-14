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
void rasterize(const vec4 clip[3], TGAColor col, TGAImage &framebuffer, std::vector<float> &zbuffer){
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
        for (int y = std::max<int>(bbminy, 0); y <= std::min<int>(bbmaxy, framebuffer.h - 1); y++) {
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

            if (z >= zbuffer[x + y * framebuffer.w]) {
                zbuffer[x + y * framebuffer.w] = z;
                framebuffer.set(x, y, col);
            }
        }
    }
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
    perspective((eyepos-center).length());
    viewport(w/16, h/16, w*7/8, h*7/8);

    TGAImage framebuffer(w, h);
    TGAImage zbufferimage(w, h);
    std::vector<float> zbuffer(w * h, -std::numeric_limits<float>::infinity());
    Model m;
    m.load("obj/diablo3_pose.obj");

    // project each cached rotated vertex using the zmin/zmax and draw lines
    for (int i = 0; i < m.nfaces(); i++) {
        vec4 clip[3];
        // build the primitive for the 3 vertices making up the face (triangle)
        for (int d : {0, 1, 2}) {
            vec3 v = m.vert(i, d);
            // First three steps of the rasterization transformation
            clip[d] = Perspective * ModelView * vec4{v.x, v.y, v.z, 1.};
        }

        TGAColor rnd;
        for (int j = 0; j < 3; j++) rnd[j] = std::rand()%256;
        rnd[3] = 255;
        rasterize(clip, rnd, framebuffer, zbuffer);
    }

    framebuffer.write("framebuf.tga");
    
    // now to draw the zbuffer we first iterate over the zbuffer vector so we
    // can get the min and max to normalize the values to [0, 255]
    float zmin = std::numeric_limits<float>::infinity();
    float zmax = -std::numeric_limits<float>::infinity();
    for (int i = 0; i < w * h; i++) {
        if (zbuffer[i] != -std::numeric_limits<float>::infinity()) {
            if (zbuffer[i] < zmin) zmin = zbuffer[i];
            if (zbuffer[i] > zmax) zmax = zbuffer[i];
        }
    }
    // now normalize the values to the 0,255 range based on the found limits
    // and for all zbuffer values of -inf make the pixel black
    for (int x = 0; x < w; x ++) {
        for (int y = 0; y < h; y++) {
            float z = zbuffer[y * w + x];
            if (z == -std::numeric_limits<float>::infinity()) {
                zbufferimage.set(x, y, TGAColor{0,0,0,255});
            } else {
                float nc = (z - zmin) * 255 / (zmax - zmin);
                zbufferimage.set(x, y, TGAColor{nc, nc, nc, 255});
            }
        }
    }

    zbufferimage.write("zbuf.tga");

    return 0;
}