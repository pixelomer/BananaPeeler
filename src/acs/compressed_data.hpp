#pragma once

#include "readable.hpp"
#include "decompressor.hpp"
#include <vector>

namespace banana {
namespace acs {

class compressed_data : public readable {
private:
    ULONG m_compressed_size;
    ULONG m_decompressed_size;
    std::vector<BYTE> m_data;
    ULONG data_size() {
        if (is_compressed()) {
            return m_compressed_size;
        }
        else {
            return m_decompressed_size;
        }
    }
public:
    bool is_compressed() {
        return m_compressed_size != 0;
    }
    ULONG decompressed_size() {
        return m_decompressed_size;
    }
    void read_from(reader &src) override {
        src .read(m_compressed_size)
            .read(m_decompressed_size)
            .read_list<BYTE>(m_data, data_size());
    }
    void set_image_data(bool compressed, USHORT width, USHORT height,
        std::vector<BYTE> const& data)
    {
        if (compressed) {
            m_compressed_size = data.size();
            m_decompressed_size = (((ULONG)width + 3) & 0xFC) * height;
        }
        else {
            m_compressed_size = 0;
            m_decompressed_size = data.size();
        }
        m_data = data;
    }
    std::vector<BYTE> const& get_raw_data() {
        return m_data;
    }
    std::vector<BYTE> decompress() {
        if (is_compressed()) {
            decompressor dec;
            return dec.decompress(m_data.data(), m_data.size(), 
                decompressed_size());
        }
        else {
            return m_data;
        }
    }
};

}
}
