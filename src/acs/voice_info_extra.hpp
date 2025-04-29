#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"

namespace banana {
namespace acs {

class voice_info_extra : public readable {
public:
    USHORT language;
    std::string dialect;
    USHORT gender;
    USHORT age;
    std::string style;

    void write_to(acd_writer &out) const {
        out .write("TTSLangID", acd_writer::hex(language))
            .write("TTSGender", gender)
            .write("TTSAge", age)
            .write("TTSStyle", style);
    }

    void read_from(reader &src) override {
        src .read(language)
            .read(dialect)
            .read(gender)
            .read(age)
            .read(style);
    }
};

}
}
