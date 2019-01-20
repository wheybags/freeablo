#include "server.h"
#include "../../fasavegame/gameloader.h"
#include "../../faworld/player.h"
#include "../../faworld/world.h"
#include "../enginemain.h"
#include "../localinputhandler.h"
#include <iostream>
#include <misc/assert.h>
#include <serial/textstream.h>

namespace Engine
{
    const char* Server::SERVER_ADDRESS = "127.0.0.1";

    Server::Server(FAWorld::World& world, LocalInputHandler& localInputHandler) : mWorld(world), mLocalInputHandler(localInputHandler)
    {
        if (0 != enet_initialize())
        {
            std::cerr << "Unable to initialize networking library." << std::endl;
        }
        mAddress.port = 6666;
        enet_address_set_host(&mAddress, SERVER_ADDRESS);
        mHost = enet_host_create(&mAddress, 32, 2, 0, 0);
        mHost->checksum = enet_crc32;
    }

    Server::~Server()
    {
        enet_host_destroy(mHost);
        enet_deinitialize();
    }

    boost::optional<std::vector<FAWorld::PlayerInput>> Server::getAndClearInputs(FAWorld::Tick tick)
    {
        if (!mInputs.count(tick))
            return boost::none;

        for (const auto& pair : mPeers)
        {
            if (pair.second.hasMap && pair.second.lastTick + 10 < tick)
                return boost::none;
        }

        std::vector<FAWorld::PlayerInput> retval;
        mInputs.at(tick).swap(retval);
        mInputs.erase(tick);
        return retval;
    }

    void Server::update()
    {
        ENetEvent event;

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
                    onPeerDisconnect(event);
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

        if (mWorld.getCurrentTick() != mLastSentTick)
        {
            // see onPeerConnect for an explanation of this
            for (auto& peer : mPeers)
            {
                if (!peer.second.mapSent && peer.second.actorId != -1)
                {
                    sendMapToPeer(peer.second);
                    peer.second.mapSent = true;
                }
            }

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
        mPeers[mNextPeerId] = Peer(event.peer);
        event.peer->data = reinterpret_cast<void*>(size_t(mNextPeerId));

        // We pass the player joining as a PlayerInput so that other clients will now about them connecting.
        // Later on, the game will create an FAWorld::Player object for the player, and inform us of this through registerNewPlayer().
        // Once that is done, we have an actor for the player, so we can send them the map, which we do from update() by checking for
        // peers that haven't been sent a map yet, but do have an actor (actorId != -1).
        EngineMain::get()->getLocalInputHandler()->addInput(FAWorld::PlayerInput(FAWorld::PlayerInput::PlayerJoinedData{mNextPeerId}, -1));
        mNextPeerId++;
    }

    void Server::registerNewPlayer(FAWorld::Player* player, uint32_t peerId)
    {
        // see onPeerConnect for an explanation of this
        Peer& peer = mPeers.at(peerId);
        peer.actorId = player->getId();
    }

    void Server::onPeerDisconnect(const ENetEvent& event)
    {
        uint32_t peerId = uint32_t(size_t(event.peer->data));
        EngineMain::get()->getLocalInputHandler()->addInput(FAWorld::PlayerInput(FAWorld::PlayerInput::PlayerLeftData{}, mPeers.at(peerId).actorId));
        mPeers.erase(peerId);
    }

    void Server::sendMapToPeer(const Peer& peer)
    {
        Serial::TextWriteStream stream;
        FASaveGame::GameSaver saver(stream);

        saver.save(uint8_t(MessageType::MapToClient));
        saver.save(mDoFullVerify);
        saver.save(peer.actorId);
        mWorld.save(saver);

        auto data = stream.getData();

        // does not take ownership of data
        ENetPacket* packet = enet_packet_create(data.first, data.second, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer.peer, RELIABLE_CHANNEL_ID, packet);
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
                mPeers.at(uint32_t(size_t(event.peer->data))).hasMap = true;
                return;
            }

            case MessageType::ClientUpdateToServer:
            {
                receiveClientUpdate(loader, mPeers.at(uint32_t(size_t(event.peer->data))));
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
