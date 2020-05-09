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
            FindClose(handle);
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
            FlushFileBuffers(handle);

            WCHAR buf[MAX_PATH];
            GetFinalPathNameByHandleW(handle, buf, MAX_PATH, VOLUME_NAME_NT);
            CopyFileW(buf, path.wstring().c_str(), false);
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
            std::fclose(handle);
        }

        posix_file(std::FILE* h)
            : handle(h)
        {}

        virtual void seek(u64 pos) override
        {
            std::fseek(handle, pos, SEEK_SET);
        }

        virtual u64 tell() const override
        {
            return std::ftell(handle);
        }

        virtual u64 size() const override
        {
            u64 cur = tell();
            std::fseek(handle, 0, SEEK_END);
            u64 len = tell();
            std::fseek(handle, cur, SEEK_SET);
            return len;
        }

        virtual void read(void* out, u64 num) override
        {
            auto out = std::fread(out, 1, num, handle);
            (void)out;
        }

        virtual void write(const void* in, u64 num) override 
        {
            auto out = std::fwrite(in, 1, num, handle);
            (void)out;
        }

        virtual void save(const fs::path& path) const override
        {
            std::fflush(handle);
            int out = creat(path.c_str(), 0660);
#if SYS_OSX
            fcopyfile(fileno(handle), out, 0, COPYFILE_ALL);
#else
            sendfile(out, fileno(handle), 0, size());
#endif
            close(out);
        }

    private:
        std::FILE* handle;
    };
#endif

    // in memory file handle wrapper
    struct ram_file : file_handle
    {
        ram_file(const std::vector<byte>& vec)
            : handle(vec)
        {
            if(handle.empty())
                handle.resize(32);
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
            if(cursor + num > handle.size())
                handle.resize(cursor + num);

            std::memcpy(out, handle.data() + cursor, num);
            cursor += num;
        }

        virtual void write(const void* in, u64 num) override 
        {
            if(num + cursor > handle.size())
                handle.resize(cursor + num);

            std::memcpy(handle.data() + cursor, in, num);
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
        std::string access = "b";

        if(mo & mode::write)
            access += "w";

        if(mo & mode::read)
            access += "r";

        std::FILE* f = std::fopen(path.c_str(), access.c_str());
        if(!f)
        {
            spdlog::critical("failed to open file {} with errno {}", path.c_str(), errno);
        }

        return { new posix_file(f) };
#endif
    }

    file from(const std::vector<svl::byte>& vec)
    {
        return file(new ram_file(vec));
    }

    file buffer()
    {
        auto out = from({});
        out.seek(0);
        return out;
    }
}