#include "client.h"

#include <limits>
#include <iostream>

#include "../../faworld/gamelevel.h"
#include "../../faworld/world.h"
#include "../../faworld/player.h"

#include "netcommon.h"



#include "netmanager.h" // TODO: REMOVE

namespace Engine
{
    Client::Client()
    {
        mAddress.port = 6666;

        enet_address_set_host(&mAddress, "127.0.0.1");
        mHost = enet_host_create(NULL, 32, 2, 0, 0);
        mServerPeer = enet_host_connect(mHost, &mAddress, 2, 0);
        enet_peer_timeout(mServerPeer, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());

        ENetEvent event;

        if (enet_host_service(mHost, &event, 5000))
        {
            std::cout << "connected" << std::endl;
        }
        else
        {
            std::cout << "connection failed" << std::endl;
        }
    }

    Client::~Client()
    {
        if (mServerPeer != NULL)
            enet_peer_disconnect(mServerPeer, 0);
        
        enet_host_destroy(mHost);
    }

    void Client::update(uint32_t tick)
    {
        #ifdef ENABLE_NET_STALL_ON_TIMEOUT
        {
            uint32_t stallThresh = getStallThreshold();

            uint32_t diff = tick - mClientTickWhenLastServerPacketReceived;
            if (diff > stallThresh)
                std::cout << "STALLED BY NETWORK" << std::endl;

            while (diff > stallThresh)
            {
                updateImp(tick);
                diff = tick - mClientTickWhenLastServerPacketReceived;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        #endif

        updateImp(tick);
    }

    void Client::updateImp(uint32_t tick)
    {
        // TODO: remove this block when we handle level switching properly
        auto player = FAWorld::World::get()->getCurrentPlayer();
        if (player->getLevel())
            mLevelIndexTmp = player->getLevel()->getLevelIndex();
        else
            mLevelIndexTmp = -1;

        ENetEvent event;

        while(enet_host_service(mHost, &event, 0))
        {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:
                {
                    readServerPacket(event, tick);

                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    std::cout << "DISCONNECT" << std::endl;
                    break;
                }

                case ENET_EVENT_TYPE_NONE:
                {
                    break;
                }

                default:
                {
                    std::cout << "UNHANDLED" << std::endl;
                    break;
                }
            }
        }

        if(mServerPeer != NULL)
            sendClientPacket();

        enet_host_flush(mHost);
    }

    void Client::receiveLevel(ENetPacket* packet, size_t position)
    {
        FAWorld::GameLevel* level = FAWorld::GameLevel::fromPacket(packet, position);

        if (level)
            FAWorld::World::get()->insertLevel(level->getLevelIndex(), level);
    }

    void Client::readServerPacket(ENetEvent& event, uint32_t tick)
    {
        if (event.packet->flags & ENET_PACKET_FLAG_RELIABLE)
        {
            size_t position = 0;

            if (mClientRecievedId == false)
            {
                mClientRecievedId = true;

                size_t playerId;
                readFromPacket(event.packet, position, playerId);

                FAWorld::World::get()->getCurrentPlayer()->mId = playerId;

                return;
            }
            int32_t typeHeader;
            readFromPacket(event.packet, position, typeHeader);

            switch (typeHeader)
            {
                case ReliableMessageKind::Level:
                {
                    receiveLevel(event.packet, position);
                    FAWorld::World::get()->setLevel(0);
                    break;
                }

                case ReliableMessageKind::Sprite:
                {
                    readSpriteResponse(event.packet, position);
                    break;
                }
            }
        }
        else
        {
            mClientTickWhenLastServerPacketReceived = tick;

            FAWorld::World& world = *FAWorld::World::get();

            Serial::ReadBitStream stream(event.packet->data, event.packet->dataLength);

            ServerPacketHeader header;
            header.faSerial(stream);

            if (header.tick > mLastServerTickProcessed)
            {
                Serial::Error::Error err = Serial::Error::Success;

                for (size_t i = 0; i < header.numPlayers; i++)
                {
                    int32_t classId;
                    int32_t actorId;

                    if (err == Serial::Error::Success)
                        err = stream.handleInt<0, 1024>(classId);
                    if (err == Serial::Error::Success)
                        err = stream.handleInt32(actorId);


                    if (err == Serial::Error::Success)
                    {
                        FAWorld::Actor* actor = world.getActorById(actorId);
                        if (actor == NULL)
                        {
                            actor = dynamic_cast<FAWorld::Actor*>(FAWorld::NetObject::construct(classId));
                            actor->mId = actorId;
                        }

                        err = actor->streamHandle(stream);
                    }


                    if (err != Serial::Error::Success)
                    {
                        std::cerr << "Serialisation read error " << Serial::Error::getName(err) << std::endl;
                        exit(1);
                    }
                }

                assert(stream.verifyZeros() && "INVALID PADDING DATA AT END");
            }
        }
    }

    void Client::sendClientPacket()
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

        if (mUnknownServerSprites.size())
            sendSpriteRequest();
    }

    void Client::sendSpriteRequest()
    {
        int32_t typeHeader = ReliableMessageKind::Sprite;
        size_t numSprites = mUnknownServerSprites.size();

        ENetPacket* packet = enet_packet_create(NULL, (numSprites + 1) * sizeof(size_t) + sizeof(int32_t), ENET_PACKET_FLAG_RELIABLE);

        size_t position = 0;
        writeToPacket(packet, position, typeHeader);
        writeToPacket(packet, position, numSprites);

        for (size_t index : mUnknownServerSprites)
        {
            writeToPacket(packet, position, index);
            mAlreadySentServerSprites.insert(index);

            std::cout << "requesting " << index << std::endl;
        }

        mUnknownServerSprites.clear();

        enet_peer_send(mServerPeer, RELIABLE_CHANNEL_ID, packet);
    }

    void Client::readSpriteResponse(ENetPacket* packet, size_t& position)
    {
        size_t numSprites;
        readFromPacket(packet, position, numSprites);

        auto renderer = FARender::Renderer::get();

        for (size_t i = 0; i < numSprites; i++)
        {
            size_t index;
            readFromPacket(packet, position, index);

            std::string path;

            char c = 0;

            do
            {
                readFromPacket(packet, position, c);

                if (c)
                    path += c;
            } while (c);

            renderer->fillServerSprite(index, path);

            std::cout << "response recieved " << index << " " << path << std::endl;
        }
    }

    FARender::FASpriteGroup* Client::getServerSprite(size_t index)
    {
        auto retval = FARender::Renderer::get()->loadServerImage(index);

        if(!mAlreadySentServerSprites.count(index))
            mUnknownServerSprites.insert(index);

        return retval;
    }
}