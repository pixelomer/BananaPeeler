#pragma once

#include "reader.hpp"

namespace banana {
namespace acs {

class readable {
public:
    virtual void read_from(reader &src) = 0;
};

}
}
