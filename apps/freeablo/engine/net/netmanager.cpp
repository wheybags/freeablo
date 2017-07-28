#include "netmanager.h"

#include "../../faworld/world.h"
#include "../../faworld/playerfactory.h"
#include "../../faworld/actor.h"
#include "../../faworld/player.h"
#include "../../faworld/monster.h"

#include <serial/bitstream.h>

#include <boost/math/special_functions.hpp>

#include <thread>
#include <chrono>


#include "server.h"
#include "client.h"


namespace Engine
{
    NetManager* singletonInstance = NULL;

    NetManager* NetManager::get()
    {
        return singletonInstance;
    }

    NetManager::NetManager(bool isServer, const FAWorld::PlayerFactory& playerFactory)
    {
        assert(singletonInstance == NULL);
        singletonInstance = this;

        enet_initialize();

        mIsServer = isServer;

        if (isServer)
            mServer = new Server(playerFactory);
        else
            mClient = new Client();
    }

    NetManager::~NetManager()
    {
        if (mIsServer)
            delete mServer;
        else
            delete mClient;
        
        enet_deinitialize();
        singletonInstance = NULL;
    }

    void NetManager::update()
    {
        mTick++;

        if (mIsServer)
            mServer->update(mTick);
        else
            mClient->update(mTick);
    }

    FARender::FASpriteGroup* NetManager::getServerSprite(size_t index)
    {
        assert(!mIsServer);
        if (index == 0)
            return FARender::getDefaultSprite();
        return mClient->getServerSprite(index);
    }

    void NetManager::sendLevelChangePacket(int32_t level)
    {
        mClient->sendLevelChangePacket(level);
    }
}
