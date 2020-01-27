#include "file.h"

#include <spdlog/spdlog.h>

namespace svl
{
    file open(const std::filesystem::path& path, u8 mo)
    {
#if SYS_WINDOWS
    DWORD access = 0;
    DWORD disp = 0;

    if(mo & mode::write)
    {
        access |= GENERIC_WRITE;
        disp |= CREATE_ALWAYS;
    }

    if(mo & mode::read)
    {
        access |= GENERIC_READ;
        disp |= OPEN_EXISTING;
    }

    const HANDLE f = CreateFileW(
        path.wstring().c_str(), 
        access,
        FILE_SHARE_READ,
        nullptr,
        disp,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if(f == INVALID_HANDLE_VALUE)
    {
        spdlog::error("failed to open file");
    }

    return { new win32_file(std::move(f)) };
#else
    std::string access;

    if(m & write)
        access += "w";

    if(m & read)
        access += "r";

    std::FILE* f = std::fopen(path.c_str(), access);
    return { new posix_file(f) };
#endif
    }

    file from(std::vector<svl::byte> vec)
    {
        return file(new ram_file(vec));
    }
}