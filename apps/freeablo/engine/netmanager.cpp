#include "netmanager.h"

#include "../faworld/world.h"

#include <boost/math/special_functions.hpp>

namespace Engine
{
    NetManager::NetManager(bool isServer)
    {
        enet_initialize();

        mAddress.port = 6666;

        mIsServer = isServer;

        FAWorld::World& world = *FAWorld::World::get();

        if(isServer)
        {
            mAddress.host = ENET_HOST_ANY;
            mHost = enet_host_create(&mAddress, 32, 2, 0, 0);

            world.setCurrentPlayerId(SERVER_PLAYER_ID);
        }
        else
        {
            enet_address_set_host(&mAddress, "127.0.0.1");
            mHost = enet_host_create(NULL, 32, 2, 0, 0);

            mServerPeer = enet_host_connect(mHost, &mAddress, 2, 0);

            ENetEvent event;

            if(enet_host_service(mHost, &event, 5000))
            {
                std::cout << "connected" << std::endl;
            }
            else
            {
                std::cout << "connection failed" << std::endl;
            }

            world.setCurrentPlayerId(mServerPeer->connectID);
        }
    }

    NetManager::~NetManager()
    {
        if(mIsServer)
        {
            for(size_t i = 0; i < mClients.size(); i++)
                enet_peer_disconnect(mClients[i], 0);
        }
        else if(mServerPeer != NULL)
        {
            enet_peer_disconnect(mServerPeer, 0);
        }

        enet_host_destroy(mHost);

        enet_deinitialize();
    }

    void NetManager::update()
    {
        mTick++;

        ENetEvent event;

        // TODO: remove this block when we handle level switching properly
        auto player = FAWorld::World::get()->getCurrentPlayer();
        if(player->getLevel())
            mLevelIndexTmp = player->getLevel()->getLevelIndex();
        else
            mLevelIndexTmp = -1;

        while(enet_host_service(mHost, &event, 0))
        {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    if(mIsServer)
                    {
                        spawnPlayer(event.peer->connectID);
                        sendLevel(0, event.peer);
                        sendLevel(1, event.peer);
                        mClients.push_back(event.peer);
                    }
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:
                {
                    if(mIsServer)
                        readClientPacket(event);
                    else
                        readServerPacket(event);

                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        if(mIsServer && mClients.size() > 0)
            sendServerPacket();
        else if(mServerPeer != NULL)
            sendClientPacket();

        enet_host_flush(mHost);
    }

    struct ServerPacketHeader
    {
        uint32_t numPlayers;
        size_t tick;
    };

    void NetManager::sendServerPacket()
    {
        FAWorld::World& world = *FAWorld::World::get();

        world.getCurrentPlayer()->startWriting();
        size_t bytesPerClient = world.getCurrentPlayer()->getWriteSize() + sizeof(uint32_t); // the uint is for player id
        size_t packetSize = (sizeof(ServerPacketHeader) + bytesPerClient*(mClients.size() +1)) ; // +1 for the local player

        ENetPacket* packet = enet_packet_create(NULL, packetSize, 0);
        size_t position = 0;

        // write header
        ServerPacketHeader header;
        header.numPlayers = mClients.size() + 1;
        header.tick = mTick;
        writeToPacket(packet, position, header);

        // write server player
        writeToPacket<uint32_t>(packet, position, SERVER_PLAYER_ID);

        world.getCurrentPlayer()->writeTo(packet, position);

        // write all clients
        for(size_t i = 0; i < mClients.size(); i++)
        {
            writeToPacket<uint32_t>(packet, position, mClients[i]->connectID);

            FAWorld::Player* player = world.getPlayer(mClients[i]->connectID);

            player->startWriting();
            player->writeTo(packet, position);
        }

        enet_host_broadcast(mHost, 0, packet);
    }

    struct ClientPacket
    {
        size_t destX;
        size_t destY;
        int32_t levelIndex; // TODO: don't just trust this data
    };

    void NetManager::sendClientPacket()
    {
        ENetPacket* packet = enet_packet_create(NULL, sizeof(ClientPacket), 0);

        auto player = FAWorld::World::get()->getCurrentPlayer();

        ClientPacket data;
        data.destX = player->destination().first;
        data.destY = player->destination().second;
        data.levelIndex = mLevelIndexTmp;

        size_t position = 0;
        writeToPacket(packet, position, data);

        enet_peer_send(mServerPeer, UNRELIABLE_CHANNEL_ID, packet);
    }

    void NetManager::readServerPacket(ENetEvent& event)
    {
        if(event.packet->flags & ENET_PACKET_FLAG_RELIABLE)
        {
            readLevel(event.packet);
            FAWorld::World::get()->setLevel(0);
        }
        else
        {
            FAWorld::World& world = *FAWorld::World::get();

            size_t position = 0;

            ServerPacketHeader header;
            readFromPacket(event.packet, position, header);

            if(header.tick > mLastServerTickProcessed)
            {
                for(size_t i = 0; i < header.numPlayers; i++)
                {
                    uint32_t playerId;
                    readFromPacket<uint32_t>(event.packet, position, playerId);

                    auto player = world.getPlayer(playerId);
                    if(player == NULL)
                    {
                        spawnPlayer(playerId);
                        player = world.getPlayer(playerId);
                    }

                    player->readFrom(event.packet, position);
                }
            }
        }
    }

    void NetManager::readClientPacket(ENetEvent& event)
    {
        ClientPacket data;

        size_t position = 0;
        readFromPacket(event.packet, position, data);

        auto world = FAWorld::World::get();

        auto player = world->getPlayer(event.peer->connectID);

        player->destination().first = data.destX;
        player->destination().second = data.destY;

        if(data.levelIndex != -1 && (player->getLevel() == NULL || data.levelIndex != (int32_t)player->getLevel()->getLevelIndex()))
        {
            auto level = world->getLevel(data.levelIndex);

            if(player->getLevel() != NULL && data.levelIndex < (int32_t)player->getLevel()->getLevelIndex())
                player->mPos = FAWorld::Position(level->downStairsPos().first, level->downStairsPos().second);
            else
                player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);

            player->setLevel(level);
        }
    }

    void NetManager::sendLevel(size_t levelIndex, ENetPeer *peer)
    {
        FAWorld::GameLevel* level = FAWorld::World::get()->getLevel(levelIndex);
        level->startWriting();
        ENetPacket* packet = enet_packet_create(NULL, level->getWriteSize(), ENET_PACKET_FLAG_RELIABLE);
        size_t position = 0;
        level->writeTo(packet, position);

        enet_peer_send(peer, RELIABLE_CHANNEL_ID, packet);
    }

    void NetManager::readLevel(ENetPacket *packet)
    {
        size_t position = 0;
        FAWorld::GameLevel* level = FAWorld::GameLevel::fromPacket(packet, position);

        if(level)
            FAWorld::World::get()->insertLevel(level->getLevelIndex(), level);
    }

    void NetManager::spawnPlayer(uint32_t id)
    {
        FAWorld::World& world = *FAWorld::World::get();
        FAWorld::Player* newPlayer = new FAWorld::Player();
        newPlayer->mPos = FAWorld::Position(76, 68);
        newPlayer->destination() = newPlayer->mPos.current();
        newPlayer->setSpriteClass("warrior");
        world.addPlayer(id, newPlayer);
    }
}
