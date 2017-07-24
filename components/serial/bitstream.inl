#include "bithackmacros.h"
#include "bitstream.h"

#include <vector>

namespace Serial
{
    template <int64_t minVal, int64_t maxVal> Error::Error WriteBitStream::handleIntBase(uint64_t& val, bool handleSign)
    {
        static_assert(minVal < maxVal, "invalid range for integer serialisation!");

        int64_t requiredBits = BITS_REQUIRED(0, maxVal)+((int)handleSign);

        int64_t pos = tell();

        for (int64_t i = 0; i < requiredBits; i++)
        {
            bool tmp = (val >> i) & 1;
            if (handleBool(tmp) != Error::Success)
            {
                seek(pos, BSPos::Start);
                return Error::EndOfStream;
            }
        }

        return Error::Success;
    }

    template <int64_t minVal, int64_t maxVal> Error::Error ReadBitStream::handleIntBase(uint64_t& val, bool handleSign)
    {
        static_assert(minVal < maxVal, "invalid range for integer serialisation!");

        int64_t requiredBits = BITS_REQUIRED(0, maxVal)+((int)handleSign);

        if (mSize - mCurrentPos < requiredBits)
            return Error::EndOfStream;

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
            uint64_t nBits = uint64_t(~0) << requiredBits;
            tmp |= nBits;
        }

        val = tmp;

        return Error::Success;
    }

#pragma region write_int_type_overloads
    template <int64_t minVal, int64_t maxVal> Error::Error WriteBitStream::handleInt(int64_t& val)
    {
        if (val > maxVal || val < minVal)
            return Error::OutOfRange;

        uint64_t val2 = (uint64_t)val;
        return handleIntBase<minVal, maxVal>(val2, true);
    }

    template <int64_t minVal, int64_t maxVal> Error::Error WriteBitStream::handleInt(uint64_t& val)
    {
        static_assert(minVal >= 0 && maxVal >= 0, "invalid range for unsigned integer!");

        uint64_t minU = (uint64_t)minVal;
        uint64_t maxU = (uint64_t)maxVal;

        if (val > maxU || val < minU)
            return Error::OutOfRange;

        return handleIntBase<minVal, maxVal>(val, false);
    }

    template <int64_t minVal, int64_t maxVal> Error::Error WriteBitStream::handleInt(int32_t& val)
    {
        int64_t val2 = (int64_t)val;
        return handleInt<minVal, maxVal>(val2);
    }

    template <int64_t minVal, int64_t maxVal> Error::Error WriteBitStream::handleInt(uint32_t& val)
    {
        uint64_t val2 = (uint64_t)val;
        return handleInt<minVal, maxVal>(val2);
    }

    template <int64_t minVal, int64_t maxVal> Error::Error WriteBitStream::handleInt(int8_t& val)
    {
        int64_t val2 = (int64_t)val;
        return handleInt<minVal, maxVal>(val2);
    }

    template <int64_t minVal, int64_t maxVal> Error::Error WriteBitStream::handleInt(uint8_t& val)
    {
        uint64_t val2 = (uint64_t)val;
        return handleInt<minVal, maxVal>(val2);
    }
#pragma endregion


#pragma region read_int_type_overloads
    template <int64_t minVal, int64_t maxVal> Error::Error ReadBitStream::handleInt(int64_t& val)
    {
        uint64_t tmp = 0;
        int64_t pos = tell();

        Error::Error retval = handleIntBase<minVal, maxVal>(tmp, true);
        if(retval != Error::Success)
            return retval;

        val = (int64_t)tmp;

        if (val > maxVal || val < minVal)
        {
            retval = Error::OutOfRange;
            seek(pos, BSPos::Start);
        }

        return retval;
    }

    template <int64_t minVal, int64_t maxVal> Error::Error ReadBitStream::handleInt(uint64_t& val)
    {
        static_assert(minVal >= 0 && maxVal >= 0, "invalid range for unsigned integer!");

        uint64_t tmp = 0;
        int64_t pos = tell();

        Error::Error retval = handleIntBase<minVal, maxVal>(tmp, false);
        if(retval != Error::Success)
            return retval;

        val = (uint64_t)tmp;

        uint64_t minU = (uint64_t)minVal;
        uint64_t maxU = (uint64_t)maxVal;

        if (val > maxU || val < minU)
        {
            retval = Error::OutOfRange;
            seek(pos, BSPos::Start);
        }

        return retval;
    }

    template <int64_t minVal, int64_t maxVal> Error::Error ReadBitStream::handleInt(int32_t& val)
    {
        int64_t tmp = 0;
        Error::Error retval = handleInt<minVal, maxVal>(tmp);
        val = (int32_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> Error::Error ReadBitStream::handleInt(uint32_t& val)
    {
        uint64_t tmp = 0;
        Error::Error retval = handleInt<minVal, maxVal>(tmp);
        val = (uint32_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> Error::Error ReadBitStream::handleInt(int8_t& val)
    {
        int64_t tmp = 0;
        Error::Error retval = handleInt<minVal, maxVal>(tmp);
        val = (int8_t)tmp;
        return retval;
    }

    template <int64_t minVal, int64_t maxVal> Error::Error ReadBitStream::handleInt(uint8_t& val)
    {
        uint64_t tmp = 0;
        Error::Error retval = handleInt<minVal, maxVal>(tmp);
        val = (uint8_t)tmp;
        return retval;
    }
#pragma endregion

    template <class SerializableClass> Error::Error WriteBitStream::handleObject(SerializableClass& o)
    {
        return o.template faSerial<WriteBitStream>(*this);
    }

    template <class SerializableClass> Error::Error ReadBitStream::handleObject(SerializableClass& o)
    {
        return o.template faSerial<ReadBitStream>(*this);
    }
}
