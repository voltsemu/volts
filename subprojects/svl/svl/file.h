#pragma once

#include <variant>

#include <svl/wrapfs.h>
#include <svl/types.h>
#include <svl/macros.h>

namespace svl
{
    struct FileHandle
    {
        virtual ~FileHandle() { }

        virtual void seek(usize) = 0;
        virtual usize tell() const = 0;
        virtual usize size() const = 0;

        virtual void read(byte*, usize) = 0;
        virtual void write(const byte*, usize) = 0;
        virtual void save(const fs::path&) const = 0;
    };

    enum class Mode
    {
        read,
        write
    };

    struct File
    {
        void seek(usize off)
        {
            handle->seek(off);
        }

        usize tell() const
        {
            return handle->tell();
        }

        usize size() const
        {
            return handle->size();
        }

        void read(byte* ptr, usize len)
        {
            handle->read(ptr, len);
        }

        /**
         *
         */
        void align(isize bound)
        {
            auto to = ROUND2(tell(), bound);
            seek(to);
        }

        template<typename T>
        T read()
        {
            static_assert(std::is_standard_layout<T>::value && std::is_trivial<T>::value);

            T out;
            handle->read((byte*)&out, sizeof(T));
            return std::move(out);
        }

        template<typename T = byte>
        std::vector<T> read(usize num)
        {
            static_assert(std::is_standard_layout<T>::value && std::is_trivial<T>::value);

            std::vector<T> out(num);
            handle->read((byte*)out.data(), num * sizeof(T));
            return std::move(out);
        }

        void write(const byte* ptr, usize len)
        {
            handle->write(ptr, len);
        }

        template<typename T>
        void write(const T& data)
        {
            static_assert(std::is_standard_layout<T>::value && std::is_trivial<T>::value);

            handle->write((const byte*)&data, sizeof(T));
        }

        template<typename T = byte>
        void write(const std::vector<T>& data)
        {
            static_assert(std::is_standard_layout<T>::value && std::is_trivial<T>::value);

            handle->write((byte*)data.data(), data.size());
        }

        void write(const std::string& str, bool term = true)
        {
            handle->write((const byte*)str.c_str(), str.size() + (term ? 1 : 0));
        }

        template<typename... T>
        void write(const std::variant<T...>& data)
        {
            std::visit([&](const auto& val) { write(val); }, data);
        }

        void insert(File&& other)
        {
            other.seek(0);
            const auto data = other.read(other.size());

            write(data);
        }

        void save(const fs::path& path) const
        {
            handle->save(path);
        }

        friend File open(const fs::path& path, Mode mode);
        friend File stream(std::vector<byte>&& data);
        friend File buffer();

    private:
        File(FileHandle* ptr) : handle(ptr) { }
        std::unique_ptr<FileHandle> handle;
    };

    File open(const fs::path& path, Mode mode = Mode::read);

    File stream(std::vector<byte>&& data);
    File buffer();
}