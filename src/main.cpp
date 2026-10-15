#include "model.h"
#include "rasterizer.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <vector>

// example shader is a random shader, just picks a random color
struct RandomShader : IShader {
    const Model &model;
    TGAColor col = {};
    vec3 tri[3]; // this is the triangle in eye coordinates

    RandomShader(const Model &m) : model(m) {

    }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.vert(face, vert); // vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz(); // vertex in eye coordinates
        return Perspective * gl_Position; // this is the clip coordinates
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
        return {false, col}; // the bool tells if the pixel should be discarded
    }
};

int main() {
    // random seed 
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // diablo pose
    constexpr int w = 800;
    constexpr int h = 800;
    constexpr vec3 eyepos{-1, 0, 2};
    constexpr vec3 center{0,0,0};
    constexpr vec3 up{0,1,0}; // change to 0, 1, 0.8 to make him jojo pose

    // initialize the static matrices based on the eye position, center, and up
    // direction
    lookat(eyepos, center, up);
    perspective((eyepos-center).length());
    viewport(w/16, h/16, w*7/8, h*7/8);

    TGAImage framebuffer(w, h);
    TGAImage zbufferimage(w, h);
    init_zbuffer(w, h);
    Model m;
    m.load("obj/diablo3_pose.obj");

    RandomShader shader(m);

    // use the shader and the rasterizer to produce the image
    for (int i = 0; i < m.nfaces(); i++) {
        shader.col = { std::rand() % 255, std::rand()%255, std::rand()%255, 255};
        Triangle clip = {shader.vertex(i, 0), shader.vertex(i, 1), shader.vertex(i, 2)};
        rasterize(clip, shader, framebuffer);
    }

    framebuffer.write("framebuf.tga");
    
    // // now to draw the zbuffer we first iterate over the zbuffer vector so we
    // // can get the min and max to normalize the values to [0, 255]
    // float zmin = std::numeric_limits<float>::infinity();
    // float zmax = -std::numeric_limits<float>::infinity();
    // for (int i = 0; i < w * h; i++) {
    //     if (zbuffer[i] != -std::numeric_limits<float>::infinity()) {
    //         if (zbuffer[i] < zmin) zmin = zbuffer[i];
    //         if (zbuffer[i] > zmax) zmax = zbuffer[i];
    //     }
    // }
    // // now normalize the values to the 0,255 range based on the found limits
    // // and for all zbuffer values of -inf make the pixel black
    // for (int x = 0; x < w; x ++) {
    //     for (int y = 0; y < h; y++) {
    //         float z = zbuffer[y * w + x];
    //         if (z == -std::numeric_limits<float>::infinity()) {
    //             zbufferimage.set(x, y, TGAColor{0,0,0,255});
    //         } else {
    //             float nc = (z - zmin) * 255 / (zmax - zmin);
    //             zbufferimage.set(x, y, TGAColor{nc, nc, nc, 255});
    //         }
    //     }
    // }

    // zbufferimage.write("zbuf.tga");

    return 0;
}