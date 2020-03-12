#pragma once
#include "streaminterface.h"
#include <limits>
#include <sstream>
#include <string>

namespace Serial
{
    class TextReadStream : public ReadStreamInterface
    {
    public:
        TextReadStream(const std::string& data) : mData(data) {}

        virtual bool read_bool() override;
        virtual int64_t read_int64_t() override;
        virtual uint64_t read_uint64_t() override;
        virtual int32_t read_int32_t() override;
        virtual uint32_t read_uint32_t() override;
        virtual int16_t read_int16_t() override;
        virtual uint16_t read_uint16_t() override;
        virtual int8_t read_int8_t() override;
        virtual uint8_t read_uint8_t() override;
        virtual std::string read_string() override;

        virtual void startCategory(const std::string& name) override;
        virtual void endCategory(const std::string& name) override;

    private:
        std::string readTypedLine(const std::string& expectedType);

        uint32_t mLine = 0;
        std::istringstream mData;
    };

    class TextWriteStream : public WriteStreamInterface
    {
    public:
        TextWriteStream() = default;

        virtual size_t getCurrentSize() const override;
        virtual void resize(size_t size) override;
        virtual std::pair<uint8_t*, size_t> getData() override;

        virtual void write(bool val) override;
        virtual void write(int64_t val) override;
        virtual void write(uint64_t val) override;
        virtual void write(int32_t val) override;
        virtual void write(uint32_t val) override;
        virtual void write(int16_t val) override;
        virtual void write(uint16_t val) override;
        virtual void write(int8_t val) override;
        virtual void write(uint8_t val) override;
        virtual void write(const std::string& val) override;

        virtual void startCategory(const std::string& name) override;
        virtual void endCategory(const std::string& name) override;

    private:
        void writeTypedLine(const std::string& type, const std::string& data);

        int32_t mTabIndex = 0;
        std::string mData;
    };
}
