#include <svl/file.h>
#include <svl/platform.h>

namespace svl
{
    struct MemoryFile : FileHandle
    {
        virtual void seek(usize pos) override
        {
            if (cursor > data.size())
            {
                data.resize(cursor);
            }

            cursor = pos;
        }

        virtual usize tell() const override
        {
            return cursor;
        }

        virtual usize size() const override
        {
            return data.size();
        }

        virtual void read(byte* ptr, usize len) override
        {
            if (len + cursor > data.size())
            {
                data.resize(len + cursor);
            }

            memcpy(ptr, data.data() + cursor, len);
            cursor += len;
        }

        virtual void write(const byte* ptr, usize len) override
        {
            if (len + cursor > data.size())
            {
                data.resize(len + cursor);
            }

            memcpy(data.data() + cursor, ptr, len);
            cursor += len;
        }

        MemoryFile(std::vector<byte>&& in)
            : cursor(0)
            , data(std::move(in))
        { }

        usize cursor;
        std::vector<byte> data;
    };

    File stream(std::vector<byte>&& data)
    {
        return File(new MemoryFile(std::move(data)));
    }

    File buffer()
    {
        return File(new MemoryFile({}));
    }

#if SYS_WINDOWS
    struct Win32File : FileHandle
    {
        Win32File(const fs::path& path, Mode mode)
        {
            DWORD access = 0;
            DWORD disposition = 0;
        }

        HANDLE handle;
    };

    using NativeFile = Win32File;
#else
    struct PosixFile : FileHandle
    {
        PosixFile(const fs::path& path, Mode mode)
        {

        }

        int fd;
    };

    using NativeFile = PosixFile;
#endif

    File open(const fs::path& path, Mode mode)
    {
        return File(new NativeFile(path, mode));
    }
}