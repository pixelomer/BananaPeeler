#pragma once

#include "readable.hpp"
#include <string>
#include <vector>

namespace banana {
namespace acs {

class state_info : public readable {
public:
    std::string name;
    std::vector<std::string> animations;

    void read_from(reader &src) override {
        src .read(name)
            .read_list<USHORT>(animations);
    }
};

}
}
