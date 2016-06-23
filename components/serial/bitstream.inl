#include "bithackmacros.h"

#include "bitstream.h"

namespace Serial
{
    void BitStreamBase::init(uint8_t* buf, int64_t sizeInBytes)
    {
        data = buf;
        currentPos = 0;
        size = sizeInBytes * 8;
    }

    WriteBitStream::WriteBitStream(uint8_t* buf, int64_t sizeInBytes)
    {
        init(buf, sizeInBytes);
    }

    ReadBitStream::ReadBitStream(uint8_t* buf, int64_t sizeInBytes)
    {
        init(buf, sizeInBytes);
    }

    int64_t BitStreamBase::tell()
    {
        return currentPos;
    }

    bool BitStreamBase::seek(int64_t offset, BSPos::BSPos origin)
    {
        int64_t newPos = 0;

        switch (origin)
        {
            case BSPos::Start:
            {
                newPos = offset;
                break;
            }

            case BSPos::Current:
            {
                newPos = currentPos + offset;
                break;
            }

            case BSPos::End:
            {
                newPos = size + offset;
                break;
            }
        }

        if (newPos >= 0 && newPos < size)
        {
            currentPos = newPos;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool WriteBitStream::handleBool(bool val)
    {
        if (currentPos < size)
        {
            int64_t bitPos = currentPos % 8;
            int64_t bytePos = (currentPos - bitPos) / 8;

            uint8_t byte = data[bytePos];
            uint8_t bVal = val;
            byte ^= (-bVal ^ byte) & (1 << bitPos);
            data[bytePos] = byte;

            currentPos++;

            return true;
        }
        
        return false;
    }

    bool ReadBitStream::handleBool(bool& val)
    {
        if (currentPos < size)
        {
            int64_t bitPos = currentPos % 8;
            int64_t bytePos = (currentPos - bitPos) / 8;

            uint8_t byte = data[bytePos];
            val = (byte >> bitPos) & 1;

            currentPos++;

            return true;
        }

        return false;
    }

    template <int64_t minVal, int64_t maxVal> bool WriteBitStream::handleInt(int64_t val)
    {
        int64_t requiredBits = BITS_REQUIRED(minVal, maxVal+1);

        if (val > maxVal || val < minVal)
            return false;

        int64_t pos = tell();

        for (int64_t i = 0; i < requiredBits; i++)
        {
            if (!handleBool((val >> i) & 1))
            {
                seek(pos, BSPos::Start);
                return false;
            }
        }

        return true;
    }

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(int64_t& val)
    {
        int64_t requiredBits = BITS_REQUIRED(minVal, maxVal+1);

        if (size - currentPos < requiredBits)
            return false;

        int64_t tmp = 0;

        for (int64_t i = 0; i < requiredBits; i++)
        {
            bool bit;
            handleBool(bit);
            int64_t ib = bit;
            tmp ^= (-ib ^ tmp) & (1 << i);
        }

        if ((tmp >> (requiredBits-1)) & 1)
        {
            int64_t nBits = ~0 << requiredBits;
            tmp |= nBits;
        }

        if (tmp > maxVal || tmp < minVal)
            return false;

        val = tmp;

        return true;
    }
}