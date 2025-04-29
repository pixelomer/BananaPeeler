#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"

namespace banana {
namespace acs {

class frame_image : public readable {
public:
    ULONG image_index;
    SHORT x_offset;
    SHORT y_offset;

    void write_to(acd_writer &out) const {
        out .begin_block("DefineImage")
            .write("Filename", "Images\\" + acd_writer::itoa(image_index) + ".bmp")
            .end_block("EndImage");
    }

    void read_from(reader &src) override {
        src .read(image_index)
            .read(x_offset)
            .read(y_offset);
    }
};

}
}
