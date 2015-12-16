#include "position.h"

#include "../engine/netmanager.h"

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

    double Position::distanceFrom(Position B)
    {
        int dx = mCurrent.first - B.mCurrent.first;
        int dy = mCurrent.second - B.mCurrent.second;

        double x = pow(dx, 2.0);
        double y = pow(dy, 2.0);
        double distance = sqrt(x + y);

        return distance;
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

    size_t Position::getWriteSize()
    {
        return sizeof(PosNetData);
    }

    bool Position::writeTo(ENetPacket *packet, size_t& position)
    {
        PosNetData data;
        data.dist = mDist;
        data.direction = mDirection;
        data.moving = mMoving;
        data.currentX = mCurrent.first;
        data.currentY = mCurrent.second;

        return Engine::writeToPacket(packet, position, data);
    }

    bool Position::readFrom(ENetPacket *packet, size_t& position)
    {
        PosNetData data;

        if(Engine::readFromPacket(packet, position, data))
        {
            mDist = data.dist;
            mDirection = data.direction;
            mMoving = data.moving;
            mCurrent.first = data.currentX;
            mCurrent.second = data.currentY;

            return true;
        }

        return false;
    }
}
