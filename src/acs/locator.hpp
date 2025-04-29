#pragma once

#include "readable.hpp"
#include <vector>

namespace banana {
namespace acs {

class locator : public readable {
public:
    ULONG offset;
    ULONG size;
    template<typename Titem>
    Titem deref(reader &src) {
        return src.read_from<Titem>(offset, size);
    }
    template<typename Titem>
    reader &deref(reader &src, Titem &output) {
        output = deref<Titem>(src);
        return src;
    }
    template<typename Titem, typename Tcount>
    std::vector<Titem> deref_list(reader &src) {
        return src.read_list_from<Titem, Tcount>(offset, size);
    }
    template<typename Tcount, typename Titem>
    reader &deref_list(reader &src, std::vector<Titem> &output) {
        output = deref_list<Titem, Tcount>(src);
        return src;
    }
    void read_from(reader &src) override {
        offset = src.read<ULONG>();
        size = src.read<ULONG>();
    }
    reader get_subreader(reader &src) {
        return src.get_subreader(offset, size);
    }
};

}
}
