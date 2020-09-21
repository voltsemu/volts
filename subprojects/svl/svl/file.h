#pragma once

#include "types.h"

#include <vector>

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
        std::vector<T> read(usize count) {
            std::vector<T> vec(count);
            read(vec.data(), sizeof(T) * count);
            return vec;
        }

        template<typename T>
        T read() {
            T data;
            read(&data, sizeof(T));
            return data;
        }

        void read(void* ptr, usize limit) {
            handle->read(ptr, limit);
        }

        template<typename T>
        void write(const std::vector<T>& vec) {
            handle->write(vec.data(), sizeof(T) * vec.size());
        }

        template<typename T>
        void write(T val) {
            handle->write(&val, sizeof(T));
        }

        void write(const void* ptr, usize limit) {
            handle->write(ptr, limit);
        }

        void seek(usize pos) {
            handle->seek(pos);
        }

        usize tell() const {
            return handle->tell();
        }

        usize size() const {
            return handle->size();
        }

        void insert(file&& other) {
            other.seek(0);
            write(other.read<u8>(other.size()));
        }

        void close() {
            handle->close();
        }

        struct file_handle {
            virtual ~file_handle() { }
            virtual void read(void* ptr, usize limit) = 0;
            virtual void write(const void* ptr, usize limit) = 0;
            virtual void seek(usize pos) = 0;
            virtual usize tell() const = 0;
            virtual usize size() const = 0;
            virtual void close() = 0;
        };

        file() = delete;
        file(const file&) = delete;

        file(file_handle* file) : handle(file) { }
        file(file&& other) {
            handle = std::move(other.handle);
        }
    private:
        unique<file_handle> handle;
    };

    enum class Mode {
        read,
        write
    };

    file buffer(usize init = 512);
    file open(const fs::path& path, Mode mode = Mode::read);

    template<typename T>
    file from(const std::vector<T>& vec) {
        auto out = buffer(vec.size());
        out.write(vec);
        out.seek(0);
        return out;
    }
}
