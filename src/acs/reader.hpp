#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <array>
#include "../types.hpp"
#include <type_traits>
#include <optional>

namespace banana {
namespace acs {

class reader {
private:
    void *m_root_data;
    uint64_t m_root_size;
    void *m_data;
    uint64_t m_size;
    uint64_t m_offset;
    int m_level;
public:
    reader(void *root_data, uint64_t root_size): m_root_data(root_data),
        m_root_size(root_size), m_data(root_data), m_size(root_size),
        m_offset(0), m_level(0) {}
    reader(void *root_data, uint64_t root_size, uint64_t root_offset, uint64_t size, int level):
        m_root_data(root_data), m_root_size(root_size),
        m_data((void *)((uint8_t *)root_data + root_offset)),
        m_size(size), m_offset(0), m_level(level) {}
    void *data() {
        return m_data;
    }
    uint64_t size() {
        return m_size;
    }
    uint64_t offset() {
        return m_offset;
    }
    void reset(uint64_t offset) {
        if (offset > m_size) {
            throw std::range_error("out of bounds");
        }
        m_offset = offset;
    }
    void seek(uint64_t offset) {
        if ((m_offset + offset) > m_size) {
            throw std::range_error("out of bounds");
        }
        m_offset += offset;
    }
    template<typename T>
    T read() {
        if constexpr (
            std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t> ||
            std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t> ||
            std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t> ||
            std::is_enum_v<T>)
        {
            uint64_t new_offset = m_offset + sizeof(T);
            if (new_offset > m_size) {
                throw std::range_error("out of bounds");
            }
            uint8_t *buf = (uint8_t *)m_data + m_offset;
            uint32_t value = buf[0];
            if constexpr (sizeof(T) >= 2) {
                value |= ((uint32_t)buf[1] << 8);
            }
            if constexpr (sizeof(T) >= 4) {
                value |= ((uint32_t)buf[2] << 16);
                value |= ((uint32_t)buf[3] << 24);
            }
            m_offset = new_offset;
            return (T)value;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            ULONG count = read<ULONG>();
            if (count == 0) {
                return "";
            }
            std::vector<WCHAR> data = read_list<WCHAR>(count + 1);
            const int BUF_SIZE = 7;
            if (MB_CUR_MAX >= BUF_SIZE) {
                throw std::runtime_error("MB_CUR_MAX >= BUF_SIZE");
            }
            std::string value;
            std::wctomb(nullptr, 0);
            for (const WCHAR wc : data) {
                std::array<char, BUF_SIZE> buf;
                const int ret = std::wctomb(buf.data(), (wchar_t)wc);
                buf[ret] = '\0';
                value += std::string(buf.data());
            }
            return value;
        }
        else {
            T result;
            result.read_from(*this);
            return result;
        }
    }
    template<typename T>
    T peek(uint64_t offset = 0) {
        uint64_t start_offset = m_offset;
        m_offset += offset;
        T data = read<T>();
        m_offset = start_offset;
        return data;
    }
    template<typename T>
    reader &read(std::optional<T> &output) {
        output = read<T>();
        return *this;
    }
    template<typename T>
    reader &read(T &output) {
        output = read<T>();
        return *this;
    }
    template<typename T>
    reader &read(std::optional<T> &output, bool condition) {
        if (condition) {
            output = read<T>();
        }
        return *this;
    }
    template<typename T>
    T read_from(uint64_t root_offset, uint64_t size) {
        return get_subreader(root_offset, size).read<T>();
    }
    template<typename Titem, typename Tcount>
    std::vector<Titem> read_list() {
        std::vector<Titem> list;
        Tcount count = read<Tcount>();
        for (Tcount i=0; i<count; ++i) {
            list.push_back(read<Titem>());
        }
        return list;
    }
    template<typename Tcount, typename Titem>
    reader &read_list(std::vector<Titem> &output) {
        output = read_list<Titem, Tcount>();
        return *this;
    }
    template<typename Titem>
    std::vector<Titem> read_list(ssize_t count) {
        std::vector<Titem> list;
        for (ssize_t i=0; i<count; ++i) {
            list.push_back(read<Titem>());
        }
        return list;
    }
    template<typename Titem>
    reader &read_list(std::vector<Titem> &output, ssize_t count) {
        output = read_list<Titem>(count);
        return *this;
    }
    template<typename Titem, std::size_t count>
    std::array<Titem, count> read_array() {
        std::array<Titem, count> array;
        for (std::size_t i=0; i<count; ++i) {
            array[i] = read<Titem>();
        }
        return array;
    }
    template<std::size_t count, typename Titem>
    reader &read_array(std::array<Titem, count> &output) {
        output = read_array<Titem, count>();
        return *this;
    }
    reader get_subreader(uint64_t root_offset, uint64_t size) {
        return { m_root_data, m_root_size,
            root_offset, size, m_level + 1 };
    }
    template<typename Titem, typename Tcount>
    std::vector<Titem> read_list_from(uint64_t root_offset, uint64_t size) {
        return get_subreader(root_offset, size).read_list<Titem, Tcount>();
    }
};

}
}
