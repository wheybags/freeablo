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
        if (!mInputs.count(tick))
            return boost::none;

        for (const auto& pair : mPeers)
        {
            if (pair.second.lastTick + 10 < tick)
                return boost::none;
        }

        std::vector<FAWorld::PlayerInput> retval;
        mInputs.at(tick).swap(retval);
        mInputs.erase(tick);
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

        if (mWorld.getCurrentTick() != mLastSentTick)
        {
            auto localInputs = mLocalInputHandler.getAndClearInputs();
            mInputsBuffer.insert(mInputsBuffer.begin(), localInputs.begin(), localInputs.end());

            // We can't have the server pulling directly from mInputsBuffer because then it would
            // execute inputs at an earlier tick than the clients. So what we do here is essentially
            // sending the buffer to all the clients, then "sending" it to the server as well, by means
            // of the mInputs map.
            sendInputsToClients(mInputsBuffer);
            mInputs[mWorld.getCurrentTick()] = std::move(mInputsBuffer);

            mLastSentTick = mWorld.getCurrentTick();
        }
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
        saver.save(mDoFullVerify);
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

            case MessageType::ClientUpdateToServer:
            {
                receiveClientUpdate(loader, mPeers.at(event.peer->connectID));
                return;
            }

            case MessageType::InputsToClient:
            case MessageType::MapToClient:
                invalid_enum(MessageType, type);
        }

        invalid_enum(MessageType, type);
    }

    void Server::sendInputsToClients(std::vector<FAWorld::PlayerInput>& inputs)
    {

        Serial::TextWriteStream stream;
        FASaveGame::GameSaver saver(stream);

        saver.save(uint8_t(MessageType::InputsToClient));

        saver.save(mWorld.getCurrentTick());
        saver.save(uint32_t(inputs.size()));
        for (auto& input : inputs)
            input.save(saver);

        if (mDoFullVerify)
        {
            Serial::TextWriteStream worldStream;
            FASaveGame::GameSaver worldSaver(worldStream);
            mWorld.save(worldSaver);

            auto worldData = worldStream.getData();
            std::string strData((const char*)worldData.first, worldData.second);
            saver.save(strData);
        }

        auto data = stream.getData();

        // does not take ownership of data
        ENetPacket* packet = enet_packet_create(data.first, data.second, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(mHost, RELIABLE_CHANNEL_ID, packet);
    }

    void Server::receiveClientUpdate(FASaveGame::GameLoader& loader, Peer& peer)
    {
        peer.lastTick = loader.load<FAWorld::Tick>();
        uint32_t size = loader.load<uint32_t>();
        size_t start = mInputsBuffer.size();
        mInputsBuffer.resize(mInputsBuffer.size() + size);
        for (uint32_t i = 0; i < size; i++)
            mInputsBuffer[start + i].load(loader);
    }
}
