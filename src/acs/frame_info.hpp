#pragma once

#include "acd_writer.hpp"
#include "readable.hpp"
#include <vector>
#include "frame_image.hpp"
#include "branch_info.hpp"
#include "overlay_info.hpp"

namespace banana {
namespace acs {

class frame_info : public readable {
public:
    std::vector<frame_image> images;
    SHORT audio_index;
    USHORT frame_duration;
    SHORT exit_frame_index;
    std::vector<branch_info> branches;
    std::vector<overlay_info> overlays;

    void write_to(acd_writer &out) const {
        out .begin_block("DefineFrame")
            .write("Duration", frame_duration);
        if (audio_index != -1) {
            out.write("SoundEffect", "Audio\\" + acd_writer::itoa(audio_index) + ".wav");
        }
        if (exit_frame_index > -1) {
            out.write("ExitBranch", exit_frame_index + 1);
        }
        if (branches.size() > 0) {
            out.begin_block("DefineBranching");
            for (auto &branch : branches) {
                out.write(branch);
            }
            out.end_block("EndBranching");
        }
        for (auto &overlay : overlays) {
            out.write(overlay);
        }
        for (auto &image : images) {
            out.write(image);
        }
        out.end_block("EndFrame");
    }

    void read_from(reader &src) override {
        src .read_list<USHORT>(images)
            .read(audio_index)
            .read(frame_duration)
            .read(exit_frame_index)
            .read_list<BYTE>(branches)
            .read_list<BYTE>(overlays);
    }
};

}
}
