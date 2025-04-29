#pragma once
#include "../types.hpp"
#include <vector>
#include <sstream>
#include <string>
#include <optional>

namespace banana {
namespace acs {

class acd_writer {
private:
    int m_indent = 0;
    std::ostringstream m_stream;

    void write_indent() {
        for (int i=0; i<m_indent; ++i) {
            m_stream << "\t";
        }
    }
public:
    struct hex {
        USHORT word;
        bool operator==(hex const& rhs) const { return word == rhs.word; }
        hex(USHORT word): word(word) {}
        hex() {}
    };
    struct unquoted_str {
        std::string value;
        bool operator==(unquoted_str const& rhs) const { return value == rhs.value; }
        unquoted_str(std::string const& value): value(value) {}
        unquoted_str() {}
    };
    typedef unquoted_str enum_name;

    template<typename T>
    static std::string itoa(T num) {
        char prefixed[22];
        snprintf(prefixed, 16, "%04ld", (int64_t)num);
        prefixed[sizeof(prefixed)-1] = 0;
        return std::string(prefixed);
    }

    template<typename T>
    acd_writer &begin_block(std::string const& header, T const& tag) {
        write_indent();
        m_stream << header << " ";
        write(tag);
        m_stream << std::endl;
        ++m_indent;
        return *this;
    }
    acd_writer &begin_block(std::string const& header) {
        write_indent();
        m_stream << header << std::endl;
        ++m_indent;
        return *this;
    }

    acd_writer &end_block(std::string const& footer) {
        --m_indent;
        write_indent();
        m_stream << footer << std::endl;
        return *this;
    }
    template<typename T>
    acd_writer &write(T const& value) {
        if constexpr (
            std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t> ||
            std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t> ||
            std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t> ||
            std::is_enum_v<T>)
        {
            m_stream << (int64_t)value;
        }
        else if constexpr (std::is_same_v<T, hex>) {
            char str[7];
            snprintf(str, sizeof(str), "0x%04hX", value.word);
            m_stream << str;
        }
        else if constexpr (std::is_same_v<T, unquoted_str>) {
            m_stream << value.value;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            m_stream << "\"" << value << "\"";
        }
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (size_t i=0; i<value.size(); ++i) {
                if (i != 0) m_stream << " | ";
                m_stream << value[i];
            }
            return *this;
        }
        else {
            value.write_to(*this);
        }
        return *this;
    }
    template<typename T>
    acd_writer &write(std::optional<T> const& value) {
        if (value.has_value()) {
            write(*value);
        }
        return *this;
    }
    template<typename T>
    acd_writer &write(std::string const& name, T const& value) {
        write_indent();
        m_stream << name << " = ";
        write(value);
        m_stream << std::endl;
        return *this;
    }
    template<typename T>
    acd_writer &write(std::string const& name, T const& value, T const& default_value) {
        if (!(value == default_value)) {
            write(name, value);
        }
        return *this;
    }
    template<typename T>
    acd_writer &write(std::string const& name, std::optional<T> const& value) {
        if (value.has_value()) {
            write(name, *value);
        }
        return *this;
    }
    acd_writer &write_empty_line(int n = 1) {
        for (int i=0; i<n; ++i) {
            m_stream << std::endl;
        }
        return *this;
    }
    acd_writer &write_separator() {
        m_stream << std::endl;
        m_stream << "//-------------------------------------------------------------------//" << std::endl;
        m_stream << std::endl;
        return *this;
    }
    std::string str() {
        std::string str = m_stream.str();
        // Remove trailing newline
        if (str[str.size()-1] == '\n') {
            str.resize(str.size()-1);
        }
        if (str[str.size()-1] == '\r') {
            str.resize(str.size()-1);
        }
        return str;
    }
    acd_writer() {
        m_stream << "//-------------------------------------------------------------------//" << std::endl;
        m_stream << "//" << std::endl;
        m_stream << "//	Microsoft Agent Character Description" << std::endl;
        m_stream << "//" << std::endl;
        m_stream << "//	Character Version 2.1" << std::endl;
        m_stream << "//	Copyright (c) 1996-98, Microsoft Corporation" << std::endl;
        m_stream << "//" << std::endl;
        m_stream << "//-------------------------------------------------------------------//" << std::endl;
        m_stream << std::endl;
    }
};

}
}
