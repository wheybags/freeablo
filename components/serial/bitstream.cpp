#define ERR_GET_NAME_IMPL
#include "bitstream.h"

#include <stddef.h>

namespace Serial
{
    void BitStreamBase::init(uint8_t* buf, int64_t sizeInBytes)
    {
        data = buf;
        currentPos = 0;
        size = sizeInBytes * 8;
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

    WriteBitStream::WriteBitStream(uint8_t* buf, int64_t sizeInBytes)
    {
        init(buf, sizeInBytes);
    }

    ReadBitStream::ReadBitStream(uint8_t* buf, int64_t sizeInBytes)
    {
        init(buf, sizeInBytes);
    }

    Error::Error WriteBitStream::handleBool(bool& val)
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

            return Error::Success;
        }

        return Error::EndOfStream;
    }

    Error::Error ReadBitStream::handleBool(bool& val)
    {
        if (currentPos < size)
        {
            int64_t bitPos = currentPos % 8;
            int64_t bytePos = (currentPos - bitPos) / 8;

            uint8_t byte = data[bytePos];
            val = (byte >> bitPos) & 1;

            currentPos++;

            return Error::Success;
        }

        return Error::EndOfStream;
    }

    // based on .NET's BinaryWriter.Write7BitEncodedInt method
    Error::Error WriteBitStream::handleInt32(int32_t& val)
    {
        uint32_t num = (uint32_t)val;

        std::vector<uint8_t> toWrite;

        while (num >= 128U)
        {
            toWrite.push_back((uint8_t)(num | 128U));
            num >>= 7;
        }

        toWrite.push_back((uint8_t)num);

        if ((size - currentPos) < ((int64_t)toWrite.size() * 8))
            return Error::EndOfStream;

        for (size_t i = 0; i < toWrite.size(); i++)
            handleInt<0, 255>(toWrite[i]);

        return Error::Success;
    }

    Error::Error ReadBitStream::handleInt32(int32_t& val)
    {
        int32_t returnValue = 0;
        int32_t bitIndex = 0;

        int64_t pos = tell();

        while (bitIndex != 35)
        {
            uint8_t currentByte = 0;
            if (handleInt<0, 255>(currentByte) != Error::Success)
            {
                seek(pos, BSPos::Start);
                return Error::OutOfRange;
            }

            returnValue |= ((int)currentByte & (int)127) << bitIndex;
            bitIndex += 7;

            if (((int)currentByte & 128) == 0)
            {
                val = returnValue;
                return Error::Success;
            }
        }

        return Error::InvalidData;
    }
}
