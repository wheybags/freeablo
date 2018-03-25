#include "server.h"
#include "../../fasavegame/gameloader.h"
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

    std::vector<FAWorld::PlayerInput> Server::getAndClearInputs() { return mLocalInputHandler.getAndClearInputs(); }

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
    }

    void Server::onPeerConnect(const ENetEvent& event)
    {
        mPeers[event.peer->connectID] = Peer(event.peer);
        EngineMain::get()->mPaused = true;

        Serial::TextWriteStream stream;
        FASaveGame::GameSaver saver(stream);

        saver.save(uint8_t(MessageType::MapToClient));
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

            case MessageType::MapToClient:
                invalid_enum(MessageType, type);
        }

        invalid_enum(MessageType, type);
    }
}
