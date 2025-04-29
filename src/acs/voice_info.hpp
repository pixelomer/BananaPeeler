#pragma once

#include <optional>
#include "acd_writer.hpp"
#include "readable.hpp"
#include "voice_info_extra.hpp"
#include "guid.hpp"

namespace banana {
namespace acs {

class voice_info : public readable {
public:
    guid tts_engine_id;
    guid tts_mode_id;
    ULONG speed;
    USHORT pitch;
    std::optional<voice_info_extra> extra;

    void write_to(acd_writer &out) const {
        out .write("TTSEngineID", tts_engine_id)
            .write("TTSModeID", tts_mode_id)
            .write(extra);
    }

    void read_from(reader &src) override {
        BOOL has_extra;
        src .read(tts_engine_id)
            .read(tts_mode_id)
            .read(speed)
            .read(pitch)
            .read(has_extra);
        if (has_extra) {
            extra = src.read<voice_info_extra>();
        }
    }
};

}
}
