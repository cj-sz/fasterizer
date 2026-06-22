#include "tga.h"
#include <fstream>

static_assert(sizeof(TGAHeader) == 18); // no padding
static_assert(sizeof(TGAColor) == 4);

TGAImage::TGAImage(int w, int h) {
    this->w = w;
    this->h = h;
    pixels.resize(w * h);
}

void TGAImage::set(int x, int y, TGAColor c) {
    if (x >= w || y >= h || x < 0 || y < 0) return;
    pixels[y * w + x] = c;
}

bool TGAImage::write(const std::string& path) const{
    
}