#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"

namespace banana {
namespace acs {

class localized_info : public readable {
public:
    USHORT language;
    USHORT unknown;
    std::string name;
    std::string description;
    std::string extra_data;

    void write_to(acd_writer &out) const {
        out.begin_block("DefineInfo", acd_writer::hex(unknown))
            .write("Name", name)
            .write("Description", description)
            .write("ExtraData", extra_data)
            .end_block("EndInfo");
    }

    void read_from(reader &src) override {
        src .read(language)
            .read(unknown)
            .read(name)
            .read(description)
            .read(extra_data);
    }
};

}
}
