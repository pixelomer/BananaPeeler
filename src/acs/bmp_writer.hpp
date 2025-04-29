#pragma once
#include "rgb_quad.hpp"
#include <array>
#include <cstring>
#include <fstream>
#include <ostream>

namespace banana {
namespace acs {

class bmp_writer {
private:
    std::array<uint8_t, 1024> m_palette;
public:
    void set_palette(std::array<rgb_quad, 256> const& palette) {
        for (int i=0; i<256; ++i) {
            m_palette[i*4 + 0] = palette[i].blue;
            m_palette[i*4 + 1] = palette[i].green;
            m_palette[i*4 + 2] = palette[i].red;
            m_palette[i*4 + 3] = palette[i].reserved;
        }
    }
    void set_palette(std::vector<rgb_quad> const& palette) {
        size_t i;
        for (i=0; i<palette.size() && i<256; ++i) {
            m_palette[i*4 + 0] = palette[i].blue;
            m_palette[i*4 + 1] = palette[i].green;
            m_palette[i*4 + 2] = palette[i].red;
            m_palette[i*4 + 3] = palette[i].reserved;
        }
        for (i=i*4; i<1024; ++i) {
            m_palette[i] = 0;
        }
    }
    std::array<uint8_t, 54> build_header(int width, int height,
        std::vector<uint8_t> const& image_data)
    {
        int image_size = width * height;
        if (image_size != (int)image_data.size()) {
            throw std::runtime_error("image size mismatch");
        }
        #define encode(num) (uint8_t)(num), (uint8_t)((num) >> 8), \
            (uint8_t)((num) >> 16), (uint8_t)((num) >> 24)
        return {
            0x42, 0x4D, // "BM" magic
            encode(image_size + 1078), // Bitmap size
            0x00, 0x00, 0x00, 0x00, // Reserved (always 0)
            0x36, 0x04, 0x00, 0x00, // Bitmap offset (always 1078)
            0x28, 0x00, 0x00, 0x00, // Header size (always 40)
            encode(width), // Image width
            encode(height), // Image height
            0x01, 0x00, // Planes (always 1)
            0x08, 0x00, // Bit count (always 8)
            0x00, 0x00, 0x00, 0x00, // Compression (no compression)
            encode(image_size), // Image size
            0x00, 0x00, 0x00, 0x00, // biXPelsPerMeter (always 0)
            0x00, 0x00, 0x00, 0x00, // biYPelsPerMeter (always 0)
            0x00, 0x00, 0x00, 0x00, // biClrUsed (always 0)
            0x00, 0x00, 0x00, 0x00, // biClrImportant (always 0)
        };
        #undef encode
    }
    std::vector<uint8_t> build_bmp(int width, int height,
        std::vector<uint8_t> const& image_data)
    {
        auto header = build_header(width, height, image_data);
        std::vector<uint8_t> bmp(image_data.size() + 1078);
        memcpy((void *)&bmp[0],
            (void *)header.data(), header.size());
        memcpy((void *)&bmp[header.size()],
            (void *)m_palette.data(), m_palette.size());
        memcpy((void *)&bmp[header.size() + m_palette.size()],
            (void *)image_data.data(), image_data.size());
        return bmp;
    }
    void write_bmp(std::ostream &output, int width, int height,
        std::vector<uint8_t> const& image_data)
    {
        auto header = build_header(width, height, image_data);
        output.write((const char *)header.data(), header.size());
        output.write((const char *)m_palette.data(), m_palette.size());
        output.write((const char *)image_data.data(), image_data.size());
    }
    void write_bmp(std::string const& filename, int width, int height,
        std::vector<uint8_t> const& image_data)
    {
        std::ofstream out;
        out.open(filename, std::ios::binary | std::ios::out);
        write_bmp(out, width, height, image_data);
        out.close();
    }
};

}
}
