#pragma once
#include "../../faworld/world.h"
#include <cstdint>
#include <fa_nuklear.h>
#include <optional>
#include <vector>

namespace FAWorld
{
    class Player;
    class PlayerInput;
}

namespace Engine
{
    class MultiplayerInterface
    {
    public:
        virtual ~MultiplayerInterface() = default;

        virtual std::optional<std::vector<FAWorld::PlayerInput>> getAndClearInputs(FAWorld::Tick tick) = 0;
        virtual void update() = 0;
        virtual void verify(FAWorld::Tick tick) = 0;
        virtual bool isServer() const = 0;
        virtual bool isMultiplayer() const = 0;
        virtual bool isPlayerRegistered(uint32_t peerId) const = 0;
        virtual void registerNewPlayer(FAWorld::Player* player, uint32_t peerId) = 0;
        virtual void doMultiplayerGui(nk_context*){};

        enum
        {
            RELIABLE_CHANNEL_ID = 10,
            SERVER_TO_CLIENT_CHANNEL_ID,
            CLIENT_TO_SERVER_CHANNEL_ID,

            CHANNEL_ID_END
        };

        enum class MessageType : uint8_t
        {
            // server-to-client
            MapToClient,
            InputsToClient,
            VerifyToClient,

            // client-to-server
            AcknowledgeMapToServer,
            ClientUpdateToServer
        };
    };
}
