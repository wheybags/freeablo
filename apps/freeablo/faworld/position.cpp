#include "position.h"

namespace FAWorld
{
    Position::Position(): mDist(0), mDirection(0),
        mMoving(false), mCurrent(std::make_pair(0,0)) {}

    Position::Position(size_t x, size_t y): mDist(0), mDirection(0), 
        mMoving(false), mCurrent(std::make_pair(x,y)) {}

    Position::Position(size_t x, size_t y, size_t direction): mDist(0), 
        mDirection(direction), mMoving(false),
        mCurrent(std::make_pair(x,y)) {}

    void Position::update()
    {
        if(mMoving)
        {
            mDist += 2;

            if(mDist >= 100)
            {
                mCurrent = next();
                mDist = 0;
            }
        }
    }

    std::pair<size_t, size_t> Position::current() const
    {
        return mCurrent;
    }

    std::pair<size_t, size_t> Position::next() const
    {
        if(!mMoving)
            return mCurrent;
        
        std::pair<size_t, size_t> retval = mCurrent;

        switch(mDirection)
        {
            case 0:
            {
                retval.first++;
                retval.second++;
                break;
            }
            
            case 7:
            {
                retval.first++;
                break;
            }

            case 6:
            {
                retval.first++;
                retval.second--;
                break;
            }

            case 5:
            {
                retval.second--;
                break;
            }
            
            case 4:
            {
                retval.first--;
                retval.second--;
                break;
            }

            case 3:
            {
                retval.first--;
                break;
            }

            case 2:
            {
                retval.first--;
                retval.second++;
                break;
            }

            case 1:
            {
                retval.second++;
                break;
            }

            default:
            {
                break;
            }
        }

        return retval;
    }

    struct PosNetData
    {
        size_t dist;
        int32_t direction;
        bool moving;
        size_t currentX;
        size_t currentY;
    };

    size_t Position::getSize()
    {
        return sizeof(PosNetData);
    }

    size_t Position::writeTo(ENetPacket *packet, size_t start)
    {
        PosNetData* data = (PosNetData*)(packet->data + start);
        data->dist = mDist;
        data->direction = mDirection;
        data->moving = mMoving;
        data->currentX = mCurrent.first;
        data->currentY = mCurrent.second;

        return start + getSize();
    }

    size_t Position::readFrom(ENetPacket *packet, size_t start)
    {
        PosNetData* data = (PosNetData*)(packet->data + start);
        mDist = data->dist;
        mDirection = data->direction;
        mMoving = data->moving;
        mCurrent.first = data->currentX;
        mCurrent.second = data->currentY;

        return start + getSize();
    }
}
