#pragma once

#include "readable.hpp"

namespace banana {
namespace acs {

class size : public readable {
public:
    USHORT width;
    USHORT height;

    void read_from(reader &src) override {
        src .read(width)
            .read(height);
    }
};

}
}
