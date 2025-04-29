#pragma once
#include "acd_writer.hpp"
#include "readable.hpp"

namespace banana {
namespace acs {

class guid : public readable {
public:
    ULONG data1;
    USHORT data2;
    USHORT data3;
    std::array<BYTE, 8> data4;

    void write_to(acd_writer &out) const {
        char guid[39];
        snprintf(guid, sizeof(guid), "{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            (unsigned int)((data1 >> 24) & 0xFF),
            (unsigned int)((data1 >> 16) & 0xFF),
            (unsigned int)((data1 >> 8) & 0xFF),
            (unsigned int)((data1 >> 0) & 0xFF),
            (unsigned int)((data2 >> 8) & 0xFF),
            (unsigned int)((data2 >> 0) & 0xFF),
            (unsigned int)((data3 >> 8) & 0xFF),
            (unsigned int)((data3 >> 0) & 0xFF),
            (unsigned int)data4[0],
            (unsigned int)data4[1],
            (unsigned int)data4[2],
            (unsigned int)data4[3],
            (unsigned int)data4[4],
            (unsigned int)data4[5],
            (unsigned int)data4[6],
            (unsigned int)data4[7]);
        guid[sizeof(guid)-1] = 0;
        out.write(acd_writer::unquoted_str(guid));
    }

    void read_from(reader &src) override {
        src .read(data1)
            .read(data2)
            .read(data3)
            .read_array(data4);
    }
};

}
}
