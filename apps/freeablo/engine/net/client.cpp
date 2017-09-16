#include "client.h"

#include <limits>
#include <iostream>
#include <thread>

#include "../../faworld/gamelevel.h"
#include "../../faworld/world.h"
#include "../../faworld/player.h"
#include "boost/variant/get.hpp"

#include "netcommon.h"
#include "netops.h"


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

        // don't send client updates until we're fully connected
        if(mServerPeer != nullptr && FAWorld::World::get()->getCurrentLevel() != nullptr)
            sendClientPacket();

        enet_host_flush(mHost);
    }

    void Client::receiveLevel(std::shared_ptr<ReadPacket> packet)
    {
        uint32_t dataSize = 0;
        packet->reader.handleInt32(dataSize);

        std::string data(dataSize, '\0');
        packet->reader.handleString((uint8_t*)&data[0], dataSize);

        auto level = FAWorld::GameLevel::loadFromString(data);
        FAWorld::World::get()->insertLevel(level->getLevelIndex(), level);
    }

    void Client::readServerPacket(ENetEvent& event, uint32_t tick)
    {
        std::shared_ptr<ReadPacket> packet = getReadPacket(event.packet);

        Serial::Error::Error err = Serial::Error::Success;

        switch (packet->type)
        {
            case PacketType::NewClient:
                packet->reader.handleInt32(FAWorld::World::get()->getCurrentPlayer()->mId);
                break;

            case PacketType::Level:
                receiveLevel(packet);
                break;

            case PacketType::Sprite:
                err = readSpriteResponse(packet);
                break;

            case PacketType::GameUpdateMessage:
                handleGameUpdateMessage(packet, tick);
                break;

            default:
                assert(false && "BAD PACKET TYPE RECEIVED");
        }

        if (err != Serial::Error::Success)
        {
            std::cerr << "Serialisation read error " << Serial::Error::getName(err) << std::endl;
            exit(1);
        }
    }

    void Client::handleGameUpdateMessage(std::shared_ptr<ReadPacket> packet, uint32_t tick)
    {
        mClientTickWhenLastServerPacketReceived = tick;

        FAWorld::World& world = *FAWorld::World::get();

        ServerPacketHeader header;
        header.faSerial(packet->reader);

        if (header.tick > mLastServerTickProcessed)
        {
            Serial::Error::Error err = Serial::Error::Success;

            for (size_t i = 0; i < header.numPlayers; i++)
            {
                int32_t classId = 0;
                int32_t actorId = 0;

                if (err == Serial::Error::Success)
                    err = packet->reader.handleInt<0, 1024>(classId);
                if (err == Serial::Error::Success)
                    err = packet->reader.handleInt32(actorId);


                if (err == Serial::Error::Success)
                {
                    FAWorld::Actor* actor = world.getActorById(actorId);

                    if (!actor && actorId == world.getCurrentPlayer()->mId)
                        actor = world.getCurrentPlayer();

                    if (actor == NULL)
                    {
                        actor = dynamic_cast<FAWorld::Actor*>(FAWorld::NetObject::construct(classId));
                        actor->mId = actorId;
                    }

                    err = actor->streamHandle(packet->reader);
                }


                if (err != Serial::Error::Success)
                {
                    std::cerr << "Serialisation read error " << Serial::Error::getName(err) << std::endl;
                    exit(1);
                }
            }

            assert(packet->reader.verifyZeros() && "INVALID PADDING DATA AT END");
        }
    }

    void Client::sendClientPacket()
    {
        auto packet = getWritePacket(Engine::PacketType::ClientToServerUpdate, 0, false, Engine::WritePacketResizableType::Resizable);

        auto player = FAWorld::World::get()->getCurrentPlayer();

        ClientPacket data;
        data.destX = player->mMoveHandler.getDestination().first;
        data.destY = player->mMoveHandler.getDestination().second;

        data.targetActorId = -1;

        if (auto actorPtr = boost::get<FAWorld::Actor*> (&player->mTarget))
            data.targetActorId = (*actorPtr)->getId();

        packet.writer.handleObject(data);

        sendPacket(packet, mServerPeer);

        if (mUnknownServerSprites.size())
            sendSpriteRequest();
    }

    void Client::sendSpriteRequest()
    {
        WritePacket packet = getWritePacket(PacketType::Sprite, 0, true, WritePacketResizableType::Resizable);
        uint32_t numSprites = mUnknownServerSprites.size();
        packet.writer.handleInt32(numSprites);

        for (uint32_t index : mUnknownServerSprites)
        {
            packet.writer.handleInt32(index);
            mAlreadySentServerSprites.insert(index);

            std::cout << "requesting " << index << std::endl;
        }

        mUnknownServerSprites.clear();
        sendPacket(packet, mServerPeer);
    }

    void Client::sendLevelChangePacket(int32_t level)
    {
        WritePacket packet = getWritePacket(PacketType::LevelChangeRequest, 0, true, WritePacketResizableType::Resizable);

        int32_t blah = level;
        packet.writer.handleInt32(blah);
        sendPacket(packet, mServerPeer);
    }


    Serial::Error::Error Client::readSpriteResponse(std::shared_ptr<ReadPacket> packet)
    {
        uint32_t numEntries = 0;
        serialise_int32(packet->reader, numEntries);

        std::cout << "SPRITE RESPONSE FOR " << numEntries << " SPRITES" << std::endl;

        auto renderer = FARender::Renderer::get();

        for (uint32_t i = 0; i < numEntries; i++)
        {
            uint32_t index = 0;
            uint32_t len = 0;
            serialise_int32(packet->reader, index);
            serialise_int32(packet->reader, len);

            std::string path(len, '\0');
            serialise_str(packet->reader, (uint8_t*)&path[0], len);

            renderer->fillServerSprite(index, path);

            std::cout << "SPRITE RESPONSE:" << index << " " << path << std::endl;
        }

        return Serial::Error::Success;
    }

    FARender::FASpriteGroup* Client::getServerSprite(size_t index)
    {
        auto retval = FARender::Renderer::get()->loadServerImage(index);

        if(!mAlreadySentServerSprites.count(index))
            mUnknownServerSprites.insert(index);

        return retval;
    }
}
