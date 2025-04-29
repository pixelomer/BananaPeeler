#pragma once

#include "readable.hpp"

namespace banana {
namespace acs {

class tray_icon : public readable {
public:
    std::vector<BYTE> monochrome_bitmap;
    std::vector<BYTE> color_bitmap;
    void read_from(reader &src) override {
        src .read_list<ULONG>(monochrome_bitmap)
            .read_list<ULONG>(color_bitmap);
    }
};

}
}
