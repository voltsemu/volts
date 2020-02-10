#include "self.h"

#include "elf.h"

#include "common.h"

namespace volts::crypt::self
{
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

    svl::file load(svl::file file, std::vector<byte> key)
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
}
