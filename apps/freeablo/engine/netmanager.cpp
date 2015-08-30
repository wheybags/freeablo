#include "netmanager.h"

#include "../faworld/world.h"

namespace Engine
{
    NetManager::NetManager(bool isServer)
    {
        enet_initialize();

        mAddress.port = 6666;

        mIsServer = isServer;

        if(isServer)
        {
            mAddress.host = ENET_HOST_ANY;
            mHost = enet_host_create(&mAddress, 32, 2, 0, 0);
        }
        else
        {
            enet_address_set_host(&mAddress, "127.0.0.1");
            mHost = enet_host_create(NULL, 32, 2, 0, 0);

            mPeer = enet_host_connect(mHost, &mAddress, 2, 0);

            ENetEvent event;

            if(enet_host_service(mHost, &event, 5000))
            {
                std::cout << "connected" << std::endl;
            }
            else
            {
                std::cout << "connection failed" << std::endl;
            }
        }

        FAWorld::World& world = *FAWorld::World::get();
        FAWorld::Player* newPlayer = new FAWorld::Player();
        newPlayer->mPos = FAWorld::Position(76, 68);
        newPlayer->destination() = newPlayer->mPos.current();

        world.addPlayer(newPlayer);
    }

    NetManager::~NetManager()
    {
        enet_peer_disconnect(mPeer, 0);
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
                    mPeer = event.peer;
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:
                {
                    if(mIsServer)
                        readClientPacket(event.packet);
                    else
                        readServerPacket(event.packet);

                    break;
                }
            }
        }

        if(mPeer)
        {
            if(mIsServer)
                sendServerPacket();
            else
                sendClientPacket();
        }

        enet_host_flush(mHost);
    }

    void NetManager::sendServerPacket()
    {
        FAWorld::World& world = *FAWorld::World::get();

        ENetPacket* packet = enet_packet_create(NULL, world.getCurrentPlayer()->getSize()*2, 0);

        size_t position = 0;
        position = world.getCurrentPlayer()->writeTo(packet, position);
        position = world.getPlayer(1)->writeTo(packet, position);

        enet_peer_send(mPeer, 0, packet);
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

        enet_peer_send(mPeer, 0, packet);
    }

    void NetManager::readServerPacket(ENetPacket* packet)
    {
        FAWorld::World& world = *FAWorld::World::get();

        size_t position = 0;
        position = world.getPlayer(1)->readFrom(packet, position);
        position = world.getCurrentPlayer()->readFrom(packet, position);
    }

    void NetManager::readClientPacket(ENetPacket* packet)
    {
        ClientPacket* data = (ClientPacket*)packet->data;

        auto player = FAWorld::World::get()->getPlayer(1);

        player->destination().first = data->destX;
        player->destination().second = data->destY;
    }
}
