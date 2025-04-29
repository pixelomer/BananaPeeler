#pragma once

#include "readable.hpp"

namespace banana {
namespace acs {

class region_data : public readable {
public:
    void read_from(reader &src) override {
        (void)src;
        //TODO
    }
};

}
}
