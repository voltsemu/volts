#include "svl/file.h"
#include "svl/platform.h"


#if SYS_WINDOWS
#   include <fileapi.h>
#   include <handleapi.h>
#else
#   include <unistd.h>
#endif

namespace svl
{

#if SYS_WINDOWS
    struct Win32File : FileHandle
    {
        Win32File(const fs::path& pth)
        {
            const wchar_t* path = pth.wstring().c_str();
        }

        virtual ~Win32File() override
        {
            CloseHandle(handle);
        }

        virtual void read(void* out, usize len) override
        {

        }

        virtual void write(const void* in, usize len) override
        {

        }

        virtual usize tell() const override
        {

        }

        virtual usize size() const override
        {

        }

        virtual void seek(usize pos) override
        {

        }
    private:
        HANDLE handle;
    };
    using NativeFile = Win32File;
#else
    struct PosixFile : FileHandle
    {
        PosixFile(const fs::path& pth)
        {

        }

        virtual ~PosixFile() override
        {
            close(fd);
        }

        virtual void read(void* out, usize len) override
        {

        }

        virtual void write(const void* in, usize len) override
        {

        }

        virtual usize tell() const override
        {

        }

        virtual usize size() const override
        {

        }

        virtual void seek(usize pos) override
        {

        }
    private:
        int fd;
    };
    using NativeFile = PosixFile;
#endif

    struct MemoryFile : FileHandle
    {

    };

    File open(const fs::path& path)
    {
        return File(new NativeFile(path));
    }
}