#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"

namespace banana {
namespace acs {

class branch_info : public readable {
public:
    USHORT next_frame_index;
    USHORT probability_percentage;

    void write_to(acd_writer &out) const {
        out .write("BranchTo", next_frame_index + 1)
            .write("Probability", probability_percentage);
    }

    void read_from(reader &src) override {
        src .read(next_frame_index)
            .read(probability_percentage);
    }
};

}
}
