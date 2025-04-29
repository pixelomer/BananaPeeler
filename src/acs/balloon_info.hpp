#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"
#include "rgb_quad.hpp"

namespace banana {
namespace acs {

class balloon_info : public readable {
public:
    BYTE number_of_lines;
    BYTE characters_per_line;
    rgb_quad foreground_color;
    rgb_quad background_color;
    rgb_quad border_color;
    std::string font_name;
    LONG font_height;
    LONG font_weight;
    BOOL italicized;
    BYTE unknown;

    void write_to(acd_writer &out) const {
        out .begin_block("DefineBalloon")
            .write("NumLines", number_of_lines)
            .write("CharsPerLine", characters_per_line)
            .write("FontName", font_name)
            .write("FontHeight", -font_height)
            .write("ForeColor", foreground_color)
            .write("BackColor", background_color)
            .write("BorderColor", border_color)
            .end_block("EndBalloon");
    }

    void read_from(reader &src) override {
        src .read(number_of_lines)
            .read(characters_per_line)
            .read(foreground_color)
            .read(background_color)
            .read(border_color)
            .read(font_name)
            .read(font_height)
            .read(font_weight)
            .read(italicized)
            .read(unknown);
    }
};

}
}
