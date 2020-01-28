#include "unself.h"

#include "elf.h"

#include "keys/keys.h"

#include <external/aes/aes.h>

#include <zlib.h>

#include <spdlog/spdlog.h>

#include <numeric>

#include "svl/endian.h"
#include "svl/convert.h"
#include "svl/file.h"

namespace volts::loader::unself
{
    using svl::endian::big;

    using namespace svl;
    namespace cvt = svl::convert;

    namespace sce
    {
        struct header
        {
            u32 magic;
            big<u32> version;
            big<u16> type;
            big<u16> category;
            big<u32> metadata_start;
            big<u64> metadata_end;
            big<u64> size;
        };

        static_assert(sizeof(header) == 32);
    }

    namespace self
    {
        struct header
        {
            big<u64> type;
            big<u64> app_info_offset;
            big<u64> elf_offset;
            big<u64> prog_offset;
            big<u64> sect_offset;
            big<u64> sect_info_offset;
            big<u64> version_offset;
            big<u64> control_offset;
            big<u64> control_length;
            pad padding[8];  
        };

        static_assert(sizeof(header) == 80);

        struct control_info
        {
            u32 type;
            u64 size;
            u64 next;

            union
            {
                struct 
                {
                    big<u32> flags;
                    pad padding[28];
                } control_flags;

                static_assert(sizeof(control_flags) == 32);

                struct 
                {
                    u8 constant[20];
                    u8 elf_digest[20];
                    big<u64> required_version;
                } elf_digest_48;

                static_assert(sizeof(elf_digest_48) == 48);

                struct
                {
                    u8 const_or_digest[20];
                    pad padding[12];
                } elf_digest_32;

                static_assert(sizeof(elf_digest_32) == 32);

                struct 
                {
                    u32 magic;
                    big<u32> version;
                    big<u32> drm_type;
                    big<u32> app_type;
                    byte content_id[48];
                    byte digest[16];
                    byte inv_digest[16];
                    byte xor_digest[16];
                    pad padding[16];
                } npdrm_info;

                static_assert(sizeof(npdrm_info) == 128);
            };
        };
    }

    namespace metadata
    {
        struct info
        {
            byte key[16];
            pad key_pad[16];

            byte iv[16];
            pad iv_pad[16];
        };

        static_assert(sizeof(info) == 64);

        struct header
        {
            big<u64> signature_length;
            big<u32> algorithm;
            big<u32> sect_count;
            big<u32> key_count;
            big<u32> header_size;
            pad padding[8];
        };

        static_assert(sizeof(header) == 32);

        struct section
        {
            big<u64> offset;
            big<u64> size;
            big<u32> type;
            big<u32> index;
            big<u32> hash_algo;
            big<u32> hash_index;
            big<u32> encrypted;

            big<u32> key_index;
            big<u32> iv_index;

            big<u32> compressed;
        };

        static_assert(sizeof(section) == 48);
    }

    struct app_info
    {
        big<u64> auth;
        big<u32> vendor;
        big<u32> type;
        big<u64> version;
        pad padding[8];
    };

    static_assert(sizeof(app_info) == 32);

    struct self_decrypter
    {
        self_decrypter(svl::file file)
            : stream(file)
        {}

        bool load_headers()
        {
            sce_header = stream.read<sce::header>();

            if(sce_header.magic != cvt::to_u32("SCE\0"))
            {
                spdlog::warn("invalid sce magic");
                return false;
            }

            self_header = stream.read<self::header>();

            info = stream.read<app_info>();
            elf_header = stream.read<elf::header<u64>>();

            if(elf_header.magic != cvt::to_u32("\177ELF"))
            {
                spdlog::error("invalid elf magic");
                return false;
            }

            stream.seek(self_header.prog_offset.get());

            prog_headers = stream.read<elf::program_header<u64>>(elf_header.prog_count);

            stream.seek(self_header.sect_offset.get());

            sect_headers = stream.read<elf::section_header<u64>>(elf_header.sect_count);

            stream.seek(self_header.control_offset.get());

            int c = 0;
            while(c < self_header.control_length)
            {
                auto ctrl = read_control_info();
                c += ctrl.size;

                controls.push_back(ctrl);
            }

            return true;
        }

        bool load_metadata(const std::vector<byte>& key)
        {
            stream.seek(sce_header.metadata_start + sizeof(sce::header));

            auto meta_info = stream.read<metadata::info>();

            const int header_size = sce_header.metadata_end
                - sizeof(sce::header)
                - sce_header.metadata_start
                - sizeof(metadata::info);

            stream.seek(sce_header.metadata_start + sizeof(sce::header) + sizeof(metadata::info));

            headers = new u8[header_size];
            stream.read((char*)headers, header_size);

            aes_context aes;

            auto k = keys::get_self_key((key_type)info.type.get(), sce_header.type, info.version);

            if(!k)
            {
                spdlog::error("couldnt find key");
                return false;
            }

            if((sce_header.type & 0x8000) != 0x8000)
            {
                if(!decrypt_npdrm(cvt::as_bytes(meta_info), key))
                    return false;

                aes_setkey_dec(&aes, k->erk, 256);
                aes_crypt_cbc(
                    &aes, 
                    AES_DECRYPT, 
                    sizeof(metadata::info), 
                    k->riv, 
                    (byte*)&meta_info, 
                    (byte*)&meta_info
                );
            }

            for(int i = 0; i < 16; i++)
            {
                if(meta_info.key_pad[i] | meta_info.iv_pad[i])
                {
                    spdlog::error("failed to decrypt metadata info");
                    return false;
                }
            }

            size_t offset = 0;
            byte stream_buffer[16] = {};
            aes_setkey_enc(&aes, meta_info.key, 128);
            aes_crypt_ctr(
                &aes,
                header_size,
                &offset,
                meta_info.iv,
                stream_buffer,
                headers,
                headers
            );

            meta_header = *(metadata::header*)headers;

            data_keys_len = meta_header.key_count * 16;

            for(int i = 0; i < meta_header.sect_count; i++)
            {
                auto sect = *(metadata::section*)(headers + sizeof(metadata::header) + sizeof(metadata::section) * i);

                if(sect.encrypted == 3)
                    data_len += sect.size;

                meta_sections.push_back(sect);
            }

            data_keys = headers + sizeof(metadata::header) + meta_header.sect_count * sizeof(metadata::section);

            return true;
        }

        void decrypt()
        {
            aes_context aes;

            int buffer_offset = 0;

            data_buffer = new byte[data_len];

            for(auto sect : meta_sections)
            {
                if(sect.encrypted != 3)
                    continue;

                size_t offset = 0;

                byte key[16];
                byte iv[16];

                memcpy(key, data_keys + (sect.key_index * 16), 16);
                memcpy(iv, data_keys + (sect.iv_index * 16), 16);

                stream.seek(sect.offset.get());

                auto buf = stream.read<svl::byte>(sect.size);

                byte aes_stream[16] = {};

                aes_setkey_enc(&aes, key, 128);
                aes_crypt_ctr(
                    &aes,
                    sect.size,
                    &offset,
                    iv,
                    aes_stream,
                    buf.data(),
                    buf.data()
                );

                memcpy(data_buffer + buffer_offset, buf.data(), sect.size);

                buffer_offset += sect.size;
            }
        }

        svl::file elf()
        {
            svl::file out = svl::from({});

            out.write(elf_header);

            out.seek(elf_header.prog_offset.get());

            out.write(prog_headers);

            int buffer_offset = 0;

            for(auto& sect : meta_sections)
            {
                if(sect.type != 2)
                    continue;

                out.seek(prog_headers[sect.index].offset);

                if(sect.compressed == 2)
                {
                    spdlog::info("compressed section in self");

                    auto size = prog_headers[sect.index].file_size;

                    byte* dbuf = new byte[size]();
                    byte* zbuf = new byte[data_len];
                    memcpy(zbuf, data_buffer, data_len);

                    uLongf dbuf_len = static_cast<uLongf>(size);

                    int res = uncompress(dbuf, &dbuf_len, zbuf + buffer_offset, data_len);

                    if(res != Z_OK)
                    {
                        spdlog::warn("zlib broke {}", res);
                    }

                    out.write(dbuf, size);
                }
                else
                {
                    out.write(data_buffer + buffer_offset, sect.size);
                }

                buffer_offset += sect.size;
            }

            if(self_header.sect_info_offset)
            {
                out.seek(self_header.sect_info_offset.get());
                out.write(sect_headers);
            }

            return out;
        }

        ~self_decrypter()
        {
            delete[] headers;
        }

    private:
        bool decrypt_npdrm(std::vector<byte> data, std::vector<byte> meta_key)
        {
            auto ctrl = std::find_if(std::begin(controls), std::end(controls), [](auto& val) { return val.type == 3; });

            if(ctrl == std::end(controls))
                return true;

            byte key[16];

            switch(ctrl->npdrm_info.version)
            {
            case 1: 
                spdlog::error("cant decrypt network license");
                return false;
            case 2: 
                spdlog::error("cant decrypt local lisence yet");
                return false;
            case 3: 
                std::memcpy(key, meta_key.empty() ? meta_key.data() : keys::free_klic, sizeof(key));
                return true;
            default: 
                spdlog::error("invalid license type");
                return false;
            }
        }

        self::control_info read_control_info()
        {
            self::control_info ret;

            ret.type = endian::byte_swap(stream.read<u32>());
            ret.size = endian::byte_swap(stream.read<u32>());
            ret.next = endian::byte_swap(stream.read<u64>());

            if(ret.type == 1)
            {
                ret.control_flags = stream.read<decltype(self::control_info::control_flags)>();
            }
            else if(ret.type == 2)
            {
                if(ret.size == 48)
                {
                    ret.elf_digest_48 = stream.read<decltype(self::control_info::elf_digest_48)>();
                }
                else if(ret.size == 64)
                {
                    ret.elf_digest_32 = stream.read<decltype(self::control_info::elf_digest_32)>();
                }
            }
            else if(ret.type == 3)
            {
                ret.npdrm_info = stream.read<decltype(self::control_info::npdrm_info)>();
            }
            else
            {
                spdlog::error("invalid control info type");
            }

            return ret;
        }

        // headers
        sce::header sce_header;
        self::header self_header;
        elf::header<u64> elf_header;
        app_info info;

        std::vector<elf::program_header<u64>> prog_headers;
        std::vector<elf::section_header<u64>> sect_headers;

        // metadata
        metadata::header meta_header;
        std::vector<self::control_info> controls;
        
        byte* data_keys;
        u32 data_keys_len;
        
        std::vector<metadata::section> meta_sections;

        // output
        byte* data_buffer;
        u32 data_len = 0;

        // input
        svl::file stream;

        // extra stuff
        // this is freed when the object is deleted
        // make sure there are no memory leaks
        u8* headers = nullptr;
    };

    svl::file load_self(svl::file file, std::vector<byte> key)
    {
        // check for debug self
        file.seek(8);
        if(auto version = file.read<endian::little<u16>>(); version == 0x80 || version == 0xC0)
        {
            spdlog::info("debug self");
        }
        else
        {  
            spdlog::info("release self");
        }

        // check for 32/64 bit elf
        file.seek(sizeof(sce::header));
        auto selfhead = file.read<self::header>();

        file.seek(selfhead.elf_offset);

        auto cls = file.read<u8>(8);

        if(cls[4] == 1)
        {
            spdlog::info("32 bit");
        }
        else
        {
            spdlog::info("64 bit");
        }

        file.seek(0);

        self_decrypter dec(file);

        if(!dec.load_headers())
        {
            return {};
        }

        if(!dec.load_metadata(key))
        {
            return {};
        }

        dec.decrypt();

        return dec.elf();
    }

    struct sce_decrypter 
    {
        svl::file stream;
        
        sce_decrypter(svl::file s)
            : stream(s)
        {}

        sce::header sce_head;

        std::vector<metadata::section> meta_sections;

        u32 data_length = 0;
        byte* data_keys = nullptr;

        u32 buf_len = 0;
        byte* data_buf = nullptr;

        bool load_headers()
        {
            sce_head = stream.read<sce::header>();

            if(sce_head.magic != cvt::to_u32("SCE\0"))
            {
                spdlog::error("invalid sce header");
                return false;
            }

            return true;
        }

        bool load_metadata()
        {
            const auto metadata_size = sce_head.metadata_end - (sizeof(sce::header) + sce_head.metadata_start + sizeof(metadata::info));
            stream.seek(sce_head.metadata_start + sizeof(sce::header));
            auto meta_info_vec = stream.read<byte>(sizeof(metadata::info));
            auto meta_head_vec = stream.read<byte>(metadata_size);

            byte key[32];
            byte iv[16];

            memcpy(key, keys::scepkg_erk, sizeof(key));
            memcpy(iv, keys::scepkg_riv, sizeof(iv));

            aes_context aes;

            if((sce_head.type & 0x8000) != 0x8000)
            {
                aes_setkey_dec(&aes, key, 256);
                aes_crypt_cbc(&aes, AES_DECRYPT, sizeof(metadata::info), iv, meta_info_vec.data(), meta_info_vec.data());
            }

            auto meta = *(metadata::info*)meta_info_vec.data();

            for(int i = 0; i < 16; i++)
            {
                if(meta.key_pad[i] | meta.iv_pad[i])
                {
                    spdlog::error("failed to decrypt metadata info");
                    return false;
                }
            }

            size_t offset = 0;
            byte block[16];
            aes_setkey_enc(&aes, meta.key, 128);
            aes_crypt_ctr(&aes, metadata_size, &offset, meta.iv, block, meta_head_vec.data(), meta_head_vec.data());

            auto meta_head = *(metadata::header*)meta_head_vec.data();

            for(int i = 0; i < meta_head.sect_count; i++)
            {
                meta_sections.push_back(*(metadata::section*)(meta_head_vec.data() + sizeof(metadata::header) + sizeof(metadata::section) * i));
            }

            data_length = meta_head.key_count * 16;
            data_keys = new byte[data_length];
            memcpy(data_keys, meta_head_vec.data() + sizeof(metadata::header) + meta_head.sect_count * sizeof(metadata::section), data_length);

            return true;
        }

        void decrypt()
        {
            aes_context aes;

            buf_len = std::reduce(meta_sections.begin(), meta_sections.end(), 0ull, [](auto l, auto r) {
                return l + r.size;
            });

            data_buf = new byte[buf_len];

            u32 buffer_offset = 0;

            byte key[16];
            byte iv[16];

            for(const auto& sect : meta_sections)
            {
                if(sect.encrypted == 3)
                {
                    size_t offset = 0;
                    byte block[16] = {};
                    
                    memcpy(key, data_keys + sect.key_index * 16, 16);
                    memcpy(iv, data_keys + sect.iv_index * 16, 16);

                    stream.seek(sect.offset);
                    auto buffer = stream.read<byte>(sect.size);

                    aes_setkey_enc(&aes, key, 128);
                    aes_crypt_ctr(&aes, sect.size, &offset, iv, block, buffer.data(), buffer.data());

                    memcpy(data_buf + buffer_offset, buffer.data(), sect.size);
                }
                else
                {
                    stream.seek(sect.offset);
                    auto buffer = stream.read<byte>(sect.size);
                    memcpy(data_buf + buffer_offset, buffer.data(), sect.size);
                }

                buffer_offset += sect.size;
            }
        }

        std::vector<svl::file> files()
        {
            std::vector<svl::file> out;

            u32 data_offset = 0;

            for(auto& sect : meta_sections)
            {
                svl::file file;

                if(sect.compressed == 2)
                {
                    const size_t size = 32 * 1024;
                    byte buf[size];

                    z_stream zstream;
                    zstream.zalloc = Z_NULL,
                    zstream.zfree = Z_NULL,
                    zstream.opaque = Z_NULL,
                    zstream.avail_in = sect.size,
                    zstream.avail_out = size,
                    zstream.next_in = data_buf + data_offset,
                    zstream.next_out = buf;
                    
                    int ret = inflateInit(&zstream);

                    file = svl::from({ 0 });
                    file.seek(0);

                    while(zstream.avail_in)
                    {
                        ret = inflate(&zstream, Z_NO_FLUSH);

                        if(ret == Z_STREAM_END)
                            break;

                        if(ret != Z_OK)
                            spdlog::info("file inflation failed");

                        if(!zstream.avail_out)
                        {
                            file.write(buf, size);
                            zstream.next_out = buf;
                            zstream.avail_out = size;
                        }
                        else
                        {
                            break;
                        }
                    }

                    int res = Z_OK;
                    res = inflate(&zstream, Z_FINISH);

                    if(res != Z_STREAM_END)
                        spdlog::info("zstream result was invalid");

                    file.write(buf, size);
                    inflateEnd(&zstream);
                }
                else
                {
                    file = svl::from({ data_buf, data_buf + data_offset });
                }

                out.push_back(std::move(file));

                data_offset += sect.size;
            }

            return out;
        }
        
        ~sce_decrypter()
        {
            delete[] data_keys;
            delete[] data_buf;
        }
    };

    std::vector<svl::file> load_sce(svl::file file)
    {
        sce_decrypter dec(file);
        
        if(!dec.load_headers())
        {
            return {};
        }

        if(!dec.load_metadata())
        {
            return {};
        }

        dec.decrypt();
        return dec.files();
    }
}
