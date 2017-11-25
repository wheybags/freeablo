#pragma once
#include <cstdint>
#include <string>

namespace Serial
{
    class ReadBitStream;
    class WriteBitStream;

    class Loader
    {
    public:
        Loader(Serial::ReadBitStream& stream);

        template <typename T> T load() = delete;

    private:
        Serial::ReadBitStream& mStream;
    };

    template <> bool Loader::load<bool>();
    template <> int64_t Loader::load<int64_t>();
    template <> uint64_t Loader::load<uint64_t>();
    template <> int32_t Loader::load<int32_t>();
    template <> uint32_t Loader::load<uint32_t>();
    template <> int16_t Loader::load<int16_t>();
    template <> uint16_t Loader::load<uint16_t>();
    template <> int8_t Loader::load<int8_t>();
    template <> uint8_t Loader::load<uint8_t>();
    template <> std::string Loader::load<std::string>();

    class Saver
    {
    public:
        Saver(Serial::WriteBitStream& stream);

        void save(bool val);
        void save(int64_t val);
        void save(uint64_t val);
        void save(int32_t val);
        void save(uint32_t val);
        void save(int16_t val);
        void save(uint16_t val);
        void save(int8_t val);
        void save(uint8_t val);
        void save(const std::string& val);

    private:
        Serial::WriteBitStream& mStream;
    };
}
