#pragma once

#include "readable.hpp"
#include <cstdio>
#include <iostream>
#include <map>
#include "acd_writer.hpp"
#include <fstream>
#include "character_info.hpp"
#include "animation_info.hpp"
#include "image_info.hpp"
#include "audio_info.hpp"

namespace banana {
namespace acs {

class header : public readable {
public:
    acs::character_info character_info;
    std::vector<animation_info> animations;
    std::vector<image_info> images;
    std::vector<audio_info> sounds;

    //animation_names["MOVING"] = "Moving"
    std::map<std::string, std::string> animation_names;

    //animation_indices["Moving"] = 4 (referring to animations[4])
    std::map<std::string, size_t> animation_indices;

    animation_info &animation_named(std::string const& name) {
        return animations[animation_indices[name]];
    }

    void read_from(reader &src) override {
        auto signature = src.read<ULONG>();
        if (signature != 0xABCDABC3) {
            std::cerr << "warning: invalid signature: " << signature << std::endl;
        }
        animation_names.clear();
        animation_indices.clear();
        src .read<locator>().deref(src, character_info)
            .read<locator>().deref_list<ULONG>(src, animations)
            .read<locator>().deref_list<ULONG>(src, images)
            .read<locator>().deref_list<ULONG>(src, sounds);
        for (size_t i=0; i<animations.size(); ++i) {
            auto &anim = animations[i];
            animation_indices[anim.name] = i;
            animation_names[anim.internal_name] = anim.name;
        }
    }

    void write_to(acd_writer &out) const {
        out.write(character_info);
        for (auto &anim : animations) {
            out .write(anim)
                .write_separator();
        }
        for (auto &state : character_info.animation_states) {
            static const std::map<std::string, std::string> builtins = {
                { "GESTURINGDOWN", "GesturingDown" },
                { "GESTURINGLEFT", "GesturingLeft" },
                { "GESTURINGRIGHT", "GesturingRight" },
                { "GESTURINGUP", "GesturingUp" },
                { "HEARING", "Hearing" },
                { "HIDING", "Hiding" },
                { "IDLINGLEVEL1", "IdlingLevel1" },
                { "IDLINGLEVEL2", "IdlingLevel2" },
                { "IDLINGLEVEL3", "IdlingLevel3" },
                { "LISTENING", "Listening" },
                { "MOVINGDOWN", "MovingDown" },
                { "MOVINGLEFT", "MovingLeft" },
                { "MOVINGRIGHT", "MovingRight" },
                { "MOVINGUP", "MovingUp" },
                { "SHOWING", "Showing" },
                { "SPEAKING", "Speaking" }
            };
            std::string name = state.name;
            if (builtins.count(name) == 1) {
                name = builtins.at(name);
            }
            out.begin_block("DefineState", name);
            for (auto name : state.animations) {
                if (animation_names.count(name) == 1) {
                    name = animation_names.at(name);
                }
                out.write("Animation", name);
            }
            out .end_block("EndState")
                .write_separator();
        }
    }

    void load(std::string const& filename) {
        std::ifstream file;
        file.open(filename, std::ios_base::binary | std::ios_base::in);
        if (file.fail()) {
            throw std::runtime_error("failed to open file");
        }
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> data(size);
        file.read((char *)&data[0], size);
        load((void *)&data[0], size);
    }

    void load(void *data, uint64_t bytes) {
        reader src { data, bytes };
        read_from(src);
    }
};

}
}
