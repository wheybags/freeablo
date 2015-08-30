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
                    if(!mIsServer)
                        world.getPlayer()->readFrom(event.packet, 0);

                    enet_packet_destroy(event.packet);
                    break;
                }
            }
        }

        if(mIsServer && mPeer)
        {
            ENetPacket* packet = enet_packet_create(NULL, world.getPlayer()->getSize(), 0);
            world.getPlayer()->writeTo(packet, 0);
            enet_peer_send(mPeer, 0, packet);
        }

        enet_host_flush(mHost);
    }
}
