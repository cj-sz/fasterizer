#include "tga.h"
#include "geometry.h"

extern mat<4> ModelView, Viewport, Perspective;

// functions required for rasterization
void viewport(const int x, const int y, const int w, const int h);
void perspective(const float f);
void lookat(const vec3 eye, const vec3 center, const vec3 up);
void init_zbuffer(const int w, const int h);

struct IShader {
    // bar here represents the barycentric weights for a pixel
    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const = 0;
};

typedef vec4 Triangle[3];
void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer);