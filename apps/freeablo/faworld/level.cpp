#include "level.h"

#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "../engine/netmanager.h"

namespace FAWorld
{
    GameLevel::GameLevel(Level::Level level) : mLevel(level)
    { }

    std::vector<Level::Monster>& GameLevel::getMonsters()
    {
        return mLevel.getMonsters();
    }

    Level::MinPillar GameLevel::getTile(size_t x, size_t y)
    {
        return mLevel[x][y];
    }

    size_t GameLevel::width() const
    {
        return mLevel.width();
    }

    size_t GameLevel::height() const
    {
        return mLevel.height();
    }

    const std::pair<size_t,size_t>& GameLevel::upStairsPos() const
    {
        return mLevel.upStairsPos();
    }

    const std::pair<size_t,size_t>& GameLevel::downStairsPos() const
    {
        return mLevel.downStairsPos();
    }

    void GameLevel::activate(size_t x, size_t y)
    {
        mLevel.activate(x, y);
    }

    size_t GameLevel::getNextLevel()
    {
        return mLevel.getNextLevel();
    }

    size_t GameLevel::getPreviousLevel()
    {
        return mLevel.getPreviousLevel();
    }

    void GameLevel::startSaving()
    {
        mDataSavingTmp.clear();
        boost::iostreams::back_insert_device<std::string> inserter(mDataSavingTmp);
        boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
        boost::archive::binary_oarchive oa(s);

        oa & mLevel;
        s.flush();
    }

    size_t GameLevel::getWriteSize()
    {
        return mDataSavingTmp.length() + sizeof(size_t); // + 1 size_t for saving the content length
    }

    bool GameLevel::writeTo(ENetPacket* packet, size_t& position)
    {
        if(!Engine::writeToPacket<size_t>(packet, position, mDataSavingTmp.length()))
            return false;

        if((position + mDataSavingTmp.length()) <= packet->dataLength)
        {
            for(size_t i = 0; i < mDataSavingTmp.length(); i++)
                packet->data[position++] = (uint8_t) mDataSavingTmp[i];

            mDataSavingTmp.clear();
            mDataSavingTmp.shrink_to_fit();

            return true;
        }

        return false;
    }

    bool GameLevel::readFrom(ENetPacket* packet, size_t& position)
    {
        size_t contentLength;
        if(!Engine::readFromPacket<size_t>(packet, position, contentLength))
            return false;

        if(position + contentLength <= packet->dataLength)
        {
            std::string strTmp(contentLength, '\0');

            for(size_t i = 0; i < strTmp.length(); i++)
                strTmp[i] = packet->data[position++];

            boost::iostreams::basic_array_source<char> device(strTmp.data(), strTmp.size());
            boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
            boost::archive::binary_iarchive ia(s);
            ia & mLevel;

            return true;
        }

        return false;
    }
}
