#include "file.h"

#include "macros.h"

#if !SYS_WIN
#   include <unistd.h>
#   include <fcntl.h>
#endif

#include <cstring>

namespace svl {
    struct native_file : file::file_handle {
#if SYS_WIN
        using native_handle = HANDLE;
        static native_file* open(const fs::path& path, Mode mode) {
            auto access = mode == Mode::read ? GENERIC_READ : GENERIC_WRITE;
            auto disp = mode == Mode::read ? OPEN_EXISTING : CREATE_ALWAYS;

            return new native_file(CreateFileW(
                path.wstring().c_str(),
                access, FILE_SHARE_READ, nullptr,
                disp, FILE_ATTRIBUTE_NORMAL, nullptr
            ));
        }

        virtual void read(void* ptr, u64 limit) override {
            ReadFile(handle, ptr, (DWORD)limit, nullptr, nullptr);
        }

        virtual void write(const void* ptr, u64 limit) override {
            WriteFile(handle, ptr, (DWORD)limit, nullptr, nullptr);
        }

        virtual void seek(u64 pos) override {
            LARGE_INTEGER loc;
            loc.QuadPart = pos;

            SetFilePointerEx(handle, loc, &loc, FILE_BEGIN);
        }

        virtual u64 tell() const override {
            LARGE_INTEGER pos;
            SetFilePointerEx(handle, pos, &pos, FILE_CURRENT);

            return pos.QuadPart;
        }

        virtual u64 size() const override {
            LARGE_INTEGER len;
            GetFileSizeEx(handle, &len);
            return len.QuadPart;
        }

        virtual void close() override {
            CloseHandle(handle);
        }
#else
        using native_handle = int;
        static native_file* open(const fs::path& path, Mode mode) {
            return new native_file(::open(
                path.c_str(),
                mode == Mode::read ? O_RDONLY : (O_CREAT | O_WRONLY)
            ));
        }

        virtual void read(void* ptr, u64 limit) override {
            ::read(handle, ptr, limit);
        }

        virtual void write(const void* ptr, u64 limit) override {
            ::write(handle, ptr, limit);
        }

        virtual void seek(u64 pos) override {
            ::lseek(handle, pos, SEEK_SET);
        }

        virtual u64 tell() const override {
            return ::lseek(handle, 0, SEEK_CUR);
        }

        virtual u64 size() const override {
            auto cur = tell();
            ::lseek(handle, 0, SEEK_END);
            auto len = tell();
            ::lseek(handle, cur, SEEK_SET);
            return len;
        }

        virtual void close() override {
            ::close(handle);
        }
#endif
        native_file(native_handle h)
            : handle(h)
        { }

        native_handle handle;
    };

    /**
     * an in memory file
     */
    struct memory_file : file::file_handle {
        memory_file()
            : memory_file(malloc(512), 512)
        { }

        memory_file(void* ptr, u64 len)
            : data(ptr)
            , cursor(0)
            , len(len)
        { }

        virtual ~memory_file() override {
            free(data);
        }

        virtual void read(void* ptr, u64 limit) override {
            auto num = std::min(limit, len - cursor);
            memcpy(ptr, (char*)data + cursor, num);
            cursor += num;
        }

        virtual void write(const void* ptr, u64 limit) override {
            if (limit + cursor >= len) {
                len = (len * 2) + limit;
                data = realloc(data, len);
            }

            memcpy((char*)data + cursor, ptr, limit);
            cursor += limit;
        }

        virtual void seek(u64 pos) override {
            cursor = pos;
            if (cursor > len) {
                len = cursor + 1;
                data = realloc(data, len);
            }
        }

        virtual u64 tell() const override {
            return cursor;
        }

        virtual u64 size() const override {
            return len;
        }

        virtual void close() override {
            free(data);
            data = nullptr;
        }

        /// data
        void* data;

        /// current offset in the file
        u64 cursor;

        /// total size of the data
        u64 len;
    };

    file buffer() {
        return file(new memory_file());
    }

    file open(const fs::path& path, Mode mode) {
        return file(native_file::open(path, mode));
    }
}