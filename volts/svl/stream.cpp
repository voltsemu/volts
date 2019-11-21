#include "stream.h"

namespace svl::streams
{
    std::vector<u8> read_n(std::istream& stream, std::streamsize n)
    {
        std::vector<u8> val(n);
        stream.read((char*)val.data(), n);
        return val;
    }

    namespace
    {
        unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    }

    void write_utf8(const fs::path& path, const std::string& str)
    {
        std::ofstream out(path.string());

        out.write((char*)bom, sizeof(bom));

        out << str;
    }
}