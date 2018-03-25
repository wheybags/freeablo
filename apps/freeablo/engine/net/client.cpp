#include "client.h"
#include "../../fagui/guimanager.h"
#include "../../fasavegame/gameloader.h"
#include "../../faworld/world.h"
#include "../enginemain.h"
#include <misc/assert.h>
#include <serial/textstream.h>

namespace Engine
{
    Client::Client()
    {
        mAddress.port = 6666;
        enet_address_set_host(&mAddress, "127.0.0.1");
        mHost = enet_host_create(NULL, 32, 2, 0, 0);
        mServerPeer = enet_host_connect(mHost, &mAddress, 2, 0);

        EngineMain::get()->mPaused = true;
    }

    Client::~Client()
    {
        if (mServerPeer != nullptr)
            enet_peer_disconnect(mServerPeer, 0);

        enet_host_destroy(mHost);
    }

    std::vector<FAWorld::PlayerInput> Client::getAndClearInputs() { return std::vector<FAWorld::PlayerInput>(); }

    void Client::update()
    {
        ENetEvent event = {};

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
    }

    void Client::processServerPacket(const ENetEvent& event)
    {
        Serial::TextReadStream stream(std::string((const char*)event.packet->data, event.packet->dataLength));
        FASaveGame::GameLoader loader(stream);

        MessageType type = MessageType(loader.load<uint8_t>());

        switch (type)
        {
            case MessageType::MapToClient:
            {
                this->receiveMap(loader);
                return;
            }

            case MessageType::AcknowledgeMapToServer:
                invalid_enum(MessageType, type);
        }

        invalid_enum(MessageType, type);
    }

    void Client::receiveMap(FASaveGame::GameLoader& loader)
    {
        // HACKY BULLSHIT HERE
        EngineMain::get()->mWorld.reset(new FAWorld::World(loader, *EngineMain::get()->mExe.get()));
        //        EngineMain::get()->mGuiManager.reset(new FAGui::GuiManager(*EngineMain::get(), *EngineMain::get()->mWorld.get()));
        //        EngineMain::get()->mWorld->setGuiManager(EngineMain::get()->mGuiManager.get());

        Serial::TextWriteStream stream;
        FASaveGame::GameSaver saver(stream);
        saver.save(uint8_t(MessageType::AcknowledgeMapToServer));
        auto data = stream.getData();

        EngineMain::get()->mPaused = false;
        EngineMain::get()->mInGame = true;

        // does not take ownership of data
        ENetPacket* packet = enet_packet_create(data.first, data.second, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(mServerPeer, RELIABLE_CHANNEL_ID, packet);
    }
}
