#pragma once

#include <mio/mmap.hpp>
#include "NumTypes.hpp"
#include "Keys.hpp"

namespace wz {

    using wzstring = std::u16string;

    class Reader final {
    public:
        explicit Reader(wz::MutableKey& new_key, const char* file_path);

#ifdef _WIN32
        explicit Reader(const char16_t* file_path);
#endif

        template<typename T> [[nodiscard]]
        T read() {
            auto result = *reinterpret_cast<T*>(&mmap[cursor]);
            cursor += sizeof(decltype(result));
            return result;
        }

        void skip(const size_t& size);

        [[nodiscard]]
        u8 read_byte();

        [[maybe_unused]] [[nodiscard]]
        std::vector<u8> read_bytes(const size_t& len);

        /*
         * read string until **null terminated**
         */
        [[nodiscard]]
        wzstring read_string();

        [[nodiscard]]
        wzstring read_string(const size_t& len);

        [[nodiscard]]
        i32 read_compressed_int();

        [[nodiscard]]
        wzstring read_wz_string();

        wzstring read_string_block(const size_t& offset);

        template<typename T> [[nodiscard]]
        T read_wz_string_from_offset(const size_t& offset, wzstring& out) {
            auto prev = cursor;
            set_position(offset);
            auto result = read<T>();
            out = read_wz_string();
            set_position(prev);
            return result;
        }

        wzstring read_wz_string_from_offset(const size_t& offset);

        [[nodiscard]]
        size_t get_position() const;

        void set_position(const size_t& size);

        [[nodiscard]]
        mio::mmap_source::size_type size() const;

        [[nodiscard]]
        bool is_wz_image();
        [[nodiscard]]
        bool is_has_property();

        void set_key(MutableKey& new_key);

    private:

        MutableKey& key;

        size_t cursor = 0;

        mio::mmap_source mmap;

        explicit Reader() = delete;

        friend class Node;


    };
}
