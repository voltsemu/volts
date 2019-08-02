#include "File.h"

#if OS_WINDOWS
#   include <fileapi.h>
#endif

namespace Volts
{
    FileHandle OpenFile(const Cthulhu::String& Path)
    {
#if OS_WINDOWS
        return CreateFileA(
            *Path,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
#elif OS_LINUX
        return open(*Path, O_RDWR | O_CREAT);
#elif OS_APPLE
        return fopen(*Path, "wb");
#endif
    }

    void CloseFile(FileHandle Handle)
    {
#if OS_WINDOWS
        FindClose(Handle);
#elif OS_LINUX
        close(Handle);
#elif OS_APPLE
        fclose(Handle);
#endif
    }

    void WriteFile(FileHandle Handle, Cthulhu::Byte* Data, Cthulhu::U32 Len)
    {
#if OS_WINDOWS
        ::WriteFile(
            Handle,
            Data,
            Len,
            nullptr,
            nullptr
        );
#elif OS_LINUX
        write(Handle, Data, Len);
#elif OS_APPLE
        fwrite(Data, Len, 1, Handle);
#endif
    }
}