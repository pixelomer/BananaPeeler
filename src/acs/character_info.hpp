#pragma once

#include "readable.hpp"
#include "locator.hpp"
#include "acd_writer.hpp"
#include "localized_info.hpp"
#include "guid.hpp"
#include "size.hpp"
#include "version.hpp"
#include "state_info.hpp"
#include "balloon_info.hpp"
#include "voice_info.hpp"
#include "tray_icon.hpp"
#include <stdexcept>
#include <vector>
#include <optional>

namespace banana {
namespace acs {

class character_info : public readable {
public:
    localized_info info;
    acs::guid guid;
    acs::size size;
    BYTE transparent_idx;
    ULONG flags;
    std::optional<acs::tray_icon> tray_icon;
    acs::version version;
    acs::version animation_version;
    std::optional<acs::balloon_info> balloon_info;
    std::vector<rgb_quad> color_table;
    std::optional<acs::voice_info> voice_info;
    std::vector<acs::state_info> animation_states;

    bool voice_enabled() const {
        // supposed to be bit 4 (?)
        return (flags >> 5) & 1;
    }

    bool balloon_enabled() const {
        bool enabled = (flags >> 9) & 1;
        bool disabled = (flags >> 8) & 1;
        if (enabled == disabled) {
            throw std::runtime_error("enabled == disabled");
        }
        return enabled;
    }

    bool size_to_text_enabled() const {
        return (flags >> 16) & 1;
    }

    bool auto_hide_enabled() const {
        return !((flags >> 17) & 1);
    }

    bool auto_pace_enabled() const {
        return !((flags >> 18) & 1);
    }

    bool standard_animation_set_supported() const {
        return (flags >> 20) & 1;
    }

    std::vector<std::string> style() const {
        std::vector<std::string> bits;
        if (voice_enabled()) {
            bits.push_back("AXS_VOICE_TTS");
        }
        bits.push_back("AXS_BALLOON_ROUNDRECT");
        if (size_to_text_enabled()) {
            bits.push_back("AXS_BALLOON_SIZETOTEXT");
        }
        return bits;
    }

    void write_to(acd_writer &out) const {
        out.begin_block("DefineCharacter")
            .write_empty_line()
            .write(info)
            .write_empty_line()
            .write("GUID", guid)
            .write("Icon", std::string("Images\\SystemTray.ico"))
            .write("Width", size.width)
            .write("Height", size.height)
            .write("Transparency", transparent_idx)
            .write("DefaultFrameDuration", 10)
            .write("Style", style())
            .write(voice_info)
            .write("ColorTable", std::string("Images\\ColorTable.bmp"))
            .write_empty_line()
            .end_block("EndCharacter")
            .write_separator();
        
        if (balloon_info.has_value()) {
            out .write(balloon_info)
                .write_separator();
        }
    }
    
    void read_from(reader &src) override {
        BOOL tray_icon_enabled;
        src .read(version)
            .read<locator>().deref(src, info)
            .read(guid)
            .read(size)
            .read(transparent_idx)
            .read(flags)
            .read(animation_version)
            .read(voice_info, voice_enabled())
            .read(balloon_info, balloon_enabled())
            .read_list<ULONG>(color_table)
            .read(tray_icon_enabled)
            .read(tray_icon, tray_icon_enabled)
            .read_list<USHORT>(animation_states);
    }
};

}
}
