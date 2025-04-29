#pragma once
#include <cstdint>
#include <stdexcept>
#include <cstdarg>

#define BIT_READER_LOGGING_ENABLED 0

namespace banana {
namespace acs {

class bit_reader {
private:
    const uint8_t *m_data;
    size_t m_len;
    size_t m_offset = 0;
    uint8_t m_bitidx = 0;

    #if BIT_READER_LOGGING_ENABLED
        uint8_t m_bit_count = 0;
    #endif

    void debug_log(const char *fmt, ...) {
        (void)fmt;
        #if BIT_READER_LOGGING_ENABLED
            va_list va;
            va_start(va, fmt);
            vfprintf(stderr, fmt, va);
            va_end(va);
        #endif
    }

    uint8_t _read_bit() {
        if (m_offset >= m_len) {
            throw std::runtime_error("out of range");
        }
        uint8_t bit = ((m_data[m_offset]) >> m_bitidx) & 1;
        ++m_bitidx;
        if (m_bitidx == 8) {
            m_bitidx = 0;
            ++m_offset;
        }
        return bit;
    }

    void log_bit(uint8_t bit) {
        (void)bit;
        #if BIT_READER_LOGGING_ENABLED
            debug_log("%hhu", bit);
            ++m_bit_count;
        #endif
    }
    void finalize_log() {
        #if BIT_READER_LOGGING_ENABLED
            do {
                debug_log(" ");
            }
            while (++m_bit_count < 22);
            debug_log(";");
            m_bit_count = 0;
        #endif
    }
public:
    bit_reader(): m_len(0) {}
    bit_reader(const uint8_t *data, size_t len): m_data(data), m_len(len) {}
    
    uint32_t read_bits(uint8_t n) {
        uint32_t data = 0;
        for (uint8_t i=0; i<n; ++i) {
            if (m_offset >= m_len) {
                throw std::runtime_error("out of range");
            }
            uint8_t bit = ((m_data[m_offset]) >> m_bitidx) & 1;
            log_bit(bit);
            ++m_bitidx;
            if (m_bitidx == 8) {
                m_bitidx = 0;
                ++m_offset;
            }
            data |= bit << i;
        }
        finalize_log();
        return data;
    }
    uint8_t read_bit() {
        uint8_t bit = _read_bit();
        log_bit(bit);
        finalize_log();
        return bit;
    }
    uint32_t read_ones(int max, uint8_t &count) {
        uint32_t data = 0;
        count = 0;
        while (_read_bit() == 1) {
            log_bit(1);
            data <<= 1;
            data |= 1;
            if (++count >= max) {
                break;
            }
        }
        if (count < max) {
            log_bit(0);
        }
        finalize_log();
        return data;
    }
    uint32_t read_ones(int max = -1) {
        uint8_t tmp;
        return read_ones(max, tmp);
    }
};

}
}
