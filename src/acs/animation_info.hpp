#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"
#include "locator.hpp"
#include "frame_info.hpp"

namespace banana {
namespace acs {

enum class transition_type : BYTE {
    UseReturnAnimation = 0,
    UseExitBranches = 1,
    NoTransition = 2
};

class animation_info : public readable {
public:
    std::string name;
    std::string internal_name;
    acs::transition_type transition_type;
    std::string return_animation_name;
    std::vector<frame_info> frames;

    void write_to(acd_writer &out) const {
        out .begin_block("DefineAnimation", name)
            .write_empty_line();
        

        switch (transition_type) {
            case acs::transition_type::UseReturnAnimation:
                out.write("Return", return_animation_name);
                break;
            default:
                out.write("TransitionType", transition_type);
                break;
        }
        
        out.write_empty_line();
        
        for (auto &frame : frames) {
            out .write(frame)
                .write_empty_line();
        }

        out.end_block("EndAnimation");
    }

    void read_from(reader &src) override {
        src .read(name)
            .read<locator>().get_subreader(src)
            .read(internal_name)
            .read(transition_type)
            .read(return_animation_name)
            .read_list<USHORT>(frames);
    }
};

}
}
