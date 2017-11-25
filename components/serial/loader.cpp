#include "loader.h"
#include "bitstream.h"

namespace Serial
{
    // TODO: remove these unions and handle 64-bit ints properly
    union Int64To32
    {
        int64_t i64;
        uint32_t i32s[2];
    };

    union UInt64To32
    {
        uint64_t i64;
        uint32_t i32s[2];
    };

    Loader::Loader(Serial::ReadBitStream& stream) : mStream(stream) {}

    template <> bool Loader::load<bool>()
    {
        bool val;
        mStream.handleBool(val);
        return val;
    }

    template <> int64_t Loader::load<int64_t>()
    {
        Int64To32 tmp;
        tmp.i32s[0] = load<uint32_t>();
        tmp.i32s[1] = load<uint32_t>();

        return tmp.i64;
    }

    template <> uint64_t Loader::load<uint64_t>()
    {
        UInt64To32 tmp;
        tmp.i32s[0] = load<uint32_t>();
        tmp.i32s[1] = load<uint32_t>();

        return tmp.i64;
    }


    template <> int32_t Loader::load<int32_t>()
    {
        int32_t val;
        mStream.handleInt32(val);
        return val;
    }

    template <> uint32_t Loader::load<uint32_t>()
    {
        uint32_t val;
        mStream.handleInt32(val);
        return val;
    }

    template <> int16_t Loader::load<int16_t>()
    {
        int32_t val;
        mStream.handleInt32(val);
        return val;
    }

    template <> uint16_t Loader::load<uint16_t>()
    {
        uint32_t val;
        mStream.handleInt32(val);
        return val;
    }

    template <> int8_t Loader::load<int8_t>()
    {
       uint8_t valu = load<uint8_t>();
       return *((int8_t*)&valu);
    }

    template <> uint8_t Loader::load<uint8_t>()
    {
        uint8_t val;
        mStream.handleInt<0, 255>(val);
        return val;
    }

    template <> std::string Loader::load<std::string>()
    {
        uint32_t len = load<uint32_t>();

        std::string val;
        val.resize(len);

        mStream.handleString((uint8_t*)val.data(), len);
        return val;
    }


    Saver::Saver(WriteBitStream& stream) : mStream(stream) {}

    void Saver::save(bool val)
    {
        mStream.handleBool(val);
    }

    void Saver::save(int64_t val)
    {
        Int64To32 tmp;
        tmp.i64 = val;
        save(tmp.i32s[0]);
        save(tmp.i32s[0]);
    }

    void Saver::save(uint64_t val)
    {
        UInt64To32 tmp;
        tmp.i64 = val;
        save(tmp.i32s[0]);
        save(tmp.i32s[0]);
    }

    void Saver::save(int32_t val)
    {
        mStream.handleInt32(val);
    }

    void Saver::save(uint32_t val)
    {
        mStream.handleInt32(val);
    }

    void Saver::save(int16_t val)
    {
        int32_t val32 = val;
        mStream.handleInt32(val32);
    }

    void Saver::save(uint16_t val)
    {
        uint32_t val32 = val;
        mStream.handleInt32(val32);
    }

    void Saver::save(int8_t val)
    {
        save(*((uint8_t*)&val));
    }

    void Saver::save(uint8_t val)
    {
        mStream.handleInt<0, 255>(val);
    }

    void Saver::save(const std::string& val)
    {
        uint32_t len = val.size();
        save(len);
        mStream.handleString((uint8_t*)val.data(), len);
    }
}
