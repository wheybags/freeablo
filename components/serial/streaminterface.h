#pragma once
#include <cstdint>
#include <misc/misc.h>
#include <string>
#include <utility>

namespace Serial
{
    class ReadStreamInterface
    {
    public:
        virtual bool read_bool() = 0;
        virtual int64_t read_int64_t() = 0;
        virtual uint64_t read_uint64_t() = 0;
        virtual int32_t read_int32_t() = 0;
        virtual uint32_t read_uint32_t() = 0;
        virtual int16_t read_int16_t() = 0;
        virtual uint16_t read_uint16_t() = 0;
        virtual int8_t read_int8_t() = 0;
        virtual uint8_t read_uint8_t() = 0;
        virtual std::string read_string() = 0;

        virtual void startCategory(const std::string& name) { UNUSED_PARAM(name); }
        virtual void endCategory(const std::string& name) { UNUSED_PARAM(name); }
    };

    class WriteStreamInterface
    {
    public:
        virtual size_t getCurrentSize() const = 0;
        virtual void resize(size_t size) = 0;
        virtual std::pair<uint8_t*, size_t> getData() = 0;

        virtual void write(bool val) = 0;
        virtual void write(int64_t val) = 0;
        virtual void write(uint64_t val) = 0;
        virtual void write(int32_t val) = 0;
        virtual void write(uint32_t val) = 0;
        virtual void write(int16_t val) = 0;
        virtual void write(uint16_t val) = 0;
        virtual void write(int8_t val) = 0;
        virtual void write(uint8_t val) = 0;
        virtual void write(const std::string& val) = 0;

        virtual void startCategory(const std::string& name) { UNUSED_PARAM(name); }
        virtual void endCategory(const std::string& name) { UNUSED_PARAM(name); }
    };
}
