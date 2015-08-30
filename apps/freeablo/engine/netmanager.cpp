#include "netmanager.h"

#include "../faworld/world.h"

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
        FAWorld::World& world = *FAWorld::World::get();

        ENetEvent event;

        while(enet_host_service(mHost, &event, 0))
        {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    if(mIsServer)
                    {
                        spawnPlayer(event.peer->connectID);
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
    };

    void NetManager::sendServerPacket()
    {
        FAWorld::World& world = *FAWorld::World::get();

        size_t bytesPerClient = world.getCurrentPlayer()->getSize() + sizeof(uint32_t); // the uint is for player id
        size_t packetSize = sizeof(ServerPacketHeader) + bytesPerClient*(mClients.size() +1); // +1 for the local player

        ENetPacket* packet = enet_packet_create(NULL, packetSize, 0);
        size_t position = 0;

        // write header
        ServerPacketHeader header;
        header.numPlayers = mClients.size() + 1;
        writeToPacket(packet, position, header);

        // write server player
        writeToPacket<uint32_t>(packet, position, SERVER_PLAYER_ID);
        position = world.getCurrentPlayer()->writeTo(packet, position);

        // write all clients
        for(size_t i = 0; i < mClients.size(); i++)
        {
            writeToPacket<uint32_t>(packet, position, mClients[i]->connectID);
            position = world.getPlayer(mClients[i]->connectID)->writeTo(packet, position);
        }

        enet_host_broadcast(mHost, 0, packet);
    }

    struct ClientPacket
    {
        size_t destX;
        size_t destY;
    };

    void NetManager::sendClientPacket()
    {
        ENetPacket* packet = enet_packet_create(NULL, sizeof(ClientPacket), 0);

        auto player = FAWorld::World::get()->getCurrentPlayer();

        ClientPacket* data = (ClientPacket*)packet->data;
        data->destX = player->destination().first;
        data->destY = player->destination().second;

        enet_peer_send(mServerPeer, 0, packet);
    }

    void NetManager::readServerPacket(ENetEvent& event)
    {
        FAWorld::World& world = *FAWorld::World::get();

        size_t position = 0;

        ServerPacketHeader header;
        readFromPacket(event.packet, position, header);

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

            position = player->readFrom(event.packet, position);
        }
    }

    void NetManager::readClientPacket(ENetEvent& event)
    {
        ClientPacket* data = (ClientPacket*)event.packet->data;

        auto player = FAWorld::World::get()->getPlayer(event.peer->connectID);

        player->destination().first = data->destX;
        player->destination().second = data->destY;
    }

    void NetManager::spawnPlayer(uint32_t id)
    {
        FAWorld::World& world = *FAWorld::World::get();
        FAWorld::Player* newPlayer = new FAWorld::Player();
        newPlayer->mPos = FAWorld::Position(76, 68);
        newPlayer->destination() = newPlayer->mPos.current();

        world.addPlayer(id, newPlayer);
    }
}
