#include "server.h"
#include "../../fasavegame/gameloader.h"
#include "../../faworld/player.h"
#include "../../faworld/world.h"
#include "../enginemain.h"
#include "../localinputhandler.h"
#include <cstring>
#include <iostream>
#include <misc/assert.h>
#include <misc/misc.h>
#include <serial/textstream.h>

namespace Engine
{
    const char* Server::SERVER_ADDRESS = "0.0.0.0";

    Server::Server(FAWorld::World& world, LocalInputHandler& localInputHandler) : mWorld(world), mLocalInputHandler(localInputHandler)
    {
        if (0 != enet_initialize())
        {
            std::cerr << "Unable to initialize networking library." << std::endl;
        }
        mAddress.port = 6666;
        enet_address_set_host(&mAddress, SERVER_ADDRESS);
        mHost = enet_host_create(&mAddress, 32, CHANNEL_ID_END, 0, 0);
        mHost->checksum = enet_crc32;
    }

    Server::~Server()
    {
        enet_host_destroy(mHost);
        enet_deinitialize();
    }

    std::optional<std::vector<FAWorld::PlayerInput>> Server::getAndClearInputs(FAWorld::Tick tick)
    {
        if (!mInputs.count(tick))
            return std::nullopt;

        /* for (const auto& pair : mPeers)
         {
             if (pair.second.hasMap && pair.second.lastTick + 10 < tick)
                 return std::nullopt;
         }*/

        std::vector<FAWorld::PlayerInput> retval;
        mInputs.at(tick).swap(retval);
        mInputs.erase(tick);

        return retval;
    }

    void Server::update()
    {
        this->handleEvents();
        this->handleMapSending();
        this->processInputs();
    }

    void Server::handleEvents()
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
    }

    // Decide on which inputs to execute this tick, and handle sending them to players + the server game instance
    void Server::processInputs()
    {
        auto localInputs = mLocalInputHandler.getAndClearInputs();
        mInputsBuffer.insert(mInputsBuffer.begin(), localInputs.begin(), localInputs.end());

        FAWorld::PlayerInput::removeUnnecessaryInputs(mInputsBuffer);

        // Don't allow more inputs in this tick than we can process in one packet.
        size_t numberOfInputsToProcessThisTick =
            std::min(mInputsBuffer.size(), (MAX_UPDATE_PACKET_SIZE - UPDATE_PACKET_START_PADDING) / FAWorld::PlayerInput::MAX_SERIALISED_INPUT_SIZE);

        // Split off the first n inputs into a separate vector
        std::vector<FAWorld::PlayerInput> thisTickInputs;
        {
            thisTickInputs.reserve(numberOfInputsToProcessThisTick);
            for (size_t i = 0; i < numberOfInputsToProcessThisTick; i++)
                thisTickInputs.push_back(mInputsBuffer[i]);

            mInputsBuffer.erase(mInputsBuffer.begin(), mInputsBuffer.begin() + numberOfInputsToProcessThisTick);
        }

        // We can't have the server pulling directly from mInputsBuffer because then it would
        // execute inputs at an earlier tick than the clients. So what we do here is essentially
        // sending the buffer to all the clients, then "sending" it to the server as well, by means
        // of the mInputs map.
        sendInputsToClients(thisTickInputs);
        mInputs[mWorld.getCurrentTick()] = std::move(thisTickInputs);

        mLastSentTick = mWorld.getCurrentTick();
    }

    void Server::onPeerConnect(const ENetEvent& event)
    {
        enet_peer_timeout(event.peer, 99999, 99999, 99999);

        mPeers[mNextPeerId] = Peer(event.peer);
        event.peer->data = reinterpret_cast<void*>(size_t(mNextPeerId));

        // We pass the player joining as a PlayerInput so that other clients will know about them connecting.
        // Later on, the game will create an FAWorld::Player object for the player, and inform us of this through registerNewPlayer().
        // Once that is done, we have an actor for the player, so we can send them the map, which we do by calling handleMapSending()
        // regularly from update(), which checks for peers that haven't been sent a map yet, but do have an actor (actorId != -1).
        EngineMain::get()->getLocalInputHandler()->addInput(FAWorld::PlayerInput(FAWorld::PlayerInput::PlayerJoinedData{mNextPeerId}, -1));
        mNextPeerId++;
    }

    bool Server::isPlayerRegistered(uint32_t peerId) const { return mPeers.at(peerId).actorId != -1; }

    void Server::registerNewPlayer(FAWorld::Player* player, uint32_t peerId)
    {
        // see onPeerConnect for an explanation of this
        Peer& peer = mPeers.at(peerId);
        peer.actorId = player->getId();
    }

    void Server::handleMapSending()
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
    }

    void Server::onPeerDisconnect(const ENetEvent& event)
    {
        uint32_t peerId = uint32_t(size_t(event.peer->data));
        EngineMain::get()->getLocalInputHandler()->addInput(FAWorld::PlayerInput(FAWorld::PlayerInput::PlayerLeftData{}, mPeers.at(peerId).actorId));
        mPeers.erase(peerId);
    }

    void Server::sendMapToPeer(Peer& peer)
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

        peer.lastTick = mWorld.getCurrentTick() - 1;
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
            case MessageType::VerifyToClient:
                invalid_enum(MessageType, type);
        }

        invalid_enum(MessageType, type);
    }

    void Server::sendInputsToClients(std::vector<FAWorld::PlayerInput>& inputs)
    {
        // The basic approach of this function is to send a fized-size packet each tick, and just fill it with as many ticks worth of
        // inputs as we can fit. We ensure that these are chosen well, and that every tick is guaranteed to be received eventually by every client.

        mOldInputs[mWorld.getCurrentTick()] = inputs;

        FAWorld::Tick oldestNeededTick = std::numeric_limits<FAWorld::Tick>::max();

        for (auto& pair : mPeers)
        {
            Peer& peer = pair.second;

            if (!peer.hasMap)
                continue;

            bool firstInPacket = true;

            auto fillPacket = [this, &firstInPacket](FAWorld::Tick& currentlyProcessingTick) -> ENetPacket* {
                Serial::TextWriteStream stream;
                FASaveGame::GameSaver saver(stream);

                saver.save(uint8_t(MessageType::InputsToClient));

                size_t lastTickEndPosition = 0;

                auto addTick = [&](FAWorld::Tick tick) {
                    saver.save(true); // Is there another tick in this packet?
                    saver.save(tick);
                    saver.save(uint32_t(mOldInputs[tick].size()));

                    if (firstInPacket)
                    {
                        release_assert(stream.getCurrentSize() < UPDATE_PACKET_START_PADDING);
                        firstInPacket = false;
                    }

                    for (const auto& input : mOldInputs[tick])
                        input.save(saver);

                    if (stream.getCurrentSize() > MAX_UPDATE_PACKET_SIZE)
                    {
                        // This assert detects single ticks with too many inputs to fit in one packet.
                        // This should not happen, as we should never allow more than
                        // ((MAX_UPDATE_PACKET_SIZE - UPDATE_PACKET_START_PADDING) / PlayerInput::MAX_SERIALISED_INPUT_SIZE) inputs in one tick.
                        // TODO: there is a few bits added to the packet before we start the inputs, we should account for them somehow.
                        release_assert(lastTickEndPosition > 0);
                        return false;
                    }

                    lastTickEndPosition = stream.getCurrentSize();
                    return true;
                };

                // On even ticks, we cycle through old inputs, and on odd ticks we just send as many of the most recent ticks
                // as we can fit. This provides a decent balance between definitely sending everything again if it's needed
                // (because of packet loss/corruption), and always sending the most recent stuff.
                if (mWorld.getCurrentTick() % 2 == 0)
                {
                    addTick(mWorld.getCurrentTick());

                    while (currentlyProcessingTick < mWorld.getCurrentTick())
                    {
                        if (!addTick(currentlyProcessingTick))
                            break;

                        currentlyProcessingTick++;
                    }
                }
                else
                {
                    FAWorld::Tick tick = mWorld.getCurrentTick();
                    while (mOldInputs.count(tick) != 0)
                    {
                        if (!addTick(tick))
                            break;

                        tick--;
                    }
                }

                if (lastTickEndPosition == 0)
                    return nullptr;

                stream.resize(lastTickEndPosition);
                saver.save(false); // There are no more ticks in this packet

                auto data = stream.getData();
                ENetPacket* packet = enet_packet_create(nullptr, data.second, ENET_PACKET_FLAG_UNSEQUENCED);
                memcpy(packet->data, data.first, data.second);

                return packet;
            };

            FAWorld::Tick currentlyProcessingTick = peer.lastSentTick;

            if (currentlyProcessingTick < peer.lastTick || currentlyProcessingTick >= mWorld.getCurrentTick() - 5)
                currentlyProcessingTick = peer.lastTick;

            oldestNeededTick = std::min(oldestNeededTick, peer.lastTick);

            peer.bytesSentLastTick = 0;

            if (ENetPacket* packet = fillPacket(currentlyProcessingTick))
            {
                peer.bytesSentLastTick += packet->dataLength;
                enet_peer_send(peer.peer, SERVER_TO_CLIENT_CHANNEL_ID, packet);

                peer.lastSentTick = currentlyProcessingTick;
            }
        }

        // Remove no-longer needed old input lists
        // Done as a manual loop, because apparently std::remove_if doesn't work on associative containers (yet)
        for (auto it = mOldInputs.begin(); it != mOldInputs.end();)
        {
            if (it->first < oldestNeededTick)
                it = mOldInputs.erase(it);
            else
                ++it;
        }

        if (mDoFullVerify && !mPeers.empty() && mLastTickVerified < mWorld.getCurrentTick())
        {
            Serial::TextWriteStream stream;
            FASaveGame::GameSaver saver(stream);

            saver.save(uint8_t(MessageType::VerifyToClient));
            saver.save(mWorld.getCurrentTick());

            // save world as a string
            {
                Serial::TextWriteStream worldStream;
                FASaveGame::GameSaver worldSaver(worldStream);
                mWorld.save(worldSaver);
                auto worldData = worldStream.getData();

                std::string strData((const char*)worldData.first, worldData.second);
                saver.save(strData);
            }

            auto data = stream.getData();
            ENetPacket* packet = enet_packet_create(data.first, data.second, ENET_PACKET_FLAG_RELIABLE);
            enet_host_broadcast(mHost, RELIABLE_CHANNEL_ID, packet);

            mLastTickVerified = mWorld.getCurrentTick();
        }
    }

    void Server::receiveClientUpdate(FASaveGame::GameLoader& loader, Peer& peer)
    {
        peer.lastTick = loader.load<FAWorld::Tick>();

        std::map<uint32_t, std::vector<FAWorld::PlayerInput>> inputSetsInPacket;
        while (loader.load<bool>())
        {
            auto& inputSet = inputSetsInPacket[loader.load<uint32_t>()];

            uint32_t size = loader.load<uint32_t>();
            inputSet.resize(size);
            for (size_t i = 0; i < size; i++)
                inputSet[i].load(loader);
        }

        for (auto it = inputSetsInPacket.begin(); it != inputSetsInPacket.end(); ++it)
        {
            if (it->first > peer.lastInputSetIdReceived)
            {
                mInputsBuffer.insert(mInputsBuffer.end(), it->second.begin(), it->second.end());
                peer.lastInputSetIdReceived = it->first;
            }
        }
    }

    void Server::doMultiplayerGui(nk_context* ctx)
    {
        if (nk_begin(ctx, "Players", nk_rect(0, 0, 600, 200), NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE))
        {
            nk_layout_row_dynamic(ctx, 30, 4);

            nk_label(ctx, "Player ID", NK_TEXT_CENTERED);
            nk_label(ctx, "Ticks behind", NK_TEXT_CENTERED);
            nk_label(ctx, "Bytes/Tick", NK_TEXT_CENTERED);
            nk_label(ctx, "Bytes/Second", NK_TEXT_CENTERED);

            for (const auto& pair : mPeers)
            {
                const Peer& peer = pair.second;

                nk_label(ctx, std::to_string(peer.actorId).c_str(), NK_TEXT_CENTERED);

                double ticksBehind = mStatsAverager.getAverage(std::to_string(peer.actorId) + "_ticks_behind", mWorld.getCurrentTick() - peer.lastTick);
                nk_label(ctx, std::to_string(ticksBehind).c_str(), NK_TEXT_RIGHT);

                double bytesPerTick = mStatsAverager.getAverage(std::to_string(peer.actorId) + "_bytes_sent_per_tick", peer.bytesSentLastTick);
                nk_label(ctx, std::to_string(bytesPerTick).c_str(), NK_TEXT_RIGHT);

                double bytesPerSec = mStatsAverager.getAverage(std::to_string(peer.actorId) + "_bytes_sent_per_second",
                                                               peer.bytesSentLastTick * FAWorld::World::getTicksInPeriod(FixedPoint(1)));
                nk_label(ctx, Misc::numberToHumanFileSize(bytesPerSec).c_str(), NK_TEXT_RIGHT);
            }
        }
        nk_end(ctx);
    }
}
