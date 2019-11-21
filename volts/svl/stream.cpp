#include "stream.h"

namespace svl
{
    memstream::memstream() {}
    memstream::memstream(const std::vector<svl::byte>& o)
    {
        vec.resize(o.size());
        memcpy(vec.data(), o.data(), o.size());
    }

    void memstream::seek(std::size_t len)
    {
        cursor = len;

        if(cursor > vec.size())
            vec.resize(cursor);
    }

    std::size_t memstream::tell() const
    {
        return cursor;
    }

    std::size_t memstream::size() const
    {
        return vec.size();
    }

    void memstream::read(void* ptr, std::size_t len)
    {
        if(cursor + len > size())
            vec.resize(cursor + len);

        memcpy(ptr, vec.data() + cursor, len);
        seek(cursor + len);
    }

    void memstream::write(void* ptr, std::size_t len)
    {
        if(cursor + len > size())
            vec.resize(cursor + len);

        memcpy(vec.data() + cursor, ptr, len);
        seek(cursor + len);
    }

    std::vector<svl::byte> memstream::take_vector()
    {
        return vec;
    }

    std::string mode_to_string(std::ios_base::openmode mode)
    {
        std::string ret;

        ret += (mode & std::ios::in) ? "r" : "w";

        if(mode & std::ios::binary)
            ret += "b";

        return ret;
    }

    fstream::fstream(const std::filesystem::path& path, std::ios_base::openmode mode)
        : file(std::fopen(path.string().c_str(), mode_to_string(mode).c_str()))
    {}

    fstream::fstream(fstream&& o)
        : file(o.file)
    {
        o.file = nullptr;
    }

    fstream::~fstream()
    {
        if(file)
            std::fclose(file);
    }

    void fstream::seek(std::size_t len)
    {
        std::fseek(file, len, SEEK_SET);
    }

    std::size_t fstream::tell() const
    {
        return std::ftell(file);
    }

    std::size_t fstream::size() const
    {
        auto pos = tell();
        std::fseek(file, 0, SEEK_END);
        auto s = tell();
        std::fseek(file, pos, SEEK_SET);
        return s;
    }

    void fstream::read(void* ptr, std::size_t len)
    {
        std::fread(ptr, 1, len, file);
    }

    void fstream::write(void* ptr, std::size_t len)
    {
        std::fwrite(ptr, 1, len, file);
    }

    namespace
    {
        unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    }

    void write_utf8(const std::filesystem::path& path, const std::string& str)
    {
        fstream out(path.string());

        out.write((void*)bom, sizeof(bom));

        out.write((void*)str.data(), str.size());
    }
}