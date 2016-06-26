#ifndef FA_BITSTREAM_H
#define FA_BITSTREAM_H

#include <cstdint>

namespace Serial
{
    namespace BSPos
    {
        enum BSPos
        {
            Start,
            Current,
            End
        };
    }

    class BitStreamBase
    {
        public:
            int64_t tell();
            bool seek(int64_t offset, BSPos::BSPos origin);

        protected:
            void init(uint8_t* buf, int64_t sizeInBytes);

            int64_t currentPos;
            int64_t size;
            uint8_t* data;
    };

    class WriteBitStream : public BitStreamBase
    {
        public:
            WriteBitStream(uint8_t* buf, int64_t sizeInBytes);

            bool handleBool(bool val);
            template <int64_t minVal, int64_t maxVal> bool handleInt(int64_t val);
            bool handleInt32(int32_t val);
    };

    class ReadBitStream : public BitStreamBase
    {
        public:
            ReadBitStream(uint8_t* buf, int64_t sizeInBytes);

            bool handleBool(bool& val);
            template <int64_t minVal, int64_t maxVal> bool handleInt(int64_t& val);
            template <int64_t minVal, int64_t maxVal> bool handleInt(uint64_t& val);

            template <int64_t minVal, int64_t maxVal> bool handleInt(int32_t& val);
            template <int64_t minVal, int64_t maxVal> bool handleInt(uint32_t& val);

            template <int64_t minVal, int64_t maxVal> bool handleInt(int8_t& val);
            template <int64_t minVal, int64_t maxVal> bool handleInt(uint8_t& val);

            bool handleInt32(int32_t& val);
    };
}

#include "bitstream.inl"

#endif // !FA_BITSTREAM_H