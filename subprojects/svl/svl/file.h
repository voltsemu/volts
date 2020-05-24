#pragma once

#include "types.h"
#include "platform.h"

#include <vector>

#include "wrapfs.h"

namespace svl
{
    /**
     * @brief file open mode
     */
    namespace mode
    {
        /// read only file
        constexpr u8 read = (1 << 0);

        /// write file, implies read as well
        constexpr u8 write = (1 << 1);
    }

    /**
     * @brief an abstract base class for all file handles
     */
    struct file_handle
    {
        /**
         * @brief virtual destructor
         */
        virtual ~file_handle() {}

        /**
         * @brief seek to an absolute offset in the file
         * @param pos the position to seek from relative to 0
         */
        virtual void seek(u64) {}

        /**
         * @brief get the current cursor position in the file
         * @return the current cursor position
         */
        virtual u64 tell() const { return 0; }

        /**
         * @brief get the size of the file in bytes
         * @return length of file data in bytes
         */
        virtual u64 size() const { return 0; }

        /**
         * @brief read binary data into a pointer
         * @param out the pointer to write to
         * @param num the amount of bytes to read
         */
        virtual void read(void*, u64) {}

        /**
         * @brief write binary data from a pointer into the file
         * @param in the binary data to read from
         * @param num the amount of bytes to read
         */
        virtual void write(const void*, u64) {}

        virtual void save(const fs::path&) const {}
    };

    /**
     * @brief represents a file stream
     * can be either an in memory stream or a disk backed file
     */
    struct file
    {
        /**
         * @brief seek to a point in the file
         * @param pos the point to seek to
         */
        void seek(u64 pos)
        {
            handle->seek(pos);
        }

        /**
         * @brief get the location of the cursor in the file
         * @return the position of the cursor
         */
        u64 tell() const
        {
            return handle->tell();
        }

        /**
         * @brief get the size of the file
         * @return size in bytes
         */
        u64 size() const
        {
            return handle->size();
        }

        /**
         * @brief read a POD type in from the stream
         * @tparam the type to read in
         * @return the read in data
         */
        template<typename T>
        T read()
        {
            static_assert(std::is_standard_layout<T>::value && std::is_trivial<T>::value);

            T val;
            handle->read(&val, sizeof(T));
            return val;
        }

        /**
         * @brief read in N amount of POD type
         * @tparam the type to read in
         * @param num the amount of data to read in
         * @return the read in data in a struct
         */
        template<typename T>
        std::vector<T> read(u64 num)
        {
            std::vector<T> vec(num);
            handle->read(vec.data(), num * sizeof(T));
            return vec;
        }

        /**
         * @brief read n bytes into a void*
         *
         * @param out the pointer to write to
         * @param num the amount of bytes to read
         */
        void read(void* out, u64 num)
        {
            handle->read(out, num);
        }

        /**
         * @brief write a POD type into the stream
         *
         * @tparam T the type to write
         * @param val the value to write
         */
        template<typename T>
        void write(const T& val)
        {
            static_assert(std::is_standard_layout<T>::value && std::is_trivial<T>::value);

            handle->write(&val, sizeof(T));
        }

        void insert(file other)
        {
            auto dist = other.tell();
            other.seek(0);
            auto data = other.read<byte>(other.size());

            write(data);
            other.seek(dist);
        }

        /**
         * @brief write an array of POD types into the stream
         *
         * @tparam T the pod type to write
         * @param vec the data to write
         */
        template<typename T>
        void write(const std::vector<T>& vec)
        {
            static_assert(std::is_standard_layout<T>::value && std::is_trivial<T>::value);

            handle->write(vec.data(), vec.size() * sizeof(T));
        }

        /**
         * @brief write data from a void* into the stream
         *
         * @param ptr the data to read from
         * @param num the amount of bytes to write
         */
        void write(const void* ptr, u64 num)
        {
            handle->write(ptr, num);
        }

        /**
         * @brief write a string to the file
         *
         * @param str the string to write
         * @param eof should the null terminator be written as well
         */
        void write(const std::string& str, bool eof = false)
        {
            handle->write(str.c_str(), str.size() + (eof ? 1 : 0));
        }

        /**
         * @brief check if the file is valid
         *
         * @return true the file is valid
         * @return false the file is invalid
         */
        bool valid() const
        {
            return !!handle;
        }

        void save(const fs::path& path) const
        {
            handle->save(path);
        }

        friend file open(const fs::path& path, u8 mo);
        friend file from(const std::vector<svl::byte>& vec);

        file(const file&) = default;

        file() : handle(nullptr) {}

    private:

        file(file_handle* ptr) : handle(ptr) {}

        /// handle to the underlying file handle
        std::shared_ptr<file_handle> handle;
    };

    /**
     * @brief open a file on disk
     *
     * @param path the file path to open
     * @param mo the open mode to use
     * @return file the opened file
     */
    file open(const fs::path& path, u8 mo);

    /**
     * @brief convert a vector to a file stream
     *
     * @param vec the vector to convert to a file
     * @return file the file stream
     */
    file from(const std::vector<svl::byte>& vec);

    /**
     * @brief create an in memory file
     *
     * @return file the in memory file stream
     */
    file buffer();
}
