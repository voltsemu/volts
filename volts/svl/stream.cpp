#include "stream.h"

namespace svl::streams
{
    std::vector<std::uint8_t> read_n(std::istream& stream, std::streamsize n)
    {
        std::vector<std::uint8_t> val(n);
        stream.read((char*)val.data(), n);
        return std::move(val);
    }

    namespace
    {
        unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    }

    void write_utf8(fs::path path, const std::string& str)
    {
        std::ofstream out(path.string());

        out.write((char*)bom, sizeof(bom));

        out << str;
    }
}