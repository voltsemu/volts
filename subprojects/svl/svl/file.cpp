#include "file.h"

#include <spdlog/spdlog.h>

#if SYS_WINDOWS
#   include "windows.h"
#elif SYS_OSX
#   include <copyfile.h>
#else
#   include <sys/sendfile.h>
#endif

#if !SYS_WINDOWS
#   include <unistd.h>
#   include <fcntl.h>
#endif

namespace svl
{

#if SYS_WINDOWS
    // win32 file handle wrapper
    struct win32_file : file_handle
    {
        virtual ~win32_file() override
        {
            CloseHandle(handle);
        }

        win32_file(const HANDLE&& h)
            : handle(h)
        {}

        virtual void seek(u64 pos) override
        {
            LARGE_INTEGER p;
            p.QuadPart = pos;

            SetFilePointerEx(handle, p, nullptr, FILE_BEGIN);
        }

        virtual u64 tell() const override
        {
            LARGE_INTEGER pos;
            pos.QuadPart = 0;
            SetFilePointerEx(handle, pos, &pos, FILE_CURRENT);

            return pos.QuadPart;
        }

        virtual u64 size() const override
        {
            LARGE_INTEGER len;
            GetFileSizeEx(handle, &len);
            return len.QuadPart;
        }

        virtual void read(void* out, u64 num) override
        {
            ReadFile(handle, out, (u32)num, nullptr, nullptr);
        }

        virtual void write(const void* in, u64 num) override
        {
            WriteFile(handle, in, num, nullptr, nullptr);
        }

        virtual void save(const fs::path& path) const override
        {
            // TODO: there is a native win32 way to do this
            // im just not sure how to get it to work
            auto cur = tell();
            auto len = size();

            // seek to front
            LARGE_INTEGER p = {};
            SetFilePointerEx(handle, p, nullptr, FILE_BEGIN);

            // read in data
            byte* buffer = new byte[len];
            ReadFile(handle, buffer, (u32)len, nullptr, nullptr);

            auto out = svl::open(path, svl::mode::write);
            out.write(buffer, len);

            delete[] buffer;

            // seek back to where we were so no state has changed
            p.QuadPart = cur;
            SetFilePointerEx(handle, p, nullptr, FILE_BEGIN);
        }

    private:
        const HANDLE handle;
    };
#else
    // posix file handle wrapper
    struct posix_file : file_handle
    {
        virtual ~posix_file() override
        {
            close(handle);
        }

        posix_file(int h)
            : handle(h)
        {}

        virtual void seek(u64 pos) override
        {
            lseek(handle, pos, SEEK_SET);
        }

        virtual u64 tell() const override
        {
            auto here = lseek(handle, 0, SEEK_CUR);
            return here;
        }

        virtual u64 size() const override
        {
            u64 cur = tell();
            lseek(handle, 0, SEEK_END);
            u64 len = tell();
            lseek(handle, cur, SEEK_SET);
            return len;
        }

        virtual void read(void* out, u64 num) override
        {
            auto val = ::read(handle, out, (size_t)num);
            (void)val;
        }

        virtual void write(const void* in, u64 num) override
        {
            auto out = ::write(handle, in, (size_t)num);
            (void)out;
        }

        virtual void save(const fs::path& path) const override
        {
            fsync(handle);
            int out = creat(path.c_str(), 0660);
#if SYS_OSX
            fcopyfile(handle, out, 0, COPYFILE_ALL);
#else
            sendfile(out, handle, 0, size());
#endif
            close(out);
        }

    private:
        int handle;
    };
#endif

    // in memory file handle wrapper
    struct ram_file : file_handle
    {
        ram_file(const std::vector<byte>& vec)
            : handle(vec)
        {
        }

        virtual void seek(u64 pos) override
        {
            cursor = pos;
            if(handle.max_size() + cursor < pos)
                handle.resize(pos);
        }

        virtual u64 size() const override
        {
            return handle.size();
        }

        virtual u64 tell() const override
        {
            return cursor;
        }

        virtual void read(void* out, u64 num) override
        {
            // if the amount of bytes to read is greater
            // than the amount of bytes left then only read
            // the number of bytes left
            // and zero the rest of the data
            if(cursor + num > handle.size())
                handle.resize(cursor + num);

            memcpy(out, handle.data() + cursor, num);
            cursor += num;
        }

        virtual void write(const void* in, u64 num) override
        {
            if(num + cursor > handle.size())
                handle.resize(cursor + num);

            memcpy(handle.data() + cursor, in, num);
            cursor += num;
        }

        virtual void save(const fs::path& path) const override
        {
            svl::open(path, mode::write).write(handle.data(), handle.size());
        }

    private:
        u64 cursor = 0;
        std::vector<byte> handle = {};
    };

    file open(const fs::path& path, u8 mo)
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
            return { new win32_file(nullptr) };
        }

        return { new win32_file(std::move(f)) };
#else
        int flags = 0;

        if(mo & mode::write)
            flags |= (O_CREAT | O_WRONLY);

        if(mo & mode::read)
            flags |= O_RDONLY;

        int fd = ::open(path.c_str(), flags);
        if(fd == -1)
        {
            spdlog::critical("failed to open file {} with errno {}", path.string(), errno);
        }

        return { new posix_file(fd) };
#endif
    }

    file from(const std::vector<svl::byte>& vec)
    {
        return file(new ram_file(vec));
    }

    file buffer()
    {
        return from({});
    }
}