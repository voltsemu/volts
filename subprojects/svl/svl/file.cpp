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

        virtual ~native_file() override {
            CloseHandle(handle);
        }

        virtual void read(void* ptr, u64 limit) override {
            ReadFile(handle, ptr, (DWORD)limit, nullptr, nullptr);
        }

        virtual void write(const void* ptr, u64 limit) override {
            WriteFile(handle, ptr, (DWORD)limit, nullptr, nullptr);
        }
#else
        using native_handle = int;
        static native_file* open(const fs::path& path, Mode mode) {
            return new native_file(::open(
                path.c_str(),
                mode == Mode::read ? O_RDONLY : (O_CREAT | O_WRONLY)
            ));
        }

        virtual ~native_file() override {
            ::close(handle);
        }

        virtual void read(void* ptr, u64 limit) override {
            ::read(handle, ptr, limit);
        }

        virtual void write(const void* ptr, u64 limit) override {
            ::write(handle, ptr, limit);
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
            : memory_file((char*)std::malloc(512), 512)
        { }

        memory_file(char* ptr, u64 len)
            : data(ptr)
            , cursor(0)
            , size(len)
        { }

        virtual ~memory_file() override {
            std::free(data);
        }

        virtual void read(void* ptr, u64 limit) override {
            auto num = std::min(limit, size - cursor);
            std::memcpy(ptr, data + cursor, num);
            cursor += num;
        }

        virtual void write(const void* ptr, u64 limit) override {
            if (limit + cursor >= size) {
                size = (size * 2) + limit;
                data = (char*)std::realloc(data, size);
            }

            std::memcpy(data + cursor, ptr, limit);
            cursor += limit;
        }

        /// data
        char* data;

        /// current offset in the file
        u64 cursor;

        /// total size of the data
        u64 size;
    };

    file buffer() {
        return file(new memory_file());
    }

    file open(const fs::path& path, Mode mode) {
        return file(native_file::open(path, mode));
    }
}