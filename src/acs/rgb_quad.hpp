#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"

namespace banana {
namespace acs {

class rgb_quad : public readable {
public:
    BYTE red, green, blue, reserved;
    rgb_quad() {}
    rgb_quad(BYTE red, BYTE green, BYTE blue, BYTE reserved):
        red(red), green(green), blue(blue), reserved(reserved) {}
    
    void write_to(acd_writer &out) const {
        char urgb[9];
        snprintf(urgb, sizeof(urgb), "%02hhx%02hhx%02hhx%02hhx",
            reserved, red, green, blue);
        urgb[sizeof(urgb)-1] = 0;
        out.write(acd_writer::unquoted_str(urgb));
    }

    void read_from(reader &src) override {
        src .read(blue)
            .read(green)
            .read(red)
            .read(reserved);
    }
};

}
}
