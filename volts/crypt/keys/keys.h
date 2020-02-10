#pragma once

#include <optional>

#include <types.h>

namespace volts::crypt
{
    /**
     * @brief the type of decryption key
     * 
     */
    enum class key_type : svl::u32
    {
        /// level 0 key
        lvl0 = 1,

        /// level 1 key
        lvl1 = 2,

        /// level 2 key
        lvl2 = 3,

        /// application key
        app = 4,

        /// disk key
        disk_img = 5,

        /// loader key
        loader = 6,

        /// other key
        other = 7,
        npdrm = 8,
    };

    /**
     * @brief a decryption key
     * 
     */
    struct key
    {
        /// key version
        svl::u64 version;

        /// key sub version
        svl::u16 revision;

        /// key type
        key_type type;

        /// key decryption erk
        svl::u8 erk[32];

        /// key decryption riv
        svl::u8 riv[16];

        /// public key
        svl::u8 pub[40];

        /// private key
        svl::u8 priv[21];

        /// decryption curve
        svl::u32 curve;
    };

    /**
     * @brief required decryption keys
     * 
     */
    namespace keys
    {
        /// decryption key for free games
        static const svl::u8 free_klic[] = {
            0x72, 0xF9, 0x90, 0x78, 0x8F, 0x9C, 0xFF, 0x74, 0x57, 0x25, 0xF0, 0x8E, 0x4C, 0x12, 0x83, 0x87
        };

        /// erk for sce packages
        static const svl::u8 scepkg_erk[] = { 
            0xA9, 0x78, 0x18, 0xBD, 0x19, 0x3A, 0x67, 0xA1, 0x6F, 0xE8, 0x3A, 0x85, 0x5E, 0x1B, 0xE9, 0xFB, 0x56, 0x40, 0x93, 0x8D,
            0x4D, 0xBC, 0xB2, 0xCB, 0x52, 0xC5, 0xA2, 0xF8, 0xB0, 0x2B, 0x10, 0x31 
        };

        // riv for sce packages
        static const svl::u8 scepkg_riv[] = { 
            0x4A, 0xCE, 0xF0, 0x12, 0x24, 0xFB, 0xEE, 0xDF, 0x82, 0x45, 0xF8, 0xFF, 0x10, 0x21, 0x1E, 0x6E 
        };

        /**
         * @brief get a self key based off requirements
         * 
         * @param type the type of key required
         * @param rev the revision of key required
         * @param ver the version of key required
         * @return std::optional<key> a key if one was found
         */
        std::optional<key> get_self_key(key_type type, svl::u16 rev, svl::u64 ver);
    }
}