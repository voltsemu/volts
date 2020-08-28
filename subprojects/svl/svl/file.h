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

        template<typename T>
        std::vector<T> read(u64 count) {
            std::vector<T> vec(count);
            read(vec.data(), sizeof(T) * count);
            return vec;
        }

        void read(void* ptr, u64 limit) {
            handle->read(ptr, limit);
        }

        template<typename T>
        void write(T val) {
            handle->write(&val, sizeof(T));
        }

        template<typename T>
        void write(const std::vector<T>& vec) {
            handle->write(vec.data(), sizeof(T) * vec.size());
        }

        void write(const void* ptr, u64 limit) {
            handle->write(ptr, limit);
        }

        void seek(u64 pos) {
            handle->seek(pos);
        }

        u64 tell() const {
            return handle->tell();
        }

        u64 size() const {
            return handle->size();
        }

        void insert(file&& other) {
            other.seek(0);
            write(other.read<u8>(other.size()));
        }

        struct file_handle {
            virtual ~file_handle() { }
            virtual void read(void* ptr, u64 limit) = 0;
            virtual void write(const void* ptr, u64 limit) = 0;
            virtual void seek(u64 pos) = 0;
            virtual u64 tell() const = 0;
            virtual u64 size() const = 0;
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
