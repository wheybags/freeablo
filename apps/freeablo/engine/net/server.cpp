#include "server.h"

#include <iostream>

#include "../../faworld/position.h"
#include "../../faworld/player.h"
#include "../../faworld/monster.h"

#include "netcommon.h"

namespace Engine
{
    Server::Server(const FAWorld::PlayerFactory& factory) : mPlayerFactory(factory)
    {
        mAddress.port = 6666;
        mAddress.host = ENET_HOST_ANY;
        mHost = enet_host_create(&mAddress, 32, 2, 0, 0);
    }

    Server::~Server()
    {
        for (size_t i = 0; i < mClients.size(); i++)
            enet_peer_disconnect(mClients[i], 0);
        
        enet_host_destroy(mHost);
    }

    void Server::update(uint32_t tick)
    {
        if (tick % 2 == 0) // server only send packets every second tick
            return;

        #ifdef ENABLE_NET_STALL_ON_TIMEOUT
        {
            uint32_t stallThresh = getStallThreshold();

            for (size_t i = 0; i < mClients.size(); i++)
            {
                uint32_t diff = tick - mServersClientData[mClients[i]->connectID].lastReceiveTick;
                if (diff > stallThresh)
                    std::cout << "STALLED BY NETWORK" << std::endl;

                while (diff > stallThresh)
                {
                    updateImp(tick);
                    diff = tick - mServersClientData[mClients[i]->connectID].lastReceiveTick;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        }
        #endif

        updateImp(tick);
    }

    void Server::updateImp(uint32_t tick)
    {
        ENetEvent event;
        while(enet_host_service(mHost, &event, 0))
        {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {

                    #ifdef ENABLE_NET_STALL_ON_TIMEOUT
                        enet_peer_timeout(event.peer, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
                    #endif

                    auto player = spawnPlayer(-1);

                    // send the client its player id
                    auto packet = enet_packet_create(NULL, sizeof(size_t), ENET_PACKET_FLAG_RELIABLE);
                    size_t position = 0;
                    writeToPacket(packet, position, player->getId());
                    enet_peer_send(event.peer, RELIABLE_CHANNEL_ID, packet);

                    // TODO: send levels on-demand
                    sendLevel(0, event.peer);
                    sendLevel(1, event.peer);
                    mClients.push_back(event.peer);
                    mServersClientData[event.peer->connectID] = ClientData(player);

                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:
                {
                    readClientPacket(event, tick);
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

        if(mClients.size() > 0)
            sendServerPacket(tick);

        enet_host_flush(mHost);
    }

    FAWorld::Player* Server::spawnPlayer(int32_t id)
    {
        auto newPlayer = mPlayerFactory.create("Warrior");
        newPlayer->mPos = FAWorld::Position(76, 68);
        newPlayer->destination() = newPlayer->mPos.current();

        if (id != -1)
            newPlayer->mId = id;

        return newPlayer;
    }

    void Server::sendLevel(size_t levelIndex, ENetPeer *peer)
    {
        int32_t typeHeader = ReliableMessageKind::Level;

        ENetPacket* packet = enet_packet_create(NULL, sizeof(int32_t), ENET_PACKET_FLAG_RELIABLE);
        size_t position = 0;
        writeToPacket(packet, position, typeHeader);

        FAWorld::GameLevel* level = FAWorld::World::get()->getLevel(levelIndex);
        level->saveToPacket(packet, position);

        enet_peer_send(peer, RELIABLE_CHANNEL_ID, packet);
    }

    void Server::sendServerPacket(uint32_t tick)
    {
        FAWorld::World& world = *FAWorld::World::get();

        size_t packetSize = 512;
        ENetPacket* packet = enet_packet_create(NULL, packetSize, ENET_PACKET_FLAG_UNSEQUENCED);

        Serial::WriteBitStream stream(packet->data, packet->dataLength);

        // write header
        ServerPacketHeader header;
        header.numPlayers = 0;
        header.tick = tick;

        header.faSerial(stream);

        std::vector<FAWorld::Actor*> allActors;
        world.getAllActors(allActors);

        std::sort(allActors.begin(), allActors.end(), [](FAWorld::Actor* a, FAWorld::Actor* b) -> bool
        {
            return a->getPriority() > b->getPriority();
        });

        Serial::Error::Error err = Serial::Error::Success;

        bool packetFull = false;
        for (auto actor : allActors)
        {
            if (((bool)dynamic_cast<FAWorld::Monster*>(actor)))
                continue;

            if (!packetFull)
            {
                int32_t classId = actor->getClassId();
                int32_t actorId = actor->getId();

                if (err == Serial::Error::Success)
                    err = stream.handleInt<0, 1024>(classId);
                if (err == Serial::Error::Success)
                    err = stream.handleInt32(actorId);
                if (err == Serial::Error::Success)
                    err = actor->streamHandle(stream);

                if (err == Serial::Error::Success)
                {
                    header.numPlayers++;
                }
                else if (err == Serial::Error::EndOfStream)
                {
                    packetFull = true;
                }
                else
                {
                    std::cerr << "Serialisation send error " << Serial::Error::getName(err) << std::endl;
                    exit(1);
                }
            }

            actor->tickDone(!packetFull);
        }

        stream.fillWithZeros();

        // rewrite packet header with correct object count
        stream.seek(0, Serial::BSPos::Start);
        header.faSerial(stream);

        enet_host_broadcast(mHost, UNRELIABLE_CHANNEL_ID, packet);
    }


    void Server::readClientPacket(ENetEvent& event, uint32_t tick)
    {
        if (event.packet->flags & ENET_PACKET_FLAG_RELIABLE)
        {
            int32_t typeHeader;
            size_t position = 0;
            readFromPacket(event.packet, position, typeHeader);

            switch (typeHeader)
            {
                case ReliableMessageKind::Sprite:
                {
                    readSpriteRequest(event.packet, event.peer, position);
                    break;
                }
            }
        }
        else
        {
            ClientPacket data;

            size_t position = 0;
            readFromPacket(event.packet, position, data);

            auto world = FAWorld::World::get();


            mServersClientData[event.peer->connectID].lastReceiveTick = tick;

            //std::cout << "GOT MESSAGE " << mTick << " " << mServersClientData[event.peer->connectID].lastReceiveTick << std::endl;

            auto player = mServersClientData[event.peer->connectID].player;

            player->destination().first = data.destX;
            player->destination().second = data.destY;

            if (data.levelIndex != -1 && (player->getLevel() == NULL || data.levelIndex != (int32_t)player->getLevel()->getLevelIndex()))
            {
                auto level = world->getLevel(data.levelIndex);

                if (player->getLevel() != NULL && data.levelIndex < (int32_t)player->getLevel()->getLevelIndex())
                    player->mPos = FAWorld::Position(level->downStairsPos().first, level->downStairsPos().second);
                else
                    player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);

                player->setLevel(level);
            }
        }
    }

    void Server::readSpriteRequest(ENetPacket* packet, ENetPeer* peer, size_t& position)
    {
        size_t numSprites;
        readFromPacket(packet, position, numSprites);

        std::vector<size_t> requestedSprites(numSprites);

        for (size_t i = 0; i < numSprites; i++)
            readFromPacket(packet, position, requestedSprites[i]);

        std::vector<std::string> paths(numSprites);

        auto renderer = FARender::Renderer::get();

        size_t size = 0;

        for (size_t i = 0; i < numSprites; i++)
        {
            paths[i] = renderer->getPathForIndex(requestedSprites[i]);
            size += paths[i].size();
            size += 1; // null terminator

            std::cout << "responding to: " << requestedSprites[i] << " " << paths[i] << std::endl;

        }

        size += numSprites * sizeof(size_t);
        size += sizeof(int32_t); // type header
        size += sizeof(size_t); // numSprites count

        int32_t typeHeader = ReliableMessageKind::Sprite;

        ENetPacket* responsePacket = enet_packet_create(NULL, size, ENET_PACKET_FLAG_RELIABLE);
        size_t writePosition = 0;

        writeToPacket(responsePacket, writePosition, typeHeader);
        writeToPacket(responsePacket, writePosition, numSprites);

        for (size_t i = 0; i < numSprites; i++)
        {
            writeToPacket(responsePacket, writePosition, requestedSprites[i]);

            std::string& path = paths[i];
            char c;

            for (size_t j = 0; j < path.size(); j++)
            {
                c = path[j];
                writeToPacket(responsePacket, writePosition, c);
            }

            c = 0;
            writeToPacket(responsePacket, writePosition, c);
        }

        enet_peer_send(peer, RELIABLE_CHANNEL_ID, responsePacket);
    }
}