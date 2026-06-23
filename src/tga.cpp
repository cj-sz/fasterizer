#include "tga.h"
#include <fstream>
#include <cmath>

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
    // Open a file stream w/o interpreting certain bit sequences
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    TGAHeader header;
    header.image_type = 2; // uncompressed true color
    header.img_width = w;
    header.img_height = h;
    header.pixel_depth = 32; // 4 bytes/pixel
    // 0x20 for top-left pixel origin
    // 0x8 signals 8 bits for alpha
    header.img_descriptor = 0x20 | 8;

    // Write the header to the file
    out.write(reinterpret_cast<const char*>(&header), sizeof(TGAHeader));
    // Write the content after. vector.data() returns a pointer to
    // the first element of the vector
    out.write(reinterpret_cast<const char*>(pixels.data()), w * h * sizeof(TGAColor));

    return out.good();
}

// Draw a line of color c between two points using
// the linear interpolation varying on t
void TGAImage::draw_line(int ax, int ay, int bx, int by, TGAColor c, TGAImage &img) {
    for (float t = 0; t <= 1; t += 0.2) {
        int x = std::round(ax + (bx - ax)*t);
        int y = std::round(ay + (by - ay)*t);
        img.set(x, y, c);
    }
}