#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"
#include "region_data.hpp"
#include <optional>
#include <string>

namespace banana {
namespace acs {

enum class overlay_type : BYTE {
    MouthClosed = 0,
    MouthWideOpen1 = 1,
    MouthWideOpen2 = 2,
    MouthWideOpen3 = 3,
    MouthWideOpen4 = 4,
    MouthMedium = 5,
    MouthNarrow = 6
};

class overlay_info : public readable {
public:
    acs::overlay_type overlay_type;
    BOOL replace_top_image;
    USHORT image_index;
    BYTE unknown;
    SHORT x_offset;
    SHORT y_offset;
    USHORT width;
    USHORT height;
    std::optional<acs::region_data> region_data;

    void write_to(acd_writer &out) const {
        out.begin_block("DefineMouth");
        const char *mouth;
        switch (overlay_type) {
            case acs::overlay_type::MouthClosed: mouth = "Closed"; break;
            case acs::overlay_type::MouthWideOpen1: mouth = "OpenWide1"; break;
            case acs::overlay_type::MouthWideOpen2: mouth = "OpenWide2"; break;
            case acs::overlay_type::MouthWideOpen3: mouth = "OpenWide3"; break;
            case acs::overlay_type::MouthWideOpen4: mouth = "OpenWide4"; break;
            case acs::overlay_type::MouthMedium: mouth = "OpenMedium"; break;
            case acs::overlay_type::MouthNarrow: mouth = "OpenNarrow"; break;
            default: mouth = ""; break;
        }
        out .write("Type", acd_writer::enum_name(mouth), { "" })
            .write("Filename", "Images\\" + acd_writer::itoa(image_index) + ".bmp")
            .write("ReplaceBaseFrame", replace_top_image, (BOOL)0)
            .end_block("EndMouth");
    }

    void read_from(reader &src) override {
        BOOL region_data_present;

        src .read(overlay_type)
            .read(replace_top_image)
            .read(image_index)
            .read(unknown)
            .read(region_data_present)
            .read(x_offset)
            .read(y_offset)
            .read(width)
            .read(height);
        
        if (region_data_present) {
            ULONG size;
            src.read(size)
                .get_subreader(src.offset(), size)
                .read(region_data);
        }
    }
};

}
}
