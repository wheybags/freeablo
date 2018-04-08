#include "server.h"
#include "../../fasavegame/gameloader.h"
#include "../../faworld/player.h"
#include "../../faworld/world.h"
#include "../enginemain.h"
#include "../localinputhandler.h"
#include <misc/assert.h>
#include <serial/textstream.h>

namespace Engine
{
    Server::Server(FAWorld::World& world, LocalInputHandler& localInputHandler) : mWorld(world), mLocalInputHandler(localInputHandler)
    {
        mAddress.port = 6666;
        mAddress.host = ENET_HOST_ANY;
        mHost = enet_host_create(&mAddress, 32, 2, 0, 0);
    }

    Server::~Server()
    {
        enet_host_destroy(mHost);
        mHost = nullptr;
    }

    boost::optional<std::vector<FAWorld::PlayerInput>> Server::getAndClearInputs(FAWorld::Tick tick)
    {
        UNUSED_PARAM(tick);

        std::vector<FAWorld::PlayerInput> retval;
        mInputs.swap(retval);
        return retval;
    }

    void Server::update()
    {
        ENetEvent event = {};

        while (enet_host_service(mHost, &event, 0))
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    onPeerConnect(event);
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    readPeerPacket(event);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    break;
                }
                case ENET_EVENT_TYPE_NONE:
                {
                    break;
                }
                default:
                    invalid_enum(ENetEventType, event.type);
            }
        }

        bool allHaveMap = true;
        for (const auto& pair : mPeers)
        {
            if (!pair.second.hasMap)
            {
                allHaveMap = false;
                break;
            }
        }

        if (allHaveMap)
            EngineMain::get()->mPaused = false;

        auto localInputs = mLocalInputHandler.getAndClearInputs();
        mInputs.insert(mInputs.begin(), localInputs.begin(), localInputs.end());

        sendInputsToClients();
    }

    void Server::onPeerConnect(const ENetEvent& event)
    {
        mPeers[event.peer->connectID] = Peer(event.peer);
        EngineMain::get()->mPaused = true;

        Serial::TextWriteStream stream;
        FASaveGame::GameSaver saver(stream);

        FAWorld::Player* newPlayer = EngineMain::get()->mPlayerFactory->create(mWorld, "Warrior");
        mWorld.registerPlayer(newPlayer);
        FAWorld::GameLevel* level = mWorld.getLevel(0);
        newPlayer->teleport(level, FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second));

        saver.save(uint8_t(MessageType::MapToClient));
        saver.save(newPlayer->getId());
        mWorld.save(saver);

        auto data = stream.getData();

        // does not take ownership of data
        ENetPacket* packet = enet_packet_create(data.first, data.second, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(event.peer, RELIABLE_CHANNEL_ID, packet);
    }

    void Server::readPeerPacket(const ENetEvent& event)
    {
        Serial::TextReadStream stream(std::string((const char*)event.packet->data, event.packet->dataLength));
        FASaveGame::GameLoader loader(stream);

        MessageType type = MessageType(loader.load<uint8_t>());

        switch (type)
        {
            case MessageType::AcknowledgeMapToServer:
            {
                mPeers.at(event.peer->connectID).hasMap = true;
                return;
            }

            case MessageType::InputsToServer:
            {
                receiveInputs(loader);
                return;
            }

            case MessageType::MapToClient:
                invalid_enum(MessageType, type);
        }

        invalid_enum(MessageType, type);
    }

    void Server::sendInputsToClients()
    {
        Serial::TextWriteStream stream;
        FASaveGame::GameSaver saver(stream);

        saver.save(uint8_t(MessageType::InputsToClient));

        saver.save(mWorld.getCurrentTick());
        saver.save(uint32_t(mInputs.size()));
        for (auto& input : mInputs)
            input.save(saver);

        auto data = stream.getData();

        // does not take ownership of data
        ENetPacket* packet = enet_packet_create(data.first, data.second, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(mHost, RELIABLE_CHANNEL_ID, packet);
    }

    void Server::receiveInputs(FASaveGame::GameLoader& loader)
    {
        uint32_t size = loader.load<uint32_t>();
        size_t start = mInputs.size();
        mInputs.resize(mInputs.size() + size);
        for (uint32_t i = 0; i < size; i++)
            mInputs[start + i].load(loader);
    }
}
