#pragma once

#include "readable.hpp"
#include "locator.hpp"

namespace banana {
namespace acs {

class audio_info : public readable {
public:
    std::vector<BYTE> data;
    ULONG unknown;

    void read_from(reader &src) override {
        auto riff = src.read<locator>().get_subreader(src);
        auto data_len = riff.peek<ULONG>(4);
        riff.read_list(data, data_len + 8);
        src.read(unknown);
    }
};

}
}
