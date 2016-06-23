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
}