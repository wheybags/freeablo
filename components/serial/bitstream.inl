#include "bithackmacros.h"
#include "bitstream.h"

#include <vector>

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

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(uint64_t& val)
    {
        int64_t tmp = 0;
        bool retval = handleInt<minVal, maxVal>(tmp);
        val = (uint64_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(int32_t& val)
    {
        int64_t tmp = 0;
        bool retval = handleInt<minVal, maxVal>(tmp);
        val = (int32_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(uint32_t& val)
    {
        int32_t tmp = 0;
        bool retval = handleInt<minVal, maxVal>(tmp);
        val = (uint32_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(int8_t& val)
    {
        int32_t tmp = 0;
        bool retval = handleInt<minVal, maxVal>(tmp);
        val = (int8_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(uint8_t& val)
    {
        int8_t tmp = 0;
        bool retval = handleInt<minVal, maxVal>(tmp);
        val = (uint8_t)tmp;
        return retval;
    }

    // based on .NET's BinaryWriter.Write7BitEncodedInt method
    bool WriteBitStream::handleInt32(int32_t val)
    {
        uint32_t num = (uint32_t)val;

        std::vector<uint8_t> toWrite;

        while (num >= 128U)
        {
            toWrite.push_back((uint8_t)(num | 128U));
            num >>= 7;
        }

        toWrite.push_back((uint8_t)num);

        if ((size - currentPos) < (toWrite.size() * 8))
            return false;

        for (size_t i = 0; i < toWrite.size(); i++)
            handleInt<0, 255>(toWrite[i]);

        return true;
    }

    bool ReadBitStream::handleInt32(int32_t& val)
    {
        int32_t returnValue = 0;
        int32_t bitIndex = 0;

        int64_t pos = tell();

        while (bitIndex != 35)
        {
            uint8_t currentByte = 0;
            if (!handleInt<0, 255>(currentByte))
            {
                seek(pos, BSPos::Start);
                return false;
            }

            returnValue |= ((int)currentByte & (int)127) << bitIndex;
            bitIndex += 7;

            if (((int)currentByte & 128) == 0)
            {
                val = returnValue;
                return true;
            }
        }

        return false;
    }
}