#pragma once

#include "../types.hpp"
#include "readable.hpp"

namespace banana {
namespace acs {

class version : public readable {
public:
    USHORT minor;
    USHORT major;
    version() {}
    version(USHORT minor, USHORT major): minor(minor), major(major) {}

    void read_from(reader &src) override {
        src .read(minor)
            .read(major);
    }
};

}
}
