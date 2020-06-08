#include "svl/file.h"
#include "svl/platform.h"
#include "svl/macros.h"
#include "svl/logger.h"

#if !SYS_WINDOWS
#   include <unistd.h>
#   include <fcntl.h>
#endif

namespace svl
{
#if SYS_WINDOWS
    struct Win32File : FileHandle
    {
        Win32File(const fs::path &pth, Mode mode)
        {
            const wchar_t *path = pth.wstring().c_str();
            DWORD access = (mode == Mode::read) ? GENERIC_READ : GENERIC_WRITE;
            DWORD disp = (mode == Mode::read) ? OPEN_EXISTING : CREATE_ALWAYS;

            handle = CreateFileW(path, access, FILE_SHARE_READ, nullptr, disp, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (handle == INVALID_HANDLE_VALUE)
                svl::fatal("failed to open {} with error {}", pth, ::GetLastError());
        }

        virtual ~Win32File() override
        {
            CHECK(CloseHandle(handle));
        }

        virtual void read(void *out, usize len) override
        {
            CHECK(ReadFile(handle, out, (DWORD)len, nullptr, nullptr));
        }

        virtual void write(const void *in, usize len) override
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
            LARGE_INTEGER dest;
            dest.QuadPart = pos;
            CHECK(SetFilePointerEx(handle, dest, nullptr, FILE_BEGIN));
        }
    private:
        HANDLE handle;
    };
    using NativeFile = Win32File;
#else
    struct PosixFile : FileHandle
    {
        PosixFile(const fs::path& pth, Mode mode)
        {
            fd = ::open(pth.c_str(), mode == Mode::read ? O_RDONLY : (O_CREAT | O_WRONLY));
            CHECK(fd);
        }

        virtual ~PosixFile() override
        {
            CHECK(close(fd));
        }

        virtual void read(void *out, usize len) override
        {
            CHECK(::read(fd, out, len));
        }

        virtual void write(const void *in, usize len) override
        {
            CHECK(::write(fd, in, len));
        }

        virtual usize tell() const override
        {
            int pos = lseek(fd, 0, SEEK_CUR);
            CHECK(pos);
            return pos;
        }

        virtual usize size() const override
        {
            auto cur = tell();
            CHECK(lseek(fd, 0, SEEK_END));
            auto len = tell();
            CHECK(lseek(fd, cur, SEEK_SET));
            return len;
        }

        virtual void seek(usize pos) override
        {
            CHECK(lseek(fd, pos, SEEK_SET));
        }
    private:
        int fd;
    };
    using NativeFile = PosixFile;
#endif

    struct MemoryFile : FileHandle
    {
        MemoryFile(std::vector<svl::byte> &&bytes)
            : data(bytes)
            , cursor(0)
        { }

        virtual ~MemoryFile() override { }

        virtual void read(void *out, usize len) override
        {
            if (cursor + len > data.size())
                data.resize(cursor + len);

            memcpy(out, data.data() + cursor, len);
            cursor += len;
        }

        virtual void write(const void *in, usize len) override
        {
            if (cursor + len > data.size())
                data.resize(cursor + len);

            memcpy(data.data() + cursor, in, len);
            cursor += len;
        }

        virtual usize tell() const override
        {
            return cursor;
        }

        virtual usize size() const override
        {
            return data.size();
        }

        virtual void seek(usize pos) override
        {
            if (data.size() < pos)
                data.resize(pos);

            cursor = pos;
        }

    private:
        std::vector<svl::byte> data;
        usize cursor;
    };

    File open(const fs::path &path, Mode mode)
    {
        return File(new NativeFile(path, mode));
    }

    File stream(std::vector<svl::byte> &&bytes)
    {
        return File(new MemoryFile(std::move(bytes)));
    }
}