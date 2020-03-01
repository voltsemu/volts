#pragma once

#include <wrapfs.h>

#include <string>
#include <map>
#include <variant>

namespace toml
{
    struct value
    {
        value();
        value(const value& other);
        value(value&& other);
        value(std::map<std::string, value> map);
        value(std::vector<value> vector);
        value(int_fast64_t integer);
        value(double number);
        value(bool boolean);
        value(tm time);

        value& operator=(std::map<std::string, value> map);
        value& operator=(std::vector<value> vector);
        value& operator=(int_fast64_t integer);
        value& operator=(double number);
        value& operator=(bool boolean);
        value& operator=(tm time);

        operator std::map<std::string, value>() const;
        operator std::vector<value>() const;
        operator int_fast64_t() const;
        operator double() const;
        operator bool() const;
        operator tm() const;

        value& operator[](int_fast64_t index) const;
        value& operator[](const std::string& key) const;
    private:
        std::variant<
            std::map<std::string, value>,
            std::vector<value>,
            int_fast64_t,
            double,
            bool,
            tm
        > data;
    };

    value load(const std::string& text);
    std::string dump(const value& data);

    value read(const fs::path& path);
    void save(const value& data, const fs::path& path);
}