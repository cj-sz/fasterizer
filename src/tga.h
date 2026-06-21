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