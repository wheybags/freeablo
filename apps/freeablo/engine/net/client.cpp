#include "client.h"
#include "../../fagui/guimanager.h"
#include "../../fasavegame/gameloader.h"
#include "../../faworld/gamelevel.h"
#include "../../faworld/player.h"
#include "../../faworld/world.h"
#include "../enginemain.h"
#include "../localinputhandler.h"
#include <iostream>
#include <misc/assert.h>
#include <serial/textstream.h>

namespace Engine
{
    Client::Client(LocalInputHandler& localInputHandler) : mLocalInputHandler(localInputHandler)
    {
        if (0 != enet_initialize())
        {
            std::cerr << "Unable to initialize networking library." << std::endl;
        }
        mAddress.port = 6666;
        enet_address_set_host(&mAddress, "127.0.0.1");
        mHost = enet_host_create(nullptr, 32, 2, 0, 0);
        mHost->checksum = enet_crc32;
        mServerPeer = enet_host_connect(mHost, &mAddress, 2, 0);
    }

    Client::~Client()
    {
        if (mServerPeer != nullptr)
            enet_peer_disconnect(mServerPeer, 0);

        enet_host_flush(mHost);
        enet_host_destroy(mHost);
        enet_deinitialize();
    }

    boost::optional<std::vector<FAWorld::PlayerInput>> Client::getAndClearInputs(FAWorld::Tick tick)
    {
        if (!mInputs.count(tick))
            return boost::none;

        std::vector<FAWorld::PlayerInput> retval;
        retval.swap(mInputs[tick]);
        mInputs.erase(tick);
        return retval;
    }

    void Client::update()
    {
        ENetEvent event;

        while (enet_host_service(mHost, &event, 0))
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    this->processServerPacket(event);
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
                case ENET_EVENT_TYPE_CONNECT:
                {
                    break;
                }
                default:
                    invalid_enum(ENetEventType, event.type);
            }
        }

        sendClientUpdate();
    }

    void Client::verify(FAWorld::Tick tick)
    {
        if (!mDoFullVerify)
            return;

        Serial::TextWriteStream worldStream;
        FASaveGame::GameSaver saver(worldStream);
        EngineMain::get()->mWorld->save(saver);

        auto worldData = worldStream.getData();
        std::string strData(reinterpret_cast<const char*>(worldData.first), worldData.second);

        if (strData != mServerStatesForFullVerify[tick])
        {
            FILE* f = fopen("SERVER.txt", "wb");
            fwrite(mServerStatesForFullVerify[tick].data(), 1, mServerStatesForFullVerify[tick].size(), f);
            fclose(f);

            f = fopen("CLIENT.txt", "wb");
            fwrite(strData.data(), 1, strData.size(), f);
            fclose(f);

            message_and_abort("desync detected");
        }

        mServerStatesForFullVerify.erase(tick);
    }

    void Client::processServerPacket(const ENetEvent& event)
    {
        Serial::TextReadStream stream(std::string(reinterpret_cast<const char*>(event.packet->data), event.packet->dataLength));
        FASaveGame::GameLoader loader(stream);

        MessageType type = MessageType(loader.load<uint8_t>());

        switch (type)
        {
            case MessageType::MapToClient:
            {
                this->receiveMap(loader);
                return;
            }

            case MessageType::InputsToClient:
            {
                receiveInputs(loader);
                return;
            }

            case MessageType::ClientUpdateToServer:
            case MessageType::AcknowledgeMapToServer:
                invalid_enum(MessageType, type);
        }

        invalid_enum(MessageType, type);
    }

    void Client::receiveMap(FASaveGame::GameLoader& loader)
    {
        mDoFullVerify = loader.load<bool>();
        int32_t myPlayerId = loader.load<int32_t>();
        EngineMain::get()->mWorld->load(loader);
        EngineMain::get()->mWorld->addCurrentPlayer(static_cast<FAWorld::Player*>(EngineMain::get()->mWorld->getActorById(myPlayerId)));

        auto myPlayer = EngineMain::get()->mWorld->getCurrentPlayer();
        release_assert(myPlayer->getLevel()->isPassable(myPlayer->mMoveHandler.getCurrentPosition().current(), myPlayer));

        Serial::TextWriteStream stream;
        FASaveGame::GameSaver saver(stream);
        saver.save(uint8_t(MessageType::AcknowledgeMapToServer));
        auto data = stream.getData();

        EngineMain::get()->mInGame = true;

        // does not take ownership of data
        ENetPacket* packet = enet_packet_create(data.first, data.second, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(mServerPeer, RELIABLE_CHANNEL_ID, packet);
    }

    void Client::receiveInputs(FASaveGame::GameLoader& loader)
    {
        FAWorld::Tick tick = loader.load<FAWorld::Tick>();

        auto& inputs = mInputs[tick];
        uint32_t size = loader.load<uint32_t>();
        inputs.resize(size);
        for (uint32_t i = 0; i < size; i++)
            inputs[i].load(loader);

        if (mDoFullVerify)
            mServerStatesForFullVerify[tick] = loader.load<std::string>();
    }

    void Client::sendClientUpdate()
    {
        auto inputs = mLocalInputHandler.getAndClearInputs();

        Serial::TextWriteStream stream;
        FASaveGame::GameSaver saver(stream);

        saver.save(uint8_t(MessageType::ClientUpdateToServer));
        saver.save(EngineMain::get()->mWorld->getCurrentTick());

        saver.save(uint32_t(inputs.size()));
        for (auto& input : inputs)
            input.save(saver);

        auto data = stream.getData();

        // does not take ownership of data
        ENetPacket* packet = enet_packet_create(data.first, data.second, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(mServerPeer, RELIABLE_CHANNEL_ID, packet);
    }
}
