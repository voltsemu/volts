#pragma once

#include <cstdint>
#include <optional>

namespace volts::files
{
    enum class key_type : std::uint32_t
    {
        lvl0 = 1,
        lvl1 = 2,
        lvl2 = 3,
        app = 4,
        disk_img = 5,
        loader = 6,
        other = 7,
        npdrm = 8,
    };

    struct key
    {
        std::uint64_t version;
        std::uint16_t revision;
        key_type type;
        std::uint8_t erk[32];
        std::uint8_t riv[16];
        std::uint8_t pub[40];
        std::uint8_t priv[21];
        std::uint32_t curve;
    };

    namespace keys
    {
        static const std::uint8_t free_klic[] = 
        {
            0x72, 0xF9, 0x90, 0x78, 0x8F, 0x9C, 0xFF, 0x74, 0x57, 0x25, 0xF0, 0x8E, 0x4C, 0x12, 0x83, 0x87
        };

        std::optional<key> get_self_key(key_type type, std::uint16_t rev, std::uint64_t ver);
    }
}