#include "svl/file.h"
#include "svl/platform.h"
#include "svl/macros.h"
#include "svl/logger.h"

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
            CHECK(CloseHandle(handle));
        }

        virtual void read(void* out, usize len) override
        {
            CHECK(ReadFile(handle, out, (DWORD)len, nullptr, nullptr));
        }

        virtual void write(const void* in, usize len) override
        {
            CHECK(WriteFile(handle, in, (DWORD)len, nullptr, nullptr));
        }

        virtual usize tell() const override
        {
            LARGE_INTEGER pos = {};
            CHECK(SetFilePointerEx(handle, pos, &pos, FILE_CURRENT));
            return pos.QuadPart;
        }

        virtual usize size() const override
        {
            LARGE_INTEGER len = {};
            CHECK(GetFileSizeEx(handle, &len));
            return len.QuadPart;
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
            fd = ::open(pth.c_str(), 0);
            CHECK(fd);
        }

        virtual ~PosixFile() override
        {
            CHECK(close(fd));
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