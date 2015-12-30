#include "netmanager.h"

#include "../faworld/world.h"
#include "../faworld/playerfactory.h"

#include <boost/math/special_functions.hpp>

namespace Engine
{
    NetManager* singletonInstance = NULL;

    NetManager* NetManager::get()
    {
        return singletonInstance;
    }

    enum ReliableMessageKind
    {
        Level = 100,
        Sprite = 101
    };

    NetManager::NetManager(bool isServer, const FAWorld::PlayerFactory& playerFactory) : mPlayerFactory(playerFactory)
    {
        assert(singletonInstance == NULL);
        singletonInstance = this;

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
                        auto player = spawnPlayer(-1);

                        // send the client its player id
                        auto packet = enet_packet_create(NULL, sizeof(size_t), ENET_PACKET_FLAG_RELIABLE);
                        size_t position = 0;
                        writeToPacket(packet, position, player->getId());
                        enet_peer_send(event.peer, RELIABLE_CHANNEL_ID, packet);

                        sendLevel(0, event.peer);
                        sendLevel(1, event.peer);
                        mClients.push_back(event.peer);
                        mServerPlayerList[event.peer->connectID] = player;
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

    FARender::FASpriteGroup* NetManager::getServerSprite(size_t index)
    {
        auto retval = FARender::Renderer::get()->loadServerImage(index);

        if(!mAlreadySentServerSprites.count(index))
            mUnknownServerSprites.insert(index);

        return retval;
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
        writeToPacket(packet, position, world.getCurrentPlayer()->getId());

        world.getCurrentPlayer()->writeTo(packet, position);

        // write all clients
        for(auto pair : mServerPlayerList)
        {
            FAWorld::Player* player = pair.second;

            writeToPacket<size_t>(packet, position, player->getId());

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

        if(mUnknownServerSprites.size())
            sendSpriteRequest(mServerPeer);
    }

    void NetManager::readServerPacket(ENetEvent& event)
    {
        if(event.packet->flags & ENET_PACKET_FLAG_RELIABLE)
        {
            size_t position = 0;

            if(mClientRecievedId == false)
            {
                mClientRecievedId = true;

                size_t playerId;
                readFromPacket(event.packet, position, playerId);

                FAWorld::World::get()->getCurrentPlayer()->mId = playerId;

                return;
            }
            int32_t typeHeader;
            readFromPacket(event.packet, position, typeHeader);

            switch(typeHeader)
            {
                case ReliableMessageKind::Level:
                {
                    readLevel(event.packet, position);
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
            FAWorld::World& world = *FAWorld::World::get();

            size_t position = 0;

            ServerPacketHeader header;
            readFromPacket(event.packet, position, header);

            if(header.tick > mLastServerTickProcessed)
            {
                for(size_t i = 0; i < header.numPlayers; i++)
                {
                    size_t playerId;
                    readFromPacket(event.packet, position, playerId);

                    auto player = dynamic_cast<FAWorld::Player*>(world.getActorById(playerId));
                    if(player == NULL)
                        player = spawnPlayer(playerId);

                    player->readFrom(event.packet, position);
                }
            }
        }
    }

    void NetManager::readClientPacket(ENetEvent& event)
    {
        if(event.packet->flags & ENET_PACKET_FLAG_RELIABLE)
        {
            int32_t typeHeader;
            size_t position = 0;
            readFromPacket(event.packet, position, typeHeader);

            switch(typeHeader)
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

            auto player = mServerPlayerList[event.peer->connectID];

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
    }

    void NetManager::sendLevel(size_t levelIndex, ENetPeer *peer)
    {
        int32_t typeHeader = ReliableMessageKind::Level;

        FAWorld::GameLevel* level = FAWorld::World::get()->getLevel(levelIndex);
        level->startWriting();
        ENetPacket* packet = enet_packet_create(NULL, level->getWriteSize() + sizeof(int32_t), ENET_PACKET_FLAG_RELIABLE);
        size_t position = 0;
        writeToPacket(packet, position, typeHeader);
        level->writeTo(packet, position);

        enet_peer_send(peer, RELIABLE_CHANNEL_ID, packet);
    }

    void NetManager::readLevel(ENetPacket *packet, size_t& position)
    {
        FAWorld::GameLevel* level = FAWorld::GameLevel::fromPacket(packet, position);

        if(level)
            FAWorld::World::get()->insertLevel(level->getLevelIndex(), level);
    }

    void NetManager::sendSpriteRequest(ENetPeer* peer)
    {
        int32_t typeHeader = ReliableMessageKind::Sprite;
        size_t numSprites = mUnknownServerSprites.size();

        ENetPacket* packet = enet_packet_create(NULL, (numSprites+1) * sizeof(size_t) + sizeof(int32_t), ENET_PACKET_FLAG_RELIABLE);

        size_t position = 0;
        writeToPacket(packet, position, typeHeader);
        writeToPacket(packet, position, numSprites);

        for(size_t index : mUnknownServerSprites)
        {
            writeToPacket(packet, position, index);
            mAlreadySentServerSprites.insert(index);

            std::cout << "requesting " << index << std::endl;
        }

        mUnknownServerSprites.clear();

        enet_peer_send(peer, RELIABLE_CHANNEL_ID, packet);
    }

    void NetManager::readSpriteRequest(ENetPacket* packet, ENetPeer* peer, size_t& position)
    {
        size_t numSprites;
        readFromPacket(packet, position, numSprites);

        std::vector<size_t> requestedSprites(numSprites);

        for(size_t i = 0; i < numSprites; i++)
            readFromPacket(packet, position, requestedSprites[i]);

        std::vector<std::string> paths(numSprites);

        auto renderer = FARender::Renderer::get();

        size_t size = 0;

        for(size_t i = 0; i < numSprites; i++)
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

        for(size_t i = 0; i < numSprites; i++)
        {
            writeToPacket(responsePacket, writePosition, requestedSprites[i]);

            std::string& path = paths[i];
            char c;

            for(size_t j = 0; j < path.size(); j++)
            {
                c = path[j];
                writeToPacket(responsePacket, writePosition, c);
            }

            c = 0;
            writeToPacket(responsePacket, writePosition, c);
        }

        enet_peer_send(peer, RELIABLE_CHANNEL_ID, responsePacket);
    }

    void NetManager::readSpriteResponse(ENetPacket* packet, size_t& position)
    {
        size_t numSprites;
        readFromPacket(packet, position, numSprites);

        auto renderer = FARender::Renderer::get();

        for(size_t i = 0; i < numSprites; i++)
        {
            size_t index;
            readFromPacket(packet, position, index);

            std::string path;

            char c = 0;

            do
            {
                readFromPacket(packet, position, c);

                if(c)
                    path += c;
            } while(c);

            renderer->fillServerSprite(index, path);

            std::cout << "response recieved " << index << " " << path << std::endl;
        }
    }

    FAWorld::Player* NetManager::spawnPlayer(int32_t id)
    {
        auto newPlayer = mPlayerFactory.create("Warrior");
        newPlayer->mPos = FAWorld::Position(76, 68);
        newPlayer->destination() = newPlayer->mPos.current();

        if(id != -1)
            newPlayer->mId = id;

        return newPlayer;
    }
}
