#include <svl/file.h>
#include <svl/platform.h>
#include <svl/panic.h>

#if !SYS_WINDOWS
#   include <unistd.h>
#   include <fcntl.h>
#endif

#if SYS_OSX
#   include <copyfile.h>
#elif SYS_LINUX
#   include <sys/sendfile.h>
#endif

namespace svl
{
    struct MemoryFile : FileHandle
    {
        virtual ~MemoryFile() override { }

        virtual void seek(usize pos) override
        {
            if (pos > data.size())
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

        virtual void save(const fs::path& path) const override
        {
            auto out = svl::open(path, Mode::write);
            out.write(data.data(), data.size());
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
        virtual ~Win32File() override
        {
            BOOL err = CloseHandle(handle);

            if (err == 0)
            {
                // TODO: check error
            }
        }

        virtual void seek(usize off) override
        {
            LARGE_INTEGER pos;
            pos.QuadPart = off;

            SetFilePointerEx(handle, pos, nullptr, FILE_BEGIN);
        }

        virtual usize tell() const override
        {
            LARGE_INTEGER off = {};
            SetFilePointerEx(handle, off, &off, FILE_CURRENT);
            return off.QuadPart;
        }

        virtual usize size() const override
        {
            LARGE_INTEGER len;
            GetFileSizeEx(handle, &len);
            return len.QuadPart;
        }

        virtual void read(byte* ptr, usize len) override
        {
            ReadFile(handle, ptr, (DWORD)len, nullptr, nullptr);
        }

        virtual void write(const byte* ptr, usize len) override
        {
            WriteFile(handle, ptr, (DWORD)len, nullptr, nullptr);
        }

        virtual void save(const fs::path& path) const override
        {
            // TODO: clean this up and use the proper win32 file copying stuff
            const usize off = tell();
            const usize len = size();

            LARGE_INTEGER pos = {};
            SetFilePointerEx(handle, pos, nullptr, FILE_BEGIN);

            byte* buffer = new byte[len];
            ReadFile(handle, buffer, (DWORD)len, nullptr, nullptr);

            auto out = svl::open(path, Mode::write);
            out.write(buffer, len);

            delete[] buffer;

            pos.QuadPart = off;
            SetFilePointerEx(handle, pos, nullptr, FILE_BEGIN);
        }

        Win32File(const fs::path& path, Mode mode)
        {
            DWORD access = 0;
            DWORD disp = 0;

            switch (mode)
            {
            case Mode::read:
                access |= GENERIC_READ;
                disp |= OPEN_EXISTING;
                break;
            case Mode::write:
                access |= GENERIC_WRITE;
                disp |= CREATE_ALWAYS;
                break;
            default:
                svl::panic("[E0003] Invalid file mode");
            }

            handle = CreateFileW(
                path.wstring().c_str(),
                access,
                FILE_SHARE_READ,
                nullptr,
                disp,
                FILE_ATTRIBUTE_NORMAL,
                nullptr
            );

            if (handle == INVALID_HANDLE_VALUE)
            {
                svl::panic("[E0004] Invalid file handle");
            }
        }

    private:
        HANDLE handle;
    };

    using NativeFile = Win32File;
#else
    struct PosixFile : FileHandle
    {
        virtual ~PosixFile() override
        {
            ::close(fd);
        }

        virtual void seek(usize off) override
        {
            ::lseek(fd, off, SEEK_SET);
        }

        virtual usize tell() const override
        {
            return ::lseek(fd, 0, SEEK_CUR);
        }

        virtual usize size() const override
        {
            const usize pos = tell();
            ::lseek(fd, 0, SEEK_END);
            const usize off = tell();
            ::lseek(fd, pos, SEEK_SET);
            return off;
        }

        virtual void read(byte* ptr, usize len) override
        {
            const int err = ::read(fd, ptr, len);

            if (err == 0 && errno != 0)
            {
                svl::panic("[E0005] Error reading from file {}", strerror(errno));
            }
        }

        virtual void write(const byte* ptr, usize len) override
        {
            const int err = ::write(fd, ptr, len);

            if (err == 0 && errno != 0)
            {
                svl::panic("[E0006] Error writing to file {}", strerror(errno));
            }
        }

        virtual void save(const fs::path& path) const override
        {
            // TODO: check err
            const int err = ::fsync(fd);
            const int out = ::creat(path.c_str(), 0660);

            if (out == -1)
            {
                svl::panic("[E0004] Invalid file handle");
            }

#if SYS_OSX
            fcopyfile(fd, out, 0, COPYFILE_ALL);
#else
            sendfile(out, fd, 0, size());
#endif
            ::close(fd);
        }

        PosixFile(const fs::path& path, Mode mode)
        {
            int flags = 0;

            switch (mode)
            {
            case Mode::read:
                flags |= O_RDONLY;
                break;
            case Mode::write:
                flags |= (O_CREAT | O_WRONLY);
                break;
            default:
                svl::panic("[E0003] Invalid file mode");
            }

            fd = ::open(path.c_str(), flags);

            if (fd == -1)
            {
                svl::panic("[E0004] Invalid file handle");
            }
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