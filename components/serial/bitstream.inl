#include "bithackmacros.h"
#include "bitstream.h"

#include <vector>

namespace Serial
{
    template <int64_t minVal, int64_t maxVal> bool WriteBitStream::handleIntBase(uint64_t val, bool handleSign)
    {
        static_assert(minVal < maxVal, "invalid range for integer serialisation!");

        int64_t requiredBits = BITS_REQUIRED(0, maxVal)+((int)handleSign);

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

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleIntBase(uint64_t& val, bool handleSign)
    {
        static_assert(minVal < maxVal, "invalid range for integer serialisation!");

        int64_t requiredBits = BITS_REQUIRED(0, maxVal)+((int)handleSign);

        if (size - currentPos < requiredBits)
            return false;

        uint64_t tmp = 0;

        for (int64_t i = 0; i < requiredBits; i++)
        {
            bool bit;
            handleBool(bit);
            int64_t ib = bit;
            tmp ^= (-ib ^ tmp) & (((int64_t)1) << i);
        }

        if (handleSign && ((tmp >> (requiredBits-1)) & 1))
        {
            int64_t nBits = ~0 << requiredBits;
            tmp |= nBits;
        }

        val = tmp;

        return true;
    }

#pragma region write_int_type_overloads
    template <int64_t minVal, int64_t maxVal> bool WriteBitStream::handleInt(int64_t val)
    {
        if (val > maxVal || val < minVal)
            return false;

        return handleIntBase<minVal, maxVal>((uint64_t)val, true);
    }

    template <int64_t minVal, int64_t maxVal> bool WriteBitStream::handleInt(uint64_t val)
    {
        static_assert(minVal >= 0 && maxVal >= 0, "invalid range for unsigned integer!");

        uint64_t minU = (uint64_t)minVal;
        uint64_t maxU = (uint64_t)maxVal;

        if (val > maxU || val < minU)
            return false;

        return handleIntBase<minVal, maxVal>((uint64_t)val, false);
    }

    template <int64_t minVal, int64_t maxVal> bool WriteBitStream::handleInt(int32_t val)
    {
        return handleInt<minVal, maxVal>((int64_t)val);
    }

    template <int64_t minVal, int64_t maxVal> bool WriteBitStream::handleInt(uint32_t val)
    {
        return handleInt<minVal, maxVal>((uint64_t)val);
    }

    template <int64_t minVal, int64_t maxVal> bool WriteBitStream::handleInt(int8_t val)
    {
        return handleInt<minVal, maxVal>((int64_t)val);
    }

    template <int64_t minVal, int64_t maxVal> bool WriteBitStream::handleInt(uint8_t val)
    {
        return handleInt<minVal, maxVal>((uint64_t)val);
    }
#pragma endregion


#pragma region read_int_type_overloads
    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(int64_t& val)
    {
        uint64_t tmp = 0;
        int64_t pos = tell();

        bool retval = handleIntBase<minVal, maxVal>(tmp, true);
        val = (int64_t)tmp;

        if (val > maxVal || val < minVal)
        {
            retval = false;
            seek(pos, BSPos::Start);
        }

        return retval;
    }

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(uint64_t& val)
    {
        static_assert(minVal >= 0 && maxVal >= 0, "invalid range for unsigned integer!");

        uint64_t tmp = 0;
        int64_t pos = tell();

        bool retval = handleIntBase<minVal, maxVal>(tmp, false);
        val = (uint64_t)tmp;

        uint64_t minU = (uint64_t)minVal;
        uint64_t maxU = (uint64_t)maxVal;

        if (val > maxU || val < minU)
        {
            retval = false;
            seek(pos, BSPos::Start);
        }

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
        uint64_t tmp = 0;
        bool retval = handleInt<minVal, maxVal>(tmp);
        val = (uint32_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(int8_t& val)
    {
        int64_t tmp = 0;
        bool retval = handleInt<minVal, maxVal>(tmp);
        val = (int8_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> bool ReadBitStream::handleInt(uint8_t& val)
    {
        uint64_t tmp = 0;
        bool retval = handleInt<minVal, maxVal>(tmp);
        val = (uint8_t)tmp;
        return retval;
    }
#pragma endregion

    template <class SerializableClass> bool WriteBitStream::handleObject(SerializableClass& o)
    {
        return o.faSerial<WriteBitStream>(*this);
    }

    template <class SerializableClass> bool ReadBitStream::handleObject(SerializableClass& o)
    {
        return o.faSerial<ReadBitStream>(*this);
    }
}