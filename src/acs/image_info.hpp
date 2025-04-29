#pragma once

#include "readable.hpp"
#include "locator.hpp"
#include "compressed_data.hpp"

namespace banana {
namespace acs {

class image_info : public readable {
public:
    BYTE unknown1;
    USHORT width;
    USHORT height;
    compressed_data image_data;
    compressed_data region_data;
    ULONG unknown2;
    void read_from(reader &src) override {
        reader detail = src.read<locator>().get_subreader(src);

        BOOL image_compressed;
        detail.read(unknown1)
            .read(width)
            .read(height)
            .read(image_compressed);
        
        image_data.set_image_data(image_compressed, width, height,
            detail.read_list<BYTE, ULONG>());
        
        detail.read(region_data);

        src.read(unknown2);
    }
};

}
}
