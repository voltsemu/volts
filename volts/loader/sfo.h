#pragma once

#include <svl/types.h>
#include <svl/file.h>
#include <svl/expected.h>

#include <cstdint>
#include <map>

namespace volts::loader::sfo
{
    /**
     * @brief the format of an sfo entry
     * 
     */
    enum class format : svl::u16
    {
        /// a null terminated string
        string = 516,

        /// an array of N bytes
        array = 4,

        /// a 4 byte wide signed integer
        integer = 1028,
    };

    /**
     * @brief an sfo value
     * 
     */
    struct value
    {
        /// the type of the entry
        format type;

        /// the raw bytes of the entry, can be cast as needed
        std::vector<svl::byte> data;

        template<typename T>
        T as() const 
        { 
            if constexpr(std::is_same_v<T, std::string>)
            {
                return (char*)data.data();
            }
            else if constexpr(std::is_integral_v<T> && sizeof(T) == 4)
            {
                return *(T*)data.data();
            }
        }
    };

    /// typealias for sfo object as its structure is simple
    using object = std::map<std::string, value>;

    /**
     * @brief parse an sfo value into a usable object
     * 
     * @param stream the file to parse
     * @return svl::expected<object> the parsed object if the file was valid
     */
    svl::expected<object> load(svl::file stream);

    void write(svl::file& stream, const object& obj);
}