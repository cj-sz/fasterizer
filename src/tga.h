#pragma once
#include <cstdint>
#include <string>
#include <vector>

// Enforce no padding for this struct only
#pragma pack(push, 1)
struct TGAHeader {
    std::uint8_t id_len = 0;
    std::uint8_t color_map_type = 0;
    std::uint8_t image_type = 0;
    std::uint16_t first_entry_idx = 0;
    std::uint16_t color_map_len = 0;
    std::uint8_t color_map_entry_size = 0;
    std::uint16_t img_x_origin = 0;
    std::uint16_t img_y_origin = 0;
    std::uint16_t img_width = 0;
    std::uint16_t img_height = 0;
    std::uint8_t pixel_depth = 0;
    std::uint8_t img_descriptor = 0;
};
#pragma pack(pop)

struct TGAColor {
    std::uint8_t bgra[4] = {0,0,0,0};
};

constexpr TGAColor white = {255, 255, 255, 255};
constexpr TGAColor blue = {255, 0, 0, 255};
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};

struct TGAImage {
    int w, h;
    std::vector<TGAColor> pixels;

    TGAImage(int w, int h);
    void set(int x, int y, TGAColor c);
    bool write(const std::string& path) const;
    void draw_line(int ax, int ay, int bx, int by, TGAColor c, TGAImage &img);
};