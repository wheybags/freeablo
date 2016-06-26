#include "bithackmacros.h"
#include "bitstream.h"

#include <vector>

namespace Serial
{
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
            tmp ^= (-ib ^ tmp) & (((int64_t)1) << i);
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
}