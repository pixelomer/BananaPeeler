#pragma once
#include "../types.hpp"
#include <stdexcept>
#include <vector>
#include "bit_reader.hpp"
#include <stdarg.h>

namespace banana {
namespace acs {

class decompressor {
private:
    bit_reader m_in;
    bool m_end;
    std::vector<BYTE> m_out;
    size_t m_outoffset;

    void debug_log(const char *fmt, ...) {
        (void)fmt;
        #if BIT_READER_LOGGING_ENABLED
            va_list va;
            va_start(va, fmt);
            vfprintf(stderr, fmt, va);
            va_end(va);
        #endif
    }

    void put_byte(BYTE byte) {
        if (m_outoffset >= m_out.size()) {
            m_out.resize(m_outoffset + 0x800);
        }
        m_out[m_outoffset++] = byte;
    }

    void process_sequence() {
        bool compressed = m_in.read_bit();
        if (compressed) {
            debug_log("compressed byte range\n");
            size_t bytes = 2;
            uint32_t seq_bits = m_in.read_ones(3);
            uint32_t copy_offset;
            if (seq_bits == 0x0) {
                debug_log("offset bit count = 6, decoded BYTE count = 2\n");
                copy_offset = m_in.read_bits(6) + 0x1;
                debug_log("offset = %u byte\n", copy_offset);
            }
            else if (seq_bits == 0x1) {
                debug_log("offset bit count = 9, decoded BYTE count = 2\n");
                copy_offset = m_in.read_bits(9) + 0x41;
                debug_log("offset = %u byte\n", copy_offset);
            }
            else if (seq_bits == 0x3) {
                debug_log("offset bit count = 12, decoded BYTE count = 2\n");
                copy_offset = m_in.read_bits(12) + 0x241;
                debug_log("offset = %u byte\n", copy_offset);
            }
            else /* if (seq_bits == 0x7) */ {
                debug_log("offset bit count = 20, decoded BYTE count = 3\n");
                bytes += 1;
                copy_offset = m_in.read_bits(20);
                if (copy_offset == 0xFFFFF) {
                    debug_log("0x000FFFFF (end of bit stream)\n");
                    m_end = true;
                    return;
                }
                copy_offset += 0x1241;
                debug_log("offset = %u byte\n", copy_offset);
            }
            uint8_t extra_bit_count;
            uint32_t extra_bytes = m_in.read_ones(12, extra_bit_count);
            if (extra_bit_count == 12) {
                throw std::runtime_error("12th bit cannot be 1");
            }
            debug_log("increment decoded count by %u\n", extra_bytes);
            bytes += extra_bytes;
            extra_bytes = m_in.read_bits(extra_bit_count);
            debug_log("increment decoded count by %u\n", extra_bytes);
            bytes += extra_bytes;
            if (copy_offset > m_outoffset) {
                throw std::runtime_error("negative offset");
            }
            for (uint32_t i=0; i<bytes; ++i) {
                put_byte(m_out[m_outoffset - copy_offset]);
            }
        }
        else {
            debug_log("uncompressed byte\n");
            BYTE byte = m_in.read_bits(8);
            debug_log("byte 0x%hhX\n", byte);
            put_byte(byte);
        }
    }
public:
    decompressor() {}

    std::vector<BYTE> decompress(const uint8_t *data, size_t len, size_t decompressed_size) {
        m_in = { data, len };
        if (m_in.read_bits(8) != 0x0) {
            throw std::runtime_error("expected initial 0x00");
        }
        debug_log("initial byte 0x00\n\n");
        m_out = {};
        m_outoffset = 0;
        m_out.resize(decompressed_size);
        m_end = false;
        while (!m_end) {
            process_sequence();
            debug_log("\n");
        }
        m_in = {};
        m_out.resize(m_outoffset);
        m_out.shrink_to_fit();
        return std::move(m_out);
    }
};

}
}
