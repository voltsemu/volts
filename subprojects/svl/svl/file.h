#pragma once

#include "types.h"

#if __has_include(<filesystem>)
#   include <filesystem>
namespace svl { namespace fs = std::filesystem; }
#elif __has_include(<experimental/filesystem>)
#   include <experimental/filesystem>
namespace svl { namespace fs = std::experimental::filesystem; }
#else
#   error no <filesystem> header found
#endif

namespace svl {
    struct file {
        template<typename T>
        T read() {
            T data;
            read(&data, sizeof(T));
            return data;
        }
        
        void read(void* ptr, u64 limit) {
            handle->read(ptr, limit);
        }

        void write(const void* ptr, u64 limit) {
            handle->write(ptr, limit);
        }

        struct file_handle {
            virtual ~file_handle() { }
            virtual void read(void* ptr, u64 limit) = 0;
            virtual void write(const void* ptr, u64 limit) = 0;
        };

        file(file_handle* file) : handle(file) { }
    private:
        unique<file_handle> handle;
    };

    enum class Mode {
        read,
        write
    };

    file buffer();
    file open(const fs::path& path, Mode mode = Mode::read);
}
