#include "sce.h"

#include "common.h"

#include <numeric>

namespace volts::crypt::sce
{
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

            std::memcpy(key, keys::scepkg_erk, sizeof(key));
            std::memcpy(iv, keys::scepkg_riv, sizeof(iv));

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
                    
                    std::memcpy(key, data_keys + sect.key_index * 16, 16);
                    std::memcpy(iv, data_keys + sect.iv_index * 16, 16);

                    stream.seek(sect.offset);
                    auto buffer = stream.read<byte>(sect.size);

                    aes_setkey_enc(&aes, key, 128);
                    aes_crypt_ctr(&aes, sect.size, &offset, iv, block, buffer.data(), buffer.data());

                    std::memcpy(data_buf + buffer_offset, buffer.data(), sect.size);
                }
                else
                {
                    stream.seek(sect.offset);
                    auto buffer = stream.read<byte>(sect.size);
                    std::memcpy(data_buf + buffer_offset, buffer.data(), sect.size);
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

                    file = svl::from({ });
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

    std::vector<svl::file> load(svl::file file)
    {
        sce_decrypter dec(file);
        
        if(!dec.load_headers() || !dec.load_metadata())
            return {};

        dec.decrypt();
        return dec.files();
    }
}